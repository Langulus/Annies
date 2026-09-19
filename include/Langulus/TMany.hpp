///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Many.hpp"


namespace Langulus::Annies::Inner
{
   template<CT::NotVoid T> requires (CT::NotHandle<T> and CT::NotReference<T>)
   using TManyBase = Com::Container<
      Com::State::Disowned<>,          // Allows disownment             
      Com::TypedStack<DMeta, T>,       // Type-constrained              
      Com::HeapMovable<0, 0, HeapEntry<0, T*>>,
      Com::CountStack<>,               // Dynamically sized             
      Com::ReserveStack<>,             // Reserve kept as member        
      Com::IndexedLinear<>,            // Indexed directly              
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
      Com::IterationRange<>,           // Ranged iteration              
      Com::State::Future<>,            // Toggle future linking         
      Com::State::Past<>,              // Toggle past linking           
      Com::State::Or<>,                // Toggle disjunction            
      Com::State::Compressed<>,        // Toggle compression            
      Com::State::Encrypted<>          // Toggle encrypted              
   >;
}

namespace Langulus::Annies
{
   /// MARK: TMany                                                            
   ///                                                                        
   /// A statically-typed contiguous container of variable size that is       
   /// binary-compatible with the type-erased alternative `Many`.             
   template<CT::NotVoid T> 
   struct TMany : Inner::TManyBase<T> {
      using CTTI_ReflectAs = Many;
      using CTTI_Deep      = Yup;
      using Base           = Inner::TManyBase<T>;
      using DeepType       = Any;

      constexpr TMany() noexcept {
         this->ConstructDefault();
      }
      constexpr TMany(TMany const& other) {
         this->Absorb(Refer(other));
      }
      constexpr TMany(TMany&& other) noexcept {
         this->Absorb(Move(other));
      }
      constexpr ~TMany() noexcept {
         this->Destroy();
      }
      
      /// Construction that either absorbs the provided containers, or        
      /// emplaces all A in the container                                     
      template<class A1, class...AN>
      constexpr TMany(A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0) {
            if constexpr (SameAsOneOf<Deint<A1>, TMany, Many>) {
               LglsAssumeUser((not SameAsOneOf<T, TMany, Many>),
                  "Ambiguous use of construction "
                  "- you should use tag-dispatch with first argument either Absorb "
                  "(if you want to overwrite the container itself) or Piecewise "
                  "(if you want to overwrite the first item) in order to clearly "
                  "state your intent. Absorb will be used by default!"
               );
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
      constexpr TMany(Inner::Absorb, A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0)
            this->Absorb(LglsFwd(a1));
         else {
            this->ConstructDefault();
            this->Concat(LglsFwd(a1), LglsFwd(an)...);
         }
      }
      
      /// Construction that emplaces all arguments inside                     
      template<class A1, class...AN>
      constexpr TMany(Inner::Piecewise, A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0)
            this->EmplaceConstruct(LglsFwd(a1));
         else {
            this->ConstructDefault();
            this->Insert(LglsFwd(a1), LglsFwd(an)...);
         }
      }

      /// Assignment                                                          
      constexpr TMany& operator = (TMany const& other) {
         return this->AssignAbsorb(Refer(other));
      }
      constexpr TMany& operator = (TMany&& other) noexcept {
         return this->AssignAbsorb(Move(other));
      }

      template<class A>
      constexpr TMany& operator = (A&& argument) {
         if constexpr (SameAsOneOf<Deint<A>, TMany, Many>) {
            LglsAssumeUser((not SameAsOneOf<T, TMany, Many>),
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
      using Com::IterationRange<>::begin;
      using Com::IterationRange<>::end;
      using Com::IterationRange<>::rbegin;
      using Com::IterationRange<>::rend;
   };

   /// MARK: CTAD                                                             
   template<CT::NotVoid T>
   TMany(T&&) -> TMany<Decvq<Deref<Deint<T>>>>;

   template<CT::NotVoid T>
   TMany(Inner::Absorb, T&&) -> TMany<TypeOf<T>>;

   template<CT::NotVoid T>
   TMany(Inner::Piecewise, T&&) -> TMany<Decvq<Deref<Deint<T>>>>;
}