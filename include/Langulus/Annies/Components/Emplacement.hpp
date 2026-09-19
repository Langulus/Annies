///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Container.hpp"
#include <Langulus/CT/Describable.hpp>
#include <Langulus/CT/Index.hpp>
#include <Langulus/Allocator.hpp>
#include <Langulus/MetaOf.hpp>


/*namespace Langulus::CT
{
   /// Check if container's elements are emplaceable using the provided       
   /// argument list. Use empty list to test if default-constructible.        
   ///   @attention type-erased elements are always emplaceable, because      
   ///      all arguments will be encapsulated in a descriptor, and will fail 
   ///      at runtime if not reflected as descriptor-constructible           
   template<class C, class...A>
   concept RangeEmplaceable = Container<C> and (
      Untyped<C> or ::std::constructible_from<TypeOf<C>, A...>
   );
}*/

namespace Langulus::Annies
{
   using DMeta = RTTI::DMeta;
}

namespace Langulus::Annies::Component
{
   enum class AllocationStrategy {
      DontAllocate,
      FreshAllocate,
      Reallocate
   };

   /// Refers back to this particular component instance through the deduced  
   /// 'this'. Just for convenience. It is #undef-ed at the end of this file. 
   #define ThisCom self.Emplacement<ID, SHARED...>

   ///                                                                        
   /// Adds public emplacement methods.                                       
   /// Unlike insertion, emplacement reuses the same memory space and         
   /// guarantees that nothing moves around.                                  
   ///   @tparam ID the data provider we're emplacing into                    
   ///   @tparam SHARED other providers that share emplacement behavior       
   template<Cid ID, Cid...SHARED>
   struct Emplacement {
      using CTTI_Component = Yup;
      using CTTI_ReflectAs = void;
      using Id             = Values<ID, SHARED...>;

      static constexpr int  ComponentPrecedence = 3000;
      static constexpr bool Shared = sizeof...(SHARED) > 0;
      template<Cid SID>
      static constexpr bool Relevant = Id::template Contains<SID>;

      /// MARK: Public                                                        
      /// Generic emplacement that constructs/overwrites specific element.    
      /// Any overwritten element will be dereferenced/destroyed first.       
      ///   @tparam E Sets the type of the container if empty. Ignored if     
      ///      container is statically-typed.                                 
      ///   @param at The index at which to emplace                           
      ///   @param arguments Constructor arguments for initializing an        
      ///      element. If C is type-erased, argument must be Describe.       
      ///   @return a reference or handle to the newly created element        
      template<class E = void, Cid SID = ID, CT::ContainsMany C, class...A> requires Relevant<SID>
      auto EmplaceAt(this C& self, CT::Index auto&& at, A&&...arguments)
      -> DecidePick<C> requires CT::IndexedLinearly<C> /*requires CT::RangeEmplaceable<C, A...>*/ {
         DecidePick<C> pick = self.template AsAt<DecidePick<C>, SID>(LglsFwd(at));
         pick.template Emplace<E, SID>(LglsFwd(arguments)...);
         return pick;
      }

