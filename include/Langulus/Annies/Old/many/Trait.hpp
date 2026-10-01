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
      /// An abstract Tag structure                                         
      /// It defines the size for CT::Tag and CT::TraitBased concepts       
      ///                                                                     
      struct Tag : Annies::Many {
         using CTTI_Abstract = Yup;
         using CTTI_Deep = No;
         using CTTI_ReflectAs = A::Tag;
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

      /// A TraitBased type is any type that inherits A::Tag                
      template<class...T>
      concept TraitBased = (DerivedFrom<T, A::Tag> and ...);

      /// A reflected trait type is any type that inherits Tag, is not      
      /// Tag itself, and is binary compatible to a Tag                   
      template<class...T>
      concept Tag = TraitBased<T...> and ((
            sizeof(T) == sizeof(A::Tag)
            and requires { {Decay<T>::CTTI_Trait} -> Same<Token>; }
         ) and ...);

   } // namespace Langulus::CT

} // namespace Langulus

namespace Langulus::Annies
{

   ///                                                                        
   ///   Tag                                                                
   ///                                                                        
   ///   A named container, used to give containers a standard intent of use  
   ///   A count is a count, no matter how you call it. So when your type     
   /// contains a count variable, you can tag it with a Traits::size_t tag     
   ///   Traits are used to access members of objects at runtime, or access   
   /// global objects, or supply paremeters                                   
   ///                                                                        
   struct Tag : A::Tag {
      using CTTI_Named = Yes<"Tag">;
      using CTTI_Abstract = No;
      using CTTI_ReflectAs = Tag;
      using CTTI_Bases = A::Tag;

      ///                                                                     
      ///   Construction & Assignment                                         
      ///                                                                     
      constexpr Tag() noexcept = default;
      Tag(const Tag&);
      Tag(Tag&&) noexcept;

      template<class T1, class...TN> //requires CT::UnfoldInsertable<T1, TN...>
      Tag(T1&&, TN&&...);

      Tag& operator = (const Tag&);
      Tag& operator = (Tag&&);
      Tag& operator = (CT::Intent auto&&);

      template<CT::Tag, CT::NotVoid>
      static Tag From();
      static Tag FromMeta(TMeta, DMeta);

      template<CT::Tag>
      static Tag From(auto&&);
      static Tag From(TMeta, auto&&);

      ///                                                                     
      ///   Capsulation                                                       
      ///                                                                     
      template<CT::Tag>
      void SetTrait() noexcept;
      void SetTrait(TMeta) noexcept;

      template<CT::Tag, CT::TraitBased = Tag>
      constexpr bool IsTrait() const;

      template<CT::TraitBased = Tag, class...TN> requires Exact<TMeta, TMeta, TN...>
      bool IsTrait(TMeta, TN...) const;

      template<CT::TraitBased = Tag>
      TMeta GetTrait() const noexcept;

      template<CT::TraitBased = Tag>
      bool IsTraitValid() const noexcept;

      template<CT::TraitBased = Tag>
      bool IsTraitSimilar(const CT::TraitBased auto&) const noexcept;

      template<CT::TraitBased = Tag>
      bool HasCorrectData() const;

      ///                                                                     
      ///   Indexing                                                          
      ///                                                                     
      Tag Select(size_t, size_t)       IF_UNSAFE(noexcept);
      Tag Select(size_t, size_t) const IF_UNSAFE(noexcept);

      ///                                                                     
      ///   Compare                                                           
      ///                                                                     
      template<CT::TraitBased = Tag, CT::NoIntent T> requires CT::NotOwned<T>
      bool operator == (const T&) const;

      ///                                                                     
      ///   Concatenation                                                     
      ///                                                                     
      template<CT::TraitBased THIS = Tag>
      THIS operator + (CT::UnfoldInsertable auto&&) const;

      template<CT::TraitBased THIS = Tag>
      THIS& operator += (CT::UnfoldInsertable auto&&);

      ///                                                                     
      ///   Conversion                                                        
      ///                                                                     
      template<CT::TraitBased = Tag>
      size_t Serialize(CT::Serial auto&) const;
   };

} // namespace Langulus::Annies