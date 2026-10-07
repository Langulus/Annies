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
      using CTTI_Tagged    = Types<TAGS...>;
      using Base           = Inner::TPinBase<T>;

      constexpr  TPin() noexcept {
         this->ConstructDefault();
      }

      constexpr  TPin(T const& source)
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

      operator bool() = delete("Boolean conversion not allowed here - too error prone");
   };

   template<CT::NotVoid T>
   using Pin = TPin<T>;
}

namespace Langulus
{
   using Annies::Pin;
}