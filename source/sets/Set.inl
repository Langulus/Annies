///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Set.hpp"
#include "../blocks/BlockSet/BlockSet-Capsulation.inl"
#include "../blocks/BlockSet/BlockSet-Iteration.inl"
#include "../blocks/BlockSet/BlockSet-Indexing.inl"
#include "../blocks/BlockSet/BlockSet-Construct.inl"
#include "../blocks/BlockSet/BlockSet-RTTI.inl"
#include "../blocks/BlockSet/BlockSet-Memory.inl"
#include "../blocks/BlockSet/BlockSet-Remove.inl"
#include "../blocks/BlockSet/BlockSet-Insert.inl"
#include "../blocks/BlockSet/BlockSet-Compare.inl"

#define TEMPLATE()   template<bool ORDERED>
#define TABLE()      Set<ORDERED>


namespace Langulus::Annies
{

   /// Refer constructor                                                      
   ///   @param other - the container to refer to                             
   TEMPLATE() LANGULUS(INLINED)
   TABLE()::Set(const Set& other)
      : Set {Refer(other)} {}

   /// Move constructor                                                       
   ///   @param other - the container to move                                 
   TEMPLATE() LANGULUS(INLINED)
   TABLE()::Set(Set&& other)
      : Set {Move(other)} {}
   
   /// Unfold constructor. If there's one set argument, it will be absorbed   
   ///   @param t1 - first element and intent                                 
   ///   @param tn... - the rest of the elements (optional, can have intents) 
   TEMPLATE() template<class T1, class...TN>
   requires CT::UnfoldInsertable<T1, TN...> LANGULUS(INLINED)
   TABLE()::Set(T1&& t1, TN&&...tn) {
      if constexpr (sizeof...(TN) == 0) {
         using S = IntentOf<decltype(t1)>;
         using T = TypeOf<S>;

         if constexpr (CT::Set<T>)
            BlockSet::BlockTransfer<Set>(S::Nest(t1));
         else
            Insert<Set>(Forward<T1>(t1));
      }
      else Insert<Set>(Forward<T1>(t1), Forward<TN>(tn)...);
   }

   /// Set destructor                                                         
   TEMPLATE() LANGULUS(INLINED)
   TABLE()::~Set() {
      BlockSet::Free<Set>();
   }

   /// Refer assignment                                                       
   ///   @param rhs - set to refer to                                         
   ///   @return a reference to this set                                      
   TEMPLATE() LANGULUS(INLINED)
   TABLE()& TABLE()::operator = (const Set& rhs) {
      return operator = (Refer(rhs));
   }

   /// Move assignment                                                        
   ///   @param rhs - unordered set to move-insert                            
   ///   @return a reference to this set                                      
   TEMPLATE() LANGULUS(INLINED)
   TABLE()& TABLE()::operator = (Set&& rhs) {
      return operator = (Move(rhs));
   }

   /// Pair/map assignment, with or without intents                           
   ///   @param rhs - the pair, or map to assign, with optional intents       
   ///   @return a reference to this container                                
   TEMPLATE() LANGULUS(INLINED)
   TABLE()& TABLE()::operator = (CT::UnfoldInsertable auto&& rhs) {
      using S = IntentOf<decltype(rhs)>;
      using T = TypeOf<S>;

      if constexpr (CT::Set<T>) {
         // Potentially absorb a container                              
         if (static_cast<const BlockSet*>(this)
          == static_cast<const BlockSet*>(&DeintCast(rhs)))
            return *this;

         BlockSet::Free<Set>();
         new (this) Set {S::Nest(rhs)};
      }
      else {
         // Unfold-insert                                               
         Clear();
         BlockSet::UnfoldInsert<Set>(S::Nest(rhs));
      }

      return *this;
   }
   
   /// Get a valid key by any index, safely                                   
   ///   @param index - the index to use                                      
   ///   @return the element, wrapped in a Block                              
   TEMPLATE() LANGULUS(INLINED)
   decltype(auto) TABLE()::Get(const CT::Index auto index) const {
      return BlockSet::Get<Set>(index);
   }
   
