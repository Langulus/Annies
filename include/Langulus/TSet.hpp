///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Set.hpp"


namespace Langulus::Annies::Inner
{
   template<CT::NotVoid T, StateValue SORT>
   requires (CT::NotHandle<T> and CT::NotReference<T>)
   using TSetBase = Com::Container<
      Com::State::Disowned<>,          // Allows disownment             
      Com::TypedStack<DMeta, T>,       // Type-constrained              
      Com::HeapMovable<8, 2, HeapEntry<0, T*>>,
      Com::CountStack<>,               // Dynamically sized             
      Com::ReserveStack<>,             // Reserve kept as member        
      Com::IndexedHashHeap<>,          // Indexed by hash table         
      Com::OwnershipStack<>,           // Allocation is referenced      
      EnableComponentIf<CT::Sparse<T>, Com::OwnershipDeepHeap<>>,
      Com::HashHeap<>,                 // Hash can be cached            
      Com::Merging<>,                  // Allows merging                
      Com::MergingOperators<>,         // <<= and >>= merging           
      Com::Assignment<>,               // Allows assignment             
      Com::Removal<>,                  // Allows clear/reset            
      Com::Conversion<>,               // Allows conversions            
      Com::Comparison<>,               // Allows comparisons            
      Com::IterationForEach<>,         // ForEach iteration             
      Com::IterationRange<>,           // Ranged iteration              
      Com::State::Sorted<SORT>,        // Toggle ordered set            
      Com::State::Compressed<>,        // Toggle compression            
      Com::State::Encrypted<>          // Toggle encryption             
   >;
}

namespace Langulus::Annies
{
   ///                                                                        
   /// A statically-typed non-contiguous set of variable size that is         
   /// binary-compatible with the type-erased alternative `Set`.              
   /// Emplacement is disabled for sets, because elements aren't allowed to   
   /// change in-place. This also means that they are only const-iteratable.  
   template<CT::NotVoid T, StateValue SORT>
   struct TSet : Inner::TSetBase<T, SORT> {
      using CTTI_ReflectAs = Set;
      using CTTI_Set       = Yup;
      using CTTI_Deep      = Yup;

      using Base           = Inner::TSetBase<T, SORT>;
      using DeepType       = Many;//Any;

      #if LANGULUS(FORCE_TYPE_ERASURE)
         using HandleType     = Handle;
         using HandleMutType  = Handle;
      #else
         using HandleType     = THandle<ConstAll<T&>>;
         using HandleMutType  = THandle<ConstAll<T&>>;
      #endif
      
      using Pick           = ConstAll<T&>;
      using PickMut        = ConstAll<T&>;

      constexpr TSet() noexcept {
         this->ConstructDefault();
      }
      constexpr TSet(TSet const& other) {
         this->Absorb(Refer(other));
      }
      constexpr TSet(TSet&& other) noexcept {
         this->Absorb(Move(other));
      }
      constexpr ~TSet() noexcept {
         this->Destroy();
      }
      
      /// Construction that either absorbs the provided containers, or        
      /// emplaces all A in the container                                     
      template<class A1, class...AN>
      constexpr TSet(A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0) {
            if constexpr (CT::Set<A1>) {
               LglsAssumeUser(CT::NotSet<T>,
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
            }
         }
         else {
            this->ConstructDefault();
            this->Merge(LglsFwd(a1));
           (this->Merge(LglsFwd(an)), ...);
         }
      }
      
      /// Construction that absorbs the provided container                    
      template<class A1, class...AN>
      constexpr TSet(Inner::Absorb, A1&& a1, AN&&...an) {
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
      constexpr TSet(Inner::Piecewise, A1&& a1, AN&&...an) {
         this->ConstructDefault();
         this->Merge(LglsFwd(a1));
        (this->Merge(LglsFwd(an)), ...);
      }

      /// Assignment                                                          
      constexpr TSet& operator = (TSet const& other) {
         return this->AssignAbsorb(Refer(other));
      }
      constexpr TSet& operator = (TSet&& other) noexcept {
         return this->AssignAbsorb(Move(other));
      }

      template<class A>
      constexpr TSet& operator = (A&& argument) {
         if constexpr (CT::Set<A>) {
            LglsAssumeUser(CT::NotSet<T>,
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
         static_assert(Same<T, Deint<A>>, "Type mismatch");
         return TAny {Annies::Piecewise, LglsFwd(argument)};
      }
      
      using Com::Comparison<>::operator <=>;
      using Com::Comparison<>::operator ==;
      using Com::IterationRange<>::begin;
      using Com::IterationRange<>::end;
      using Com::IterationRange<>::rbegin;
      using Com::IterationRange<>::rend;
   };

   template<CT::NotVoid T>
   using TSetSorted = TSet<T, StateValue::Enabled>;

   template<CT::NotVoid T>
   using TSetUnsorted = TSet<T, StateValue::Disabled>;
}

namespace Langulus
{
   using Annies::TSet;
   using Annies::TSetSorted;
   using Annies::TSetUnsorted;
}