      /// Generic emplacement that constructs/overwrites the first element.   
      /// Any overwritten element will be dereferenced/destroyed first.       
      ///   @tparam E Sets the type of the container if empty. Ignored if     
      ///      container is statically-typed.                                 
      ///   @param arguments Constructor arguments                            
      ///   @return a reference or handle to the newly created element        
      template<class E = void, Cid SID = ID, CT::Container C, class...A> requires Relevant<SID>//TODO its not clear whether Emplace works on all dimensions or not - figure it out
      auto Emplace(this C& self, A&&...arguments)
      -> DecidePick<C> /*requires CT::RangeEmplaceable<C, A...>*/ {
         auto a = self.template GetAllocation<SID>();
         if (not a) {
            // No ownership, just fresh-allocate                        
            try {
               if constexpr (sizeof...(arguments) > 0)
                  ThisCom::template EmplaceConstruct<SID, AllocationStrategy::FreshAllocate, E>(LglsFwd(arguments)...);
               else
                  ThisCom::template EmplaceDefault<SID, AllocationStrategy::FreshAllocate, E>();
            }
            catch (...) {
               // Reset heap count in case 'self' was disowned          
               if_available(self.template SetReservedInner<SID>(0));
               if_available(self.template SetHashTableInner<SID>(nullptr));
               self.template ResetCount<SID>();
               throw;
            }
         }
         else if (self.template IsEmpty<SID>()) {
            // The container is empty, but an allocation is available   
            if (a->GetUses() != 1) {
               // We're not the only owner of this memory.              
               // We have to branch off with a fresh allocation.        
               DecvqAllCast(a)->AddRef(-1);

               try {
                  if constexpr (sizeof...(arguments) > 0)
                     ThisCom::template EmplaceConstruct<SID, AllocationStrategy::FreshAllocate, E>(LglsFwd(arguments)...);
                  else
                     ThisCom::template EmplaceDefault<SID, AllocationStrategy::FreshAllocate, E>();
               }
               catch (...) {
                  self.ResetAllAllocations();
                  throw;
               }
            }
            else {
               // Emplace a new element on the first position.          
               // We're allowed to reuse the memory.                    
               if constexpr (sizeof...(arguments) > 0)
                  ThisCom::template EmplaceConstruct<SID, AllocationStrategy::Reallocate, E>(LglsFwd(arguments)...);
               else
                  ThisCom::template EmplaceDefault<SID, AllocationStrategy::Reallocate, E>();
            }
         }
         else {
            // The container is not empty                               
            if (a->GetUses() != 1) {
               // We're not the only owner of this memory.              
               // We have to branch off with a fresh allocation.        
               self.Free();

               try {
                  if constexpr (sizeof...(arguments) > 0)
                     ThisCom::template EmplaceConstruct<SID, AllocationStrategy::FreshAllocate, E>(LglsFwd(arguments)...);
                  else
                     ThisCom::template EmplaceDefault<SID, AllocationStrategy::FreshAllocate, E>();
               }
               catch (...) {
                  self.ResetAllAllocations();
                  throw;
               }
            }
            else {
               // We're allowed to reuse the memory.                    
               // Need to destroy and overwrite only the first element. 
               auto item = self.GetHandle();
               item.template Free<false>();
               if_available(item.ResetAllEntries());
               //TODO clear the correspnding hash table spot?

               // Emplace a new element on the first position.          
               // Any state change is forbidden - container is full.    
               try {
                  if constexpr (sizeof...(arguments) > 0)
                     ThisCom::template EmplaceConstruct<SID, AllocationStrategy::DontAllocate, E>(LglsFwd(arguments)...);
                  else
                     ThisCom::template EmplaceDefault<SID, AllocationStrategy::DontAllocate, E>();
               }
               catch (...) {
                  // If emplacement fails, we are forced to destroy     
                  // all remaining elements as well.                    
                  if constexpr (CT::ContainsMany<C>) {
                     item += 1;
                     const auto itemsEnd = self.GetHandle() + self.template GetCount<SID>();
                     while (item.GetRaw() != itemsEnd.GetRaw()) {
                        item.template Free<false>();
                        //item.template DestroyElement<true, SID>();
                        ++item;
                     }
                  }

                  Allocator::Deallocate(DecvqAllCast(a));
                  self.ResetAllAllocations();
                  throw;
               }
            }
         }

         // Return a reference/handle to the newly emplaced element     
         return self.template As<Deref<DecidePick<C>>, SID>();
      }

   protected:
      /// MARK: Protected                                                     
      LglsComHeapMovable(friend);
      LglsComInsertion(friend);
      LglsComMerging(friend);
      LglsComIndexedCommonHashed(friend);
      
      template<CT::Handle, CT::Handle> friend struct THandlePair;

