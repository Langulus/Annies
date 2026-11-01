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
#include "Annies/Components/IndexedComposite.hpp"
//#include <Langulus/IntentOf.hpp>


namespace Langulus::Annies::Inner
{
   using RecipeList  = TMany<Recipe>;
   using MessyList   = TMany<Many>;
   using TagList     = TMany<Tag>;

   using RecipeMap   = TMapUnsorted<DMeta, RecipeList>;
   using MessyMap    = TMapUnsorted<DMeta, MessyList>;
   using TagMap      = TMapUnsorted<TMeta, TagList>;

   /// MARK: NeatBase                                                         
   using NeatBase = Com::Container<
      Com::StateStack<>,                     // No states               
      Com::Multiprovider<
         // Tags are ordered first by their tag type, then by their     
         // order of appearance. Duplicate tag types are allowed.       
         // Tag contents may or may not also be normalized.             
         Com::Stack<TagMap,    0>,
         // Subconstructs are sorted first by the recipe target, then   
         // by their order of appearance. Their contents may or may not 
         // also be normalized.                                         
         Com::Stack<RecipeMap, 1>,
         // Any other container that doesn't fit in the above is sorted 
         // first by its type, then by the order of appearance.         
         // Contents may or may not be normalized.                      
         Com::Stack<MessyMap,  2>
      >,
      Com::IndexedComposite<0, 0, 1, 2>,     // Composite indexing      
      Com::HashEmergent<Hash, 0, 1, 2>,      // Emergent hash           
      Com::Deepening<Many>,                  // Makes container deep    
      Com::Assignment<false, 0, 1, 2>,       // Assignment              
      Com::InsertionOperators<>,             // Insertion operators     
      Com::MergingOperators<>,               // Merge operators         
      /*Com::Comparison<false, true, 0, 1, 2>,*/ // Allows comparisons      
      Com::IterationForEach<0, 1, 2>,        // ForEach iteration       
      Com::IterationRange<0, 1, 2>,          // Ranged iteration        
      Com::Descriptor<0, 1, 2>               // Descriptor interface    
   >;
}

namespace Langulus::Annies
{

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
   struct Neat : Inner::NeatBase {
      using CTTI_ReflectAs = Neat;
      using CTTI_Neat      = Yup;

      using HandleType     = Handle;
      using HandleMutType  = Handle;
      using Pick           = Handle;
      using PickMut        = Handle;

      static constexpr bool ContainsMany = true;

   protected:
      auto& GetTagsInner(this auto&& self) noexcept {
         return *self.template Stack<Inner::TagMap, 0>::GetRaw();
      }

      auto& GetRecsInner(this auto&& self) noexcept {
         return *self.template Stack<Inner::RecipeMap, 1>::GetRaw();
      }

      auto& GetMessInner(this auto&& self) noexcept {
         return *self.template Stack<Inner::MessyMap, 2>::GetRaw();
      }

   public:
      ///                                                                     
      /// MARK:Construction                                                   
      constexpr Neat() noexcept {
         this->ConstructDefault();
      }
      constexpr Neat(Neat const& other) {
         this->Absorb(Refer(other));
      }
      constexpr Neat(Neat&& other) noexcept  {
         this->Absorb(Move(other));
      }
      constexpr ~Neat() noexcept {
         this->Destroy();
      }

      /// Construction that either absorbs the provided containers, or        
      /// emplaces all A in the container                                     
      template<Disambiguate A1, class...AN>
      constexpr Neat(A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0) {
            if constexpr (Same<Deint<A1>, Neat>)
               this->Absorb(LglsFwd(a1));
            else
               this->Insert(LglsFwd(a1));
         }
         else {
            this->ConstructDefault();
            this->Insert(LglsFwd(a1));
           (this->Insert(LglsFwd(an)), ...);
         }
      }
      
