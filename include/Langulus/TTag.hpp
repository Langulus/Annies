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
   template<class TAG>
   using TTagBase = typename ManyBase::template Include<
      Com::TaggedStack<TMeta, TAG>  // Add tag                          
   >;
}

namespace Langulus::Annies
{
   /// MARK: TTag                                                             
   ///                                                                        
   /// A statically-tagged and statically-tagged equivalent of Tag            
   template<class TAG> 
   struct TTag : Inner::TTagBase<TAG> {
      using CTTI_ReflectAs = Tag;
      using CTTI_Tag       = Yup;
      using Base           = Inner::TTagBase<TAG>;

      constexpr TTag() noexcept {
         this->ConstructDefault();
      }
      constexpr TTag(TTag const& other) {
         this->Absorb(Refer(other));
      }
      constexpr TTag(TTag&& other) noexcept  {
         this->Absorb(Move(other));
      }
      constexpr ~TTag() noexcept {
         this->Destroy();
      }

      /// Construction that either absorbs the provided containers, or        
      /// emplaces all A in the container                                     
      template<Disambiguate A1, class...AN>
      constexpr TTag(A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0) {
            if constexpr (CT::DeepDense<Deint<A1>> or CT::Tag<A1>) {
               LglsAssumeUser((SameAsOneOf<Deint<A1>, TTag, Tag>),
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
         if constexpr (CT::DeepDense<Deint<A>> or CT::Tag<A>) {
            LglsAssumeUser(SameAsOneOf<Deint<A>, TTag, Tag>,
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
   using Annies::TTag;
   using Annies::Tag;
}

/// Define a tag                                                              
///   @param T the tag, as it appears in namespace Langulus::Tags             
///   @param INFOSTRING information about the tag's purpose                   
#define LANGULUS_DEFINE_TAG(T, INFOSTRING, ...) \
   namespace Langulus::Tags { struct T; } \
   namespace Langulus::CTTI { template<> struct DefineTag<::Langulus::Tags::T> : NamedTag<#T> {}; } \
   namespace Langulus::Tags { \
      struct T : Annies::TTag<T> { \
         using CTTI_Info = Yes<INFOSTRING>; \
         __VA_ARGS__; \
      }; \
   }