   /// Accesses set elements based on a safe index, that accounts for         
   /// empty table slots                                                      
   ///   @param idx - the index to access                                     
   ///   @return a reference to the value, or a block if set is type-erased   
   TEMPLATE() LANGULUS(INLINED)
   decltype(auto) TABLE()::operator[] (const CT::Index auto index) const {
      return BlockSet::operator[]<Set>(index);
   }
   
   /// Get iterator to first element                                          
   ///   @return an iterator to the first element, or end if empty            
   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::begin() noexcept -> Iterator {
      return BlockSet::begin<Set>();
   }
   
   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::begin() const noexcept -> ConstIterator {
      return BlockSet::begin<Set>();
   }

   /// Get iterator to the last element                                       
   ///   @return an iterator to the last element, or end if empty             
   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::last() noexcept -> Iterator {
      return BlockSet::last<Set>();
   }

   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::last() const noexcept -> ConstIterator {
      return BlockSet::last<Set>();
   }

   /// Iterate keys inside the map, and perform a set of functions on them    
   /// depending on the contained type                                        
   /// You can break the loop, by returning false inside f()                  
   ///   @param f - the functions to call for each key block                  
   ///   @return the number of successful f() executions                      
   TEMPLATE() template<bool REVERSE> LANGULUS(INLINED)
   size_t TABLE()::ForEach(auto&&...f) const {
      static_assert(sizeof...(f) > 0, "No iterators in ForEach");
      return BlockSet::ForEach<REVERSE, const Set>(
         Forward<Deref<decltype(f)>>(f)...);
   }

   TEMPLATE() template<bool REVERSE> LANGULUS(INLINED)
   size_t TABLE()::ForEach(auto&&...f) {
      static_assert(sizeof...(f) > 0, "No iterators in ForEach");
      return BlockSet::ForEach<REVERSE, Set>(
         Forward<Deref<decltype(f)>>(f)...);
   }

   /// Iterate all keys inside the set, and perform f() on them               
   /// You can break the loop, by returning false inside f()                  
   ///   @param f - the function to call for each key block                   
   ///   @return the number of successful f() executions                      
   TEMPLATE() template<bool REVERSE> LANGULUS(INLINED)
   size_t TABLE()::ForEachElement(auto&& f) const {
      return BlockSet::ForEachElement<REVERSE, const Set>(
         Forward<Deref<decltype(f)>>(f));
   }

   TEMPLATE() template<bool REVERSE> LANGULUS(INLINED)
   size_t TABLE()::ForEachElement(auto&& f) {
      return BlockSet::ForEachElement<REVERSE, Set>(
         Forward<Deref<decltype(f)>>(f));
   }

   /// Iterate each subblock of keys inside the set, and perform a set of     
   /// functions on them                                                      
   ///   @param f - the functions to call for each key block                  
   ///   @return the number of successful f() executions                      
   TEMPLATE() template<bool REVERSE, bool SKIP> LANGULUS(INLINED)
   size_t TABLE()::ForEachDeep(auto&&...f) const {
      static_assert(sizeof...(f) > 0, "No iterators in ForEachDeep");
      return BlockSet::ForEachDeep<REVERSE, const Set>(
         Forward<Deref<decltype(f)>>(f)...);
   }

   TEMPLATE() template<bool REVERSE, bool SKIP> LANGULUS(INLINED)
   size_t TABLE()::ForEachDeep(auto&&...f) {
      static_assert(sizeof...(f) > 0, "No iterators in ForEachDeep");
      return BlockSet::ForEachDeep<REVERSE, Set>(
         Forward<Deref<decltype(f)>>(f)...);
   }
   
   /// Check if this value type is similar to one of the listed types,        
   /// ignoring density and cv-qualifiers                                     
   ///   @tparam T1, TN... - the types to compare against                     
   ///   @return true if value type is similar to at least one of the types   
   TEMPLATE() template<CT::NotVoid T1, CT::NotVoid...TN> LANGULUS(INLINED)
   constexpr bool TABLE()::Is() const noexcept {
      return BlockSet::Is<Set, T1, TN...>();
   }
   
