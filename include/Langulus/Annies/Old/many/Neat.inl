///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Neat.hpp"
#include "TMany.inl"
#include "TTrait.inl"
#include "Construct.inl"
#include "../maps/TMap.inl"
#include "../one/Ref.inl"
#include "../verbs/Verb.hpp"
#include "../text/Text.hpp"


namespace Langulus::Annies
{

   /// Refer-constructor                                                      
   ///   @param other - neat container to shallow-copy                        
   LANGULUS(INLINED)
   Neat::Neat(Neat const& other)
      : Neat {Refer(other)} {}

   /// Move-constructor                                                       
   ///   @param other - neat container to move                                
   LANGULUS(INLINED)
   Neat::Neat(Neat&& other) noexcept
      : Neat {Move(other)} {}

   /// Intent constructor                                                     
   ///   @param other - the container and intent to use                       
   template<template<class> class S>
   requires CT::Intent<S<Neat>> LANGULUS(INLINED)
   Neat::Neat(S<Neat>&& other)
      : mHash         {other->mHash}
      , mTags       {other.Nest(other->mTags)}
      , mRecipes   {other.Nest(other->mRecipes)}
      , mAnythingElse {other.Nest(other->mAnythingElse)} {
      // Reset remote hash if moving                                    
      if constexpr (ResetsOnMove<S>())
         other->mHash = {};
   }

   /// Tidy up any number of elements sequentially, each element can have     
   /// individual intents. Deep contents are normalized only for CT::Deep.    
   ///   @param t1 - first element and intent                                 
   ///   @param tn... - the rest of the elements (optional, can have intents) 
   template<class T1, class...TN>
   requires CT::UnfoldInsertable<T1, TN...> LANGULUS(INLINED)
   Neat::Neat(T1&& t1, TN&&...tn) : Neat {} {
      Insert(Forward<T1>(t1), Forward<TN>(tn)...);
   }

   /// Intent assignment with another normalized descriptor                   
   ///   @param other - normalized descriptor and intent to assign            
   ///   @return a reference to this descriptor                               
   template<template<class> class S>
   requires CT::Intent<S<Neat>> LANGULUS(INLINED)
   Neat& Neat::operator = (S<Neat>&& other) {
      mTags       = other.Nest(other->mTags);
      mRecipes   = other.Nest(other->mRecipes);
      mAnythingElse = other.Nest(other->mAnythingElse);
      mHash         = other->mHash;

      // Reset remote hash if moving                                    
      if constexpr (ResetsOnMove<S>())
         other->mHash = {};
      return *this;
   }

   /// Clear the container without deallocating                               
   LANGULUS(INLINED) void Neat::Clear() {
      mHash = {};
      mTags.Clear();
      mRecipes.Clear();
      mAnythingElse.Clear();
   }
   
   /// Clear and deallocate the container                                     
   LANGULUS(INLINED) void Neat::Reset() {
      mHash = {};
      mTags.Reset();
      mRecipes.Reset();
      mAnythingElse.Reset();
   }

   /// Get the hash of a neat container (and cache it)                        
   ///   @attention missing elements never participate in hashing/comparison  
   ///   @return the hash                                                     
   LANGULUS(INLINED)
   Hash Neat::GetHash() const {
      if (mHash)
         return mHash;

      if (IsEmpty())
         mHash = {};
      else
         mHash = HashOf(mTags, mRecipes, mAnythingElse);
      return mHash;
   }

   /// Check if the container is empty                                        
   ///   @return true if empty                                                
   LANGULUS(INLINED)
   bool Neat::IsEmpty() const noexcept {
      return mTags.IsEmpty()
         and mRecipes.IsEmpty()
         and mAnythingElse.IsEmpty();
   }
   
   /// Check if the container has missing entries, nest-scan                  
   ///   @return true if there's at least one missing entry                   
   LANGULUS(INLINED)
   bool Neat::IsMissingDeep() const {
      return mTags.IsKeyMissingDeep() or mTags.IsValueMissingDeep()
          or mRecipes.IsKeyMissingDeep() or mRecipes.IsValueMissingDeep()
          or mAnythingElse.IsKeyMissingDeep() or mAnythingElse.IsValueMissingDeep();
   }

   /// Check if construct contains executable elements                        
   ///   @return true if there's at least one executable entry                
   LANGULUS(INLINED)
   bool Neat::IsExecutable() const noexcept {
      return mTags.IsKeyExecutableDeep() or mTags.IsValueExecutableDeep()
          or mRecipes.IsKeyExecutableDeep() or mRecipes.IsValueExecutableDeep()
          or mAnythingElse.IsKeyExecutableDeep() or mAnythingElse.IsValueExecutableDeep();
   }

   /// Check if the container is not empty                                    
   ///   @return true if not empty                                            
   LANGULUS(INLINED)
   Neat::operator bool() const noexcept {
      return not IsEmpty();
   }

   /// Compare neat container                                                 
   ///   @param rhs - the container to compare with                           
   ///   @return true if descriptors match                                    
   LANGULUS(INLINED)
   bool Neat::operator == (Neat const& rhs) const {
      if (GetHash() != rhs.GetHash())
         return false;

      return mTags == rhs.mTags
         and mRecipes == rhs.mRecipes
         and mAnythingElse == rhs.mAnythingElse;
   }

