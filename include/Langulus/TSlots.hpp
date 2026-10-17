///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Many.hpp"
#include "Annies/Components/IndexedCoverage-Stack.hpp"


namespace Langulus::Annies::Inner
{
   template<CT::NotVoid T> requires (CT::NotHandle<T> and CT::NotReference<T>)
   using TSlotsBase = Com::Container<
      Com::State::Compressed<>,        // Toggle compression            
      Com::State::Encrypted<>,         // Toggle encrypted              
      Com::TypedStack<DMeta, T>,       // Type-constrained              
      Com::HeapMovable<0, 0, HeapEntry<0, T*>>,
      Com::CountStack<>,               // Dynamically sized             
      Com::ReserveStack<>,             // Reserve kept as member        
      Com::IndexedCoverageStack<>,     // Indexed by coverage mask      
      Com::OwnershipStack<>,           // Allocation is referenced      
      EnableComponentIf<CT::Sparse<T>, Com::OwnershipDeepHeap<>>,
      Com::HashStack<>,                // Hash can be cached            
      Com::Insertion<>,                // Allows insertion              
      Com::InsertionOperators<>,       // << and >> insertion           
      Com::Merging<>,                  // Allows merging                
      Com::MergingOperators<>,         // <<= and >>= merging           
      Com::Emplacement<>,              // Allows emplacement            
      Com::Assignment<>,               // Allows assignment             
      Com::Removal<>,                  // Allows clear/reset            
      Com::Conversion<>,               // Allows conversions            
      Com::Comparison<>,               // Allows comparisons            
      Com::IterationForEach<>,         // ForEach iteration             
      Com::IterationRange<>            // Ranged iteration              
   >;
}

namespace Langulus::Annies
{
   /// MARK: TSlots                                                           
   ///                                                                        
   /// A statically-typed non-contiguous container of constant size, capable  
   /// of reusing empty cells.                                                
   template<CT::NotVoid T> 
   struct TSlots : Inner::TSlotsBase<T> {
      using CTTI_ReflectAs = TSlots;
      using CTTI_Deep      = Yup;
      using CTTI_POD       = No; //TODO could be, when we implement SVO
      using Base           = Inner::TSlotsBase<T>;
      using DeepType       = Many;//Any;

      constexpr TSlots() noexcept {
         this->ConstructDefault();
      }
      constexpr TSlots(TSlots const& other) {
         this->Absorb(Refer(other));
      }
      constexpr TSlots(TSlots&& other) noexcept {
         this->Absorb(Move(other));
      }
      constexpr ~TSlots() noexcept {
         this->Destroy();
      }
      
      /// Construction that either absorbs the provided containers, or        
      /// emplaces all A in the container                                     
      template<class A1, class...AN>
      constexpr TSlots(A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0) {
            if constexpr (Same<Deint<A1>, TSlots>) {
               /*LglsAssumeUser(not SameAsOneOf<T, TMany, Many>,
                  "Ambiguous use of construction "
                  "- you should use tag-dispatch with first argument either Absorb "
                  "(if you want to overwrite the container itself) or Piecewise "
                  "(if you want to overwrite the first item) in order to clearly "
                  "state your intent. Absorb will be used by default!"
               );*/
               this->Absorb(LglsFwd(a1));
            }
            else this->EmplaceConstruct(LglsFwd(a1));
         }
         else {
            this->ConstructDefault();
            this->Insert(LglsFwd(a1), LglsFwd(an)...);
         }
      }
      
      /// Construction that absorbs the provided container                    
      template<class A1, class...AN>
      constexpr TSlots(Inner::Absorb, A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0)
            this->Absorb(LglsFwd(a1));
         else {
            this->ConstructDefault();
            this->Concat(LglsFwd(a1), LglsFwd(an)...);
         }
      }
      
      /// Construction that emplaces all arguments inside                     
      template<class A1, class...AN>
      constexpr TSlots(Inner::Piecewise, A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0)
            this->EmplaceConstruct(LglsFwd(a1));
         else {
            this->ConstructDefault();
            this->Insert(LglsFwd(a1), LglsFwd(an)...);
         }
      }

      /// Assignment                                                          
      constexpr TSlots& operator = (TSlots const& other) {
         return this->AssignAbsorb(Refer(other));
      }
      constexpr TSlots& operator = (TSlots&& other) noexcept {
         return this->AssignAbsorb(Move(other));
      }

      template<class A>
      constexpr TSlots& operator = (A&& argument) {
         if constexpr (Same<Deint<A>, TSlots>) {
            /*LglsAssumeUser(not SameAsOneOf<T, TMany, Many>,
               "Ambiguous use of assignment "
               "- you should use either AssignAbsorb (if you want to overwrite "
               "the container itself) or Assign (if you want to overwrite the "
               "first item) in order to clearly state your intent. "
               "AssignAbsorb will be used by default!"
            );*/
            return this->AssignAbsorb(LglsFwd(argument));
         }
         else return this->Assign(LglsFwd(argument));
      }
      
      using Com::Comparison<>::operator <=>;
      using Com::Comparison<>::operator ==;
      using Com::IterationRange<>::begin;
      using Com::IterationRange<>::end;
      using Com::IterationRange<>::rbegin;
      using Com::IterationRange<>::rend;
   };

   /// MARK: CTAD                                                             
   template<CT::NotVoid T>
   TSlots(T&&) -> TSlots<Decvq<Deref<Deint<T>>>>;

   template<CT::NotVoid T>
   TSlots(Inner::Absorb, T&&) -> TSlots<TypeOf<T>>;

   template<CT::NotVoid T>
   TSlots(Inner::Piecewise, T&&) -> TSlots<Decvq<Deref<Deint<T>>>>;
}

namespace Langulus
{
   using Annies::TSlots;
}