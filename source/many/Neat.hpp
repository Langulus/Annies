///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "TMany.hpp"
#include "Trait.hpp"
#include "Construct.hpp"
#include "../maps/TMap.hpp"
//#include <Langulus/Core/Sequences.hpp>


namespace Langulus::Annies
{

   ///                                                                        
   ///   Neat - a normalized data container                                   
   ///                                                                        
   ///   Turns messy containers into neatly and consistently orderless ones,  
   /// that are very fast on compare/search/insert/remove, albeit quite a bit 
   /// larger.                                                                
   ///   Neats are extensively used as descriptors in factories, to check     
   /// whether an element with the same signature already exists.             
   ///   Elements that are marked missing are never considered part of the    
   /// descriptor, and are filled by the context (i.e. Traits::Parent(?))     
   ///                                                                        
   class Neat {
   protected:
      template<class TYPE>
      friend struct Block;

      using TraitList     = TMany<Trait>;
      using ConstructList = TMany<Construct>;
      using TailList      = TMany<Messy>;

      // The hash of the container                                      
      // Kept as first member, in order to quickly access it            
      mutable Hash mHash;

      // Traits are ordered first by their trait type, then by their    
      // order of appearance. Duplicate trait types are allowed         
      // Trait contents may or may not also be normalized               
      TUnorderedMap<TMeta, TraitList> mTraits;

      // Subconstructs are sorted first by the construct type, and then 
      // by their order of appearance. Their contents may or may not    
      // also be normalized                                             
      TUnorderedMap<DMeta, ConstructList> mConstructs;

      // Any other block type that doesn't fit in the above is sorted   
      // first by the block type, then by the order of appearance       
      // These sub-blocks' contents may or may not be normalized        
      TUnorderedMap<DMeta, TailList> mAnythingElse;

   public:
      //LANGULUS(DEEP) true;
      static constexpr bool Ownership = true;
      static constexpr bool CTTI_Container = true;

      ///                                                                     
      ///   Construction                                                      
      ///                                                                     
      constexpr Neat() = default;
      Neat(const Neat&);
      Neat(Neat&&) noexcept;

      template<template<class> class S> requires CT::Intent<S<Neat>>
      Neat(S<Neat>&&);

      template<class T1, class...TN> requires CT::UnfoldInsertable<T1, TN...>
      Neat(T1&&, TN&&...);

      ///                                                                     
      ///   Assignment                                                        
      ///                                                                     
      Neat& operator = (const Neat&) = default;
      Neat& operator = (Neat&&) noexcept = default;

      template<template<class> class S> requires CT::Intent<S<Neat>>
      Neat& operator = (S<Neat>&&);

      ///                                                                     
      ///   Comparison                                                        
      ///                                                                     
      bool operator == (const Neat&) const;

      void Clear();
      void Reset();

      ///                                                                     
      ///   Encapsulation                                                     
      ///                                                                     
      Hash GetHash() const;
      bool IsEmpty() const noexcept;
      bool IsMissingDeep() const;
      bool IsExecutable() const noexcept;

      explicit operator bool() const noexcept;

      template<CT::Trait>
      auto GetTraits() -> TraitList*;

      template<CT::Trait>
      auto GetTraits()      const -> const TraitList*;
      auto GetTraits(TMeta)       ->       TraitList*;
      auto GetTraits(TMeta) const -> const TraitList*;

      template<CT::Data>
      auto GetData() -> TailList*;

      template<CT::Data>
      auto GetData()      const -> const TailList*;
      auto GetData(DMeta)       ->       TailList*;
      auto GetData(DMeta) const -> const TailList*;
      
      template<CT::Data>
      auto GetConstructs() -> ConstructList*;

      template<CT::Data>
      auto FindType()      const -> DMeta;
      auto FindType(DMeta) const -> DMeta;

      template<CT::Data>
      auto GetConstructs()      const -> const ConstructList*;
      auto GetConstructs(DMeta)       ->       ConstructList*;
      auto GetConstructs(DMeta) const -> const ConstructList*;

      template<CT::Trait>
      void SetDefaultTrait(CT::NotVoid auto&&);

      template<CT::Trait...>
      bool ExtractTrait(CT::NotVoid auto&...) const;
      auto ExtractData(CT::NotVoid auto&) const -> size_t;
      auto ExtractDataAs(CT::NotVoid auto&) const -> size_t;

      template<CT::Trait>
      auto GetTrait(size_t = 0)        const -> const Trait*;
      auto GetTrait(TMeta, size_t = 0) const -> const Trait*;

   protected:
      template<CT::Trait>
      bool ExtractTraitInner(CT::NotVoid auto&...) const;
      template<size_t...IDX>
      bool ExtractTraitInner(const TraitList&, ExpandedSequence<IDX...>, CT::NotVoid auto&...) const;
      template<size_t>
      bool ExtractTraitInnerInner(const TraitList&, CT::NotVoid auto&) const;

   public:
      ///                                                                     
      ///   Iteration                                                         
      ///                                                                     
      template<bool MUTABLE = true>
      size_t ForEach(auto&&...);
      size_t ForEach(auto&&...) const;

      template<bool MUTABLE = true>
      size_t ForEachDeep(auto&&...);
      size_t ForEachDeep(auto&&...) const;

      template<bool MUTABLE = true>
      size_t ForEachTrait(auto&&);
      size_t ForEachTrait(auto&&) const;

      template<bool MUTABLE = true>
      size_t ForEachConstruct(auto&&);
      size_t ForEachConstruct(auto&&) const;

      template<bool MUTABLE = true>
      size_t ForEachTail(auto&&);
      size_t ForEachTail(auto&&) const;

   protected:
      template<bool MUTABLE = true>
      size_t ForEachInner(auto&&);
      size_t ForEachInner(auto&&) const;

   public:
      ///                                                                     
      ///   Insertion                                                         
      ///                                                                     
      template<class T1, class...TN>
      size_t Insert(T1&&, TN&&...);
      void  Merge(const Neat&);
      Neat& SetTrait(CT::TraitBased auto&&, size_t = 0);

      Neat& operator <<  (auto&&);
      Neat& operator <<= (auto&&);

   protected:
      size_t UnfoldInsert(auto&&);
      void InsertInner(auto&&);

      void AddTrait(CT::Intent auto&&);
      void AddConstruct(CT::Intent auto&&);
      void AddVerb(CT::Intent auto&&);

   public:
      ///                                                                     
      ///   Removal                                                           
      ///                                                                     
      template<CT::Data, bool EMPTY_TOO = false>
      size_t RemoveData();
      template<CT::Data>
      size_t RemoveConstructs();
      template<CT::Trait, bool EMPTY_TOO = false>
      size_t RemoveTrait();

      ///                                                                     
      ///   Conversion                                                        
      ///                                                                     
      size_t Serialize(CT::Serial auto&) const;
   };

} // namespace Langulus::Annies
