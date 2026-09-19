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
#include "Annies/Components/Tagged-Stack.hpp"
#include "Annies/Components/Heap-Movable.hpp"
#include "Annies/Components/Ownership-Stack.hpp"
#include "Annies/Components/Count-Stack.hpp"
#include "Annies/Components/Reserve-Stack.hpp"
#include "Annies/Components/OwnershipDeep-Heap.hpp"
#include "Annies/Components/Hash-Stack.hpp"
#include "Annies/Components/Emplacement.hpp"
#include "Annies/Components/IndexedLinear.hpp"
#include "Annies/Components/Insertion.hpp"
#include "Annies/Components/InsertionOperators.hpp"
#include "Annies/Components/Merging.hpp"
#include "Annies/Components/MergingOperators.hpp"
#include "Annies/Components/Assignment.hpp"
#include "Annies/Components/Removal.hpp"
#include "Annies/Components/Conversion.hpp"
#include "Annies/Components/Comparison.hpp"
#include "Annies/Components/Iteration-ForEach.hpp"
#include "Annies/Components/Iteration-Range.hpp"
#include "Annies/Components/State-Stack.hpp"
#include "Annies/States/Disowned.hpp"
#include "Annies/States/Future.hpp"
#include "Annies/States/Past.hpp"
#include "Annies/States/Compressed.hpp"
#include "Annies/States/Encrypted.hpp"
#include "Annies/States/Or.hpp"
#include "Annies/States/Tracked.hpp"


namespace Langulus::Annies::Inner
{
   using TagBase = Com::Container<
      Com::State::Disowned<>,          // Allows disownment             
      Com::TypedStack<DMeta>,          // Type-erased                   
      Com::TaggedStack<TMeta>,         // Tag-erased                    
      Com::HeapMovable<>,              // Pointer to heap memory        
      Com::CountStack<>,               // Dynamically sized             
      Com::ReserveStack<>,             // Reserve kept as member        
      Com::IndexedLinear<>,            // Indexed directly              
      Com::OwnershipStack<>,           // Allocation is referenced      
      Com::OwnershipDeepHeap<>,        // Sparse elements are referenced
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
      Com::State::Encrypted<>          // Toggle encryption             
   >;
}

namespace Langulus::Annies
{
   ///                                                                        
   ///   This is the same as Many, but also carries tag information as a      
   /// member. Binary-compatible with its templated equivalent TTag.          
   struct Tag : Inner::TagBase {
      using CTTI_ReflectAs = Tag;
      using CTTI_Deep      = No;
      using Base           = Inner::TagBase;

      constexpr Tag() noexcept {
         this->ConstructDefault();
      }
      constexpr Tag(Tag const& other) {
         this->Absorb(Refer(other));
      }
      constexpr Tag(Tag&& other) noexcept  {
         this->Absorb(Move(other));
      }
      constexpr ~Tag() noexcept {
         this->Destroy();
      }

      /// Construction that either absorbs the provided containers, or        
      /// emplaces all A in the container                                     
      template<NotTag A1, class...AN>
      constexpr Tag(A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0) {
            if constexpr (CT::DeepDense<Deint<A1>>) {
               LglsAssumeUser((Same<Deint<A1>, Tag>),
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
      
      /// Construction that absorbs the provided containers                   
      template<class A1, class...AN>
      constexpr Tag(Inner::Absorb, A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0)
            this->Absorb(LglsFwd(a1));
         else {
            this->ConstructDefault();
            this->Concat(LglsFwd(a1), LglsFwd(an)...);
         }
      }
      
      /// Construction that emplaces all arguments inside                     
      template<class A1, class...AN>
      constexpr Tag(Inner::Piecewise, A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0)
            this->EmplaceConstruct(LglsFwd(a1));
         else {
            this->ConstructDefault();
            this->Insert(LglsFwd(a1), LglsFwd(an)...);
         }
      }
      
      /// Assignment                                                          
      constexpr Tag& operator = (Tag const& other) {
         return this->AssignAbsorb(Refer(other));
      }
      constexpr Tag& operator = (Tag&& other) noexcept {
         return this->AssignAbsorb(Move(other));
      }
      
      template<class A>
      constexpr Tag& operator = (A&& argument) {
         if constexpr (CT::DeepDense<Deint<A>>) {
            LglsAssumeUser((Same<Deint<A>, Tag>),
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