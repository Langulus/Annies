///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../BlockSet.hpp"
#include "../../text/Text.hpp"


namespace Langulus::Annies
{

   /// Checks if both sets contain the same entries                           
   ///   @param rhs - the set/value to compare against, type-erased or not    
   ///   @return true if sets are the same                                    
   template<CT::Set THIS>
   bool BlockSet::operator == (const CT::NoIntent auto& rhs) const {
      using OTHER = Deref<decltype(rhs)>;

      if constexpr (CT::Set<OTHER>) {
         // Compare against another set of any kind                     
         if (rhs.GetCount() != GetCount()
         or not IsTypeCompatibleWith<THIS>(rhs))
            return false;

         // If reached, then both sets contain similar types of data    
         using TYPED = Conditional<CT::Typed<THIS>, THIS, OTHER>;
         auto info = GetInfo();
         const auto infoEnd = GetInfoEnd();
         while (info != infoEnd) {
            if (*info) {
               // Compare each valid entry...                           
               const auto index = info - GetInfo();
               if constexpr (CT::Typed<TYPED>) {
                  // ...with known types, if any of the sets are typed  
                  if (rhs.template FindInner<TYPED>(GetRef<TYPED>(index))
                  == InvalidOffset)
                     return false;
               }
               else {
                  // ...via rtti, since all sets are type-erased        
                  if (rhs.template FindBlockInner<TYPED>(GetRef<TYPED>(index))
                  == InvalidOffset)
                     return false;
               }
            }

            ++info;
         }

         // If reached, then both sets are the same                     
         return true;
      }
      else {
         // Compare against a single value                              
         return GetCount() == 1 and Contains<THIS>(rhs);
      }
   }

   /// Get hash of the set contents                                           
   ///   @attention the hash is not cached, so this is a slow operation       
   ///   @return the hash                                                     
   template<CT::Set THIS> LANGULUS(INLINED)
   Hash BlockSet::GetHash() const {
      TMany<Hash> hashes;
      for (auto& element : reinterpret_cast<const THIS&>(*this))
         hashes << element.GetHash();
      return hashes.GetHash();
   }

   /// Search for a key inside the table                                      
   ///   @param key - the key to search for                                   
   ///   @return true if key is found, false otherwise                        
   template<CT::Set THIS> LANGULUS(INLINED)
   bool BlockSet::Contains(const CT::NoIntent auto& key) const {
      return FindInner<THIS>(key) != InvalidOffset;
   }

   /// Search for a key inside the table, and return it if found              
   ///   @param key - the key to search for                                   
   ///   @return the index if key was found, or IndexNone if not              
   template<CT::Set THIS> LANGULUS(INLINED)
   Index BlockSet::Find(const CT::NoIntent auto& key) const {
      const auto offset = FindInner<THIS>(key);
      return offset != InvalidOffset ? Index {offset} : IndexNone;
   }

   /// Search for a key inside the table, and return an iterator to it        
   ///   @param key - the key to search for                                   
   ///   @return the iterator                                                 
   template<CT::Set THIS> LANGULUS(INLINED)
   BlockSet::Iterator<THIS> BlockSet::FindIt(const CT::NoIntent auto& key) {
      const auto offset = FindInner<THIS>(key);
      if (offset == InvalidOffset)
         return end();

      return {
         GetInfo() + offset, GetInfoEnd(),
         GetRaw<THIS>(offset)
      };
   }

   template<CT::Set THIS> LANGULUS(INLINED)
   BlockSet::Iterator<const THIS> BlockSet::FindIt(const CT::NoIntent auto& key) const {
      return const_cast<BlockSet*>(this)->template FindIt<THIS>(key);
   }