      /// Clone the 'rhs'.                                                    
      /// Assumes all indirections are ordinary pointers, and is thus faster. 
      ///   @attention Works in one dimension at a time!                      
      template<Cid SID = ID, CT::Container C, CT::NoIntent IT> requires Relevant<SID>
      void EmplaceByCloningStandardPointers(this C& self, IT const& rhs) {
         /*static_assert(CT::ContainsOne<C>,
            "Emplacing only first element in a container with many. "
            "GetHandle() first?"
         );*/

         constexpr bool has_entries = requires { self.template GetEntriesInner<SID>(); };
         [[maybe_unused]] DMeta T;
         // If T is Text**, then dst/src are Text***                    
         void** dst = static_cast<void**>(self.template GetRawVoid<SID>());
         void** src;
         
         if constexpr (CT::Handle<IT>) {
            if constexpr (CT::TypeErased<C> or CT::TypeErased<IT>) {
               T = rhs.template GetType<SID>();
               LglsAssumeDev(self.template IsSame<SID>(T), "Type mismatch");               
            }
            else static_assert(Same<TypeOf<C, SID>, TypeOf<IT, SID>>, "Type mismatch");
            src = static_cast<void**>(rhs.template GetRawVoid<SID>());
         }
         else {
            if constexpr (CT::TypeErased<C>) {
               LglsAssumeDev((self.template IsSame<IT, SID>()), "Type mismatch");
               T = self.template GetType<SID>();
            }
            else static_assert(Same<TypeOf<C, SID>, IT>, "Type mismatch");   
            src = static_cast<void**>(const_cast<void*>(static_cast<const void*>(&rhs)));
         }

         if constexpr (CT::TypeErased<C>) {
            const size_t indirects = T.GetIndirections();
            if (indirects > 0) {
               // Allocate the origin first                             
               const auto originT = T.GetOrigin();
               #if LANGULUS_FEATURE(MANAGED_MEMORY)
                  const auto cloned_origin = Allocator::Allocate(
                     originT,
                     pot_t(Roof2(originT.GetSize()))
                  );
               #else
                  const auto cloned_origin = Allocator::Allocate(
                     originT.GetAlignment(),
                     pot_t(Roof2(originT.GetSize()))
                  );
               #endif
               LglsAssert(cloned_origin, "Out of memory");

               // If T is Text**, ent is Allocation*[2]                 
               [[maybe_unused]] Allocation const* const* ent;
               if constexpr (has_entries)
                  ent = self.template GetEntriesInner<SID>();
               
               if (indirects > 1) {
                  // Allocate multiple indirections                     
                  // If T is Text**, we allocate one intermediate ptr   
                  #if LANGULUS_FEATURE(MANAGED_MEMORY)
                     const auto cloned_ptrs = Allocator::Allocate(
                        T,
                        pot_t(Roof2(T.GetSize() * (indirects - 1)))
                     );
                  #else
                     const auto cloned_ptrs = Allocator::Allocate(
                        T.GetAlignment(),
                        pot_t(Roof2(T.GetSize() * (indirects - 1)))
                     );
                  #endif
                  
                  if (not cloned_ptrs) {
                     Allocator::Deallocate(cloned_origin);
                     LglsError("Out of memory");
                     return;
                  }
                  
                  cloned_ptrs->AddRef(indirects - 2);

                  // Given dst being Text***, we have:                  
                  //    *dst = cloned_ptrs                              
                  //   **dst = cloned_origin                            
                  //  ***dst = ***src                                   
                  void** ptr = static_cast<void**>(static_cast<void*>(cloned_ptrs->GetBlockStart()));
                  *dst = ptr;

                  if constexpr (has_entries)
                     DecvqAllCast(*ent) = cloned_ptrs;

                  T = T.GetDeptr();

                  do {
                     // Chain all intermediate pointers                 
                     src = static_cast<void**>(*src);
                     dst = static_cast<void**>(*dst);
                     T = T.GetDeptr();
                     *dst = dst + 1;

                     if constexpr (has_entries) {
                        ++ent;
                        DecvqAllCast(*ent) = cloned_ptrs;
                     }
                  }
                  while (T.IsSparse());
               }
               else T = T.GetDeptr();

               // The last indirection points to the cloned origin      
               *dst = cloned_origin->GetBlockStart();
               src = static_cast<void**>(*src);
               dst = static_cast<void**>(*dst);

               if constexpr (has_entries)
                  DecvqAllCast(*ent) = cloned_origin;
            }

            // Finally, clone inside the allocated origin               
            T.GetCloneConstructor()(src, dst);
         }
         else {
            //                                                          
            // Both sides are statically-typed and we can benefit       
            // from a lot of compile-time optimizations.                
            using T = TypeOf<C, SID>;
            constexpr size_t indirects = IndirectsOf<T>;
            if constexpr (indirects > 0) {
               // Clone the origin first                                
               using originT = Decay<T>;
               #if LANGULUS_FEATURE(MANAGED_MEMORY)
                  const auto cloned_origin = Allocator::Allocate(
                     MetaDataOf<originT>(),
                     pot_t(Roof2(sizeof(originT)))
                  );
               #else
                  const auto cloned_origin = Allocator::Allocate(
                     pot_t(alignof(originT)),
                     pot_t(Roof2(sizeof(originT)))
                  );
               #endif
               LglsAssert(cloned_origin, "Out of memory");
               [[maybe_unused]] Allocation const* const* ent;
               if constexpr (has_entries)
                  ent = self.template GetEntriesInner<SID>();

               if constexpr (indirects > 1) {
                  // Multiple indirections                              
                  #if LANGULUS_FEATURE(MANAGED_MEMORY)
                     const auto cloned_ptrs = Allocator::Allocate(
                        MetaDataOf<T>(),
                        pot_t(Roof2(sizeof(T) * (indirects - 1)))
                     );
                  #else
                     const auto cloned_ptrs = Allocator::Allocate(
                        pot_t(alignof(T)),
                        pot_t(Roof2(sizeof(T) * (indirects - 1)))
                     );
                  #endif
                  
                  if (not cloned_ptrs) {
                     Allocator::Deallocate(cloned_origin);
                     LglsError("Out of memory");
                  }
                  cloned_ptrs->AddRef(indirects - 2);

                  // Given dst being Text***, we have:                  
                  //    *dst = cloned_ptrs                              
                  //   **dst = cloned_origin                            
                  //  ***dst = ***src                                   
                  void** ptr = static_cast<void**>(static_cast<void*>(cloned_ptrs->GetBlockStart()));
                  *dst = ptr;
                  if constexpr (has_entries)
                     DecvqAllCast(*ent) = cloned_ptrs;

                  ForEachIndirection<Deptr<T>>([&src, &dst, &ent, &cloned_ptrs] {
                     // Chain all intermediate pointers                 
                     src = static_cast<void**>(*src);
                     dst = static_cast<void**>(*dst);

                     *dst = dst + 1;

                     if constexpr (has_entries) {
                        ++ent;
                        DecvqAllCast(*ent) = cloned_ptrs;
                     }
                  });
               }
               
               // The last indirection points to the cloned origin      
               *dst = cloned_origin->GetBlockStart();
               src = static_cast<void**>(*src);
               dst = static_cast<void**>(*dst);
               if constexpr (has_entries)
                  DecvqAllCast(*ent) = cloned_origin;
            }

            IntentNew(dst, Clone(*static_cast<Decay<T>*>(static_cast<void*>(src))));
         }
      }

