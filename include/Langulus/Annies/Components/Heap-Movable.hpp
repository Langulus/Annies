///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Heap-Reference.hpp"
#include "Langulus/Assume.hpp"
#include "Langulus/Typenav.hpp"


namespace Langulus::Annies::Component
{
   /// Refers back to this particular component instance through the deduced  
   /// 'this'. Just for convenience. It is #undef-ed at the end of this file. 
   #define ThisCom self.HeapMovable<INITIAL_SIZE, GROWTH_FACTOR, ENTRY0, ENTRYN...>

   ///                                                                        
   /// Interfaces a heap. Adds a member that points to the heap memory.       
   /// The heap is allowed to move on reallocation.                           
   ///   @tparam INITIAL_SIZE the initial size (in elements). Used in hashed  
   ///      containers in order to control hash table size. If 0, the heap    
   ///      will use reflected type properties only.                          
   ///   @tparam GROWTH_FACTOR growth factor on reallocation. Used in hashed  
   ///      containers in order to control hash table growth on reallocation. 
   ///      If 0, the heap will grow according to reflected type properties.  
   ///   @tparam ENTRY0 the first entry                                       
   ///   @tparam ENTRYN optional extensions that include more data into       
   ///      the heap allocation. Each ID must correspond to a matching type   
   ///      component ID. Each entry also allows for pointer customization,   
   ///      including support for packed pointers.                            
   template<unsigned INITIAL_SIZE, unsigned GROWTH_FACTOR, CT::HeapEntry ENTRY0, CT::HeapEntry...ENTRYN>
   struct HeapMovable : HeapReference<ENTRY0, ENTRYN...> {
      using Id = typename HeapReference<ENTRY0, ENTRYN...>::Id;
      using HeapProvider = Id;

      static constexpr unsigned InitialSize  = INITIAL_SIZE;
      static constexpr unsigned GrowthFactor = GROWTH_FACTOR;
      static constexpr bool Reallocatable = true;
      //template<Cid SID>
      //static constexpr bool Relevant = Id::template Contains<SID>;

   protected:
      LglsComIterationOperators(friend);
      LglsComReserveEmergent(friend);
      LglsComInsertion(friend);
      LglsComMerging(friend);
      LglsComAssignment(friend);
      LglsComEmplacement(friend);
      LglsComConversion(friend);
      LglsComOwnershipEmergent(friend);

      template<CT::Container C>
      using Count = typename Deref<C>::CountType;

      template<CT::Container C>
      static constexpr auto CountMax = ::std::numeric_limits<Count<C>>::max();

      using Base = HeapReference<ENTRY0, ENTRYN...>;
      
      /// MARK: ConstructDefault                                              
      /// Default-initialize the heap pointer                                 
      constexpr void ConstructDefault(this auto& self) noexcept {
         ThisCom::SetHeapInner(nullptr);
      }
      
      /// MARK: SliceFrom                                                     
      /// Transfer from any kind of container, respecting intents.            
      /// Do it for a particular dimension.                                   
      ///   @param intent The intent and container to transfer from.          
      template<Cid D, class C, CT::Intent I> requires CT::Container<I>
      void SliceFrom(this C& self, I&& intent) {
         static_assert(CT::Disowned<I>);
         ThisCom::SetHeapInner(intent->template GetRaw<D>());
      }

