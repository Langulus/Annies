///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Container.hpp"
#include <Langulus/CT/DefineTag.hpp>
#include <Langulus/MetaOf.hpp>


namespace Langulus::Annies::Component
{
   ///                                                                        
   /// Implements descriptor interface full of convenience services.          
   ///   @tparam ID, SHARED heaps/stacks we're using as data source           
   template<Cid ID, Cid...SHARED>
   struct Descriptor {
      using CTTI_Component = Yup;
      using CTTI_ReflectAs = void;
      using Id             = Values<ID, SHARED...>;
      
      static constexpr int  ComponentPrecedence = 5000;
      static constexpr bool Shared = (sizeof...(SHARED) > 0);
      template<Cid SID>
      static constexpr bool Relevant = Id::template Contains<SID>;


      /// Set a default tag, if such wasn't already set. Analogous to how     
      /// CMake set() works - if tag was already cached, it won't be          
      /// overwritten unless you force it.                                    
      ///   @tparam TAG the tag to set                                        
      ///   @param value the value to assign                                  
      ///   @param force whether to set tag even if it already exists         
      template<CT::DefineTag TAG>
      void Set(this auto& self, auto&& value, bool force) {
         bool satisfied = false;
         self.ForEachDeep([&](TAG& tag) {
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
         self.Compose(TAG {LglsFwd(value)}); //TODO neat should implement compose/insert in its own way
      }

      /// Extract a tag from the descriptor                                   
      ///   @tparam TAG... tags we're searching for                           
      ///   @param values [out] where to save the value, if found             
      ///   @return true if any of the values changed                         
      template<CT::DefineTag...TAG>
      bool ExtractTag(this auto const& self, auto&...values) {
         bool satisfied = false;
         ((satisfied |= self.template ExtractTagInner<TAG>(values)), ...);
         return satisfied;
      }

      /// Extract data capable of initializing D. Pointer arithmetic allowed. 
      ///   @param value [out] where to save the value(s), if found           
      ///   @return the number of extracted values (always 1 if not an array) 
      template<class D>
      auto ExtractData(this auto const& self, D& value) -> size_t {
         size_t progress = 0;

         if constexpr (CT::Array<D>) {
            self.ForEachDeep([&](TypeOf<D> const& data) {
               //TODO can be optimized-out for POD
               value[progress] = data;
               ++progress;
               return (progress >= ExtentOf<D>) ? Loop::Break : Loop::Continue;
            });
         }
         else {
            self.ForEachDeep([&](D const& data) {
               value = data;
               ++progress;
               return Loop::Break;
            });
         }

         return progress;
      }

      /// Extract any data that is convertible to D                           
      ///   @param value [out] where to save the value, if found              
      ///   @return the number of extracted values (always 1 if not an array) 
      template<class D, class SELF>
      auto ExtractDataAs(this SELF const& self, D& value) -> size_t {
         size_t progress = 0;

         self.ForEachDeep([&](auto const& group) {
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
      auto FindType(this auto const& self) -> RTTI::DMeta {
         return self.FindType(MetaDataOf<T>());
      }

      /// Find data in recipes or tail. Only pointer arithmetic casts are     
      /// allowed, such as inheritance.                                       
      ///   @tparam type the type requirement                                 
      ///   @return the first type that matches                               
      template<class TYPE, class SELF>
      auto FindType(this SELF const& self, RTTI::DMeta type) -> RTTI::DMeta {
         bool ambiguous = false;
         RTTI::DMeta found;

         self.ForEachDeep([&](auto const& group) noexcept {
            group.ForEachMeta("Recipe", [&](auto const& recipe) noexcept {
               if (not recipe.GetTarget().CastsTo(type))
                  return;

               if (not found)
                  found = recipe.GetTarget();
               else
                  ambiguous = true;
            });

            if (not group.CastsTo(type))
               return;

            if (not found)
               found = group.GetType();
            else
               ambiguous = true;
         });

         if (ambiguous) {
            Logger::Warning(
               "Multiple types found in descriptor - all except the first `",
               found, "` will be ignored on FindType"
            );
         }

         return found;
      }
   };
}