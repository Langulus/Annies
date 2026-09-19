///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Tag.hpp"


namespace Langulus::Annies::Inner
{
   template<CT::DefineTag TAG, CT::NotVoid T> requires (CT::NotHandle<T> and CT::NotReference<T>)
   using TTagBase = Com::Container<
      Com::State::Disowned<>,          // Allows disownment             
      Com::TypedStack<DMeta, T>,       // Type-constrained              
      Com::TaggedStack<TMeta, TAG>,    // Tag-constrained               
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
   /// MARK: TTag                                                             
   ///                                                                        
   /// A statically-tagged and statically-tagged equivalent of Tag            
   template<CT::DefineTag TAG, CT::NotVoid T> 
   struct TTag : Inner::TTagBase<TAG, T> {
      using CTTI_ReflectAs = Tag;
      using CTTI_Deep      = No;
      using Base           = Inner::TTagBase<TAG, T>;

      constexpr TTag() noexcept {
         this->ConstructDefault();
      }
      constexpr TTag(TTag const& other) {
         this->Absorb(Refer(other));
      }
      constexpr TTag(TTag&& other) noexcept {
         this->Absorb(Move(other));
      }
      constexpr ~TTag() noexcept {
         this->Destroy();
      }
      
      /// Construction that either absorbs the provided containers, or        
      /// emplaces all A in the container                                     
      template<class A1, class...AN>
      constexpr TTag(A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0) {
            if constexpr (SameAsOneOf<Deint<A1>, TTag, Tag>) {
               LglsAssumeUser((not SameAsOneOf<T, TTag, Tag>),
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
      constexpr TTag(Inner::Absorb, A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0)
            this->Absorb(LglsFwd(a1));
         else {
            this->ConstructDefault();
            this->Concat(LglsFwd(a1), LglsFwd(an)...);
         }
      }
      
      /// Construction that emplaces all arguments inside                     
      template<class A1, class...AN>
      constexpr TTag(Inner::Piecewise, A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0)
            this->EmplaceConstruct(LglsFwd(a1));
         else {
            this->ConstructDefault();
            this->Insert(LglsFwd(a1), LglsFwd(an)...);
         }
      }

      /// Assignment                                                          
      constexpr TTag& operator = (TTag const& other) {
         return this->AssignAbsorb(Refer(other));
      }
      constexpr TTag& operator = (TTag&& other) noexcept {
         return this->AssignAbsorb(Move(other));
      }

      template<class A>
      constexpr TTag& operator = (A&& argument) {
         if constexpr (SameAsOneOf<Deint<A>, TTag, Tag>) {
            LglsAssumeUser((not SameAsOneOf<T, TTag, Tag>),
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
   TTag(T&&) -> TTag<Decvq<Deref<Deint<T>>>>;

   template<CT::NotVoid T>
   TTag(Inner::Absorb, T&&) -> TTag<TypeOf<T>>;

   template<CT::NotVoid T>
   TTag(Inner::Piecewise, T&&) -> TTag<Decvq<Deref<Deint<T>>>>;
}