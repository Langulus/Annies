///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Block.hpp"
//#include <Langulus/Annies/Memory.hpp>


namespace Langulus::Annies
{
   
   /// Remove the first occurence of a given item                             
   ///   @tparam REVERSE - whether to search from the back                    
   ///   @param item - the item type to search for and remove                 
   ///   @return 1 if the element was found and removed, 0 otherwise          
   template<class TYPE> template<bool REVERSE> LANGULUS(INLINED)
   Count Block<TYPE>::Remove(const CT::NoIntent auto& item) {
      const auto found = Find<REVERSE>(item);
      return found ? RemoveIndex(found.GetOffsetUnsafe(), 1) : 0;
   }
   
   /// Remove sequential indices                                              
   ///   @param index - index to start removing from                          
   ///   @param count - number of contiguous items to remove                  
   ///   @return the number of removed elements                               
   template<class TYPE>
   Count Block<TYPE>::RemoveIndex(CT::Index auto index, const Count count) {
      using INDEX = Deref<decltype(index)>;

      if constexpr (CT::Similar<INDEX, Index>) {
         // By special indices                                          
         if (index == IndexAll) {
            const auto oldCount = mCount;
            Free();
            ResetMemory();
            ResetState();
            return oldCount;
         }

         auto idx = Constrain(index);
         if (idx.IsSpecial())
            return 0;

         return RemoveIndex(idx.GetOffsetUnsafe(), count);
      }
      else {
         // By simple index                                             
         Offset idx = index;
         const auto ender = idx + count;
         const auto removed = ender - idx;
         LglsAssumeDev(ender <= mCount, "Out of range");

         if constexpr (TypeErased) {
            if (IsConstant() or IsStatic()) {
               if (mType->mIsPOD and idx + count >= mCount) {
                  // If data is POD and elements are on the back, we can
                  // get around constantness and staticness, by simply  
                  // truncating the count without any reprecussions     
                  const auto removed = mCount - idx;
                  mCount = idx;
                  return removed;
               }
               else {
                  LANGULUS_ASSERT(not IsConstant(), Access,
                     "Removing from constant container");
                  LANGULUS_ASSERT(not IsStatic(), Access,
                     "Removing from static container");
                  return 0;
               }
            }

            // First call the destructors on the correct region         
            BranchOut();
            CropInner(idx, removed).FreeInner();

            if (ender < mCount) {
               // Fill gap by invoking abandon-constructors             
               // We're moving to the left, so no reverse is required   
               const auto tail = mCount - ender;
               CropInner(idx, tail).CreateWithIntent(
                  Abandon(CropInner(ender, tail)));
            }
         }
         else {
            if (IsStatic() and ender == mCount) {
               // If data is static and elements are on the back, we    
               // can get around constantness and staticness, by        
               // truncating the count without any reprecussions        
               // We can't destroy static element anyways               
               mCount = idx;
               return count;
            }

            LANGULUS_ASSERT(IsMutable(), Access,
               "Attempting to remove from constant container");
            LANGULUS_ASSERT(not IsStatic(), Access,
               "Attempting to remove from static container");

            // First call the destructors on the correct region         
            BranchOut();
            CropInner(idx, count).FreeInner();

            if constexpr (CT::Sparse<TYPE> or CT::POD<TYPE>) {
               // Batch move                                            
               MoveMemory(GetRaw() + idx, GetRaw() + ender, mCount - ender);

               if constexpr (CT::Sparse<TYPE>)
                  MoveMemory(GetEntries() + idx, GetEntries() + ender, mCount - ender);
            }
            else if (ender < mCount) {
               // Fill gap	if any by invoking move constructions        
               // Moving to the left, so no overlap possible            
               const auto tail = mCount - ender;
               CropInner(idx, tail).CreateWithIntent(
                  Abandon(CropInner(ender, tail)));
            }
         }

         // Change count                                                
         mCount -= removed;
         return removed;
      }
   }

