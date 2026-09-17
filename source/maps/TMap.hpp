///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Map.hpp"


namespace Langulus::CT
{

   /// Concept for recognizing arguments, with which a statically typed       
   /// map can be constructed                                                 
   template<class K, class V, class...A>
   concept DeepMapMakable = UnfoldMakableFrom<Annies::TPair<K, V>, A...>
        or (sizeof...(A) == 1
           and Map<Deint<FirstOf<A...>>> and (IntentOf<FirstOf<A...>>::Shallow
            or IntentMakableAlt<
              typename IntentOf<FirstOf<A...>>::template As<Annies::TPair<K, V>>>
        ));

   /// Concept for recognizing argument, with which a statically typed        
   /// map can be assigned                                                    
   template<class K, class V, class A>
   concept DeepMapAssignable = UnfoldMakableFrom<Annies::TPair<K, V>, A>
        or (Map<Deint<A>> and (IntentOf<A>::Shallow
           or IntentAssignableAlt<
             typename IntentOf<A>::template As<Annies::TPair<K, V>>>));

} // namespace Langulus::CT

namespace Langulus::Annies
{

   ///                                                                        
   /// A hashmap implementation, using the Robin Hood algorithm               
   ///                                                                        
   template<CT::NotVoid K, CT::NotVoid V, bool ORDERED>
   struct TMap : Map<ORDERED> {
      using Key = K;
      using Value = V;
      using Base = Map<ORDERED>;
      using Self = TMap<K, V, ORDERED>;
      using Pair = TPair<K, V>;
      using PairRef = TPair<const K&, V&>;
      using PairConstRef = TPair<const K&, const V&>;

      LANGULUS(POD)       false;
      LANGULUS(TYPED)     Pair;
      LANGULUS_BASES(Map<ORDERED>);

   protected:
      static_assert(CT::Comparable<K, K>,
         "Map's key type must be equality-comparable to itself");

      friend struct BlockMap;
      using typename Base::InfoType;
      using Base::InvalidOffset;
      using Base::mInfo;
      using Base::mKeys;
      using Base::mValues;

   public:
      ///                                                                     
      ///   Construction & Assignment                                         
      ///                                                                     
      constexpr TMap();
      TMap(const TMap&);
      TMap(TMap&&) noexcept;

      template<class T1, class...TN>
      requires CT::DeepMapMakable<K, V, T1, TN...>
      TMap(T1&&, TN&&...);

      ~TMap();

      auto operator = (const TMap&) -> TMap&;
      auto operator = (TMap&&) -> TMap&;

      template<class T1> requires CT::DeepMapAssignable<K, V, T1>
      auto operator = (T1&&) -> TMap&;

      auto BranchOut() -> TMap&;

      ///                                                                     
      ///   Capsulation                                                       
      ///                                                                     
      DMeta GetKeyType() const noexcept;
      DMeta GetValueType() const noexcept;
      constexpr bool IsKeyTyped() const noexcept;
      constexpr bool IsValueTyped() const noexcept;
      constexpr bool IsKeyUntyped() const noexcept;
      constexpr bool IsValueUntyped() const noexcept;
      constexpr bool IsKeyTypeConstrained() const noexcept;
      constexpr bool IsValueTypeConstrained() const noexcept;
      constexpr bool IsKeyDeep() const noexcept;
      constexpr bool IsValueDeep() const noexcept;
      constexpr bool IsKeySparse() const noexcept;
      constexpr bool IsValueSparse() const noexcept;
      constexpr bool IsKeyDense() const noexcept;
      constexpr bool IsValueDense() const noexcept;
      constexpr Size GetKeyStride() const noexcept;
      constexpr Size GetValueStride() const noexcept;
      Count GetKeyCountDeep() const noexcept;
      Count GetKeyCountElementsDeep() const noexcept;
      Count GetValueCountDeep() const noexcept;
      Count GetValueCountElementsDeep() const noexcept;

      bool IsKeyMissingDeep() const;
      bool IsValueMissingDeep() const;

      bool IsKeyExecutable() const;
      bool IsValueExecutable() const;
      bool IsKeyExecutableDeep() const;
      bool IsValueExecutableDeep() const;

      using Base::GetCount;
      using Base::GetReserved;
      using Base::GetInfo;
      using Base::GetInfoEnd;
      using Base::IsEmpty;

      ///                                                                     
      ///   RTTI                                                              
      ///                                                                     
      template<CT::Data, CT::NotVoid...>
      constexpr bool IsKey() const noexcept;
      bool IsKey(DMeta) const noexcept;

      template<CT::Data, CT::NotVoid...>
      constexpr bool IsKeySimilar() const noexcept;
      bool IsKeySimilar(DMeta) const noexcept;

      template<CT::Data, CT::NotVoid...>
      constexpr bool IsKeyExact() const noexcept;
      bool IsKeyExact(DMeta) const noexcept;

      template<CT::Data, CT::NotVoid...>
      constexpr bool IsValue() const noexcept;
      bool IsValue(DMeta) const noexcept;

      template<CT::Data, CT::NotVoid...>
      constexpr bool IsValueSimilar() const noexcept;
      bool IsValueSimilar(DMeta) const noexcept;

      template<CT::Data, CT::NotVoid...>
      constexpr bool IsValueExact() const noexcept;
      bool IsValueExact(DMeta) const noexcept;

   protected:
      template<CT::NoIntent, CT::NoIntent>
      void Mutate() noexcept;
      void Mutate(DMeta, DMeta);

