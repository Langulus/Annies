///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Many.hpp"
#include "Annies/Components/Tagged-Stack.hpp"


namespace Langulus::Annies::Inner
{
   /// Verbs extend the usual type-erased Any, by adding charge and verb ID   
   /// as members.                                                            
   using TagBase = typename ManyBase::template Include<
      Com::TaggedStack<TMeta>      // Add tag                           
   >;
}

namespace Langulus::Annies
{
   ///                                                                        
   ///   This is the same as Many, but also carries tag information as a      
   /// member. Binary-compatible with its templated equivalent TTag.          
   struct Tag : Inner::TagBase {
      using CTTI_Tag  = Yup;
      using Base      = Inner::TagBase;

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
            if constexpr (CT::DeepDense<Deint<A1>> or CT::Tag<A1>) {
               /**LglsAssumeUser((Same<Deint<A1>, Tag>),
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
         if constexpr (CT::DeepDense<Deint<A>> or CT::Tag<A>) {
            /*LglsAssumeUser((Same<Deint<A>, Tag>),
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
   };
}

namespace Langulus
{
   using Annies::Tag;
}