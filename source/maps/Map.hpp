///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../blocks/BlockMap.hpp"
#include "../pairs/TPair.hpp"


namespace Langulus::Annies
{

   ///                                                                        
   ///   Type-erased map                                                      
   ///                                                                        
   template<bool ORDERED = false>
   struct Map : BlockMap {
      LANGULUS(POD) false;
      LANGULUS(ACT_AS) Map;
      LANGULUS_BASES(BlockMap);

      friend struct BlockMap;
      static constexpr bool Ownership = true;
      static constexpr bool Ordered = ORDERED;

      using Key = void;
      using Value = void;
      using Self = Map<ORDERED>;
      using Pair = Annies::Pair;
      using PairRef = Pair;
      using PairConstRef = Pair;

      ///                                                                     
      ///   Construction                                                      
      ///                                                                     
      constexpr Map() noexcept = default;
      Map(const Map&);
      Map(Map&&);

      template<class T1, class...TN>
      requires CT::UnfoldInsertable<T1, TN...>
      Map(T1&&, TN&&...);

      ~Map();

      ///                                                                     
      ///   Assignment                                                        
      ///                                                                     
      auto operator = (const Map&) -> Map&;
      auto operator = (Map&&) -> Map&;
      auto operator = (CT::UnfoldInsertable auto&&) -> Map&;

      ///                                                                     
      ///   Indexing                                                          
      ///                                                                     
      auto GetKey  (CT::Index auto)       -> Block<>;
      auto GetKey  (CT::Index auto) const -> Block<>;
      auto GetValue(CT::Index auto)       -> Block<>;
      auto GetValue(CT::Index auto) const -> Block<>;
      auto GetPair (CT::Index auto)       -> Pair;
      auto GetPair (CT::Index auto) const -> Pair;

      ///                                                                     
      ///   Iteration                                                         
      ///                                                                     
      using Iterator      = BlockMap::Iterator<Map>;
      using ConstIterator = BlockMap::Iterator<const Map>;

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

      ///                                                                     
      ///   Comparison                                                        
      ///                                                                     
      using BlockMap::operator ==;

      auto Find  (const CT::NoIntent auto&) const -> Index;
      auto FindIt(const CT::NoIntent auto&) -> Iterator;
      auto FindIt(const CT::NoIntent auto&) const -> ConstIterator;

      decltype(auto) At(const CT::NoIntent auto&);
      decltype(auto) At(const CT::NoIntent auto&) const;

      decltype(auto) operator[] (const CT::NoIntent auto&);
      decltype(auto) operator[] (const CT::NoIntent auto&) const;

      ///                                                                     
      ///   Memory management                                                 
      ///                                                                     
      void Reserve(Count);

      ///                                                                     
      ///   Insertion                                                         
      ///                                                                     
      Count Insert(auto&&, auto&&);

      template<class T1, class T2> requires CT::Block<Deint<T1>, Deint<T2>>
      Count InsertBlock(T1&&, T2&&);

      template<class T1, class...TN>
      Count InsertPair(T1&&, TN&&...);

      auto operator << (CT::UnfoldInsertable auto&&) -> Map&;
      auto operator >> (CT::UnfoldInsertable auto&&) -> Map&;

      ///                                                                     
      ///   Removal                                                           
      ///                                                                     
      auto RemoveKey(const CT::NoIntent auto&) -> Count;
      auto RemoveValue(const CT::NoIntent auto&) -> Count;
      auto RemovePair(const CT::Pair auto&) -> Count;
      auto RemoveIt(const Iterator&) -> Iterator;

      void Clear();
      void Reset();
      void Compact();
   };

} // namespace Langulus::Annies