   /// Check if value type loosely matches a given type, ignoring             
   /// density and cv-qualifiers                                              
   ///   @param meta - the type to check for                                  
   ///   @return true if this set contains similar value data                 
   TEMPLATE() LANGULUS(INLINED)
   bool TABLE()::Is(DMeta meta) const noexcept {
      return BlockSet::Is<Set>(meta);
   }

   /// Check if value type is similar to one of the listed types,             
   /// ignoring cv-qualifiers only                                            
   ///   @tparam T1, TN... - the types to compare against                     
   ///   @return true if value type is similar to at least one of the types   
   TEMPLATE() template<CT::NotVoid T1, CT::NotVoid...TN> LANGULUS(INLINED)
   constexpr bool TABLE()::IsSimilar() const noexcept {
      return BlockSet::IsSimilar<Set, T1, TN...>();
   }
   
   /// Check if value type loosely matches a given type, ignoring             
   /// cv-qualifiers only                                                     
   ///   @param meta - the type to check for                                  
   ///   @return true if this set contains similar value data                 
   TEMPLATE() LANGULUS(INLINED)
   bool TABLE()::IsSimilar(DMeta meta) const noexcept {
      return BlockSet::IsSimilar<Set>(meta);
   }

   /// Check if value type is exactly as one of the listed types,             
   /// including by density and cv-qualifiers                                 
   ///   @tparam T1, TN... - the types to compare against                     
   ///   @return true if value type exactly matches at least one type         
   TEMPLATE() template<CT::NotVoid T1, CT::NotVoid...TN> LANGULUS(INLINED)
   constexpr bool TABLE()::IsExact() const noexcept {
      return BlockSet::IsExact<Set, T1, TN...>();
   }
   
   /// Check if value type is exactly the provided type,                      
   /// including the density and cv-qualifiers                                
   ///   @param type - the type to match                                      
   ///   @return true if this set contains this exact value data              
   TEMPLATE() LANGULUS(INLINED)
   bool TABLE()::IsExact(DMeta meta) const noexcept {
      return BlockSet::IsExact<Set>(meta);
   }

   /// Search for a key inside the table, and return it if found              
   ///   @param key - the key to search for                                   
   ///   @return the index if key was found, or IndexNone if not              
   TEMPLATE() LANGULUS(INLINED)
   Index TABLE()::Find(const CT::NoIntent auto& key) const {
      return BlockSet::Find<Set>(key);
   }

   /// Search for a key inside the table, and return an iterator to it        
   ///   @param key - the key to search for                                   
   ///   @return the iterator                                                 
   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::FindIt(const CT::NoIntent auto& key) -> Iterator {
      return BlockSet::FindIt<Set>(key);
   }

   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::FindIt(const CT::NoIntent auto& key) const -> ConstIterator {
      return BlockSet::FindIt<Set>(key);
   }

   /// Insert an element/set/array in the set                                 
   ///   @attention << and >> do the same thing, as sets aren't sequential    
   ///   @param other - the data to insert                                    
   ///   @return a reference to this container for chaining                   
   TEMPLATE() LANGULUS(INLINED)
   TABLE()& TABLE()::operator << (CT::UnfoldInsertable auto&& other) {
      BlockSet::UnfoldInsert<Set>(Forward<decltype(other)>(other));
      return *this;
   }
   
   /// Insert an element/set/array in the set                                 
   ///   @attention << and >> do the same thing, as sets aren't sequential    
   ///   @param other - the data to insert                                    
   ///   @return a reference to this container for chaining                   
   TEMPLATE() LANGULUS(INLINED)
   TABLE()& TABLE()::operator >> (CT::UnfoldInsertable auto&& other) {
      BlockSet::UnfoldInsert<Set>(Forward<decltype(other)>(other));
      return *this;
   }
   
   /// Reserves space for the specified number of elements                    
   ///   @attention does nothing if reserving less than current reserve       
   ///   @param count - number of elements to allocate                        
   TEMPLATE() LANGULUS(INLINED)
   void TABLE()::Reserve(size_t count) {
      BlockSet::Reserve<Set>(count);
   }

} // namespace Langulus::Annies

#undef TEMPLATE
#undef TABLE