   /// Merge two neat containers                                              
   ///   @param rhs - the container to merge                                  
   LANGULUS(INLINED)
   void Neat::Merge(Neat const& rhs) {
      if (not rhs)
         return;

      mTags       += rhs.mTags;
      mRecipes   += rhs.mRecipes;
      mAnythingElse += rhs.mAnythingElse;
      mHash = {};
   }

   /// Get list of traits, corresponding to a static trait                    
   ///   @tparam T - trait type to search for                                 
   ///   @return the trait list, or nullptr if no such list exists            
   ///   @attention the list can be empty, if trait was provided with no      
   ///              contents                                                  
   template<CT::Tag T> LANGULUS(INLINED)
   auto Neat::GetTags() -> TagList* {
      return GetTags(MetaTagOf<T>());
   }

   /// Get list of traits, corresponding to a static trait (const)            
   ///   @tparam T - trait type to search for                                 
   ///   @return the trait list, or nullptr if no such list exists            
   ///   @attention the list can be empty, if trait was provided with no      
   ///              contents                                                  
   template<CT::Tag T> LANGULUS(INLINED)
   auto Neat::GetTags() const -> const TagList* {
      return GetTags(MetaTagOf<T>());
   }
   
   /// Get list of traits, corresponding to a type                            
   ///   @param t - trait type to search for                                  
   ///   @return the trait list, or nullptr if no such list exists            
   ///   @attention the list can be empty, if trait was provided with no      
   ///              contents                                                  
   LANGULUS(INLINED)
   auto Neat::GetTags(TMeta t) -> TagList* {
      LglsAssumeUser(t, "Can't get invalid trait");
      auto found = mTags.Find(t);
      return found ? &mTags.GetValue(found) : nullptr;
   }

   /// Get list of traits, corresponding to a type (const)                    
   ///   @param t - trait type to search for                                  
   ///   @return the trait list, or nullptr if no such list exists            
   ///   @attention the list can be empty, if trait was provided with no      
   ///              contents                                                  
   LANGULUS(INLINED)
   auto Neat::GetTags(TMeta t) const -> const TagList* {
      return const_cast<Neat*>(this)->GetTags(t);
   }
   
   /// Get list of data, corresponding to a static type                       
   ///   @tparam T - type to search for                                       
   ///   @return the data list, or nullptr if no such list exists             
   template<CT::NotVoid T> LANGULUS(INLINED)
   auto Neat::GetData() -> TailList* {
      return GetData(MetaDataOf<Decay<T>>());
   }

   /// Get list of data, corresponding to a static type (const)               
   ///   @tparam T - type to search for                                       
   ///   @return the data list, or nullptr if no such list exists             
   template<CT::NotVoid T> LANGULUS(INLINED)
   auto Neat::GetData() const -> const TailList* {
      return GetData(MetaDataOf<Decay<T>>());
   }
      
   /// Get list of data, corresponding to a type                              
   ///   @param d - type to search for                                        
   ///   @return the data list, or nullptr if no such list exists             
   LANGULUS(INLINED)
   auto Neat::GetData(DMeta d) -> TailList* {
      auto found = mAnythingElse.Find(d ? d->mOrigin : nullptr);
      return found ? &mAnythingElse.GetValue(found) : nullptr;
   }

   /// Get list of data, corresponding to a type (const)                      
   ///   @param d - type to search for                                        
   ///   @return the data list, or nullptr if no such list exists             
   LANGULUS(INLINED)
   auto Neat::GetData(DMeta d) const -> const TailList* {
      return const_cast<Neat*>(this)->GetData(d);
   }

   /// Get list of constructs, corresponding to a static type                 
   ///   @tparam T - type to search for                                       
   ///   @return the construct list, or nullptr if no such list exists        
   template<CT::NotVoid T> LANGULUS(INLINED)
   auto Neat::GetConstructs() -> ConstructList* {
      return GetConstructs(MetaDataOf<Decay<T>>());
   }

   /// Get list of constructs, corresponding to a static type (const)         
   ///   @tparam T - type to search for                                       
   ///   @return the construct list, or nullptr if no such list exists        
   template<CT::NotVoid T> LANGULUS(INLINED)
   auto Neat::GetConstructs() const -> const ConstructList* {
      return GetConstructs(MetaDataOf<Decay<T>>());
   }
   
   /// Get list of constructs, corresponding to a type                        
   ///   @param d - type to search for                                        
   ///   @return the construct list, or nullptr if no such list exists        
   LANGULUS(INLINED)
   auto Neat::GetConstructs(DMeta d) -> ConstructList* {
      auto found = mRecipes.Find(d ? d->mOrigin : nullptr);
      return found ? &mRecipes.GetValue(found) : nullptr;
   }

   /// Get list of constructs, corresponding to a type (const)                
   ///   @param d - type to search for                                        
   ///   @return the construct list, or nullptr if no such list exists        
   LANGULUS(INLINED)
   auto Neat::GetConstructs(DMeta d) const -> const ConstructList* {
      return const_cast<Neat*>(this)->GetConstructs(d);
   }

   /// Find data in constructs or tail, that casts to T                       
   ///   @tparam T - type requirement                                         
   ///   @return the first type that matches                                  
   template<CT::NotVoid T> LANGULUS(INLINED)
   DMeta Neat::FindType() const {
      return FindType(MetaDataOf<T>());
   }

