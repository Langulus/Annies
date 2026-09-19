///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../States/Verbed.hpp"
#include "Langulus/IntentOf.hpp"
#include "Langulus/Typenav.hpp"
#include <Langulus/RTTI/MetaVerb.hpp>
#include <Langulus/CT/Charged.hpp>
#include <Langulus/CT/DefineVerb.hpp>
#include <Langulus/CT/Verbed.hpp>


namespace Langulus::Annies
{
   using VMeta = RTTI::VMeta;
}

namespace Langulus::Annies::Component
{
   /// Refers back to this particular component instance through the deduced  
   /// 'this'. Just for convenience. It is #undef-ed at the end of this file. 
   #define ThisCom self.VerbedStack<META, TYPE, CONSTRAIN, ID>

   ///                                                                        
   /// Defines the contained verb as a member variable, allowing the use of   
   /// verb-erasure. You can optionally constrain the verb at runtime.        
   ///   @attention when constrained, the value on the stack is used only     
   ///      for padding so that containers are binary-compatible, but not     
   ///      really red or written to.                                         
   ///   @tparam META the type of the meta                                    
   ///   @tparam TYPE optionally static verb, use void for verb-erasure       
   ///   @tparam CONSTRAIN override verb-constraint                           
   ///   @tparam ID data provider that gets verbed                            
   template<class META, class TYPE, bool CONSTRAIN, Cid ID>
   struct VerbedStack : State::Verbed<StateValueIf(CONSTRAIN or not ::std::is_void_v<TYPE>), ID> {
      using CTTI_Component  = Yup;
      using CTTI_Executable = Yup;
      using CTTI_ReflectAs  = void;
      using CTTI_Verbed     = TYPE;
      using StackRequest    = META;
      using Id              = Values<ID>;

      static constexpr int  ComponentPrecedence = -2800;
      static constexpr bool VerbErased = CT::Void<TYPE>;

      /// MARK: Public                                                        
      /// Get the contained verb                                              
      template<Cid SID = ID>
      constexpr META GetVerb(this auto const& self) noexcept {
         if consteval { return META {}; }
         else {
            if constexpr (VerbErased)
               return ThisCom::GetVerbInner();
            else
               return MetaVerbOf<TYPE>();
         }
      }

      /// Get the reflected verb name.                                        
      /// If the container is charged, this will return the negative verb     
      /// token if mass is below zero. Otherwise results always in the        
      /// default (positive) token.                                           
      template<Cid SID = ID, class C>
      constexpr auto GetVerbName(this C const& self) noexcept {
         if constexpr (VerbErased) {
            if constexpr (CT::Charged<C>) {
               if (self.GetMass() < 0)
                  return ThisCom::GetVerbInner().GetNegativeName();
            }

            return ThisCom::GetVerbInner().GetPositiveName();
         }
         else {
            if constexpr (CT::Charged<C>) {
               if (self.GetMass() < 0)
                  return NegativeNameOfVerb<TYPE>;
            }

            return PositiveNameOfVerb<TYPE>;
         }
      }

      /// Check if container has a verb specified                             
      template<Cid SID = ID>
      constexpr bool IsVerbed(this auto const& self) noexcept {
         if constexpr (VerbErased)
            return static_cast<bool>(ThisCom::GetVerbInner());
         else
            return true;
      }

      /// Check if verb is the provided type (can run at compile-time         
      /// if container is statically-verbed)                                  
      ///   @tparam T the verb to compare against                             
      ///   @return true if verbs match                                       
      template<CT::DefineVerb T, Cid SID = ID>
      constexpr bool IsVerb(this auto const& self) noexcept {
         if constexpr (VerbErased)
            return ThisCom::GetVerbInner().Is(MetaVerbOf<T>());
         else
            return Exact<TYPE, T>;
      }
      
      /// Set the contained verb if possible.                                 
      /// This is still used if statically verbed - checks if verbs are       
      /// compatible in constructors and assigners.                           
      ///   @tparam T the new verb                                            
      template<CT::DefineVerb T, Cid SID = ID, CT::Container C>
      void SetVerb(this C& self) {
         static_assert(VerbErased or Exact<T, TYPE>, "Verb mismatch");
         if constexpr (VerbErased)
            ThisCom::SetVerb(MetaVerbOf<T>());
      }

      /// Set the contained verb if possible.                                 
      /// This is still used if statically verbed - checks if verbs are       
      /// compatible in constructors and assigners.                           
      /// This particular override doesn't benefit from compile-time checks.  
      ///   @param type the new verb                                          
      template<Cid SID = ID, CT::Container C>
      void SetVerb(this C& self, META type) {
         if constexpr (VerbErased) {
            // This container is verb-erased                            
            auto& t = ThisCom::GetVerbInner();
            if (t == type)
               return;
         
            if (not t) {
               t = type;
               return;
            }

            LglsAssert(not ThisCom::IsVerbConstrained(),
               "Attempting to mutate verb-locked container"
               " of verb ", t, " to verb ", type
            );
            
            t = type;
         }
         else {
            // This container is statically verbed                      
            auto local = MetaVerbOf<TYPE>();
            LglsAssert(local.Is(type), "Verb mismatch", ": ", local,
               " is not ", type);
         }
      }
      
