///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include <Langulus/Many.hpp>
#include <Langulus/Recipe.hpp>


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

      /*template<CT::DefineTag>
      void Set(auto&&, bool force = false);

      template<CT::DefineTag...>
      bool ExtractTag(auto&...) const;
      auto ExtractData(auto&) const -> size_t;
      auto ExtractDataAs(auto&) const -> size_t;

      template<CT::NotVoid>
      auto FindType() const -> RTTI::DMeta;

      template<class TYPE>
      auto FindType(RTTI::DMeta) const -> RTTI::DMeta;*/

   };
}

#include <Langulus/Annies/Components/Descriptor.hpp>

#define TEMPLATE template<Cid ID, Cid...SHARED>
#define LGLSCOM  Descriptor<ID, SHARED...>
#define ThisCom  self.Descriptor<ID, SHARED...>

namespace Langulus::Annies::Component
{
   /// Set a default tag, if such wasn't already set. Analogous to how     
   /// CMake set() works - if tag was already cached, it won't be          
   /// overwritten unless you force it.                                    
   ///   @tparam TAG the tag to set                                        
   ///   @param value the value to assign                                  
   ///   @param force whether to set tag even if it already exists         
   TEMPLATE template<CT::DefineTag TAG, class D> requires (not CT::DefineTag<D>)
   void LGLSCOM::Set(this auto& self, D&& value, bool force) {
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

      self.Compose(TAG {LglsFwd(value)}); //TODO neat should implement compose/insert in its own way
   }

   /// Same as above but accepts tag as the value                          
   TEMPLATE template<CT::DefineTag TAG>
   void LGLSCOM::Set(this auto& self, TAG&& value, bool force) {
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

      self.Compose(LglsFwd(value)); //TODO neat should implement compose/insert in its own way
   }

   /// Extract a tag from the descriptor                                   
   ///   @tparam TAG... tags we're searching for                           
   ///   @param values [out] where to save the value, if found             
   ///   @return true if any of the values changed                         
   TEMPLATE template<CT::DefineTag...TAG>
   bool LGLSCOM::ExtractTag(this auto const& self, auto&...values) {
      bool satisfied = false;
      ((satisfied |= self.template ExtractTagInner<TAG>(values...)), ...);
      return satisfied;
   }

   /// Extract data capable of initializing D. Pointer arithmetic allowed. 
   ///   @param value [out] where to save the value(s), if found           
   ///   @return the number of extracted values (always 1 if not an array) 
   TEMPLATE template<class D>
   auto LGLSCOM::ExtractData(this auto const& self, D& value) -> size_t {
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
   TEMPLATE template<class D, class SELF>
   auto LGLSCOM::ExtractDataAs(this SELF const& self, D& value) -> size_t {
      size_t progress = 0;
      using DeepType = typename SELF::DeepType;
      self.ForEachDeep([&](DeepType const& group) {
         if constexpr (CT::Array<D>) {
            const auto count  = group.GetCount();
            const auto remain = ExtentOf<D> - progress;
            const auto toscan = remain < count ? remain : count;
            for (size_t i = 0; i < toscan; ++i) {
               //TODO can be optimized-out for POD
               try {
                  value[progress] = group.template AsAt<TypeOf<D>>(i);
                  ++progress;
               }
               catch (...) {}
            }

            return (progress >= ExtentOf<D>) ? Loop::Break : Loop::Continue;
         }
         else {
            try {
               value = DecvqAllCast(group.template As<D>());
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
   TEMPLATE template<CT::NotVoid T>
   auto LGLSCOM::FindType(this auto const& self) -> RTTI::DMeta {
      return self.FindType(MetaDataOf<T>());
   }

   /// Find data in recipes. Only pointer arithmetic casts are allowed.       
   ///   @tparam type the type requirement                                    
   ///   @return the first type that matches                                  
   TEMPLATE template<class C>
   auto LGLSCOM::FindType(this C const& self, RTTI::DMeta type) -> RTTI::DMeta {
      bool ambiguous = false;
      RTTI::DMeta found;
      
      self.ForEachDeep(
         [&type,&found,&ambiguous](Recipe const& recipe) {
            // For each recipe...                                       
            if (not recipe.GetTarget().CastsTo(type))
               return;

            if (not found)
               found = recipe.GetTarget();
            else
               ambiguous = true;
         },
         [&type,&found,&ambiguous](RTTI::DMeta const& meta) {
            // For each type...                                         
            if (not meta or not meta.CastsTo(type))
               return;

            if (not found)
               found = meta;
            else
               ambiguous = true;
         }
      );

      LglsAssumeUserWarn(not ambiguous,
         "Multiple types found in descriptor - all except the first `",
         found, "` will be ignored on FindType"
      );
      return found;
   }

   /// Get a reflected member variable inside the first element by index   
   ///   @param index the member index to search for                       
   ///   @return type-erased container to the reflected member(s)          
   TEMPLATE
   auto LGLSCOM::GetMember(this auto&& self, CT::Index auto&& index) -> RTTI::Block {
      if (self.IsEmpty())
         return {};

      auto& members = self.GetType().GetMembers();
      //TODO special indices
      return self.GetMemberInner(members[index]);
   }

   /// Get handle to a reflected member variable inside the first element  
   ///   @attention assumes block is not empty                             
   ///   @param member the member reflection to search for                 
   ///   @return type-erased container to the reflected member(s)          
   TEMPLATE
   auto LGLSCOM::GetMemberInner(this auto&& self, RTTI::DefinitionData::Member const& member)
   -> RTTI::Block {
      LglsAssumeDev(not self.IsEmpty(),
         "Getting member from an empty descriptor");
         
      return {
         member.member(const_cast<void*>(self.GetRaw())),
         member.type(),
         member.extent
      };
      /*return {
         Annies::Stackwise, Many::StateType{0},
         member.type(), member.member(const_cast<void*>(self.GetRaw())),
         member.extent, member.extent, self.GetAllocation(), Hash {0}
      };*/
   }

   ///                                                                     
   TEMPLATE
   template<CT::DefineTag TAG, size_t IDX, class D>
   bool LGLSCOM::ExtractTagInnerInner(this auto const& self, D& value) {
      bool satisfied = false;
      size_t counter = 0;

      self.ForEachDeep([&](TAG const& tag) {
         if (counter < IDX) {
            // We're only interested in the Nth tag                
            ++counter;
            return Loop::Continue;
         }

         if constexpr (CT::Deep<D>) {
            value = tag.GetData();
            satisfied = true;
         }
         else if (tag.GetData().ExtractDataAs(value))
            satisfied = true;

         return Loop::Break;
      });

      return satisfied;
   }

   ///                                                                     
   TEMPLATE template<CT::DefineTag TAG, size_t...IDX>
   bool LGLSCOM::ExtractTagInner(this auto const& self, ExpandedSequence<IDX...>, auto&...values) {
      return (self.template ExtractTagInnerInner<TAG, IDX>(values) or ...);
   }

   ///                                                                     
   TEMPLATE template<CT::DefineTag TAG>
   bool LGLSCOM::ExtractTagInner(this auto const& self, auto&...values) {
      return self.template ExtractTagInner<TAG>( 
         Sequence<sizeof...(values)>::Expand,
         values...
      );
   }
}

#undef TEMPLATE
#undef LGLSCOM
#undef ThisCom
