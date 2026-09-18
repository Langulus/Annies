///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Map.hpp"
#include "../pairs/Pair.inl"
#include "../blocks/BlockMap/BlockMap-Capsulation.inl"
#include "../blocks/BlockMap/BlockMap-Iteration.inl"
#include "../blocks/BlockMap/BlockMap-Compare.inl"
#include "../blocks/BlockMap/BlockMap-Construct.inl"
#include "../blocks/BlockMap/BlockMap-RTTI.inl"
#include "../blocks/BlockMap/BlockMap-Memory.inl"
#include "../blocks/BlockMap/BlockMap-Remove.inl"
#include "../blocks/BlockMap/BlockMap-Insert.inl"
#include "../blocks/BlockMap/BlockMap-Indexing.inl"

#define TEMPLATE()   template<bool ORDERED>
#define TABLE()      Map<ORDERED>


namespace Langulus::Annies
{

   /// Refer constructor                                                      
   ///   @param other - the container to refer to                             
   TEMPLATE() LANGULUS(INLINED)
   TABLE()::Map(const Map& other)
      : Map {Refer(other)} {}

   /// Move constructor                                                       
   ///   @param other - the container to move                                 
   TEMPLATE() LANGULUS(INLINED)
   TABLE()::Map(Map&& other)
      : Map {Move(other)} {}

   /// Unfold constructor. If there's one map argument, it will be absorbed   
   /// Otherwise, elements are expected to initialize a pair each             
   ///   @param t1 - first pair, with or without intent                       
   ///   @param tn... - the rest of the pairs (optional, can have intents)    
   TEMPLATE() template<class T1, class...TN>
   requires CT::UnfoldInsertable<T1, TN...> LANGULUS(INLINED)
   TABLE()::Map(T1&& t1, TN&&...tn) {
      if constexpr (sizeof...(TN) == 0) {
         using S = IntentOf<decltype(t1)>;
         using T = TypeOf<S>;

         if constexpr (CT::Map<T>)
            BlockMap::BlockTransfer<Map>(S::Nest(t1));
         else
            BlockMap::InsertPair<Map>(Forward<T1>(t1));
      }
      else BlockMap::InsertPair<Map>(Forward<T1>(t1), Forward<TN>(tn)...);
   }

   /// Map destructor                                                         
   TEMPLATE() LANGULUS(INLINED)
   TABLE()::~Map() {
      BlockMap::Free<Map>();
   }

   /// Refer assignment                                                       
   ///   @param rhs - map to refer to                                         
   ///   @return a reference to this map                                      
   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::operator = (const Map& rhs) -> Map& {
      return operator = (Refer(rhs));
   }

   /// Move assignment                                                        
   ///   @param rhs - unordered map to move over                              
   ///   @return a reference to this map                                      
   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::operator = (Map&& rhs) -> Map& {
      return operator = (Move(rhs));
   }

   /// Pair/map assignment, with or without intent                            
   ///   @param rhs - the pair, or map to assign                              
   ///   @return a reference to this container                                
   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::operator = (CT::UnfoldInsertable auto&& rhs) -> Map& {
      using S = IntentOf<decltype(rhs)>;
      using T = TypeOf<S>;

      if constexpr (CT::Map<T>) {
         // Potentially absorb a container                              
         if (static_cast<const BlockMap*>(this)
          == static_cast<const BlockMap*>(&DeintCast(rhs)))
            return *this;

         BlockMap::Free<Map>();
         new (this) Map {S::Nest(rhs)};
      }
      else {
         // Unfold-insert                                               
         BlockMap::Clear<Map>();
         BlockMap::UnfoldInsert<Map>(S::Nest(rhs));
      }

      return *this;
   }
   
   /// Get a key at an index                                                  
   ///   @attention will throw OutOfRange if there's no pair at the index     
   ///   @param index - the index                                             
   ///   @return the mutable key reference                                    
   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::GetKey(CT::Index auto index) -> Block<> {
      return BlockMap::GetKey<Map>(index);
   }

   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::GetKey(CT::Index auto index) const -> Block<> {
      return BlockMap::GetKey<Map>(index);
   }