      /// MARK: ConstructFrom                                                 
      /// Transfer from any kind of container, respecting intents             
      ///   @param intent The intent and container to transfer from.          
      ///   @param reserve Optional reserve override, which is taken into     
      ///      account only when we're cloning or copying, as only then       
      ///      a new allocation occurs.                                       
      template<class C, CT::Intent I> requires CT::Container<I>
      void ConstructFrom(this C& self, I&& intent, size_t reserve = 0) {
         using IT = Decvq<Deref<Deint<I>>>;
         decltype(auto) from = LglsFwd(intent.what);

         size_t count = from.template GetCount<Id::First>();
         if constexpr (CT::Copied<I> or CT::Cloned<I> or not CT::HeapAllocated<I>) {
            // Do a copy or clone.                                      
            // Verify that all dimensions are copiable/clonable, and    
            // make sure that 'count' and 'reserve' are consistent      
            // across all dimensions.                                   
            Id::ForEach([&]<Cid D> {
               LglsAssumeDev(from.template GetCount<D>() == from.template GetCount<Id::First>(),
                  "Inconsistent count across dimensions");
               LglsAssumeDev(from.template GetReserved<D>() == from.template GetReserved<Id::First>(),
                  "Inconsistent reserve across dimensions");

               if constexpr (CT::TypeErased<IT>) {
                  auto type = from.template GetType<D>();
                  if constexpr (CT::Cloned<I>) {
                     LglsAssert(type.GetCloneConstructor(),
                        "Can't clone-construct elements"
                        " - no clone-constructor was reflected for type ",
                        type
                     );
                  }
                  else {
                     LglsAssert(type.GetReferConstructor(),
                        "Can't refer-construct elements"
                        " - no refer-constructor was reflected for type ",
                        type
                     );
                  }
               }
               else {
                  if constexpr (CT::Cloned<I>) {
                     static_assert(CT::CloneConstructible<TypeOf<IT, D>>,
                        "Contained type is not clone-constructible");
                  }
                  else {
                     static_assert(CT::ReferConstructible<TypeOf<IT, D>>,
                        "Contained type is not refer-constructible");
                  }
               }
            });

            // Allocate new memory and set count, so that handle        
            // iteration is valid                                       
            if constexpr (CT::Contiguous<C>)
               ThisCom::AllocateFresh(count > reserve ? count : reserve);
            else {
               const auto rhs_reserve = from.template GetReserved<Id::First>();
               ThisCom::AllocateFresh(rhs_reserve > reserve ? rhs_reserve : reserve);
            }

            if_available(self.template SetCountInner<Id::First>(count));

            // Copy/Clone items                                         
            if constexpr (CT::TypeErased<C>) {
               if  (self.template GetType<ENTRY0::Id>().IsPOD()
               and (self.template GetType<ENTRYN::Id>().IsPOD() and ...)
               and  self.template GetType<ENTRY0::Id>().IsDense()
               and (self.template GetType<ENTRYN::Id>().IsDense() and ...))
                  self.CloneAllItemsBatched(from);
               else
                  self.template CopyOrCloneAllItemsOneByOne<CT::Cloned<I>>(from);
            }
            else {
               if constexpr (CT::POD   <TypeOf<C, ENTRY0::Id>, TypeOf<C, ENTRYN::Id>...>
               and           CT::Dense <TypeOf<C, ENTRY0::Id>, TypeOf<C, ENTRYN::Id>...>)
                  self.CloneAllItemsBatched(from);
               else
                  self.template CopyOrCloneAllItemsOneByOne<CT::Cloned<I>>(from);
            }
                     
            // Full success                                             
            if constexpr (requires { from.template GetHashInner<Id::First>(); }) {
                    if_available(self.template SetHashInner<Id::First>(from.template GetHashInner<Id::First>()))
               else if_available(self.template ResetHash<Id::First>())
            }
         }
         else {
            // Move/Refer/Abandon/Disown other                          
            /// @attention this should never be reached if I is stack   
            ///    allocated                                            
            ThisCom::SetHeapInner(from.template GetRaw<Id::First>());

            if constexpr (CT::Moved<I> and CT::OwnedStrong<I>) {
               // We are moving 'from'. If it is owned on destruction,  
               // it needs to be fully reset.                           
               from.template SetHeapInner<Id::First>(nullptr);
            }
            else if constexpr (CT::Abandoned<I> and CT::OwnedStrong<I>) {
               // We are abandoning 'from'.                             
               // First and foremost, if 'from' supports State::Disowned
               // we do just that in the last ownership component.      
               // If disownment is not supported, we need to reset all  
               // properties responsible for dereferencing and          
               // destruction. This will be done in their respective    
               // components, if possible:                              
               //    1. Allocation responsible for shallow ownership    
               //    2. Count responsible for deep ownership            
               if constexpr (not IT::CanBeDisowned
               and (not requires { from.template SetCountInner<Id::First>(0); }
                    or IT::CountHeapRequests())
               ) {
                  // Some monocontainers have no variable count, and in 
                  // some cases the heap pointer is used to specify the 
                  // count of 1. In these cases, this component is      
                  // responsible to reset the pointer. Same applies in  
                  // the cases where vital properties are positioned on 
                  // the heap (CountHeap/HashHeap/OwnershipDeepHeap)    
                  from.template SetHeapInner<Id::First>(nullptr);
               }
            }
         }
      }
      
      /// MARK: AllocateFresh                                                 
      /// Allocate a fresh allocation                                         
      ///   @attention works on all relevant dimensions at once               
      ///   @attention changes allocation, heap pointer and reserve count only
      ///   @param request request to fulfill                                 
      template<Cid SID = Id::First, CT::Container C> //requires Relevant<SID>
      void AllocateFresh(this C& self, size_t elements /*const Request& request*/) {
         const auto request = ThisCom::RequestHeap(elements);

         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            auto al = Allocator::Allocate(self.template GetType<SID>(), request.mTotalBytes);
         #else
            auto al = Allocator::Allocate(self.template GetAlignment<SID>(), request.mTotalBytes);
         #endif
         LglsAssert(al, "Out of memory");
         
         ThisCom::SetHeapInner(static_cast<void*>(al->GetBlockStart() + request.mHeaderBytes));
         self.template SetAllocationInner<SID>(al);
         if_available(self.template SetReservedInner<SID>(request.mReserved));
         Id::ForEach([&self]<Cid D>{
            self.template ConstructHeapRequestPerDimension<D>();
         });
         if_available(self.ConstructHeapRequestGlobal());
      }

