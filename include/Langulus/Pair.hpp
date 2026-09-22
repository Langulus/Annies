///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "HandlePair.hpp"
#include "Annies/Components/Typed-Stack.hpp"
#include "Annies/Components/Multitype.hpp"
#include "Annies/Components/Heap-Movable.hpp"
#include "Annies/Components/Count-Static.hpp"
#include "Annies/Components/Reserve-Static.hpp"
#include "Annies/Components/Ownership-Stack.hpp"
#include "Annies/Components/OwnershipDeep-Heap.hpp"
#include "Annies/Components/Hash-Emergent.hpp"
#include "Annies/Components/Emplacement.hpp"
#include "Annies/Components/Assignment.hpp"
#include "Annies/Components/Removal.hpp"
#include "Annies/Components/Conversion.hpp"
#include "Annies/Components/Comparison.hpp"
#include "Annies/States/Encrypted.hpp"
#include "Annies/States/Disowned.hpp"


namespace Langulus::Annies::Inner
{
   /// Type-erased heap-based pair container                                  
   using PairBase = Com::Container<
      Com::State::Disowned<>,             // Allows disownment          
      Com::Multitype<Com::TypedStack<DMeta, void, false, 0>,
                     Com::TypedStack<DMeta, void, false, 1>>,
      Com::HeapMovable<0, 0, HeapEntry<0>, HeapEntry<1>>,
      Com::CountStatic<1u, 0, 1>,         // Statically sized to 1      
      Com::ReserveStatic<1u, 0, 1>,       // Statically reserved to 1   
      Com::OwnershipStack<Com::StrongOwnership, 0, 1>,
      Com::OwnershipDeepHeap<Com::StrongOwnership, true, 0, 1>,
      Com::HashEmergent<0, Hash, 1>,      // Hash retrieved from items  
      Com::Emplacement<0, 1>,             // Allows emplacement         
      Com::Assignment<false, 0, 1>,       // Allows assignment          
      Com::Removal<0, 1>,                 // Allows clear/reset         
      Com::Conversion<0, 1>,              // Allows conversion          
      Com::Comparison<false, true, 0, 1>, // Allows comparisons         
      Com::State::Encrypted<>             // Toggle encryption          
   >;
}

namespace Langulus::Annies
{
   /// MARK: Pair                                                             
   ///                                                                        
   /// A type-erased pair                                                     
   struct Pair : Inner::PairBase {
      using CTTI_ReflectAs = Pair;
      using CTTI_Deep      = Yup;
      using CTTI_Pair      = Yup;

      static constexpr bool ReferenceElements = true;

      using Base           = Inner::PairBase;
      using DeepType       = Many;//Any;
      using HandleType     = THandlePair<Handle, Handle>;
      using HandleMutType  = THandlePair<HandleMut, HandleMut>;
      using Pick           = HandleType;
      using PickMut        = HandleMutType;

      constexpr Pair() noexcept {
         this->ConstructDefault();
      }
      constexpr Pair(Pair const& other) {
         this->Absorb(Refer(other));
      }
      constexpr Pair(Pair&& other) noexcept  {
         this->Absorb(Move(other));
      }
      constexpr ~Pair() noexcept {
         this->Destroy();
      }
      
      constexpr Pair(CT::Pair auto&& p) {
         this->Absorb(LglsFwd(p));
      }
      
      constexpr Pair(Inner::Absorb, CT::Pair auto&& p) {
         this->Absorb(LglsFwd(p));
      }

      constexpr Pair(NotTag auto&& a1, NotTag auto&& a2) {
         this->ResetState();
         this->DeduceType(a1, a2);
         this->AllocateFresh(1);
         this->template EmplaceConstruct<0, Com::AllocationStrategy::DontAllocate>(LglsFwd(a1));
         this->template EmplaceConstruct<1, Com::AllocationStrategy::DontAllocate>(LglsFwd(a2));
      }

      constexpr Pair(Inner::Piecewise, auto&& a1, auto&& a2)
         : Pair {LglsFwd(a1), LglsFwd(a2)} {}

      /// Assignment                                                          
      constexpr Pair& operator = (Pair const& other) {
         return this->AssignAbsorb(Refer(other));
      }
      constexpr Pair& operator = (Pair&& other) noexcept {
         return this->AssignAbsorb(Move(other));
      }
      constexpr Pair& operator = (CT::Pair auto&& pair) {
         return this->AssignAbsorb(LglsFwd(pair));
      }
      
      /// Clear the pair and assign a new key and value.                      
      /// It is safe to use different types.                                  
      constexpr Pair& Assign(auto&& a1, auto&& a2) {
         this->Reset();
         this->DeduceType(a1, a2);
         this->AllocateFresh(1);
         this->template EmplaceConstruct<0, Com::AllocationStrategy::DontAllocate>(LglsFwd(a1));
         this->template EmplaceConstruct<1, Com::AllocationStrategy::DontAllocate>(LglsFwd(a2));
         return *this;
      }

      using Com::Comparison<false, true, 0, 1>::operator <=>;
      using Com::Comparison<false, true, 0, 1>::operator ==;

      auto GetKeyHandle() const noexcept -> typename HandleType::KeyHandle {
         return {*this};
      }

      auto GetKeyHandle() noexcept -> typename HandleMutType::KeyHandle {
         return {*this};
      }

      auto GetValHandle() const noexcept -> typename HandleType::ValHandle {
         return {Slice<1>, *this};
      }

      auto GetValHandle() noexcept -> typename HandleMutType::ValHandle {
         return {Slice<1>, *this};
      }
   };

   static_assert(CT::TypeErased<Pair>);
}

namespace Langulus
{
   using Annies::Pair;
}