   /// Get a value at an index                                                
   ///   @attention will throw OutOfRange if there's no pair at the index     
   ///   @param i - the index                                                 
   ///   @return the mutable value reference                                  
   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::GetValue(CT::Index auto index) -> Block<> {
      return BlockMap::GetValue<Map>(index);
   }

   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::GetValue(CT::Index auto index) const -> Block<> {
      return BlockMap::GetValue<Map>(index);
   }

   /// Get a pair at an index                                                 
   ///   @attention will throw OutOfRange if there's no pair at the index     
   ///   @param index - the index                                             
   ///   @return the mutable pair reference                                   
   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::GetPair(CT::Index auto index) -> Pair {
      return BlockMap::GetPair<Map>(index);
   }

   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::GetPair(CT::Index auto index) const -> Pair {
      return BlockMap::GetPair<Map>(index);
   }

   /// Get iterator to first element                                          
   ///   @return an iterator to the first element, or end if empty            
   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::begin() noexcept -> Iterator {
      return BlockMap::begin<Map>();
   }

   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::begin() const noexcept -> ConstIterator {
      return BlockMap::begin<Map>();
   }

   /// Get iterator to the last element                                       
   ///   @return an iterator to the last element, or end if empty             
   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::last() noexcept -> Iterator {
      return BlockMap::last<Map>();
   }

   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::last() const noexcept -> ConstIterator {
      return BlockMap::last<Map>();
   }

   TEMPLATE() template<bool REVERSE> LANGULUS(INLINED)
   size_t TABLE()::ForEach(auto&& call) const {
      return BlockMap::ForEach<REVERSE, const Map>(
         Forward<Deref<decltype(call)>>(call));
   }

   TEMPLATE() template<bool REVERSE> LANGULUS(INLINED)
   size_t TABLE()::ForEach(auto&& call) {
      return BlockMap::ForEach<REVERSE, Map>(
         Forward<Deref<decltype(call)>>(call));
   }

   TEMPLATE() template<bool REVERSE> LANGULUS(INLINED)
   size_t TABLE()::ForEachKeyElement(auto&& call) const {
      return BlockMap::ForEachKeyElement<REVERSE, const Map>(
         Forward<Deref<decltype(call)>>(call));
   }

   TEMPLATE() template<bool REVERSE> LANGULUS(INLINED)
   size_t TABLE()::ForEachKeyElement(auto&& call) {
      return BlockMap::ForEachKeyElement<REVERSE, Map>(
         Forward<Deref<decltype(call)>>(call));
   }

   TEMPLATE() template<bool REVERSE> LANGULUS(INLINED)
   size_t TABLE()::ForEachValueElement(auto&& call) const {
      return BlockMap::ForEachValueElement<REVERSE, const Map>(
         Forward<Deref<decltype(call)>>(call));
   }

   TEMPLATE() template<bool REVERSE> LANGULUS(INLINED)
   size_t TABLE()::ForEachValueElement(auto&& call) {
      return BlockMap::ForEachValueElement<REVERSE, Map>(
         Forward<Deref<decltype(call)>>(call));
   }

   TEMPLATE() template<bool REVERSE> LANGULUS(INLINED)
   size_t TABLE()::ForEachKey(auto&&...call) const {
      static_assert(sizeof...(call) > 0, "No iterators in ForEachKey");
      return BlockMap::ForEachKey<REVERSE, const Map>(
         Forward<Deref<decltype(call)>>(call)...);
   }

   TEMPLATE() template<bool REVERSE> LANGULUS(INLINED)
   size_t TABLE()::ForEachKey(auto&&...call) {
      static_assert(sizeof...(call) > 0, "No iterators in ForEachKey");
      return BlockMap::ForEachKey<REVERSE, Map>(
         Forward<Deref<decltype(call)>>(call)...);
   }