      /// MARK: AllocateMore                                                  
      /// Allocate a number of elements, relying on the type of the container 
      ///   @attention assumes container is typed                             
      ///   @attention works on all relevant dimensions at once               
      ///   @param elements number of elements to allocate                    
      template<Cid SID = Id::First, CT::Container C> //requires Relevant<SID>
      void AllocateMore(this C& self, Count<C> elements) {
         static_assert(not CT::Handle<C>, "Handles aren't allowed to reallocate");
         LglsAssumeDev(elements > self.template GetCount<SID>(), "Bad element count");
         const auto al = DecvqAllCast(self.template GetAllocation<SID>());

         if (not al) {
            //                                                          
            // Allocate a fresh set of elements                         
            ThisCom::AllocateFresh(elements);
            return;
         }

         LglsAssumeDev(al->GetUses() == 1,
            "Can't reuse memory of a heap used from multiple places. "
            "Container should've branched-out prior to AllocateMore. "
         );

         const auto request = ThisCom::RequestHeap(elements);
         if constexpr (CT::ContainsMany<C>) {
            if (self.template GetReserved<SID>() >= request.mReserved)
               return;

            if (request.mTotalBytes <= al->GetSize()) {
               // In some cases, no reallocation happens, but reserved  
               // count may still change, due to the allocation being   
               // rounded to the closest power-of-two. Move heap footers
               // accordingly in such cases.                            
               self.RemapAllHeapRequests(request.mReserved);
               return;
            }

            //                                                          
            // Reallocate                                               
            C previous {Abandon {self}};
            if_available(self.DisableDisowned());

            #if LANGULUS_FEATURE(MANAGED_MEMORY)
               auto reallocated = Allocator::Reallocate(self.template GetType<SID>(), request.mTotalBytes, al);
            #else
               auto reallocated = Allocator::Reallocate(request.mTotalBytes, al);
            #endif
            LglsAssert(reallocated, "Out of memory");

            self.template SetAllocationInner<SID>(reallocated);

            if (reallocated != al) {
               // Memory moved, and we should move all elements with    
               // it. We're moving to new memory, so no reverse is      
               // required.                                             
               ThisCom::SetHeapInner(static_cast<void*>(reallocated->GetBlockStart() + request.mHeaderBytes));

               if (previous.GetCount()) {
                  auto to = self.GetHandle().ForceMutable();
                  try {
                     previous.template Apply<false>([&to,&self,&previous](auto&& from) {
                        (void) self; (void) previous;

                        if constexpr (CT::Supported<decltype(from)>) {
                           Id::ForEach([&]<Cid D>{
                              to.template EmplaceWithIntent<D>(Abandon(from));
                           });
                        }
                        ++to;
                     });
                  }
                  catch (...) {
                     self.template SetCountInner<SID>(to - self.GetHandle());
                     throw;
                  }
               }

               self.TransferAllHeapRequests(previous, request.mReserved);
            }
            else {
               // Memory didn't move, make sure we don't free anything  
               LglsAssumeDev(self.template GetAllocationInner<SID>() == reallocated,
                  "Allocation pointer should still be the same");

               if_available(previous.EnableDisowned())
               else previous.template SetCountInner<SID>(0);
               self.RemapAllHeapRequests(request.mReserved);
            }
         }
      }

