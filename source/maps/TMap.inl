///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "TMap.hpp"
#include "Map.inl"
#include "../pairs/TPair.inl"

#define TEMPLATE()   template<CT::Data K, CT::Data V, bool ORDERED>
#define TABLE()      TMap<K, V, ORDERED>


namespace Langulus::Annies
{

   /// Default construction                                                   
   TEMPLATE() LANGULUS(INLINED)
   constexpr TABLE()::TMap()
      : Map<ORDERED> {} {
      mKeys.mState = DataState::Typed;
      mValues.mState = DataState::Typed;

      if constexpr (CT::Constant<K>)
         mKeys.MakeConst();
      if constexpr (CT::Constant<V>)
         mValues.MakeConst();
   }

   /// Refer construction                                                     
   ///   @param other - the table to copy                                     
   TEMPLATE() LANGULUS(INLINED)
   TABLE()::TMap(const TMap& other)
      : TMap {Refer(other)} {}

   /// Move construction                                                      
   ///   @param other - the table to move                                     
   TEMPLATE() LANGULUS(INLINED)
   TABLE()::TMap(TMap&& other) noexcept
      : TMap {Move(other)} {}
   
   /// Create from a list of pairs, an array, as well as any other kinds of   
   /// maps. Each argument can be with or without intent.                     
   ///   @param t1 - first element and intent                                 
   ///   @param tn - tail of elements (optional, can have intents)            
   TEMPLATE() template<class T1, class...TN>
   requires CT::DeepMapMakable<K, V, T1, TN...> LANGULUS(INLINED)
   TABLE()::TMap(T1&& t1, TN&&...tn) {
      if constexpr (sizeof...(TN) == 0) {
         using S  = IntentOf<decltype(t1)>;
         using ST = TypeOf<S>;

         if constexpr (CT::Map<ST>) {
            if constexpr (CT::Typed<ST>) {
               // Not type-erased map, do compile-time type checks      
               using STT = TypeOf<ST>;
               if constexpr (CT::Similar<Pair, STT>) {
                  // Type is binary compatible, just transfer           
                  BlockMap::BlockTransfer<TMap>(S::Nest(t1));
               }
               else InsertPair(Forward<T1>(t1));
            }
            else {
               // Type-erased map, do run-time type checks              
               if (mKeys.mType   == DeintCast(t1).GetKeyType()
               and mValues.mType == DeintCast(t1).GetValueType()) {
                  // If types are exactly the same, it is safe to       
                  // absorb the map, essentially converting a type-     
                  // erased Map back to its TMap equivalent             
                  BlockMap::BlockTransfer<TMap>(S::Nest(t1));
               }
               else InsertPair(Forward<T1>(t1));
            }
         }
         else InsertPair(Forward<T1>(t1));
      }
      else InsertPair(Forward<T1>(t1), Forward<TN>(tn)...);
   }

   /// Destroys the map and all it's contents                                 
   TEMPLATE()
   TABLE()::~TMap() {
      BlockMap::Free<TMap>();
   }

   /// Move a table                                                           
   ///   @param pair - the table to move                                      
   ///   @return a reference to this table                                    
   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::operator = (TMap&& pair) -> TMap& {
      return operator = (Move(pair));
   }

   /// Refer assignment                                                       
   ///   @param pair - the table to reference                                 
   ///   @return a reference to this table                                    
   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::operator = (const TMap& pair) -> TMap& {
      return operator = (Refer(pair));
   }
   
   /// Generic assignment                                                     
   ///   @param rhs - the element/array/container to assign                   
   ///   @return a reference to this container                                
   TEMPLATE() template<class T1>
   requires CT::DeepMapAssignable<K, V, T1> LANGULUS(INLINED)
   auto TABLE()::operator = (T1&& rhs) -> TMap& {
      using S  = IntentOf<decltype(rhs)>;
      using ST = TypeOf<S>;
       
      if constexpr (CT::Map<ST>) {
         // Potentially absorb the container                            
         if (static_cast<const BlockMap*>(this)
          == static_cast<const BlockMap*>(&DeintCast(rhs)))
            return *this;

         BlockMap::Free<TMap>();
         new (this) TMap {S::Nest(rhs)};
      }
      else {
         // Unfold-insert                                               
         if (mKeys.mEntry) {
            if (mKeys.mEntry->GetUses() == 1) {
               if (not IsEmpty()) {
                  BlockMap::GetKeys<TMap>().FreeInner(mInfo);
                  BlockMap::GetVals<TMap>().FreeInner(mInfo);
                  mKeys.mCount = 0;
               }
            }
            else BlockMap::Free<TMap>();
         }

         BlockMap::UnfoldInsert<TMap>(S::Nest(rhs));
      }

      return *this;
   }

