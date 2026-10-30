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
#include <Langulus/CT/Signed.hpp>


namespace Langulus::Annies::Component
{
   ///                                                                        
   /// Tracks count on the stack.                                             
   /// Count shows how many elements inside a container are initialized.      
   /// Stack-based counting increases the container size, but doesn't require 
   /// indirections, making count lookup faster and more cache-friendly.      
   ///   @tparam T the count type                                             
   ///   @attention only one count component is allowed per container         
   template<class T>
   struct CountStack {
      /// Refers back to this particular component instance through the       
      /// deduced 'this'. Just for convenience. It is #undef-ed at the end of 
      /// this file.                                                          
      #define ThisCom self.CountStack<T>

      using CTTI_Component = Yup;
      using CTTI_ReflectAs = void;

      using CountType    = T;
      using StackRequest = T;

      static constexpr int  ComponentPrecedence = -1000;
      static constexpr bool ContainsMany = true;

      static_assert(CT::Integer<T> and not CT::Signed<T>,
         "Count type must be an unsigned integer");

      /// MARK: Public                                                        
      /// Check if there are no initialized elements                          
      constexpr bool IsEmpty(this auto const& self) noexcept {
         return ThisCom::GetCountInner() == 0;
      }

      /// Get the number of initialized elements                              
      constexpr auto GetCount(this auto const& self) noexcept -> size_t {
         return ThisCom::GetCountInner();
      }

      /// Explicit boolean conversion to allow using containers in ifs        
      explicit constexpr operator bool(this auto const& self) noexcept {
         return ThisCom::GetCountInner() != 0;
      }

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
      constexpr auto& GetCountInner(this auto&& self) noexcept {
         return self.template AccessStack<CountStack>();
      }
      
      /// Set the number of initialized elements                              
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
            ThisCom::SetCountInner(from.GetCount());

            // Count is responsible for deep ownership, it has to       
            // be reset on move to disable destruction in 'from'.       
            if constexpr (CT::Moved<I>) {
               // Moving                                                
               if_available(from.SetCountInner(0));
            }
            else if constexpr (CT::Abandoned<I> and CT::OwnedDeepStrong<I>) {
               // Abandoning                                            
               if constexpr (not IT::CanBeDisowned) {
                  // In case source is not disownable, this component   
                  // must make sure to reset relevant properties.       
                  if_available(from.SetCountInner(0));
               }
            }
         }
      }

      /// Reset count (inner)                                                 
      ///   @attention doesn't destroy elements, only resets hash and count   
      constexpr void ResetCount(this auto& self) noexcept {
         ThisCom::SetCountInner(0);
         if_available(self.SetHashEmpty());
      }
   };

   #undef ThisCom
}
