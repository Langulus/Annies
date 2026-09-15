///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../BlockMap.hpp"
#include "../../text/Text.hpp"


namespace Langulus::Annies
{
      
   /// Safely erases element at a specific iterator                           
   ///   @attention assumes iterator is produced by this map instance         
   ///   @attention assumes that iterator points to a valid entry             
   ///   @param index - the index to remove                                   
   ///   @return the iterator of the previous element, unless index is the    
   ///           first, or already at the end                                 
   template<CT::Map THIS> LANGULUS(INLINED)
   auto BlockMap::RemoveIt(const Iterator<THIS>& index) -> Iterator<THIS> {
      const auto sentinel = GetReserved();
      auto offset = static_cast<Offset>(index.mInfo - mInfo);
      if (offset >= sentinel)
         return end();

      RemoveInner<THIS>(offset--);

      if (IsEmpty())
         return end();
      
      while (offset < sentinel and not mInfo[offset])
         --offset;

      if (offset >= sentinel)
         offset = 0;

      return {
         mInfo + offset, 
         index.mSentinel,
         GetRawKey<THIS>(offset),
         GetRawVal<THIS>(offset)
      };
   }

   /// Unfold-erase pairs by key                                              
   ///   @param key - the key to search for                                   
   ///   @return the number of removed pairs                                  
   template<CT::Map THIS> LANGULUS(INLINED)
   Count BlockMap::RemoveKey(const CT::NoIntent auto& key) {
      using K = Deref<decltype(key)>;
      if (IsEmpty())
         return 0;

      if constexpr (CT::Array<K>) {
         if constexpr (CT::StringLiteral<K>) {
            if (IsKeySimilar<THIS, Text>()) {
               // Implicitly make a text container on string literal    
               return RemoveKeyInner<THIS>(Text {Disown(key)});
            }
            else if (IsKeySimilar<THIS, char*, wchar_t*>()) {
               // Cast away the extent, search for pointer              
               return RemoveKeyInner<THIS>(static_cast<const Deext<K>*>(key));
            }
            else return 0;
         }
         else if (IsKeySimilar<THIS, Deext<K>>()
         or (CT::Typed<THIS> and CT::Comparable<typename THIS::Key, Deext<K>>)) {
            // Remove all matching keys in the array                    
            Count removed = 0;
            for (auto& element : key)
               removed += RemoveKeyInner<THIS>(key);
            return removed;
         }
      }
      else {
         if (IsKeySimilar<THIS, K>()
         or (CT::Typed<THIS> and CT::Comparable<typename THIS::Key, K>)) {
            // Remove a single key                                      
            return RemoveKeyInner<THIS>(key);
         }
         else if constexpr (CT::Owned<K>) {
            if (IsKeySimilar<THIS, TypeOf<K>>()
            or (CT::Typed<THIS> and CT::Comparable<typename THIS::Key, TypeOf<K>>)) {
               // Remove a single key                                      
               return RemoveKeyInner<THIS>(key.Get());
            }
         }
      }
      return 0;
   }
   
   /// Erase a pair via key (inner)                                           
   ///   @param key - the key to search for                                   
   ///   @return 1 if pair was removed                                        
   template<CT::Map THIS> LANGULUS(INLINED)
   Count BlockMap::RemoveKeyInner(const CT::NoIntent auto& key) {
      const auto found = FindInner<THIS>(key);
      if (found != InvalidOffset) {
         // Key found, remove the pair                                  
         RemoveInner<THIS>(found);
         return 1;
      }

      // No such key was found                                          
      return 0;
   }

   /// Unfold-erase pairs by value                                            
   ///   @param value - the values to search for                              
   ///   @return the number of removed pairs                                  
   template<CT::Map THIS> LANGULUS(INLINED)
   Count BlockMap::RemoveValue(const CT::NoIntent auto& value) {
      using V = Deref<decltype(value)>;
      if (IsEmpty())
         return 0;

      if constexpr (CT::Array<V>) {
         if constexpr (CT::StringLiteral<V>) {
            if (IsValueSimilar<THIS, Text>()) {
               // Implicitly make a text container on string literal    
               return RemoveValInner<THIS>(Text {Disown(value)});
            }
            else if (IsValueSimilar<THIS, char*, wchar_t*>()) {
               // Cast away the extent, search for pointer instead      
               return RemoveValInner<THIS>(static_cast<const Deext<V>*>(value));
            }
            else return 0;
         }
         else if (IsValueSimilar<THIS, Deext<V>>()
         or (CT::Typed<THIS> and CT::Comparable<typename THIS::Value, Deext<V>>)) {
            // Remove all matching values in the array                  
            Count removed = 0;
            for (auto& element : value)
               removed += RemoveValInner<THIS>(value);
            return removed;
         }
      }
      else if constexpr (CT::Owned<V> or CT::Handle<V>) {
         // Remove a single value                                       
         return RemoveValInner<THIS>(value.Get());
      }
      else if (IsValueSimilar<THIS, V>()
      or (CT::Typed<THIS> and CT::Comparable<typename THIS::Value, V>)) {
         // Remove a single value                                       
         return RemoveValInner<THIS>(value);
      }
      return 0;
   }

   /// Erase all pairs with a given value                                     
   ///   @attention this is significantly slower than removing a key          
   ///   @param value - the value to search for                               
   ///   @return the number of removed pairs                                  
   template<CT::Map THIS>
   Count BlockMap::RemoveValInner(const CT::NoIntent auto& value) {
      Count removed = 0;
      auto psl = GetInfo();
      const auto pslEnd = GetInfoEnd();
      auto val = GetValHandle<THIS>(0);

      while (psl != pslEnd) {
         if (*psl and val == value) {
            const auto index = psl - GetInfo();
            if (BranchOut<THIS>()) {
               // Refresh pointers if memory moves                      
               val = GetValHandle<THIS>(index);
               psl = GetInfo() + index;
            }

            // Remove every pair with matching value                    
            auto key = GetKeyHandle<THIS>(index);
            key.FreeInner();
            val.FreeInner();
            *psl = 0;
            ++removed;
            --mKeys.mCount;
         }

         ++val;
         ++psl;
      }

      // Fill gaps if any                                               
      ShiftPairs<THIS>();
      return removed;
   }
   
