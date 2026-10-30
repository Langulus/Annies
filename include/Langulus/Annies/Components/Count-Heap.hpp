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
#include <Langulus/CT/Contiguous.hpp>
#include <Langulus/CT/Signed.hpp>


namespace Langulus::Annies::Component
{
   ///                                                                        
   /// Defines count as a part of the heap.                                   
   /// Count shows how many elements inside a container are initialized       
   /// Heap-based counting keeps the counter inside the container's heap      
   /// allocation, and requires an indirection everytime count is accessed.   
   /// It is a bit slower and less cache-friendly, but results in more        
   /// stack-compact containers.                                              
   ///   @tparam T the count type                                             
   ///   @attention only one count component is allowed per container         
   template<class T>
   struct CountHeap {
      /// Refers back to this particular component instance through the       
      /// deduced 'this'. Just for convenience. It is #undef-ed at the end of 
      /// this file.                                                          
      #define ThisCom self.CountHeap<T>

      using CTTI_Component = Yup;
      using CTTI_ReflectAs = void;

      using CountType   = T;
      using IndexType   = Index::At<T>;
      using HeapRequest = T;

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
      LglsComIndexedCommon(friend);
      LglsComIndexedLinear(friend);
      LglsComHeapMovable(friend);
      LglsComConversion(friend);

      /// Get count (inner)                                                   
      constexpr auto& GetCountInner(this auto&& self) noexcept {
         return self.template AccessHeap<CountHeap>();
      }
      
      /// Set the number of initialized elements                              
      ///   @attention allowed only when heap is owned only here              
      constexpr void SetCountInner(this auto& self, T c) assumptious {
         LglsAssumeDev(self.template GetUses<SID>() == 1);
         ThisCom::GetCountInner() = c;
      }

      /// Reset count (inner)                                                 
      ///   @attention doesn't destroy elements, only resets hash and count   
      constexpr void ResetCount(this auto& self) assumptious {
         ThisCom::SetCountInner(0);
         if_available(self.SetHashEmpty());
      }
   };

   #undef ThisCom
}