   /// Find data in constructs or tail, that casts to a type                  
   ///   @param type - type requirement                                       
   ///   @return the first type that matches                                  
   inline DMeta Neat::FindType(DMeta type) const {
      DMeta primitive;
      bool  ambiguous = false;

      ForEachRecipe([&](Recipe const& c) noexcept {
         if (not c.CastsTo(type))
            return;

         if (not primitive)
            primitive = c.GetType();
         else
            ambiguous = true;
      });

      ForEachTail([&](const Block<>& c) noexcept {
         if (not c.CastsToMeta(type))
            return;

         if (not primitive)
            primitive = c.GetType();
         else
            ambiguous = true;
      });

      if (ambiguous) {
         Logger::Warning(
            "Multiple primitives defined in a single Neat on FindData - "
            "all except the first `", primitive, "` will be ignored"
         );
      }

      return primitive;
   }

   /// Set a default trait, if such wasn't already set                        
   //TODO isn't this like simply merge?? also make merge test only by trait id when merging traits!
   ///   @tparam T - trait to set                                             
   ///   @param value - the value to assign                                   
   template<CT::Tag T> LANGULUS(INLINED)
   void Neat::SetDefaultTag(CT::NotVoid auto&& value) {
      auto found = GetTags<T>();
      if (found and *found)
         return;

      AddTag(Abandon(T {Forward<Deref<decltype(value)>>(value)}));
   }

   /// Extract a trait from the descriptor                                    
   ///   @tparam T... - trait(s) we're searching for                          
   ///   @param values - [out] where to save the value, if found              
   ///   @return true if value changed                                        
   template<CT::Tag...T> LANGULUS(INLINED)
   bool Neat::ExtractTag(CT::NotVoid auto&...values) const {
      return (ExtractTagInner<T>(values...) or ...);
   }
   
   /// Extract a trait from the descriptor                                    
   ///   @tparam T - trait we're searching for                                
   ///   @param values - [out] where to save the value, if found              
   ///   @return true if value changed                                        
   template<CT::Tag T> LANGULUS(INLINED)
   bool Neat::ExtractTagInner(CT::NotVoid auto&...values) const {
      auto found = GetTags<T>();
      if (found) {
         return ExtractTagInner(
            *found, Sequence<sizeof...(values)>::Expand, values...
         );
      }
      return false;
   }

   ///                                                                        
   template<size_t...IDX>
   bool Neat::ExtractTagInner(
      const TagList& found, ExpandedSequence<IDX...>, CT::NotVoid auto&...values
   ) const {
      return (ExtractTagInnerInner<IDX>(found, values) or ...);
   }
   
   ///                                                                        
   template<size_t IDX>
   bool Neat::ExtractTagInnerInner(const TagList& found, CT::NotVoid auto& value) const {
      if (IDX >= found.GetCount())
         return false;

      using D = Deref<decltype(value)>;
      if constexpr (CT::Deep<D>) {
         value = found[IDX];
         return true;
      }
      else try {
         value = found[IDX].AsCast<D>();
         return true;
      }
      catch (...) {}
      return false;
   }
   
   /// Extract data of an exact type, doing only pointer arithmetic           
   ///   @param value - [out] where to save the value(s), if found            
   ///   @return the number of extracted values (always 1 if not an array)    
   LANGULUS(INLINED)
   size_t Neat::ExtractData(CT::NotVoid auto& value) const {
      using D = Deref<decltype(value)>;
      if constexpr (CT::Array<D>) {
         // Fill a bounded array                                        
         auto found = GetData<Decay<D>>();
         if (found) {
            size_t scanned = 0;
            for (auto& group : *found) {
               const auto toscan = ::std::min(ExtentOf<D> - scanned, group.GetCount());
               for (size_t i = 0; i < toscan; ++i) {
                  //TODO can be optimized-out for POD
                  value[scanned + i] = group.template Get<Deext<D>>(i);
               }

               scanned += toscan;
               if (scanned >= ExtentOf<D>)
                  return ExtentOf<D>;
            }

            return scanned;
         }
      }
      else {
         // Fill a single value                                         
         auto found = GetData<Decay<D>>();
         if (found) {
            value = (*found)[0].template Get<D>();
            return 1;
         }
      }

      return 0;
   }
   
   /// Extract any data that is convertible to D                              
   ///   @param value - [out] where to save the value, if found               
   ///   @return the number of extracted values (always 1 if not an array)    
   inline size_t Neat::ExtractDataAs(CT::NotVoid auto& value) const {
      using D = Deref<decltype(value)>;

      if constexpr (CT::Array<D>) {
         // Fill a bounded array                                        
         size_t scanned = 0;
         for (auto pair : mAnythingElse) {
            for (auto& group : pair.GetValue()) {
               const auto toscan = ::std::min(ExtentOf<D> - scanned, group.GetCount());
               for (size_t i = 0; i < toscan; ++i) {
                  //TODO can be optimized-out for POD
                  try {
                     value[scanned] = group.template AsCast<Deext<D>>(i);
                     ++scanned;
                  }
                  catch (...) {}
               }

               if (scanned >= ExtentOf<D>)
                  return ExtentOf<D>;
            }
         }

         return scanned;
      }
      else {
         // Fill a single value                                         
         for (auto pair : mAnythingElse) {
            for (auto& group : pair.GetValue()) {
               try {
                  value = group.template AsCast<D>();
                  return 1;
               }
               catch (...) {}
            }
         }
      }

      return 0;
   }