   /// Erases a statically typed pair at a specific index                     
   ///   @attention if this map has more than one use, it will be copied to   
   ///      a new place, before any removals are done                         
   ///   @attention assumes that index points to a valid entry                
   ///   @param index - the index to remove                                   
   template<CT::Map THIS>
   void BlockMap::RemoveInner(const Offset index) {
      BranchOut<THIS>();
      auto psl = GetInfo() + index;
      LANGULUS_ASSUME(DevAssumes, *psl, "Removing an invalid pair");

      // Destroy the key, info and value at the start                   
      // Use statically typed optimizations where possible              
      auto key = GetKeyHandle<THIS>(index);
      key.FreeInner();
      ++key;

      auto val = GetValHandle<THIS>(index);
      val.FreeInner();
      ++val;

      *(psl++) = 0;

      // And shift backwards, until a zero or 1 is reached              
      // That way we move every entry that is far from its start        
      // closer to it. Moving is costly, unless you use pointers        
      try_again:
      while (*psl > 1) {
         psl[-1] = (*psl) - 1;

         (key--).CreateWithIntent(Abandon(key));
         key.FreeInner();
         ++key;

         (val--).CreateWithIntent(Abandon(val));
         val.FreeInner();
         ++val;

         *(psl++) = 0;
      }

      // Be aware, that iterator might loop around                      
      const auto pslEnd = GetInfoEnd();
      if (psl == pslEnd and *GetInfo() > 1) {
         const auto last = mKeys.mReserved - 1;
         psl = GetInfo();
         GetInfo()[last] = (*psl) - 1;

         // Shift first pair to the back                                
         key = GetKeyHandle<THIS>(0);
         GetKeyHandle<THIS>(last).CreateWithIntent(Abandon(key));
         key.FreeInner();
         ++key;

         val = GetValHandle<THIS>(0);
         GetValHandle<THIS>(last).CreateWithIntent(Abandon(val));
         val.FreeInner();
         ++val;

         *(psl++) = 0;

         // And continue the vicious cycle                              
         goto try_again;
      }

      // Success                                                        
      --mKeys.mCount;
   }

   /// Clears all data, but doesn't deallocate                                
   template<CT::Map THIS> LANGULUS(INLINED)
   void BlockMap::Clear() {
      if (IsEmpty())
         return;

      // Always destroy values before keys, because keys contain mInfo  
      if (mValues.mEntry->GetUses() == 1) {
         // Value memory can be reused                                  
         GetVals<THIS>().FreeInner(mInfo);
      }
      else {
         // Data is used from multiple locations, don't change data     
         // We're forced to dereference and reset value pointers        
         GetVals<THIS>().template FreeInner<false>(mInfo);
         const_cast<Allocation*>(mValues.mEntry)->Free();
      }

      if (mKeys.mEntry->GetUses() == 1) {
         // Key memory can be reused, which means info is reusable, too 
         GetKeys<THIS>().FreeInner(mInfo);
      }
      else {
         // Data is used from multiple locations, don't change data     
         // We're forced to dereference and reset key pointers          
         GetKeys<THIS>().template FreeInner<false>(mInfo);
         const_cast<Allocation*>(mKeys.mEntry)->Free();
      }

      // Info array must be cleared at the end                          
      if (mKeys.mEntry->GetUses() == 1) {
         ZeroMemory(mInfo, GetReserved());
         mKeys.mCount = 0;
      }
      else {
         mInfo = nullptr;
         mKeys.ResetMemory();
      }

      if (mValues.mEntry->GetUses() != 1)
         mValues.ResetMemory();
   }

   /// Clears all data and deallocates                                        
   template<CT::Map THIS> LANGULUS(INLINED)
   void BlockMap::Reset() {
      // Always destroy values before keys, because keys contain mInfo  
      if (mValues.mEntry) {
         if (mValues.mEntry->GetUses() == 1) {
            // Remove all used keys and values, they're used only here  
            if (not IsEmpty())
               GetVals<THIS>().FreeInner(mInfo);
            Allocator::Deallocate(const_cast<Allocation*>(mValues.mEntry));
         }
         else {
            // Data is used from multiple locations, just deref values  
            if (not IsEmpty())
               GetVals<THIS>().template FreeInner<false>(mInfo);
            const_cast<Allocation*>(mValues.mEntry)->Free();
         }
      }

      if (mKeys.mEntry) {
         if (mKeys.mEntry->GetUses() == 1) {
            // Remove all used keys, they're used only here             
            if (not IsEmpty())
               GetKeys<THIS>().FreeInner(mInfo);
            Allocator::Deallocate(const_cast<Allocation*>(mKeys.mEntry));
         }
         else {
            // Data is used from multiple locations, just deref keys    
            if (not IsEmpty())
               GetKeys<THIS>().template FreeInner<false>(mInfo);
            const_cast<Allocation*>(mKeys.mEntry)->Free();
         }
      }

      mInfo = nullptr;
      mKeys.ResetMemory();
      mValues.ResetMemory();
      mKeys.ResetState();
      mValues.ResetState();
   }
   
   /// If possible reallocates the map to a smaller one                       
   template<CT::Map THIS> LANGULUS(INLINED)
   void BlockMap::Compact() {
      TODO();
   }

} // namespace Langulus::Annies
