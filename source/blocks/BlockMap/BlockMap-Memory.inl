///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../BlockMap.hpp"


namespace Langulus::Annies
{

   /// Reserves space for the specified number of pairs                       
   ///   @attention does nothing if reserving less than current reserve       
   ///   @param count - number of pairs to allocate                           
   template<CT::Map THIS> LANGULUS(INLINED)
   void BlockMap::Reserve(const Count count) {
      if (GetReserved()) {
         while (count > GetReserved())
            AllocateMore<THIS>();
      }
      else if (count) {
         AllocateFresh<THIS>(
            Roof2(count < MinimalAllocation ? MinimalAllocation : count)
         );

         // Zero the info array and set the sentinel at the end         
         ZeroMemory(mInfo, GetReserved());
         mInfo[GetReserved()] = 1;
      }
   }
   
   /// Allocate a fresh set of keys and values (for internal use only)        
   ///   @attention doesn't initialize anything, but the memory state         
   ///   @attention doesn't modify count, doesn't set info sentinel           
   ///   @attention assumes count is a power-of-two                           
   ///   @param count - the new number of pairs                               
   template<CT::Map THIS> LANGULUS(INLINED)
   void BlockMap::AllocateFresh(const Count count) {
      LglsAssumeDev(IsPowerOfTwo(count),
         "Table reallocation count is not a power-of-two");

      Offset infoOffset;
      const auto keyAndInfoSize = RequestKeyAndInfoSize<THIS>(count, infoOffset);
      mKeys.mEntry = Allocator::Allocate(mKeys.mType, keyAndInfoSize);
      LANGULUS_ASSERT(mKeys.mEntry, Allocate, "Out of memory");

      const auto valueByteSize = RequestValuesSize(count);
      mValues.mEntry = Allocator::Allocate(mValues.mType, valueByteSize);

      if (not mValues.mEntry) {
         Allocator::Deallocate(const_cast<Allocation*>(mKeys.mEntry));
         mKeys.mEntry = nullptr;
         LANGULUS_THROW(Allocate, "Out of memory");
      }

      mValues.mRaw = const_cast<Byte*>(mValues.mEntry->GetBlockStart());
      mKeys.mReserved = count;

      // Precalculate the info pointer, it's costly                     
      mKeys.mRaw = const_cast<Byte*>(mKeys.mEntry->GetBlockStart());
      mInfo = reinterpret_cast<InfoType*>(mKeys.mRaw + infoOffset);
   }

   /// Allocate or reallocate key, value, and info array                      
   ///   @attention assumes count is a power-of-two                           
   ///   @attention assumes key and value types have been set prior           
   ///   @tparam REUSE - true to reallocate, false to allocate fresh          
   ///   @param count - the new number of pairs                               
   ///   @return true if another resize is required after this one            
   template<CT::Map THIS, bool REUSE>
   bool BlockMap::AllocateData(const Count count) {
      LglsAssumeDev(IsPowerOfTwo(count),
         "Table reallocation count is not a power-of-two");
      LglsAssumeDev(mKeys.mType and mValues.mType,
         "Key and value types haven't been set");

      if constexpr (REUSE) {
         LglsAssumeDev(
            mKeys.GetUses() == 1 and mValues.GetUses() == 1,
            "Can't reuse memory of a map used from multiple places, "
            "BranchOut should've been called prior to AllocateData"
         );
      }

      Offset infoOffset;
      BlockMap old = *this;

      // Allocate new keys                                              
      const auto keyAndInfoSize = RequestKeyAndInfoSize<THIS>(count, infoOffset);
      if constexpr (REUSE)
         mKeys.mEntry = Allocator::Reallocate(
            keyAndInfoSize, const_cast<Allocation*>(mKeys.mEntry));
      else
         mKeys.mEntry = Allocator::Allocate(mKeys.mType, keyAndInfoSize);

      LANGULUS_ASSERT(mKeys.mEntry, Allocate,
         "Out of memory on allocating/reallocating keys");

      // Allocate new values                                            
      const auto valueByteSize = RequestValuesSize(count);
      if constexpr (REUSE)
         mValues.mEntry = Allocator::Reallocate(
            valueByteSize, const_cast<Allocation*>(mValues.mEntry));
      else
         mValues.mEntry = Allocator::Allocate(mValues.mType, valueByteSize);

      if (not mValues.mEntry) {
         Allocator::Deallocate(const_cast<Allocation*>(mKeys.mEntry));
         mKeys.mEntry = nullptr;
         LANGULUS_THROW(Allocate,
            "Out of memory on allocating/reallocating values");
      }

      mValues.mRaw = const_cast<Byte*>(mValues.mEntry->GetBlockStart());
      mKeys.mReserved = count;

      // Precalculate the info pointer, it's costly                     
      mKeys.mRaw = const_cast<Byte*>(mKeys.mEntry->GetBlockStart());
      mInfo = reinterpret_cast<InfoType*>(mKeys.mRaw + infoOffset);
      // Set the sentinel                                               
      mInfo[count] = 1;

      // Zero or move the info array                                    
      if constexpr (REUSE) {
         // Check if any data was reused                                
         if (mKeys.mEntry == old.mKeys.mEntry
         or mValues.mEntry == old.mValues.mEntry) {
            // No escape from this scope                                
            std::nullptr_t force_reuse;
            bool continue_resizing;
            if (mKeys.mEntry == old.mKeys.mEntry) {
               if (mValues.mEntry == old.mValues.mEntry)
                  continue_resizing = Rehash<THIS>(old.mInfo, old.GetReserved(), force_reuse, force_reuse);
               else
                  continue_resizing = Rehash<THIS>(old.mInfo, old.GetReserved(), force_reuse, old);
            }
            else continue_resizing = Rehash<THIS>(old.mInfo, old.GetReserved(), old, force_reuse);

            return continue_resizing;
         }
      }

      // If reached, then both keys and values are newly allocated      
      ZeroMemory(mInfo, count);
      if (old.IsEmpty()) {
         // There are no old values, the previous map was empty         
         // Just do an early return right here                          
         return false;
      }

      // If reached, then keys or values (or both) moved                
      // Reinsert all pairs to rehash                                   
      mKeys.mCount = 0;

      auto key = old.GetKeyHandle<THIS>(0);
      auto val = old.GetValHandle<THIS>(0);
      const auto infoend = old.GetInfoEnd();

      // This should gracefully handle oversaturation by nesting        
      // the AllocateData calls. Shouldn't affect anything but the      
      // hashmask, because the iterators are in the old block...        
      while (old.mInfo != infoend) {
         if (*old.mInfo) {
            if constexpr (CT::TypedMap<THIS>) {
               InsertInner<THIS, false>(
                  GetBucket<THIS>(GetReserved() - 1, key.Get()),
                  Abandon(key), Abandon(val)
               );
               key.FreeInner();
               val.FreeInner();
            }
            else {
               InsertBlockInner<THIS, false>(
                  GetBucketUnknown(GetReserved() - 1, key),
                  Abandon(key), Abandon(val)
               );

               if (key) key.FreeInner();
               else key.mCount = 1;

               if (val) val.FreeInner();
               else val.mCount = 1;
            }
         }

         ++key;
         ++val;
         ++old.mInfo;
      }

      // Free the old allocations                                       
      if constexpr (REUSE) {
         // When reusing, keys and values can potentially remain same   
         // Avoid deallocating them if that's the case                  
         if (old.mValues.mEntry != mValues.mEntry) {
            LglsAssumeDev(old.mValues.mEntry->GetUses() == 1,
               "Shouln't happen");
            Allocator::Deallocate(const_cast<Allocation*>(old.mValues.mEntry));
         }

         if (old.mKeys.mEntry != mKeys.mEntry) {
            LglsAssumeDev(old.mKeys.mEntry->GetUses() == 1,
               "Shouln't happen");
            Allocator::Deallocate(const_cast<Allocation*>(old.mKeys.mEntry));
         }
      }
      else {
         // Not reusing, so either deallocate, or dereference           
         if (old.mKeys.mEntry) {
            if (old.mKeys.mEntry->GetUses() > 1)
               const_cast<Allocation*>(old.mKeys.mEntry)->Free();
            else
               Allocator::Deallocate(const_cast<Allocation*>(old.mKeys.mEntry));
         }

         if (old.mValues.mEntry) {
            if (old.mValues.mEntry->GetUses() > 1)
               const_cast<Allocation*>(old.mValues.mEntry)->Free();
            else
               Allocator::Deallocate(const_cast<Allocation*>(old.mValues.mEntry));
         }
      }

      return false;
   }