   /// Push and sort anything, with or without intents                        
   ///   @attention hash will be recomputed on demand                         
   ///   @param item - the thing to push, as well as the semantic to use      
   LANGULUS(INLINED)
   void Neat::InsertInner(auto&& item) {
      using S = IntentOf<decltype(item)>;
      using T = TypeOf<S>;

      if constexpr (CT::DefineTag<T>) {
         // Insert trait to its bucket                                  
         AddTag(S::Nest(item));
      }
      else if constexpr (CT::Same<T, CMeta>) {
         // Expand the constant, and push it                            
         operator << (Clone(Block<> {{}, DeintCast(item)}));
      }
      else if constexpr (CT::Recipe<T>) {
         // Construct's arguments are always Neat                       
         AddRecipe(S::Nest(item));
      }
      else if constexpr (CT::VerbBased<T>) {
         // Verbs have to respect their ordering, they all go into the  
         // same mAnythingElse[AVerb] bucket                            
         AddVerb(S::Nest(item));
      }
      else if constexpr (CT::Owned<T>) {
         // Make sure we strip any owning handle away                   
         InsertInner(S::Nest(DeintCast(item).Get()));
      }
      else if constexpr (CT::Deep<T>) {
         DEBUGGERY((DeintCast(item).TrackingReport(nullptr,
            "Inserting in Neat container with intent: ", NameOf<S>())));

         // Pushing an entire pack, make sure we flatten it, if it is   
         // allowed                                                     
         const auto meta = DeintCast(item).GetUnconstrainedState()
            ? MetaDataOf<Decay<T>>()
            : DeintCast(item).GetType();
         const auto found = mAnythingElse.FindIt(meta);

         if (found)
            found.GetValue() << S::Nest(item);
         else
            mAnythingElse.Insert(meta, S::Nest(item));
      }
      else {
         // RHS is nothing special, just add it as it is                
         const auto meta = MetaDataOf<Decay<T>>();
         const auto found = mAnythingElse.FindIt(meta);

         if (found)
            found.GetValue() << Messy {S::Nest(item)};
         else
            mAnythingElse.Insert(meta, Messy {S::Nest(item)});
      }

      // Demand a new hash on the next compare                          
      mHash = {};
   }

   /// Insert an element, array of elements, or another set                   
   ///   @param item - the argument to unfold and insert, can have intent     
   ///   @return the number of inserted elements after unfolding              
   LANGULUS(INLINED)
   size_t Neat::UnfoldInsert(auto&& item) {
      using S = IntentOf<decltype(item)>;
      using T = TypeOf<S>;

      if constexpr (CT::Array<T>) {
         if constexpr (CT::StringLiteral<T>) {
            // Implicitly convert string literals to Text containers    
            InsertInner(Text {S::Nest(item)});
            return 1;
         }
         else {
            // Unfold-insert anything else                              
            size_t inserted = 0;
            for (auto& key : item)
               inserted += UnfoldInsert(S::Nest(key));
            return inserted;
         }
      }
      else if constexpr (CT::Neat<T>) {
         // Insert Neat, by inserting each element from it              
         size_t inserted = 0;

         DeintCast(item).ForEach([&](Many const& subitem) {
            inserted += UnfoldInsert(
               S::Nest(const_cast<Many&>(subitem)));
         });
         return inserted;
      }
      else if constexpr (CT::Deep<T>) {
         // Push anything deep here, flattening it, unless it has state 
         if (DeintCast(item).GetUnconstrainedState()) {
            // Item has state, so just push it as it is, to preserve it 
            InsertInner(S::Nest(item));
            return 1;
         }
         else if (DeintCast(item).IsDeep()) {
            // Item is deep, flatten it                                 
            size_t inserted = 0;
            DeintCast(item).ForEach([&](Many const& subitem) {
               inserted += UnfoldInsert(
                  S::Nest(const_cast<Many&>(subitem)));
            });
            return inserted;
         }
         else {
            // Item is not deep, filter based on type                   
            const auto inserted = DeintCast(item).ForEach(
               [&](Recipe const& c) {
                  InsertInner(S::Nest(const_cast<Recipe&>(c)));
               },
               [&](Neat const& neat) {
                  UnfoldInsert(S::Nest(const_cast<Neat&>(neat)));
               },
               [&](Tag const& trait) {
                  InsertInner(S::Nest(const_cast<Tag&>(trait)));
               },
               [&](Verb const& verb) {
                  InsertInner(S::Nest(const_cast<Verb&>(verb)));
               },
               [&](DMeta const& meta) { InsertInner(meta); },
               [&](TMeta const& meta) { InsertInner(meta); },
               [&](CMeta const& meta) { InsertInner(meta); }
            );

            if (not inserted) {
               // Item contains nothing special, just add it as it is   
               InsertInner(S::Nest(item));
               return 1;
            }

            return inserted;
         }
      }
      else {
         // Some of the arguments might still be used directly to       
         // make an element, forward these to standard insertion here   
         InsertInner(S::Nest(item));
         return 1;
      }
   }

   /// Push and sort anything, with or without intents                        
   ///   @param key - the key and intent to add                               
   ///   @return 1 if pair was inserted, zero otherwise                       
   template<class T1, class...TN> LANGULUS(INLINED)
   size_t Neat::Insert(T1&& t1, TN&&...tn) {
      size_t inserted = 0;
        inserted += UnfoldInsert(Forward<T1>(t1));
      ((inserted += UnfoldInsert(Forward<TN>(tn))), ...);
      return inserted;
   }
   
