///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Component.hpp"
#include <Langulus/CT/Index.hpp>


namespace Langulus::Annies::Component
{
   /// Refers back to this particular component instance through the deduced  
   /// 'this'. Just for convenience. It is #undef-ed at the end of this file. 
   #define ThisCom self.CountStack<T, ID, SHARED...>

   ///                                                                        
   /// Tracks count on the stack.                                             
   /// Count shows how many elements inside a container are initialized.      
   /// Stack-based counting increases the container size, but doesn't require 
   /// indirections, making count lookup faster and more cache-friendly.      
   ///   @tparam T the count type                                             
   ///   @tparam ID provider ID to keep count of                              
   ///   @tparam SHARED provider IDs that share the same count variable       
   template<class T, Cid ID, Cid...SHARED>
   struct CountStack {
      using CTTI_Component = Yup;
      using CTTI_ReflectAs = void;
      using Id = Values<ID, SHARED...>;

      using CountType    = T;
      using IndexType    = Index::At<T>;
      using StackRequest = T;
      using Dimensions   = Id;

      static constexpr int  ComponentPrecedence = -1000;
      static constexpr bool ContainsMany = true;
      //template<Cid SID>
      //static constexpr bool Relevant = Id::template Contains<SID>;

      static_assert(CT::Integer<T> and not CT::Signed<T>,
         "Count type must be an unsigned integer");

      /// MARK: Public                                                        
      /// Check if there are no initialized elements                          
      template<Cid SID = ID> //requires Relevant<SID>
      constexpr bool IsEmpty(this auto const& self) noexcept {
         return ThisCom::GetCountInner() == 0;
      }

      /// Get the number of initialized elements                              
      template<Cid SID = ID> //requires Relevant<SID>
      constexpr T GetCount(this auto const& self) noexcept {
         return ThisCom::GetCountInner();
      }

      /// Explicit boolean conversion to allow using containers in ifs        
      explicit constexpr operator bool(this auto const& self) noexcept {
         return ThisCom::GetCountInner() != 0;
      }

      T GetCountDeep() const noexcept;
      T GetCountItemsDeep() const noexcept;

   protected:
      /// MARK: Protected                                                     
      LglsComRemoval(friend);
      LglsComEmplacement(friend);
      LglsComInsertion(friend);
      LglsComMerging(friend);
      LglsComIndexedCommon(friend);
      LglsComIndexedLinear(friend);
      LglsComHeapMovable(friend);
      LglsComConversion(friend);

      /// Get count (inner)                                                   
      template<Cid SID = ID> //requires Relevant<SID>
      constexpr auto& GetCountInner(this auto&& self) noexcept {
         return self.template AccessStack<CountStack>();
      }
      
      /// Set the number of initialized elements                              
      template<Cid SID = ID> //requires Relevant<SID>
      constexpr void SetCountInner(this auto& self, T c) noexcept {
         ThisCom::GetCountInner() = c;
      }
      
      /// Default-initialize count to zero                                    
      constexpr void ConstructDefault(this auto& self) noexcept {
         ThisCom::SetCountInner(0);
      }
      
      /// Transfer from any kind of container, respecting intents             
      ///   @attention this is noop when constructing from deep intents,      
      ///      since element constructors might throw and stuff be partially  
      ///      inserted. In those cases, count is set by the heap components. 
      ///   @param intent the intent and container to transfer from           
      template<class SELF, CT::Intent I> requires CT::Container<I>
      void ConstructFrom(this SELF& self, I&& intent) {
         if constexpr (not CT::Copied<I> and not CT::Cloned<I> and CT::HeapAllocated<I>) {
            using IT = Decvq<Deref<Deint<I>>>;
            decltype(auto) from = LglsFwd(intent.what);
            ThisCom::SetCountInner(from.template GetCount<ID>());

            // Count is responsible for deep ownership, it has to       
            // be reset on move to disable destruction in 'from'.       
            if constexpr (CT::Moved<I>) {
               // Moving                                                
               if_available(from.template SetCountInner<ID>(0));
            }
            else if constexpr (CT::Abandoned<I> and CT::OwnedDeepStrong<I>) {
               // Abandoning                                            
               if constexpr (not IT::CanBeDisowned) {
                  // In case source is not disownable, this component   
                  // must make sure to reset relevant properties.       
                  if_available(from.template SetCountInner<ID>(0));
               }
            }
         }
      }

      /// Reset count (inner)                                                 
      ///   @attention doesn't destroy elements, only resets hash and count   
      template<Cid SID = ID> //requires Relevant<SID>
      constexpr void ResetCount(this auto& self) noexcept {
         ThisCom::SetCountInner(0);
         if_available(self.template SetHashInner<SID>(1));
      }
   };

   #undef ThisCom
}
