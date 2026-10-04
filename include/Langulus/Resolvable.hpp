///                                                                           
/// Langulus::Flow                                                            
/// Copyright (c) 2017 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include <Langulus/Handle.hpp>
#include <Langulus/CT/Resolvable.hpp>


namespace Langulus
{
   ///                                                                        
   /// Gives derived types the ability to be resolved to their derived type   
   /// at runtime through RTTI.                                               
   ///                                                                        
   struct Resolvable {
      using CTTI_Resolvable = Yup;

      template<class T> requires (not ::std::is_same_v<T, Resolvable>)
      auto GetResolved(this T&& self) noexcept -> Annies::HandleDisowned {
         return {Annies::Stackwise, MetaDataOf<T>(), &self};
      }

      /*using CTTI_Abstract   = Yup;

   private:
      // Concrete type of the resolvable                                
      const RTTI::DMeta mClassType;
      // Pointer to an instance of the above type                       
      void* const mClassPointer;

   public:
      Resolvable() = delete;
      Resolvable(const Resolvable&) = delete;
      Resolvable(Resolvable&&) = delete;
      Resolvable& operator = (const Resolvable&) = delete;
      Resolvable& operator = (Resolvable&&) = delete;

      template<CT::Resolvable T>
      Resolvable(const T* p) noexcept
         : mClassType    {MetaDataOf<T>()}
         , mClassPointer {p} {}

      auto GetResolved() const noexcept -> Annies::HandleDisowned {
         return {Annies::Stackwise, mClassType, mClassPointer};
      }*/

      //virtual ~Resolvable() = default;


      /*template<CT::Data>
      bool CastsTo() const;
      bool CastsTo(DMeta) const IF_UNSAFE(noexcept);

      template<CT::Data>
      bool Is() const;
      bool Is(DMeta) const noexcept;

      Token GetToken() const IF_UNSAFE(noexcept);
      DMeta GetType()  const noexcept;
      Block<> GetBlock() const noexcept;

      template<bool DISPATCH = true, bool DEFAULT = true>
      auto& Run(CT::VerbBased auto&&);

      Many Run(Code const&);
      Many Run(Many const&);
      Many Run(const Temporal&);

      Block<> GetMember(TMeta) noexcept;
      Block<> GetMember(TMeta) const noexcept;

      Block<> GetMember(TMeta, CT::Index auto) noexcept;
      Block<> GetMember(TMeta, CT::Index auto) const noexcept;

      #if LANGULUS_FEATURE(MANAGED_MEMORY)
         Block<> GetMember(Token const&) noexcept;
         Block<> GetMember(Token const&) const noexcept;

         Block<> GetMember(Token const&, CT::Index auto) noexcept;
         Block<> GetMember(Token const&, CT::Index auto) const noexcept;
      #endif

      template<CT::Tag>
      bool GetTag(CT::NotVoid auto&) const;
      bool GetValue(CT::NotVoid auto&) const;

      template<CT::Tag, bool DIRECT = false>
      bool SetTrait(CT::NotVoid auto&&);
      template<bool DIRECT = false>
      bool SetValue(CT::NotVoid auto&&);

      // All inheritances of Resolvable will become convertible to Text 
      // and will share the reflected conversions list, but with one    
      // condition: the conversion operator must remain implicit.       
      operator Text() const;

      Text Self() const;*/
   };
}

/*namespace Langulus
{
   Annies::Text IdentityOf(const auto&);
   Annies::Text IdentityOf(Token const&, const auto&);
}
*/