   /// Remove a deep index corresponding to a whole sub-block                 
   ///   @param index - index to remove                                       
   ///   @return 1 if block at that index was removed, 0 otherwise            
   template<class TYPE>
   Count Block<TYPE>::RemoveIndexDeep(CT::Index auto index) {
      if constexpr (not CT::Same<decltype(index), Index>) {
         if (not IsDeep())
            return 0;

         --index;

         for (Count i = 0; i != mCount; i += 1) {
            if (index == 0)
               return RemoveIndex(i);

            auto& ith = GetDeep(i);
            const auto count = ith.GetCountDeep();
            if (index <= count and ith.RemoveIndexDeep(index))
               return 1;

            index -= count;
         }

         return 0;
      }
      else TODO();
   }

   /// Remove an element by using an iterator                                 
   ///   @param index - the iterator of the starting element                  
   ///   @param count - the number of elements to remove                      
   ///   @return an iterator pointing to the element at index - 1, or at end  
   ///      if block became empty                                             
   template<class TYPE>
   auto Block<TYPE>::RemoveIt(const Iterator& index, const Count count) -> Iterator {
      if (index.mValue >= GetRawEnd())
         return end();

      const auto rawstart = GetRaw();
      RemoveIndex(index.mValue - rawstart, count);

      if (IsEmpty())
         return end();
      else if (index.mValue == rawstart)
         return {rawstart, GetRawEnd()};
      else
         return {index.mValue - 1, GetRawEnd()};
   }

   /// Remove elements at the back                                            
   ///   @param count - the new count                                         
   template<class TYPE>
   void Block<TYPE>::Trim(const Count count) {
      if (count >= mCount)
         return;

      if (IsConstant() or IsStatic()) {
         if (mType->mIsPOD) {
            // If data is POD and elements are on the back, we can      
            // get around constantness and staticness, by simply        
            // truncating the count without any reprecussions           
            mCount = count;
         }
         else {
            LANGULUS_ASSERT(not IsConstant(), Access,
               "Removing from constant container");
            LANGULUS_ASSERT(not IsStatic(), Access,
               "Removing from static container");
         }

         return;
      }

      // Call destructors and change count                              
      BranchOut();
      CropInner(count, mCount - count).FreeInner();
      mCount = count;
   }

   /// Flattens unnecessarily deep containers and combines their states       
   /// when possible                                                          
   /// Discards ORness if container has only one element                      
   template<class TYPE>
   void Block<TYPE>::Optimize() {
      while (GetCount() == 1 and IsDeep()) {
         auto& subPack = GetDeep();
         if (not CanFitState(subPack)) {
            subPack.Optimize();
            if (subPack.IsEmpty())
               Reset();
            return;
         }

         auto temporary = subPack;
         subPack.ResetMemory();
         Free();
         *this = temporary;
      }

      if (GetCount() > 1 and IsDeep()) {
         for (Count i = 0; i < mCount; ++i) {
            auto& subBlock = GetDeep(i);
            subBlock.Optimize();
            if (subBlock.IsEmpty()) {
               RemoveIndex(i);
               --i;
            }
         }
      }
   }

   /// Destroy all elements, but don't deallocate memory if possible          
   template<class TYPE> LANGULUS(INLINED)
   void Block<TYPE>::Clear() {
      if (not mEntry) {
         // Data is either static or unallocated                        
         // Don't call destructors, just clear it up                    
         mRaw   = nullptr;
         mCount = mReserved = 0;
         ResetType();
         return;
      }

      if (mEntry->GetUses() == 1) {
         // Entry is used only in this block, so it's safe to           
         // destroy all elements. We will reuse the entry and type      
         if (mCount) {
            FreeInner();
            mCount = 0;
         }
      }
      else {
         // If reached, then data is referenced from multiple places    
         // Don't call destructors, just clear it up and dereference    
         const_cast<Allocation*>(mEntry)->Free();
         mRaw   = nullptr;
         mEntry = nullptr;
         mCount = mReserved = 0;
         ResetType();
      }
   }

   /// Destroy all elements, deallocate block and reset state                 
   template<class TYPE> LANGULUS(INLINED)
   void Block<TYPE>::Reset() {
      Free();
      ResetMemory();
      ResetState();
   }
   
   /// Reset the block's state                                                
   /// Type constraints shall remain, if any                                  
   template<class TYPE> LANGULUS(INLINED)
   constexpr void Block<TYPE>::ResetState() noexcept {
      mState &= DataState::Typed;
      ResetType();
   }

} // namespace Langulus::Annies