   /// Branch the map, by doing a shallow copy                                
   ///   @return a reference to this instance                                 
   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::BranchOut() -> TMap& {
      BlockMap::BranchOut<TMap>();
      return *this;
   }

   /// Get the key meta data                                                  
   /// Also implicitly initializes the internal key type                      
   ///   @attention this shouldn't be called on static initialization time    
   ///   @return the meta definition of the key type                          
   TEMPLATE() LANGULUS(INLINED)
   DMeta TABLE()::GetKeyType() const noexcept {
      return BlockMap::GetKeyType<TMap>();
   }

   /// Get the value meta data                                                
   /// Also implicitly initializes the internal key type                      
   ///   @attention this shouldn't be called on static initialization time    
   ///   @return the meta definition of the value type                        
   TEMPLATE() LANGULUS(INLINED)
   DMeta TABLE()::GetValueType() const noexcept {
      return BlockMap::GetValueType<TMap>();
   }

   /// Templated tables are always typed                                      
   ///   @return true                                                         
   TEMPLATE() LANGULUS(INLINED)
   constexpr bool TABLE()::IsKeyTyped() const noexcept {
      return true;
   }
   
   /// Templated tables are always typed                                      
   ///   @return true                                                         
   TEMPLATE() LANGULUS(INLINED)
   constexpr bool TABLE()::IsValueTyped() const noexcept {
      return true;
   }
   
   /// Templated tables are always typed                                      
   ///   @return false                                                        
   TEMPLATE() LANGULUS(INLINED)
   constexpr bool TABLE()::IsKeyUntyped() const noexcept {
      return false;
   }
   
   /// Templated tables are always typed                                      
   ///   @return false                                                        
   TEMPLATE() LANGULUS(INLINED)
   constexpr bool TABLE()::IsValueUntyped() const noexcept {
      return false;
   }
   
   /// Templated tables are always type-constrained                           
   ///   @return true                                                         
   TEMPLATE() LANGULUS(INLINED)
   constexpr bool TABLE()::IsKeyTypeConstrained() const noexcept {
      return true;
   }
   
   /// Templated tables are always type-constrained                           
   ///   @return true                                                         
   TEMPLATE() LANGULUS(INLINED)
   constexpr bool TABLE()::IsValueTypeConstrained() const noexcept {
      return true;
   }
   
   /// Check if key type is deep                                              
   TEMPLATE() LANGULUS(INLINED)
   constexpr bool TABLE()::IsKeyDeep() const noexcept {
      return BlockMap::IsKeyDeep<TMap>();
   }
   
   /// Check if value type is deep                                            
   TEMPLATE() LANGULUS(INLINED)
   constexpr bool TABLE()::IsValueDeep() const noexcept {
      return BlockMap::IsValueDeep<TMap>();
   }

   /// Check if the key type is a pointer                                     
   TEMPLATE() LANGULUS(INLINED)
   constexpr bool TABLE()::IsKeySparse() const noexcept {
      return CT::Sparse<K>;
   }
   
   /// Check if the value type is a pointer                                   
   TEMPLATE() LANGULUS(INLINED)
   constexpr bool TABLE()::IsValueSparse() const noexcept {
      return CT::Sparse<V>;
   }

   /// Check if the key type is not a pointer                                 
   TEMPLATE() LANGULUS(INLINED)
   constexpr bool TABLE()::IsKeyDense() const noexcept {
      return CT::Dense<K>;
   }

   /// Check if the value type is not a pointer                               
   TEMPLATE() LANGULUS(INLINED)
   constexpr bool TABLE()::IsValueDense() const noexcept {
      return CT::Dense<V>;
   }

   /// Get the size of a single key, in bytes                                 
   ///   @return the number of bytes a single key contains                    
   TEMPLATE() LANGULUS(INLINED)
   constexpr Size TABLE()::GetKeyStride() const noexcept {
      return sizeof(K); 
   }
   
   /// Get the size of a single value, in bytes                               
   ///   @return the number of bytes a single value contains                  
   TEMPLATE() LANGULUS(INLINED)
   constexpr Size TABLE()::GetValueStride() const noexcept {
      return sizeof(V); 
   }

   /// Get the number of deep key containers                                  
   ///   @return the number of deep key containers                            
   TEMPLATE() LANGULUS(INLINED)
   Count TABLE()::GetKeyCountDeep() const noexcept {
      return BlockMap::GetKeyCountDeep<TMap>();
   }