      /// Construction that absorbs the provided containers                   
      template<class A1, class...AN>
      constexpr Neat(Inner::Absorb, A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0) {
            if constexpr (Same<Deint<A1>, Neat>)
               this->Absorb(LglsFwd(a1));
            else {
               this->ConstructDefault();
               this->AbsorbPartial(LglsFwd(a1));
            }
         }
         else {
            this->ConstructDefault();
            this->AbsorbPartial(LglsFwd(a1));
           (this->AbsorbPartial(LglsFwd(an)), ...);
         }
      }

      /// Construct via intent                                                
      /*template<template<class> class I> requires CT::Intent<I<Neat>>
      Neat(I<Neat>&& other)
         : mHash         {   other->mHash}
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
      }*/

      ///                                                                     
      /// MARK:Assignment                                                     
      constexpr Neat& operator = (Neat const& other) {
         return this->AssignAbsorb(Refer(other));
      }
      constexpr Neat& operator = (Neat&& other) noexcept {
         return this->AssignAbsorb(Move(other));
      }
      
      template<class A>
      constexpr Neat& operator = (A&& argument) {
         if constexpr (Same<Deint<A>, Neat>)
            return this->AssignAbsorb(LglsFwd(argument));
         else
            return this->Assign(LglsFwd(argument));
      }

      //using Com::Comparison<false, true, 0, 1, 2>::operator <=>;
      //using Com::Comparison<false, true, 0, 1, 2>::operator ==;

      /*Neat& operator = (Neat const&) = default;
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
      }*/

   public:
      /// Clear the container without deallocating                            
      void Clear() {
         GetTagsInner().Clear();
         GetRecsInner().Clear();
         GetMessInner().Clear();
      }

      /// Clear and deallocate the container                                  
      void Reset() {
         GetTagsInner().Reset();
         GetRecsInner().Reset();
         GetMessInner().Reset();
      }

      ///                                                                     
      /// MARK: Encapsulation                                                 
      /// Get the hash of a neat container (and cache it)                     
      ///   @attention missing scopes never participate in hashing/comparison 
      ///   @return the hash                                                  
      /*Hash GetHash() const {
         if (mHash)
            return mHash;

         if (IsEmpty())
            mHash = 1;
         else
            mHash = HashOf(mTags, mRecipes, mAnythingElse);//TODO we can just push/pop xor these, as order doesn't matter here
         return mHash;
      }*/

      /// Get list of tags corresponding to a statically specified tag        
      ///   @tparam T tag definition to search for                            
      ///   @return the tag list, or nullptr if no such list exists           
      ///   @attention the list may be empty, if tag was added as TMeta       
      template<CT::DefineTag T>
      auto GetTags(this auto&& self) {
         return self.GetTags(MetaTagOf<T>());
      }

      template<class C>
      auto GetTags(this C&& self, TMeta t) -> LglsMutIf(C, Inner::TagList*) {
         LglsAssumeUser(t, "Can't get invalid tag");
         auto found = self.GetTagsInner().Find(t);
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
      auto GetData(this C&& self, DMeta d) -> LglsMutIf(C, Inner::MessyList*) {
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
      auto GetRecipes(this auto&& self, DMeta r) -> LglsMutIf(C, Inner::RecipeList*) {
         auto found = self.GetRecipesInner().Find(r ? r.GetOrigin() : r);
         if (found)
            return &*found.GetValHandle();
         return nullptr;
      }

      /// Find data in recipe targets or tail, that casts to T                
      ///   @tparam T type requirement                                        
      ///   @return the first type that matches                               
      /*template<CT::NotVoid T>
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
      }*/

      /// Set a default tag, if such wasn't already set                       
      //TODO isn't this like simply merge?? also make merge test only by tag id when merging tags!
      ///   @tparam T tag to set                                              
      ///   @param value the value to assign to the tag                       
      /*template<CT::DefineTag T>
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
      auto ExtractDataAs (CT::NotVoid auto&) const -> size_t;*/

   protected:
      /*template<CT::DefineTag>
      bool ExtractTagInner(CT::NotVoid auto&...) const;
      template<size_t...IDX>
      bool ExtractTagInner(TagList const&, ExpandedSequence<IDX...>, CT::NotVoid auto&...) const;
      template<size_t>
      bool ExtractTagInnerInner(TagList const&, CT::NotVoid auto&) const;*/

   public:
      ///                                                                     
      ///   Iteration                                                         
      /*size_t ForEach      (this auto&&, auto&&...);
      size_t ForEachTag   (this auto&&, auto&&);
      size_t ForEachDeep  (this auto&&, auto&&...);
      size_t ForEachRecipe(this auto&&, auto&&);
      size_t ForEachTail  (this auto&&, auto&&);

   protected:
      size_t ForEachInner (this auto&&, auto&&);*/

   public:
      ///                                                                     
      ///   Insertion                                                         
      auto Insert(auto&&, auto&&...) -> size_t;

      /// Merge another Neat into this one                                    
      ///   @param rhs the container to merge                                 
      void Merge(Neat const& rhs) {
         if (not rhs)
            return;

         auto conflict_resolver = [](auto&& lhs, auto&& rhs) {
            lhs.Concat(LglsFwd(rhs));
         };

         GetTagsInner().MergeRange(rhs.GetTagsInner(), conflict_resolver);
         GetRecsInner().MergeRange(rhs.GetRecsInner(), conflict_resolver);
         GetMessInner().MergeRange(rhs.GetMessInner(), conflict_resolver);
      }

      //Neat& SetTag(CT::DefineTag auto&&, size_t = 0);

      //Neat& operator <<  (auto&&);
      //Neat& operator <<= (auto&&);

   protected:
      auto UnfoldInsert (auto&&) -> size_t;
      void InsertInner  (auto&&);

      /// Absorb some part of the neat                                        
      ///   @attention assumes container has been initialized                 
      template<class T>
      void AbsorbPartial(T&& a) {
         if constexpr (Same<Deint<T>, Inner::TagMap>)
            this->GetTagsInner().Absorb(LglsFwd(a));
         else if constexpr (Same<Deint<T>, Inner::MessyMap>)
            this->GetMessInner().Absorb(LglsFwd(a));
         else if constexpr (Same<Deint<T>, Inner::RecipeMap>)
            this->GetRecsInner().Absorb(LglsFwd(a));
         else if constexpr (Same<Deint<T>, Inner::TagList>) {
            for (auto& tag : DeintCast(a))
               this->AddTag(NestIntentOf(a, tag));
         }
         else if constexpr (Same<Deint<T>, Inner::RecipeList>) {
            for (auto& recipe : DeintCast(a))
               this->AddRecipe(NestIntentOf(a, recipe));
         }
         else if constexpr (Same<Deint<T>, Inner::MessyList>) {
            for (auto& messy : DeintCast(a))
               this->AddData(NestIntentOf(a, messy));
         }
         else this->Insert(LglsFwd(a));
      }

   public:
      ///                                                                     
      /// MARK: Insertion                                                     
      /// Add tag to the tag map                                              
      template<CT::Intent T>
      void AddTag(T&& tag) {
         if constexpr (Same<Deint<T>, TMeta>)
            this->GetTagsInner().Merge(LglsFwd(tag));
         else {
            static_assert(CT::Tag<T>, "Must be a tag container");
            if (not tag->IsValid()) {
               this->GetTagsInner().Merge(tag->GetTag());
               return;
            }

            auto found = this->GetTagsInner().Find(tag->GetTag());
            if (found) {
               found.GetVal()->Insert(
                  NestIntentOf(tag, tag->template ReinterpretAs<Tag>())
               );
            }
            else this->GetTagsInner().Merge(
               tag->GetTag(),
               NestIntentOf(tag, tag->template ReinterpretAs<Tag>())
            );
         }
      }

      /// Add recipe to the recipe map                                        
      template<CT::Intent T>
      void AddRecipe(T&& recipe) {
         if constexpr (Same<Deint<T>, DMeta>)
            this->GetRecsInner().Merge(recipe->GetOrigin());
         else {
            static_assert(CT::Recipe<T>, "Must be a recipe container");
            const auto meta = recipe->GetTarget().GetOrigin();
            if (not recipe->IsValid()) {
               this->GetRecsInner().Merge(meta);
               return;
            }

            auto found = this->GetRecsInner().Find(meta);
            if (found)
               found.GetVal()->Insert(LglsFwd(recipe));
            else
               this->GetRecsInner().Merge(meta, LglsFwd(recipe));
         }
      }

      /// Add verb/data to the data map                                       
      template<CT::Intent T>
      void AddData(T&& data) {
         static_assert(not CT::Tag<T>);
         static_assert(not CT::Recipe<T>);
         using AS = CT::ReflectedAs<Decvq<Deref<Deint<T>>>>;

         if constexpr (CT::Container<AS> and not CT::Multidimensional<AS>) {
            // Push a one-dimensional container                         
            const auto meta = data->GetType();
            if (not data->IsValid()) {
               this->GetMessInner().Merge(meta);
               return;
            }

            auto found = this->GetMessInner().Find(meta);
            if (found) {
               found.GetVal()->Insert(Many {Annies::Absorb, LglsFwd(data)});
            }
            else this->GetMessInner().Merge(
               meta, Many {Annies::Absorb, LglsFwd(data)}
            );
         }
         else {
            // Push any other raw data/map                              
            const auto meta = MetaDataOf<AS>();
            auto found = this->GetMessInner().Find(meta);
            if (found) {
               found.GetVal()->Insert(Many {LglsFwd(data)});
            }
            else this->GetMessInner().Merge(meta, Many {LglsFwd(data)});
         }
      }

      ///                                                                     
      /// MARK: Removal                                                       
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