      /// Set all contained verbs by copying them from another container.     
      /// This is still used if statically verbed - checks if verbs are       
      /// compatible in constructors and assigners.                           
      ///   @param other the container to copy verbs from                     
      template<Cid SID = ID, CT::Container I, class SELF> requires CT::NoIntent<I>
      void AbsorbVerb(this SELF& self, I const& other) {
         if constexpr (VerbErased or CT::VerbErased<I>) {
            auto T = other.template GetVerb<SID>();
            ThisCom::SetVerb(T);
         }
         else {
            using T = VerbOf<I, SID>;
            ThisCom::template SetVerb<T>();
         }
      }

   protected:
      /// MARK: Protected                                                     
      /// Reset the verb of the container, unless it's verb-constrained.      
      /// If this container isn't verb-erased, this call is a no-op.          
      template<Cid SID = ID>
      constexpr void ResetVerb(this auto& self) noexcept {
         if constexpr (VerbErased) {
            if constexpr (requires { self.template IsVerbConstrained<SID>(); }) {
               if (not self.template IsVerbConstrained<SID>())
                  ThisCom::SetVerbInner({});
            }
            else ThisCom::SetVerbInner({});
         }
      }
      
      /// Resets all verbs, in case container is not Multiverb                
      constexpr void ResetAllVerbs(this auto& self) noexcept {
         if constexpr (VerbErased)
            ThisCom::ResetVerb();
      }
      
      /// Get the contained verb (inner)                                      
      template<Cid SID = ID>
      constexpr auto& GetVerbInner(this auto&& self) noexcept {
         auto& member = self.template AccessStack<VerbedStack>();
         if constexpr (not VerbErased) {
            // Statically verbed containers generally don't use the     
            // stack member - it's there only for binary compatiblity.  
            // Set the member only if really, REALLY need by reference. 
            DecvqAllCast(member) = MetaVerbOf<TYPE>();
         }
         return (member);
      }

      /// Set the contained verb (inner)                                      
      ///   @attention noop if verb-erased                                    
      template<Cid SID = ID>
      constexpr void SetVerbInner(this auto& self, const META& type) noexcept {
         if constexpr (VerbErased)
            ThisCom::GetVerbInner() = type;
      }

      /// Transfer from any kind of container, respecting intents.            
      /// Do it for a particular dimension.                                   
      ///   @param intent The intent and container to transfer from.          
      template<Cid D, class SELF, CT::Intent I> requires CT::Container<I>
      void SliceFrom(this SELF& self, I&& intent) {
         static_assert(CT::Disowned<I>);

         ThisCom::template AbsorbVerb<D>(LglsFwd(intent));

         if constexpr (VerbErased) { //TODO type constraints are either pointless, or should happen only if source is !Copied and !Cloned and !HeapAllocated. so what about verb constraints?
            // While we are interfacing external memory, we have to     
            // keep the verb-constrained state, otherwise we risk       
            // interpreting static memory the wrong way.                
            if constexpr (not CONSTRAIN) {
               if constexpr (not CT::VerbErased<I>)
                  // From statically-verbed to dynamically-verbed       
                  ThisCom::EnableVerbConstrained();
               else if (intent->template IsVerbConstrained<D>())
                  // From dynamically-verbed to dynamically-verbed      
                  ThisCom::EnableVerbConstrained();
            }
         }
      }

      /// Transfer from any kind of container, respecting intents             
      ///   @param intent the intent and container to transfer from           
      template<class SELF, CT::Intent I> requires CT::Container<I>
      void ConstructFrom(this SELF& self, I&& intent) {
         ThisCom::AbsorbVerb(LglsFwd(intent));

         if constexpr (VerbErased) { //TODO type constraints are either pointless, or should happen only if source is !Copied and !Cloned and !HeapAllocated. so what about verb constraints?
            // While we are interfacing external memory, we have to     
            // keep the cverb-constrained state, otherwise we risk      
            // interpreting static memory the wrong way.                
            if constexpr (not CONSTRAIN) {
               if constexpr (not CT::VerbErased<I>)
                  // From statically-verbed to dynamically-verbed       
                  ThisCom::EnableVerbConstrained();
               else if (intent->template IsVerbConstrained<ID>())
                  // From dynamically-verbed to dynamically-verbed      
                  ThisCom::EnableVerbConstrained();
            }
         }

         if constexpr (CT::Moved<I> and CT::VerbErased<I>)
            intent->template SetVerbInner<ID>(META{});
      }
   };

   #undef ThisCom
}
