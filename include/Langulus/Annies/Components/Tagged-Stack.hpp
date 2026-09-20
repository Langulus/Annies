///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../States/Tagged.hpp"
#include "Langulus/IntentOf.hpp"
#include "Langulus/Typenav.hpp"
#include <Langulus/RTTI/MetaTag.hpp>


namespace Langulus::Annies
{
   using TMeta = RTTI::TMeta;
}

namespace Langulus::Annies::Component
{
   /// Refers back to this particular component instance through the deduced  
   /// 'this'. Just for convenience. It is #undef-ed at the end of this file. 
   #define ThisCom self.TaggedStack<META, TYPE, CONSTRAIN, ID>

   ///                                                                        
   /// Defines the contained tag as a member variable, allowing the use of    
   /// tag-erasure. You can optionally constrain the tag at runtime.          
   ///   @attention when constrained, the value on the stack is used only     
   ///      for padding so that containers are binary-compatible, but not     
   ///      really red or written to.                                         
   ///   @tparam META the type of the meta                                    
   ///   @tparam TYPE optionally static tag, use void for tag-erasure         
   ///   @tparam CONSTRAIN override tag-constraint                            
   ///   @tparam ID data provider that gets tagged                            
   template<class META, class TYPE, bool CONSTRAIN, Cid ID>
   struct TaggedStack : State::Tagged<StateValueIf(CONSTRAIN or not ::std::is_void_v<TYPE>), ID> {
      using CTTI_Component = Yup;
      using CTTI_Tagged    = TYPE;
      using CTTI_ReflectAs = void;
      using StackRequest   = META;
      using Id             = Values<ID>;

      static constexpr int  ComponentPrecedence = -2900;
      static constexpr bool TagErased = CT::Void<TYPE>;

      /// MARK: Public                                                        
      /// Get the contained tag - not possible at compile-time yet            
      ///   @tparam SID - tag selector                                        
      template<Cid SID = ID>
      constexpr META GetTag(this auto const& self) noexcept {
         if consteval { return META {}; }
         else {
            if constexpr (TagErased)
               return ThisCom::GetTagInner();
            else
               return MetaTagOf<TYPE>();
         }
      }

      /// Get the reflected tag name                                          
      ///   @tparam SID - tag selector                                        
      template<Cid SID = ID>
      constexpr auto GetTagName(this auto const& self) noexcept {
         if constexpr (TagErased)
            return ThisCom::GetTagInner().GetName();
         else
            return NameOfTag<TYPE>;
      }

      /// Check if block has a tag                                            
      ///   @tparam SID - tag selector                                        
      ///   @return true if tag is specified                                  
      template<Cid SID = ID>
      constexpr bool IsTagged(this auto const& self) noexcept {
         if constexpr (TagErased)
            return static_cast<bool>(ThisCom::GetTagInner());
         else
            return true;
      }

      /// Check if tag is the provided type (can run at compile-time          
      /// if container is statically-tagged)                                  
      ///   @tparam T the tag to compare against                              
      ///   @return true if tags match                                        
      template<CT::DefineTag T, Cid SID = ID>
      constexpr bool IsTag(this auto const& self) noexcept {
         if constexpr (TagErased)
            return ThisCom::GetTagInner().Is(MetaTagOf<T>());
         else
            return Exact<TYPE, T>;
      }
      
      /// Set the contained tag if possible.                                  
      /// This is still used if statically tagged - checks if tags are        
      /// compatible in constructors and assigners.                           
      ///   @tparam T the new tag                                             
      template<CT::DefineTag T, Cid SID = ID, CT::Container C>
      void SetTag(this C& self) {
         static_assert(TagErased or Exact<T, TYPE>, "Tag mismatch");
         if constexpr (TagErased)
            ThisCom::SetTag(MetaTagOf<T>());
      }

      /// Set the contained tag if possible.                                  
      /// This is still used if statically tagged - checks if tags are        
      /// compatible in constructors and assigners.                           
      /// This particular override doesn't benefit from compile-time checks.  
      ///   @param type the new tag                                           
      template<Cid SID = ID, CT::Container C>
      void SetTag(this C& self, META type) {
         if constexpr (TagErased) {
            // This container is tag-erased                             
            auto& t = ThisCom::GetTagInner();
            if (t == type)
               return;
         
            if (not t) {
               t = type;
               return;
            }

            LglsAssert(not ThisCom::IsTagConstrained(),
               "Attempting to mutate tag-locked container"
               " of tag ", t, " to tag ", type
            );
            
            t = type;
         }
         else {
            // This container is statically tagged                      
            auto local = MetaTagOf<TYPE>();
            LglsAssert(local.Is(type), "Tag mismatch", ": ", local,
               " is not ", type);
         }
      }
      
