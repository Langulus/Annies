///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../blocks/BlockSet.hpp"


namespace Langulus::Annies
{

   ///                                                                        
   ///   Type-erased set                                                      
   ///                                                                        
   template<bool ORDERED = false>
   struct Set : BlockSet {
      LANGULUS(POD) false;
      LANGULUS(ACT_AS) Set;
      LANGULUS_BASES(BlockSet);

      using BlockType = Many;

      static constexpr bool Ownership = true;
      static constexpr bool Ordered = ORDERED;

      ///                                                                     
      ///   Construction                                                      
      ///                                                                     
      constexpr Set() noexcept = default;
      Set(const Set&);
      Set(Set&&);

      template<class T1, class...TAIL>
      requires CT::UnfoldInsertable<T1, TAIL...>
      Set(T1&&, TAIL&&...);

      ~Set();

      ///                                                                     
      ///   Assignment                                                        
      ///                                                                     
      Set& operator = (const Set&);
      Set& operator = (Set&&);
      Set& operator = (CT::UnfoldInsertable auto&&);

      ///                                                                     
      ///   Indexing                                                          
      ///                                                                     
      decltype(auto) Get(CT::Index auto) const;

      decltype(auto) operator[] (CT::Index auto) const;

      ///                                                                     
      ///   Iteration                                                         
      ///                                                                     
      using Iterator = BlockSet::Iterator<Set>;
      using ConstIterator = BlockSet::Iterator<const Set>;

      auto begin()       noexcept -> Iterator;
      auto begin() const noexcept -> ConstIterator;
      auto last()        noexcept -> Iterator;
      auto last()  const noexcept -> ConstIterator;

      template<bool REVERSE = false>
      size_t ForEach(auto&&...);
      template<bool REVERSE = false>
      size_t ForEach(auto&&...) const;

      template<bool REVERSE = false>
      size_t ForEachElement(auto&&);
      template<bool REVERSE = false>
      size_t ForEachElement(auto&&) const;
      
      template<bool REVERSE = false, bool SKIP = true>
      size_t ForEachDeep(auto&&...);
      template<bool REVERSE = false, bool SKIP = true>
      size_t ForEachDeep(auto&&...) const;

      ///                                                                     
      ///   RTTI                                                              
      ///                                                                     
      template<CT::Data, CT::NotVoid...>
      constexpr bool Is() const noexcept;
      bool Is(DMeta) const noexcept;

      template<CT::Data, CT::NotVoid...>
      constexpr bool IsSimilar() const noexcept;
      bool IsSimilar(DMeta) const noexcept;

      template<CT::Data, CT::NotVoid...>
      constexpr bool IsExact() const noexcept;
      bool IsExact(DMeta) const noexcept;

      ///                                                                     
      ///   Comparison                                                        
      ///                                                                     
      using BlockSet::operator ==;

      auto Find  (const CT::NoIntent auto&) const -> Index;
      auto FindIt(const CT::NoIntent auto&)       -> Iterator;
      auto FindIt(const CT::NoIntent auto&) const -> ConstIterator;

      ///                                                                     
      ///   Memory management                                                 
      ///                                                                     
      void Reserve(size_t);

      ///                                                                     
      ///   Insertion                                                         
      ///                                                                     
      Set& operator << (CT::UnfoldInsertable auto&&);
      Set& operator >> (CT::UnfoldInsertable auto&&);
   };

} // namespace Langulus::Annies
