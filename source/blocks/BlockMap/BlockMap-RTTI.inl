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
   
   /// Checks type compatibility and sets type of type-erased maps            
   ///   @tparam K - the key type                                             
   ///   @tparam V - the value type                                           
   template<CT::Map THIS, CT::NoIntent K, CT::NoIntent V> LANGULUS(INLINED)
   void BlockMap::Mutate() {
      if constexpr (CT::Typed<THIS>) {
         static_assert(CT::MakableFrom<typename THIS::Key, K>,
            "Key is not constructible from the provided key block");
         static_assert(CT::MakableFrom<typename THIS::Value, V>,
            "Value is not constructible from the provided value block");

         // Set the type for type-erased map compatiblity               
         mKeys.mType   = MetaDataOf<typename THIS::Key>();
         mValues.mType = MetaDataOf<typename THIS::Value>();
      }
      else Mutate<THIS>(MetaDataOf<K>(), MetaDataOf<V>());
   }

   /// Checks type compatibility and sets type for the type-erased map        
   ///   @param key - the key type                                            
   ///   @param value - the value type                                        
   template<CT::Map THIS> LANGULUS(INLINED)
   void BlockMap::Mutate(DMeta key, DMeta value) {
      if constexpr (CT::Typed<THIS>) {
         // Set the type for type-erased map compatiblity               
         mKeys.mType = MetaDataOf<typename THIS::Key>();
         mValues.mType = MetaDataOf<typename THIS::Value>();

         LANGULUS_ASSERT(mKeys.mType->IsSimilar(key), Mutate,
            "Can't mutate to incompatible key");
         LANGULUS_ASSERT(mValues.mType->IsSimilar(value), Mutate,
            "Can't mutate to incompatible value");
      }
      else {
         // Type-erased maps are free to mutate, as long as types aren't
         // specified yet                                               
         if (not mKeys.mType) {
            // Set a fresh key type                                     
            mKeys.mType = key;
         }
         else {
            // Key type already set, so check compatibility             
            LANGULUS_ASSERT(mKeys.IsExact(key), Mutate,
               "Attempting to mutate type-erased unordered map's key type"
            );
         }

         if (not mValues.mType) {
            // Set a fresh value type                                   
            mValues.mType = value;
         }
         else {
            // Value type already set, so check compatibility           
            LANGULUS_ASSERT(mValues.IsExact(value), Mutate,
               "Attempting to mutate type-erased unordered map's value type"
            );
         }
      }
   }
   
   /// Check if this key type is similar to one of the listed types,          
   /// ignoring density and cv-qualifiers                                     
   ///   @tparam T1, TN... - the types to compare against                     
   ///   @return true if key type is similar to at least one of the types     
   template<CT::Map THIS, CT::Data T1, CT::Data...TN> LANGULUS(INLINED)
   constexpr bool BlockMap::IsKey() const noexcept {
      return GetKeys<THIS>().template Is<T1, TN...>();
   }

   /// Check if key type loosely matches a given type, ignoring               
   /// density and cv-qualifiers                                              
   ///   @param meta - the type to check for                                  
   ///   @return true if this map contains similar key data                   
   template<CT::Map THIS> LANGULUS(INLINED)
   bool BlockMap::IsKey(DMeta meta) const noexcept {
      return GetKeys<THIS>().Is(meta);
   }

   /// Check if key type is similar to one of the listed types,               
   /// ignoring cv-qualifiers only                                            
   ///   @tparam T1, TN... - the types to compare against                     
   ///   @return true if key type is similar to at least one of the types     
   template<CT::Map THIS, CT::Data T1, CT::Data...TN> LANGULUS(INLINED)
   constexpr bool BlockMap::IsKeySimilar() const noexcept {
      return GetKeys<THIS>().template IsSimilar<T1, TN...>();
   }

   /// Check if key type loosely matches a given type, ignoring               
   /// cv-qualifiers only                                                     
   ///   @param meta - the type to check for                                  
   ///   @return true if this map contains similar key data                   
   template<CT::Map THIS> LANGULUS(INLINED)
   bool BlockMap::IsKeySimilar(DMeta meta) const noexcept {
      return GetKeys<THIS>().IsSimilar(meta);
   }
   
   /// Check if key type is exactly as one of the listed types,               
   /// including by density and cv-qualifiers                                 
   ///   @tparam T1, TN... - the types to compare against                     
   ///   @return true if key type exactly matches at least one type           
   template<CT::Map THIS, CT::Data T1, CT::Data...TN> LANGULUS(INLINED)
   constexpr bool BlockMap::IsKeyExact() const noexcept {
      return GetKeys<THIS>().template IsExact<T1, TN...>();
   }
   
   /// Check if key type is exactly the provided type,                        
   /// including the density and cv-qualifiers                                
   ///   @param type - the type to match                                      
   ///   @return true if this map contains this exact key data                
   template<CT::Map THIS> LANGULUS(INLINED)
   bool BlockMap::IsKeyExact(DMeta meta) const noexcept {
      return GetKeys<THIS>().IsExact(meta);
   }
   
   /// Check if this value type is similar to one of the listed types,        
   /// ignoring density and cv-qualifiers                                     
   ///   @tparam T1, TN... - the types to compare against                     
   ///   @return true if value type is similar to at least one of the types   
   template<CT::Map THIS, CT::Data T1, CT::Data...TN> LANGULUS(INLINED)
   constexpr bool BlockMap::IsValue() const noexcept {
      return GetVals<THIS>().template Is<T1, TN...>();
   }
   
   /// Check if value type loosely matches a given type, ignoring             
   /// density and cv-qualifiers                                              
   ///   @param meta - the type to check for                                  
   ///   @return true if this map contains similar value data                 
   template<CT::Map THIS> LANGULUS(INLINED)
   bool BlockMap::IsValue(DMeta meta) const noexcept {
      return GetVals<THIS>().Is(meta);
   }

   /// Check if value type is similar to one of the listed types,             
   /// ignoring cv-qualifiers only                                            
   ///   @tparam T1, TN... - the types to compare against                     
   ///   @return true if value type is similar to at least one of the types   
   template<CT::Map THIS, CT::Data T1, CT::Data...TN> LANGULUS(INLINED)
   constexpr bool BlockMap::IsValueSimilar() const noexcept {
      return GetVals<THIS>().template IsSimilar<T1, TN...>();
   }

   /// Check if value type loosely matches a given type, ignoring             
   /// cv-qualifiers only                                                     
   ///   @param meta - the type to check for                                  
   ///   @return true if this map contains similar value data                 
   template<CT::Map THIS> LANGULUS(INLINED)
   bool BlockMap::IsValueSimilar(DMeta meta) const noexcept {
      return GetVals<THIS>().IsSimilar(meta);
   }
   
   /// Check if value type is exactly as one of the listed types,             
   /// including by density and cv-qualifiers                                 
   ///   @tparam T1, TN... - the types to compare against                     
   ///   @return true if value type exactly matches at least one type         
   template<CT::Map THIS, CT::Data T1, CT::Data...TN> LANGULUS(INLINED)
   constexpr bool BlockMap::IsValueExact() const noexcept {
      return GetVals<THIS>().template IsExact<T1, TN...>();
   }
   
   /// Check if value type is exactly the provided type,                      
   /// including the density and cv-qualifiers                                
   ///   @param type - the type to match                                      
   ///   @return true if this map contains this exact value data              
   template<CT::Map THIS> LANGULUS(INLINED)
   bool BlockMap::IsValueExact(DMeta meta) const noexcept {
      return GetVals<THIS>().IsExact(meta);
   }

   /// Check if types of a map is compatible with this map                    
   ///   @param other - map to test with                                      
   ///   @return true if both maps are type-compatible                        
   template<CT::Map THIS> LANGULUS(INLINED)
   constexpr bool BlockMap::IsTypeCompatibleWith(CT::Map auto const& other) const noexcept {
      using RHS = Deref<decltype(other)>;
      if constexpr (CT::Typed<THIS, RHS>) {
         // Static type check                                           
         return CT::Similar<typename THIS::Key,   typename RHS::Key>
            and CT::Similar<typename THIS::Value, typename RHS::Value>;
      }
      else {
         // Dynamic type check                                          
         return mKeys.IsSimilar(other.GetKeyType())
            and mValues.IsSimilar(other.GetValueType());
      }
   }

   /// Check if types of a pair are compatible with this map                  
   ///   @param other - map to test with                                      
   ///   @return true if both maps are type-compatible                        
   template<CT::Map THIS> LANGULUS(INLINED)
   constexpr bool BlockMap::IsTypeCompatibleWith(CT::Pair auto const& other) const noexcept {
      using RHS = Deref<decltype(other)>;
      if constexpr (CT::Typed<THIS, RHS>) {
         // Static type check                                           
         return CT::Similar<typename THIS::Key,   typename RHS::Key>
            and CT::Similar<typename THIS::Value, typename RHS::Value>;
      }
      else {
         // Dynamic type check                                          
         return mKeys.IsSimilar(other.GetKeyType())
            and mValues.IsSimilar(other.GetValueType());
      }
   }

} // namespace Langulus::Annies