   /// Get the number of deep key containers                                  
   ///   @return the number of deep key containers                            
   TEMPLATE() LANGULUS(INLINED)
   Count TABLE()::GetKeyCountElementsDeep() const noexcept {
      return BlockMap::GetKeyCountElementsDeep<TMap>();
   }

   /// Get the number of deep value containers                                
   ///   @return the number of deep value containers                          
   TEMPLATE() LANGULUS(INLINED)
   Count TABLE()::GetValueCountDeep() const noexcept {
      return BlockMap::GetValueCountDeep<TMap>();
   }

   /// Get the number of deep value containers                                
   ///   @return the number of deep value containers                          
   TEMPLATE() LANGULUS(INLINED)
   Count TABLE()::GetValueCountElementsDeep() const noexcept {
      return BlockMap::GetValueCountElementsDeep<TMap>();
   }

   /// Check if the map contains at least one missing entry (nested)          
   ///   @return true if the map has missing entries                          
   TEMPLATE() LANGULUS(INLINED)
   bool TABLE()::IsKeyMissingDeep() const {
      return BlockMap::IsKeyMissingDeep<TMap>();
   }
   
   /// Check if the map contains at least one missing entry (nested)          
   ///   @return true if the map has missing entries                          
   TEMPLATE() LANGULUS(INLINED)
   bool TABLE()::IsValueMissingDeep() const {
      return BlockMap::IsValueMissingDeep<TMap>();
   }
   
   /// Check if the map contains at least one executable key (nested)         
   ///   @return true if the map has executable entries                       
   TEMPLATE() LANGULUS(INLINED)
   bool TABLE()::IsKeyExecutableDeep() const {
      return BlockMap::IsKeyExecutableDeep<TMap>();
   }
   
   /// Check if the map contains at least one executable value (nested)       
   ///   @return true if the map has executable entries                       
   TEMPLATE() LANGULUS(INLINED)
   bool TABLE()::IsValueExecutableDeep() const {
      return BlockMap::IsValueExecutableDeep<TMap>();
   }

   /// Check if the map contains executable keys                              
   ///   @return true if the map has executable key type                      
   TEMPLATE() LANGULUS(INLINED)
   bool TABLE()::IsKeyExecutable() const {
      return BlockMap::IsKeyExecutable<TMap>();
   }
   
   /// Check if the map contains executable values                            
   ///   @return true if the map has executable value type                    
   TEMPLATE() LANGULUS(INLINED)
   bool TABLE()::IsValueExecutable() const {
      return BlockMap::IsValueExecutable<TMap>();
   }

   /// Check if key origin type matches any of the list                       
   ///   @tparam K1, KN... - the list of types to compare against             
   ///   @return true if key type matches at least one of the others          
   TEMPLATE() template<CT::Data K1, CT::Data...KN> LANGULUS(INLINED)
   constexpr bool TABLE()::IsKey() const noexcept {
      return BlockMap::IsKey<TMap, K1, KN...>();
   }

   /// Check if key origin type matches another                               
   ///   @param key - the key type to compare against                         
   ///   @return true if key matches the contained key origin type            
   TEMPLATE() LANGULUS(INLINED)
   bool TABLE()::IsKey(DMeta key) const noexcept {
      return BlockMap::IsKey<TMap>(key);
   }

   /// Check if cv-unqualified key type matches any of the list               
   ///   @tparam K1, KN... - the list of types to compare against             
   ///   @return true if key type matches at least one of the others          
   TEMPLATE() template<CT::Data K1, CT::Data...KN> LANGULUS(INLINED)
   constexpr bool TABLE()::IsKeySimilar() const noexcept {
      return BlockMap::IsKeySimilar<TMap, K1, KN...>();
   }

   /// Check if cv-unqualified key type matches another                       
   ///   @param key - the key type to compare against                         
   ///   @return true if key matches the contained key unqualified type       
   TEMPLATE() LANGULUS(INLINED)
   bool TABLE()::IsKeySimilar(DMeta key) const noexcept {
      return BlockMap::IsKeySimilar<TMap>(key);
   }

   /// Check if key type exactly matches any of the list                      
   ///   @tparam K1, KN... - the list of types to compare against             
   ///   @return true if key type matches at least one of the others          
   TEMPLATE() template<CT::Data K1, CT::Data...KN> LANGULUS(INLINED)
   constexpr bool TABLE()::IsKeyExact() const noexcept {
      return BlockMap::IsKeyExact<TMap, K1, KN...>();
   }

   /// Check if key type exactly matches any of the list                      
   ///   @param key - the key type to compare against                         
   ///   @return true if key matches the contained key unqualified type       
   TEMPLATE() LANGULUS(INLINED)
   bool TABLE()::IsKeyExact(DMeta key) const noexcept {
      return BlockMap::IsKeyExact<TMap>(key);
   }