      /// MARK: AllocateLess                                                  
      /// Shrink the block, depending on currently reserved	elements.         
      /// Initialized elements on the back will be destroyed.                 
      /// When MANAGED_MEMORY is enabled we have a strong guarantee that      
      /// allocations never move when shrinking.                              
      ///   @attention works on all relevant dimensions at once               
      ///   @param desiredReserve number of elements to reserve               
      template<Cid SID = Id::First, CT::Container C> //requires Relevant<SID>
      void AllocateLess(this C& self, const Count<C> desiredReserve) {
         static_assert(not CT::Handle<C>,
            "Handles aren't allowed to reallocate");
         static_assert(CT::ContainsMany<C>,
            "This makes sense to be called only by containers that support many elements");

         const auto al = DecvqAllCast(self.template GetAllocation<SID>());
         if (not al) {
            //                                                          
            // We have to branch out                                    
            const C backup {Abandon{self}};
            if_available(self.DisableDisowned());
            ThisCom::AllocateFresh(desiredReserve /*ThisCom::RequestHeap(desiredReserve)*/);

            // Reinsert only the relevant items                         
            auto to = self.GetHandle();
            try {
               backup.Apply([&to](auto& from) {
                  Id::ForEach([&]<Cid D>{
                     to.template EmplaceWithIntent<D>(Refer(from)); //TODO won't work for maps/sets
                  });
                  ++to;
               });
            }
            catch (...) {
               self.template SetCountInner<SID>(to - self.GetHandle());
               throw;
            }

            if constexpr (CT::Contiguous<C>) {
               self.template SetCountInner<SID>(backup.template GetCount<SID>() < desiredReserve
                  ? backup.template GetCount<SID>()
                  : desiredReserve
               );
            }
            else self.template SetCountInner<SID>(backup.template GetCount<SID>());
            return;
         }

         LglsAssumeDev(desiredReserve <= self.template GetReserved<SID>(),
            "Can't shrink allocation using more elements");
         LglsAssumeDev(al->GetUses() == 1,
            "Can't reuse memory of a block used from multiple places");

         if (self.template GetCount<SID>() > desiredReserve) {
            auto temp = self.SelectInner(desiredReserve);
            temp.template Free<false>();
            if_available(self.template SetCountInner<SID>(desiredReserve));
         }

         const auto request = ThisCom::RequestHeap(desiredReserve);
         if (self.template GetReserved<SID>() == request.mReserved)
            return;
      
         // Memory doesn't move, but reserved count changed so all      
         // HeapRequests which are PerElement need to be moved around   
         // _before_ we restrict the memory!                            
         self.RemapAllHeapRequests(request.mReserved);
   
         if (request.mTotalBytes <= al->GetSize()) {
            return;
         }

         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            self.template SetAllocationInner<SID>(
               Allocator::Reallocate(self.template GetType<SID>(), request.mTotalBytes, al)
            );
         #else
            self.template SetAllocationInner<SID>(
               Allocator::Reallocate(request.mTotalBytes, al)
            );
         #endif
      }

      /// MARK: GetLocalHeap                                                  
      /// Helper function that navigates to the start of the local heap       
      /// footer of a given dimension and custom reserve count                
      template<Cid SID, CT::Container C>
      auto* GetLocalHeap(this C&& self, const size_t reserved) assumptious {
         auto to_footer = self.template GetRawAs<uint8_t, Id::First>();
         Id::ForEachConstOr([&]<Cid i>{
            size_t indirections;
            if constexpr (CT::TypeErased<C>) {
               const auto T = self.template GetType<i>();
               LglsAssumeDev((bool) T, "Requesting allocation size for an untyped container");
               to_footer = Align(to_footer, T.GetAlignment());
               to_footer += reserved * T.GetSize();
               indirections = T.GetIndirections();
            }
            else {
               using T = TypeOf<C, i>;
               to_footer = Align(to_footer, alignof(T));
               to_footer += reserved * sizeof(T);
               indirections = IndirectsOf<T>;
            }

            if constexpr (i == SID)
               return true;
            else {
               to_footer += Decay<C>::template DefineHeapFooter<i>(reserved, indirections);
               return No {};
            }
         });
         return to_footer;
      }

      /// MARK: GetGlobalHeap                                                 
      /// Helper function that navigates to the start of the global heap      
      /// footer for a custom reserve count                                   
      template<CT::Container C>
      auto* GetGlobalHeap(this C&& self, const size_t reserved) assumptious {
         auto to_footer = self.template GetLocalHeap<Id::Last>(reserved);
         auto indirections = self.template GetIndirections<Id::Last>();
         to_footer += Decay<C>::template DefineHeapFooter<Id::Last>(reserved, indirections);

         /*auto to_footer = self.template GetRawAs<uint8_t, Id::First>();
         Id::ForEach([&self,&to_footer,&reserved]<Cid i>{
            if constexpr (CT::TypeErased<C>) {
               const auto T = self.template GetType<i>();
               LglsAssumeDev((bool) T, "Requesting allocation size for an untyped container");
               to_footer = Align(to_footer, T.GetAlignment());
               to_footer += reserved * T.GetSize();
            }
            else {
               using T = TypeOf<C, i>;
               to_footer = Align(to_footer, alignof(T));
               to_footer += reserved * sizeof(T);
            }

            to_footer += self.template DefineHeapFooter<i>(reserved);
         });*/
         return to_footer;
      }

