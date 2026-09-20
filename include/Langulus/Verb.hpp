///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Many.hpp"
#include "Annies/Components/Charged-Stack.hpp"
#include "Annies/Components/Verbed-Stack.hpp"
#include <compare>


namespace Langulus::Flow
{
   struct Code;
}

namespace Langulus::Annies::Inner
{
   /// Verbs extend the usual type-erased Any, by adding charge and verb ID   
   /// as members.                                                            
   using VerbBase = typename ManyBase::template Include<
      Com::VerbedStack<VMeta>,     // Add verb, make executable         
      Com::ChargedStack<>          // Add charge                        
   >;
}

namespace Langulus::Annies
{
   ///                                                                        
   /// MARK: Verb                                                             
   /// A type-erased container specifically designed for fully capsulating    
   /// function calls.                                                        
   struct Verb : Inner::VerbBase {
   private:
      // The number of successful executions                            
      size_t successes = 0;

   public:
      // Verb context                                                   
      Many source;
      // The container where output goes after execution                
      Many output;

      using CTTI_Members = Members<&Verb::source, &Verb::output>;

   public:
      /// MARK: Construct                                                     
      constexpr Verb() noexcept = default;
      Verb(const Verb&);
      Verb(Verb&&);

      template<template<class> class S> requires CT::Intent<S<Verb>>
      Verb(S<Verb>&&);

      ~Verb() = default;

      /// MARK: Assign                                                        
      Verb& operator = (const Verb&);
      Verb& operator = (Verb&&);

      template<template<class> class S> requires CT::Intent<S<Verb>>
      Verb& operator = (S<Verb>&&);

      /// MARK: Access                                                        
      auto GetHash() const -> Hash;
      auto GetOperatorToken(bool& tokenized) const -> Text;

      auto GetSource()       noexcept -> Many&;
      auto GetSource() const noexcept -> Many const&;

      auto GetArgument()       noexcept -> Many&;
      auto GetArgument() const noexcept -> Many const&;

      auto GetOutput()       noexcept -> Many&;
      auto GetOutput() const noexcept -> Many const&;

      auto operator -> ()       noexcept -> Many*;
      auto operator -> () const noexcept -> Many const*;
      
      auto GetSuccesses() const noexcept -> size_t;
      bool IsDone() const noexcept;

      bool IsMissing() const noexcept;
      bool IsMissingDeep() const noexcept;
      bool Validate(CT::Index auto&&) const noexcept;

      void Done(size_t) noexcept;
      void Done() noexcept;
      void Undo() noexcept;

      /// MARK: Compare                                                       
      bool operator ==  (const Verb&) const;
      auto operator <=> (const Verb&) const -> std::partial_ordering;

      /// MARK: Removal                                                       
      void Reset();
   };
}

namespace Langulus
{
   using Annies::Verb;
}

LANGULUS_MORPHISM(Langulus::Annies::Verb, Langulus::Annies::Text, Langulus::Flow::Code);