///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Verb.hpp"


namespace Langulus::Annies::Inner
{
   /// TVerbs extend the usual type-erased Any, by adding charge and verb ID  
   /// as members.                                                            
   template<CT::DefineVerb V>
   using TVerbBase = typename ManyBase::template Include<
      Com::VerbedStack<VMeta, V>,  // Add verb, make executable         
      Com::ChargedStack<>          // Add charge                        
   >;
}

namespace Langulus::Annies
{
   ///                                                                        
   /// MARK: TVerb                                                            
   /// A type-erased container specifically designed for fully capsulating    
   /// function calls. This one is verb-constrained, i.e. the verb it contains
   /// is known at compile-time.                                              
   template<CT::DefineVerb V>
   struct TVerb : Inner::TVerbBase<V> {
      using CTTI_ReflectAs = Verb;

   private:
      // The number of successful executions                            
      size_t successes = 0;

   public:
      // Verb context                                                   
      Many context;
      // The container where output goes after execution                
      Many output;

      using CTTI_Members = Members<&TVerb::context, &TVerb::output>;

      /// MARK: Construct                                                     
      constexpr TVerb() noexcept = default;
      TVerb(const TVerb&);
      TVerb(TVerb&&);

      template<template<class> class S> requires CT::Intent<S<TVerb>>
      TVerb(S<TVerb>&&);

      ~TVerb() = default;

      /// MARK: Assign                                                        
      TVerb& operator = (const TVerb&);
      TVerb& operator = (TVerb&&);

      template<template<class> class S> requires CT::Intent<S<TVerb>>
      TVerb& operator = (S<TVerb>&&);

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
      bool operator ==  (const TVerb&) const;
      auto operator <=> (const TVerb&) const -> std::partial_ordering;

      /// MARK: Removal                                                       
      void Reset();
   };
}

namespace Langulus
{
   using Annies::TVerb;
}

//LANGULUS_MORPHISM(Langulus::Annies::TVerb, Langulus::Annies::Text, Langulus::Flow::Code);

/// Define a verb                                                             
///   @param P - positive verb name, as it exists in namespace Langulus::Verbs
///   @param N - negative verb name (optional, same as positive if "")        
///   @param INFOSTRING - information about the trait's purpose               
#define LANGULUS_DEFINE_VERB(P, N, INFOSTRING) \
   namespace Langulus::Verbs { struct P; } \
   namespace Langulus::CTTI  { template<> struct DefineVerb<::Langulus::Verbs::P> : NamedVerb<#P,#N> {}; } \
   namespace Langulus::Verbs { \
      struct P : Annies::TVerb<P> { \
         using CTTI_Info = Yes<INFOSTRING>; \
      }; \
   }