      /// MARK: RemapGlobalHeapRequests                                       
      /// Remap global footer requests onto the new reserve                   
      ///   @attention since global heap footer is at the end of memory       
      ///      it should be moved before any other footer when enlarging, and 
      ///      vice versa when shrinking, so that we dont lose any data.      
      ///   @param oldReserved the currently reserved number of elements      
      ///   @param newReserved the newly reserved number of elements          
      ///   @attention works on all relevant dimensions at once!              
      template<bool SHRINKING, CT::Container C>
      void RemapGlobalHeapRequests(this C& self, const size_t oldReserved, const size_t newReserved) {
         size_t from[C::template CountHeapFooterRequests<Id::First>() + 1];
         size_t to  [C::template CountHeapFooterRequests<Id::First>() + 1];
         size_t idx = 1;
         from[0] = to[0] = 0;
         
         ForEach(typename C::ComponentList{}, [&]<class COM> {
            if constexpr (requires { typename COM::HeapRequest; }) {
               using R = typename COM::HeapRequest;
               if constexpr (IsGlobalFooterRequest<R>) { //TODO maybe check for intersection?
                  size_t shift = sizeof(TypeOf<R>);

                  if constexpr (R::AllocatedPerElement) {
                     from[idx] = from[idx-1] + shift * oldReserved;
                     to  [idx] = to  [idx-1] + shift * newReserved;
                  }
                  else {
                     from[idx] = from[idx-1] + shift;
                     to  [idx] = to  [idx-1] + shift;
                  }

                  ++idx;
               }
            }            
         });

         // Calculate the new destination                               
         const auto to_footer = self.GetGlobalHeap(newReserved);
         const auto from_footer = self.GetGlobalHeap(oldReserved);

         --idx;
         if constexpr (not SHRINKING) {
            // When newReserved is larger than reserved, stuff has to   
            // move left to right, so it must be done in reverse so that
            // we don't destroy any data. The newly formed gaps need    
            // to be filled with zeroes.                                
            while (idx) {
               --idx;
               const auto src_range = from[idx + 1] - from[idx];
               LglsAssumeDev(src_range,
                  "Empty ranges should've been omitted in the previous loop");
                  
               memmove(to_footer + to[idx], from_footer + from[idx], src_range);
               const auto dst_range = to[idx + 1] - to[idx];
               memset (to_footer + to[idx] + src_range, 0, dst_range - src_range);
            }
         }
         else {
            // When newReserved is smaller than reserved, stuff has to  
            // move right to left. No gaps will be formed.              
            for (size_t i = 0; i < idx; ++i) {
               const auto src_range = from[i + 1] - from[i];
               LglsAssumeDev(src_range,
                  "Empty ranges should've been omitted in the previous loop");

               memmove(to_footer + to[i], from_footer + from[i], src_range);
            }
         }
      }

      /// MARK: RemapLocalHeapRequests                                        
      /// Remap footer requests onto the new reserve                          
      ///   @param newReserved the newly reserved number of elements          
      ///   @attention works on one dimension at a time!                      
      //TODO shouldn't this also move any dimensions != 0 as well, if in the same heap?????
      //TODO this will nullify a new hash table, but it doesn't call SetHashTableInner to move the pointer if kept on the stack!! 
      //TODO i've worked around this by using IndexedHashHeap instead of IndexedHashStack for sets and maps, for now
      template<Cid SID, bool SHRINKING, CT::Container C>
      void RemapLocalHeapRequests(this C& self, const size_t oldReserved, const size_t newReserved) {
         [[maybe_unused]]
         const auto indirect = self.template GetIndirections<SID>();

         size_t from[C::template CountHeapFooterRequests<SID>() + 1];
         size_t to  [C::template CountHeapFooterRequests<SID>() + 1];
         size_t idx = 1;
         from[0] = to[0] = 0;
         
         ForEach(typename C::ComponentList{}, [&]<class COM> {
            if constexpr (requires { typename COM::HeapRequest; }) {
               using R = typename COM::HeapRequest;
               if constexpr (IsLocalFooterRequest<R> and COM::Id::template Contains<SID>) {
                  size_t shift = sizeof(TypeOf<R>);
                  if constexpr (R::AllocatedPerIndirection) {
                     shift *= indirect;
                     if (not shift)
                        return;
                  }

                  if constexpr (R::AllocatedPerElement) {
                     from[idx] = from[idx-1] + shift * oldReserved;
                     to  [idx] = to  [idx-1] + shift * newReserved;
                  }
                  else {
                     from[idx] = from[idx-1] + shift;
                     to  [idx] = to  [idx-1] + shift;
                  }

                  ++idx;
               }
            }            
         });

         // Calculate the new destination                               
         const auto to_footer = self.template GetLocalHeap<SID>(newReserved);
         const auto from_footer = self.template GetLocalHeap<SID>(oldReserved);

         --idx;
         if constexpr (not SHRINKING) {
            // When newReserved is larger than reserved, stuff has to   
            // move left to right, so it must be done in reverse so that
            // we don't destroy any data. The newly formed gaps need    
            // to be filled with zeroes.                                
            while (idx) {
               --idx;
               const auto src_range = from[idx + 1] - from[idx];
               LglsAssumeDev(src_range,
                  "Empty ranges should've been omitted in the previous loop");
                  
               memmove(to_footer + to[idx], from_footer + from[idx], src_range);
               const auto dst_range = to[idx + 1] - to[idx];
               memset (to_footer + to[idx] + src_range, 0, dst_range - src_range);
            }
         }
         else {
            // When newReserved is smaller than reserved, stuff has to  
            // move right to left. No gaps will be formed.              
            for (size_t i = 0; i < idx; ++i) {
               const auto src_range = from[i + 1] - from[i];
               LglsAssumeDev(src_range,
                  "Empty ranges should've been omitted in the previous loop");

               memmove(to_footer + to[i], from_footer + from[i], src_range);
            }
         }
      }

