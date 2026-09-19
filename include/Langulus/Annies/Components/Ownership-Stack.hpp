///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Ownership-Emergent.hpp"


namespace Langulus::Annies::Component
{
   /// Refers back to this particular component instance through the deduced  
   /// 'this'. Just for convenience. It is #undef-ed at the end of this file. 
   #define ThisCom self.OwnershipStack<STYLE, ID, SHARED...>

   ///                                                                        
   /// Keep a pointer to the heap allocation as a member.                     
   /// Manage its ownership by referencing and dereferencing it.              
   ///   @tparam STYLE whether ownership will be automatically applied on     
   ///      construction, reassignment and destruction. Usually 0 if container
   ///      is just a view, or in other cases where you want to carry an      
   ///      allocation pointer, but not necessarily reference it.             
   ///   @tparam ID provider we're keeping track of                           
   ///   @tparam SHARED other providers that will share the same allocation   
   ///      variable.                                                         
   template<unsigned STYLE, Cid ID, Cid...SHARED>
   struct OwnershipStack : OwnershipEmergent<STYLE, ID, SHARED...> {
      using StackRequest = AllocationPtr;
      using Id = typename OwnershipEmergent<STYLE, ID, SHARED...>::Id;

      //template<Cid SID>
      //static constexpr bool Relevant = Id::template Contains<SID>;

      /// Get the allocation                                                  
      template<Cid SID = ID> //requires Relevant<SID>
      constexpr auto GetAllocation(this auto const& self) noexcept {
         return ThisCom::GetAllocationInner();
      }

      /// Shallow-copy all initialized elements in memory to another          
      /// allocation, that is owned once only by this container.              
      ///   @attention if we already own the memory just Keep() it once       
      template<Cid SID = ID, CT::Container C>
      requires (CT::HeapAllocated<C> /*and Relevant<SID>*/)
      void TakeOwnership(this C& self) {
         auto rawData = self.template GetRaw<SID>();
         if (not rawData)
            return;

         auto& a = ThisCom::GetAllocationInner();
         if (a)
            return; // We already own this allocation                   

      #if LANGULUS_FEATURE(MANAGED_MEMORY)
         // The heap might already be ours and we just don't know it    
         if (auto found = Allocator::Find(rawData)) {
            a = found;
            DecvqAllCast(a)->AddRef(1);
            return;
         }
      #endif

         // Shallow-copy all elements in a fresh allocation             
         C temp {Copy {self}};
         self = Abandon {temp};
      }

   protected:
      LglsComHeapReference(friend);
      LglsComHeapMovable(friend);
      LglsComRemoval(friend);
      LglsComEmplacement(friend);
      LglsComReserveStatic(friend);

      /// Get allocation (inner)                                              
      ///   @attention may be uninitialized                                   
      template<Cid SID = ID> //requires Relevant<SID>
      constexpr auto& GetAllocationInner(this auto&& self) noexcept {
         return self.template AccessStack<OwnershipStack>();
      }
      
      /// Set allocation (inner)                                              
      ///   @attention this will not dereference previous allocation          
      template<Cid SID = ID> //requires Relevant<SID>
      constexpr void SetAllocationInner(this auto& self, Allocation const* a) noexcept {
         ThisCom::GetAllocationInner() = const_cast<Allocation*>(a);
      }

      /// Automatically set the allocation by searching for it using the heap 
      /// pointer. If allocation wasn't found, it will be set to nullptr.     
      ///   @attention this will not dereference previous allocation          
   #if LANGULUS_FEATURE(MANAGED_MEMORY)
      template<Cid SID = ID> //requires Relevant<SID>
      void FindAllocationInner(this auto& self) noexcept {
         const auto heap = self.template GetRaw<SID>();
         if (not heap) {
            ThisCom::SetAllocationInner(nullptr);
            return;
         }

         if (auto found = Allocator::Find(heap)) {
            ThisCom::SetAllocationInner(found);
            if constexpr (STYLE & OnCreateAndDestroy)
               ThisCom::Keep();
         }
         else ThisCom::SetAllocationInner(nullptr);
      }
   #else
      template<Cid SID = ID> //requires Relevant<SID>
      void FindAllocationInner(this auto& self) noexcept {
         ThisCom::SetAllocationInner(nullptr);
      }
   #endif

