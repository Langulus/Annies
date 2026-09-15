///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "TSet.hpp"
#include "Set.inl"

#define TEMPLATE()   template<CT::Data T, bool ORDERED>
#define TABLE()      TSet<T, ORDERED>


namespace Langulus::Annies
{

   /// Default construction                                                   
   TEMPLATE() LANGULUS(INLINED)
   constexpr TABLE()::TSet() {
      mKeys.mState = DataState::Typed;
      if constexpr (CT::Constant<T>)
         mKeys.MakeConst();
   }

   /// Refer construction                                                     
   ///   @param other - the table to refer to                                 
   TEMPLATE() LANGULUS(INLINED)
   TABLE()::TSet(const TSet& other)
      : TSet {Refer(other)} {}

   /// Move construction                                                      
   ///   @param other - the table to move                                     
   TEMPLATE() LANGULUS(INLINED)
   TABLE()::TSet(TSet&& other)
      : TSet {Move(other)} {}
   
   /// Create from a list of elements, an array, as well as any other kinds   
   /// of set. Each argument can have an intent or not.                       
   ///   @param t1 - first element and intent                                 
   ///   @param tn - tail of elements (optional, can have intents)            
   TEMPLATE() template<class T1, class...TN>
   requires CT::DeepSetMakable<T, T1, TN...> LANGULUS(INLINED)
   TABLE()::TSet(T1&& t1, TN&&...tn) {
      mKeys.mType = MetaDataOf<T>();

      if constexpr (sizeof...(TN) == 0) {
         using S  = IntentOf<decltype(t1)>;
         using ST = TypeOf<S>;

         if constexpr (CT::Set<ST>) {
            if constexpr (CT::Typed<ST>) {
               // Not type-erased set, do compile-time type checks      
               using STT = TypeOf<ST>;
               if constexpr (CT::Similar<T, STT>) {
                  // Type is binary compatible, just transfer set       
                  BlockSet::BlockTransfer<TSet>(S::Nest(t1));
               }
               else if constexpr (CT::Sparse<T, STT>) {
                  if constexpr (CT::DerivedFrom<T, STT>) {
                     // The statically typed set contains items that    
                     // are base of this container's type. Each element 
                     // should be dynamically cast to this type         
                     for (auto pointer : DeintCast(t1)) {
                        auto dcast = dynamic_cast<T>(&(*pointer));
                        if (dcast)
                           (*this) << dcast;
                     }
                  }
                  else if constexpr (CT::DerivedFrom<STT, T>) {
                     // The statically typed set contains items that    
                     // are derived from this container's type. Each    
                     // element should be statically sliced to this type
                     for (auto pointer : DeintCast(t1))
                        (*this) << static_cast<T>(&(*pointer));
                  }
                  else Insert(Forward<T1>(t1));
               }
               else Insert(Forward<T1>(t1));
            }
            else {
               // Type-erased set, do run-time type checks              
               if (mKeys.mType == DeintCast(t1).GetType()) {
                  // If types are exactly the same, it is safe to       
                  // absorb the set, essentially converting a type-     
                  // erased Set back to its TSet equivalent             
                  BlockSet::BlockTransfer<TSet>(S::Nest(t1));
               }
               else Insert(Forward<T1>(t1));
            }
         }
         else Insert(Forward<T1>(t1));
      }
      else Insert(Forward<T1>(t1), Forward<TN>(tn)...);
   }

   /// Destroys the set and all it's contents                                 
   TEMPLATE()
   TABLE()::~TSet() {
      BlockSet::Free<TSet>();
   }

   /// Refer assignment                                                       
   ///   @param rhs - the container to refer to                               
   ///   @return a reference to this container                                
   TEMPLATE() LANGULUS(INLINED)
   TABLE()& TABLE()::operator = (const TSet& rhs) {
      static_assert(CT::DeepSetAssignable<T, Referred<TSet<T>>>);
      return operator = (Refer(rhs));
   }