      /// MARK: TransferGlobalHeapRequests                                    
      /// Transfer global footer requests onto the new reserve and heap       
      ///   @attention since global heap footer is at the end of memory       
      ///      it should be moved before any other footer when enlarging, and 
      ///      vice versa when shrinking, so that we dont lose any data.      
      ///   @param oldReserved the currently reserved number of elements      
      ///   @param newReserved the newly reserved number of elements          
      ///   @attention works on all relevant dimensions at once!              
      template<bool SHRINKING, CT::Container C>
      void TransferGlobalHeapRequests(this C& self, CT::Container auto const& oldSelf, const size_t oldReserved, const size_t newReserved) {
         size_t from[C::template CountHeapFooterRequests<Id::First>() + 1];
         size_t to  [C::template CountHeapFooterRequests<Id::First>() + 1];
         size_t idx = 1;
         from[0] = to[0] = 0;
         
         ForEach(typename C::ComponentList{}, [&]<class COM> {
            if constexpr (requires { typename COM::HeapRequest; }) {
               using R = typename COM::HeapRequest;
               if constexpr (IsGlobalFooterRequest<R>) { //TODO maybe check for intersection?
                  size_t shift = sizeof(TypeOf<R>);

                  if constexpr (R::AllocatedPerElement) {
                     from[idx] = from[idx-1] + shift * oldReserved;
                     to  [idx] = to  [idx-1] + shift * newReserved;
                  }
                  else {
                     from[idx] = from[idx-1] + shift;
                     to  [idx] = to  [idx-1] + shift;
                  }

                  ++idx;
               }
            }            
         });

         // Calculate the new destination                               
         const auto to_footer = self.GetGlobalHeap(newReserved);
         const auto from_footer = oldSelf.GetGlobalHeap(oldReserved);

         --idx;

         for (size_t i = 0; i < idx; ++i) {
            const auto src_range = from[i + 1] - from[i];
            LglsAssumeDev(src_range,
               "Empty ranges should've been omitted in the previous loop");

            memcpy(to_footer + to[i], from_footer + from[i], src_range);

            if constexpr (not SHRINKING) {
               // Fill gaps with zeroes                                 
               const auto dst_range = to[i + 1] - to[i];
               memset (to_footer + to[i] + src_range, 0, dst_range - src_range);
            }
         }
      }

      /// MARK: TransferLocalHeapRequests                                     
      /// Transfer footer requests onto the new reserve and heap              
      ///   @param newReserved the newly reserved number of elements          
      ///   @attention works on one dimension at a time!                      
      //TODO shouldn't this also move any dimensions != 0 as well, if in the same heap?????
      //TODO this will nullify a new hash table, but it doesn't call SetHashTableInner to move the pointer if kept on the stack!! 
      //TODO i've worked around this by using IndexedHashHeap instead of IndexedHashStack for sets and maps, for now
      template<Cid SID, bool SHRINKING, CT::Container C>
      void TransferLocalHeapRequests(this C& self, CT::Container auto const& oldSelf, const size_t oldReserved, const size_t newReserved) {
         [[maybe_unused]]
         const auto indirect = self.template GetIndirections<SID>();

         size_t from[C::template CountHeapFooterRequests<SID>() + 1];
         size_t to  [C::template CountHeapFooterRequests<SID>() + 1];
         size_t idx = 1;
         from[0] = to[0] = 0;
         
         ForEach(typename C::ComponentList{}, [&]<class COM> {
            if constexpr (requires { typename COM::HeapRequest; }) {
               using R = typename COM::HeapRequest;
               if constexpr (IsLocalFooterRequest<R> and COM::Id::template Contains<SID>) {
                  size_t shift = sizeof(TypeOf<R>);
                  if constexpr (R::AllocatedPerIndirection) {
                     shift *= indirect;
                     if (not shift)
                        return;
                  }

                  if constexpr (R::AllocatedPerElement) {
                     from[idx] = from[idx-1] + shift * oldReserved;
                     to  [idx] = to  [idx-1] + shift * newReserved;
                  }
                  else {
                     from[idx] = from[idx-1] + shift;
                     to  [idx] = to  [idx-1] + shift;
                  }

                  ++idx;
               }
            }            
         });

         // Calculate the new destination                               
         const auto to_footer = self.template GetLocalHeap<SID>(newReserved);
         const auto from_footer = oldSelf.template GetLocalHeap<SID>(oldReserved);

         --idx;
   
         for (size_t i = 0; i < idx; ++i) {
            const auto src_range = from[i + 1] - from[i];
            LglsAssumeDev(src_range,
               "Empty ranges should've been omitted in the previous loop");

            memmove(to_footer + to[i], from_footer + from[i], src_range);

            if constexpr (not SHRINKING) {
               // Fill gaps with zeroes                                 
               const auto dst_range = to[i + 1] - to[i];
               memset (to_footer + to[i] + src_range, 0, dst_range - src_range);
            }
         }
      }

