///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Pair.hpp"


namespace Langulus::Annies
{

   /// Construct pair from any other kind of pair                             
   ///   @param other - the pair to construct with                            
   template<class P> requires CT::Pair<Deint<P>> LANGULUS(INLINED)
   Pair::Pair(P&& other)
      : mKey   {IntentOf<decltype(other)>::Nest(DeintCast(other).mKey)}
      , mValue {IntentOf<decltype(other)>::Nest(DeintCast(other).mValue)} {}

   /// Construct pair manually                                                
   ///   @param key - the key                                                 
   ///   @param value - the value                                             
   template<class K, class V>
   requires CT::UnfoldInsertable<K, V> LANGULUS(INLINED)
   Pair::Pair(K&& key, V&& val)
      : mKey   {Many::Wrap(Forward<K>(key))}
      , mValue {Many::Wrap(Forward<V>(val))} {}

   /// Assign any kind of pair                                                
   ///   @param rhs - the pair to assign                                      
   template<class P> requires CT::Pair<Deint<P>> LANGULUS(INLINED)
   Pair& Pair::operator = (P&& rhs) {
      using S = IntentOf<decltype(rhs)>;
      mKey   = Many::Wrap(S::Nest(DeintCast(rhs).mKey));
      mValue = Many::Wrap(S::Nest(DeintCast(rhs).mValue));
      return *this;
   }

   /// Get the pair's hash                                                    
   ///   @attention hash is not cached, so this function is slow              
   ///   @return the hash                                                     
   LANGULUS(INLINED)
   Hash Pair::GetHash() const {
      return HashOf(mKey, mValue);
   }

   /// Get the contained key                                                  
   ///   @return a reference to the contained key                             
   LANGULUS(INLINED)
   Many const& Pair::GetKey() const noexcept {
      return mKey;
   }

   LANGULUS(INLINED)
   Many& Pair::GetKey() noexcept {
      return mKey;
   }

   /// Get the contained key                                                  
   ///   @return a reference to the contained key                             
   LANGULUS(INLINED)
   Many const& Pair::GetKeyBlock() const noexcept {
      return mKey;
   }

   LANGULUS(INLINED)
   Many& Pair::GetKeyBlock() noexcept {
      return mKey;
   }

   /// Get the contained value                                                
   ///   @return a reference to the contained key                             
   LANGULUS(INLINED)
   Many const& Pair::GetValue() const noexcept {
      return mValue;
   }

   LANGULUS(INLINED)
   Many& Pair::GetValue() noexcept {
      return mValue;
   }

   /// Get the contained value                                                
   ///   @return a reference to the contained key                             
   LANGULUS(INLINED)
   Many const& Pair::GetValueBlock() const noexcept {
      return mValue;
   }

   LANGULUS(INLINED)
   Many& Pair::GetValueBlock() noexcept {
      return mValue;
   }

   LANGULUS(INLINED)
   bool Pair::operator == (CT::Pair auto const& rhs) const {
      return mKey == rhs.mKey and mValue == rhs.mValue;
   }

   /// Clear anything contained, but don't release memory                     
   LANGULUS(INLINED)
   void Pair::Clear() {
      mKey.Clear();
      mValue.Clear();
   }

   /// Clear and release memory                                               
   LANGULUS(INLINED)
   void Pair::Reset() {
      mKey.Reset();
      mValue.Reset();
   }

} // namespace Langulus::Annies
