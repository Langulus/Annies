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
   
   /// Convert an index to an offset                                          
   /// Complex indices will be fully constrained                              
   /// Unsigned/signed integers are directly forwarded without any overhead   
   ///   @attention assumes index is in container reserve limit, if integer   
   ///   @param index - the index to simplify                                 
   ///   @return the simplified index, as a simple offset                     
   template<CT::Set THIS, CT::Index INDEX> LANGULUS(INLINED)
   Offset BlockSet::SimplifyIndex(const INDEX index) const
   noexcept(not LANGULUS_SAFE() and CT::BuiltinInteger<INDEX>) {
      if constexpr (CT::Same<INDEX, Index>) {
         // This is the most safe path, throws on errors, but slow      
         if constexpr (CT::Typed<THIS>) {
            const auto result = index.Constrained(GetReserved());

            if (result == IndexBiggest) {
               TODO();
            }
            else if (result == IndexSmallest) {
               TODO();
            }

            return result.GetOffset();
         }
         else return index.Constrained(GetReserved()).GetOffset();
      }
      else {
         // Unsafe, works only on assumptions                           
         // Using an integer index explicitly makes a statement, that   
         // you know what you're doing                                  
         LglsAssumeUser(
            static_cast<Count>(index) < GetReserved(),
            "Integer index out of range");

         if constexpr (CT::Signed<INDEX>) {
            LglsAssumeUser(index >= 0, 
               "Integer index is below zero, "
               "use Index for reverse indices instead"
            );
         }

         return index;
      }
   }

   /// Get a valid key by any index, safely                                   
   ///   @param index - the index to use                                      
   ///   @return the element, wrapped in a Block                              
   template<CT::Set THIS> LANGULUS(INLINED)
   decltype(auto) BlockSet::Get(const CT::Index auto index) const {
      if (IsEmpty())
         LANGULUS_OOPS(OutOfRange, "Set is empty");

      const auto idx = SimplifyIndex<THIS>(index);
      if (not mInfo[idx])
         LANGULUS_OOPS(OutOfRange, "No element at given index");
      return GetRef<THIS>(idx);
   }
   
   /// Accesses set elements based on a safe index, that accounts for         
   /// empty table slots                                                      
   ///   @param idx - the index to access                                     
   ///   @return a reference to the value, or a block if set is type-erased   
   template<CT::Set THIS> LANGULUS(INLINED)
   decltype(auto) BlockSet::operator[] (const CT::Index auto index) const {
      if (IsEmpty())
         LANGULUS_OOPS(Access, "Attempting to access an empty set by index");

      using INDEX = decltype(index);
      Offset i;
      if constexpr (CT::Same<INDEX, Index>) {
         // This is the most safe path, throws on errors, but slow      
         if constexpr (CT::Typed<THIS>) {
            const auto result = index.Constrained(GetCount());
            if (result == IndexBiggest) {
               TODO();
            }
            else if (result == IndexSmallest) {
               TODO();
            }
            i = result.GetOffset();
         }
         else i = index.Constrained(GetCount()).GetOffset();
      }
      else {
         // Unsafe, works only on assumptions                           
         // Using an integer index explicitly makes a statement, that   
         // you know what you're doing                                  
         LglsAssumeUser(
            static_cast<Count>(index) < GetCount(),
            "Integer index out of range");

         if constexpr (CT::Signed<INDEX>) {
            LglsAssumeUser(index >= 0, 
               "Integer index is below zero, "
               "use Index for reverse indices instead"
            );
         }

         i = static_cast<Offset>(index);
      }

      auto info = GetInfo();
      const auto infoEnd = GetInfoEnd();
      while (info != infoEnd) {
         if (*info) {
            if (i == 0)
               return GetRef<THIS>(info - GetInfo());
            --i;
         }
         ++info;
      }

      // If reached, then index was invalid                             
      LANGULUS_OOPS(Access, "Unknown error when accessing set via index");
      return GetRef<THIS>(0);
   }

   /// Get a raw key by an unsafe offset                                      
   ///   @attention assumes index is in container's limits                    
   ///   @param i - the offset to use                                         
   ///   @return the element, wrapped in a Block                              
   template<CT::Set THIS> LANGULUS(INLINED)
   decltype(auto) BlockSet::GetRaw(const Offset i) IF_UNSAFE(noexcept) {
      LglsAssumeDev(i < GetReserved(),
         "Index out of limits when accessing set",
         ", index ", i, " is beyond the reserved ", GetReserved(), " elements");

      if constexpr (CT::Typed<THIS>) {
         IF_SAFE(using K = TypeOf<THIS>);
         LglsAssumeDev((IsSimilar<THIS, K>()),
            "Wrong type when accessing set",
            ", using type `", NameOf<K>(), "` instead of `", GetType(), '`');
         return GetValues<THIS>().GetRaw() + i;
      }
      else return GetValues<THIS>().GetElementInner(i);
   }

   template<CT::Set THIS> LANGULUS(INLINED)
   decltype(auto) BlockSet::GetRaw(const Offset i) const IF_UNSAFE(noexcept) {
      return const_cast<BlockSet*>(this)->template GetRaw<THIS>(i);
   }

   /// Get a key ref by an unsafe offset                                      
   ///   @attention assumes index is in container's limits                    
   ///   @param i - the offset to use                                         
   ///   @return the element, wrapped in a Block                              
   template<CT::Set THIS> LANGULUS(INLINED)
   decltype(auto) BlockSet::GetRef(const Offset i) IF_UNSAFE(noexcept) {
      if constexpr (CT::Typed<THIS>)
         return *GetRaw<THIS>(i);
      else
         return GetRaw<THIS>(i);
   }

   template<CT::Set THIS> LANGULUS(INLINED)
   decltype(auto) BlockSet::GetRef(const Offset i) const IF_UNSAFE(noexcept) {
      return const_cast<BlockSet*>(this)->template GetRef<THIS>(i);
   }

   /// Get the bucket index, based on the provided value's hash               
   ///   @param mask - a mask for ANDing the relevant part of the hash        
   ///   @param value - the value to hash                                     
   ///   @return the bucket index                                             
   LANGULUS(INLINED)
   Offset BlockSet::GetBucket(const Offset mask, const CT::NoIntent auto& value) noexcept {
      return HashOf(value).mHash & mask;
   }
   
   /// Get the bucket index, based on the wrapped value's hash                
   ///   @param mask - a mask for ANDing the relevant part of the hash        
   ///   @param value - the value to hash, wrapped in a block                 
   ///   @return the bucket index                                             
   LANGULUS(INLINED)
   Offset BlockSet::GetBucketUnknown(const Offset mask, const Block<>& value) noexcept {
      return value.GetHash().mHash & mask;
   }

   /// Get an element handle                                                  
   ///   @attention assumes index is in container's limits                    
   ///   @param i - the key index                                             
   ///   @return the handle                                                   
   template<CT::Set THIS> LANGULUS(INLINED)
   auto BlockSet::GetHandle(const Offset i) IF_UNSAFE(noexcept) {
      LglsAssumeDev(i < GetReserved(),
         "Index out of limits when accessing set",
         ", index ", i, " is beyond the reserved ", GetReserved(), " elements");

      if constexpr (CT::Typed<THIS>) {
         IF_SAFE(using K = TypeOf<THIS>);
         LglsAssumeDev((IsSimilar<THIS, K>()),
            "Wrong type when accessing set",
            ", using type `", NameOf<K>(), "` instead of `", GetType(), '`');
         return GetValues<THIS>().GetHandle(i);
      }
      else return GetValues<THIS>().GetElementInner(i);
   }

} // namespace Langulus::Annies
