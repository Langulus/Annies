///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "OwnershipDeep-Emergent.hpp"
#include <Langulus/CT/Index.hpp>


namespace Langulus::Annies::Component
{
   /// Refers back to this particular component instance through the deduced  
   /// 'this'. Just for convenience. It is #undef-ed at the end of this file. 
   #define ThisCom self.OwnershipDeepReference<STYLE, REF_INDIVIDUAL, ID, SHARED...>

   ///                                                                        
   /// The pointer to the array of allocations for each element and           
   /// indirection is kept locally. Useful to carry allocation data inside    
   /// handles.                                                               
   ///   @tparam STYLE whether ownership will be automatically applied on     
   ///      construction, reassignment and destruction. Usually 0 if container
   ///      is just a view, or in other cases where you want to carry an      
   ///      allocation pointer, but not necessarily reference it.             
   ///   @tparam REF_INDIVIDUAL toggles whether contained items that were     
   ///      reflected as CT::Referenced get referenced. Elements will get     
   ///      referenced even if no entry for the element exist, but you can    
   ///      avoid referencing altogether if you use the Disown intent.        
   ///      To be more specific - when GetAllocation() is nullptr and the     
   ///      entire container is considered disowned.                          
   ///   @tparam ID which heap/stack are we keeping track of?                 
   ///   @tparam SHARED additional provider IDs that share the same behavior  
   template<unsigned STYLE, bool REF_INDIVIDUAL, Cid ID, Cid...SHARED>
   struct OwnershipDeepReference : OwnershipDeepEmergent<STYLE, REF_INDIVIDUAL, ID, SHARED...> {
      using StackRequest = EntryPtr;
      using Id = typename OwnershipDeepEmergent<STYLE, REF_INDIVIDUAL, ID, SHARED...>::Id;

      //template<Cid SID>
      //static constexpr bool Relevant = Id::template Contains<SID>;
      static constexpr bool Shared   = sizeof...(SHARED) > 0;

      /// MARK: Public                                                        
      /// Get entry array if containing pointers                              
      ///   @attention may contain invalid data for discontiguous containers  
      ///   @return the array of entries                                      
      template<Cid SID = ID>// requires Relevant<SID>
      auto GetEntries(this auto const& self) assumptious
      -> Allocation const* const* {
         if (self.template IsSparse<SID>())
            return ThisCom::GetEntriesInner();
         return nullptr;
      }

      /// Get entry array for all indirections of a specific element          
      ///   @return the array of entries                                      
      template<Cid SID = ID, CT::Container C> requires (/*Relevant<SID> and*/ CT::Indexed<C>)
      auto GetEntriesAt(this C const& self, CT::Index auto&& idx) assumptious
      -> Allocation const* const* {
         if (self.template IsSparse<SID>()) {
            LglsAssumeDev(self.template GetRaw<SID>(), "No memory available");
            return ThisCom::GetEntriesInner() + self.template SimplifyIndex<SID>(LglsFwd(idx));
         }
         return nullptr;
      }

      auto GetKeyEntries(this auto&& self) assumptious -> AllocationPtr const* requires Shared {
         return ThisCom::template GetEntries<Id::First>();
      }
      auto GetValEntries(this auto&& self) assumptious -> AllocationPtr const* requires Shared {
         return ThisCom::template GetEntries<Id::Second>();
      }

      auto GetKeyEntriesAt(this auto&& self, CT::Index auto&& idx) assumptious requires Shared {
         return ThisCom::template GetEntriesAt<Id::First>(LglsFwd(idx));
      }
      auto GetValEntriesAt(this auto&& self, CT::Index auto&& idx) assumptious requires Shared {
         return ThisCom::template GetEntriesAt<Id::Second>(LglsFwd(idx));
      }

   protected:
      /// MARK: Protected                                                     
      LglsComOwnershipDeepEmergent(friend);
      LglsComEmplacement(friend);
      LglsComIterationOperators(friend);

      /// Get the entry array (inner, unsafe)                                 
      template<Cid SID = ID>// requires Relevant<SID>
      constexpr auto& GetEntriesInner(this auto&& self) noexcept {
         return self.template AccessStack<OwnershipDeepReference>();
      }

      /// Set the entry array (inner)                                         
      constexpr void SetEntriesInner(this auto& self, EntryPtr entries) noexcept {
         ThisCom::GetEntriesInner() = DecvqAllCast(entries);
      }

      /// Default-initialization of this component                            
      void ConstructDefault(this auto& self) noexcept {
         ThisCom::SetEntriesInner(nullptr);
      }

      /// Transfer from any kind of container, respecting intents.            
      /// Do it for a particular dimension.                                   
      ///   @param intent The intent and container to transfer from.          
      template<Cid D, class SELF, CT::Intent I> requires CT::Container<I>
      void SliceFrom(this SELF& self, I&& intent) {
         static_assert(CT::Disowned<I>);
         if constexpr (requires { intent->template GetEntries<D>(); })
            ThisCom::SetEntriesInner(intent->template GetEntries<D>());
         else
            ThisCom::SetEntriesInner(nullptr);
      }

      /// Copy the pointer to the entries, and reference if we have to        
      ///   @param intent The intent and container to transfer from.          
      template<class SELF, CT::Intent I>
      requires (CT::Container<I> and (CT::TypeErased<Deint<I>> or CT::Sparse<TypeOf<Deint<I>, ID>>))
      void ConstructFrom(this SELF& self, I&& intent) noexcept {
         using IT = Decvq<Deref<Deint<I>>>;
         decltype(auto) from = LglsFwd(intent.what);

         if constexpr (not requires { from.template GetEntries<ID>(); })
            ThisCom::SetEntriesInner(nullptr);
         else 
            ThisCom::SetEntriesInner(from.template GetEntries<ID>()); 

         if constexpr ((STYLE & OnCreateAndDestroy) != 0) {
            if constexpr (CT::Referred<I> or (IT::OwnedDeep & OnCreateAndDestroy) == 0) {
               // Refer                                                 
               ThisCom::Keep();
            }
            else if constexpr (CT::Moved<I> or CT::Abandoned<I>) {
               // Move/Abandon                                          
               if (from.IsDisowned()) {
                  // Right was never owned, now we own it               
                  ThisCom::Keep();
               }
               else if constexpr (CT::Moved<I> or not IT::CanBeDisowned) {
                  // Transfer ownership if we can, otherwise refer      
                  // Deep ownership can be reset in two ways: either    
                  // reset the entries pointer, or reset the count.     
                  if constexpr (CT::HasVariableCount<I>) {
                     LglsAssumeDev(from.IsEmpty(),
                        "Remote count should've been reset prior to this call");
                  }
                  else if_available(from.template SetEntriesInner<ID>(nullptr))
                  else ThisCom::Keep();
               }
            }
         }
      }
   };

   #undef ThisCom
}
