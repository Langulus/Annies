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
      const Many& what;

      using CTTI_ReflectAs     = void;
      using CTTI_Abstract      = Yup;
      using CTTI_Allocatable   = No;
      using CTTI_Intent        = Yup;

      Describe() = delete;
      constexpr Describe(const Describe&) noexcept = default;
      explicit constexpr Describe(Describe&&) noexcept = default;

      explicit constexpr Describe(const Many& descriptor) noexcept
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
      bool ExtractTagInnerInner(D& value) {
         bool satisfied = false;
         size_t counter = 0;

         what.ForEachDeep([&](const TAG& tag) {
            if (counter < IDX) {
               // We're only interested in the Nth trait                
               ++counter;
               return Loop::Continue;
            }

            if constexpr (CT::Deep<D>) {
               value = tag.GetData();
               satisfied = true;
            }
            else if (tag.ExtractDataAs(value))
               satisfied = true;

            return Loop::Break;
         });

         return satisfied;
      }

      ///                                                                     
      template<CT::DefineTag TAG, size_t...IDX>
      bool ExtractTagInner(ExpandedSequence<IDX...>, auto&...values) {
         return (ExtractTagInnerInner<TAG, IDX>(values) or ...);
      }

      ///                                                                     
      template<CT::DefineTag TAG>
      bool ExtractTagInner(auto&...values) {
         return ExtractTagInner<TAG>( 
            Sequence<sizeof...(values)>::Expand, values...
         );
      }
   };


   /// Set a default tag, if such wasn't already set. Analogous to how        
   /// CMake set() works - if tag was already cached, it won't be             
   /// overwritten unless you force it.                                       
   ///   @tparam TAG the tag to set                                           
   ///   @param value the value to assign                                     
   ///   @param force whether to set tag even if it already exists            
   template<CT::DefineTag TAG>
   void Describe::Set(auto&& value, bool force) {
      bool satisfied = false;
      what.ForEachDeep([&](TAG& tag) {
         if (tag) {
            // Tag has already been set, no point in searching more     
            if (force)
               tag = LglsFwd(value);
            satisfied = true;
            return Loop::Break;
         }
         else return Loop::Continue;
      });

      if (satisfied)
         return;

      // No Neat was found, so just push one containing the tag         
      what.Compose(TAG {LglsFwd(value)}); //TODO neat should implement compose/insert in its own way
   }

   /// Extract a tag from the descriptor                                      
   ///   @tparam TAG... tags we're searching for                              
   ///   @param values [out] where to save the value, if found                
   ///   @return true if any of the values changed                            
   template<CT::DefineTag...TAG>
   bool Describe::ExtractTag(auto&...values) const {
      bool satisfied = false;
      ((satisfied |= ExtractTagInner<TAG>(values)), ...);
      return satisfied;
   }

   /// Extract data capable of initializing D. Pointer arithmetic allowed.    
   ///   @param value [out] where to save the value(s), if found              
   ///   @return the number of extracted values (always 1 if not an array)    
   template<class D>
   auto Describe::ExtractData(D& value) const -> size_t {
      size_t progress = 0;

      if constexpr (CT::Array<D>) {
         what.ForEachDeep([&](const TypeOf<D>& data) {
            //TODO can be optimized-out for POD
            value[progress] = data;
            ++progress;
            return (progress >= ExtentOf<D>) ? Loop::Break : Loop::Continue;
         });
      }
      else {
         what.ForEachDeep([&](const D& data) {
            value = data;
            ++progress;
            return Loop::Break;
         });
      }

      return progress;
   }

   /// Extract any data that is convertible to D                              
   ///   @param value - [out] where to save the value, if found               
   ///   @return the number of extracted values (always 1 if not an array)    
   auto Describe::ExtractDataAs(auto& value) const -> size_t {
      using D = Deref<decltype(value)>;
      size_t progress = 0;

      what.ForEachDeep([&](const Many& group) {
         if constexpr (CT::Array<D>) {
            const auto count  = group.GetCount();
            const auto remain = ExtentOf<D> - progress;
            const auto toscan = remain < count ? remain : count;
            for (size_t i = 0; i < toscan; ++i) {
               //TODO can be optimized-out for POD
               try {
                  value[progress] = group.template As<TypeOf<D>>(i);
                  ++progress;
               }
               catch (...) {}
            }

            return (progress >= ExtentOf<D>) ? Loop::Break : Loop::Continue;
         }
         else {
            try {
               value = group.template As<D>();
               ++progress;
               return Loop::Break;
            }
            catch (...) {}
            return Loop::Continue;
         }
      });

      return progress;
   }

   /// Find data in recipes or tail. Only pointer arithmetic casts are        
   /// allowed, such as inheritance.                                          
   ///   @tparam T the type requirement                                       
   ///   @return the first type that matches                                  
   template<CT::NotVoid T>
   auto Describe::FindType() const -> RTTI::DMeta {
      return FindType(MetaDataOf<T>());
   }

   /// Find data in recipes or tail. Only pointer arithmetic casts are        
   /// allowed, such as inheritance.                                          
   ///   @tparam type the type requirement                                    
   ///   @return the first type that matches                                  
   template<class TYPE>
   auto Describe::FindType(RTTI::DMeta type) const -> RTTI::DMeta {
      bool ambiguous = false;
      RTTI::DMeta found;

      what.ForEachDeep([&](const Many& group) noexcept {
         group.ForEach([&](const Recipe& recipe) noexcept {
            if (not recipe.CastsTo(type))
               return;

            if (not found) found = recipe.GetType();
            else ambiguous = true;
         });

         if (not group.CastsTo(type))
            return;

         if (not found) found = group.GetType();
         else ambiguous = true;
      });

      if (ambiguous) {
         Logger::Warning(
            "Multiple types found in descriptor - all except the first `",
            found, "` will be ignored on FindType"
         );
      }

      return found;
   }
}