   /// Move assignment                                                        
   ///   @param rhs - the container to move                                   
   ///   @return a reference to this container                                
   TEMPLATE() LANGULUS(INLINED)
   TABLE()& TABLE()::operator = (TSet&& rhs) {
      static_assert(CT::DeepSetAssignable<T, Moved<TSet<T>>>);
      return operator = (Move(rhs));
   }

   /// Generic assignment                                                     
   ///   @param rhs - the element/array/container to assign                   
   ///   @return a reference to this container                                
   TEMPLATE() template<class T1>
   requires CT::DeepSetAssignable<T, T1> LANGULUS(INLINED)
   TABLE()& TABLE()::operator = (T1&& rhs) {
      using S  = IntentOf<decltype(rhs)>;
      using ST = TypeOf<S>;
       
      if constexpr (CT::Set<ST>) {
         // Potentially absorb a set                                    
         if (static_cast<const BlockSet*>(this)
          == static_cast<const BlockSet*>(&DeintCast(rhs)))
            return *this;

         BlockSet::Free<TSet>();
         new (this) TSet {S::Nest(rhs)};
      }
      else {
         // Unfold-insert                                               
         Clear();
         BlockSet::UnfoldInsert<TSet>(S::Nest(rhs));
      }

      return *this;
   }

   /// Templated tables are always typed                                      
   ///   @return false                                                        
   TEMPLATE() LANGULUS(INLINED)
   constexpr bool TABLE()::IsTyped() const noexcept {
      return true;
   }
   
   /// Templated tables are always typed                                      
   ///   @return false                                                        
   TEMPLATE() LANGULUS(INLINED)
   constexpr bool TABLE()::IsUntyped() const noexcept {
      return false;
   }
   
   /// Templated tables are always type-constrained                           
   ///   @return true                                                         
   TEMPLATE() LANGULUS(INLINED)
   constexpr bool TABLE()::IsTypeConstrained() const noexcept {
      return true;
   }
   
   /// Check if key type is deep                                              
   TEMPLATE() LANGULUS(INLINED)
   constexpr bool TABLE()::IsDeep() const noexcept {
      return BlockSet::IsDeep<TSet>();
   }

   /// Check if the key type is a pointer                                     
   TEMPLATE() LANGULUS(INLINED)
   constexpr bool TABLE()::IsSparse() const noexcept {
      return CT::Sparse<T>;
   }

   /// Check if the key type is not a pointer                                 
   TEMPLATE() LANGULUS(INLINED)
   constexpr bool TABLE()::IsDense() const noexcept {
      return CT::Dense<T>;
   }

   /// Get the size of a single key, in bytes                                 
   ///   @return the number of bytes a single key contains                    
   TEMPLATE()
   constexpr Size TABLE()::GetStride() const noexcept {
      return sizeof(T); 
   }

   /// Check if a type can be inserted to this block                          
   ///   @param type - check if a given type is insertable to this block      
   ///   @return true if able to insert an instance of the type to this block 
   TEMPLATE() LANGULUS(INLINED)
   constexpr bool TABLE()::IsInsertable(DMeta type) const noexcept {
      return BlockSet::IsInsertable<TSet>(type);
   }

   /// Check if a static type can be inserted                                 
   ///   @return true if able to insert an instance of the type to this block 
   TEMPLATE() template<CT::Data T1> LANGULUS(INLINED)
   constexpr bool TABLE()::IsInsertable() const noexcept {
      return BlockSet::IsInsertable<T1, TSet>();
   }

   /// Get the key meta data                                                  
   /// Also implicitly initializes the internal key type                      
   ///   @attention this shouldn't be called on static initialization time    
   ///   @return the meta definition of the key type                          
   TEMPLATE() LANGULUS(INLINED)
   DMeta TABLE()::GetType() const {
      return BlockSet::GetType<TSet>();
   }
   
   /// Check if this value type is similar to one of the listed types,        
   /// ignoring density and cv-qualifiers                                     
   ///   @tparam T1, TN... - the types to compare against                     
   ///   @return true if value type is similar to at least one of the types   
   TEMPLATE() template<CT::Data T1, CT::Data...TN> LANGULUS(INLINED)
   constexpr bool TABLE()::Is() const noexcept {
      return BlockSet::Is<TSet, T1, TN...>();
   }
   
