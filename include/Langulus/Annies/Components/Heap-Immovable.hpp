///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Heap-Movable.hpp"


namespace Langulus::Annies::Component
{
   ///                                                                        
   /// Interfaces a heap. Adds a member that points to the heap memory.       
   /// The heap is not allowed to move on reallocation.                       
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
   ///   @attention this component will allocate an additional silent byte    
   ///      if container is CT::NullTerminated.                               
   template<unsigned INITIAL_SIZE, unsigned GROWTH_FACTOR, CT::HeapEntry ENTRY0, CT::HeapEntry...ENTRYN>
   struct HeapImmovable : HeapMovable<INITIAL_SIZE, GROWTH_FACTOR, ENTRY0, ENTRYN...> {
      /// Refers back to this particular component instance through the       
      /// deduced 'this'. Just for convenience. It is #undef-ed at the end of 
      /// this file.                                                          
      #define ThisCom self.HeapImmovable<INITIAL_SIZE, GROWTH_FACTOR, ENTRY0, ENTRYN...>
      using Base         = HeapMovable<INITIAL_SIZE, GROWTH_FACTOR, ENTRY0, ENTRYN...>;
      using Id           = typename Base::Id;
      using HeapProvider = typename Base::HeapProvider;

      static constexpr unsigned InitialSize   = INITIAL_SIZE;
      static constexpr unsigned GrowthFactor  = GROWTH_FACTOR;
      static constexpr bool     Reallocatable = true;

   protected:
      LglsComIterationOperators(friend);
      LglsComReserveEmergent(friend);
      LglsComInsertion(friend);
      LglsComMerging(friend);
      LglsComAssignment(friend);
      LglsComEmplacement(friend);
      LglsComConversion(friend);
      LglsComOwnershipEmergent(friend);


      /// MARK: AllocateMore                                                  
      /// Allocate a number of elements, relying on the type of the container 
      ///   @attention assumes container is typed                             
      ///   @attention works on all relevant dimensions at once               
      ///   @attention may allocate an additional element if the container    
      ///      is CT::NullTerminated                                          
      ///   @param elements number of elements to allocate                    
      template<Cid SID = Id::First, CT::Container C>
      void AllocateMore(this C& self, size_t elements) {
         static_assert(not CT::Handle<C>, "Handles aren't allowed to reallocate");
         LglsAssumeDev(elements > self.GetCount(), "Bad element count");
         const auto al = DecvqAllCast(self.template GetAllocation<SID>());

         if (not al) {
            //                                                          
            // Allocate a fresh set of elements                         
            ThisCom::AllocateFresh(elements);
            return;
         }

         /*LglsAssumeDev(al->GetUses() == 1,
            "Can't reuse memory of a heap used from multiple places. "
            "Container should've branched-out prior to AllocateMore. "
         );*/

         const auto request = ThisCom::RequestHeap(elements + size_t(CT::NullTerminated<C>));
         if constexpr (CT::ContainsMany<C>) {
            if (self.GetReserved() >= request.mReserved)
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

            TODO(); //TODO now allowed to reallocate, instead allocate a new block and attach it to an imaginary data[mReserved] element
            #if LANGULUS_FEATURE(MANAGED_MEMORY)
               auto reallocated = Allocator::Reallocate(self.template GetType<SID>(), request.mTotalBytes, al);
            #else
               auto reallocated = Allocator::Reallocate(request.mTotalBytes, al);
            #endif
            LglsAssert(reallocated, "Out of memory");

            self.template SetAllocationInner<SID>(reallocated);

            // Memory never moves, make sure we don't free anything     
            LglsAssumeDev(self.template GetAllocationInner<SID>() == reallocated,
               "Allocation pointer should still be the same");

            if_available(previous.EnableDisowned())
            else previous.SetCountInner(0);
            self.RemapAllHeapRequests(request.mReserved);
         }
      }

      /// MARK: AllocateLess                                                  
      /// Shrink the block, depending on currently reserved	elements.         
      /// Initialized elements on the back will be destroyed.                 
      /// When MANAGED_MEMORY is enabled we have a strong guarantee that      
      /// allocations never move when shrinking.                              
      ///   @attention works on all relevant dimensions at once               
      ///   @attention designed for linearly allocated containers only        
      ///   @param desiredReserve number of elements to reserve               
      template<Cid SID = Id::First, CT::IndexedLinearly C>
      void AllocateLess(this C& self, size_t const desiredReserve) {
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
            ThisCom::AllocateFresh(desiredReserve);

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
               self.SetCountInner(to - self.GetHandle());
               throw;
            }

            if constexpr (CT::Contiguous<C>) {
               self.SetCountInner(backup.GetCount() < desiredReserve
                  ? backup.GetCount()
                  : desiredReserve
               );
            }
            else self.SetCountInner(backup.GetCount());
            return;
         }

         LglsAssumeDev(desiredReserve <= self.GetReserved(),
            "Can't shrink allocation using more elements");
         LglsAssumeDev(al->GetUses() == 1,
            "Can't reuse memory of a block used from multiple places");

         if (self.GetCount() > desiredReserve) {
            auto temp = self.SelectInner(desiredReserve);
            temp.template Free<false>();
            if_available(self.SetCountInner(desiredReserve));
         }

         const auto request = ThisCom::RequestHeap(desiredReserve + size_t(CT::NullTerminated<C>));
         if (self.GetReserved() == request.mReserved)
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

      /// MARK: BranchOut                                                     
      /// Branch out the current container by doing a shallow copy.           
      /// Happens when you try to modify a container with strong ownership    
      /// from somewhere else (when GetUses() > 1), or when container is      
      /// disowned. Allocates a fresh allocation in the case we haven't       
      /// allocated anything yet.                                             
      ///   @param newReserve usually branching is accompanied by a resize,   
      ///      so specify it here                                             
      template<Cid SID = Id::First, CT::Container C>
      void BranchOut(this C& self, size_t newReserve) {
         TODO(); //TODO hives shouldn't branch out, right? that's their whole point.
         if (not self.IsDisowned() and self.template GetUses<SID>() == 1) {
            // No need to branch out - reuse the current allocation     
            // unless container was disowned                            
            ThisCom::AllocateMore(newReserve);
            return;
         }

         if (self.IsEmpty()) {
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

      /// Get a handle from a pointer to an element                           
      template<CT::Container C>
      auto GetHandleFromPointer(this C&& self, CT::Sparse auto ptr) -> DecideHandle<C> {
         const auto memory_start = ThisCom::GetRaw();
         if (ptr < memory_start)
            return {};

         //TODO ptr may be in one of the cascades, this is immovable heap after all
         const auto memory_end = ThisCom::GetRawReserveEnd();
         if (ptr >= memory_end)
            return {};

         size_t offset;
         if constexpr (CT::TypeErased<C>) {
            offset = reinterpret_cast<uint8_t const*>(ptr)
                   - static_cast<uint8_t const*>(memory_start);
            offset /= self.GetStride();
         }
         else offset = ptr - memory_start;

         return self.GetHandle() + offset;
      }
   };

   #undef ThisCom
}
