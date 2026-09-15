///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Pair.hpp"
#include "../one/Own.hpp"
#include "../one/Ref.hpp"


namespace Langulus::CT
{

   /// Concept for recognizing arguments, with which a statically typed       
   /// pair can be constructed                                                
   template<class K, class V, class A>
   concept PairMakable = Pair<Deint<A>> and NotReference<K, V>
       and (IntentOf<A>::Shallow or (
            IntentMakableAlt<typename IntentOf<A>::template As<K>>
        and IntentMakableAlt<typename IntentOf<A>::template As<V>>));

   /// Concept for recognizing argument, with which a statically typed        
   /// pair can be assigned                                                   
   template<class K, class V, class A>
   concept PairAssignable = Pair<Deint<A>> and NotReference<K, V>
       and (IntentOf<A>::Shallow or (
            IntentAssignableAlt<typename IntentOf<A>::template As<K>>
        and IntentAssignableAlt<typename IntentOf<A>::template As<V>>));

   /// Concept for recognizing argument, against which a pair can be compared 
   template<class K, class V, class A>
   concept PairComparable = Pair<A>
       and Comparable<K, typename A::Key>
       and Comparable<V, typename A::Value>;

} // namespace Langulus::CT

namespace Langulus::Annies
{

   ///                                                                        
   ///   A helper structure for pairing keys and values of any type           
   ///                                                                        
   ///   This is the statically typed pair, and it can be used with           
   /// references, as well as dense or sparse values. When key or value types 
   /// are references, the TPair acts as a simple intermediate type, often    
   /// used to access elements inside maps.                                   
   ///   @attention TPair is not binary-compatible with its type-erased       
   ///      counterpart Pair                                                  
   ///                                                                        
   template<class K, class V>
   struct TPair : A::Pair {
      using Key = K;
      using Value = V;

      LANGULUS_ABSTRACT() false;
      LANGULUS(TYPED) TPair<K, V>;

   private:
      Conditional<CT::Reference<K> or CT::Dense<K>, K, Ref<Deptr<K>>> mKey;
      Conditional<CT::Reference<V> or CT::Dense<V>, V, Ref<Deptr<V>>> mValue;

   public:
      ///                                                                     
      ///   Construction & Assignment                                         
      ///                                                                     
      TPair() = default;
      TPair(TPair const&) = default;
      TPair(TPair&&) = default;

      template<class P> requires CT::PairMakable<K, V, P>
      TPair(P&&);

      template<class K1, class V1>
      requires (CT::MakableFrom<K, K1> and CT::MakableFrom<V, V1>
           and  CT::NotReference<K, V>)
      TPair(K1&&, V1&&);

      TPair(K&&, V&&) noexcept requires CT::Reference<K, V>;

      TPair& operator = (TPair const&) = default;
      TPair& operator = (TPair&&) = default;
      template<class P> requires CT::PairAssignable<K, V, P>
      TPair& operator = (P&&);

      ///                                                                     
      ///   Capsulation                                                       
      ///                                                                     
      Hash GetHash() const requires CT::Hashable<K, V>;

      auto GetKey()         const noexcept -> const K&;
      auto GetKey()               noexcept -> Conditional<CT::Dense<K>, K&, K>;
      auto GetKeyBlock()    const noexcept -> Block<K>;
      auto GetKeyBlock()          noexcept -> Block<K>;
      auto GetKeyHandle()         -> Handle<K>;
      auto GetKeyHandle()   const -> Handle<const K>;

      auto GetValue()       const noexcept -> const V&;
      auto GetValue()             noexcept -> Conditional<CT::Dense<V>, V&, V>;
      auto GetValueBlock()  const noexcept -> Block<V>;
      auto GetValueBlock()        noexcept -> Block<V>;
      auto GetValueHandle()       -> Handle<V>;
      auto GetValueHandle() const -> Handle<const V>;

      ///                                                                     
      ///   Comparison                                                        
      ///                                                                     
      template<class P> requires CT::PairComparable<K, V, P>
      bool operator == (const P&) const;

      operator TPair<const Deref<K>&, const Deref<V>&>() const noexcept
      requires CT::Reference<K, V>;

      ///                                                                     
      ///   Removal                                                           
      ///                                                                     
      void Clear();
      void Reset();
   };

   /// Deduction guides                                                       
   template<class K, class V>
   TPair(K&&, V&&) -> TPair<Deref<K>, Deref<V>>;

} // namespace Langulus::Annies
