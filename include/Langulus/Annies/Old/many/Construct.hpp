///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
//#include "Many.hpp"
#include "../Charge.hpp"
#include <Langulus/Many.hpp>

namespace Langulus::Annies
{

   ///                                                                        
   ///   Construct                                                            
   ///                                                                        
   ///   Used to contain constructor arguments for any type. It is just a     
   /// type-erased Many, but also carries a charge and a type. It is often    
   /// used in Verbs::Create to provide instructions on how to instantiate a  
   /// data type.                                                             
   ///                                                                        
   struct Construct {
   private:
      // What are we constructing?                                      
      DMeta  mType {};
      // Precomputed hash                                               
      mutable Hash mHash {};
      // How many things, when, at what frequency/priority?             
      Charge mCharge;
      // What properties does the thing have?                           
      Many   mDescriptor;

   public:
      static constexpr bool Ownership = true;

      constexpr Construct() noexcept = default;
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
      Construct& operator <<= (auto&&);

      ///                                                                     
      ///   Conversion                                                        
      ///                                                                     
      size_t Serialize(CT::Serial auto&) const;
   };

} // namespace Langulus::Annies