   /// Check if value origin type matches any of the list                     
   ///   @tparam V1, VN... - the list of types to compare against             
   ///   @return true if value type matches at least one of the others        
   TEMPLATE() template<CT::Data V1, CT::Data...VN> LANGULUS(INLINED)
   constexpr bool TABLE()::IsValue() const noexcept {
      return BlockMap::IsValue<TMap, V1, VN...>();
   }

   /// Check if value origin type matches another                             
   ///   @param value - the value type to compare against                     
   ///   @return true if value matches the contained key origin type          
   TEMPLATE() LANGULUS(INLINED)
   bool TABLE()::IsValue(DMeta value) const noexcept {
      return BlockMap::IsValue<TMap>(value);
   }

   /// Check if cv-unqualified value type matches any of the list             
   ///   @tparam V1, VN... - the list of types to compare against             
   ///   @return true if value type matches at least one of the others        
   TEMPLATE() template<CT::Data V1, CT::Data...VN> LANGULUS(INLINED)
   constexpr bool TABLE()::IsValueSimilar() const noexcept {
      return BlockMap::IsValueSimilar<TMap, V1, VN...>();
   }

   /// Check if cv-unqualified value type matches another                     
   ///   @param value - the value type to compare against                     
   ///   @return true if value matches the contained value unqualified type   
   TEMPLATE() LANGULUS(INLINED)
   bool TABLE()::IsValueSimilar(DMeta value) const noexcept {
      return BlockMap::IsValueSimilar<TMap>(value);
   }

   /// Check if value type exactly matches any of the list                    
   ///   @tparam V1, VN... - the list of types to compare against             
   ///   @return true if value type matches at least one of the others        
   TEMPLATE() template<CT::Data V1, CT::Data...VN> LANGULUS(INLINED)
   constexpr bool TABLE()::IsValueExact() const noexcept {
      return BlockMap::IsValueExact<TMap, V1, VN...>();
   }

   /// Check if value type exactly matches any of the list                    
   ///   @param value - the value type to compare against                     
   ///   @return true if value matches the contained value unqualified type   
   TEMPLATE() LANGULUS(INLINED)
   bool TABLE()::IsValueExact(DMeta value) const noexcept {
      return BlockMap::IsValueExact<TMap>(value);
   }

   /// Checks type compatibility and sets type for the type-erased map        
   ///   @param key - the key type                                            
   ///   @param value - the value type                                        
   TEMPLATE() template<CT::NoIntent K1, CT::NoIntent V1>
   LANGULUS(INLINED) void TABLE()::Mutate() noexcept {
      return BlockMap::Mutate<TMap, K1, V1>();
   }

   /// Checks type compatibility and sets type for the type-erased map        
   ///   @param key - the key type                                            
   ///   @param value - the value type                                        
   TEMPLATE() LANGULUS(INLINED)
   void TABLE()::Mutate(DMeta key, DMeta value) {
      return BlockMap::Mutate<TMap>(key, value);
   }

   /// Reserve a new table size                                               
   ///   @param count - the number of elements to reserve                     
   TEMPLATE() LANGULUS(INLINED)
   void TABLE()::Reserve(Count count) {
      return BlockMap::Reserve<TMap>(count);
   }

   /// Insert pair with separate key and value, with or without intents       
   ///   @param key - the key and intent to insert                            
   ///   @param val - the value and intent to insert                          
   ///   @return 1 if pair was inserted, zero otherwise                       
   TEMPLATE() template<class K1, class V1>
   requires (CT::MakableFrom<K, K1> and CT::MakableFrom<V, V1>) LANGULUS(INLINED)
   Count TABLE()::Insert(K1&& key, V1&& val) {
      return BlockMap::Insert<TMap>(Forward<K1>(key), Forward<V1>(val));
   }
   
   TEMPLATE() template<class K1>
   requires (CT::MakableFrom<K, K1> and CT::Defaultable<V>) LANGULUS(INLINED)
   Count TABLE()::Insert(K1&& key) {
      return BlockMap::Insert<TMap>(Forward<K1>(key), V {});
   }
   
   /// Insert pair(s) by manually providing key and value blocks              
   ///   @attention only the overlapping elements will be inserted            
   ///   @param key - the key block to insert                                 
   ///   @param val - the value block to insert                               
   ///   @return the number of inserted pairs                                 
   TEMPLATE() template<class K1, class V1> LANGULUS(INLINED)
   Count TABLE()::InsertBlock(K1&& key, V1&& val) {
      return BlockMap::InsertBlock<TMap>(Forward<K1>(key), Forward<V1>(val));
   }

