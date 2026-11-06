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
         this->ConstructDefault();
         this->GetTagsInner().Absorb(Refer(other.GetTagsInner()));
         this->GetRecsInner().Absorb(Refer(other.GetRecsInner()));
         this->GetMessInner().Absorb(Refer(other.GetMessInner()));
      }
      constexpr Neat(Neat&& other) noexcept  {
         this->ConstructDefault();
         this->GetTagsInner().Absorb( Move(other.GetTagsInner()));
         this->GetRecsInner().Absorb( Move(other.GetRecsInner()));
         this->GetMessInner().Absorb( Move(other.GetMessInner()));
      }
      constexpr ~Neat() noexcept {
         this->Destroy();
      }

      /// Construction that either absorbs the provided containers, or        
      /// emplaces all A in the container                                     
      template<Disambiguate A1, class...AN>
      constexpr Neat(A1&& a1, AN&&...an) {
         this->ConstructDefault();

         if constexpr (sizeof...(AN) == 0) {
            if constexpr (Same<Deint<A1>, Neat>) {
               this->GetTagsInner().Absorb(NestIntentOf(a1, DeintCast(a1).GetTagsInner()));
               this->GetRecsInner().Absorb(NestIntentOf(a1, DeintCast(a1).GetRecsInner()));
               this->GetMessInner().Absorb(NestIntentOf(a1, DeintCast(a1).GetMessInner()));
            }
            else this->Insert(LglsFwd(a1));
         }
         else {
            this->Insert(LglsFwd(a1));
           (this->Insert(LglsFwd(an)), ...);
         }
      }
      
      /// Construction that absorbs the provided containers                   
      template<class A1, class...AN>
      constexpr Neat(Inner::Absorb, A1&& a1, AN&&...an) {
         this->ConstructDefault();

         if constexpr (sizeof...(AN) == 0) {
            if constexpr (Same<Deint<A1>, Neat>) {
               this->GetTagsInner().Absorb(NestIntentOf(a1, DeintCast(a1).GetTagsInner()));
               this->GetRecsInner().Absorb(NestIntentOf(a1, DeintCast(a1).GetRecsInner()));
               this->GetMessInner().Absorb(NestIntentOf(a1, DeintCast(a1).GetMessInner()));
            }
            else this->AbsorbPartial(LglsFwd(a1));
         }
         else {
            this->AbsorbPartial(LglsFwd(a1));
           (this->AbsorbPartial(LglsFwd(an)), ...);
         }
      }

      ///                                                                     
      /// MARK:Assignment                                                     
      constexpr Neat& operator = (Neat const& other) {
         this->GetTagsInner().AssignAbsorb(Refer(other.GetTagsInner()));
         this->GetRecsInner().AssignAbsorb(Refer(other.GetRecsInner()));
         this->GetMessInner().AssignAbsorb(Refer(other.GetMessInner()));
         return *this;
      }

      constexpr Neat& operator = (Neat&& other) noexcept {
         this->GetTagsInner().AssignAbsorb( Move(other.GetTagsInner()));
         this->GetRecsInner().AssignAbsorb( Move(other.GetRecsInner()));
         this->GetMessInner().AssignAbsorb( Move(other.GetMessInner()));
         return *this;
      }
      
      template<class A>
      constexpr Neat& operator = (A&& argument) {
         if constexpr (Same<Deint<A>, Neat>) {
            this->GetTagsInner().AssignAbsorb(NestIntentOf(argument, DeintCast(argument).GetTagsInner()));
            this->GetRecsInner().AssignAbsorb(NestIntentOf(argument, DeintCast(argument).GetRecsInner()));
            this->GetMessInner().AssignAbsorb(NestIntentOf(argument, DeintCast(argument).GetMessInner()));
         }
         else {
            this->Clear();
            this->Insert(LglsFwd(argument));
         }
         return *this;
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
               found.GetVal().Insert(
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
               found.GetVal().Insert(LglsFwd(recipe));
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
               found.GetVal().Insert(Many {Annies::Absorb, LglsFwd(data)});
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
               found.GetVal().Insert(Many {LglsFwd(data)});
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

   protected:
      LglsComDeepening(friend);
      LglsComDescriptor(friend);

      /// MARK: ForEach                                                       
      template<class C, class F, CT::Handle H>
      LoopControl InvokeF(this C&& self, F&& f, H&& handle) noexcept_if(f) {
         using A = Deref<ArgumentOf<F>>;
         using R = ReturnOf<F>;

         if constexpr (CT::Bool<R>) {
            // Execute and consider 'f' returning true/false            
            if constexpr (Same<DMeta, A> and Same<TypeOf<H>, Recipe>) {
               if (not f(handle.Get()->GetTarget()))
                  return Loop::NextLoop;
            }
            else {
               if (not f(*handle.template Get<A>()))
                  return Loop::NextLoop;
            }
         }
         else if constexpr (Exact<R, LoopControl>) {
            // Execute and consider 'f' returning LoopControl           
            LoopControl loop = Loop::Continue;
            if constexpr (Same<DMeta, A> and Same<TypeOf<H>, Recipe>)
               loop = f(handle.Get()->GetTarget());
            else
               loop = f(*handle.template Get<A>());

            switch (loop.mControl) {
               case LoopControl::Repeat:
                  --handle;
                  break;
               case LoopControl::Discard:
                  if constexpr (CT::Mutable<C>) {
                     // Discard is allowed only if THIS is mutable.     
                     // Why bother removing, when there's only one      
                     // element? Just propagate discard instead! The    
                     // pack should be reset from above.                
                     if (self.GetCount() == 1)
                        return Loop::Discard;

                     // Otherwise just erase and continue               
                     handle = self.Erase(handle);
                     --handle;
                     return Loop::Continue;
                  }
                  else {
                     LglsAssumeUserWarn(false,
                        "Attempting to discard while iterating constant container. "
                        "No discard will be performed."
                     );
                     return Loop::Continue;
                  }
                  break;
               default: return loop;
            }
         }
         else {
            // Just execute, always loop through everything             
            if constexpr (Same<DMeta, A> and Same<TypeOf<H>, Recipe>)
               f(handle.Get()->GetTarget());
            else
               f(*handle.template Get<A>());
         }
         return Loop::Continue;
      }

   public://TODO figure out why i can't access it from Deepening
      /// Iterate and execute call for each flat element, counting each       
      /// successfull execution. Custom inner routine designed specifically   
      /// for Neat, searching in a specific bucket depending on lambda.       
      ///   @attention assumes neat is non-empty                              
      ///   @tparam REVERSE whether to iterate in reverse                     
      ///   @param f the function to execute for each element of type A       
      ///   @param index [out] counts the successful executions               
      ///   @return the last 'f' result - dictates whether loop continues     
      template<bool REVERSE, class C, class F>//TODO reverse not implemented here
      LoopControl ForEachInner(this C&& self, F&& f, size_t& index) noexcept_if(f) {
         using A = ArgumentOf<F>;

         static_assert(CT::Slab<A> or CT::Mutable<C> or CT::ConstantEverywhere<A>,
            "Mutable reference/pointer iterator not allowed for constant container");
         LglsAssumeDev(not self.IsEmpty(), "Can't iterate empty container");

         LoopControl loop = Loop::Continue;
         if constexpr (CT::Deep<A>) {
            //TODO definitely need a better binary-compatibility check lmao
            static_assert(sizeof(Decay<A>) == sizeof(Many));

            if constexpr (CT::TypeErased<A>) {
               // Deep argument, which is also type-erased, and this    
               // means we travel through _all_ buckets                 
               self.GetTagsInner().Apply([&loop,&f,&self,&index](auto& pair) noexcept_if(f) {
                  ++index;
                  auto bucket = pair.GetValHandle();
                  auto& as_type_erased = reinterpret_cast<THandle<Decay<A>&>&>(bucket);
                  loop = self.InvokeF(LglsFwd(f), as_type_erased);
                  return loop == Loop::Continue;
               });
               if (loop != Loop::Continue)
                  return loop;

               self.GetRecsInner().Apply([&loop,&f,&self,&index](auto& pair) noexcept_if(f) {
                  ++index;
                  auto bucket = pair.GetValHandle();
                  auto& as_type_erased = reinterpret_cast<THandle<Decay<A>&>&>(bucket);
                  loop = self.InvokeF(LglsFwd(f), as_type_erased);
                  return loop == Loop::Continue;
               });
               if (loop != Loop::Continue)
                  return loop;

               self.GetMessInner().Apply([&loop,&f,&self,&index](auto& pair) noexcept_if(f) {
                  ++index;
                  auto bucket = pair.GetValHandle();
                  auto& as_type_erased = reinterpret_cast<THandle<Decay<A>&>&>(bucket);
                  loop = self.InvokeF(LglsFwd(f), as_type_erased);
                  return loop == Loop::Continue;
               });
            }
            else {
               // We are interested only in buckets of the same static  
               // type as the iterator.                                 
               using AType = TypeOf<A>;

               if constexpr (CT::DefineTag<AType>) {
                  auto found = self.GetTagsInner().Find(MetaTagOf<AType>());
                  if (not found)
                     return Loop::NextLoop;
                  ++index;
                  return self.InvokeF(LglsFwd(f), found.GetValHandle());
               }
               else if constexpr (CT::Tag<AType>) {
                  self.GetTagsInner().Apply([&loop,&f,&self,&index](auto& pair) noexcept_if(f) {
                     ++index;
                     loop = self.InvokeF(LglsFwd(f), pair.GetValHandle());
                     return loop == Loop::Continue;
                  });
               }
               else if constexpr (CT::Recipe<AType>) {
                  self.GetRecsInner().Apply([&loop,&f,&self,&index](auto& pair) noexcept_if(f) {
                     ++index;
                     loop = self.InvokeF(LglsFwd(f), pair.GetValHandle());
                     return loop == Loop::Continue;
                  });
               }
               else {
                  auto found = self.GetMessInner().Find(MetaDataOf<AType>());
                  if (not found)
                     return Loop::NextLoop;

                  found.GetValHandle()->Apply([&loop,&f,&self,&index](auto& pair) noexcept_if(f) {
                     ++index;
                     loop = self.InvokeF(LglsFwd(f), pair);
                     return loop == Loop::Continue;
                  });
               }
            }
         }
         else if constexpr (Same<A, DMeta>) {
            // We are interested only in empty recipes with the given   
            // target type.                                             
            self.GetRecsInner().Apply([&loop,&f,&self,&index](auto& pair) noexcept_if(f) {
               pair.GetValHandle()->Apply([&loop,&f,&self,&index](auto& recipe) noexcept_if(f) {
                  if (not recipe->GetDescriptor().IsValid())
                     return true;

                  ++index;
                  loop = self.InvokeF(LglsFwd(f), recipe);
                  return loop == Loop::Continue;
               });
               return loop == Loop::Continue;
            });
         }
         else if constexpr (Same<A, TMeta>) {
            // We are interested only in empty tags with the given tag  
            self.GetTagsInner().Apply([&loop,&f,&self,&index](auto& pair) noexcept_if(f) {
               pair.GetValHandle()->Apply([&loop,&f,&self,&index](auto& tag) noexcept_if(f) {
                  if (not tag->GetData().IsValid())
                     return true;

                  ++index;
                  loop = self.InvokeF(LglsFwd(f), tag);
                  return loop == Loop::Continue;
               });
               return loop == Loop::Continue;
            });
         }
         else if constexpr (CT::Recipe<A>) {
            // We are interested in all recipes                         
            self.GetRecsInner().Apply([&loop,&f,&self,&index](auto& pair) noexcept_if(f) {
               pair.GetValHandle()->Apply([&loop,&f,&self,&index](auto& recipe) noexcept_if(f) {
                  ++index;
                  loop = self.InvokeF(LglsFwd(f), recipe);
                  return loop == Loop::Continue;
               });
               return loop == Loop::Continue;
            });
         }
         else if constexpr (CT::DefineTag<A>) {
            // We are interested in specific tags                       
            auto found = self.GetTagsInner().Find(MetaTagOf<A>());
            if (not found)
               return Loop::NextLoop;

            using TAG_BUCKET = LglsMutIf(C, TMany<Decay<A>>&);
            auto& specific_tags = reinterpret_cast<TAG_BUCKET>(*found.GetValHandle());
            specific_tags.Apply([&loop,&f,&self,&index](auto& tag) noexcept_if(f) {
               ++index;
               loop = self.InvokeF(LglsFwd(f), tag);
               return loop == Loop::Continue;
            });
         }
         else if constexpr (CT::Tag<A>) {
            // We are interested in all tags                            
            self.GetTagsInner().Apply([&loop,&f,&self,&index](auto& pair) noexcept_if(f) {
               pair.GetValHandle()->Apply([&loop,&f,&self,&index](auto& tag) noexcept_if(f) {
                  ++index;
                  loop = self.InvokeF(LglsFwd(f), tag);
                  return loop == Loop::Continue;
               });
               return loop == Loop::Continue;
            });
         }
         else {
            // We are interested in all data of the given type          
            auto found = self.GetMessInner().Find(MetaDataOf<A>());
            if (not found)
               return Loop::NextLoop;

            found.GetValHandle()->Apply([&loop,&f,&self,&index](auto& data) noexcept_if(f) {
               data->Apply([&loop,&f,&self,&index](auto& element) noexcept_if(f) {
                  ++index;
                  loop = self.InvokeF(LglsFwd(f), element);
                  return loop == Loop::Continue;
               });
               return loop == Loop::Continue;
            });
         }

         return loop;
      }
      
      /// Iterate and execute call for each deep element, counting each       
      /// successfull execution. Custom inner routine designed specifically   
      /// for Neat. Neat is generally considered a flattened container, but   
      /// that is true only if there are no OR/MISSING states. If there are   
      /// such states, then we can nest deep into those.                      
      ///   @tparam REVERSE whether to iterate in reverse                     
      ///   @tparam SKIP whether to execute call for intermediate blocks      
      ///   @param f the function to execute for each element of type A       
      ///   @param index [out] counts the successful executions               
      ///   @return the last 'f' result                                       
      template<bool REVERSE, bool SKIP, CT::Container C, class F> //TODO reverse not implemented here
      LoopControl ForEachDeepInner(this C&& self, size_t depth, F&& f, size_t& index) noexcept_if(f) {
         // First do the conventional thing, and then if there's        
         // depth, delve into messy Many with states.                   
         return self.template ForEachInner<REVERSE>(LglsFwd(f), index);

         //TODO delve into messy Many with states - it's the only place where nesting can happen
         //TODO if user pushed some deep containers with states
         (void) depth;
      }
      
      /// Iterate all other types of data - skips iterating tags and recipes. 
      /// You can provide a TMany iterator, to filter based on data type.     
      ///   @param f the function to execute for each block.                  
      ///      If `f` returns bool, you can decide when to break the loop by  
      ///      simply returning Flow::Break (or just false).                  
      ///   @return the number of executions of 'f'                           
      template<CT::Container C, class F>
      size_t ForEachData(this C&& self, F&& f) {
         using A = ArgumentOf<F>;

         static_assert(CT::Slab<A> or CT::Mutable<C> or CT::ConstantEverywhere<A>,
            "Mutable reference/pointer iterator not allowed for constant container");
         
         if (self.GetMessInner().IsEmpty())
            return 0;

         size_t index = 0;
         if constexpr (CT::Deep<A>) {
            //TODO definitely need a better binary-compatibility check lmao
            static_assert(sizeof(Decay<A>) == sizeof(Many));

            if constexpr (CT::TypeErased<A>) {
               // Deep argument, which is also type-erased, and this    
               // means we travel through _all_ messy buckets           
               self.GetMessInner().Apply([&f,&self,&index](auto& pair) /*noexcept_if(f)*/ {
                  ++index;
                  auto bucket = pair.GetValHandle();
                  auto& as_type_erased = reinterpret_cast<THandle<Decay<A>&>&>(bucket);
                  auto loop = self.InvokeF(LglsFwd(f), as_type_erased);
                  return loop == Loop::Continue;
               });
            }
            else {
               // We are interested only in buckets of the same static  
               // type as the iterator.                                 
               using AType = TypeOf<A>;
               auto found = self.GetMessInner().Find(MetaDataOf<AType>());
               if (not found)
                  return 0;

               found.GetValHandle()->Apply([&f,&self,&index](auto& pair) /*noexcept_if(f)*/ {
                  ++index;
                  auto loop = self.InvokeF(LglsFwd(f), pair);
                  return loop == Loop::Continue;
               });
            }
         }
         else self.ForEachInner(LglsFwd(f), index);
         return index;
      }
      
      /// MARK: Apply                                                         
      /// Visit all element's handles and perform a function on them.         
      /// Specialized for Neat.                                               
      ///   @param f the function to perform. If the lambda returns bool,     
      ///      you can end the loop early by returning false.                 
      ///   @param cookie the element/hash table spot to start off from       
      ///   @attention this will ignore any ordering                          
      ///   @attention assumes container isn't empty                          
      template<CT::Neat C>//TODO reverse?
      void Apply(this C& self, auto&& f, [[maybe_unused]] size_t cookie = 0) {
         LglsAssumeDev(not self.IsEmpty(), "Make sure Neat isn't empty");

         bool abort = false;
         self.GetTagsInner().Apply([&f,&cookie,&abort](auto& pair) /*noexcept_if(f)*/ {
            const size_t tag_count = pair.GetValHandle()->GetCount();
            if (cookie >= tag_count) {
               cookie -= tag_count;
               return true;
            }

            auto tag = pair.GetValHandle()->GetHandle();
            auto const end = tag + (tag_count - cookie);
            cookie = 0;
            while(tag != end) {
               if constexpr (CT::Bool<decltype(f(tag))>) {
                  if (not f(tag)) {
                     abort = true;
                     return false;
                  }
               }
               else f(tag);
               ++tag;
            }
            return true;
         });
         if (abort)
            return;

         self.GetRecsInner().Apply([&f,&cookie,&abort](auto& pair) /*noexcept_if(f)*/ {
            const size_t recipe_count = pair.GetValHandle()->GetCount();
            if (cookie >= recipe_count) {
               cookie -= recipe_count;
               return true;
            }

            auto recipe = pair.GetValHandle()->GetHandle();
            auto const end = recipe + (recipe_count - cookie);
            cookie = 0;
            while(recipe != end) {
               if constexpr (CT::Bool<decltype(f(recipe))>) {
                  if (not f(recipe)) {
                     abort = true;
                     return false;
                  }
               }
               else f(recipe);
               ++recipe;
            }
            return true;
         });
         if (abort)
            return;

         self.GetMessInner().Apply([&f,&cookie](auto& pair) /*noexcept_if(f)*/ {
            const size_t data_count = pair.GetValHandle()->GetCount();
            if (cookie >= data_count) {
               cookie -= data_count;
               return true;
            }

            auto data = pair.GetValHandle()->GetHandle();
            auto const end = data + (data_count - cookie);
            cookie = 0;
            while(data != end) {
               if constexpr (CT::Bool<decltype(f(data))>) {
                  if (not f(data))
                     return false;
               }
               else f(data);
               ++data;
            }
            return true;
         });
      }
   };
}

#include "Annies/Neat.inl"