      /// Set all contained tags by copying them from another container.      
      /// This is still used if statically tagged - checks if tags are        
      /// compatible in constructors and assigners.                           
      ///   @param other the container to copy types from                     
      template<Cid SID = ID, CT::Container I, class SELF> requires CT::NoIntent<I>
      void AbsorbTag(this SELF& self, I const& other) {
         if constexpr (TagErased or CT::TagErased<I>) {
            auto T = other.template GetTag<SID>();
            ThisCom::SetTag(T);
         }
         else {
            using T = TagOf<I, SID>;
            ThisCom::template SetTag<T>();
         }
      }

   protected:
      /// MARK: Protected                                                     
      /*LglsComRemoval(friend);
      LglsComHeapMovable(friend);
      LglsComIndexedCommon(friend);
      LglsComEmplacement(friend);*/

      /// Reset the tag of the container, unless it's tag-constrained.        
      /// If this container isn't tag-erased, this call is a no-op.           
      template<Cid SID = ID>
      constexpr void ResetType(this auto& self) noexcept {
         if constexpr (TagErased) {
            if constexpr (requires { self.template IsTagConstrained<SID>(); }) {
               if (not self.template IsTagConstrained<SID>())
                  ThisCom::SetTagInner({});
            }
            else ThisCom::SetTagInner({});
         }
      }
      
      /// Resets all tags, in case container is not Multitag                  
      constexpr void ResetAllTags(this auto& self) noexcept {
         if constexpr (TagErased)
            ThisCom::ResetTag();
      }
      
      /// Get the contained tag (inner)                                       
      template<Cid SID = ID>
      constexpr auto& GetTagInner(this auto&& self) noexcept {
         auto& member = self.template AccessStack<TaggedStack>();
         if constexpr (not TagErased) {
            // Statically tagged containers generally don't use the     
            // stack member - it's there only for binary compatiblity.  
            // Set the member only if really, REALLY need by reference. 
            DecvqAllCast(member) = MetaTagOf<TYPE>();
         }
         return (member);
      }

      /// Set the contained tag (inner)                                       
      ///   @attention noop if tag-erased                                     
      template<Cid SID = ID>
      constexpr void SetTagInner(this auto& self, const META& type) noexcept {
         if constexpr (TagErased)
            ThisCom::GetTagInner() = type;
      }

      /// Transfer from any kind of container, respecting intents.            
      /// Do it for a particular dimension.                                   
      ///   @param intent The intent and container to transfer from.          
      template<Cid D, class SELF, CT::Intent I> requires CT::Container<I>
      void SliceFrom(this SELF& self, I&& intent) {
         static_assert(CT::Disowned<I>);

         ThisCom::template AbsorbTag<D>(LglsFwd(intent));

         if constexpr (TagErased) { //TODO type constraints are either pointless, or should happen only if source is !Copied and !Cloned and !HeapAllocated. so what about tag constraints?
            // While we are interfacing external memory, we have to     
            // keep the tag-constrained state, otherwise we risk        
            // interpreting static memory the wrong way.                
            if constexpr (not CONSTRAIN) {
               if constexpr (not CT::TagErased<I>)
                  // From statically-tagged to dynamically-tagged       
                  ThisCom::EnableTagConstrained();
               else if (intent->template IsTagConstrained<D>())
                  // From dynamically-tagged to dynamically-tagged      
                  ThisCom::EnableTagConstrained();
            }
         }
      }

      /// Transfer from any kind of container, respecting intents             
      ///   @param intent the intent and container to transfer from           
      template<class SELF, CT::Intent I> requires CT::Container<I>
      void ConstructFrom(this SELF& self, I&& intent) {
         ThisCom::AbsorbTag(LglsFwd(intent));

         if constexpr (TagErased) { //TODO type constraints are either pointless, or should happen only if source is !Copied and !Cloned and !HeapAllocated. so what about tag constraints?
            // While we are interfacing external memory, we have to     
            // keep the type-constrained state, otherwise we risk       
            // interpreting static memory the wrong way.                
            if constexpr (not CONSTRAIN) {
               if constexpr (not CT::TagErased<I>)
                  // From statically-tagged to dynamically-tagged       
                  ThisCom::EnableTagConstrained();
               else if (intent->template IsTagConstrained<ID>())
                  // From dynamically-tagged to dynamically-tagged      
                  ThisCom::EnableTagConstrained();
            }
         }

         if constexpr (CT::Moved<I> and CT::TagErased<I>)
            intent->template SetTagInner<ID>(META{});
      }
   };

   #undef ThisCom
}