   TEMPLATE() template<bool REVERSE> LANGULUS(INLINED)
   size_t TABLE()::ForEachValue(auto&&...call) const {
      static_assert(sizeof...(call) > 0, "No iterators in ForEachValue");
      return BlockMap::ForEachValue<REVERSE, const Map>(
         Forward<Deref<decltype(call)>>(call)...);
   }

   TEMPLATE() template<bool REVERSE> LANGULUS(INLINED)
   size_t TABLE()::ForEachValue(auto&&...call) {
      static_assert(sizeof...(call) > 0, "No iterators in ForEachValue");
      return BlockMap::ForEachValue<REVERSE, Map>(
         Forward<Deref<decltype(call)>>(call)...);
   }

   TEMPLATE() template<bool REVERSE, bool SKIP> LANGULUS(INLINED)
   size_t TABLE()::ForEachKeyDeep(auto&&...call) const {
      static_assert(sizeof...(call) > 0, "No iterators in ForEachKeyDeep");
      return BlockMap::ForEachKeyDeep<REVERSE, SKIP, const Map>(
         Forward<Deref<decltype(call)>>(call)...);
   }

   TEMPLATE() template<bool REVERSE, bool SKIP> LANGULUS(INLINED)
   size_t TABLE()::ForEachKeyDeep(auto&&...call) {
      static_assert(sizeof...(call) > 0, "No iterators in ForEachKeyDeep");
      return BlockMap::ForEachKeyDeep<REVERSE, SKIP, Map>(
         Forward<Deref<decltype(call)>>(call)...);
   }

   TEMPLATE() template<bool REVERSE, bool SKIP> LANGULUS(INLINED)
   size_t TABLE()::ForEachValueDeep(auto&&...call) const {
      static_assert(sizeof...(call) > 0, "No iterators in ForEachValueDeep");
      return BlockMap::ForEachValueDeep<REVERSE, SKIP, const Map>(
         Forward<Deref<decltype(call)>>(call)...);
   }

   TEMPLATE() template<bool REVERSE, bool SKIP> LANGULUS(INLINED)
   size_t TABLE()::ForEachValueDeep(auto&&...call) {
      static_assert(sizeof...(call) > 0, "No iterators in ForEachValueDeep");
      return BlockMap::ForEachValueDeep<REVERSE, SKIP, Map>(
         Forward<Deref<decltype(call)>>(call)...);
   }
   
   /// Check if key origin type matches any of the list                       
   ///   @tparam K1, KN... - the list of types to compare against             
   ///   @return true if key type matches at least one of the others          
   TEMPLATE() template<CT::NotVoid K1, CT::NotVoid...KN> LANGULUS(INLINED)
   constexpr bool TABLE()::IsKey() const noexcept {
      return BlockMap::IsKey<Map, K1, KN...>();
   }

   /// Check if key origin type matches another                               
   ///   @param key - the key type to compare against                         
   ///   @return true if key matches the contained key origin type            
   TEMPLATE() LANGULUS(INLINED)
   bool TABLE()::IsKey(DMeta key) const noexcept {
      return BlockMap::IsKey<Map>(key);
   }

   /// Check if cv-unqualified key type matches any of the list               
   ///   @tparam K1, KN... - the list of types to compare against             
   ///   @return true if key type matches at least one of the others          
   TEMPLATE() template<CT::NotVoid K1, CT::NotVoid...KN> LANGULUS(INLINED)
   constexpr bool TABLE()::IsKeySimilar() const noexcept {
      return BlockMap::IsKeySimilar<Map, K1, KN...>();
   }

   /// Check if cv-unqualified key type matches another                       
   ///   @param key - the key type to compare against                         
   ///   @return true if key matches the contained key unqualified type       
   TEMPLATE() LANGULUS(INLINED)
   bool TABLE()::IsKeySimilar(DMeta key) const noexcept {
      return BlockMap::IsKeySimilar<Map>(key);
   }

