///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Neat.hpp"
#include "../Any.hpp"


namespace Langulus::Annies
{
   /// Extract a tag from the descriptor                                      
   ///   @tparam T tag we're searching for                                    
   ///   @param values [out] where to save the value, if found                
   ///   @return true if value changed                                        
   template<CT::DefineTag T>
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
         value = found[IDX].template As<D>();
         return true;
      }
      catch (...) {}
      return false;
   }
   
   /// Extract data of an exact type, doing only pointer arithmetic           
   ///   @param value [out] where to save the value(s), if found              
   ///   @return the number of extracted values (always 1 if not an array)    
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
   ///   @param value [out] where to save the value, if found                 
   ///   @return the number of extracted values (always 1 if not an array)    
   size_t Neat::ExtractDataAs(CT::NotVoid auto& value) const {
      using D = Deref<decltype(value)>;

      if constexpr (CT::Array<D>) {
         // Fill a bounded array                                        
         size_t scanned = 0;
         for (auto pair : mAnythingElse) {
            for (auto& group : pair.GetVal()) {
               const auto toscan = ::std::min(ExtentOf<D> - scanned, group.GetCount());
               for (size_t i = 0; i < toscan; ++i) {
                  //TODO can be optimized-out for POD
                  try {
                     value[scanned] = group.template As<Deext<D>>(i);
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
            for (auto& group : pair.GetVal()) {
               try {
                  value = group.template As<D>();
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
   ///   @param item data with optional intent                                
   void Neat::InsertInner(auto&& item) {
      using S = IntentOf(item);
      using T = Deint<S>;

      if constexpr (CT::DefineTag<T>) {
         // Insert tag to its bucket                                    
         AddTag(S::Nest(item));
      }
      else if constexpr (Same<T, RTTI::CMeta>) {
         // Expand the constant, and push it.                           
         // The expanded constant can be anything, so make sure it is   
         // inserted properly. Might end up in a bucket.                
         InsertInner(Any::FromConstant(DeintCast(item)));
      }
      else if constexpr (CT::Recipe<T>) {
         // Construct's arguments are always Neat                       
         AddRecipe(S::Nest(item));
      }
      else if constexpr (CT::Executable<T>) {
         // Verbs have to respect their ordering, they all go into the  
         // same mAnythingElse[AVerb] bucket                            
         AddVerb(S::Nest(item));
      }
      else if constexpr (CT::Owned<T>) {
         // Make sure we strip any owning handle away                   
         InsertInner(S::Nest(DeintCast(item).Get()));
      }
      else if constexpr (CT::Deep<T>) {
         // Pushing an entire pack, make sure we flatten it, if it is   
         // allowed                                                     
         const auto meta = DeintCast(item).GetUnconstrainedState()
            ? MetaDataOf<Decay<T>>()
            : DeintCast(item).GetType();
         const auto found = mAnythingElse.Find(meta);

         if (found)
            found.GetValue() << S::Nest(item);
         else
            mAnythingElse.Merge(meta, S::Nest(item));
      }
      else {
         // RHS is nothing special, just add it as it is                
         const auto meta = MetaDataOf<Decay<T>>();
         const auto found = mAnythingElse.Find(meta);

         if (found)
            found.GetValue() << Messy {S::Nest(item)};
         else
            mAnythingElse.Merge(meta, Messy {S::Nest(item)});
      }

      // Demand a new hash on the next compare                          
      mHash = {};
   }

   /// Insert an element, array of elements, or another set                   
   ///   @param item the argument to unfold and insert, can have intent       
   ///   @return the number of inserted elements after unfolding              
   size_t Neat::UnfoldInsert(auto&& item) {
      using S = IntentOf(item);
      using T = Deint<S>;

      if constexpr (CT::Array<T>) {
         if constexpr (CT::Text<T>) {
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
               [&](Tag const& tag) {
                  InsertInner(S::Nest(const_cast<Tag&>(tag)));
               },
               [&](Verb const& verb) {
                  InsertInner(S::Nest(const_cast<Verb&>(verb)));
               },
               [&](RTTI::DMeta const& meta) { InsertInner(meta); },
               [&](RTTI::TMeta const& meta) { InsertInner(meta); },
               [&](RTTI::CMeta const& meta) { InsertInner(meta); }
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
   ///   @param key the key and intent to add                                 
   ///   @return 1 if pair was inserted, zero otherwise                       
   template<class T1, class...TN>
   size_t Neat::Insert(T1&& t1, TN&&...tn) {
      size_t inserted = 0;
        inserted += UnfoldInsert(Forward<T1>(t1));
      ((inserted += UnfoldInsert(Forward<TN>(tn))), ...);
      return inserted;
   }
   
   /// Push and sort anything, with or without intents                        
   ///   @param rhs the pair and intent to insert                             
   ///   @return a reference to this table for chaining                       
   template<class T>
   Neat& Neat::operator << (T&& rhs) {
      Insert(Forward<T>(rhs));
      return *this;
   }

   /// Insert anything if it doesn't exist already, with or without intents   
   ///   @attention hash will be recomputed on demand, if anything was pushed 
   ///   @param rhs the item and intent to insert                             
   ///   @return a reference to this Neat container                           
   Neat& Neat::operator <<= (auto&& rhs) {
      using S = IntentOf(rhs);
      using T = Deint<S>;
      decltype(auto) rhsd = DeintCast(rhs);

      if constexpr (CT::DefineTag<T>) {
         // Check if the trait already exists, before pushing it        
         if (not GetTags(rhsd.GetTag()))
            return operator << (S::Nest(rhs));
      }
      else if constexpr (Same<T, TMeta>) {
         // Check if the trait already exists, before pushing it        
         if (not GetTags(rhsd))
            return operator << (S::Nest(rhs));
      }
      else if constexpr (CT::Recipe<T>) {
         // Check if the construct already exists, before pushing it    
         if (not GetConstructs(rhsd.GetType()))
            return operator << (S::Nest(rhs));
      }
      else if constexpr (Same<T, DMeta>) {
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
   ///   @param tag tag to set                                                
   ///   @param index the index we're interested with if repeated             
   ///   @return a reference to this construct for chaining                   
   Neat& Neat::SetTag(CT::DefineTag auto&& tag, size_t index) {
      const auto meta = tag.GetTag();
      auto found = mTags/*.BranchOut()*/.Find(meta);

      if (found) {
         // A group of similar tags was found                           
         auto& group = found->mValue;
         if (group.GetCount() > index)
            group[index] = LglsFwd(tag);
         else
            group << LglsFwd(tag);
      }
      else {
         // If reached, a new trait group to be inserted                
         mTags.Merge(meta, LglsFwd(tag));
      }

      mHash = {};
      return *this;
   }

   /// Push a tag to the appropriate bucket                                   
   ///   @attention this is an inner function that doesn't affect the hash    
   ///   @param messy the tag and intent to insert                            
   void Neat::AddTag(CT::Intent auto&& messy) {
      using S = IntentOf(messy);
      using T = Deint<S>;

      if constexpr (CT::DefineTag<T>) {
         // Insert a tag with contents                                  
         const auto meta = messy->GetTag();
         auto found = mTags/*.BranchOut()*/.Find(meta);
         if (found)
            found.GetValue() << messy.Forward();
         else
            mTags.Merge(meta, messy.Forward());
      }
      else if constexpr (Exact<T, TMeta>) {
         // Insert tag without contents                                 
         const auto tag = *messy;
         auto found = mTags/*.BranchOut()*/.Find(tag);
         if (found)
            found.GetValue() << Tag::From(tag);
         else
            mTags.Merge(tag, Tag::From(tag));
      }
      else static_assert(false, "Can't insert tag");
   }
   
   /// Push verbs to the appropriate bucket                                   
   ///   @attention this is an inner function that doesn't affect the hash    
   ///   @param verb the verb (and intent) to insert                          
   void Neat::AddVerb(CT::Intent auto&& verb) {
      using S = IntentOf(verb);
      using V = Deint<S>;
      static_assert(CT::Executable<V>, "V isn't a verb");

      // Insert deep data - we have to flatten it                       
      const auto meta = MetaDataOf<Decay<V>>();
      auto found = mAnythingElse/*.BranchOut()*/.Find(meta);
      if (found)
         found.GetValue() << verb.Forward();
      else
         mAnythingElse.Merge(meta, verb.Forward());
   }

   /// Push a construct to the appropriate bucket                             
   ///   @attention this is an inner function that doesn't affect the hash    
   ///   @param messy the construct and intent to insert                      
   void Neat::AddRecipe(CT::Intent auto&& messy) {
      using S = IntentOf(messy);
      using T = Deint<S>;

      if constexpr (CT::Recipe<T>) {
         const auto meta = messy->GetType().GetOrigin();
         const auto found = mRecipes/*.BranchOut()*/.Find(meta);

         if (found)
            found.GetValue() << messy.Forward();
         else
            mRecipes.Merge(meta, messy.Forward());
      }
      else static_assert(false, "Can't insert construct");
   }

   /// Iterate through all relevant bucketed items.                           
   /// Depending on F's arguments, different portions of the container will   
   /// be iterated. Use a generic Block/Many type to iterate everything.      
   ///   @param call the function(s) to execute for each element              
   ///   @return the number of executions of 'call'                           
   size_t Neat::ForEach(this auto&& self, auto&&...call) {
      static_assert(sizeof...(call) > 0, "No iterators in ForEach");
      if (self.IsEmpty())
         return 0;

      size_t result = 0;
      (void) (... or (0 != (result = self.ForEachInner(call))));
      return result;
   }
   
   /// Iterate through all relevant bucketed items, inclusively               
   ///   @param call the function(s) to execute for each element              
   ///   @return the number of executions of all calls                        
   size_t Neat::ForEachDeep(this auto&& self, auto&&...call) {
      static_assert(sizeof...(call) > 0, "No iterators in ForEachDeep");
      size_t executions = 0;
      ((executions += self.ForEachInner(call)), ...);
      return executions;
   }
   
   /// Iterate through all relevant bucketed items.                           
   /// Depending on F's arguments, different portions of the container will   
   /// be iterated. Use a generic Block/Many type to iterate everything.      
   ///   @tparam F the function signature (deducible)                         
   ///   @param call the function to execute for each element                 
   ///   @return the number of executions of 'call'                           
   size_t Neat::ForEachInner(this auto&& self, auto&& call) {
      using A = LglsArgumentOf(call);
      static_assert(CT::Slab<A> or LglsMutable(self) or CT::ConstantEverywhere<A>,
         "Mutable reference/pointer iterator not allowed for constant container");

      if constexpr (CT::Deep<A>) {
         // Iterate everything                                          
         size_t counter = 0;
         counter += self.ForEachTag   (call);
         counter += self.ForEachRecipe(call);
         counter += self.ForEachTail  (call);
         return counter;
      }
      else if constexpr (CT::DefineTag<A>) {
         // Iterate tags only                                           
         return self.ForEachTag(call);
      }
      else if constexpr (CT::Recipe<A>) {
         // Iterate recipes only                                        
         return self.ForEachRecipe(call);
      }
      else {
         // Iterate anything else inside the tail                       
         return self.ForEachTail(call);
      }
   }

   /// Iterate all traits                                                     
   /// You can provide a static Tag iterator, to filter based on trait type   
   ///   @attention if F's argument is a generic Block/Many type, the trait   
   ///      will be wrapped in it                                             
   ///   @param call the function to execute for each trait                   
   ///      If `call` returns bool, you can decide when to break the loop by  
   ///      simply returning Flow::Break (or just false)                      
   ///   @return the number of executions of 'call'                           
   size_t Neat::ForEachTag(this auto&& self, auto&& call) {
      using A = LglsArgumentOf(call);
      using R = LglsReturnOf(call);

      static_assert(CT::DefineTag<A> or CT::Deep<A>,
         "Iterator must be either trait-based or deep");
      static_assert(CT::Slab<A> or LglsMutable(self) or CT::ConstantEverywhere<A>,
         "Mutable reference/pointer iterator not allowed for constant container");

      size_t index = 0;
      if constexpr (CT::Tag<A>) {
         // Static tag provided, extract filter                         
         using TagType = Decay<A>;
         const auto filter = MetaTagOf<TagType>();
         const auto found = self.mTags.Find(filter);
         if (not found)
            return index;

         // Iterate all relevant tags                                   
         for (auto& data : found.GetVal()) {
            if constexpr (CT::Bool<R>) {
               if (not call(reinterpret_cast<TagType&>(data)))
                  return index + 1;
            }
            else call(reinterpret_cast<TagType&>(data));

            ++index;
         }
      }
      else {
         // Iterate all tags, either using type-erased tag, or deep     
         for (auto group : self.mTags) {
            for (auto& data : group.GetVal()) {
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

   /// Iterate all constructs                                                 
   ///   @attention if call's argument is a generic Block/Many type, construct
   ///      will be wrapped in it                                             
   ///   @param call the function to execute for each construct               
   ///      If `call` returns bool, you can decide when to break the loop by  
   ///      simply returning Flow::Break (or just false)                      
   ///   @return the number of executions of 'call'                           
   size_t Neat::ForEachRecipe(this auto&& self, auto&& call) {
      using A = LglsArgumentOf(call);
      using R = LglsReturnOf(call);

      static_assert(CT::Recipe<A> or CT::Deep<A>,
         "Iterator must be either a Construct or deep");
      static_assert(CT::Slab<A> or LglsMutable(self) or CT::ConstantEverywhere<A>,
         "Mutable reference/pointer iterator not allowed for constant container");

      // Iterate all constructs                                         
      size_t index = 0;
      for (auto group : self.mRecipes) {
         for (auto& data : group.GetVal()) {
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

   /// Iterate all other types of data                                        
   /// You can provide a TMany iterator, to filter based on data type         
   ///   @param call the function to execute for each block                   
   ///      If `call` returns bool, you can decide when to break the loop by  
   ///      simply returning Flow::Break (or just false)                      
   ///   @return the number of executions of 'call'                           
   size_t Neat::ForEachTail(this auto&& self, auto&& call) {
      using A = LglsArgumentOf(call);
      using R = LglsReturnOf(call);
      static_assert(CT::Slab<A> or LglsMutable(self) or CT::ConstantEverywhere<A>,
         "Mutable reference/pointer iterator not allowed for constant container");

      size_t index = 0;
      if constexpr (CT::Deep<A> and CT::Typed<A>) {
         // Statically typed container provided, extract filter         
         const auto filter = MetaDataOf<Decay<TypeOf<A>>>;
         const auto found = self.mAnythingElse.FindIt(filter);
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
         for (auto group : self.mAnythingElse) {
            for (auto& data : group.GetVal()) {
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
         const auto found = self.mAnythingElse.FindIt(filter);
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

   /// Remove data that matches the given type                                
   ///   @tparam T type of data to remove                                     
   ///   @tparam EMPTY_TOO use true, to remove all empty data entries, that   
   ///      are usually produced, by pushing DMeta (disabled by default)      
   ///   @return the number of removed data entries                           
   template<CT::NotVoid T, bool EMPTY_TOO>
   size_t Neat::RemoveData() {
      const auto filter = MetaDataOf<Decay<T>>();
      auto found = mAnythingElse.Find(filter);
      if (not found)
         return 0;

      if constexpr (EMPTY_TOO) {
         // Remove everything                                           
         const auto count = found.mValue->GetCount();
         mAnythingElse.Erase(found);
         return count;
      }

      /*if (mAnythingElse.GetKeys().GetUses() > 1
      or  mAnythingElse.GetVals().GetUses() > 1) {
         // mAnythingElse is used from multiple locations, and we must  
         // branch out this particular instance before modifying it     //TODO do we?
         found = mAnythingElse.BranchOut().Find(filter);
      }*/

      size_t count = 0;
      for (auto data : KeepIterator(found.mValue)) {
         if (not *data)
            continue;

         // Remove only matching data entries, that aren't empty        
         data = found.mValue.RemoveIt(data);
         ++count;
      }

      if (not found.GetValue())
         mAnythingElse.Erase(found);
      return count;
   }

   /// Remove constructs that match the given type                            
   ///   @tparam T type of construct to remove                                
   ///   @return the number of removed constructs                             
   template<CT::NotVoid T>
   size_t Neat::RemoveRecipes() {
      const auto filter = MetaDataOf<Decay<T>>();
      auto found = mRecipes.Find(filter);
      if (not found)
         return 0;

      /*if (mRecipes.GetKeys().GetUses() > 1
      or  mRecipes.GetVals().GetUses() > 1) {
         // mRecipes is used from multiple locations, and we must    
         // branch out this particular instance before modifying it     //TODO do we?
         found = mRecipes.BranchOut().Find(filter);
      }*/

      size_t count = 0;
      for (auto data : KeepIterator(*found.mValue)) {
         if (not *data)
            continue;

         data = found.mValue->RemoveIt(data);
         ++count;
      }

      if (not *found.mValue)
         mRecipes.Erase(found);
      return count;
   }

   /// Remove traits that match the given trait type                          
   ///   @tparam T type of trait to remove                                    
   ///   @tparam EMPTY_TOO use true, to remove all empty trait entries,       
   ///      that are usually made by pushing a TMeta (disabled by default)    
   ///   @return the number of removed trait entries                          
   template<CT::DefineTag T, bool EMPTY_TOO>
   size_t Neat::RemoveTag() {
      const auto filter = MetaTagOf<T>();
      const auto found = mTags.Find(filter);
      if (not found)
         return 0;

      if constexpr (EMPTY_TOO) {
         // Remove everything                                           
         const auto count = found.mValue->GetCount();
         mTags.Erase(found);
         return count;
      }

      /*if (mTags.GetKeys().GetUses() > 1
      or  mTags.GetVals().GetUses() > 1) {
         // mTags is used from multiple locations, and we must        
         // branch out this particular instance before modifying it     //TODO do we?
         found = mTags.BranchOut().Find(filter);
      }*/

      size_t count = 0;
      for (auto data : KeepIterator(*found.mValue)) {
         if (not *data)
            continue;

         // Remove only matching trait entries which aren't empty       
         data = found.mValue->RemoveIt(data);
         ++count;
      }

      if (not *found.mValue)
         mTags.Erase(found);
      return count;
   }
   
   /// Serialize the neat to anything text-based                              
   ///   @param to the serialized container                                   
   ///   @return the number of elements written to 'to'                       
   /*size_t Neat::Serialize(CT::Serial auto& to) const {
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
   }*/
}