   public:
      ///                                                                     
      ///   Indexing                                                          
      ///                                                                     
      auto GetKey  (CT::Index auto)       -> K&;
      auto GetKey  (CT::Index auto) const -> K const&;
      auto GetValue(CT::Index auto)       -> V&;
      auto GetValue(CT::Index auto) const -> V const&;
      auto GetPair (CT::Index auto)       -> PairRef;
      auto GetPair (CT::Index auto) const -> PairConstRef;

   protected:
      using Base::GetBucket;
      using Base::GetBucketUnknown;

   public:
      ///                                                                     
      ///   Iteration                                                         
      ///                                                                     
      using Iterator = BlockMap::Iterator<TMap>;
      using ConstIterator = BlockMap::Iterator<const TMap>;

      auto begin()       noexcept -> Iterator;
      auto begin() const noexcept -> ConstIterator;
      auto last()       noexcept -> Iterator;
      auto last() const noexcept -> ConstIterator;

      template<bool REVERSE = false>
      Count ForEach(auto&&) const;
      template<bool REVERSE = false>
      Count ForEach(auto&&);

      template<bool REVERSE = false>
      Count ForEachKeyElement(auto&&) const;
      template<bool REVERSE = false>
      Count ForEachKeyElement(auto&&);

      template<bool REVERSE = false>
      Count ForEachValueElement(auto&&) const;
      template<bool REVERSE = false>
      Count ForEachValueElement(auto&&);

      template<bool REVERSE = false>
      Count ForEachKey(auto&&...) const;
      template<bool REVERSE = false>
      Count ForEachKey(auto&&...);

      template<bool REVERSE = false>
      Count ForEachValue(auto&&...) const;
      template<bool REVERSE = false>
      Count ForEachValue(auto&&...);

      template<bool REVERSE = false, bool SKIP = true>
      Count ForEachKeyDeep(auto&&...) const;
      template<bool REVERSE = false, bool SKIP = true>
      Count ForEachKeyDeep(auto&&...);

      template<bool REVERSE = false, bool SKIP = true>
      Count ForEachValueDeep(auto&&...) const;
      template<bool REVERSE = false, bool SKIP = true>
      Count ForEachValueDeep(auto&&...);

      ///                                                                     
      ///   Comparison                                                        
      ///                                                                     
      bool operator == (CT::Map  auto const&) const requires CT::Comparable<V, V>;
      bool operator == (CT::Pair auto const&) const requires CT::Comparable<V, V>;

      Hash GetHash() const requires CT::Hashable<K, V>;

      template<CT::NoIntent K1> requires CT::Comparable<K, K1>
      bool ContainsKey(K1 const&) const;

      template<CT::NoIntent V1> requires CT::Comparable<V, V1>
      bool ContainsValue(V1 const&) const;

      template<CT::Pair P> requires CT::Comparable<TPair<K, V>, P>
      bool ContainsPair(P const&) const;

      template<CT::NoIntent K1> requires CT::Comparable<K, K1>
      auto Find(K1 const&) const -> Index;

      template<CT::NoIntent K1> requires CT::Comparable<K, K1>
      auto FindIt(K1 const&) -> Iterator;

      template<CT::NoIntent K1> requires CT::Comparable<K, K1>
      auto FindIt(K1 const&) const -> ConstIterator;

      template<CT::NoIntent K1> requires CT::Comparable<K, K1>
      decltype(auto) At(K1 const&);

      template<CT::NoIntent K1> requires CT::Comparable<K, K1>
      decltype(auto) At(K1 const&) const;

      template<CT::NoIntent K1> requires CT::Comparable<K, K1>
      decltype(auto) operator[] (K1 const&);

      template<CT::NoIntent K1> requires CT::Comparable<K, K1>
      decltype(auto) operator[] (K1 const&) const;

      ///                                                                     
      ///   Memory management                                                 
      ///                                                                     
      void Reserve(Count);

      ///                                                                     
      ///   Insertion                                                         
      ///                                                                     
      template<class K1, class V1>
      requires (CT::MakableFrom<K, K1> and CT::MakableFrom<V, V1>)
      Count Insert(K1&&, V1&&);

      template<class K1>
      requires (CT::MakableFrom<K, K1> and CT::Defaultable<V>)
      Count Insert(K1&&);

      Count InsertBlock(auto&&, auto&&);

      template<class T1, class...TN>
      requires CT::UnfoldMakableFrom<TPair<K, V>, T1, TN...>
      Count InsertPair(T1&&, TN&&...);

      template<class T1>
      requires CT::UnfoldMakableFrom<TPair<K, V>, T1>
      auto operator << (T1&&) -> TMap&;

      template<class T1>
      requires CT::UnfoldMakableFrom<TPair<K, V>, T1>
      auto operator >> (T1&&) -> TMap&;

      auto operator += (const TMap&) -> TMap&;

      ///                                                                     
      ///   Removal                                                           
      ///                                                                     
      template<CT::NoIntent K1> requires CT::Comparable<K, K1>
      auto RemoveKey(const K1&) -> Count;
      template<CT::NoIntent V1> requires CT::Comparable<V, V1>
      auto RemoveValue(const V1&) -> Count;
      template<CT::Pair P> requires CT::Comparable<TPair<K, V>, P>
      auto RemovePair(const P&) -> Count;

      auto RemoveIt(const Iterator&) -> Iterator;

      void Clear();
      void Reset();
      void Compact();

   protected:
      static Size RequestValuesSize(Count) noexcept;
   };

} // namespace Langulus::Annies