   /// Push and sort anything, with or without intents                        
   ///   @param rhs - the pair and intent to insert                           
   ///   @return a reference to this table for chaining                       
   template<class T> LANGULUS(INLINED)
   Neat& Neat::operator << (T&& rhs) {
      Insert(Forward<T>(rhs));
      return *this;
   }

   /// Insert anything if it doesn't exist already, with or without intents   
   ///   @attention hash will be recomputed on demand, if anything was pushed 
   ///   @param rhs - the item and intent to insert                           
   ///   @return a reference to this Neat container                           
   LANGULUS(INLINED)
   Neat& Neat::operator <<= (auto&& rhs) {
      using S = IntentOf<decltype(rhs)>;
      using T = TypeOf<S>;
      decltype(auto) rhsd = DeintCast(rhs);

      if constexpr (CT::DefineTag<T>) {
         // Check if the trait already exists, before pushing it        
         if (not GetTags(rhsd.GetTag()))
            return operator << (S::Nest(rhs));
      }
      else if constexpr (CT::Same<T, TMeta>) {
         // Check if the trait already exists, before pushing it        
         if (not GetTags(rhsd))
            return operator << (S::Nest(rhs));
      }
      else if constexpr (CT::Recipe<T>) {
         // Check if the construct already exists, before pushing it    
         if (not GetConstructs(rhsd.GetType()))
            return operator << (S::Nest(rhs));
      }
      else if constexpr (CT::Same<T, DMeta>) {
         // Check if the construct already exists, before pushing it    
         if (not GetData(rhsd))
            return operator << (S::Nest(rhs));
      }
      else if constexpr (CT::Deep<T>) {
         // Check anything deep here, flattening it, unless it is OR    
         if (rhsd.GetUnsconstrainedState())
            return operator << (S::Nest(rhs));

         rhsd.ForEach([&](Many const& group) {
            if (not GetData(group.GetType()))
               operator << (S::Nest(const_cast<Many&>(group)));
         });
      }
      else {
         // Check anything else                                         
         if (not GetData<T>())
            return operator << (S::Nest(rhs));
      }

      return *this;
   }
   
   /// Set a tagged argument inside constructor by moving                     
   ///   @attention hash will be recomputed on demand                         
   ///   @param trait - trait to set                                          
   ///   @param index - the index we're interested with if repeated           
   ///   @return a reference to this construct for chaining                   
   Neat& Neat::SetTag(CT::DefineTag auto&& trait, size_t index) {
      const auto meta = trait.GetTag();
      auto found = mTags.BranchOut().FindIt(meta);

      if (found) {
         // A group of similar traits was found                         
         auto& group = found->mValue;
         if (group.GetCount() > index)
            group[index] = Forward<Tag>(trait);
         else
            group << Forward<Tag>(trait);
      }
      else {
         // If reached, a new trait group to be inserted                
         mTags.Insert(meta, Forward<Tag>(trait));
      }

      mHash = {};
      return *this;
   }

   /// Push a trait to the appropriate bucket                                 
   ///   @attention this is an inner function that doesn't affect the hash    
   ///   @param messy - the trait and intent to insert                        
   LANGULUS(INLINED)
   void Neat::AddTag(CT::Intent auto&& messy) {
      using S = IntentOf<decltype(messy)>;
      using T = TypeOf<S>;

      if constexpr (CT::DefineTag<T>) {
         // Insert a trait with contents                                
         const auto meta = messy->GetTag();
         auto found = mTags.BranchOut().FindIt(meta);
         if (found)
            found.GetValue() << messy.Forward();
         else
            mTags.Insert(meta, messy.Forward());
      }
      else if constexpr (CT::Exact<T, TMeta>) {
         // Insert trait without contents                               
         const auto trait = *messy;
         auto found = mTags.BranchOut().FindIt(trait);
         if (found)
            found.GetValue() << Tag::FromMeta(trait);
         else
            mTags.Insert(trait, Tag::FromMeta(trait));
      }
      else static_assert(false, "Can't insert trait");
   }
   
   /// Push verbs to the appropriate bucket                                   
   ///   @attention this is an inner function that doesn't affect the hash    
   ///   @param verb - the verb (and intent) to insert                        
   LANGULUS(INLINED)
   void Neat::AddVerb(CT::Intent auto&& verb) {
      using S = IntentOf<decltype(verb)>;
      using V = TypeOf<S>;
      static_assert(CT::VerbBased<V>, "V isn't a verb");

      // Make sure verb's ID is populated before pushing it             
      if constexpr (CT::Verb<V>)
         (void) verb->GetVerb();

      // Insert deep data - we have to flatten it                       
      const auto meta = MetaDataOf<Decay<V>>();
      auto found = mAnythingElse.BranchOut().FindIt(meta);
      if (found)
         found.GetValue() << verb.Forward();
      else
         mAnythingElse.Insert(meta, verb.Forward());
   }

   /// Push a construct to the appropriate bucket                             
   ///   @attention this is an inner function that doesn't affect the hash    
   ///   @param messy - the construct and intent to insert                    
   LANGULUS(INLINED)
   void Neat::AddRecipe(CT::Intent auto&& messy) {
      using S = IntentOf<decltype(messy)>;
      using T = TypeOf<S>;

      if constexpr (CT::Recipe<T>) {
         const auto meta = messy->GetType()
            ? messy->GetType()->mOrigin
            : nullptr;
         const auto found = mRecipes.BranchOut().FindIt(meta);

         if (found)
            found.GetValue() << messy.Forward();
         else
            mRecipes.Insert(meta, messy.Forward());
      }
      else static_assert(false, "Can't insert construct");
   }

