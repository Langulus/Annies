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
#include "TTable.hpp"
#include "Annies/Components/IndexedComposite.hpp"


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
   /// insert, or remove, albeit quite a bit larger. It is designed to act    
   /// exactly like Many on the outside, while being completely different on  
   /// the inside.                                                            
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
      /// emplaces all A in the container.                                    
      ///   @attention converting to Neat is explicit because it involves a   
      ///      lot of overhead                                                
      template<Disambiguate A1, class...AN>
      explicit constexpr Neat(A1&& a1, AN&&...an) {
         this->ConstructDefault();

         if constexpr (sizeof...(AN) == 0 and not CT::BoundedArray<A1>) {
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

      /// If deep, counts number of categories. If not deep, returns the      
      /// cumulative size of the category                                     
      size_t GetCount() const noexcept {
         size_t cat_count = GetCategoryCount();
         if (cat_count != 1)
            return cat_count;
            
         size_t cumulative_size = 0;
         if (not GetTagsInner().IsEmpty()) {
            for (auto& tag_list : InValuesOf(GetTagsInner()))
               cumulative_size += tag_list.GetCount();
            return cumulative_size;
         }
         
         if (not GetRecsInner().IsEmpty()) {
            for (auto& rec_list : InValuesOf(GetRecsInner()))
               cumulative_size += rec_list.GetCount();
            return cumulative_size;
         }
      
         return GetMessInner().begin()->GetVal().GetCount();
      }

      /// How many buckets are populated?                                     
      size_t GetCategoryCount() const noexcept {
         size_t cross_category_size = 0;
         if (not GetTagsInner().IsEmpty())
            ++cross_category_size;
         if (not GetRecsInner().IsEmpty())
            ++cross_category_size;

         cross_category_size += GetMessInner().GetCount();
         return cross_category_size;
      }

      /// Check if Neat is empty                                              
      bool IsEmpty() const noexcept {
         return GetCategoryCount() == 0;
      }

      /// Check if Neat is empty                                              
      explicit operator bool() const noexcept {
         return not IsEmpty();
      }

      ///                                                                     
      /// MARK: Capsulation                                                   
      /// By default, a Neat is an AND container spread into buckets. It can  
      /// be OR, if there's exactly one kind of bucket, and it is OR.         
      bool IsOr() const noexcept {
         if (GetCategoryCount() != 1)
            return false;

         if (not GetTagsInner().IsEmpty())
            return GetTagsInner().begin()->GetVal().IsOr();
         else if (not GetRecsInner().IsEmpty())
            return GetRecsInner().begin()->GetVal().IsOr();
         return GetMessInner().begin()->GetVal().IsOr();
      }

      /// By default, a Neat is flat if containing one type of elements,      
      /// unless that one type is deep. If Neat contains different kinds of   
      /// elements, then it can be considered deep.                           
      bool IsDeep() const noexcept {
         if (GetCategoryCount() == 1 and not GetMessInner().IsEmpty())
            return GetMessInner().begin()->GetKey().IsDeep();
         return true;
      }

      /// If Neat contains exactly one kind of data, you can test its type.   
      /// If Neat contains more than one kind of data, it is only Many.       
      template<class T>
      bool Is() const noexcept {
         if (GetCategoryCount() != 1)
            return Akin<T, Many>;

         const auto meta = MetaDataOf<T>();
         if (not GetTagsInner().IsEmpty())
            return MetaDataOf<Tag>().Is(meta);
         if (not GetRecsInner().IsEmpty())
            return MetaDataOf<Recipe>().Is(meta);
            
         return GetMessInner().begin()->GetKey().Is(meta);
      }

      template<class T>
      bool IsSame() const noexcept {
         if (GetCategoryCount() != 1)
            return Same<T, Many>;

         const auto meta = MetaDataOf<T>();
         if (not GetTagsInner().IsEmpty())
            return MetaDataOf<Tag>().IsSame(meta);
         if (not GetRecsInner().IsEmpty())
            return MetaDataOf<Recipe>().IsSame(meta);
            
         return GetMessInner().begin()->GetKey().IsSame(meta);
      }

      template<class T>
      bool IsExact() const noexcept {
         if (GetCategoryCount() != 1)
            return Exact<T, Many>;

         const auto meta = MetaDataOf<T>();
         if (not GetTagsInner().IsEmpty())
            return MetaDataOf<Tag>().IsExact(meta);
         if (not GetRecsInner().IsEmpty())
            return MetaDataOf<Recipe>().IsExact(meta);
            
         return GetMessInner().begin()->GetKey().IsExact(meta);
      }

      /// If Neat contains exactly one kind of data, you can test if castable 
      /// to it. If Neat contains more than one kind of data, it is castable  
      /// only to deep types.                                                 
      template<class T>
      bool CastsTo() const noexcept {
         if (GetCategoryCount() != 1)
            return CT::Deep<T>;

         if (not GetTagsInner().IsEmpty())
            return CT::Tag<T>;
         if (not GetRecsInner().IsEmpty())
            return CT::Recipe<T>;
            
         return GetMessInner().begin()->GetKey().CastsTo(MetaDataOf<T>());
      }

      bool CastsTo(DMeta meta) const noexcept {
         if (GetCategoryCount() != 1)
            return meta.CastsTo(MetaDataOf<Many>());

         if (not GetTagsInner().IsEmpty())
            return meta.CastsTo(MetaDataOf<Tag>());
         if (not GetRecsInner().IsEmpty())
            return meta.CastsTo(MetaDataOf<Recipe>());
            
         return GetMessInner().begin()->GetKey().CastsTo(meta);
      }

      /// Get pointer to the first element in the proper bucket.              
      ///   @attention no type-safety                                         
      ///   @attention assumes the bucket is valid and properly typed         
      ///   @return pointer to the first element of the desired type          
      template<class AS>
      auto* Get(this auto&& self) {
         LglsAssumeUser(not self.IsEmpty(), "Empty container on Neat::As");
         LglsAssumeUser(self.GetCategoryCount() == 1, "Multiple categories on Neat::Get");

         static_assert(not CT::Handle<AS>,    "AS can't be a handle");
         static_assert(not CT::Reference<AS>, "Strip references first");
         static_assert(not CT::Void<AS>,      "AS can't be coid");

         if constexpr (CT::Tag<AS>)
            return self.GetTagsInner().GetVal().template Get<AS>();
         else if constexpr (CT::Recipe<AS>)
            return self.GetRecsInner().GetVal().template Get<AS>();
         else
            return self.GetMessInner().Find(MetaDataOf<AS>()).GetVal()[0].template Get<AS>();
      }

      /// Get the first element of the proper bucket wrapped in a desired type
      ///   @attention no type-safety                                         
      ///   @attention assumes the container contains exactly one category    
      ///   @return pointer to the first element of the desired type          
      template<class AS>
      decltype(auto) As(this auto&& self) {
         LglsAssert(not self.IsEmpty(), "Empty container on Neat::As");
         LglsAssert(self.GetCategoryCount() == 1, "Multiple categories on Neat::As");

         if constexpr (CT::Tag<AS>)
            return self.GetTagsInner().GetVal().template As<AS>();
         else if constexpr (CT::Recipe<AS>)
            return self.GetRecsInner().GetVal().template As<AS>();
         else if constexpr (Same<AS, Many>)
            return self.GetRecsInner().begin()->GetVal().template As<AS>();
         else
            return self.GetMessInner().Find(MetaDataOf<AS>()).GetVal()[0].template As<AS>();
      }

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
         const auto found = self.GetTagsInner().Find(meta);
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
         auto found = self.GetMessInner().Find(d ? d.GetOrigin() : d);
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

      ///                                                                     
      /// MARK: Insertion                                                     
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

            if constexpr (T::IsMoved()) {
               decltype(auto) tag_erased = reinterpret_cast<Tag&&>(tag.what);
               if (found)
                  found.GetVal().Insert(NestIntentOf(tag, tag_erased));
               else
                  this->GetTagsInner().Merge(tag->GetTag(), NestIntentOf(tag, tag_erased));
            }
            else {
               decltype(auto) tag_erased = reinterpret_cast<Tag const&>(tag.what);
               if (found)
                  found.GetVal().Insert(NestIntentOf(tag, tag_erased));
               else
                  this->GetTagsInner().Merge(tag->GetTag(), NestIntentOf(tag, tag_erased));
            }
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

      ///                                                                     
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
         else if constexpr (CT::DefineTag<Decay<A>>) {
            // We are interested in specific tags                       
            auto found = self.GetTagsInner().Find(MetaTagOf<Decay<A>>());
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

#if defined(LANGULUS_EXPORT_ALL) or defined(LANGULUS_EXPORT_ANNIES)
namespace Langulus::Annies::Component
{
   extern template void  Inner::NeatBase::ConstructDefault<Neat>(this Neat&& self) noexcept;
   //extern template void  Inner::NeatBase::Absorb<Many, Many>(this Many& self, Many&& from);
   extern template void  Inner::NeatBase::Destroy<Neat>(this Neat& self);
   extern template void  Inner::NeatBase::Free<true, Neat>(this Neat& self);
   extern template void  Inner::NeatBase::Free<false, Neat>(this Neat& self);
   extern template void  Inner::NeatBase::Keep<Neat>(this Neat& self);
   //extern template Many& Inner::NeatBase::AssignAbsorb<Many, Many>(this Many& self, Many&& rhs);
   extern template bool  Inner::NeatBase::IsMissingDeep<Neat>(this Neat const& self) noexcept;
}

namespace Langulus::Annies::Component
{
   extern template void  Inner::TManyBase<Recipe>::ConstructDefault<TMany<Recipe>>(this TMany<Recipe>&& self) noexcept;
   extern template void  Inner::TManyBase<Recipe>::Absorb<TMany<Recipe>, TMany<Recipe>>(this TMany<Recipe>& self, TMany<Recipe>&& from);
   extern template void  Inner::TManyBase<Recipe>::Destroy<TMany<Recipe>>(this TMany<Recipe>& self);
   extern template void  Inner::TManyBase<Recipe>::Free<true, TMany<Recipe>>(this TMany<Recipe>& self);
   extern template void  Inner::TManyBase<Recipe>::Free<false, TMany<Recipe>>(this TMany<Recipe>& self);
   extern template void  Inner::TManyBase<Recipe>::Keep<TMany<Recipe>>(this TMany<Recipe>& self);
   extern template TMany<Recipe>& Inner::TManyBase<Recipe>::AssignAbsorb<TMany<Recipe>, TMany<Recipe>>(this TMany<Recipe>& self, TMany<Recipe>&& rhs);
   extern template bool  Inner::TManyBase<Recipe>::IsMissingDeep<TMany<Recipe>>(this TMany<Recipe> const& self) noexcept;

   extern template void  Assignment<false, 0>::PrepareForReconstruction<TMany<Recipe>>(this TMany<Recipe>&);
   extern template bool  Assignment<false, 0>::PrepareForReassignment<TMany<Recipe>>(this TMany<Recipe>&);

   extern template bool   Comparison<false, true, 0>::CompareEqual<0, TMany<Recipe>, TMany<Recipe>>(this TMany<Recipe> const&, TMany<Recipe> const&);
   extern template bool   Comparison<false, true, 0>::CompareHashes<TMany<Recipe>, TMany<Recipe>>(this TMany<Recipe> const&, TMany<Recipe> const&);
   extern template size_t Comparison<false, true, 0>::Matches<TMany<Recipe>, TMany<Recipe>>(this TMany<Recipe> const&, TMany<Recipe> const&);

   extern template size_t Deepening<Many>::GetCountDeep<TMany<Recipe>>(this TMany<Recipe> const&, size_t);
   extern template size_t Deepening<Many>::GetReservedDeep<TMany<Recipe>>(this TMany<Recipe> const&, size_t);
   //extern template Many&  Deepening<Many>::Deepen<TMany<Recipe>>(this TMany<Recipe>&);

   extern template void Removal<0>::Trim<TMany<Recipe>>(this TMany<Recipe>&, size_t) noexcept;
   extern template void Removal<0>::Clear<TMany<Recipe>>(this TMany<Recipe>&);
   extern template void Removal<0>::Reset<TMany<Recipe>>(this TMany<Recipe>&);
}

namespace Langulus::Annies::Component
{
   extern template void  Inner::TManyBase<Tag>::ConstructDefault<TMany<Tag>>(this TMany<Tag>&& self) noexcept;
   extern template void  Inner::TManyBase<Tag>::Absorb<TMany<Tag>, TMany<Tag>>(this TMany<Tag>& self, TMany<Tag>&& from);
   extern template void  Inner::TManyBase<Tag>::Destroy<TMany<Tag>>(this TMany<Tag>& self);
   extern template void  Inner::TManyBase<Tag>::Free<true, TMany<Tag>>(this TMany<Tag>& self);
   extern template void  Inner::TManyBase<Tag>::Free<false, TMany<Tag>>(this TMany<Tag>& self);
   extern template void  Inner::TManyBase<Tag>::Keep<TMany<Tag>>(this TMany<Tag>& self);
   extern template TMany<Tag>& Inner::TManyBase<Tag>::AssignAbsorb<TMany<Tag>, TMany<Tag>>(this TMany<Tag>& self, TMany<Tag>&& rhs);
   extern template bool  Inner::TManyBase<Tag>::IsMissingDeep<TMany<Tag>>(this TMany<Tag> const& self) noexcept;

   extern template void  Assignment<false, 0>::PrepareForReconstruction<TMany<Tag>>(this TMany<Tag>&);
   extern template bool  Assignment<false, 0>::PrepareForReassignment<TMany<Tag>>(this TMany<Tag>&);

   extern template bool   Comparison<false, true, 0>::CompareEqual<0, TMany<Tag>, TMany<Tag>>(this TMany<Tag> const&, TMany<Tag> const&);
   extern template bool   Comparison<false, true, 0>::CompareHashes<TMany<Tag>, TMany<Tag>>(this TMany<Tag> const&, TMany<Tag> const&);
   extern template size_t Comparison<false, true, 0>::Matches<TMany<Tag>, TMany<Tag>>(this TMany<Tag> const&, TMany<Tag> const&);

   extern template size_t Deepening<Many>::GetCountDeep<TMany<Tag>>(this TMany<Tag> const&, size_t);
   extern template size_t Deepening<Many>::GetReservedDeep<TMany<Tag>>(this TMany<Tag> const&, size_t);
   //extern template Many&  Deepening<Many>::Deepen<TMany<Recipe>>(this TMany<Recipe>&);

   extern template void Removal<0>::Trim<TMany<Tag>>(this TMany<Tag>&, size_t) noexcept;
   extern template void Removal<0>::Clear<TMany<Tag>>(this TMany<Tag>&);
   extern template void Removal<0>::Reset<TMany<Tag>>(this TMany<Tag>&);
}

namespace Langulus::Annies::Component
{
   extern template void  Inner::TManyBase<Many>::ConstructDefault<TMany<Many>>(this TMany<Many>&& self) noexcept;
   extern template void  Inner::TManyBase<Many>::Absorb<TMany<Many>, TMany<Many>>(this TMany<Many>& self, TMany<Many>&& from);
   extern template void  Inner::TManyBase<Many>::Destroy<TMany<Many>>(this TMany<Many>& self);
   extern template void  Inner::TManyBase<Many>::Free<true, TMany<Many>>(this TMany<Many>& self);
   extern template void  Inner::TManyBase<Many>::Free<false, TMany<Many>>(this TMany<Many>& self);
   extern template void  Inner::TManyBase<Many>::Keep<TMany<Many>>(this TMany<Many>& self);
   extern template TMany<Many>& Inner::TManyBase<Many>::AssignAbsorb<TMany<Many>, TMany<Many>>(this TMany<Many>& self, TMany<Many>&& rhs);
   extern template bool  Inner::TManyBase<Many>::IsMissingDeep<TMany<Many>>(this TMany<Many> const& self) noexcept;

   extern template void  Assignment<false, 0>::PrepareForReconstruction<TMany<Many>>(this TMany<Many>&);
   extern template bool  Assignment<false, 0>::PrepareForReassignment<TMany<Many>>(this TMany<Many>&);

   extern template bool   Comparison<false, true, 0>::CompareEqual<0, TMany<Many>, TMany<Many>>(this TMany<Many> const&, TMany<Many> const&);
   extern template bool   Comparison<false, true, 0>::CompareHashes<TMany<Many>, TMany<Many>>(this TMany<Many> const&, TMany<Many> const&);
   extern template size_t Comparison<false, true, 0>::Matches<TMany<Many>, TMany<Many>>(this TMany<Many> const&, TMany<Many> const&);

   extern template size_t Deepening<Many>::GetCountDeep<TMany<Many>>(this TMany<Many> const&, size_t);
   extern template size_t Deepening<Many>::GetReservedDeep<TMany<Many>>(this TMany<Many> const&, size_t);
   extern template Many&  Deepening<Many>::Deepen<TMany<Many>>(this TMany<Many>&);

   extern template void Removal<0>::Trim<TMany<Many>>(this TMany<Many>&, size_t) noexcept;
   extern template void Removal<0>::Clear<TMany<Many>>(this TMany<Many>&);
   extern template void Removal<0>::Reset<TMany<Many>>(this TMany<Many>&);
}
#else
namespace Langulus::Annies::Component
{
   template LANGULUS_IMPORT() void  Inner::NeatBase::ConstructDefault<Neat>(this Neat&& self) noexcept;
   //extern template void  Inner::NeatBase::Absorb<Many, Many>(this Many& self, Many&& from);
   template LANGULUS_IMPORT() void  Inner::NeatBase::Destroy<Neat>(this Neat& self);
   template LANGULUS_IMPORT() void  Inner::NeatBase::Free<true, Neat>(this Neat& self);
   template LANGULUS_IMPORT() void  Inner::NeatBase::Free<false, Neat>(this Neat& self);
   template LANGULUS_IMPORT() void  Inner::NeatBase::Keep<Neat>(this Neat& self);
   //extern template Many& Inner::NeatBase::AssignAbsorb<Many, Many>(this Many& self, Many&& rhs);
   template LANGULUS_IMPORT() bool  Inner::NeatBase::IsMissingDeep<Neat>(this Neat const& self) noexcept;
}

namespace Langulus::Annies::Component
{
   template LANGULUS_IMPORT() void  Inner::TManyBase<Recipe>::ConstructDefault<TMany<Recipe>>(this TMany<Recipe>&& self) noexcept;
   template LANGULUS_IMPORT() void  Inner::TManyBase<Recipe>::Absorb<TMany<Recipe>, TMany<Recipe>>(this TMany<Recipe>& self, TMany<Recipe>&& from);
   template LANGULUS_IMPORT() void  Inner::TManyBase<Recipe>::Destroy<TMany<Recipe>>(this TMany<Recipe>& self);
   template LANGULUS_IMPORT() void  Inner::TManyBase<Recipe>::Free<true, TMany<Recipe>>(this TMany<Recipe>& self);
   template LANGULUS_IMPORT() void  Inner::TManyBase<Recipe>::Free<false, TMany<Recipe>>(this TMany<Recipe>& self);
   template LANGULUS_IMPORT() void  Inner::TManyBase<Recipe>::Keep<TMany<Recipe>>(this TMany<Recipe>& self);
   template LANGULUS_IMPORT() TMany<Recipe>& Inner::TManyBase<Recipe>::AssignAbsorb<TMany<Recipe>, TMany<Recipe>>(this TMany<Recipe>& self, TMany<Recipe>&& rhs);
   template LANGULUS_IMPORT() bool  Inner::TManyBase<Recipe>::IsMissingDeep<TMany<Recipe>>(this TMany<Recipe> const& self) noexcept;
 
   template LANGULUS_IMPORT() void  Assignment<false, 0>::PrepareForReconstruction<TMany<Recipe>>(this TMany<Recipe>&);
   template LANGULUS_IMPORT() bool  Assignment<false, 0>::PrepareForReassignment<TMany<Recipe>>(this TMany<Recipe>&);
 
   template LANGULUS_IMPORT() bool   Comparison<false, true, 0>::CompareEqual<0, TMany<Recipe>, TMany<Recipe>>(this TMany<Recipe> const&, TMany<Recipe> const&);
   template LANGULUS_IMPORT() bool   Comparison<false, true, 0>::CompareHashes<TMany<Recipe>, TMany<Recipe>>(this TMany<Recipe> const&, TMany<Recipe> const&);
   template LANGULUS_IMPORT() size_t Comparison<false, true, 0>::Matches<TMany<Recipe>, TMany<Recipe>>(this TMany<Recipe> const&, TMany<Recipe> const&);
 
   template LANGULUS_IMPORT() size_t Deepening<Many>::GetCountDeep<TMany<Recipe>>(this TMany<Recipe> const&, size_t);
   template LANGULUS_IMPORT() size_t Deepening<Many>::GetReservedDeep<TMany<Recipe>>(this TMany<Recipe> const&, size_t);
   //extern template Many&  Deepening<Many>::Deepen<TMany<Recipe>>(this TMany<Recipe>&);

   template LANGULUS_IMPORT() void Removal<0>::Trim<TMany<Recipe>>(this TMany<Recipe>&, size_t) noexcept;
   template LANGULUS_IMPORT() void Removal<0>::Clear<TMany<Recipe>>(this TMany<Recipe>&);
   template LANGULUS_IMPORT() void Removal<0>::Reset<TMany<Recipe>>(this TMany<Recipe>&);
}

namespace Langulus::Annies::Component
{
   template LANGULUS_IMPORT() void  Inner::TManyBase<Tag>::ConstructDefault<TMany<Tag>>(this TMany<Tag>&& self) noexcept;
   template LANGULUS_IMPORT() void  Inner::TManyBase<Tag>::Absorb<TMany<Tag>, TMany<Tag>>(this TMany<Tag>& self, TMany<Tag>&& from);
   template LANGULUS_IMPORT() void  Inner::TManyBase<Tag>::Destroy<TMany<Tag>>(this TMany<Tag>& self);
   template LANGULUS_IMPORT() void  Inner::TManyBase<Tag>::Free<true, TMany<Tag>>(this TMany<Tag>& self);
   template LANGULUS_IMPORT() void  Inner::TManyBase<Tag>::Free<false, TMany<Tag>>(this TMany<Tag>& self);
   template LANGULUS_IMPORT() void  Inner::TManyBase<Tag>::Keep<TMany<Tag>>(this TMany<Tag>& self);
   template LANGULUS_IMPORT() TMany<Tag>& Inner::TManyBase<Tag>::AssignAbsorb<TMany<Tag>, TMany<Tag>>(this TMany<Tag>& self, TMany<Tag>&& rhs);
   template LANGULUS_IMPORT() bool  Inner::TManyBase<Tag>::IsMissingDeep<TMany<Tag>>(this TMany<Tag> const& self) noexcept;
 
   template LANGULUS_IMPORT() void  Assignment<false, 0>::PrepareForReconstruction<TMany<Tag>>(this TMany<Tag>&);
   template LANGULUS_IMPORT() bool  Assignment<false, 0>::PrepareForReassignment<TMany<Tag>>(this TMany<Tag>&);
 
   template LANGULUS_IMPORT() bool   Comparison<false, true, 0>::CompareEqual<0, TMany<Tag>, TMany<Tag>>(this TMany<Tag> const&, TMany<Tag> const&);
   template LANGULUS_IMPORT() bool   Comparison<false, true, 0>::CompareHashes<TMany<Tag>, TMany<Tag>>(this TMany<Tag> const&, TMany<Tag> const&);
   template LANGULUS_IMPORT() size_t Comparison<false, true, 0>::Matches<TMany<Tag>, TMany<Tag>>(this TMany<Tag> const&, TMany<Tag> const&);
 
   template LANGULUS_IMPORT() size_t Deepening<Many>::GetCountDeep<TMany<Tag>>(this TMany<Tag> const&, size_t);
   template LANGULUS_IMPORT() size_t Deepening<Many>::GetReservedDeep<TMany<Tag>>(this TMany<Tag> const&, size_t);
   //extern template Many&  Deepening<Many>::Deepen<TMany<Recipe>>(this TMany<Recipe>&);

   template LANGULUS_IMPORT() void Removal<0>::Trim<TMany<Tag>>(this TMany<Tag>&, size_t) noexcept;
   template LANGULUS_IMPORT() void Removal<0>::Clear<TMany<Tag>>(this TMany<Tag>&);
   template LANGULUS_IMPORT() void Removal<0>::Reset<TMany<Tag>>(this TMany<Tag>&);
}

namespace Langulus::Annies::Component
{
   template LANGULUS_IMPORT() void  Inner::TManyBase<Many>::ConstructDefault<TMany<Many>>(this TMany<Many>&& self) noexcept;
   template LANGULUS_IMPORT() void  Inner::TManyBase<Many>::Absorb<TMany<Many>, TMany<Many>>(this TMany<Many>& self, TMany<Many>&& from);
   template LANGULUS_IMPORT() void  Inner::TManyBase<Many>::Destroy<TMany<Many>>(this TMany<Many>& self);
   template LANGULUS_IMPORT() void  Inner::TManyBase<Many>::Free<true, TMany<Many>>(this TMany<Many>& self);
   template LANGULUS_IMPORT() void  Inner::TManyBase<Many>::Free<false, TMany<Many>>(this TMany<Many>& self);
   template LANGULUS_IMPORT() void  Inner::TManyBase<Many>::Keep<TMany<Many>>(this TMany<Many>& self);
   template LANGULUS_IMPORT() TMany<Many>& Inner::TManyBase<Many>::AssignAbsorb<TMany<Many>, TMany<Many>>(this TMany<Many>& self, TMany<Many>&& rhs);
   template LANGULUS_IMPORT() bool  Inner::TManyBase<Many>::IsMissingDeep<TMany<Many>>(this TMany<Many> const& self) noexcept;

   template LANGULUS_IMPORT() void  Assignment<false, 0>::PrepareForReconstruction<TMany<Many>>(this TMany<Many>&);
   template LANGULUS_IMPORT() bool  Assignment<false, 0>::PrepareForReassignment<TMany<Many>>(this TMany<Many>&);

   template LANGULUS_IMPORT() bool   Comparison<false, true, 0>::CompareEqual<0, TMany<Many>, TMany<Many>>(this TMany<Many> const&, TMany<Many> const&);
   template LANGULUS_IMPORT() bool   Comparison<false, true, 0>::CompareHashes<TMany<Many>, TMany<Many>>(this TMany<Many> const&, TMany<Many> const&);
   template LANGULUS_IMPORT() size_t Comparison<false, true, 0>::Matches<TMany<Many>, TMany<Many>>(this TMany<Many> const&, TMany<Many> const&);

   template LANGULUS_IMPORT() size_t Deepening<Many>::GetCountDeep<TMany<Many>>(this TMany<Many> const&, size_t);
   template LANGULUS_IMPORT() size_t Deepening<Many>::GetReservedDeep<TMany<Many>>(this TMany<Many> const&, size_t);
   template LANGULUS_IMPORT() Many&  Deepening<Many>::Deepen<TMany<Many>>(this TMany<Many>&);

   template LANGULUS_IMPORT() void Removal<0>::Trim<TMany<Many>>(this TMany<Many>&, size_t) noexcept;
   template LANGULUS_IMPORT() void Removal<0>::Clear<TMany<Many>>(this TMany<Many>&);
   template LANGULUS_IMPORT() void Removal<0>::Reset<TMany<Many>>(this TMany<Many>&);
}
#endif

#include <Langulus/Annies/Functional/Assign.hpp>
#include <Langulus/Annies/Functional/Deepen.hpp>
#include <Langulus/Annies/Functional/ForEach.hpp>
#include <Langulus/Annies/Functional/Compare.hpp>
#include <Langulus/Annies/Functional/Merge.hpp>
#include <Langulus/Annies/Functional/Insert.hpp>
#include <Langulus/Annies/Functional/Remove.hpp>
#include <Langulus/Annies/Functional/Describe.hpp>
#include <Langulus/Annies/Functional/Convert.hpp>
