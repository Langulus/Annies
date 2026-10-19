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
#include "TTag.hpp"


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
      using CTTI_Container = Yup;

      using RecipeList = TMany<Recipe>;
      using TailList   = TMany<Messy>;
      using TagList    = TMany<Tag>;

   private:
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
      ///                                                                     
      /// MARK:Construction                                                   
      constexpr Neat() noexcept = default;
      constexpr Neat(Neat const&) noexcept = default;
      constexpr Neat(Neat&&) noexcept = default;

      /// Construct via intent                                                
      template<template<class> class I> requires CT::Intent<I<Neat>>
      Neat(I<Neat>&& other)
         : mHash         {other->mHash}
         , mTags         {I {other->mTags}}
         , mRecipes      {I {other->mRecipes}}
         , mAnythingElse {I {other->mAnythingElse}} {
         if constexpr (I<Neat>::ResetsOnMove())
            other->mHash = {};
      }

      /// Construct by tidying up some data                                   
      template<class A1, class...AN>
      Neat(A1&& a1, AN&&...an) : Neat {} {
         Insert(LglsFwd(a1));
        (Insert(LglsFwd(an)), ...);
      }

      ///                                                                     
      /// MARK:Assignment                                                     
      Neat& operator = (Neat const&) = default;
      Neat& operator = (Neat&&) noexcept = default;

      /// Assign via intent                                                   
      template<template<class> class I> requires CT::Intent<I<Neat>>
      Neat& operator = (I<Neat>&& other) {
         mTags         = I {other->mTags};
         mRecipes      = I {other->mRecipes};
         mAnythingElse = I {other->mAnythingElse};
         mHash         = other->mHash;

         if constexpr (I<Neat>::ResetsOnMove())
            other->mHash = {};
         return *this;
      }

      ///                                                                     
      /// MARK:Comparison                                                     
      /// Compare neat container                                              
      ///   @param rhs the container to compare with                          
      ///   @return true if descriptors match                                 
      bool operator == (Neat const& rhs) const {
         if (GetHash() != rhs.GetHash())
            return false;

         return mTags == rhs.mTags
            and mRecipes == rhs.mRecipes
            and mAnythingElse == rhs.mAnythingElse;
      }

      /// Clear the container without deallocating                            
      void Clear() {
         mHash = {};
         mTags.Clear();
         mRecipes.Clear();
         mAnythingElse.Clear();
      }

      /// Clear and deallocate the container                                  
      void Reset() {
         mHash = {};
         mTags.Reset();
         mRecipes.Reset();
         mAnythingElse.Reset();
      }

      ///                                                                     
      /// MARK: Encapsulation                                                 
      /// Get the hash of a neat container (and cache it)                     
      ///   @attention missing scopes never participate in hashing/comparison 
      ///   @return the hash                                                  
      Hash GetHash() const {
         if (mHash)
            return mHash;

         if (IsEmpty())
            mHash = 1;
         else
            mHash = HashOf(mTags, mRecipes, mAnythingElse);//TODO we can just push/pop xor these, as order doesn't matter here
         return mHash;
      }

      /// Check if the container is empty                                     
      ///   @return true if empty                                             
      bool IsEmpty() const noexcept {
         return mTags.IsEmpty()
            and mRecipes.IsEmpty()
            and mAnythingElse.IsEmpty();
      }

      /// Check if the container is not empty                                 
      ///   @return true if not empty                                         
      explicit constexpr operator bool() const noexcept {
         return not IsEmpty();
      }

      /// Check if the container has missing scopes, nested                   
      ///   @return true if there's at least one missing entry                
      bool IsMissingDeep() const {
         return mTags.IsMissingDeep()
             or mRecipes.IsMissingDeep()
             or mAnythingElse.IsMissingDeep();
      }

      /// Check if the container has executable items, nested                 
      ///   @return true if there's at least one executable entry             
      bool IsExecutable() const noexcept {
         return mTags.IsExecutable()
             or mRecipes.IsExecutable()
             or mAnythingElse.IsExecutable();
      }

      /// Get list of tags, corresponding to a static tag                     
      ///   @tparam T tag definition to search for                            
      ///   @return the tag list, or nullptr if no such list exists           
      ///   @attention the list can be empty, if tag was provided with no     //TODO huh? wdym?
      ///      contents                                                       
      template<CT::DefineTag T>
      auto GetTags(this auto&& self) {
         return self.GetTags(MetaTagOf<T>());
      }

      template<class C>
      auto GetTags(this C&& self, TMeta t) -> LglsMutIf(C, TagList*) {
         LglsAssumeUser(t, "Can't get invalid tag");
         auto found = self.mTags.Find(t);
         if (found)
            return &*found.GetValHandle();
         return nullptr;
      }

      /// Get tag                                                             
      ///   @tparam T the type of tag to search for                           
      ///   @return selected data or nullptr if none was found                
      ///   @attention if not nullptr, returned Many might contain a Neat     
      template<CT::DefineTag T>
      auto GetTag(this auto&& self, size_t index = 0) {
         return self.GetTag(MetaTagOf<T>(), index);
      }

      template<class C>
      auto GetTag(this C&& self, TMeta meta, size_t index = 0) -> LglsMutIf(C, Tag*) {
         const auto found = self.mTags.Find(meta);
         if (found and found.GetValHandle()->GetCount() > index)
            return &((*found.GetValHandle())[index]);
         return nullptr;
      }

      /// Get list of data, corresponding to a static type                    
      ///   @tparam T type to search for                                      
      ///   @return the data list, or nullptr if no such list exists          
      template<CT::NotVoid T>
      auto GetData(this auto&& self) {
         return self.GetData(MetaDataOf<Decay<T>>());
      }

      template<class C>
      auto GetData(this C&& self, DMeta d) -> LglsMutIf(C, TailList*) {
         auto found = self.mAnythingElse.Find(d ? d.GetOrigin() : d);
         if (found)
            return &*found.GetValHandle();
         return nullptr;
      }
      
      /// Get list of recipes, corresponding to a static type                 
      ///   @tparam T type to search for                                      
      ///   @return the recipe list, or nullptr if no such list exists        
      template<CT::NotVoid T>
      auto GetRecipes(this auto&& self) {
         return self.GetRecipes(MetaDataOf<Decay<T>>());
      }

      template<class C>
      auto GetRecipes(this auto&& self, DMeta r) -> LglsMutIf(C, RecipeList*) {
         auto found = self.mRecipes.Find(r ? r.GetOrigin() : r);
         if (found)
            return &*found.GetValHandle();
         return nullptr;
      }

      /// Find data in recipe targets or tail, that casts to T                
      ///   @tparam T type requirement                                        
      ///   @return the first type that matches                               
      template<CT::NotVoid T>
      auto FindType() const -> DMeta {
         return FindType(MetaDataOf<T>());
      }

      auto FindType(DMeta type) const -> DMeta {
         DMeta primitive;
         bool  ambiguous = false;
         ForEachRecipe([&](Recipe const& c) noexcept {
            if (not c.GetTarget().CastsTo(type))
               return;

            if (not primitive)
               primitive = c.GetTarget();
            else
               ambiguous = true;
         });
         ForEachTail([&](Many const& c) noexcept {
            if (not c.CastsTo(type))
               return;

            if (not primitive)
               primitive = c.GetType();
            else
               ambiguous = true;
         });

         LglsAssumeDevWarn(not ambiguous,
            "Multiple primitives defined in a single Neat on FindData - "
            "all except the first `", primitive, "` will be ignored"
         );
         return primitive;
      }

      /// Set a default tag, if such wasn't already set                       
      //TODO isn't this like simply merge?? also make merge test only by tag id when merging tags!
      ///   @tparam T tag to set                                              
      ///   @param value the value to assign to the tag                       
      template<CT::DefineTag T>
      void SetDefaultTag (CT::NotVoid auto&& data) {
         auto found = GetTags<T>();
         if (found and *found)
            return;

         AddTag(T {LglsFwd(data)});
      }

      /// Extract tags from the descriptor, write their values to outputs     
      ///   @tparam T... tag(s) we're searching for                           
      ///   @param values [out] where to save the value(s), if found          
      ///   @return true if any output changed                                
      template<CT::DefineTag...T>
      bool ExtractTags(CT::NotVoid auto&...outputs) const {
         return (ExtractTagInner<T>(outputs...) or ...);
      }

      auto ExtractData   (CT::NotVoid auto&) const -> size_t;
      auto ExtractDataAs (CT::NotVoid auto&) const -> size_t;

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
      size_t ForEach      (this auto&&, auto&&...);
      size_t ForEachTag   (this auto&&, auto&&);
      size_t ForEachDeep  (this auto&&, auto&&...);
      size_t ForEachRecipe(this auto&&, auto&&);
      size_t ForEachTail  (this auto&&, auto&&);

   protected:
      size_t ForEachInner (this auto&&, auto&&);

   public:
      ///                                                                     
      ///   Insertion                                                         
      template<class T1, class...TN>
      auto  Insert(T1&&, TN&&...) -> size_t;

      /// Merge another Neat into this one                                    
      ///   @param rhs the container to merge                                 
      void Merge(Neat const& rhs) {
         if (not rhs)
            return;

         auto conflict_resolver = [](auto& lhs, auto& rhs) {
            lhs.MergeRange(rhs);
         };

         mTags.MergeRange(rhs.mTags, conflict_resolver);
         mRecipes.MergeRange(rhs.mRecipes, conflict_resolver);
         mAnythingElse.MergeRange(rhs.mAnythingElse, conflict_resolver);
         mHash = {};
      }

      Neat& SetTag(CT::DefineTag auto&&, size_t = 0);

      Neat& operator <<  (auto&&);
      Neat& operator <<= (auto&&);

   protected:
      auto UnfoldInsert (auto&&) -> size_t;
      void InsertInner  (auto&&);

      void AddTag   (CT::Intent auto&&);
      void AddRecipe(CT::Intent auto&&);
      void AddVerb  (CT::Intent auto&&);

   public:
      ///                                                                     
      ///   Removal                                                           
      template<CT::NotVoid, bool EMPTY_TOO = false>
      size_t RemoveData();
      template<CT::NotVoid>
      size_t RemoveRecipes();
      template<CT::DefineTag, bool EMPTY_TOO = false>
      size_t RemoveTag();
   };
}

namespace Langulus
{
   using Annies::Neat;
}

#include "Annies/Neat.inl"