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
#include <Langulus/CT/Integer.hpp>
#include <Langulus/Utils/Values.hpp>


namespace Langulus::Annies::Component
{
   /// Refers back to this particular component instance through the deduced  
   /// 'this'. Just for convenience. It is #undef-ed at the end of this file. 
   #define ThisCom self.CountStatic<COUNT, ID, SHARED...>

   ///                                                                        
   ///   A compile-time count                                                 
   ///                                                                        
   ///   Count shows how many elements inside a container are initialized.    
   ///   Compile-time counting isn't really counting, and doesn't take up     
   /// space, but is useful for defining single-element containers that       
   /// still need the API required to function alongside other components.    
   ///   In these cases, count is equal to COUNT if container has a heap      
   /// component that has been allocated - otherwise it is 0. If no heap      
   /// component exists or can't be null, then the count is always COUNT.     
   ///   @tparam COUNT the count type and value                               
   ///   @tparam ID provider ID to keep count of                              
   ///   @tparam SHARED provider IDs that share the same count variable       
   template<auto COUNT, Cid ID, Cid...SHARED>
   struct CountStatic {
      using CTTI_Component  = Yup;
      using CTTI_ReflectAs  = void;
      using CTTI_Contiguous = Maybe<COUNT == 1>;
      using Id              = Values<ID, SHARED...>;

      static constexpr int  ComponentPrecedence = -1000;
      static constexpr bool ContainsMany = COUNT > 1;
      //template<Cid SID>
      //static constexpr bool Relevant = Id::template Contains<SID>;

      using CountType   = decltype(COUNT);
      using ReserveType = CountType;
      using IndexType   = Index::At<CountType>;
      using Dimensions  = Id;

      static_assert(COUNT > 0,
         "Can't have a container of zero or negative count");
      static_assert(CT::Integer<CountType> and not CT::Signed<CountType>,
         "Count type must be an unsigned integer");

      /// MARK: Public                                                        
      /// Equal to COUNT if container has a heap component that has been      
      /// allocated - zero otherwise. If no heap component exists, then the   
      /// count is always COUNT.                                              
      template<Cid SID = ID> //requires Relevant<SID>
      constexpr auto GetCount(this auto const& self) noexcept -> CountType {
         return ThisCom::template GetCountInner<SID>();
      }
      
      /// Check if empty                                                      
      template<Cid SID = ID> //requires Relevant<SID>
      constexpr bool IsEmpty(this auto const& self) noexcept {
         return ThisCom::template GetCountInner<SID>() == CountType {0};
      }

      /// Explicit boolean conversion to allow using containers in ifs        
      explicit constexpr operator bool(this auto const& self) noexcept {
         return ThisCom::GetCountInner() != CountType {0};
      }

   protected:
      /// MARK: Protected                                                     
      LglsComRemoval(friend);
      LglsComEmplacement(friend);
      LglsComInsertion(friend);
      LglsComIndexedLinear(friend);
      LglsComHeapMovable(friend);
      LglsComConversion(friend);

      /// Get count (inner)                                                   
      template<Cid SID = ID, CT::Container C> //requires Relevant<SID>
      constexpr auto GetCountInner(this C const& self) noexcept -> CountType {
         if constexpr (CT::HasVariableCount<C>)
            return self.template GetRaw<SID>() ? COUNT : CountType {0};
         else
            return COUNT;
      }

      /// Doesn't transfer anything but still serves the purpose of checking  
      /// whether count is compatible, and to enable disowned state           
      ///   @attention this is noop when constructing from deep intents,      
      ///      since element constructors might throw and stuff be partially  
      ///      inserted. In those cases, count is set by the heap components. 
      ///   @param intent the intent and container to transfer from           
      template<class SELF, CT::Intent I> requires (CT::Container<I>
      and not CT::Copied<I> and not CT::Cloned<I> and not CT::Disowned<I> and CT::HeapAllocated<I>)
      void ConstructFrom(this SELF&, I&& intent) {
         using IT = Decvq<Deref<Deint<I>>>;
         decltype(auto) from = LglsFwd(intent.what);
         LglsAssumeDev(from.template IsEmpty<ID>() or COUNT <= from.template GetCount<ID>(),
            "Incompatible count");

         // Count is responsible for deep ownership, it has to          
         // be reset on move to disable destruction in 'from'.          
         if constexpr (CT::Moved<I>) {
            // Moving                                                   
            if_available(from.template SetCountInner<ID>(0));
         }
         else if constexpr (CT::Abandoned<I> and CT::OwnedStrong<I>) {
            // Abandoning                                               
            if constexpr (not IT::CanBeDisowned) {
               // In case source is not disownable, this component      
               // must make sure to reset relevant properties.          
               if_available(from.template SetCountInner<ID>(0));
            }
         }
      }

      /// Reset count (inner)                                                 
      ///   @attention doesn't destroy elements, only resets hash and count   
      template<Cid SID = ID> //requires Relevant<SID>
      constexpr void ResetCount(this auto& self) noexcept {
         if_available(self.template SetHeapInner<SID>(nullptr));
         if_available(self.template SetHashInner<SID>(1));
      }
   };

   #undef ThisCom
}