   /// Check if value type loosely matches a given type, ignoring             
   /// density and cv-qualifiers                                              
   ///   @param meta - the type to check for                                  
   ///   @return true if this set contains similar value data                 
   TEMPLATE() LANGULUS(INLINED)
   bool TABLE()::Is(DMeta meta) const noexcept {
      return BlockSet::Is<TSet>(meta);
   }

   /// Check if value type is similar to one of the listed types,             
   /// ignoring cv-qualifiers only                                            
   ///   @tparam T1, TN... - the types to compare against                     
   ///   @return true if value type is similar to at least one of the types   
   TEMPLATE() template<CT::Data T1, CT::Data...TN> LANGULUS(INLINED)
   constexpr bool TABLE()::IsSimilar() const noexcept {
      return BlockSet::IsSimilar<TSet, T1, TN...>();
   }
   
   /// Check if value type loosely matches a given type, ignoring             
   /// cv-qualifiers only                                                     
   ///   @param meta - the type to check for                                  
   ///   @return true if this set contains similar value data                 
   TEMPLATE() LANGULUS(INLINED)
   bool TABLE()::IsSimilar(DMeta meta) const noexcept {
      return BlockSet::IsSimilar<TSet>(meta);
   }

   /// Check if value type is exactly as one of the listed types,             
   /// including by density and cv-qualifiers                                 
   ///   @tparam T1, TN... - the types to compare against                     
   ///   @return true if value type exactly matches at least one type         
   TEMPLATE() template<CT::Data T1, CT::Data...TN> LANGULUS(INLINED)
   constexpr bool TABLE()::IsExact() const noexcept {
      return BlockSet::IsExact<TSet, T1, TN...>();
   }
   
   /// Check if value type is exactly the provided type,                      
   /// including the density and cv-qualifiers                                
   ///   @param type - the type to match                                      
   ///   @return true if this set contains this exact value data              
   TEMPLATE() LANGULUS(INLINED)
   bool TABLE()::IsExact(DMeta meta) const noexcept {
      return BlockSet::IsExact<TSet>(meta);
   }
   
   /// Get a valid key by any index, safely                                   
   ///   @param index - the index to use                                      
   ///   @return the element, wrapped in a Block                              
   TEMPLATE() LANGULUS(INLINED)
   const T& TABLE()::Get(const CT::Index auto index) const {
      return BlockSet::Get<TSet>(index);
   }
   
   /// Accesses set elements based on a safe index, that accounts for         
   /// empty table slots                                                      
   ///   @param idx - the index to access                                     
   ///   @return a reference to the value, or a block if set is type-erased   
   TEMPLATE() LANGULUS(INLINED)
   const T& TABLE()::operator[] (const CT::Index auto index) const {
      return BlockSet::operator[]<TSet>(index);
   }

   /// Reserves space for the specified number of pairs                       
   ///   @attention does nothing if reserving less than current reserve       
   ///   @param count - number of pairs to allocate                           
   TEMPLATE() LANGULUS(INLINED)
   void TABLE()::Reserve(Count count) {
      BlockSet::Reserve<TSet>(count);
   }

   /// Unfold-insert elements, with or without intents                        
   ///   @param t1 - element, or array of elements, to insert                 
   ///   @param tn... - the rest of the elements (optional)                   
   ///   @return the number of inserted elements                              
   TEMPLATE() template<class T1, class...TN>
   requires CT::UnfoldMakableFrom<T, T1, TN...> LANGULUS(INLINED)
   Count TABLE()::Insert(T1&& t1, TN&&...tn) {
      return BlockSet::Insert<TSet>(Forward<T1>(t1), Forward<TN>(tn)...);
   }
   
   /// Insert all elements of a set, with or without intents                  
   ///   @param t1 - the set to insert                                        
   ///   @return number of inserted elements                                  
   TEMPLATE() template<class T1> requires CT::Set<Deint<T1>> LANGULUS(INLINED)
   Count TABLE()::InsertBlock(T1&& t1) {
      return BlockSet::InsertBlock<TSet>(Forward<T1>(t1));
   }
   