   /// Get a tagged argument inside constructor                               
   ///   @param meta - trait to search for                                    
   ///   @param index - the index we're interested in, if repeated            
   ///   @return selected data or nullptr if none was found                   
   ///   @attention if not nullptr, returned Many might contain a Neat        
   inline auto Neat::GetTag(TMeta meta, size_t index) const -> const Tag* {
      const auto found = mTags.FindIt(meta);
      if (found and found.GetValue().GetCount() > index)
         return &(found.GetValue()[index]);
      return nullptr;
   }

   /// Get traits from constructor                                            
   ///   @tparam T - the type of trait to search for                          
   ///   @return selected data or nullptr if none was found                   
   ///   @attention if not nullptr, returned Many might contain a Neat        
   template<CT::Tag T> LANGULUS(INLINED)
   auto Neat::GetTag(size_t index) const -> const Tag* {
      return Get(MetaTagOf<T>(), index);
   }

   /// Iterate through all relevant bucketed items                            
   /// Depending on F's arguments, different portions of the container will   
   /// be iterated. Use a generic Block/Many type to iterate everything.      
   ///   @tparam MUTABLE - whether changes inside container are allowed       
   ///   @param call - the function(s) to execute for each element            
   ///   @return the number of executions of 'call'                           
   template<bool MUTABLE> LANGULUS(INLINED)
   size_t Neat::ForEach(auto&&...call) {
      static_assert(sizeof...(call) > 0, "No iterators in ForEach");
      if (IsEmpty())
         return 0;

      size_t result = 0;
      (void) (... or (0 != (result = 
         ForEachInner<MUTABLE>(Forward<Deref<decltype(call)>>(call))
      )));
      return result;
   }

   ///                                                                        
   LANGULUS(INLINED)
   size_t Neat::ForEach(auto&&...call) const {
      static_assert(sizeof...(call) > 0, "No iterators in ForEach");
      return const_cast<Neat*>(this)->template
         ForEach<false>(Forward<Deref<decltype(call)>>(call)...);
   }
   
   /// Iterate through all relevant bucketed items, inclusively               
   ///   @tparam MUTABLE - whether changes inside container are allowed       
   ///   @param call - the function(s) to execute for each element            
   ///   @return the number of executions of all calls                        
   template<bool MUTABLE> LANGULUS(INLINED)
   size_t Neat::ForEachDeep(auto&&...call) {
      static_assert(sizeof...(call) > 0, "No iterators in ForEachDeep");
      size_t executions = 0;
      ((executions += ForEachInner<MUTABLE>(
         Forward<Deref<decltype(call)>>(call))), ...);
      return executions;
   }

   /// Neat containers are always flat, so deep iteration is same as flat one 
   LANGULUS(INLINED)
   size_t Neat::ForEachDeep(auto&&...call) const {
      static_assert(sizeof...(call) > 0, "No iterators in ForEachDeep");
      return const_cast<Neat*>(this)->template
         ForEachDeep<false>(Forward<Deref<decltype(call)>>(call)...);
   }
   
   /// Iterate through all relevant bucketed items                            
   /// Depending on F's arguments, different portions of the container will   
   /// be iterated. Use a generic Block/Many type to iterate everything.      
   ///   @tparam MUTABLE - whether changes inside container are allowed       
   ///   @tparam F - the function signature (deducible)                       
   ///   @param call - the function to execute for each element               
   ///   @return the number of executions of 'call'                           
   template<bool MUTABLE, class F> LANGULUS(INLINED)
   size_t Neat::ForEachInner(F&& call) {
      using A = ArgumentOf<F>;
      static_assert(CT::Slab<A> or CT::Constant<Deptr<A>> or MUTABLE,
         "Non constant iterator for constant Neat block");

      if constexpr (CT::Deep<A>) {
         // Iterate everything                                          
         size_t counter = 0;
         counter += ForEachTag<MUTABLE>(call);
         counter += ForEachRecipe<MUTABLE>(call);
         counter += ForEachTail<MUTABLE>(call);
         return counter;
      }
      else if constexpr (CT::DefineTag<A>) {
         // Iterate traits only                                         
         return ForEachTag<MUTABLE>(Forward<F>(call));
      }
      else if constexpr (CT::Recipe<A>) {
         // Iterate constructs only                                     
         return ForEachRecipe<MUTABLE>(Forward<F>(call));
      }
      else {
         // Iterate anything else inside the tail                       
         return ForEachTail<MUTABLE>(Forward<F>(call));
      }
   }

   ///                                                                        
   template<class F> LANGULUS(INLINED)
   size_t Neat::ForEachInner(F&& call) const {
      return const_cast<Neat*>(this)->template 
         ForEach<false>(Forward<F>(call));
   }