   #if LANGULUS_FEATURE(MANAGED_MEMORY)
      /// Clone the 'rhs'.                                                    
      /// This is a more generic approach that is considerably slower.        
      ///   @attention Works in one dimension at a time!                      
      //TODO could benefit from static optimization                          
      template<Cid SID = ID, CT::Container C, CT::NoIntent IT> requires Relevant<SID>
      void EmplaceByCloningCustomPointers(this C& self, IT const& rhs) {
         /*static_assert(CT::ContainsOne<C>,
            "Emplacing only first element in a container with many. "
            "GetHandle() first?"
         );*/

         void const* src_origin;         
         if constexpr (CT::Handle<IT>) {
            if constexpr (CT::TypeErased<C> or CT::TypeErased<IT>)
               LglsAssumeDev(self.template IsSame<SID>(rhs.template GetType<SID>()), "Type mismatch");
            else
               static_assert(Same<TypeOf<C, SID>, TypeOf<IT, SID>>, "Type mismatch");
            
            src_origin = rhs.template GetDense<SID>().GetRaw();
         }
         else {
            if constexpr (CT::TypeErased<C>)
               LglsAssumeDev((self.template IsSame<IT, SID>()), "Type mismatch");
            else
               static_assert(Same<TypeOf<C, SID>, IT>, "Type mismatch");
            
            src_origin = static_cast<const void*>(&DenseCast(rhs));
         }

         // Clone the origin first                                      
         const DMeta T = self.template GetType<SID>();
         auto indirections = T.GetIndirections();

         if (indirections) {
            // Containing sparse data                                   
            DMeta prev_type = T.GetDeptr(indirections - 1);
            DMeta type = T.GetOrigin();
            auto cloned = Allocator::AllocatePackedInner(
               prev_type.GetPointerSpecification(),
               type, pot_t(Roof2(type.GetSize()))
            );
            LglsAssert(cloned, "Out of memory");

            type.GetCloneConstructor()(
               const_cast<void*>(src_origin),
               cloned->GetBlockStart()
            );
            --indirections;

            // Then clone all indirection layers in reverse order       
            [[maybe_unused]] EntryPtr entries;
            if constexpr (CT::OwnedDeep<C>) {
               entries = self.template GetEntriesInner<SID>();
               DecvqAllCast(entries[indirections]) = cloned;
            }
         
            auto next_pointer = cloned->GetBlockStartPacked(prev_type.GetPointerSpecification());
            while (indirections) {
               type = prev_type;
               prev_type = T.GetDeptr(indirections - 1);
               cloned = Allocator::AllocatePackedInner(
                  prev_type.GetPointerSpecification(),
                  type, pot_t(Roof2(type.GetSize()))
               );

               // Chain the pointers                                    
               memcpy(cloned->GetBlockStart(), &next_pointer, type.GetSize());
               next_pointer = cloned->GetBlockStartPacked(prev_type.GetPointerSpecification());
               --indirections;

               // Save the new indirection allocation                   
               if constexpr (CT::OwnedDeep<C>) {
                  DecvqAllCast(entries[indirections]) = cloned;
               }
            }

            // The final indirection is stored in mHeap                 
            memcpy(self.template GetRawVoid<SID>(), &next_pointer, T.GetSize());
         }
         else {
            // Containing dense data                                    
            T.GetCloneConstructor()(
               const_cast<void*>(src_origin),
               self.template GetRawVoid<SID>()
            );
         }
      }
   #endif

