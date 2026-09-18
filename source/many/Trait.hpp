///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Many.hpp"
#include "../one/Own.hpp"


namespace Langulus
{
   namespace A
   {

      ///                                                                     
      /// An abstract Trait structure                                         
      /// It defines the size for CT::Trait and CT::TraitBased concepts       
      ///                                                                     
      struct Trait : Annies::Many {
         using CTTI_Abstract = Yup;
         using CTTI_Deep = No;
         using CTTI_ReflectAs = A::Trait;
         using CTTI_Bases = Annies::Many;

      protected:
         using Base  = Annies::Many;
         using TMeta = Annies::TMeta;

         // The trait tag                                               
         mutable TMeta mTraitType {};
      };

   } // namespace Langulus::A

   namespace CT
   {

      /// A TraitBased type is any type that inherits A::Trait                
      template<class...T>
      concept TraitBased = (DerivedFrom<T, A::Trait> and ...);

      /// A reflected trait type is any type that inherits Trait, is not      
      /// Trait itself, and is binary compatible to a Trait                   
      template<class...T>
      concept Trait = TraitBased<T...> and ((
            sizeof(T) == sizeof(A::Trait)
            and requires { {Decay<T>::CTTI_Trait} -> Same<Token>; }
         ) and ...);

   } // namespace Langulus::CT

} // namespace Langulus

namespace Langulus::Annies
{

   ///                                                                        
   ///   Trait                                                                
   ///                                                                        
   ///   A named container, used to give containers a standard intent of use  
   ///   A count is a count, no matter how you call it. So when your type     
   /// contains a count variable, you can tag it with a Traits::size_t tag     
   ///   Traits are used to access members of objects at runtime, or access   
   /// global objects, or supply paremeters                                   
   ///                                                                        
   struct Trait : A::Trait {
      using CTTI_Named = Yes<"Trait">;
      using CTTI_Abstract = No;
      using CTTI_ReflectAs = Trait;
      using CTTI_Bases = A::Trait;

      ///                                                                     
      ///   Construction & Assignment                                         
      ///                                                                     
      constexpr Trait() noexcept = default;
      Trait(const Trait&);
      Trait(Trait&&) noexcept;

      template<class T1, class...TN> //requires CT::UnfoldInsertable<T1, TN...>
      Trait(T1&&, TN&&...);

      Trait& operator = (const Trait&);
      Trait& operator = (Trait&&);
      Trait& operator = (CT::Intent auto&&);

      template<CT::Trait, CT::NotVoid>
      static Trait From();
      static Trait FromMeta(TMeta, DMeta);

      template<CT::Trait>
      static Trait From(auto&&);
      static Trait From(TMeta, auto&&);

      ///                                                                     
      ///   Capsulation                                                       
      ///                                                                     
      template<CT::Trait>
      void SetTrait() noexcept;
      void SetTrait(TMeta) noexcept;

      template<CT::Trait, CT::TraitBased = Trait>
      constexpr bool IsTrait() const;

      template<CT::TraitBased = Trait, class...TN> requires Exact<TMeta, TMeta, TN...>
      bool IsTrait(TMeta, TN...) const;

      template<CT::TraitBased = Trait>
      TMeta GetTrait() const noexcept;

      template<CT::TraitBased = Trait>
      bool IsTraitValid() const noexcept;

      template<CT::TraitBased = Trait>
      bool IsTraitSimilar(const CT::TraitBased auto&) const noexcept;

      template<CT::TraitBased = Trait>
      bool HasCorrectData() const;

      ///                                                                     
      ///   Indexing                                                          
      ///                                                                     
      Trait Select(size_t, size_t)       IF_UNSAFE(noexcept);
      Trait Select(size_t, size_t) const IF_UNSAFE(noexcept);

      ///                                                                     
      ///   Compare                                                           
      ///                                                                     
      template<CT::TraitBased = Trait, CT::NoIntent T> requires CT::NotOwned<T>
      bool operator == (const T&) const;

      ///                                                                     
      ///   Concatenation                                                     
      ///                                                                     
      template<CT::TraitBased THIS = Trait>
      THIS operator + (CT::UnfoldInsertable auto&&) const;

      template<CT::TraitBased THIS = Trait>
      THIS& operator += (CT::UnfoldInsertable auto&&);

      ///                                                                     
      ///   Conversion                                                        
      ///                                                                     
      template<CT::TraitBased = Trait>
      size_t Serialize(CT::Serial auto&) const;
   };

} // namespace Langulus::Annies