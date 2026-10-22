///                                                                           
/// Langulus::Flow                                                            
/// Copyright (c) 2017 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include <Langulus/Allocator.hpp>
#include <Langulus/CT/Downcastable.hpp>


namespace Langulus
{
   ///                                                                        
   /// Gives base types the ability to be resolved to their derived type      
   /// at runtime through RTTI. I.e. performs a downcast to the most concrete 
   /// type.                                                                  
   struct Downcastable {
   private:
      // Concrete type of the resolvable                                
      const RTTI::DMeta mClassType;
      // Pointer to an instance of the above type                       
      void* const mClassPointer;

   public:
      using CTTI_Downcastable = Yup;

      Downcastable() = delete;
      Downcastable(Downcastable const&) = delete;
      Downcastable(Downcastable&&) = delete;
      Downcastable& operator = (Downcastable const&) = delete;
      Downcastable& operator = (Downcastable&&) = delete;

      /// Instruct how to downcast at construction                            
      template<CT::Downcastable T>
      Downcastable(T const* p) noexcept
         : mClassType    {MetaDataOf<T>()}
         , mClassPointer {const_cast<void*>(static_cast<void const*>(p))} {
         (void) MetaDataOf<T const>(); // Make sure const is reflected  
      }

      /// Perform downcast                                                    
      auto Downcast() noexcept -> RTTI::Block {
         return { mClassPointer, mClassType, 1 };
      }

      auto Downcast() const noexcept -> RTTI::Block {
         return { mClassPointer, mClassType.AddConst(), 1 };
      }

      /// Get the downcasted type                                             
      RTTI::DMeta GetType() const noexcept { return mClassType; }

      /// Test if downcasted type matches another type                        
      template<class T>
      bool Is() const { return mClassType.Is(MetaDataOf<T>()); }
      bool Is(RTTI::DMeta d) const noexcept { return mClassType.Is(d); }

      /// Test if downcasted type casts to another type (using only pointer   
      /// arithmetics)                                                        
      template<class T>
      bool CastsTo() const { return mClassType.CastsTo(MetaDataOf<T>()); }
      bool CastsTo(RTTI::DMeta d) const noexcept { return mClassType.CastsTo(d); }

      /// Get the first member matching a runtime tag definition              
      ///   @param tag the tag to search for                                  
      ///   @return the static mutable memory block representing the member   
      auto GetMember(RTTI::TMeta tag, RTTI::DMeta type, size_t &offset) noexcept -> RTTI::Block {
         return Downcast().GetMember(tag, type, offset);
      }

      auto GetMember(RTTI::TMeta tag, RTTI::DMeta type, size_t &offset) const noexcept -> RTTI::Block {
         return Downcast().GetMember(tag, type, offset);
      }

      auto GetMember(RTTI::TMeta tag) noexcept -> RTTI::Block {
         size_t unused = 0;
         return Downcast().GetMember(tag, {}, unused);
      }

      auto GetMember(RTTI::TMeta tag) const noexcept -> RTTI::Block {
         size_t unused = 0;
         return Downcast().GetMember(tag, {}, unused);
      }

   #if LANGULUS_FEATURE(MANAGED_MEMORY)
      /// Get the first member matching a runtime tag token                   
      ///   @param tag the tag to search for                                  
      ///   @return the static mutable memory block representing the member   
      auto GetMember(Token const& tag) noexcept -> RTTI::Block {
         return GetMember(RTTI::Registry::GetMetaTagByToken(tag));
      }

      auto GetMember(Token const& tag) const noexcept -> RTTI::Block {
         return GetMember(RTTI::Registry::GetMetaTagByToken(tag));
      }
   #endif


      /*
      Token GetToken() const IF_UNSAFE(noexcept);
      DMeta GetType()  const noexcept;
      Block<> GetBlock() const noexcept;

      template<bool DISPATCH = true, bool DEFAULT = true>
      auto& Run(CT::VerbBased auto&&);

      Many Run(Code const&);
      Many Run(Many const&);
      Many Run(Temporal const&);

      template<CT::Tag>
      bool GetTag(CT::NotVoid auto&) const;
      bool GetValue(CT::NotVoid auto&) const;

      template<CT::Tag, bool DIRECT = false>
      bool SetTag(CT::NotVoid auto&&);
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
   Annies::Text IdentityOf(auto const&);
   Annies::Text IdentityOf(Token const&, auto const&);
}
*/