   /// Unfold-insert pairs, with or without intents                           
   ///   @param t1, tn... - pairs, or arrays of pairs, to insert              
   ///   @return the number of inserted pairs                                 
   TEMPLATE() template<class T1, class...TN>
   requires CT::UnfoldMakableFrom<TPair<K, V>, T1, TN...> LANGULUS(INLINED)
   Count TABLE()::InsertPair(T1&& t1, TN&&...tn) {
      Count inserted = 0;
        inserted += BlockMap::UnfoldInsert<TMap>(Forward<T1>(t1));
      ((inserted += BlockMap::UnfoldInsert<TMap>(Forward<TN>(tn))), ...);
      return inserted;
   }

   /// Insert pair                                                            
   ///   @param pair - the pair to insert                                     
   ///   @return a reference to this map for chaining                         
   TEMPLATE() template<class T1>
   requires CT::UnfoldMakableFrom<TPair<K, V>, T1> LANGULUS(INLINED)
   auto TABLE()::operator << (T1&& pair) -> TMap& {
      InsertPair(Forward<T1>(pair));
      return *this;
   }

   /// Insert pair                                                            
   ///   @param pair - the pair to insert                                     
   ///   @return a reference to this map for chaining                         
   TEMPLATE() template<class T1>
   requires CT::UnfoldMakableFrom<TPair<K, V>, T1> LANGULUS(INLINED)
   auto TABLE()::operator >> (T1&& pair) -> TMap& {
      InsertPair(Forward<T1>(pair));
      return *this;
   }

   /// Combine the contents of two maps (destructively)                       
   ///   @param rhs - the map to concatenate                                  
   ///   @return a reference to this table for chaining                       
   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::operator += (const TABLE()& rhs) -> TMap& {
      for (auto pair : rhs) {
         auto found = Find(pair.GetKey());
         if (found) {
            if constexpr (requires (V& lhs) { lhs += rhs; })
               GetValue(found) += pair.GetValue();
            else
               LANGULUS_THROW(Concat, "No concat operator available");
         }
         else Insert(pair.GetKey(), pair.GetValue()); //TODO use handles instead? or better yed - directly insert pair?
      }

      return *this;
   }

   /// Request a new size of value container                                  
   ///   @attention assumes value type has been set                           
   ///   @param count - number of values to allocate                          
   ///   @return the requested byte size                                      
   TEMPLATE() LANGULUS(INLINED)
   Size TABLE()::RequestValuesSize(const Count count) noexcept {
      Offset valueByteSize = count * sizeof(V);
      if constexpr (CT::Sparse<V>)
         valueByteSize *= 2;
      return valueByteSize;
   }

   /// Erase a pair via key                                                   
   ///   @param key - the key to search for                                   
   ///   @return 1 if key was found and pair was removed                      
   TEMPLATE() template<CT::NoIntent K1>
   requires CT::Comparable<K, K1> LANGULUS(INLINED)
   Count TABLE()::RemoveKey(const K1& key) {
      return BlockMap::RemoveKey<TMap>(key);
   }

   /// Erase all pairs with a given value                                     
   ///   @param value - the match to search for                               
   ///   @return the number of removed pairs                                  
   TEMPLATE() template<CT::NoIntent V1>
   requires CT::Comparable<V, V1> LANGULUS(INLINED)
   Count TABLE()::RemoveValue(const V1& value) {
      return BlockMap::RemoveValue<TMap>(value);
   }
     
   /// Erase all pairs matching a pair                                        
   ///   @param value - the match to search for                               
   ///   @return the number of removed pairs                                  
   TEMPLATE() template<CT::Pair P>
   requires CT::Comparable<TPair<K, V>, P> LANGULUS(INLINED)
   Count TABLE()::RemovePair(const P& pair) {
      return BlockMap::RemovePair<TMap>(pair);
   }
     
   /// Safely erases element at a specific iterator                           
   ///   @attention assumes iterator is produced by this map instance         
   ///   @attention assumes that iterator points to a valid entry             
   ///   @param index - the index to remove                                   
   ///   @return the iterator of the previous element, unless index is the    
   ///           first, or at the end already                                 
   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::RemoveIt(const Iterator& index) -> Iterator {
      return BlockMap::RemoveIt<TMap>(index);
   }

   /// Destroy all contained pairs, but don't deallocate                      
   TEMPLATE() LANGULUS(INLINED)
   void TABLE()::Clear() {
      return BlockMap::Clear<TMap>();
   }

   /// Destroy all contained pairs and deallocate                             
   TEMPLATE() LANGULUS(INLINED)
   void TABLE()::Reset() {
      return BlockMap::Reset<TMap>();
   }

