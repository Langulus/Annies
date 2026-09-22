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
   template<CT::NotVoid T> requires (CT::NotHandle<T> and CT::NotReference<T>)
   using TPinBase = Com::Container<
      Com::State::Pinned<>,               // Allows disownment          
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
   template<CT::NotVoid T>
   struct TPin : Inner::TPinBase<T> {
      using CTTI_Deep      = Yup;
      using Base           = Inner::TPinBase<T>;

      constexpr  TPin() noexcept {
         this->ConstructDefault();
      }

      constexpr  TPin(const T& source)
         : Base {Stackwise, source} {}

      constexpr  TPin(T&& source) noexcept
         : Base {Stackwise, LglsFwd(source)} {}

      constexpr ~TPin() noexcept = default;

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

      /// Boolean conversion not allowed here, too error prone                
      operator bool() = delete;
   };

   template<CT::NotVoid T>
   using Pin = TPin<T>;
}

namespace Langulus
{
   using Annies::Pin;
}