      /// Resets allocation and all of its derivatives                        
      template<Cid SID = ID> //requires Relevant<SID>
      constexpr void ResetAllocationInner(this auto&& self) noexcept {
         ThisCom::SetAllocationInner(nullptr);
         if_available(self.template SetReservedInner<SID>(0));
         if_available(self.template SetHashTableInner<SID>(nullptr));
         self.template ResetCount<SID>();
      }

      /// Same as ResetAllocationInner, but here in case container lacks it   
      constexpr void ResetAllAllocations(this auto&& self) noexcept {
         ThisCom::ResetAllocationInner();
      }

      /// Default-initialize the component                                    
      ///   @attention this will not dereference previous allocation          
      constexpr void ConstructDefault(this auto& self) noexcept {
         ThisCom::SetAllocationInner(nullptr);
      }

      /// Transfer from any kind of container, respecting intents.            
      /// Do it for a particular dimension.                                   
      ///   @param intent The intent and container to transfer from.          
      template<Cid D, class SELF, CT::Intent I> requires CT::Container<I>
      void SliceFrom(this SELF& self, I&& intent) {
         static_assert(CT::Disowned<I>);
         ThisCom::SetAllocationInner(intent->template GetAllocation<D>());
      }

      /// Transfer from any kind of container, respecting intents             
      ///   @param intent the intent and container to transfer from           
      ///   @important notice that Copy and Clone intents are not handled     
      ///      here. They're handled in heap components instead, in case      
      ///      something throws an exception while constructing.              
      template<class SELF, CT::Intent I> requires CT::Container<I>
      void ConstructFrom(this SELF& self, I&& intent) {
         if constexpr (not CT::Copied<I> and not CT::Cloned<I> and CT::HeapAllocated<I>) {
            using IT = Decvq<Deref<Deint<I>>>;
            decltype(auto) from = LglsFwd(intent.what);

            if constexpr (CT::Disowned<I>) {
               // Disown                                                
               if constexpr (not SELF::CanBeDisowned and STYLE & OnCreateAndDestroy) {
                  // This container can't be marked as disowned - we    
                  // must reset ownership unless it's weak.             
                  ThisCom::SetAllocationInner(nullptr);
               }
               else {
                  // We are allowed to propagate the allocation pointer,
                  // but don't bother resetting or searching!           
                  // If it's there, it's there...                       
                  if_available(ThisCom::SetAllocationInner(from.template GetAllocationInner<ID>()))
                  else ThisCom::SetAllocationInner(nullptr);
               }
            }
            /*else if constexpr (not requires { IT::Owned; }) { //TODO this should be done only if no GetAllocationInner or GetAllocation is available
               // No ownership on the right side - we must search the   
               // memory allocation ourselves.                          
               ThisCom::FindAllocationInner();
            }*/
            else if constexpr (CT::Referred<I>) {
               // Refer                                                 
               if constexpr (requires { from.template GetAllocationInner<ID>(); }) {
                  const auto al = from.template GetAllocationInner<ID>();
                  if (al) {
                     ThisCom::SetAllocationInner(al);
                     
                     if constexpr (STYLE & OnCreateAndDestroy)
                        ThisCom::Keep();
                  }
                  else ThisCom::FindAllocationInner();
               }
               else ThisCom::FindAllocationInner();
            }
            else if constexpr (CT::Moved<I> or CT::Abandoned<I>) {
               // Abandon/Move                                          
               if constexpr (requires { from.template GetAllocationInner<ID>(); }) {
                  const auto al = from.template GetAllocationInner<ID>();
                  if (al) {
                     ThisCom::SetAllocationInner(al);

                     if (STYLE & OnCreateAndDestroy and from.IsDisowned()) {
                        // Source was disowned, we now own it           
                        ThisCom::Keep();
                     }

                     if constexpr (CT::Moved<I> or not IT::CanBeDisowned) {
                        // Transfer ownership in order to disown        
                        from.template SetAllocationInner<ID>(nullptr);
                     }
                  }
                  else ThisCom::FindAllocationInner();
               }
               else ThisCom::FindAllocationInner();
            }
         }
      }
   };

   #undef ThisCom
}