   /// Insert all elements of a block, with or without intents                
   ///   @param t1 - the block to insert                                      
   ///   @return number of inserted elements                                  
   TEMPLATE() template<class T1> requires CT::Block<Deint<T1>> LANGULUS(INLINED)
   Count TABLE()::InsertBlock(T1&& t1) {
      return BlockSet::InsertBlock<TSet>(Forward<T1>(t1));
   }

   /// Move-insert a pair inside the map, with or without intents             
   ///   @param rhs - the pair and intent to insert                           
   ///   @return a reference to this table for chaining                       
   TEMPLATE() template<class T1>
   requires CT::UnfoldMakableFrom<T, T1> LANGULUS(INLINED)
   TABLE()& TABLE()::operator << (T1&& rhs){
      Insert(Forward<T1>(rhs));
      return *this;
   }

   /// Clears all data, but doesn't deallocate, and retains state             
   TEMPLATE()
   void TABLE()::Clear() {
      BlockSet::Clear<TSet>();
   }

   /// Clears all data, state, and deallocates                                
   TEMPLATE()
   void TABLE()::Reset() {
      BlockSet::Reset<TSet>();
   }
   
   /// Safely erases element at a specific iterator                           
   ///   @attention assumes iterator is produced by this map instance         
   ///   @param index - the index to remove                                   
   ///   @return the iterator of the previous element, unless index is the    
   ///           first, or at the end already                                 
   TEMPLATE()
   auto TABLE()::RemoveIt(const Iterator& index) -> Iterator {
      const auto sentinel = GetReserved();
      auto offset = static_cast<Offset>(index.mInfo - mInfo);
      if (offset >= sentinel)
         return end();

      RemoveInner<T>(offset--); //TODO what if map shrinks, offset might become invalid? Doesn't shrink for now
      
      while (offset < sentinel and 0 == mInfo[offset])
         --offset;

      if (offset >= sentinel)
         offset = 0;

      return {mInfo + offset, index.mSentinel, &GetRaw<TSet>(offset)};
   }

   /// Erase a pair via key                                                   
   ///   @param match - the key to search for                                 
   ///   @return the number of removed pairs                                  
   TEMPLATE() LANGULUS(INLINED)
   Count TABLE()::Remove(const T& match) {
      return BlockSet::template Remove<TABLE(), T>(match);
   }

   /// If possible reallocates the map to a smaller one                       
   TEMPLATE()
   void TABLE()::Compact() {
      TODO();
   }


   ///                                                                        
   ///   SEARCH                                                               
   ///                                                                        
   
   /// Checks if both tables contain the same entries                         
   ///   @param other - the table to compare against                          
   ///   @return true if tables match                                         
   TEMPLATE() template<CT::NoIntent T1>
   requires (CT::Set<T1> or CT::Comparable<T, T1>) LANGULUS(INLINED)
   bool TABLE()::operator == (const T1& other) const {
      return BlockSet::operator == <TSet> (other);
   }

   /// Search for a key inside the table                                      
   ///   @param key - the key to search for                                   
   ///   @return true if key is found, false otherwise                        
   TEMPLATE() template<CT::NoIntent T1>
   requires CT::Comparable<T, T1> LANGULUS(INLINED)
   bool TABLE()::Contains(T1 const& key) const {
      return BlockSet::Contains<TSet>(key);
   }

   /// Search for a key inside the table, and return it if found              
   ///   @param key - the key to search for                                   
   ///   @return the index if key was found, or IndexNone if not              
   TEMPLATE() template<CT::NoIntent T1>
   requires CT::Comparable<T, T1> LANGULUS(INLINED)
   auto TABLE()::Find(T1 const& key) const -> Index {
      return BlockSet::Find<TSet>(key);
   }
   
