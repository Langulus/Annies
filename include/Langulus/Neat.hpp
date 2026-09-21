///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Many.hpp"
#include "TMany.hpp"
#include "TMap.hpp"
#include "Recipe.hpp"
#include "Tag.hpp"


namespace Langulus::Annies
{
   using Messy = Many;

   ///                                                                        
   /// MARK: Neat                                                             
   ///                                                                        
   ///   Normalized data container. Turns messy containers into neatly and    
   /// consistently ordered ones, that are very fast on compare, search,      
   /// insert, or remove, albeit quite a bit larger.                          
   ///   Neats are extensively used as descriptors in factories, to check     
   /// whether an element with the same signature already exists.             
   ///   Elements that are marked missing are never considered part of the    
   /// recipe and are filled by the context (i.e. Tags::Parent(?)).           
   struct Neat {
      using RecipeList = TMany<Recipe>;
      using TailList   = TMany<Messy>;
      using TagList    = TMany<Tag>;

      // The hash of the container.                                     
      // Kept as first member, in order to quickly access it.           
      mutable Hash mHash;

      // Tags are ordered first by their tag type, then by their        
      // order of appearance. Duplicate tag types are allowed.          
      // Tag contents may or may not also be normalized.                
      TMapUnsorted<TMeta, TagList> mTags;

      // Subconstructs are sorted first by the recipe subject, and then 
      // by their order of appearance. Their contents may or may not    
      // also be normalized.                                            
      TMapUnsorted<DMeta, RecipeList> mRecipes;

      // Any other container that doesn't fit in the above is sorted    
      // first by its type, then by the order of appearance.            
      // Contents may or may not be normalized.                         
      TMapUnsorted<DMeta, TailList> mAnythingElse;

   public:
      using CTTI_Container = Yup;

      ///                                                                     
      ///   Construction                                                      
      constexpr Neat() noexcept = default;
      constexpr Neat(const Neat&) noexcept = default;
      constexpr Neat(Neat&&) noexcept = default;

      template<template<class> class S> requires CT::Intent<S<Neat>>
      Neat(S<Neat>&&);

      /*template<class A1, class...AN>
      Neat(A1&&, AN&&...) requires CT::RangeInsertable<Many, A1, AN...>;*/

      ///                                                                     
      ///   Assignment                                                        
      Neat& operator = (const Neat&) = default;
      Neat& operator = (Neat&&) noexcept = default;

      template<template<class> class S> requires CT::Intent<S<Neat>>
      Neat& operator = (S<Neat>&&);

      ///                                                                     
      ///   Comparison                                                        
      bool operator == (const Neat&) const;

      void Clear();
      void Reset();

      ///                                                                     
      ///   Encapsulation                                                     
      Hash GetHash() const;
      bool IsEmpty() const noexcept;
      bool IsMissingDeep() const;
      bool IsExecutable() const noexcept;

      explicit operator bool() const noexcept;

      template<CT::DefineTag>
      auto GetTags()      -> TagList*;
      auto GetTags(TMeta) -> TagList*;

      template<CT::NotVoid>
      auto GetData()      -> TailList*;
      auto GetData(DMeta) -> TailList*;
      
      template<CT::NotVoid>
      auto GetRecipes()      -> RecipeList*;
      auto GetRecipes(DMeta) -> RecipeList*;

      template<CT::NotVoid>
      auto FindType()      const -> DMeta;
      auto FindType(DMeta) const -> DMeta;

      template<CT::DefineTag>
      void SetDefaultTag (CT::NotVoid auto&&);

      template<CT::DefineTag...>
      bool ExtractTag    (CT::NotVoid auto&...) const;
      auto ExtractData   (CT::NotVoid auto&) const -> size_t;
      auto ExtractDataAs (CT::NotVoid auto&) const -> size_t;

      template<CT::DefineTag>
      auto GetTag(size_t = 0)        const -> const Tag*;
      auto GetTag(TMeta, size_t = 0) const -> const Tag*;

   protected:
      template<CT::DefineTag>
      bool ExtractTagInner(CT::NotVoid auto&...) const;
      template<size_t...IDX>
      bool ExtractTagInner(const TagList&, ExpandedSequence<IDX...>, CT::NotVoid auto&...) const;
      template<size_t>
      bool ExtractTagInnerInner(const TagList&, CT::NotVoid auto&) const;

   public:
      ///                                                                     
      ///   Iteration                                                         
      size_t ForEach          (auto&&...);
      size_t ForEachTag       (auto&&);
      size_t ForEachDeep      (auto&&...);
      size_t ForEachConstruct (auto&&);
      size_t ForEachTail      (auto&&);

   protected:
      size_t ForEachInner     (auto&&);

   public:
      ///                                                                     
      ///   Insertion                                                         
      template<class T1, class...TN>
      size_t Insert(T1&&, TN&&...);
      void  Merge(const Neat&);
      Neat& SetTag(CT::DefineTag auto&&, size_t = 0);

      Neat& operator <<  (auto&&);
      Neat& operator <<= (auto&&);

   protected:
      auto UnfoldInsert (auto&&) -> size_t;
      void InsertInner  (auto&&);

      void AddTag       (CT::Intent auto&&);
      void AddConstruct (CT::Intent auto&&);
      void AddVerb      (CT::Intent auto&&);

   public:
      ///                                                                     
      ///   Removal                                                           
      template<CT::NotVoid, bool EMPTY_TOO = false>
      size_t RemoveData();
      template<CT::NotVoid>
      size_t RemoveConstructs();
      template<CT::DefineTag, bool EMPTY_TOO = false>
      size_t RemoveTag();
   };
}

namespace Langulus
{
   using Annies::Neat;
}