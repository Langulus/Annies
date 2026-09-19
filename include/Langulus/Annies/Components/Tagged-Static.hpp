///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
//#include "../Container.hpp"
#include "../Component.hpp"
#include <Langulus/MetaOf.hpp>
#include <Langulus/CT/Deep.hpp>


namespace Langulus::Annies
{
   using TMeta = RTTI::TMeta;
}

namespace Langulus::Annies::Component
{
   ///                                                                        
   /// Defines the contained tag at compile-time.                             
   /// Doesn't allow for tag-erasure and doesn't take up space.               
   ///   @tparam META the tag of the definition                               
   ///   @tparam TYPE static tag, can't be void                               
   ///   @tparam ID which heap/stack is tagged?                               
   template<class META, CT::NotVoid TYPE, Cid ID>
   struct TaggedStatic {
      using CTTI_Component = Yup;
      using CTTI_Tags      = TYPE;
      using CTTI_ReflectAs = void;
      using Id             = Values<ID>;

      static constexpr int  ComponentPrecedence = -3000;
      static constexpr bool TagErased  = false;

      /// MARK: Public                                                        
      /// Get the reflected tag definition                                    
      template<Cid SID = ID>
      META GetTag() const noexcept {
         return MetaTagOf<TYPE>();
      }

      /// Get the reflected tag name                                          
      template<Cid SID = ID>
      constexpr auto GetTagName() const noexcept {
         return RTTI::NameOfTag<TYPE>();
      }

      /// Statically tagged containers are always tagged                      
      template<Cid SID = ID>
      constexpr bool IsTagged() const noexcept {
         return true;
      }

      /// Check if tag is the provided type. Always happens at compile-time.  
      ///   @tparam T the tag to compare against                              
      ///   @return true if tags are the same                                 
      template<CT::DefineTag T, Cid SID = ID>
      constexpr bool IsTag() const noexcept {
         return Exact<TYPE, T>;
      }

      /// Always returns true                                                 
      template<Cid SID = ID>
      constexpr bool IsTagConstrained() const noexcept {
         return true;
      }

      /// Does nothing                                                        
      template<Cid SID = ID>
      constexpr void EnableTagConstrained() const noexcept { }

      /// Can't disable tag-constraint in a statically-tagged container       
      template<Cid SID = ID>
      constexpr void DisableTagConstrained() const noexcept {
         static_assert(false,
            "Can't disable tag-constraint in a statically-tagged container"
         );
      }

      /// This is still used if statically-tagged - checks if tags are        
      /// compatible in constructors and assigners                            
      ///   @tparam T the new tag                                             
      template<CT::DefineTag T, Cid SID = ID>
      constexpr void SetTag() {
         static_assert(Exact<T, TYPE>, "Tag mismatch");
      }

      /// This is still used if statically-tagged - checks if tags are        
      /// compatible when arguments are tag-erased. This particular override  
      /// doesn't benefit from compile-time checks.                           
      ///   @param type the new tag                                           
      template<Cid SID = ID>
      void SetTag(META type) {
         LglsAssert(GetTag().Is(type), "Tag mismatch");
      }
      
      /// Set all contained data tags by copying them from another container. 
      /// This is still used if statically tagged - checks if tags are        
      /// compatible in constructors and assigners.                           
      ///   @param other the container to copy tags from                      
      template<Cid SID = ID, CT::Container I, class SELF> requires CT::NoIntent<I>
      void AbsorbTag(this SELF& self, I const& other) {
         if constexpr (TagErased or CT::TagErased<I>) {
            auto T = other.template GetTag<SID>();
            self.template SetTag<ID>(T);
         }
         else {
            using T = Deref<TagOf<I, SID>>;
            self.template SetTag<T, ID>();
         }
      }
   };
}