   /// Iterate all traits                                                     
   /// You can provide a static Tag iterator, to filter based on trait type 
   ///   @attention if F's argument is a generic Block/Many type, the trait   
   ///      will be wrapped in it                                             
   ///   @tparam MUTABLE - whether changes inside container are allowed       
   ///   @param call - the function to execute for each trait                 
   ///      If `call` returns bool, you can decide when to break the loop by  
   ///      simply returning Flow::Break (or just false)                      
   ///   @return the number of executions of 'call'                           
   template<bool MUTABLE, class F>
   size_t Neat::ForEachTag(F&& call) {
      using A = ArgumentOf<F>;
      using R = ReturnOf<F>;

      static_assert(CT::DefineTag<A> or CT::Deep<A>,
         "Iterator must be either trait-based or deep");
      static_assert(CT::Slab<A> or CT::Constant<A> or MUTABLE,
         "Non constant iterator for constant Neat block");

      size_t index = 0;
      if constexpr (CT::Tag<A>) {
         // Static trait provided, extract filter                       
         using TagType = Decay<A>;
         const auto filter = MetaTagOf<TagType>();
         const auto found = mTags.FindIt(filter);
         if (not found)
            return index;

         // Iterate all relevant traits                                 
         for (auto& data : found.GetValue()) {
            if constexpr (CT::Bool<R>) {
               if (not call(reinterpret_cast<TagType&>(data)))
                  return index + 1;
            }
            else call(reinterpret_cast<TagType&>(data));

            ++index;
         }
      }
      else {
         // Iterate all traits, either using type-erased trait, or deep 
         for (auto group : mTags) {
            for (auto& data : group.GetValue()) {
               if constexpr (CT::Deep<A>) {
                  auto wrapper = MakeBlock<Decay<A>>(data);
                  if constexpr (CT::Bool<R>) {
                     if (not call(wrapper))
                        return index + 1;
                  }
                  else call(wrapper);
               }
               else {
                  if constexpr (CT::Bool<R>) {
                     if (not call(data))
                        return index + 1;
                  }
                  else call(data);
               }

               ++index;
            }
         }
      }

      return index;
   }

   ///                                                                        
   template<class F> LANGULUS(INLINED)
   size_t Neat::ForEachTag(F&& call) const {
      return const_cast<Neat*>(this)->template
         ForEachTag<false>(Forward<F>(call));
   }

   /// Iterate all constructs                                                 
   ///   @attention if call's argument is a generic Block/Many type, construct
   ///      will be wrapped in it                                             
   ///   @tparam MUTABLE - whether changes inside container are allowed       
   ///   @param call - the function to execute for each construct             
   ///      If `call` returns bool, you can decide when to break the loop by  
   ///      simply returning Flow::Break (or just false)                      
   ///   @return the number of executions of 'call'                           
   template<bool MUTABLE, class F>
   size_t Neat::ForEachRecipe(F&& call) {
      using A = ArgumentOf<F>;
      using R = ReturnOf<F>;

      static_assert(CT::Recipe<A> or CT::Deep<A>,
         "Iterator must be either a Construct or deep");
      static_assert(CT::Slab<A> or CT::Constant<A> or MUTABLE,
         "Non constant iterator for constant Neat block");

      // Iterate all constructs                                         
      size_t index = 0;
      for (auto group : mRecipes) {
         for (auto& data : group.GetValue()) {
            if constexpr (CT::Deep<A>) {
               // Iterate using deep A                                  
               auto wrapper = MakeBlock<Decay<A>>(data);
               if constexpr (CT::Bool<R>) {
                  if (not call(wrapper))
                     return index + 1;
               }
               else call(wrapper);
            }
            else {
               // Iterate using construct                               
               if constexpr (CT::Bool<R>) {
                  if (not call(data))
                     return index + 1;
               }
               else call(data);
            }

            ++index;
         }
      }

      return index;
   }

   ///                                                                        
   template<class F> LANGULUS(INLINED)
   size_t Neat::ForEachRecipe(F&& call) const {
      return const_cast<Neat*>(this)->template
         ForEachRecipe<false>(Forward<F>(call));
   }

   /// Iterate all other types of data                                        
   /// You can provide a TMany iterator, to filter based on data type         
   ///   @tparam MUTABLE - whether changes inside container are allowed       
   ///   @param call - the function to execute for each block                 
   ///      If `call` returns bool, you can decide when to break the loop by  
   ///      simply returning Flow::Break (or just false)                      
   ///   @return the number of executions of 'call'                           
   template<bool MUTABLE, class F>
   size_t Neat::ForEachTail(F&& call) {
      using A = ArgumentOf<F>;
      using R = ReturnOf<F>;
      static_assert(CT::Slab<A> or CT::Constant<Deptr<A>> or MUTABLE,
         "Non constant iterator for constant Neat block");

      size_t index = 0;
      if constexpr (CT::Deep<A> and CT::Typed<A>) {
         // Statically typed container provided, extract filter         
         const auto filter = MetaDataOf<Decay<TypeOf<A>>>;
         const auto found = mAnythingElse.FindIt(filter);
         if (not found)
            return 0;

         // Iterate all relevant datas                                  
         for (auto& data : found.GetValue()) {
            auto& dataTyped = reinterpret_cast<Decay<A>&>(data);
            if constexpr (CT::Bool<R>) {
               if (not call(dataTyped))
                  return index + 1;
            }
            else call(dataTyped);

            ++index;
         }
      }
      else if constexpr (CT::Deep<A>) {
         // Type-erased container provided, iterate the entire tail     
         for (auto group : mAnythingElse) {
            for (auto& data : group.GetValue()) {
               if constexpr (CT::Bool<R>) {
                  if (not call(data))
                     return index + 1;
               }
               else call(data);

               ++index;
            }
         }
      }
      else {
         // Anything else                                               
         const auto filter = MetaDataOf<Decay<A>>();
         const auto found = mAnythingElse.FindIt(filter);
         if (not found)
            return 0;

         // Iterate all relevant datas                                  
         for (auto& data : found.GetValue()) {
            for (auto element : data) {
               if constexpr (CT::Bool<R>) {
                  if (not call(element.template Get<A>()))
                     return index + 1;
               }
               else call(element.template Get<A>());
            }

            ++index;
         }
      }

      return index;
   }