   /// Reduce reserved size, depending on number of contained elements        
   TEMPLATE() LANGULUS(INLINED)
   void TABLE()::Compact() {
      return BlockMap::Compact<TMap>();
   }
   
   /// Compare this map against another map, type-erased or not               
   ///   @param rhs - map to compare against                                  
   ///   @return true if contents of both maps are the same                   
   TEMPLATE() LANGULUS(INLINED)
   bool TABLE()::operator == (CT::Map auto const& rhs) const
   requires CT::Comparable<V, V> {
      return BlockMap::operator == <TMap> (rhs);
   }

   /// Compare this map against a pair, type-erased or not                    
   ///   @param rhs - pair to compare against                                 
   ///   @return true this map contains only this exact pair                  
   TEMPLATE() LANGULUS(INLINED)
   bool TABLE()::operator == (CT::Pair auto const& rhs) const
   requires CT::Comparable<V, V> {
      return BlockMap::operator == <TMap> (rhs);
   }

   /// Hash the contents of map                                               
   ///   @attention hashing is slow, it is recommended to cache the value     
   ///   @return the cache                                                    
   TEMPLATE() LANGULUS(INLINED)
   Hash TABLE()::GetHash() const requires CT::Hashable<K, V> {
      return BlockMap::GetHash<TMap>();
   }

   /// Search for a key inside the table                                      
   ///   @param key - the key to search for                                   
   ///   @return true if key is found, false otherwise                        
   TEMPLATE() template<CT::NoIntent K1>
   requires CT::Comparable<K, K1> LANGULUS(INLINED)
   bool TABLE()::ContainsKey(K1 const& key) const {
      return BlockMap::ContainsKey<TMap>(key);
   }

   /// Search for a value inside the table                                    
   ///   @param val - the value to search for                                 
   ///   @return true if value is found, false otherwise                      
   TEMPLATE() template<CT::NoIntent V1>
   requires CT::Comparable<V, V1> LANGULUS(INLINED)
   bool TABLE()::ContainsValue(V1 const& val) const {
      return BlockMap::ContainsValue<TMap>(val);
   }

   /// Search for a pair inside the table                                     
   ///   @param pair - the pair to search for                                 
   ///   @return true if pair is found, false otherwise                       
   TEMPLATE() template<CT::Pair P>
   requires CT::Comparable<TPair<K, V>, P> LANGULUS(INLINED)
   bool TABLE()::ContainsPair(P const& pair) const {
      return BlockMap::ContainsPair<TMap>(pair);
   }

   /// Search for a key inside the table, and return it if found              
   ///   @param key - the key to search for                                   
   ///   @return the index if key was found, or IndexNone if not              
   TEMPLATE() template<CT::NoIntent K1>
   requires CT::Comparable<K, K1> LANGULUS(INLINED)
   Index TABLE()::Find(K1 const& key) const {
      return BlockMap::Find<TMap>(key);
   }
   
   /// Search for a key inside the table, and return an iterator to it        
   ///   @param key - the key to search for                                   
   ///   @return the iterator                                                 
   TEMPLATE() template<CT::NoIntent K1>
   requires CT::Comparable<K, K1> LANGULUS(INLINED)
   auto TABLE()::FindIt(K1 const& key) -> Iterator {
      return BlockMap::FindIt<TMap>(key);
   }

   TEMPLATE() template<CT::NoIntent K1>
   requires CT::Comparable<K, K1> LANGULUS(INLINED)
   auto TABLE()::FindIt(K1 const& key) const -> ConstIterator {
      return BlockMap::FindIt<TMap>(key);
   }
   
   /// Returns a reference to the value found for key                         
   /// Throws Except::OutOfRange if element cannot be found                   
   ///   @param key - the key to search for                                   
   ///   @return a reference to the value                                     
   TEMPLATE() template<CT::NoIntent K1>
   requires CT::Comparable<K, K1> LANGULUS(INLINED)
   decltype(auto) TABLE()::At(K1 const& key) {
      return BlockMap::At<TMap>(key);
   }

   TEMPLATE() template<CT::NoIntent K1>
   requires CT::Comparable<K, K1> LANGULUS(INLINED)
   decltype(auto) TABLE()::At(K1 const& key) const {
      return BlockMap::At<TMap>(key);
   }

   /// Access value by key                                                    
   ///   @param key - the key to find                                         
   ///   @return a reference to the value                                     
   TEMPLATE() template<CT::NoIntent K1>
   requires CT::Comparable<K, K1> LANGULUS(INLINED)
   decltype(auto) TABLE()::operator[] (K1 const& key) {
      return BlockMap::operator [] <TMap>(key);
   }

