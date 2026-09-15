///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../BlockSet.hpp"


namespace Langulus::Annies
{

   /// Reserves space for the specified number of elements                    
   ///   @attention does nothing if reserving less than current reserve       
   ///   @param count - number of elements to allocate                        
   template<CT::Set THIS> LANGULUS(INLINED)
   void BlockSet::Reserve(const Count count) {
      AllocateInner<THIS>(
         Roof2(count < MinimalAllocation ? MinimalAllocation : count)
      );
   }

   /// Allocate a fresh set of keys (for internal use only)                   
   ///   @attention doesn't initialize anything, but the memory state         
   ///   @attention doesn't modify count, doesn't set info sentinel           
   ///   @attention assumes count is a power-of-two                           
   ///   @param count - the new number of elements                            
   template<CT::Set THIS> LANGULUS(INLINED)
   void BlockSet::AllocateFresh(const Count count) {
      LANGULUS_ASSUME(DevAssumes, IsPowerOfTwo(count),
         "Table reallocation count is not a power-of-two");

      Offset infoOffset;
      const auto keyAndInfoSize = RequestKeyAndInfoSize<THIS>(count, infoOffset);
      mKeys.mEntry = Allocator::Allocate(mKeys.mType, keyAndInfoSize);
      LANGULUS_ASSERT(mKeys.mEntry, Allocate, "Out of memory");

      mKeys.mReserved = count;

      // Precalculate the info pointer, it's costly                     
      mKeys.mRaw = const_cast<Byte*>(mKeys.mEntry->GetBlockStart());
      mInfo = reinterpret_cast<InfoType*>(mKeys.mRaw + infoOffset);
   }

   /// Allocate or reallocate key and info array                              
   ///   @attention assumes count is a power-of-two                           
   ///   @attention assumes key type have been set prior                      
   ///   @tparam REUSE - true to reallocate, false to allocate fresh          
   ///   @param count - the new number of pairs                               
   template<CT::Set THIS, bool REUSE>
   void BlockSet::AllocateData(const Count count) {
      LANGULUS_ASSUME(DevAssumes, IsPowerOfTwo(count),
         "Table reallocation count is not a power-of-two");
      LANGULUS_ASSUME(DevAssumes, mKeys.mType,
         "Key type haven't been set");

      auto& me = reinterpret_cast<const THIS&>(*this);
      Offset infoOffset;

      // Allocate new keys                                              
      BlockSet old = *this;
      const auto keyAndInfoSize = RequestKeyAndInfoSize<THIS>(count, infoOffset);
      if constexpr (REUSE)
         mKeys.mEntry = Allocator::Reallocate(
            keyAndInfoSize, const_cast<Allocation*>(mKeys.mEntry));
      else
         mKeys.mEntry = Allocator::Allocate(mKeys.mType, keyAndInfoSize);

      LANGULUS_ASSERT(mKeys.mEntry, Allocate,
         "Out of memory on allocating/reallocating keys");

      mKeys.mReserved = count;

      // Precalculate the info pointer, it's costly                     
      mKeys.mRaw = const_cast<Byte*>(mKeys.mEntry->GetBlockStart());
      mInfo = reinterpret_cast<InfoType*>(mKeys.mRaw + infoOffset);
      // Set the sentinel                                               
      mInfo[count] = 1;

      // Zero or move the info array                                    
      if constexpr (REUSE) {
         // Check if keys were reused                                   
         if (mKeys.mEntry == old.mKeys.mEntry) {
            // Data was reused, but info always moves (null the rest)   
            MoveMemory(mInfo, old.mInfo, old.GetReserved());
            ZeroMemory(mInfo + old.GetReserved(), count - old.GetReserved());

            // Data was reused, but entries always move if sparse keys  
            if (me.IsSparse()) {
               MoveMemory(
                  mKeys.mRawSparse + count,
                  mKeys.mRawSparse + old.GetReserved(),
                  old.GetReserved()
               );
            };

            Rehash<THIS>(old.GetReserved());
            return;
         }
      }

      // If reached, then keys are newly allocated                      
      ZeroMemory(mInfo, count);
      if (not old.mKeys) {
         // There are no old values, the previous set was empty         
         // Just do an early return right here                          
         return;
      }

      // If reached, then keys moved - reinsert all keys to rehash     
      mKeys.mCount = 0;
      
      auto key = old.GetHandle<THIS>(0);
      const auto hashmask = GetReserved() - 1;
      const auto oldInfoEnd = old.GetInfoEnd();
      while (old.mInfo != oldInfoEnd) {
         if (*old.mInfo) {
            if constexpr (CT::Typed<THIS>) {
               InsertInner<THIS, false>(
                  GetBucket(hashmask, key.Get()),
                  Abandon(key)
               );
               key.FreeInner();
            }
            else {
               InsertBlockInner<THIS, false>(
                  GetBucketUnknown(hashmask, key),
                  Abandon(key)
               );

               if (key)
                  key.FreeInner();
               else
                  key.mCount = 1;
            }
         }

         ++key;
         ++old.mInfo;
      }

      // Free the old allocations                                       
      if (old.mKeys.mEntry and old.mKeys.mEntry != mKeys.mEntry) {
         // Not reusing, so either deallocate, or dereference           
         if (old.mKeys.mEntry->GetUses() > 1)
            const_cast<Allocation*>(old.mKeys.mEntry)->Free();
         else
            Allocator::Deallocate(const_cast<Allocation*>(old.mKeys.mEntry));
      }
   }

   /// Reserves space for the specified number of pairs                       
   ///   @attention does nothing if reserving less than current reserve       
   ///   @attention assumes count is a power-of-two number                    
   ///   @param count - number of pairs to allocate                           
   template<CT::Set THIS> LANGULUS(INLINED)
   void BlockSet::AllocateInner(const Count count) {
      // Shrinking is never allowed, you'll have to do it explicitly    
      // via Compact()                                                  
      if (count <= GetReserved())
         return;

      // Allocate/Reallocate the keys and info                          
      if (IsAllocated() and GetUses() == 1)
         AllocateData<THIS, true>(count);
      else
         AllocateData<THIS, false>(count);
   }
   
   /// Reference memory block once                                            
   template<CT::Set THIS, bool DEEP> LANGULUS(INLINED)
   void BlockSet::Keep() const noexcept {
      if (not mKeys.mEntry)
         return;

      const_cast<Allocation*>(mKeys.mEntry)->Keep(1);

      if constexpr (DEEP)
         GetValues<THIS>().KeepInner(mInfo);
   }

   /// Dereference memory block once and destroy all elements if data was     
   /// fully dereferenced                                                     
   ///   @attention this never modifies any state, except mKeys.mEntry        
   template<CT::Set THIS> LANGULUS(INLINED)
   void BlockSet::Free() {
      if (not mKeys.mEntry)
         return;

      LANGULUS_ASSUME(DevAssumes,
         mKeys.mEntry->GetUses() >= 1, "Bad memory dereferencing");

      if (mKeys.mEntry->GetUses() == 1) {
         if (not IsEmpty())
            GetValues<THIS>().FreeInner(mInfo);

         // Deallocate stuff                                            
         Allocator::Deallocate(const_cast<Allocation*>(mKeys.mEntry));
      }
      else {
         // Dereference memory                                          
         if (not IsEmpty())
            GetValues<THIS>().template FreeInner<false>(mInfo);

         const_cast<Allocation*>(mKeys.mEntry)->Free();
      }

      mKeys.mEntry = nullptr;
   }

} // namespace Langulus::Annies
