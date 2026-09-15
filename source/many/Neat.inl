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
   Neat::Neat(const Neat& other)
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
      , mTraits       {other.Nest(other->mTraits)}
      , mConstructs   {other.Nest(other->mConstructs)}
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
      mTraits       = other.Nest(other->mTraits);
      mConstructs   = other.Nest(other->mConstructs);
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
      mTraits.Clear();
      mConstructs.Clear();
      mAnythingElse.Clear();
   }
   
   /// Clear and deallocate the container                                     
   LANGULUS(INLINED) void Neat::Reset() {
      mHash = {};
      mTraits.Reset();
      mConstructs.Reset();
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
         mHash = HashOf(mTraits, mConstructs, mAnythingElse);
      return mHash;
   }

   /// Check if the container is empty                                        
   ///   @return true if empty                                                
   LANGULUS(INLINED)
   bool Neat::IsEmpty() const noexcept {
      return mTraits.IsEmpty()
         and mConstructs.IsEmpty()
         and mAnythingElse.IsEmpty();
   }
   
   /// Check if the container has missing entries, nest-scan                  
   ///   @return true if there's at least one missing entry                   
   LANGULUS(INLINED)
   bool Neat::IsMissingDeep() const {
      return mTraits.IsKeyMissingDeep() or mTraits.IsValueMissingDeep()
          or mConstructs.IsKeyMissingDeep() or mConstructs.IsValueMissingDeep()
          or mAnythingElse.IsKeyMissingDeep() or mAnythingElse.IsValueMissingDeep();
   }

   /// Check if construct contains executable elements                        
   ///   @return true if there's at least one executable entry                
   LANGULUS(INLINED)
   bool Neat::IsExecutable() const noexcept {
      return mTraits.IsKeyExecutableDeep() or mTraits.IsValueExecutableDeep()
          or mConstructs.IsKeyExecutableDeep() or mConstructs.IsValueExecutableDeep()
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
   bool Neat::operator == (const Neat& rhs) const {
      if (GetHash() != rhs.GetHash())
         return false;

      return mTraits == rhs.mTraits
         and mConstructs == rhs.mConstructs
         and mAnythingElse == rhs.mAnythingElse;
   }

   /// Merge two neat containers                                              
   ///   @param rhs - the container to merge                                  
   LANGULUS(INLINED)
   void Neat::Merge(const Neat& rhs) {
      if (not rhs)
         return;

      mTraits       += rhs.mTraits;
      mConstructs   += rhs.mConstructs;
      mAnythingElse += rhs.mAnythingElse;
      mHash = {};
   }

   /// Get list of traits, corresponding to a static trait                    
   ///   @tparam T - trait type to search for                                 
   ///   @return the trait list, or nullptr if no such list exists            
   ///   @attention the list can be empty, if trait was provided with no      
   ///              contents                                                  
   template<CT::Trait T> LANGULUS(INLINED)
   auto Neat::GetTraits() -> TraitList* {
      return GetTraits(MetaTraitOf<T>());
   }

   /// Get list of traits, corresponding to a static trait (const)            
   ///   @tparam T - trait type to search for                                 
   ///   @return the trait list, or nullptr if no such list exists            
   ///   @attention the list can be empty, if trait was provided with no      
   ///              contents                                                  
   template<CT::Trait T> LANGULUS(INLINED)
   auto Neat::GetTraits() const -> const TraitList* {
      return GetTraits(MetaTraitOf<T>());
   }
   
   /// Get list of traits, corresponding to a type                            
   ///   @param t - trait type to search for                                  
   ///   @return the trait list, or nullptr if no such list exists            
   ///   @attention the list can be empty, if trait was provided with no      
   ///              contents                                                  
   LANGULUS(INLINED)
   auto Neat::GetTraits(TMeta t) -> TraitList* {
      LANGULUS_ASSUME(UserAssumes, t, "Can't get invalid trait");
      auto found = mTraits.Find(t);
      return found ? &mTraits.GetValue(found) : nullptr;
   }

   /// Get list of traits, corresponding to a type (const)                    
   ///   @param t - trait type to search for                                  
   ///   @return the trait list, or nullptr if no such list exists            
   ///   @attention the list can be empty, if trait was provided with no      
   ///              contents                                                  
   LANGULUS(INLINED)
   auto Neat::GetTraits(TMeta t) const -> const TraitList* {
      return const_cast<Neat*>(this)->GetTraits(t);
   }
   
   /// Get list of data, corresponding to a static type                       
   ///   @tparam T - type to search for                                       
   ///   @return the data list, or nullptr if no such list exists             
   template<CT::Data T> LANGULUS(INLINED)
   auto Neat::GetData() -> TailList* {
      return GetData(MetaDataOf<Decay<T>>());
   }

   /// Get list of data, corresponding to a static type (const)               
   ///   @tparam T - type to search for                                       
   ///   @return the data list, or nullptr if no such list exists             
   template<CT::Data T> LANGULUS(INLINED)
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
   template<CT::Data T> LANGULUS(INLINED)
   auto Neat::GetConstructs() -> ConstructList* {
      return GetConstructs(MetaDataOf<Decay<T>>());
   }

   /// Get list of constructs, corresponding to a static type (const)         
   ///   @tparam T - type to search for                                       
   ///   @return the construct list, or nullptr if no such list exists        
   template<CT::Data T> LANGULUS(INLINED)
   auto Neat::GetConstructs() const -> const ConstructList* {
      return GetConstructs(MetaDataOf<Decay<T>>());
   }
   
   /// Get list of constructs, corresponding to a type                        
   ///   @param d - type to search for                                        
   ///   @return the construct list, or nullptr if no such list exists        
   LANGULUS(INLINED)
   auto Neat::GetConstructs(DMeta d) -> ConstructList* {
      auto found = mConstructs.Find(d ? d->mOrigin : nullptr);
      return found ? &mConstructs.GetValue(found) : nullptr;
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
   template<CT::Data T> LANGULUS(INLINED)
   DMeta Neat::FindType() const {
      return FindType(MetaDataOf<T>());
   }

   /// Find data in constructs or tail, that casts to a type                  
   ///   @param type - type requirement                                       
   ///   @return the first type that matches                                  
   inline DMeta Neat::FindType(DMeta type) const {
      DMeta primitive;
      bool  ambiguous = false;

      ForEachConstruct([&](const Construct& c) noexcept {
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
   template<CT::Trait T> LANGULUS(INLINED)
   void Neat::SetDefaultTrait(CT::Data auto&& value) {
      auto found = GetTraits<T>();
      if (found and *found)
         return;

      AddTrait(Abandon(T {Forward<Deref<decltype(value)>>(value)}));
   }

   /// Extract a trait from the descriptor                                    
   ///   @tparam T... - trait(s) we're searching for                          
   ///   @param values - [out] where to save the value, if found              
   ///   @return true if value changed                                        
   template<CT::Trait...T> LANGULUS(INLINED)
   bool Neat::ExtractTrait(CT::Data auto&...values) const {
      return (ExtractTraitInner<T>(values...) or ...);
   }
   
   /// Extract a trait from the descriptor                                    
   ///   @tparam T - trait we're searching for                                
   ///   @param values - [out] where to save the value, if found              
   ///   @return true if value changed                                        
   template<CT::Trait T> LANGULUS(INLINED)
   bool Neat::ExtractTraitInner(CT::Data auto&...values) const {
      auto found = GetTraits<T>();
      if (found) {
         return ExtractTraitInner(
            *found, Sequence<sizeof...(values)>::Expand, values...
         );
      }
      return false;
   }

   ///                                                                        
   template<Offset...IDX>
   bool Neat::ExtractTraitInner(
      const TraitList& found, ExpandedSequence<IDX...>, CT::Data auto&...values
   ) const {
      return (ExtractTraitInnerInner<IDX>(found, values) or ...);
   }
   
   ///                                                                        
   template<Offset IDX>
   bool Neat::ExtractTraitInnerInner(const TraitList& found, CT::Data auto& value) const {
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
   Count Neat::ExtractData(CT::Data auto& value) const {
      using D = Deref<decltype(value)>;
      if constexpr (CT::Array<D>) {
         // Fill a bounded array                                        
         auto found = GetData<Decay<D>>();
         if (found) {
            Count scanned = 0;
            for (auto& group : *found) {
               const auto toscan = ::std::min(ExtentOf<D> - scanned, group.GetCount());
               for (Offset i = 0; i < toscan; ++i) {
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
   inline Count Neat::ExtractDataAs(CT::Data auto& value) const {
      using D = Deref<decltype(value)>;

      if constexpr (CT::Array<D>) {
         // Fill a bounded array                                        
         Count scanned = 0;
         for (auto pair : mAnythingElse) {
            for (auto& group : pair.GetValue()) {
               const auto toscan = ::std::min(ExtentOf<D> - scanned, group.GetCount());
               for (Offset i = 0; i < toscan; ++i) {
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

      if constexpr (CT::TraitBased<T>) {
         // Insert trait to its bucket                                  
         AddTrait(S::Nest(item));
      }
      else if constexpr (CT::Same<T, CMeta>) {
         // Expand the constant, and push it                            
         operator << (Clone(Block<> {{}, DeintCast(item)}));
      }
      else if constexpr (CT::Construct<T>) {
         // Construct's arguments are always Neat                       
         AddConstruct(S::Nest(item));
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
   Count Neat::UnfoldInsert(auto&& item) {
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
            Count inserted = 0;
            for (auto& key : item)
               inserted += UnfoldInsert(S::Nest(key));
            return inserted;
         }
      }
      else if constexpr (CT::Neat<T>) {
         // Insert Neat, by inserting each element from it              
         Count inserted = 0;

         DeintCast(item).ForEach([&](const Many& subitem) {
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
            Count inserted = 0;
            DeintCast(item).ForEach([&](const Many& subitem) {
               inserted += UnfoldInsert(
                  S::Nest(const_cast<Many&>(subitem)));
            });
            return inserted;
         }
         else {
            // Item is not deep, filter based on type                   
            const auto inserted = DeintCast(item).ForEach(
               [&](const Construct& c) {
                  InsertInner(S::Nest(const_cast<Construct&>(c)));
               },
               [&](const Neat& neat) {
                  UnfoldInsert(S::Nest(const_cast<Neat&>(neat)));
               },
               [&](const Trait& trait) {
                  InsertInner(S::Nest(const_cast<Trait&>(trait)));
               },
               [&](const A::Verb& verb) {
                  InsertInner(S::Nest(const_cast<A::Verb&>(verb)));
               },
               [&](const DMeta& meta) { InsertInner(meta); },
               [&](const TMeta& meta) { InsertInner(meta); },
               [&](const CMeta& meta) { InsertInner(meta); }
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
   Count Neat::Insert(T1&& t1, TN&&...tn) {
      Count inserted = 0;
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

      if constexpr (CT::TraitBased<T>) {
         // Check if the trait already exists, before pushing it        
         if (not GetTraits(rhsd.GetTrait()))
            return operator << (S::Nest(rhs));
      }
      else if constexpr (CT::Same<T, TMeta>) {
         // Check if the trait already exists, before pushing it        
         if (not GetTraits(rhsd))
            return operator << (S::Nest(rhs));
      }
      else if constexpr (CT::Construct<T>) {
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

         rhsd.ForEach([&](const Many& group) {
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
   Neat& Neat::SetTrait(CT::TraitBased auto&& trait, Offset index) {
      const auto meta = trait.GetTrait();
      auto found = mTraits.BranchOut().FindIt(meta);

      if (found) {
         // A group of similar traits was found                         
         auto& group = found->mValue;
         if (group.GetCount() > index)
            group[index] = Forward<Trait>(trait);
         else
            group << Forward<Trait>(trait);
      }
      else {
         // If reached, a new trait group to be inserted                
         mTraits.Insert(meta, Forward<Trait>(trait));
      }

      mHash = {};
      return *this;
   }

   /// Push a trait to the appropriate bucket                                 
   ///   @attention this is an inner function that doesn't affect the hash    
   ///   @param messy - the trait and intent to insert                        
   LANGULUS(INLINED)
   void Neat::AddTrait(CT::Intent auto&& messy) {
      using S = IntentOf<decltype(messy)>;
      using T = TypeOf<S>;

      if constexpr (CT::TraitBased<T>) {
         // Insert a trait with contents                                
         const auto meta = messy->GetTrait();
         auto found = mTraits.BranchOut().FindIt(meta);
         if (found)
            found.GetValue() << messy.Forward();
         else
            mTraits.Insert(meta, messy.Forward());
      }
      else if constexpr (CT::Exact<T, TMeta>) {
         // Insert trait without contents                               
         const auto trait = *messy;
         auto found = mTraits.BranchOut().FindIt(trait);
         if (found)
            found.GetValue() << Trait::FromMeta(trait);
         else
            mTraits.Insert(trait, Trait::FromMeta(trait));
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
   void Neat::AddConstruct(CT::Intent auto&& messy) {
      using S = IntentOf<decltype(messy)>;
      using T = TypeOf<S>;

      if constexpr (CT::Construct<T>) {
         const auto meta = messy->GetType()
            ? messy->GetType()->mOrigin
            : nullptr;
         const auto found = mConstructs.BranchOut().FindIt(meta);

         if (found)
            found.GetValue() << messy.Forward();
         else
            mConstructs.Insert(meta, messy.Forward());
      }
      else static_assert(false, "Can't insert construct");
   }

   /// Get a tagged argument inside constructor                               
   ///   @param meta - trait to search for                                    
   ///   @param index - the index we're interested in, if repeated            
   ///   @return selected data or nullptr if none was found                   
   ///   @attention if not nullptr, returned Many might contain a Neat        
   inline auto Neat::GetTrait(TMeta meta, Offset index) const -> const Trait* {
      const auto found = mTraits.FindIt(meta);
      if (found and found.GetValue().GetCount() > index)
         return &(found.GetValue()[index]);
      return nullptr;
   }

   /// Get traits from constructor                                            
   ///   @tparam T - the type of trait to search for                          
   ///   @return selected data or nullptr if none was found                   
   ///   @attention if not nullptr, returned Many might contain a Neat        
   template<CT::Trait T> LANGULUS(INLINED)
   auto Neat::GetTrait(Offset index) const -> const Trait* {
      return Get(MetaTraitOf<T>(), index);
   }

   /// Iterate through all relevant bucketed items                            
   /// Depending on F's arguments, different portions of the container will   
   /// be iterated. Use a generic Block/Many type to iterate everything.      
   ///   @tparam MUTABLE - whether changes inside container are allowed       
   ///   @param call - the function(s) to execute for each element            
   ///   @return the number of executions of 'call'                           
   template<bool MUTABLE> LANGULUS(INLINED)
   Count Neat::ForEach(auto&&...call) {
      static_assert(sizeof...(call) > 0, "No iterators in ForEach");
      if (IsEmpty())
         return 0;

      Count result = 0;
      (void) (... or (0 != (result = 
         ForEachInner<MUTABLE>(Forward<Deref<decltype(call)>>(call))
      )));
      return result;
   }

   ///                                                                        
   LANGULUS(INLINED)
   Count Neat::ForEach(auto&&...call) const {
      static_assert(sizeof...(call) > 0, "No iterators in ForEach");
      return const_cast<Neat*>(this)->template
         ForEach<false>(Forward<Deref<decltype(call)>>(call)...);
   }
   
   /// Iterate through all relevant bucketed items, inclusively               
   ///   @tparam MUTABLE - whether changes inside container are allowed       
   ///   @param call - the function(s) to execute for each element            
   ///   @return the number of executions of all calls                        
   template<bool MUTABLE> LANGULUS(INLINED)
   Count Neat::ForEachDeep(auto&&...call) {
      static_assert(sizeof...(call) > 0, "No iterators in ForEachDeep");
      Count executions = 0;
      ((executions += ForEachInner<MUTABLE>(
         Forward<Deref<decltype(call)>>(call))), ...);
      return executions;
   }

   /// Neat containers are always flat, so deep iteration is same as flat one 
   LANGULUS(INLINED)
   Count Neat::ForEachDeep(auto&&...call) const {
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
   Count Neat::ForEachInner(F&& call) {
      using A = ArgumentOf<F>;
      static_assert(CT::Slab<A> or CT::Constant<Deptr<A>> or MUTABLE,
         "Non constant iterator for constant Neat block");

      if constexpr (CT::Deep<A>) {
         // Iterate everything                                          
         Count counter = 0;
         counter += ForEachTrait<MUTABLE>(call);
         counter += ForEachConstruct<MUTABLE>(call);
         counter += ForEachTail<MUTABLE>(call);
         return counter;
      }
      else if constexpr (CT::TraitBased<A>) {
         // Iterate traits only                                         
         return ForEachTrait<MUTABLE>(Forward<F>(call));
      }
      else if constexpr (CT::Construct<A>) {
         // Iterate constructs only                                     
         return ForEachConstruct<MUTABLE>(Forward<F>(call));
      }
      else {
         // Iterate anything else inside the tail                       
         return ForEachTail<MUTABLE>(Forward<F>(call));
      }
   }

   ///                                                                        
   template<class F> LANGULUS(INLINED)
   Count Neat::ForEachInner(F&& call) const {
      return const_cast<Neat*>(this)->template 
         ForEach<false>(Forward<F>(call));
   }

   /// Iterate all traits                                                     
   /// You can provide a static Trait iterator, to filter based on trait type 
   ///   @attention if F's argument is a generic Block/Many type, the trait   
   ///      will be wrapped in it                                             
   ///   @tparam MUTABLE - whether changes inside container are allowed       
   ///   @param call - the function to execute for each trait                 
   ///      If `call` returns bool, you can decide when to break the loop by  
   ///      simply returning Flow::Break (or just false)                      
   ///   @return the number of executions of 'call'                           
   template<bool MUTABLE, class F>
   Count Neat::ForEachTrait(F&& call) {
      using A = ArgumentOf<F>;
      using R = ReturnOf<F>;

      static_assert(CT::TraitBased<A> or CT::Deep<A>,
         "Iterator must be either trait-based or deep");
      static_assert(CT::Slab<A> or CT::Constant<A> or MUTABLE,
         "Non constant iterator for constant Neat block");

      Count index = 0;
      if constexpr (CT::Trait<A>) {
         // Static trait provided, extract filter                       
         using TraitType = Decay<A>;
         const auto filter = MetaTraitOf<TraitType>();
         const auto found = mTraits.FindIt(filter);
         if (not found)
            return index;

         // Iterate all relevant traits                                 
         for (auto& data : found.GetValue()) {
            if constexpr (CT::Bool<R>) {
               if (not call(reinterpret_cast<TraitType&>(data)))
                  return index + 1;
            }
            else call(reinterpret_cast<TraitType&>(data));

            ++index;
         }
      }
      else {
         // Iterate all traits, either using type-erased trait, or deep 
         for (auto group : mTraits) {
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
   Count Neat::ForEachTrait(F&& call) const {
      return const_cast<Neat*>(this)->template
         ForEachTrait<false>(Forward<F>(call));
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
   Count Neat::ForEachConstruct(F&& call) {
      using A = ArgumentOf<F>;
      using R = ReturnOf<F>;

      static_assert(CT::Construct<A> or CT::Deep<A>,
         "Iterator must be either a Construct or deep");
      static_assert(CT::Slab<A> or CT::Constant<A> or MUTABLE,
         "Non constant iterator for constant Neat block");

      // Iterate all constructs                                         
      Count index = 0;
      for (auto group : mConstructs) {
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
   Count Neat::ForEachConstruct(F&& call) const {
      return const_cast<Neat*>(this)->template
         ForEachConstruct<false>(Forward<F>(call));
   }

   /// Iterate all other types of data                                        
   /// You can provide a TMany iterator, to filter based on data type         
   ///   @tparam MUTABLE - whether changes inside container are allowed       
   ///   @param call - the function to execute for each block                 
   ///      If `call` returns bool, you can decide when to break the loop by  
   ///      simply returning Flow::Break (or just false)                      
   ///   @return the number of executions of 'call'                           
   template<bool MUTABLE, class F>
   Count Neat::ForEachTail(F&& call) {
      using A = ArgumentOf<F>;
      using R = ReturnOf<F>;
      static_assert(CT::Slab<A> or CT::Constant<Deptr<A>> or MUTABLE,
         "Non constant iterator for constant Neat block");

      Count index = 0;
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
   Count Neat::ForEachTail(F&& call) const {
      return const_cast<Neat*>(this)->template
         ForEachTail<false>(Forward<F>(call));
   }

   /// Remove data that matches the given type                                
   ///   @tparam T - type of data to remove                                   
   ///   @tparam EMPTY_TOO - use true, to remove all empty data entries, that 
   ///      are usually produced, by pushing DMeta (disabled by default)      
   ///   @return the number of removed data entries                           
   template<CT::Data T, bool EMPTY_TOO>
   Count Neat::RemoveData() {
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

      Count count = 0;
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
   template<CT::Data T>
   Count Neat::RemoveConstructs() {
      const auto filter = MetaDataOf<Decay<T>>();
      auto found = mConstructs.FindIt(filter);
      if (not found)
         return 0;

      if (mConstructs.GetKeys().GetUses() > 1
      or  mConstructs.GetVals().GetUses() > 1) {
         // mConstructs is used from multiple locations, and we must    
         // branch out this particular instance before modifying it     
         found = mConstructs.BranchOut().FindIt(filter);
      }

      Count count = 0;
      for (auto data : KeepIterator(*found.mValue)) {
         if (not *data)
            continue;

         data = found.mValue->RemoveIt(data);
         ++count;
      }

      if (not *found.mValue)
         mConstructs.RemoveIt(found);
      return count;
   }

   /// Remove traits that match the given trait type                          
   ///   @tparam T - type of trait to remove                                  
   ///   @tparam EMPTY_TOO - use true, to remove all empty trait entries,     
   ///      that are usually made by pushing a TMeta (disabled by default)    
   ///   @return the number of removed trait entries                          
   template<CT::Trait T, bool EMPTY_TOO>
   Count Neat::RemoveTrait() {
      const auto filter = MetaTraitOf<T>();
      const auto found = mTraits.FindIt(filter);
      if (not found)
         return 0;

      if constexpr (EMPTY_TOO) {
         // Remove everything                                           
         const auto count = found.mValue->GetCount();
         mTraits.RemoveIt(found);
         return count;
      }

      if (mTraits.GetKeys().GetUses() > 1
      or  mTraits.GetVals().GetUses() > 1) {
         // mTraits is used from multiple locations, and we must        
         // branch out this particular instance before modifying it     
         found = mTraits.BranchOut().FindIt(filter);
      }

      Count count = 0;
      for (auto data : KeepIterator(*found.mValue)) {
         if (not *data)
            continue;

         // Remove only matching trait entries which aren't empty       
         data = found.mValue->RemoveIt(data);
         ++count;
      }

      if (not *found.mValue)
         mTraits.RemoveIt(found);
      return count;
   }
   
   /// Serialize the neat to anything text-based                              
   ///   @param to - the serialized container                                 
   ///   @return the number of elements written to 'to'                       
   LANGULUS(INLINED)
   Count Neat::Serialize(CT::Serial auto& to) const {
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

      for (auto pair : mTraits) {
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

      for (auto pair : mConstructs) {
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
