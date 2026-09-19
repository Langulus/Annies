///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Handle.hpp"
#include "Annies/Components/Typed-Stack.hpp"
#include "Annies/Components/Heap-Movable.hpp"
#include "Annies/Components/Ownership-Stack.hpp"
#include "Annies/Components/Count-Static.hpp"
#include "Annies/Components/Reserve-Emergent.hpp"
#include "Annies/Components/OwnershipDeep-Heap.hpp"
#include "Annies/Components/Hash-Emergent.hpp"
#include "Annies/Components/Emplacement.hpp"
#include "Annies/Components/Assignment.hpp"
#include "Annies/Components/Removal.hpp"
#include "Annies/Components/Conversion.hpp"
#include "Annies/Components/Comparison.hpp"
#include "Annies/Components/State-Stack.hpp"
#include "Annies/States/Future.hpp"
#include "Annies/States/Past.hpp"
#include "Annies/States/Compressed.hpp"
#include "Annies/States/Encrypted.hpp"
#include "Annies/States/Tracked.hpp"
#include "Annies/States/Disowned.hpp"


namespace Langulus::Annies::Inner
{
   using AnyBase = Com::Container<
      Com::State::Disowned<>,          // Allows disownment             
      Com::TypedStack<DMeta>,          // Type-erased                   
      Com::HeapMovable<>,              // Pointer to heap memory        
      Com::CountStatic<1u>,            // Statically sized to 1         
      Com::ReserveEmergent<>,          // Reserve derived from alloc    
      Com::OwnershipStack<>,           // Allocation is referenced      
      Com::OwnershipDeepHeap<>,        // Sparse elements are referenced
      Com::HashEmergent<>,             // Hash is retrieved from item   
      Com::Emplacement<>,              // Allows emplacement            
      Com::Assignment<>,               // Allows assignment             
      Com::Removal<>,                  // Allows clear/reset            
      Com::Conversion<>,               // Allows conversion             
      Com::Comparison<>,               // Allows comparisons            
      Com::State::Future<>,            // Allows future linking         
      Com::State::Past<>               // Allows past linking           
   >;
}

namespace Langulus::Annies
{
   ///                                                                        
   /// A type-erased container of size 1.                                     
   /// This is the most universal and feature-complete container, that        
   /// supports all kinds of data states: compression, encryption, linking,   
   /// and so on. For a slightly smaller and faster representation, consider  
   /// using Own or Ref instead. If you want to contain a number of similar   
   /// elements use Many instead.                                             
   struct Any : Inner::AnyBase {
      using CTTI_ReflectAs = Any;
      using CTTI_Deep      = Yup;
      using Base           = Inner::AnyBase;
      using DeepType       = Any;

      constexpr Any() noexcept {
         this->ConstructDefault();
      }
      constexpr Any(Any const& other) {
         this->Absorb(Refer(other));
      }
      constexpr Any(Any&& other) noexcept  {
         this->Absorb(Move(other));
      }
      constexpr ~Any() noexcept {
         this->Destroy();
      }

      /// Construction that either absorbs the provided container, or         
      /// emplaces A in the container                                         
      template<class A>
      constexpr Any(A&& argument) {
         if constexpr (CT::ContainsOne<A>) {
            LglsAssumeUser((Same<Deint<A>, Any>),
               "Ambiguous use of construction "
               "- you should use tag-dispatch with first argument either Absorb "
               "(if you want to overwrite the container itself) or Piecewise "
               "(if you want to overwrite the first item) in order to clearly "
               "state your intent. Absorb will be used by default!"
            );
            this->Absorb(LglsFwd(argument));
         }
         else this->EmplaceConstruct(LglsFwd(argument));
      }
      
      /// Construction that absorbs the provided container                    
      template<class A>
      constexpr Any(Inner::Absorb, A&& argument) {
         this->Absorb(LglsFwd(argument));
      }
      
      /// Construction that emplaces A inside                                 
      template<class A>
      constexpr Any(Inner::Piecewise, A&& argument) {
         this->EmplaceConstruct(LglsFwd(argument));
      }
      
      /// Assignment                                                          
      constexpr Any& operator = (Any const& other) {
         return this->AssignAbsorb(Refer(other));
      }
      constexpr Any& operator = (Any&& other) noexcept {
         return this->AssignAbsorb(Move(other));
      }
      
      template<class A>
      constexpr Any& operator = (A&& argument) {
         if constexpr (CT::ContainsOne<A>) {
            LglsAssumeUser((Same<Deint<A>, Any>),
               "Ambiguous use of assignment "
               "- you should use either AssignAbsorb (if you want to overwrite "
               "the container itself) or Assign (if you want to overwrite the "
               "first item) in order to clearly state your intent. "
               "AssignAbsorb will be used by default!"
            );
            return this->AssignAbsorb(LglsFwd(argument));
         }
         else return this->Assign(LglsFwd(argument));
      }

      using Com::Comparison<>::operator <=>;
      using Com::Comparison<>::operator ==;
   };
}

namespace Langulus
{
   using Annies::Any;
}