      /// MARK: RemapAllHeapRequests                                          
      /// Move all bits and pieces of footer requests that need to move when  
      /// reserved count increases or decreases.                              
      ///   @attention works on all dimensions at once                        
      ///   @attention changes the reserved count (if changeable)             
      template<CT::Container C>
      void RemapAllHeapRequests(this C& self, const size_t newReserved) {
         if constexpr (C::template CountHeapFooterRequests<Id::First>() > 0) {
            const auto oldReserved = self.template GetReserved<Id::First>();
            LglsAssumeDev(newReserved != oldReserved,
               "Should be called only when different");

            if (newReserved > oldReserved) {
               // When newReserved is larger than reserved, stuff has to
               // move left to right, so it must be done in reverse so  
               // that we don't destroy any data. The newly formed gaps 
               // need to be filled with zeroes.                        
               self.template RemapGlobalHeapRequests<false>(oldReserved, newReserved);
               Id::ForEach([&]<Cid D> {
                  self.template RemapLocalHeapRequests<D, false>(oldReserved, newReserved);
               });
            }
            else {
               Id::ForEach([&]<Cid D> {
                  self.template RemapLocalHeapRequests<D, true>(oldReserved, newReserved);
               });
               self.template RemapGlobalHeapRequests<true>(oldReserved, newReserved);
            }
         }

         if_available(self.template SetReservedInner<Id::First>(newReserved));
      }

      /// MARK: TransferAllHeapRequests                                       
      /// Move all bits and pieces of footer requests that need to move when  
      /// reserved count increases or decreases, after memory moves.          
      ///   @attention works on all dimensions at once                        
      ///   @attention changes the reserved count (if changeable)             
      template<CT::Container C>
      void TransferAllHeapRequests(this C& self, CT::Container auto const& oldSelf, const size_t newReserved) {
         if constexpr (C::template CountHeapFooterRequests<Id::First>() > 0) {
            const auto oldReserved = self.template GetReserved<Id::First>();
            LglsAssumeDev(newReserved != oldReserved,
               "Should be called only when different");
            LglsAssumeDev(self.GetHeapInner() != oldSelf.GetHeapInner(),
               "Should be called only when heaps differ");

            if (newReserved > oldReserved) {
               // When newReserved is larger than reserved, stuff has to
               // move left to right, so it must be done in reverse so  
               // that we don't destroy any data. The newly formed gaps 
               // need to be filled with zeroes.                        
               self.template TransferGlobalHeapRequests<false>(oldSelf, oldReserved, newReserved);
               Id::ForEach([&]<Cid D> {
                  self.template TransferLocalHeapRequests<D, false>(oldSelf, oldReserved, newReserved);
               });
            }
            else {
               Id::ForEach([&]<Cid D> {
                  self.template TransferLocalHeapRequests<D, true>(oldSelf, oldReserved, newReserved);
               });
               self.template TransferGlobalHeapRequests<true>(oldSelf, oldReserved, newReserved);
            }
         }

         if_available(self.template SetReservedInner<Id::First>(newReserved));
      }

      /// MARK: PartialSuccess                                                
      /// Invoked to remedy the situation when element constructors throw     
      ///   @param n the number of elements that were actually initialized    
      template<Cid SID = Id::First, CT::Container C> //requires Relevant<SID>
      void PartialSuccess(this C& self, Count<C> n) {
         if constexpr (requires { self.template SetCountInner<SID>(1); }) {
            // Partial success is supported                             
            self.template SetCountInner<SID>(n);
            self.template ResetHash<SID>();

            if constexpr (not CT::Contiguous<C>) {
               // Partial success involving maps/sets might result in   
               // gaps in the hash table that need to be accounted for. 
               self.template ShiftEntries<SID>();
            }
         }
         else {
            // Partial success is not allowed - we have to deallocate   
            // and make sure CountStatic reports as empty.              
            (void) n;
            Allocator::Deallocate(DecvqAllCast(self.template GetAllocationInner<SID>()));
            self.template ResetAllocationInner<SID>();
         }
      }