   /// Check if key type exactly matches any of the list                      
   ///   @tparam K1, KN... - the list of types to compare against             
   ///   @return true if key type matches at least one of the others          
   TEMPLATE() template<CT::NotVoid K1, CT::NotVoid...KN> LANGULUS(INLINED)
   constexpr bool TABLE()::IsKeyExact() const noexcept {
      return BlockMap::IsKeyExact<Map, K1, KN...>();
   }

   /// Check if key type exactly matches any of the list                      
   ///   @param key - the key type to compare against                         
   ///   @return true if key matches the contained key unqualified type       
   TEMPLATE() LANGULUS(INLINED)
   bool TABLE()::IsKeyExact(DMeta key) const noexcept {
      return BlockMap::IsKeyExact<Map>(key);
   }

   /// Check if value origin type matches any of the list                     
   ///   @tparam V1, VN... - the list of types to compare against             
   ///   @return true if value type matches at least one of the others        
   TEMPLATE() template<CT::NotVoid V1, CT::NotVoid...VN> LANGULUS(INLINED)
   constexpr bool TABLE()::IsValue() const noexcept {
      return BlockMap::IsValue<Map, V1, VN...>();
   }

   /// Check if value origin type matches another                             
   ///   @param value - the value type to compare against                     
   ///   @return true if value matches the contained key origin type          
   TEMPLATE() LANGULUS(INLINED)
   bool TABLE()::IsValue(DMeta value) const noexcept {
      return BlockMap::IsValue<Map>(value);
   }

   /// Check if cv-unqualified value type matches any of the list             
   ///   @tparam V1, VN... - the list of types to compare against             
   ///   @return true if value type matches at least one of the others        
   TEMPLATE() template<CT::NotVoid V1, CT::NotVoid...VN> LANGULUS(INLINED)
   constexpr bool TABLE()::IsValueSimilar() const noexcept {
      return BlockMap::IsValueSimilar<Map, V1, VN...>();
   }

   /// Check if cv-unqualified value type matches another                     
   ///   @param value - the value type to compare against                     
   ///   @return true if value matches the contained value unqualified type   
   TEMPLATE() LANGULUS(INLINED)
   bool TABLE()::IsValueSimilar(DMeta value) const noexcept {
      return BlockMap::IsValueSimilar<Map>(value);
   }

   /// Check if value type exactly matches any of the list                    
   ///   @tparam V1, VN... - the list of types to compare against             
   ///   @return true if value type matches at least one of the others        
   TEMPLATE() template<CT::NotVoid V1, CT::NotVoid...VN> LANGULUS(INLINED)
   constexpr bool TABLE()::IsValueExact() const noexcept {
      return BlockMap::IsValueExact<Map, V1, VN...>();
   }

   /// Check if value type exactly matches any of the list                    
   ///   @param value - the value type to compare against                     
   ///   @return true if value matches the contained value unqualified type   
   TEMPLATE() LANGULUS(INLINED)
   bool TABLE()::IsValueExact(DMeta value) const noexcept {
      return BlockMap::IsValueExact<Map>(value);
   }

   /// Search for a key inside the table, and return it if found              
   ///   @param key - the key to search for                                   
   ///   @return the index if key was found, or IndexNone if not              
   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::Find(const CT::NoIntent auto& key) const -> Index {
      return BlockMap::Find<Map>(key);
   }

   /// Search for a key inside the table, and return an iterator to it        
   ///   @param key - the key to search for                                   
   ///   @return the iterator                                                 
   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::FindIt(const CT::NoIntent auto& key) -> Iterator {
      return BlockMap::FindIt<Map>(key);
   }

   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::FindIt(const CT::NoIntent auto& key) const -> ConstIterator {
      return BlockMap::FindIt<Map>(key);
   }

   /// Returns a reference to the value found for key                         
   /// Throws Except::OutOfRange if element cannot be found                   
   ///   @param key - the key to search for                                   
   ///   @return a reference to the value                                     
   TEMPLATE() LANGULUS(INLINED)
   decltype(auto) TABLE()::At(const CT::NoIntent auto& key) {
      return BlockMap::At<Map>(key);
   }

