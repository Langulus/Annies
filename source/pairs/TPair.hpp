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
   /*template<class K, class V, class A>
   concept PairMakable = Pair<Deint<A>> and NotReference<K, V>
       and (IntentOf(LglsFake(A))::Shallow or (
            IntentConstructible<IntentOf(LglsFake(A)), K>
        and IntentConstructible<IntentOf(LglsFake(A)), V>);

   /// Concept for recognizing argument, with which a statically typed        
   /// pair can be assigned                                                   
   template<class K, class V, class A>
   concept PairAssignable = Pair<Deint<A>> and NotReference<K, V>
       and (IntentOf(LglsFake(A))::Shallow or (
            IntentAssignable<typename IntentOf<A>::template As<K>>
        and IntentAssignable<typename IntentOf<A>::template As<V>>));*/

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
      using CTTI_Abstract = No;
      using CTTI_Typed = TPair<K, V>;

   private:
      std::conditional_t<CT::Reference<K> or CT::Dense<K>, K, Ref<Deptr<K>>> mKey;
      std::conditional_t<CT::Reference<V> or CT::Dense<V>, V, Ref<Deptr<V>>> mValue;

   public:
      ///                                                                     
      ///   Construction & Assignment                                         
      ///                                                                     
      TPair() = default;
      TPair(TPair const&) = default;
      TPair(TPair&&) = default;

      template<CT::Pair P> //requires CT::PairMakable<K, V, P>
      TPair(P&&);

      template<class K1, class V1>
      requires (std::constructible_from<K, K1> and std::constructible_from<V, V1>
           and  CT::NotReference<K, V>)
      TPair(K1&&, V1&&);

      TPair(K&&, V&&) noexcept requires CT::Reference<K, V>;

      TPair& operator = (TPair const&) = default;
      TPair& operator = (TPair&&) = default;
      template<CT::Pair P> //requires CT::PairAssignable<K, V, P>
      TPair& operator = (P&&);

      ///                                                                     
      ///   Capsulation                                                       
      ///                                                                     
      Hash GetHash() const requires CT::Hashable<K, V>;

      auto GetKey()         const noexcept -> const K&;
      auto GetKey()               noexcept -> std::conditional_t<CT::Dense<K>, K&, K>;
      auto GetKeyBlock()    const noexcept -> Block<K>;
      auto GetKeyBlock()          noexcept -> Block<K>;
      auto GetKeyHandle()         -> Handle<K>;
      auto GetKeyHandle()   const -> Handle<const K>;

      auto GetValue()       const noexcept -> const V&;
      auto GetValue()             noexcept -> std::conditional_t<CT::Dense<V>, V&, V>;
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