      /// Emplace on top of the first element using an intent                 
      ///   @attention Assumes destination memory has been preallocated,      
      ///      including all levels of indirection.                           
      ///   @attention Does not modify any container state.                   
      ///   @attention This overwrites previous handle without dereferencing  
      ///      it and without destroying anything.                            
      ///   @attention Works in one dimension at a time!                      
      ///   @param intent constructor argument. If this container             
      ///      is statically typed, this can be any constructor argument,     
      ///      otherwise it has to be an instance of the contained type.      
      template<Cid SID = ID, CT::Container C, CT::Intent I> requires Relevant<SID>
      void EmplaceWithIntent(this C&& self, I&& intent) {
         /*static_assert(CT::ContainsOne<C>,
            "Emplacing only first element in a container with many. "
            "GetHandle() first?"
         );*/
         using IT = Decvq<Deref<TypeOf<I>>>;
         LglsAssumeDev(self.template GetRaw<SID>(), "Invalid heap");
         LglsAssumeDev(self.template IsTyped<SID>(), "Invalid type");
         decltype(auto) rhs = LglsFwd(intent.what);
         /*static_assert(not CT::Copied<I> or not CT::HeapAllocated<C>,
            "Since this function assumes container has been preallocated, "
            "it makes no sense to copy here unless data is on the stack - "
            "it should be handled outside this call."
         );*/ // this is generally true, but EmplaceWithIntent may be used on assignment, and in those cases this usage is valid

         if constexpr (CT::Cloned<I>) {
            // Clone a handle or element                                
            #if LANGULUS_FEATURE(MANAGED_MEMORY)
               ThisCom::template EmplaceByCloningCustomPointers<SID>(rhs);
            #else
               ThisCom::template EmplaceByCloningStandardPointers<SID>(rhs);
            #endif
         }
         else if constexpr (CT::Handle<IT>) {
            // We're emplacing using a handle, which can be faster due  
            // to carrying allocation data with itself when sparse,     
            // instead of searching for it when having DeepOwnership.   
            // Doesn't matter if managed memory is disabled.            
            // We emplace each dimension separately.                    
            void* const dst = self.template GetRawVoid<SID>();

            if constexpr (CT::TypeErased<C, IT>) {
               //                                                       
               // Either this container or the handle is type-erased    
               auto T = rhs.template GetType<SID>();
               LglsAssumeDev(self.template IsSame<SID>(T), "Type mismatch");
               void* const src = rhs.template GetRawVoid<SID>();

               if constexpr (CT::Moved<I>) {
                  /*if (rhs.template IsConstant<SID>())
                     T.GetReferConstructor()(src, dst);
                  else*/
                     T.GetMoveConstructor()(src, dst);
               }
               else if constexpr (CT::Abandoned<I>) {
                  /*if (rhs.template IsConstant<SID>())
                     T.GetReferConstructor()(src, dst);
                  else*/
                     T.GetAbandonConstructor()(src, dst);
               }
               else if constexpr (CT::Referred<I>)
                  T.GetReferConstructor()(src, dst);
               else if constexpr (CT::Disowned<I>)
                  T.GetDisownConstructor()(src, dst);
               else if constexpr (CT::Copied<I>)
                  T.GetCopyConstructor()(src, dst);
               else if constexpr (CT::Cloned<I>)
                  T.GetCloneConstructor()(src, dst);
               else
                  static_assert(false, "Unrecognized intent");
            }
            else {
               //                                                       
               // Both sides are statically-typed and we can benefit    
               // from a lot of compile-time optimizations.             
               if constexpr (CT::Typed<C, IT>)
                  static_assert(Same<TypeOf<C, SID>, TypeOf<IT, SID>>, "Type mismatch");
               else
                  LglsAssumeDev(self.template IsSame<SID>(rhs), "Type mismatch");

               using T = Tif<CT::Typed<C>, TypeOf<C, SID>, TypeOf<IT, SID>>;
               if constexpr (CT::Mutable<T> or not I::IsMoved())
                  IntentNew(dst, I::Nest(*rhs.template GetRawAs<T, SID>()));
               else
                  IntentNew(dst, Refer(*rhs.template GetRawAs<T, SID>()));
            }
               
            if_available(self.template EmplaceEntries<SID>(LglsFwd(intent)));
         }
         else {
            void* const dst = self.template GetRawVoid<SID>();

            if constexpr (CT::TypeErased<C, IT>) {
               //                                                       
               // This container is type-erased                         
               LglsAssumeDev((self.template IsSame<IT, SID>()), "Type mismatch");
               auto T = self.template GetType<SID>();
               void* const src = const_cast<void*>(static_cast<const void*>(&rhs));
               
               if constexpr (CT::Moved<I>)
                  T.GetMoveConstructor()(src, dst);
               else if constexpr (CT::Abandoned<I>)
                  T.GetAbandonConstructor()(src, dst);
               else if constexpr (CT::Referred<I>)
                  T.GetReferConstructor()(src, dst);
               else if constexpr (CT::Disowned<I>)
                  T.GetDisownConstructor()(src, dst);
               else if constexpr (CT::Copied<I>)
                  T.GetCopyConstructor()(src, dst);
               else if constexpr (CT::Cloned<I>)
                  T.GetCloneConstructor()(src, dst);
               else
                  static_assert(false, "Unrecognized intent");
            }
            else {
               //                                                       
               // This container is statically-typed                    
               if constexpr (CT::Typed<C>)
                  static_assert(Same<TypeOf<C, SID>, IT>, "Type mismatch");
               else
                  LglsAssumeDev((self.template IsSame<IT, SID>()), "Type mismatch");

               IntentNew(dst, LglsFwd(intent));
            }

            if_available(self.template EmplaceEntries<SID>(LglsFwd(intent)));
         }
      }
      