   /// Find the index of a key                                                
   /// The key may not match the contained key type, if THIS is typed         
   ///   @param match - the key to search for                                 
   ///   @return the index, or InvalidOffset if not found                     
   template<CT::Set THIS>
   Offset BlockSet::FindInner(const CT::NoIntent auto& match) const {
      using K = Deref<decltype(match)>;
      if (IsEmpty())
         return InvalidOffset;

      auto& me = reinterpret_cast<const THIS&>(*this);
      if constexpr (CT::StringLiteral<K>) {
         if (me.template IsSimilar<Text>()) {
            // Implicitly make a text container on string literal       
            return FindInner<THIS>(Text {Disown(match)});
         }
         else if (me.template IsSimilar<char*, wchar_t*>()) {
            // Cast away the extent, search for pointer                 
            return FindInner<THIS>(static_cast<const Deext<K>*>(match));
         }
         else return InvalidOffset;
      }
      else {
         if constexpr (CT::Typed<THIS>) {
            // Types known at compile-time                              
            static_assert(CT::Comparable<TypeOf<THIS>, K>,
               "Types not comparable");            
         }
         else if (not CT::Deep<K>
              and not me.IsDeep()
              and not me.template IsSimilar<K>())
            return InvalidOffset;

         // Get the starting index based on the key hash                
         const auto start = GetBucket(GetReserved() - 1, match);
         auto info = GetInfo() + start;
         if (not *info)
            return InvalidOffset;

         // Test first candidate                                        
         if (GetRef<THIS>(start) == match)
            return start;

         // Test all candidates on the right up until the end           
         ++info;

         auto infoEnd = GetInfoEnd();
         while (info != infoEnd) {
            if (not *info)
               return InvalidOffset;

            const auto i = info - GetInfo();
            if (GetRef<THIS>(i) == match)
               return i;

            ++info;
         }

         // Reached only if info has reached the end                    
         // Keys might loop around, continue the search from the start  
         info = GetInfo();
         if (not *info)
            return InvalidOffset;

         if (GetRef<THIS>(0) == match)
            return 0;

         ++info;

         infoEnd = GetInfo() + start;
         while (info != infoEnd) {
            if (not *info)
               return InvalidOffset;

            const auto i = info - GetInfo();
            if (GetRef<THIS>(i) == match)
               return i;

            ++info;
         }

         // No such key was found                                       
         return InvalidOffset;
      }
   }
   
   /// Find the index of a pair by an unknown type-erased key                 
   ///   @attention assumes map is not empty                                  
   ///   @attention assumes keys are of the exactly same type                 
   ///   @param match - the key to search for                                 
   ///   @return the index, or InvalidOffset if not found                     
   template<CT::Set THIS>
   Offset BlockSet::FindBlockInner(const Block<>& match) const {
      if (IsEmpty() or not IsSimilar<THIS>(match.GetType()))
         return InvalidOffset;

      // Get the starting index based on the key hash                   
      const auto start = GetBucketUnknown(GetReserved() - 1, match);
      auto info = GetInfo() + start;
      if (not *info)
         return InvalidOffset;

      // Test first candidate                                           
      if (GetRef<THIS>(start) == match)
         return start;

      // Test all candidates on the right up until the end              
      ++info;

      auto infoEnd = GetInfoEnd();
      while (info != infoEnd) {
         if (not *info)
            return InvalidOffset;

         const auto i = info - GetInfo();
         if (GetRef<THIS>(i) == match)
            return i;

         ++info;
      }

      // Reached only if info has reached the end                       
      // Keys might loop around, continue the search from the start     
      info = GetInfo();
      if (not *info)
         return InvalidOffset;

      if (GetRef<THIS>(0) == match)
         return 0;

      ++info;

      infoEnd = GetInfo() + start;
      while (info != infoEnd) {
         if (not *info)
            return InvalidOffset;

         const auto i = info - GetInfo();
         if (GetRef<THIS>(i) == match)
            return i;

         ++info;
      }
      
      // No such key was found                                          
      return InvalidOffset;
   }

} // namespace Langulus::Annies