      /// MARK: BranchOut                                                     
      /// Branch out the current container by doing a shallow copy.           
      /// Happens when you try to modify a container with strong ownership    
      /// from somewhere else (when GetUses() > 1), or when container is      
      /// disowned. Allocates a fresh allocation in the case we haven't       
      /// allocated anything yet.                                             
      ///   @param newReserve usually branching is accompanied by a resize,   
      ///      so specify it here                                             
      template<Cid SID = Id::First, CT::Container C> //requires Relevant<SID>
      void BranchOut(this C& self, Count<C> newReserve) {
         if (not self.IsDisowned() and self.template GetUses<SID>() == 1) {
            // No need to branch out - reuse the current allocation     
            // unless container was disowned                            
            ThisCom::AllocateMore(newReserve);
            return;
         }

         if (self.template IsEmpty<SID>()) {
            // Empty - do a fresh allocation                            
            ThisCom::AllocateFresh(newReserve);
            return;
         }
         else {
            // Branch out by performing a shallow clone                 
            const C backup {Abandon{self}};
            if_available(self.DisableDisowned()); //TODO redundant?
            ThisCom::ConstructFrom(Copy(backup), newReserve);
         }
      }

      /// MARK: CopyOrCloneAllItemsOneByOne                                   
      /// Transfer all items one by one, account for exceptions               
      ///   @attention works on all relevant dimensions at once!              
      ///   @attention can be used only for Copy/Clone intents!               
      template<bool CLONE, CT::Container C>
      void CopyOrCloneAllItemsOneByOne(this C& self, auto const& from) {
         auto dst = self.GetHandle().ForceMutable();
         try {
            from.template Apply<false>([&dst,&self,&from](auto const& src) {
               (void) self; (void) from;

               if constexpr (CT::Supported<decltype(src)>) {
                  Id::ForEach([&dst,&src]<Cid D>{
                     if constexpr (CLONE)
                        dst.template EmplaceWithIntent<D>(Clone(src));
                     else
                        dst.template EmplaceWithIntent<D>(Refer(src));
                  });

                  if constexpr (not CT::Contiguous<C>) {
                     // Copy hash table entry as well                   
                     const auto idx = dst - self.GetHandle();
                     self.template GetHashTableInner<Id::First>()[idx]
                        = from.template GetHashTableInner<Id::First>()[idx];
                  }
               }
               ++dst;
            });
         }
         catch (...) {
            //TODO what if it throws while all dimensions except the first one are uninitialized??
            //TODO badly designed!
            // Partial success                                          
            auto n = dst - self.GetHandle();
            if constexpr (not requires { self.template SetCountInner<Id::First>(1); }) {
               // Partial success is not allowed - we have to           
               // destroy everything we initialized                     
               while (n) {
                  dst.Free();
                  --dst;
                  --n;
               }
            }
            ThisCom::PartialSuccess(n);
            throw;
         }
      }

      /// MARK: CloneAllItemsBatched                                          
      /// Transfer all items one by one, account for exceptions               
      ///   @attention works on all relevant dimensions at once!              
      ///   @attention can be used only for Copy/Clone intents!               
      ///   @attention source and destination should not overlap!             
      ///   @attention assumes 'self' has been reserved                       
      template<CT::Container C1, CT::Container C2>
      void CloneAllItemsBatched(this C1& self, C2 const& from) {
         Id::ForEach([&]<Cid D> {
            auto dst = self.template GetSlice<D>().ForceMutable();
            auto src = from.template GetSlice<D>();
            if constexpr (CT::Contiguous<C1>)
               memcpy(dst.GetRaw(), src.GetRaw(), from.GetBytesize());
            else
               memcpy(dst.GetRaw(), src.GetRaw(), from.GetStride() * from.GetReserved());
         });

         //TODO just copy these along with elements in the above Id::ForEach if oldReserved and newReserved are the same
         if constexpr (C1::template CountHeapFooterRequests<Id::First>() > 0
         and           C2::template CountHeapFooterRequests<Id::First>() > 0) {
            const size_t oldReserved = from.GetReserved();
            const size_t newReserved = self.GetReserved();
            Id::ForEach([&]<Cid D> {
               self.template TransferLocalHeapRequests<D, false>(from, oldReserved, newReserved);
            });
            self.template TransferGlobalHeapRequests<false>(from, oldReserved, newReserved);
         }
      }
   };

   #undef ThisCom
}