   /// Search for a key inside the table, and return an iterator to it        
   ///   @param key - the key to search for                                   
   ///   @return the iterator                                                 
   TEMPLATE() template<CT::NoIntent T1>
   requires CT::Comparable<T, T1> LANGULUS(INLINED)
   auto TABLE()::FindIt(T1 const& key) -> Iterator {
      const auto found = FindInner<TABLE()>(key);
      if (found == InvalidOffset)
         return end();

      return {
         GetInfo() + found, GetInfoEnd(),
         GetRaw<TABLE()>(found)
      };
   }

   TEMPLATE() template<CT::NoIntent T1>
   requires CT::Comparable<T, T1> LANGULUS(INLINED)
   auto TABLE()::FindIt(T1 const& key) const -> ConstIterator {
      return const_cast<TABLE()*>(this)->FindIt(key);
   }

   /// Get iterator to first element                                          
   ///   @return an iterator to the first element, or end if empty            
   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::begin() noexcept -> Iterator {
      return BlockSet::begin<TSet>();
   }
   
   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::begin() const noexcept -> ConstIterator {
      return BlockSet::begin<TSet>();
   }

   /// Get iterator to the last element                                       
   ///   @return an iterator to the last element, or end if empty             
   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::last() noexcept -> Iterator {
      return BlockSet::last<TSet>();
   }

   TEMPLATE() LANGULUS(INLINED)
   auto TABLE()::last() const noexcept -> ConstIterator {
      return BlockSet::last<TSet>();
   }

   /// Iterate keys inside the map, and perform a set of functions on them    
   /// depending on the contained type                                        
   /// You can break the loop, by returning false inside f()                  
   ///   @param f - the functions to call for each key block                  
   ///   @return the number of successful f() executions                      
   TEMPLATE() template<bool REVERSE> LANGULUS(INLINED)
   Count TABLE()::ForEach(auto&&...f) const {
      static_assert(sizeof...(f) > 0, "No iterators in ForEach");
      return BlockSet::ForEach<REVERSE, const TSet>(
         Forward<Deref<decltype(f)>>(f)...);
   }

   TEMPLATE() template<bool REVERSE> LANGULUS(INLINED)
   Count TABLE()::ForEach(auto&&...f) {
      static_assert(sizeof...(f) > 0, "No iterators in ForEach");
      return BlockSet::ForEach<REVERSE, TSet>(
         Forward<Deref<decltype(f)>>(f)...);
   }

   /// Iterate all keys inside the set, and perform f() on them               
   /// You can break the loop, by returning false inside f()                  
   ///   @param f - the function to call for each key block                   
   ///   @return the number of successful f() executions                      
   TEMPLATE() template<bool REVERSE> LANGULUS(INLINED)
   Count TABLE()::ForEachElement(auto&& f) const {
      return BlockSet::ForEachElement<REVERSE, const TSet>(
         Forward<Deref<decltype(f)>>(f));
   }

   TEMPLATE() template<bool REVERSE> LANGULUS(INLINED)
   Count TABLE()::ForEachElement(auto&& f) {
      return BlockSet::ForEachElement<REVERSE, TSet>(
         Forward<Deref<decltype(f)>>(f));
   }

   /// Iterate each subblock of keys inside the set, and perform a set of     
   /// functions on them                                                      
   ///   @param f - the functions to call for each key block                  
   ///   @return the number of successful f() executions                      
   TEMPLATE() template<bool REVERSE, bool SKIP> LANGULUS(INLINED)
   Count TABLE()::ForEachDeep(auto&&...f) const {
      static_assert(sizeof...(f) > 0, "No iterators in ForEachDeep");
      return BlockSet::ForEachDeep<REVERSE, const TSet>(
         Forward<Deref<decltype(f)>>(f)...);
   }

   TEMPLATE() template<bool REVERSE, bool SKIP> LANGULUS(INLINED)
   Count TABLE()::ForEachDeep(auto&&...f) {
      static_assert(sizeof...(f) > 0, "No iterators in ForEachDeep");
      return BlockSet::ForEachDeep<REVERSE, TSet>(
         Forward<Deref<decltype(f)>>(f)...);
   }

} // namespace Langulus::Annies

#undef TEMPLATE
#undef TABLE