   TEMPLATE() template<CT::NoIntent K1>
   requires CT::Comparable<K, K1> LANGULUS(INLINED)
   decltype(auto) TABLE()::operator[] (K1 const& key) const {
      return BlockMap::operator [] <TMap>(key);
   }

   /// Get a key at an index                                                  
   ///   @attention will throw OutOfRange if there's no pair at the index     
   ///   @param index - the index                                             
   ///   @return the mutable key reference                                    
   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::GetKey(CT::Index auto index) -> K& {
      return BlockMap::GetKey<TMap>(index);
   }

   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::GetKey(CT::Index auto index) const -> const K& {
      return BlockMap::GetKey<TMap>(index);
   }

   /// Get a value at an index                                                
   ///   @attention will throw OutOfRange if there's no pair at the index     
   ///   @param i - the index                                                 
   ///   @return the mutable value reference                                  
   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::GetValue(CT::Index auto index) -> V& {
      return BlockMap::GetValue<TMap>(index);
   }

   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::GetValue(CT::Index auto index) const -> const V& {
      return BlockMap::GetValue<TMap>(index);
   }

   /// Get a pair at an index                                                 
   ///   @attention will throw OutOfRange if there's no pair at the index     
   ///   @param index - the index                                             
   ///   @return the mutable pair reference                                   
   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::GetPair(CT::Index auto index) -> PairRef {
      return BlockMap::GetPair<TMap>(index);
   }

   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::GetPair(CT::Index auto index) const -> PairConstRef {
      return BlockMap::GetPair<TMap>(index);
   }

   /// Get iterator to first element                                          
   ///   @return an iterator to the first element, or end if empty            
   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::begin() noexcept -> Iterator {
      return BlockMap::begin<TMap>();
   }

   TEMPLATE()
   auto TABLE()::begin() const noexcept -> ConstIterator {
      return BlockMap::begin<TMap>();
   }

   /// Get iterator to the last element                                       
   ///   @return an iterator to the last element, or end if empty             
   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::last() noexcept -> Iterator {
      return BlockMap::last<TMap>();
   }

   TEMPLATE()
   auto TABLE()::last() const noexcept -> ConstIterator {
      return BlockMap::last<TMap>();
   }

   /// Iterate all pairs inside the map, and perform call() on them           
   /// You can break the loop, by returning false inside call()               
   ///   @tparam REVERSE - whether to iterate in reverse                      
   ///   @param call - the function to call for each pair                     
   ///   @return the number of successful call() executions                   
   TEMPLATE() template<bool REVERSE> LANGULUS(INLINED)
   Count TABLE()::ForEach(auto&& call) const {
      using F = Deref<decltype(call)>;
      return BlockMap::ForEach<REVERSE, const TMap>(Forward<F>(call));
   }

   TEMPLATE() template<bool REVERSE> LANGULUS(INLINED)
   Count TABLE()::ForEach(auto&& call) {
      using F = Deref<decltype(call)>;
      return BlockMap::ForEach<REVERSE, TMap>(Forward<F>(call));
   }
   
   /// Iterate all keys as type-erased blocks inside the map, and perform     
   /// call() with each of them                                               
   /// You can break the loop, by returning false inside call()               
   ///   @tparam REVERSE - whether to iterate in reverse                      
   ///   @param call - the function to call for each key block                
   ///   @return the number of successful call() executions                   
   TEMPLATE() template<bool REVERSE> LANGULUS(INLINED)
   Count TABLE()::ForEachKeyElement(auto&& call) const {
      using F = Deref<decltype(call)>;
      return BlockMap::ForEachKeyElement<REVERSE, const TMap>(Forward<F>(call));
   }

   TEMPLATE() template<bool REVERSE> LANGULUS(INLINED)
   Count TABLE()::ForEachKeyElement(auto&& call) {
      using F = Deref<decltype(call)>;
      return BlockMap::ForEachKeyElement<REVERSE, TMap>(Forward<F>(call));
   }
   
   /// Iterate all values as type-erased blocks inside the map, and perform   
   /// call() with each of them                                               
   /// You can break the loop, by returning false inside call()               
   ///   @param call - the function to call for each value block              
   ///   @return the number of successful call() executions                   
   TEMPLATE() template<bool REVERSE> LANGULUS(INLINED)
   Count TABLE()::ForEachValueElement(auto&& call) const {
      using F = Deref<decltype(call)>;
      return BlockMap::ForEachValueElement<REVERSE, const TMap>(Forward<F>(call));
   }