   /// Doubles the reserved memory                                            
   template<CT::Map THIS> LANGULUS(INLINED)
   void BlockMap::AllocateMore() {
      LglsAssumeDev(GetReserved(),
         "Can't AllocateMore, needs to AllocateFresh first");

      if (IsAllocated() and mKeys.GetUses() == 1 and mValues.GetUses() == 1)
         while (AllocateData<THIS, true>(GetReserved() * 2));
      else
         while (AllocateData<THIS, false>(GetReserved() * 2));
   }
   
   /// Reference memory block once                                            
   template<CT::Map THIS, bool DEEP> LANGULUS(INLINED)
   void BlockMap::Keep() const noexcept {
      if (mKeys.mEntry) {
         const_cast<Allocation*>(mKeys.mEntry)->Keep(1);

         if constexpr (DEEP)
            GetKeys<THIS>().KeepInner(mInfo);
      }

      if (mValues.mEntry) {
         const_cast<Allocation*>(mValues.mEntry)->Keep(1);

         if constexpr (DEEP)
            GetVals<THIS>().KeepInner(mInfo);
      }
   }

   /// Dereference memory block once and destroy all elements if data was     
   /// fully dereferenced                                                     
   ///   @attention this doesn't modify any immediate map state               
   template<CT::Map THIS> LANGULUS(INLINED)
   void BlockMap::Free() {
      // Always destroy values first, because keys also contain mInfo   
      if (mValues.mEntry) {
         LglsAssumeDev(mValues.mEntry->GetUses() >= 1,
            "Bad value memory dereferencing");

         if (mValues.mEntry->GetUses() == 1) {
            if (not IsEmpty())
               GetVals<THIS>().FreeInner(mInfo);

            // Deallocate values                                        
            Allocator::Deallocate(const_cast<Allocation*>(mValues.mEntry));
         }
         else {
            // Dereference values                                       
            if (not IsEmpty())
               GetVals<THIS>().template FreeInner<false>(mInfo);

            const_cast<Allocation*>(mValues.mEntry)->Free();
         }

         mValues.mEntry = nullptr;
      }

      if (mKeys.mEntry) {
         LglsAssumeDev(mKeys.mEntry->GetUses() >= 1,
            "Bad key memory dereferencing");

         if (mKeys.mEntry->GetUses() == 1) {
            if (not IsEmpty())
               GetKeys<THIS>().FreeInner(mInfo);

            // Deallocate keys                                          
            Allocator::Deallocate(const_cast<Allocation*>(mKeys.mEntry));
         }
         else {
            // Dereference keys                                         
            if (not IsEmpty())
               GetKeys<THIS>().template FreeInner<false>(mInfo);

            const_cast<Allocation*>(mKeys.mEntry)->Free();
         }

         mKeys.mEntry = nullptr;
      }
   }

} // namespace Langulus::Annies