      /// Emplace a new default-constructed item at the first position.       
      ///   @attention This overwrites previous handle without dereferencing  
      ///      it, and without destroying anything                            
      ///   @attention Doesn't modify count                                   
      ///   @attention Works in one dimension at a time! Beware when dealing  
      ///      with multiple dimensions!                                      
      template<Cid SID = ID, AllocationStrategy STRAT = AllocationStrategy::FreshAllocate, class E = void, CT::Container C>
      requires Relevant<SID>
      void EmplaceDefault(this C& self) {
         static_assert(CT::ContainsOne<C>,
            "Emplacing only first element in a container with many. "
            "GetHandle() first?"
         );

         if constexpr (STRAT == AllocationStrategy::FreshAllocate) {
            if_available(self.ResetState());
         }

         if constexpr (CT::TypeErased<C>) {
            //                                                          
            // This container is type-erased                            
            if constexpr (STRAT != AllocationStrategy::DontAllocate) {
               static_assert(not Shared,
                  "Can't EmplaceDefault one dimension at a time in a "
                  "type-erased container. All types need to be set "
                  "prior to allocating for this to work. You have to "
                  "manually call SetType and AllocateFresh prior to "
                  "calling this function with STRAT == DontAllocate"
               );
   
               if constexpr (CT::NotVoid<E>)
                  self.template SetType<E, SID>();
            }

            if constexpr (CT::NotVoid<E>) {
               // The type we're constructing is statically known       
               static_assert(CT::Defaultable<E>,
                  "Contained type is not default-constructible");
               LglsAssumeDev((self.template IsSame<E, SID>()), "Type mismatch");

               // Allocate if we have to                                
               if constexpr (STRAT == AllocationStrategy::FreshAllocate)
                  self.template AllocateFresh<SID>(1);
               else if constexpr (STRAT == AllocationStrategy::Reallocate)
                  self.template AllocateMore<SID>(1);

               // Construct the first element                           
               new (self.template GetRaw<SID>()) E {};

               if constexpr (CT::Sparse<E>)
                  if_available(*self.template GetEntriesInner<SID>() = nullptr);
            }
            else {
               // The type we're constructing isn't statically known    
               auto T = self.template GetTypeInner<SID>();
               LglsAssert(T,
                  "Unknown type for default-construction");
               auto constructor = T.GetDefaultConstructor();
               LglsAssert(constructor,
                  "Contained type is not default-constructible");

               // Allocate if we have to. Do it only after we're sure   
               // that construction is possible                         
               if constexpr (STRAT == AllocationStrategy::FreshAllocate)
                  self.template AllocateFresh<SID>(1);
               else if constexpr (STRAT == AllocationStrategy::Reallocate)
                  self.template AllocateMore<SID>(1);

               // Construct the first element                           
               constructor(self.template GetRaw<SID>());

               if constexpr (requires { self.template GetEntriesInner<SID>(); }) {
                  if (T.IsSparse())
                     *self.template GetEntriesInner<SID>() = nullptr;
               }
            }
         }
         else {
            //                                                          
            // This container is statically-typed. E is ignored.        
            // Allocate if we have to                                   
            if constexpr (STRAT == AllocationStrategy::FreshAllocate) {
               self.template AllocateFresh<SID>(1);
            }
            else if constexpr (STRAT == AllocationStrategy::Reallocate) {
               self.template AllocateMore<SID>(1);
            }

            // Construct the first element                              
            using T = TypeOf<C, SID>;
            LglsAssumeDev((self.template IsSame<T, SID>()), "Type mismatch");
            new (self.template GetRaw<SID>()) T {};
            
            if constexpr (CT::Sparse<T>)
               if_available(*self.template GetEntriesInner<SID>() = nullptr);
         }

         // Update count                                                
         if constexpr (STRAT == AllocationStrategy::FreshAllocate) {
            if_available(self.template SetCountInner<SID>(1));
         }
         else if constexpr (STRAT == AllocationStrategy::Reallocate
         and requires { self.template SetCountInner<SID>(1); }) {
            if (self.template IsEmpty<SID>())
               self.template SetCountInner<SID>(1);
         }

         // Update hash                                                 
         if_available(self.template SetHashInner<SID>(0));
      }

