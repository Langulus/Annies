///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Many.hpp"
#include "Recipe.hpp"


namespace Langulus
{
   ///                                                                        
   /// MARK: Describe                                                         
   ///                                                                        
   ///   Descriptor intermediate type, used in constructors and assignment    
   /// operators to enable describe-construction/assignment. The inner type   
   /// is always a reference to a type-erased container.                      
   ///   Provides services for setting and reading back properties. Often     
   /// used in Describe constructors to set member variables and stuff.       
   struct Describe {
      using Many = Annies::Many;
      Many const& what;

      using CTTI_ReflectAs     = void;
      using CTTI_Abstract      = Yup;
      using CTTI_Allocatable   = No;
      using CTTI_Intent        = Yup;

      Describe() = delete("Contained reference needs to be specified");
      constexpr Describe(const Describe&) noexcept = default;
      explicit constexpr Describe(Describe&&) noexcept = default;

      explicit constexpr Describe(Many const& descriptor) noexcept
         : what {descriptor} {}

      auto& operator *  () const noexcept { return  what; }
      auto* operator -> () const noexcept { return &what; }
      explicit operator bool () const noexcept { return static_cast<bool>(what); }

      template<CT::DefineTag>
      void Set(auto&&, bool force = false);

      template<CT::DefineTag...>
      bool ExtractTag(auto&...) const;
      auto ExtractData(auto&) const -> size_t;
      auto ExtractDataAs(auto&) const -> size_t;

      template<CT::NotVoid>
      auto FindType() const -> RTTI::DMeta;

      template<class TYPE>
      auto FindType(RTTI::DMeta) const -> RTTI::DMeta;

   private:
      ///                                                                     
      template<CT::DefineTag TAG, size_t IDX, class D>
      bool ExtractTagInnerInner(D& value) const {
         bool satisfied = false;
         size_t counter = 0;

         what.ForEachDeep([&](const TAG& tag) {
            if (counter < IDX) {
               // We're only interested in the Nth tag                
               ++counter;
               return Loop::Continue;
            }

            if constexpr (CT::Deep<D>) {
               value = tag.GetData();
               satisfied = true;
            }
            else if (Describe{tag.GetData()}.ExtractDataAs(value))
               satisfied = true;

            return Loop::Break;
         });

         return satisfied;
      }

      ///                                                                     
      template<CT::DefineTag TAG, size_t...IDX>
      bool ExtractTagInner(ExpandedSequence<IDX...>, auto&...values) const {
         return (ExtractTagInnerInner<TAG, IDX>(values) or ...);
      }

      ///                                                                     
      template<CT::DefineTag TAG>
      bool ExtractTagInner(auto&...values) const {
         return ExtractTagInner<TAG>( 
            Sequence<sizeof...(values)>::Expand, values...
         );
      }
   };
}
