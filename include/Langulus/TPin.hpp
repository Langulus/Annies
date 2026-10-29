///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Annies/Container.hpp"
#include "Annies/Components/Typed-Static.hpp"
#include "Annies/Components/Stack.hpp"
#include "Annies/Components/Count-Static.hpp"
#include "Annies/Components/Emplacement.hpp"
#include "Annies/Components/Assignment.hpp"
#include "Annies/Components/Comparison.hpp"
#include "Annies/States/Pinned.hpp"


namespace Langulus::Annies::Inner
{
   template<CT::NotVoid T>
   using TPinBase = Com::Container<
      Com::State::Pinned<>,               // Allows pinning             
      Com::TypedStatic<DMeta, T>,         // Statically typed           
      Com::Stack<T>,                      // Element on the stack       
      Com::CountStatic<1u>,               // Statically sized           
      Com::Emplacement<>,                 // Can be emplaced            
      Com::Assignment<>,                  // Can be reassigned          
      Com::Comparison<>                   // Can be compared            
   >;
}

namespace Langulus::Annies
{
   ///                                                                        
   /// A statically typed stack-based container of size 1.                    
   /// Mainly serves to transfer values and/or pointers on move.              
   /// Allows for pinning, which disables overwrite on assignment.            
   /// Especially useful for members that are part of a hierarchy and can     
   /// be overridden by things from upper in the hierarchy.                   
   /// You can optionally add tags to it.                                     
   template<CT::NotVoid T, class...TAGS>
   struct TPin : Inner::TPinBase<T> {
      using CTTI_ReflectAs = TPin;
      using CTTI_Deep      = Yup;
      using CTTI_POD       = Maybe<CT::POD<T>>;
      using CTTI_Tagged    = Types<TAGS...>;
      using Base           = Inner::TPinBase<T>;

      constexpr  TPin() noexcept {
         this->ConstructDefault();
      }

      constexpr  TPin(TPin const& other) {
         this->Absorb(Refer(other));
      }

      constexpr  TPin(TPin&& other) noexcept {
         this->Absorb(Move(other));
      }

      /// Construction that either absorbs the provided container, or         
      /// emplaces T in the container, using A... as constructor arguments    
      template<class...A>
      constexpr TPin(A&&...arguments) {
         if constexpr (sizeof...(A) == 1 and CT::ContainsOne<A...>) {
            LglsAssumeUser((Same<Deint<A>, TPin> and ...),
               "Ambiguous use of construction "
               "- you should use tag-dispatch with first argument either Absorb "
               "(if you want to overwrite the container itself) or Piecewise "
               "(if you want to overwrite the first item) in order to clearly "
               "state your intent. Absorb will be used by default!"
            );
            this->Absorb(LglsFwd(arguments)...);
         }
         else this->EmplaceConstruct(LglsFwd(arguments)...);
      }
      
      /// Construction that absorbs the provided container                    
      template<class A>
      constexpr TPin(Inner::Absorb, A&& argument) {
         this->Absorb(LglsFwd(argument));
      }
      
      /// Emplaces T inside, using A... as constructor arguments              
      template<class...A>
      constexpr TPin(Inner::Piecewise, A&&...arguments) {
         this->EmplaceConstruct(LglsFwd(arguments)...);
      }

      constexpr ~TPin() noexcept {
         this->Destroy();
      }

      /// Assignment                                                          
      constexpr TPin& operator = (TPin const& other) {
         return this->AssignAbsorb(Refer(other));
      }
      constexpr TPin& operator = (TPin&& other) noexcept {
         return this->AssignAbsorb(Move(other));
      }

      template<class A>
      constexpr TPin& operator = (A&& argument) {
         if constexpr (CT::ContainsOne<A>) {
            LglsAssumeUser(Same<Deint<A>, TPin>,
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

      /// Three-way comparison                                                
      constexpr auto operator <=> (const TPin& rhs) const noexcept {
         return Base::GetStackInner() <=> rhs.GetStackInner();
      }

      friend constexpr auto operator <=> (const TPin& lhs, T const& rhs) noexcept {
         return lhs.GetStackInner() <=> rhs;
      }

      friend constexpr auto operator <=> (T const& lhs, const TPin& rhs) noexcept {
         return lhs <=> rhs.GetStackInner();
      }

      /// Equality comparison                                                 
      constexpr bool operator == (const TPin& rhs) const noexcept {
         return Base::GetStackInner() == rhs.GetStackInner();
      }

      friend constexpr bool operator == (const TPin& lhs, T const& rhs) noexcept {
         return lhs.GetStackInner() == rhs;
      }

      friend constexpr bool operator == (T const& lhs, const TPin& rhs) noexcept {
         return lhs == rhs.GetStackInner();
      }

      explicit constexpr operator bool() const {
         return static_cast<bool>(Base::GetStackInner());
      }

      /// Access the first element inside the container                       
      constexpr auto& operator -> (this auto&& self) assumptious
      requires ::std::is_class_v<Decay<T>> {
         return self.GetStackInner();
      }

      /// Forward New call to the contained type if available                 
      constexpr decltype(auto) New(auto&&...args)
      requires requires { Base::GetStackInner().New(LglsFwd(args)...); } {
         return Base::GetStackInner().New(LglsFwd(args)...);
      }
   };

   template<CT::NotVoid T>
   using Pin = TPin<T>;
}

namespace Langulus
{
   using Annies::Pin;
}