      /// Emplace a new manually constructed item at the first position.      
      /// If zero arguments were provided, this will EmplaceDefault.          
      /// Supports describe-construction and handles.                         
      ///   @attention This overwrites previous handle without dereferencing  
      ///      it, and without destroying anything!                           
      ///   @attention Works in one dimension at a time! Beware when dealing  
      ///      with multiple dimensions!                                      
      template<Cid SID = ID, AllocationStrategy STRAT = AllocationStrategy::FreshAllocate, class E = void, CT::Container C, class...A>
      requires Relevant<SID>
      void EmplaceConstruct(this C& self, A&&...arguments) {
         static_assert(sizeof...(A) > 0,
            "No arguments - use EmplaceDefault instead");

         if constexpr (STRAT == AllocationStrategy::FreshAllocate) {
            if_available(self.ResetState());
         }

         if constexpr (CT::TypeErased<C>) {
            //                                                          
            // This container is type-erased                            
            if constexpr (STRAT != AllocationStrategy::DontAllocate) {
               static_assert(not Shared,
                  "Can't EmplaceConstruct one dimension at a time in a "
                  "type-erased container. All types need to be set "
                  "prior to allocating for this to work. You have to "
                  "manually call SetType and AllocateFresh prior to "
                  "calling this function with STRAT == DontAllocate"
               );
            }

            if constexpr (sizeof...(A) == 1) {
               using A1 = typename Types<A...>::First;
               
               if constexpr (Same<A1, Describe>) {
                  // Describe-construct first element                   
                  if constexpr (CT::NotVoid<E>) {
                     // The type we're describing is statically known   
                     static_assert(CT::Dense<E>,
                        "Describe-construction works only for dense data");
                     static_assert(CT::DescribeConstructible<E>,
                        "Contained type is not describe-constructible");

                     if constexpr (STRAT != AllocationStrategy::DontAllocate)
                        self.template SetType<E, SID>();
                     LglsAssumeDev((self.template IsSame<E, SID>()), "Type mismatch");

                     // Allocate if we have to                          
                     if constexpr (STRAT == AllocationStrategy::FreshAllocate)
                        self.template AllocateFresh<SID>(1);
                     else if constexpr (STRAT == AllocationStrategy::Reallocate)
                        self.template AllocateMore<SID>(1);

                     new (self.template GetRaw<SID>()) E {LglsFwd(arguments)...};
                  }
                  else {
                     // The type we're describing isn't known statically
                     auto T = self.template GetTypeInner<SID>();
                     LglsAssert((bool) T,
                        "Unknown type for describe-construction");
                     LglsAssert(T.IsDense(),
                        "Describe-construction works only for dense data");
                     auto constructor = T.GetDescribeConstructor();
                     LglsAssert(constructor,
                        "Contained type is not describe-constructible");

                     // Allocate if we have to. Do it only after we're  
                     // sure that construction is possible              
                     if constexpr (STRAT == AllocationStrategy::FreshAllocate)
                        self.template AllocateFresh<SID>(1);
                     else if constexpr (STRAT == AllocationStrategy::Reallocate)
                        self.template AllocateMore<SID>(1);

                     // Describe-construct the first element            
                     constructor(self.template GetRaw<SID>(), LglsFwd(arguments.what)...);
                  }
               }
               else {
                  if constexpr (STRAT != AllocationStrategy::DontAllocate) {
                     if constexpr (CT::Handle<A1>)
                        self.AbsorbType(Copy(arguments)...);
                     else
                        self.DeduceType(arguments...);
                  }

                  // Allocate if we have to                             
                  if constexpr (STRAT == AllocationStrategy::FreshAllocate)
                     self.template AllocateFresh<SID>(1);
                  else if constexpr (STRAT == AllocationStrategy::Reallocate)
                     self.template AllocateMore<SID>(1);

                  // Construct the first element                        
                  if constexpr (CT::Copied<IntentOf(arguments)...>)
                     ThisCom::template EmplaceWithIntent<SID>(Refer(LglsFwd(arguments))...);
                  else
                     ThisCom::template EmplaceWithIntent<SID>(FWDIntent(arguments)...);
               }
            }
            else {
               static_assert(CT::NotVoid<E>,
                  "Too many arguments for emplacing in a type-erased container. "
                  "You should provide an 'E' type.");
               static_assert(CT::Dense<E>,
                  "Too many arguments for emplacing a sparse instance");

               // Set type if we have to                                
               if constexpr (STRAT != AllocationStrategy::DontAllocate)
                  self.template SetType<E, SID>();
               LglsAssumeDev((self.template IsSame<E, SID>()), "Type mismatch");

               // Construct the first element                           
               ThisCom::template EmplaceWithIntent<SID>(Abandon{E {LglsFwd(arguments)...}});
            }
         }
         else {
            //                                                          
            // This container is statically-typed. E is ignored.        
            // Allocate if we have to                                   
            if constexpr (STRAT == AllocationStrategy::FreshAllocate) {
               if_available(self.template AllocateFresh<SID>(1));
            }
            else if constexpr (STRAT == AllocationStrategy::Reallocate) {
               if_available(self.template AllocateMore<SID>(1));
            }

            // Construct the first element                              
            using T = TypeOf<C, SID>;
            if constexpr (sizeof...(A) == 1 and (Same<T, Deint<A>...> or CT::Handle<A...>)) {
               if constexpr (CT::Copied<IntentOf(arguments)...>)
                  ThisCom::template EmplaceWithIntent<SID>(Refer(LglsFwd(arguments))...);
               else
                  ThisCom::template EmplaceWithIntent<SID>(FWDIntent(arguments)...);
            }
            else {
               static_assert(CT::Dense<T>,
                  "Too many arguments for emplacing a sparse instance");

               if constexpr (requires { Decvq<T> {LglsFwd(arguments)...}; } )
                  ThisCom::template EmplaceWithIntent<SID>(Abandon {Decvq<T> {LglsFwd(arguments)...}});
               else
                  ThisCom::template EmplaceWithIntent<SID>(Abandon {Decvq<T> {LglsFwd(arguments.what)...}});
            }
         }
         
         // Update count always _after_ emplacement - EmplaceWithIntent 
         // might throw, and we wouldn't want to have valid elements if 
         // this happens!                                               
         if constexpr (STRAT == AllocationStrategy::FreshAllocate) {
            if_available(self.template SetCountInner<SID>(1));
         }
         else if constexpr (STRAT == AllocationStrategy::Reallocate
         and requires { self.template SetCountInner<SID>(1); }) {
            if (self.template IsEmpty<SID>())
               self.template SetCountInner<SID>(1);
         }

         // Update hash                                                 
         if_available(self.template SetHashInner<SID>(0));
      }
   };

   #undef ThisCom
}
