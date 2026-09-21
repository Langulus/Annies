///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Handle.hpp"
#include "Annies/Container.hpp"
#include "Annies/Components/Typed-Stack.hpp"
#include "Annies/Components/Heap-Movable.hpp"
#include "Annies/Components/Count-Stack.hpp"
#include "Annies/Components/Reserve-Stack.hpp"
#include "Annies/Components/Ownership-Stack.hpp"
#include "Annies/Components/OwnershipDeep-Heap.hpp"
#include "Annies/Components/Hash-Heap.hpp"
#include "Annies/Components/IndexedHash-Heap.hpp"
#include "Annies/Components/Merging.hpp"
#include "Annies/Components/MergingOperators.hpp"
#include "Annies/Components/Assignment.hpp"
#include "Annies/Components/Removal.hpp"
#include "Annies/Components/Conversion.hpp"
#include "Annies/Components/Comparison.hpp"
#include "Annies/Components/Iteration-ForEach.hpp"
#include "Annies/Components/Iteration-Range.hpp"
#include "Annies/Components/State-Stack.hpp"
#include "Annies/States/Typed.hpp"
#include "Annies/States/Sorted.hpp"
#include "Annies/States/Compressed.hpp"
#include "Annies/States/Encrypted.hpp"
#include "Annies/States/Tracked.hpp"


namespace Langulus::Annies::Inner
{
   /// MARK: SetBase                                                          
   template<StateValue SORT>
   using SetBase = Com::Container<
      Com::State::Disowned<>,          // Allows disownment             
      Com::TypedStack<DMeta>,          // Type-erased                   
      Com::HeapMovable<8, 2>,          // Pointer to heap memory        
      Com::CountStack<>,               // Dynamically sized             
      Com::ReserveStack<>,             // Reserve kept as member        
      Com::IndexedHashHeap<>,          // Indexed by hash table         
      Com::OwnershipStack<>,           // Allocation is referenced      
      Com::OwnershipDeepHeap<>,        // Sparse elements are referenced
      Com::HashHeap<>,                 // Hash can be cached            
      Com::Merging<>,                  // Allows merging                
      Com::MergingOperators<>,         // <<= and >>= merging           
      Com::Assignment<>,               // Allows assignment             
      Com::Removal<>,                  // Allows clear/reset            
      Com::Conversion<>,               // Allows conversions            
      Com::Comparison<>,               // Allows comparisons            
      Com::IterationForEach<>,         // ForEach iteration             
      Com::IterationRange<>,           // Ranged iteration              
      Com::State::Sorted<SORT>,        // Toggle ordering               
      Com::State::Compressed<>,        // Toggle compression            
      Com::State::Encrypted<>          // Toggle encryption             
   >;

   /// MARK: Set                                                              
   ///                                                                        
   ///   A universal type-erased non-contiguous set of variable size.         
   /// Emplacement is disabled for sets, because elements aren't allowed to   
   /// change in-place. This also means that they are only const-iteratable.  
   template<StateValue SORTED>
   struct Set : SetBase<SORTED> {
      using CTTI_ReflectAs = Set;
      using CTTI_Set       = Yup;
      using CTTI_Deep      = Yup;

      using Base           = SetBase<SORTED>;
      using DeepType       = Many;
      using HandleType     = Handle;
      using HandleMutType  = Handle;
      using Pick           = Handle;
      using PickMut        = Handle;

      constexpr Set() noexcept {
         this->ConstructDefault();
      }
      constexpr Set(Set const& other) {
         this->Absorb(Refer(other));
      }
      constexpr Set(Set&& other) noexcept  {
         this->Absorb(Move(other));
      }
      constexpr ~Set() noexcept {
         this->Destroy();
      }

      /// Construction that either absorbs the provided containers, or        
      /// emplaces all A in the container                                     
      template<class A1, class...AN>
      constexpr Set(A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0 and CT::Set<A1>) {
            LglsAssumeUser((Same<Deint<A1>, Set>),
               "Ambiguous use of construction "
               "- you should use tag-dispatch with first argument either Absorb "
               "(if you want to overwrite the container itself) or Piecewise "
               "(if you want to overwrite the first item) in order to clearly "
               "state your intent. Absorb will be used by default!"
            );
            this->Absorb(LglsFwd(a1));
         }
         else {
            this->ConstructDefault();
            this->Merge(LglsFwd(a1));
           (this->Merge(LglsFwd(an)), ...);
         }
      }
      
      /// Construction that absorbs the provided containers                   
      template<class A1, class...AN>
      constexpr Set(Inner::Absorb, A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0)
            this->Absorb(LglsFwd(a1));
         else {
            this->ConstructDefault();
            this->MergeRange(LglsFwd(a1));
           (this->MergeRange(LglsFwd(an)), ...);
         }
      }
      
      /// Construction that emplaces all arguments inside                     
      template<class A1, class...AN>
      constexpr Set(Inner::Piecewise, A1&& a1, AN&&...an) {
         this->ConstructDefault();
         this->Merge(LglsFwd(a1));
        (this->Merge(LglsFwd(an)), ...);
      }
      
      /// Assignment                                                          
      constexpr Set& operator = (Set const& other) {
         return this->AssignAbsorb(Refer(other));
      }
      constexpr Set& operator = (Set&& other) noexcept {
         return this->AssignAbsorb(Move(other));
      }
      
      template<class A>
      constexpr Set& operator = (A&& argument) {
         if constexpr (CT::Set<A>) {
            LglsAssumeUser((Same<Deint<A>, Set>),
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

      /// Create a temporary swapper with compatible elements and initialize  
      /// it with a compatible value, by shallow copying it.                  
      template<class A>
      constexpr auto CreateSwapper(A&& argument) assumptious {
         LglsAssumeDev(this->template IsSame<Deint<A>>(), "Type mismatch");
         return TAny {Annies::Piecewise, LglsFwd(argument)};
      }
   };
}

namespace Langulus::Annies
{
   using Set         = Inner::Set<StateValue::Variable>;
   using SetSorted   = Inner::Set<StateValue::Enabled>;
   using SetUnsorted = Inner::Set<StateValue::Disabled>;
}

namespace Langulus
{
   using Annies::Set;
}