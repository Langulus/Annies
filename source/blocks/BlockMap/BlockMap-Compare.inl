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

   /// Compare this map against another map, type-erased or not               
   ///   @param rhs - map to compare against                                  
   ///   @return true if contents of both maps are the same                   
   template<CT::Map THIS>
   bool BlockMap::operator == (CT::Map auto const& rhs) const {
      if (rhs.GetCount() != GetCount()
      or not IsTypeCompatibleWith<THIS>(rhs))
         return false;

      // If reached, then both maps contain similar types of data       
      // Prefer the map that is statically typed, if there is one       
      using RHS = Conditional<CT::Typed<THIS>, THIS, Deref<decltype(rhs)>>;
      auto info = GetInfo();
      const auto infoEnd = GetInfoEnd();
      while (info != infoEnd) {
         if (*info) {
            // Compare each valid pair...                               
            const auto lidx = info - GetInfo();
            Offset ridx;
            if constexpr (CT::Typed<RHS>)
               ridx = rhs.template FindInner<RHS>(GetKeyRef<RHS>(lidx));
            else
               ridx = rhs.template FindBlockInner<RHS>(GetKeyRef<RHS>(lidx));

            if (ridx == InvalidOffset
            or GetValRef<RHS>(lidx) != rhs.template GetValRef<RHS>(ridx))
               return false;
         }

         ++info;
      }

      // If reached, then both maps are the same                        
      return true;
   }

   /// Compare this map against a single pair, type-erased or not             
   ///   @param rhs - pair to compare against                                 
   ///   @return true this map contains only this exact pair                  
   template<CT::Map THIS>
   bool BlockMap::operator == (CT::Pair auto const& rhs) const {
      if (1 != GetCount() or not IsTypeCompatibleWith<THIS>(rhs))
         return false;

      // If reached, then pair contains similar types of data           
      using P = Deref<decltype(rhs)>;
      using RHS = Conditional<CT::Typed<THIS>
         , THIS
         , TMap<typename P::Key, typename P::Value, THIS::Ordered>>;

      Offset idx;
      if constexpr (CT::Typed<RHS>)
         idx = FindInner<RHS>(rhs.mKey);
      else
         idx = FindBlockInner(rhs.mKey);

      if (idx == InvalidOffset or GetValRef<RHS>(idx) != rhs.mValue)
         return false;

      // If reached, then map contains that exact pair                  
      return true;
   }
   
   /// Get hash of the map contents                                           
   ///   @attention the hash is not cached, so this is a slow operation       
   ///   @return the hash                                                     
   template<CT::Map THIS> LANGULUS(INLINED)
   Hash BlockMap::GetHash() const {
      if (IsEmpty())
         return {};

      TMany<Hash> hashes;
      hashes.Reserve(GetCount());
      for (auto pair : reinterpret_cast<const THIS&>(*this))
         hashes << pair.GetHash();
      return hashes.GetHash();
   }

   /// Search for a key inside the table                                      
   ///   @param key - the key to search for                                   
   ///   @return true if key is found, false otherwise                        
   template<CT::Map THIS> LANGULUS(INLINED)
   bool BlockMap::ContainsKey(const CT::NoIntent auto& key) const {
      return FindInner<THIS>(key) != InvalidOffset;
   }

   /// Search for a value inside the table                                    
   ///   @param value - the value to search for                               
   ///   @return true if value is found, false otherwise                      
   template<CT::Map THIS> LANGULUS(INLINED)
   bool BlockMap::ContainsValue(const CT::NoIntent auto& value) const {
      if (IsEmpty())
         return false;

      // Check argument compatiblity                                    
      using V = Deref<decltype(value)>;
      if constexpr (not CT::Typed<THIS>) {
         if (not IsValueSimilar<THIS, V>()) {
            if constexpr (CT::Owned<V>) {
               if (not IsValueSimilar<THIS, TypeOf<V>>())
                  return false;
            }
            else return false;
         }
      }
      else {
         static_assert(CT::Comparable<typename THIS::Value, V>,
            "Provided value is not comparable to map's value type");
      }

      auto info = GetInfo();
      const auto infoEnd = GetInfoEnd();

      while (info != infoEnd) {
         if (*info and GetValRef<THIS>(info - GetInfo()) == value)
            return true;
         ++info;
      }

      return false;
   }

   /// Search for a pair inside the table                                     
   ///   @param pair - the pair to search for                                 
   ///   @return true if pair is found, false otherwise                       
   template<CT::Map THIS> LANGULUS(INLINED)
   bool BlockMap::ContainsPair(const CT::Pair auto& pair) const {
      if (IsEmpty())
         return false;

      using P = Deref<decltype(pair)>;
      auto& me = reinterpret_cast<THIS&>(*this);
      Offset found;

      if constexpr (CT::Typed<P>) {
         // Search for a typed pair                                     
         using V = typename P::Value;
         if (not me.template ValueIsSimilar<V>()
         and not (CT::Typed<THIS> and CT::Comparable<typename THIS::Value, V>))
            return false;

         found = FindInner<THIS>(pair.mKey);
      }
      else {
         // Search for a type-erased pair                               
         if (not me.IsValueSimilar(pair.GetValueType()))
            return false;

         found = FindInnerUnknown<THIS>(pair.mKey);
      }

      return found != InvalidOffset and me.GetValueInner(found) == pair.mValue;
   }
   
   /// Search for a key inside the table, and return it if found              
   ///   @param key - the key to search for                                   
   ///   @return the index if key was found, or IndexNone if not              
   template<CT::Map THIS> LANGULUS(INLINED)
   Index BlockMap::Find(const CT::NoIntent auto& key) const {
      const auto offset = FindInner<THIS>(key);
      return offset != InvalidOffset ? Index {offset} : IndexNone;
   }

   /// Search for a key inside the table, and return an iterator to it        
   ///   @param key - the key to search for                                   
   ///   @return the iterator                                                 
   template<CT::Map THIS> LANGULUS(INLINED)
   BlockMap::Iterator<THIS> BlockMap::FindIt(const CT::NoIntent auto& key) {
      const auto offset = FindInner<THIS>(key);
      if (offset == InvalidOffset)
         return end();

      return {
         GetInfo() + offset, GetInfoEnd(),
         GetRawKey<THIS>(offset),
         GetRawVal<THIS>(offset)
      };
   }
   
   template<CT::Map THIS> LANGULUS(INLINED)
   BlockMap::Iterator<const THIS> BlockMap::FindIt(const CT::NoIntent auto& key) const {
      return const_cast<BlockMap*>(this)->template FindIt<THIS>(key);
   }

   /// Returns a reference to the value found for key                         
   /// Throws Except::OutOfRange if element cannot be found                   
   ///   @param key - the key to search for                                   
   ///   @return the value, wrapped in a type-erased block                    
   template<CT::Map THIS> LANGULUS(INLINED)
   decltype(auto) BlockMap::At(const CT::NoIntent auto& key) {
      const auto found = FindInner<THIS>(key);
      LANGULUS_ASSERT(found != InvalidOffset, OutOfRange, "Key not found");
      return GetValRef<THIS>(found);
   }
   
   template<CT::Map THIS> LANGULUS(INLINED)
   decltype(auto) BlockMap::At(const CT::NoIntent auto& key) const {
      return const_cast<BlockMap*>(this)->template At<THIS>(key);
   }

   /// Access value by key                                                    
   ///   @param key - the key to find                                         
   ///   @return the value, wrapped in a type-erased block                    
   template<CT::Map THIS> LANGULUS(INLINED)
   decltype(auto) BlockMap::operator[] (const CT::NoIntent auto& key) {
      return At<THIS>(key);
   }

   template<CT::Map THIS> LANGULUS(INLINED)
   decltype(auto) BlockMap::operator[] (const CT::NoIntent auto& key) const {
      return At<THIS>(key);
   }

   /// Find the index of a pair by key                                        
   /// The key may not match the contained key type                           
   ///   @param match - the key to search for                                 
   ///   @return the index, or InvalidOffset if not found                     
   template<CT::Map THIS>
   Offset BlockMap::FindInner(const CT::NoIntent auto& match) const {
      if (IsEmpty())
         return InvalidOffset;

      using K = Deref<decltype(match)>;
      if constexpr (CT::StringLiteral<K>) {
         if (IsKeySimilar<THIS, Text>()) {
            // Implicitly make a text container on string literal       
            return FindInner<THIS>(Text {Disown(match)});
         }
         else if (IsKeySimilar<THIS, char*, wchar_t*>()) {
            // Cast away the extent, search for pointer                 
            return FindInner<THIS>(static_cast<const Deext<K>*>(match));
         }
         else return InvalidOffset;
      }
      else {
         if constexpr (CT::Typed<THIS>) {
            // Types known at compile-time                              
            static_assert(CT::Comparable<typename THIS::Key, K>,
               "Types not comparable");
         }
         else if (not IsKeySimilar<THIS, K>()) {
            // K might be wrapped inside Own                            
            if constexpr (CT::Owned<K>) {
               if (not IsKeySimilar<THIS, TypeOf<K>>())
                  return InvalidOffset;
            }
            else return InvalidOffset;
         }

         // Get the starting index based on the key hash                
         const auto start = GetBucket<THIS>(GetReserved() - 1, match);
         auto info = GetInfo() + start;
         if (not *info)
            return InvalidOffset;

         // Test first candidate                                        
         if (GetKeyRef<THIS>(start) == match)
            return start;

         // Test all candidates on the right up until the end           
         ++info;

         auto infoEnd = GetInfoEnd();
         while (info < infoEnd) {
            if (not *info)
               return InvalidOffset;

            const auto i = info - GetInfo();
            if (GetKeyRef<THIS>(i) == match)
               return i;

            ++info;
         }

         // Reached only if info has reached the end                    
         // Keys might loop around, continue the search from the start  
         info = GetInfo();
         if (not *info)
            return InvalidOffset;

         if (GetKeyRef<THIS>(0) == match)
            return 0;

         ++info;

         infoEnd = GetInfo() + start;
         while (info < infoEnd) {
            if (not *info)
               return InvalidOffset;

            const auto i = info - GetInfo();
            if (GetKeyRef<THIS>(i) == match)
               return i;

            ++info;
         }

         // No such key was found                                       
         return InvalidOffset;
      }
   }
   
   /// Find the index of a pair by a type-erased key                          
   ///   @param match - the key to search for                                 
   ///   @return the index, or InvalidOffset if not found                     
   template<CT::Map THIS>
   Offset BlockMap::FindBlockInner(const Block<>& match) const {
      if (IsEmpty() or not IsKeySimilar<THIS>(match.GetType()))
         return InvalidOffset;

      // Get the starting index based on the key hash                   
      const auto start = GetBucketUnknown(GetReserved() - 1, match);
      auto info = GetInfo() + start;
      if (not *info)
         return InvalidOffset;

      // Test first candidate                                           
      if (GetKeyRef<THIS>(start) == match)
         return start;

      // Test all candidates on the right up until the end              
      ++info;

      auto infoEnd = GetInfoEnd();
      while (info != infoEnd) {
         if (not *info)
            return InvalidOffset;

         const auto i = info - GetInfo();
         if (GetKeyRef<THIS>(i) == match)
            return i;

         ++info;
      }

      // Reached only if info has reached the end                       
      // Keys might loop around, continue the search from the start     
      info = GetInfo();
      if (not *info)
         return InvalidOffset;

      if (GetKeyRef<THIS>(0) == match)
         return 0;

      ++info;

      infoEnd = GetInfo() + start;
      while (info != infoEnd) {
         if (not *info)
            return InvalidOffset;

         const auto i = info - GetInfo();
         if (GetKeyRef<THIS>(i) == match)
            return i;

         ++info;
      }
      
      // No such key was found                                          
      return InvalidOffset;
   }
   
} // namespace Langulus::Annies