   ///                                                                        
   template<class F> LANGULUS(INLINED)
   size_t Neat::ForEachTail(F&& call) const {
      return const_cast<Neat*>(this)->template
         ForEachTail<false>(Forward<F>(call));
   }

   /// Remove data that matches the given type                                
   ///   @tparam T - type of data to remove                                   
   ///   @tparam EMPTY_TOO - use true, to remove all empty data entries, that 
   ///      are usually produced, by pushing DMeta (disabled by default)      
   ///   @return the number of removed data entries                           
   template<CT::NotVoid T, bool EMPTY_TOO>
   size_t Neat::RemoveData() {
      const auto filter = MetaDataOf<Decay<T>>();
      auto found = mAnythingElse.FindIt(filter);
      if (not found)
         return 0;

      if constexpr (EMPTY_TOO) {
         // Remove everything                                           
         const auto count = found.mValue->GetCount();
         mAnythingElse.RemoveIt(found);
         return count;
      }

      if (mAnythingElse.GetKeys().GetUses() > 1
      or  mAnythingElse.GetVals().GetUses() > 1) {
         // mAnythingElse is used from multiple locations, and we must  
         // branch out this particular instance before modifying it     
         found = mAnythingElse.BranchOut().FindIt(filter);
      }

      size_t count = 0;
      for (auto data : KeepIterator(found.mValue)) {
         if (not *data)
            continue;

         // Remove only matching data entries, that aren't empty        
         data = found.mValue.RemoveIt(data);
         ++count;
      }

      if (not found.GetValue())
         mAnythingElse.RemoveIt(found);
      return count;
   }

   /// Remove constructs that match the given type                            
   ///   @tparam T - type of construct to remove                              
   ///   @return the number of removed constructs                             
   template<CT::NotVoid T>
   size_t Neat::RemoveRecipes() {
      const auto filter = MetaDataOf<Decay<T>>();
      auto found = mRecipes.FindIt(filter);
      if (not found)
         return 0;

      if (mRecipes.GetKeys().GetUses() > 1
      or  mRecipes.GetVals().GetUses() > 1) {
         // mRecipes is used from multiple locations, and we must    
         // branch out this particular instance before modifying it     
         found = mRecipes.BranchOut().FindIt(filter);
      }

      size_t count = 0;
      for (auto data : KeepIterator(*found.mValue)) {
         if (not *data)
            continue;

         data = found.mValue->RemoveIt(data);
         ++count;
      }

      if (not *found.mValue)
         mRecipes.RemoveIt(found);
      return count;
   }

   /// Remove traits that match the given trait type                          
   ///   @tparam T - type of trait to remove                                  
   ///   @tparam EMPTY_TOO - use true, to remove all empty trait entries,     
   ///      that are usually made by pushing a TMeta (disabled by default)    
   ///   @return the number of removed trait entries                          
   template<CT::Tag T, bool EMPTY_TOO>
   size_t Neat::RemoveTag() {
      const auto filter = MetaTagOf<T>();
      const auto found = mTags.FindIt(filter);
      if (not found)
         return 0;

      if constexpr (EMPTY_TOO) {
         // Remove everything                                           
         const auto count = found.mValue->GetCount();
         mTags.RemoveIt(found);
         return count;
      }

      if (mTags.GetKeys().GetUses() > 1
      or  mTags.GetVals().GetUses() > 1) {
         // mTags is used from multiple locations, and we must        
         // branch out this particular instance before modifying it     
         found = mTags.BranchOut().FindIt(filter);
      }

      size_t count = 0;
      for (auto data : KeepIterator(*found.mValue)) {
         if (not *data)
            continue;

         // Remove only matching trait entries which aren't empty       
         data = found.mValue->RemoveIt(data);
         ++count;
      }

      if (not *found.mValue)
         mTags.RemoveIt(found);
      return count;
   }
   
   /// Serialize the neat to anything text-based                              
   ///   @param to - the serialized container                                 
   ///   @return the number of elements written to 'to'                       
   LANGULUS(INLINED)
   size_t Neat::Serialize(CT::Serial auto& to) const {
      const auto initial = to.GetCount();
      using OUT = Deref<decltype(to)>;

      bool separator = false;
      for (auto pair : mAnythingElse) {
         for (auto& group : pair.GetValue()) {
            if (separator)
               to += ", ";

            if (group.IsValid())
               group.SerializeToText<void>(to);
            else
               to += static_cast<OUT>(pair.GetKey());
            separator = true;
         }
      }

      for (auto pair : mTags) {
         for (auto& trait : pair.GetValue()) {
            if (separator)
               to += ", ";

            if (trait.IsValid())
               trait.Serialize(to);
            else
               to += static_cast<OUT>(pair.GetKey());
            separator = true;
         }
      }

      for (auto pair : mRecipes) {
         for (auto& construct : pair.GetValue()) {
            if (separator)
               to += ", ";

            if (construct->IsValid()
            or not construct.GetCharge().IsDefault())
               construct.Serialize(to);
            else
               to += static_cast<OUT>(pair.GetKey());
            separator = true;
         }
      }

      return to.GetCount() - initial;
   }

} // namespace Langulus::Annies
