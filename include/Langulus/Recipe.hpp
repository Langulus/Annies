///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include <Langulus/Many.hpp>
#include <Langulus/Annies/Components/Stack.hpp>
#include <Langulus/Annies/Components/Charged-Stack.hpp>


namespace Langulus::Annies::Inner
{
   /// Recipe extends the usual type-erased Many, by adding charge and        
   /// type as members.                                                       
   using RecipeBase = typename ManyBase::template Include<
      Com::Stack<DMeta, 1>,         // Add the constructed type         
      Com::ChargedStack<>           // Add charge                       
   >;
}

namespace Langulus::Annies
{
   ///                                                                        
   ///   Recipe                                                               
   ///                                                                        
   ///   Used to contain creation instructions for any type. It is just a     
   /// type-erased Many - a descriptor - with a charge and a type.            
   /// Optimized for detecting small differences in descriptions.             
   /// These scripts mean the completely same thing:                          
   /// 1) create*2(input(name("test")) output(Thing))                         
   /// 2) create*2 Thing(name("test"))      // memoized as Recipe             
   /// 3) create Thing*2(name("test"))      // memoized as Recipe             
   /// 4) create Thing*2("test")            // memoized as Recipe             
   /// 5) create Recipe*2(type(Thing), input(name("test")))                   
   /// 6) create*2 Recipe(Thing, name("test"))                                
   /// After execution all these are replaced by the two created Things       
   struct Recipe : Inner::RecipeBase {
      /*constexpr Construct() noexcept = default;
      Construct(const Construct&) noexcept;
      Construct(Construct&&) noexcept;

      template<template<class> class S> requires CT::Intent<S<Construct>>
      Construct(S<Construct>&&);

      Construct(DMeta);
      Construct(DMeta, auto&&, const Charge& = {});

      #if LANGULUS_FEATURE(MANAGED_REFLECTION)
         Construct(const Token&);
         Construct(const Token&, auto&&, const Charge& = {});
      #endif

      Construct& operator = (const Construct&) noexcept;
      Construct& operator = (Construct&&) noexcept;
      template<template<class> class S> requires CT::Intent<S<Construct>>
      Construct& operator = (S<Construct>&&);

   public:
      Hash GetHash() const;

      template<CT::NotVoid, CT::NotVoid T1, CT::NotVoid...TN>
      static Construct From(T1&&, TN&&...);
      template<CT::NotVoid>
      static Construct From();

      #if LANGULUS_FEATURE(MANAGED_REFLECTION)
         template<CT::NotVoid T1, CT::NotVoid...TN>
         static Construct FromToken(const Token&, T1&&, TN&&...);
         static Construct FromToken(const Token&);
      #endif

      // Intentionally undefined, because it requires Langulus::Flow    
      // and relies on Verbs::Create                                    
      bool StaticCreation(Many&) const;

   public:
      bool operator == (const Construct&) const;

      template<CT::NotVoid>
      bool CastsTo() const;
      bool CastsTo(DMeta) const;

      template<CT::NotVoid>
      bool Is() const;
      bool Is(DMeta) const;

      template<CT::NotVoid>
      void SetType();
      void SetType(DMeta) noexcept;

      auto GetDescriptor() const noexcept -> Many const&;
      auto GetDescriptor()       noexcept -> Many&;
      auto GetCharge() const noexcept -> Charge const&;
      auto GetCharge()       noexcept -> Charge&;

      DMeta GetType() const noexcept;
      Token GetToken() const noexcept;
      DMeta GetProducer() const noexcept;
      bool  IsExecutable() const noexcept;
      bool  IsTyped() const noexcept;
      bool  IsUntyped() const noexcept;

      void Clear();
      void Reset();
      void ResetCharge() noexcept;

      auto operator -> () const -> const Many*;
      auto operator -> ()       ->       Many*;

      Construct& operator <<  (auto&&);
      Construct& operator <<= (auto&&);*/

      ///                                                                     
      ///   Conversion                                                        
      ///                                                                     
      //size_t Serialize(CT::Serial auto&) const;
   };
}

namespace Langulus
{
   using Annies::Recipe;
}