   TEMPLATE() LANGULUS(INLINED)
   decltype(auto) TABLE()::At(const CT::NoIntent auto& key) const {
      return BlockMap::At<Map>(key);
   }

   /// Access value by key                                                    
   ///   @param key - the key to find                                         
   ///   @return a reference to the value                                     
   TEMPLATE() LANGULUS(INLINED)
   decltype(auto) TABLE()::operator[] (const CT::NoIntent auto& key) {
      return BlockMap::operator [] <Map> (key);
   }

   TEMPLATE() LANGULUS(INLINED)
   decltype(auto) TABLE()::operator[] (const CT::NoIntent auto& key) const {
      return BlockMap::operator [] <Map> (key);
   }
   
   /// Reserves space for the specified number of pairs                       
   ///   @attention does nothing if reserving less than current reserve       
   ///   @param count - number of pairs to allocate                           
   TEMPLATE() LANGULUS(INLINED)
   void TABLE()::Reserve(size_t count) {
      BlockMap::Reserve<Map>(count);
   }

   /// Insert a pair                                                          
   ///   @param other - the data to insert                                    
   ///   @return a reference to this container for chaining                   
   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::operator << (CT::UnfoldInsertable auto&& other) -> Map& {
      BlockMap::UnfoldInsert<Map>(Forward<decltype(other)>(other));
      return *this;
   }
   
   /// Insert a pair                                                          
   ///   @param other - the data to insert                                    
   ///   @return a reference to this container for chaining                   
   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::operator >> (CT::UnfoldInsertable auto&& other) -> Map& {
      BlockMap::UnfoldInsert<Map>(Forward<decltype(other)>(other));
      return *this;
   }
   
   /// Erase a pair via key                                                   
   ///   @param key - the key to search for                                   
   ///   @return 1 if key was found and pair was removed                      
   TEMPLATE() LANGULUS(INLINED)
   size_t TABLE()::RemoveKey(const CT::NoIntent auto& key) {
      return BlockMap::RemoveKey<Map>(key);
   }

   /// Erase all pairs with a given value                                     
   ///   @param value - the match to search for                               
   ///   @return the number of removed pairs                                  
   TEMPLATE() LANGULUS(INLINED)
   size_t TABLE()::RemoveValue(const CT::NoIntent auto& value) {
      return BlockMap::RemoveValue<Map>(value);
   }
     
   /// Erase all pairs matching a pair                                        
   ///   @param value - the match to search for                               
   ///   @return the number of removed pairs                                  
   TEMPLATE() LANGULUS(INLINED)
   size_t TABLE()::RemovePair(const CT::Pair auto& pair) {
      return BlockMap::RemovePair<Map>(pair);
   }
     
   /// Safely erases element at a specific iterator                           
   ///   @attention assumes iterator is produced by this map instance         
   ///   @attention assumes that iterator points to a valid entry             
   ///   @param index - the index to remove                                   
   ///   @return the iterator of the previous element, unless index is the    
   ///           first, or at the end already                                 
   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::RemoveIt(const Iterator& index) -> Iterator {
      return BlockMap::RemoveIt<Map>(index);
   }

   /// Destroy all contained pairs, but don't deallocate                      
   TEMPLATE() LANGULUS(INLINED)
   void TABLE()::Clear() {
      return BlockMap::Clear<Map>();
   }

   /// Destroy all contained pairs and deallocate                             
   TEMPLATE() LANGULUS(INLINED)
   void TABLE()::Reset() {
      return BlockMap::Reset<Map>();
   }

   /// Reduce reserved size, depending on number of contained elements        
   TEMPLATE() LANGULUS(INLINED)
   void TABLE()::Compact() {
      return BlockMap::Compact<Map>();
   }

} // namespace Langulus::Annies

#undef TEMPLATE
#undef TABLE