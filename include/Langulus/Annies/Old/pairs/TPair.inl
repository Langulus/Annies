///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "TPair.hpp"

#define TEMPLATE()   template<class K, class V>
#define PAIR()       TPair<K, V>


namespace Langulus::Annies
{

   /// Constructor from any other pair (if K and V aren't references)         
   ///   @param pair - the pair and intent to use for initialization          
   TEMPLATE() template<class P>
   requires CT::PairMakable<K, V, P> LANGULUS(INLINED)
   PAIR()::TPair(P&& pair)
      : mKey   {IntentOf<decltype(pair)>::Nest(DeintCast(pair).mKey)}
      , mValue {IntentOf<decltype(pair)>::Nest(DeintCast(pair).mValue)} {}

   /// Constructor from key and value, if K and V aren't references,          
   /// not aggregate types                                                    
   ///   @param key - the key and intent                                      
   ///   @param val - the value and intent                                    
   TEMPLATE() template<class K1, class V1>
   requires (CT::MakableFrom<K, K1> and CT::MakableFrom<V, V1>
   and CT::NotReference<K, V>) LANGULUS(INLINED)
   PAIR()::TPair(K1&& key, V1&& val)
      : mKey   {IntentOf<decltype(key)>::Nest(key)}
      , mValue {IntentOf<decltype(val)>::Nest(val)} {}

   /// Constructor from key and value (if K and V are references)             
   ///   @param key - the key and intent                                      
   ///   @param val - the value and intent                                    
   TEMPLATE() LANGULUS(INLINED)
   PAIR()::TPair(K&& key, V&& val) noexcept requires CT::Reference<K, V>
      : mKey   {Forward<K>(key)}
      , mValue {Forward<V>(val)} {}

   /// Assignment from any other pair (if K and V aren't references)          
   ///   @param pair - the pair and intent to assign                          
   TEMPLATE() template<class P>
   requires CT::PairAssignable<K, V, P> LANGULUS(INLINED)
   PAIR()& PAIR()::operator = (P&& pair) {
      using S = IntentOf<decltype(pair)>;
      mKey   = S::Nest(DeintCast(pair).mKey);
      mValue = S::Nest(DeintCast(pair).mValue);
      return *this;
   }

   /// Comparison                                                             
   ///   @param rhs - pair to compare against                                 
   ///   @return true if pairs match                                          
   TEMPLATE() template<class P>
   requires CT::PairComparable<K, V, P> LANGULUS(INLINED)
   bool PAIR()::operator == (const P& rhs) const {
      return mKey == rhs.mKey and mValue == rhs.mValue;
   }

   /// Get the pair's hash                                                    
   ///   @attention hash is not cached, so this function is slow              
   ///   @return the hash                                                     
   TEMPLATE() LANGULUS(ALWAYS_INLINED)
   Hash PAIR()::GetHash() const requires CT::Hashable<K, V> {
      return HashOf(mKey, mValue);
   }

   /// Implicit cast to constant pair                                         
   TEMPLATE() LANGULUS(ALWAYS_INLINED)
   PAIR()::operator TPair<const Deref<K>&, const Deref<V>&>() const noexcept
   requires CT::Reference<K, V> {
      return {mKey, mValue};
   }

   /// Get contained key                                                      
   ///   @return the key                                                      
   TEMPLATE() LANGULUS(INLINED)
   auto PAIR()::GetKey() noexcept -> Conditional<CT::Dense<K>, K&, K> {
      return mKey;
   }
   
   TEMPLATE() LANGULUS(ALWAYS_INLINED)
   auto PAIR()::GetKey() const noexcept -> const K& {
      return mKey;
   }

   /// Get contained key as a block                                           
   ///   @return the key block                                                
   TEMPLATE() LANGULUS(INLINED)
   Block<K> PAIR()::GetKeyBlock() noexcept {
      // Notice entry is missing, which means that memory entry will    
      // be sought if block is transferred to an owning one             
      return { DataState::Typed, MetaDataOf<Deref<K>>(), 1, &mKey };
   }
   
   TEMPLATE() LANGULUS(ALWAYS_INLINED)
   Block<K> PAIR()::GetKeyBlock() const noexcept {
      auto block = const_cast<PAIR()*>(this)->GetKeyBlock();
      block.MakeConst();
      return block;
   }

   /// Get contained key as a handle                                          
   ///   @return the handle                                                   
   TEMPLATE() LANGULUS(INLINED)
      Handle<K> PAIR()::GetKeyHandle() {
      if constexpr (CT::Sparse<K> and not CT::Reference<K>)
         return mKey.GetHandle();
      else
         return {&mKey};
   }

   TEMPLATE() LANGULUS(ALWAYS_INLINED)
      Handle<const K> PAIR()::GetKeyHandle() const {
      return const_cast<PAIR()*>(this)->GetKeyHandle().MakeConst();
   }

   /// Get contained value                                                    
   ///   @return the value                                                    
   TEMPLATE() LANGULUS(INLINED)
   auto PAIR()::GetValue() noexcept -> Conditional<CT::Dense<V>, V&, V> {
      return mValue;
   }

   TEMPLATE() LANGULUS(ALWAYS_INLINED)
   auto PAIR()::GetValue() const noexcept -> const V& {
      return mValue;
   }

   /// Get contained value as a block                                         
   ///   @return the value block                                              
   TEMPLATE() LANGULUS(INLINED)
   Block<V> PAIR()::GetValueBlock() noexcept {
      // Notice entry is missing, which means that memory entry will    
      // be sought if block is transferred to an owning one             
      return { DataState::Typed, MetaDataOf<Deref<V>>(), 1, &mValue };
   }

   TEMPLATE() LANGULUS(ALWAYS_INLINED)
   Block<V> PAIR()::GetValueBlock() const noexcept {
      auto block = const_cast<PAIR()*>(this)->GetValueBlock();
      block.MakeConst();
      return block;
   }

   /// Get contained value as a handle                                        
   ///   @return the handle                                                   
   TEMPLATE() LANGULUS(INLINED)
   Handle<V> PAIR()::GetValueHandle() {
      if constexpr (CT::Sparse<V> and not CT::Reference<V>)
         return mValue.GetHandle();
      else
         return {&mValue};
   }

   TEMPLATE() LANGULUS(ALWAYS_INLINED)
   Handle<const V> PAIR()::GetValueHandle() const {
      return const_cast<PAIR()*>(this)->GetValueHandle().MakeConst();
   }

   /// Clear anything contained, but don't release memory                     
   TEMPLATE() LANGULUS(INLINED)
   void PAIR()::Clear() {
      if constexpr (not CT::Reference<K> and requires { mKey.Clear(); })
         mKey.Clear();
      if constexpr (not CT::Reference<V> and requires { mValue.Clear(); })
         mValue.Clear();
   }

   /// Clear and release memory                                               
   TEMPLATE() LANGULUS(ALWAYS_INLINED)
   void PAIR()::Reset() {
      if constexpr (not CT::Reference<K> and requires { mKey.Reset(); })
         mKey.Reset();
      if constexpr (not CT::Reference<V> and requires { mValue.Reset(); })
         mValue.Reset();
   }

} // namespace Langulus::Annies

#undef TEMPLATE
#undef PAIR

