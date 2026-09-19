///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
//#include "../many/Many.hpp"
//#include "../Index.hpp"
//#include "../Charge.hpp"
#include "VerbState.hpp"
#include <Langulus/Many.hpp>
#include <Langulus/Annies/Components/Charge-Stack.hpp>


namespace Langulus::A
{

   ///                                                                        
   /// MARK: Verb                                                             
   /// A type-erased container specifically designed for fully capsulating    
   /// function calls.                                                        
   struct Verb : Annies::Many, Annies::Components::Charge {
      using CTTI_POD = No;
      using CTTI_Nullable = No;
      using CTTI_Deep = No;
      using CTTI_ReflectAs = Verb;
      using CTTI_Bases = Types<Annies::Many, Annies::Charge>;
      static constexpr bool CTTI_Container = true;

   protected:
      using Real      = Langulus::Real;
      using Charge    = Langulus::Charge;
      using VMeta     = Annies::VMeta;
      using VerbState = Annies::VerbState;
      using Many      = Annies::Many;
      using Text      = Annies::Text;

      // Verb meta, mass, rate, time and priority                       
      mutable VMeta mVerb {};
      // The number of successful executions                            
      size_t mSuccesses {};
      // Verb short-circuiting                                          
      VerbState mState {};
      // Verb context                                                   
      Many mSource;
      // The container where output goes after execution                
      Many mOutput;

      using CTTI_Members = Members<&Verb::mVerb,
                                   &Verb::mState,
                                   &Verb::mSource>;

   public:
      ///                                                                     
      ///   Construction & Assignment                                         
      ///                                                                     
      constexpr Verb() noexcept = default;
      Verb(const Verb&);
      Verb(Verb&&);

      template<template<class> class S> requires CT::Intent<S<Verb>>
      Verb(S<Verb>&&);

      ~Verb() = default;

      Verb& operator = (const Verb&);
      Verb& operator = (Verb&&);

      template<template<class> class S> requires CT::Intent<S<Verb>>
      Verb& operator = (S<Verb>&&);

      ///                                                                     
      ///   Capsulation                                                       
      ///                                                                     
      auto GetVerb() const noexcept -> VMeta;
      auto GetHash() const -> Hash;
      auto GetCharge() const noexcept -> const Charge&;
      auto GetMass() const noexcept -> Real;
      auto GetRate() const noexcept -> Real;
      auto GetTime() const noexcept -> Real;
      auto GetPriority() const noexcept -> Real;
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
      auto GetVerbState() const noexcept -> VerbState;
      bool IsDone() const noexcept;

      /*constexpr bool IsMulticast() const noexcept;
      constexpr bool IsMonocast() const noexcept;*/
      constexpr bool IsShortCircuited() const noexcept;
      constexpr bool IsLongCircuited() const noexcept;

      bool IsMissing() const noexcept;
      bool IsMissingDeep() const noexcept;
      bool Validate(Annies::Index) const noexcept;

      void Done(size_t) noexcept;
      void Done() noexcept;
      void Undo() noexcept;

      explicit operator Annies::Text() const;

   protected:
      void SerializeVerb(CT::Serial auto&) const;

   public:
      ///                                                                     
      ///   Comparison                                                        
      ///                                                                     
      bool operator == (const Verb&) const;
      bool operator == (VMeta) const noexcept;

      bool operator <  (const Verb&) const noexcept;
      bool operator >  (const Verb&) const noexcept;

      bool operator <= (const Verb&) const noexcept;
      bool operator >= (const Verb&) const noexcept;

      ///                                                                     
      ///   Removal                                                           
      ///                                                                     
      void Reset();
   };

} // namespace Langulus::A

LANGULUS_MORPHISM(Langulus::A::Verb, Langulus::Annies::Text);

namespace Langulus::CT
{
   
   /// A VerbBased type is any type that inherits A::Verb                     
   template<class...T>
   concept VerbBased = (DerivedFrom<T, A::Verb> and ...);

   /// A reflected verb type is any type that inherits A::Verb, is binary     
   /// compatible to it, and is reflected as a verb                           
   template<class...T>
   concept Verb = VerbBased<T...> and ((
      sizeof(T) == sizeof(A::Verb) and (
         requires {
            {Decay<T>::CTTI_Verb} -> Same<Token>;
         } or requires {
            {Decay<T>::CTTI_PositiveVerb} -> Same<Token>;
            {Decay<T>::CTTI_NegativeVerb} -> Same<Token>;
         }
      )) and ...);

   /// Concept for recognizing arguments, with which a verb can be constructed
   template<class T1, class...TN>
   concept VerbMakable = UnfoldInsertable<T1, TN...>
        or (sizeof...(TN) == 0 and VerbBased<Deint<T1>>);

   /// Concept for recognizing argument, with which a verb can be assigned    
   template<class A>
   concept VerbAssignable = VerbMakable<A>;

} // namespace Langulus::CT