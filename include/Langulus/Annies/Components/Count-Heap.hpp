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


namespace Langulus::Annies::Component
{
   /// Refers back to this particular component instance through the deduced  
   /// 'this'. Just for convenience. It is #undef-ed at the end of this file. 
   #define ThisCom self.CountHeap<T, ID, SHARED...>

   ///                                                                        
   /// Defines count as a part of the heap                                    
   /// Count shows how many elements inside a container are initialized       
   /// Heap-based counting keeps the counter inside the container's heap      
   /// allocation, and requires an indirection everytime count is accessed.   
   /// It is a bit slower and less cache-friendly, but results in more        
   /// compact containers                                                     
   ///   @tparam T the count type                                             
   ///   @tparam ID provider ID to keep count of                              
   ///   @tparam SHARED provider IDs that share the same count variable       
   template<class T, Cid ID, Cid...SHARED>
   struct CountHeap {
      using CTTI_Component = Yup;
      using CTTI_ReflectAs = void;
      using Id = Values<ID, SHARED...>;

      using CountType   = T;
      using IndexType   = Index::At<T>;
      using HeapRequest = T;
      using Dimensions  = Id;

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
      LglsComIndexedCommon(friend);
      LglsComIndexedLinear(friend);
      LglsComHeapMovable(friend);
      LglsComConversion(friend);

      /// Get count (inner)                                                   
      template<Cid SID = ID> //requires Relevant<SID>
      constexpr auto& GetCountInner(this auto&& self) noexcept {
         return self.template AccessHeap<CountHeap>();
      }
      
      /// Set the number of initialized elements                              
      template<Cid SID = ID> //requires Relevant<SID>
      constexpr void SetCountInner(this auto& self, T c) assumptious {
         LglsAssumeDev(self.template GetUses<SID>() == 1);
         ThisCom::GetCountInner() = c;
      }

      /// Reset count (inner)                                                 
      ///   @attention doesn't destroy elements, only resets hash and count   
      template<Cid SID = ID> //requires Relevant<SID>
      constexpr void ResetCount(this auto& self) assumptious {
         ThisCom::SetCountInner(0);
         if_available(self.template SetHashInner<SID>(1));
      }
   };

   #undef ThisCom
}