   TEMPLATE() template<bool REVERSE> LANGULUS(INLINED)
   Count TABLE()::ForEachValueElement(auto&& call) {
      using F = Deref<decltype(call)>;
      return BlockMap::ForEachValueElement<REVERSE, TMap>(Forward<F>(call));
   }

   /// Try different call() functions with different argument types on all    
   /// keys contained in the map - execute for those types that match         
   /// You can break the loop, by returning false inside call()               
   ///   @tparam REVERSE - whether to iterate in reverse                      
   ///   @param calls - the functions to attempt                              
   ///   @return the number of successful call() executions                   
   TEMPLATE() template<bool REVERSE, class...F> LANGULUS(INLINED)
   Count TABLE()::ForEachKey(F&&...calls) const {
      static_assert(sizeof...(F) > 0, "No iterators in ForEachKey");
      return BlockMap::ForEachKey<REVERSE, const TMap>(Forward<F>(calls)...);
   }

   TEMPLATE() template<bool REVERSE, class...F> LANGULUS(INLINED)
   Count TABLE()::ForEachKey(F&&...calls) {
      static_assert(sizeof...(F) > 0, "No iterators in ForEachKey");
      return BlockMap::ForEachKey<REVERSE, TMap>(Forward<F>(calls)...);
   }

   /// Try different call() functions with different argument types on all    
   /// values contained in the map - execute for those types that match       
   /// You can break the loop, by returning false inside call()               
   ///   @tparam REVERSE - whether to iterate in reverse                      
   ///   @param calls - the functions to attempt                              
   ///   @return the number of successful call() executions                   
   TEMPLATE() template<bool REVERSE, class...F> LANGULUS(INLINED)
   Count TABLE()::ForEachValue(F&&...calls) const {
      static_assert(sizeof...(F) > 0, "No iterators in ForEachValue");
      return BlockMap::ForEachValue<REVERSE, const TMap>(Forward<F>(calls)...);
   }

   TEMPLATE() template<bool REVERSE, class...F> LANGULUS(INLINED)
   Count TABLE()::ForEachValue(F&&...calls) {
      static_assert(sizeof...(F) > 0, "No iterators in ForEachValue");
      return BlockMap::ForEachValue<REVERSE, TMap>(Forward<F>(calls)...);
   }

   /// Try different call() functions with different argument types on all    
   /// keys and subkeys contained in the map - execute for those types that   
   /// match. You can break the loop, by returning false inside call()        
   ///   @tparam REVERSE - whether to iterate in reverse                      
   ///   @tparam SKIP - whether to execute for intermediate containers too    
   ///   @param calls - the functions to attempt                              
   ///   @return the number of successful call() executions                   
   TEMPLATE() template<bool REVERSE, bool SKIP, class...F> LANGULUS(INLINED)
   Count TABLE()::ForEachKeyDeep(F&&...calls) const {
      static_assert(sizeof...(F) > 0, "No iterators in ForEachKeyDeep");
      return BlockMap::ForEachKeyDeep<REVERSE, SKIP, const TMap>(Forward<F>(calls)...);
   }

   TEMPLATE() template<bool REVERSE, bool SKIP, class...F> LANGULUS(INLINED)
   Count TABLE()::ForEachKeyDeep(F&&...calls) {
      static_assert(sizeof...(F) > 0, "No iterators in ForEachKeyDeep");
      return BlockMap::ForEachKeyDeep<REVERSE, SKIP, TMap>(Forward<F>(calls)...);
   }

   /// Try different call() functions with different argument types on all    
   /// values and subvalues contained in the map - execute for those types    
   /// that match. You can break the loop, by returning false inside call()   
   ///   @tparam REVERSE - whether to iterate in reverse                      
   ///   @tparam SKIP - whether to execute for intermediate containers too    
   ///   @param calls - the functions to attempt                              
   ///   @return the number of successful call() executions                   
   TEMPLATE() template<bool REVERSE, bool SKIP, class...F> LANGULUS(INLINED)
   Count TABLE()::ForEachValueDeep(F&&...calls) const {
      static_assert(sizeof...(F) > 0, "No iterators in ForEachValueDeep");
      return BlockMap::ForEachValueDeep<REVERSE, SKIP, const TMap>(Forward<F>(calls)...);
   }

   TEMPLATE() template<bool REVERSE, bool SKIP, class...F> LANGULUS(INLINED)
   Count TABLE()::ForEachValueDeep(F&&...calls) {
      static_assert(sizeof...(F) > 0, "No iterators in ForEachValueDeep");
      return BlockMap::ForEachValueDeep<REVERSE, SKIP, TMap>(Forward<F>(calls)...);
   }

} // namespace Langulus::Annies

#undef TEMPLATE
#undef TABLE
