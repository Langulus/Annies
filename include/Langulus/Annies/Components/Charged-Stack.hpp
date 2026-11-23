///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Component.hpp"
#include <Langulus/IntentOf.hpp>
#include <Langulus/CT/Charged.hpp>


namespace Langulus::Annies::Component
{
   ///                                                                        
   /// MARK: Charge                                                           
   /// Carrying the four dimensions *ominous music*                           
   template<Cid ID>
   struct ChargedStack {
      /// Refers back to this particular component instance through the       
      /// deduced 'this'. Just for convenience. It is #undef-ed at the end of 
      /// this file.                                                          
      #define ThisCom self.ChargedStack<ID>

      using CTTI_ReflectAs = void;
      using CTTI_Component = Yup;
      using CTTI_Nullable  = No;
      using CTTI_Charged   = Yup;
      using StackRequest   = Langulus::Charge;
      using Id             = Values<ID>;

      static constexpr int ComponentPrecedence = -100;
      
      /// Get the contained charge                                            
      template<Cid SID = ID>
      constexpr auto& GetCharge(this auto&& self) noexcept {
         return ThisCom::GetChargeInner();
      }
      
      /// Set the charge                                                      
      ///   @param c the new charge                                           
      template<Cid SID = ID>
      void SetCharge(this auto& self, Langulus::Charge const& c) {
         ThisCom::GetChargeInner() = c;
      }

      /// Reset the charge                                                    
      template<Cid SID = ID>
      constexpr void ResetCharge(this auto& self) noexcept {
         ThisCom::SetChargeInner({});
      }
      
      /// Resets all charges, in case container is not Multicharge            
      constexpr void ResetAllCharges(this auto& self) noexcept {
         ThisCom::ResetCharge();
      }

      /// Get the mass component                                              
      Real GetMass(this auto const& self) noexcept {
         return ThisCom::GetChargeInner().mass;
      }
      
      /// Get the rate component                                              
      Real GetRate(this auto const& self) noexcept {
         return ThisCom::GetChargeInner().rate;
      }
      
      /// Get the time component                                              
      Real GetTime(this auto const& self) noexcept {
         return ThisCom::GetChargeInner().time;
      }
      
      /// Get the precedence component                                        
      Real GetPrecedence(this auto const& self) noexcept {
         return ThisCom::GetChargeInner().precedence;
      }
      
      /// Get the mass component                                              
      template<class SELF>
      auto SetMass(this SELF&& self, Real mass) noexcept -> SELF&& {
         ThisCom::GetChargeInner().mass = mass;
         return LglsFwd(self);
      }
      
      /// Get the rate component                                              
      template<class SELF>
      auto SetRate(this SELF&& self, Real rate) noexcept -> SELF&& {
         ThisCom::GetChargeInner().rate = rate;
         return LglsFwd(self);
      }
      
      /// Get the time component                                              
      template<class SELF>
      auto SetTime(this SELF&& self, Real time) noexcept -> SELF&& {
         ThisCom::GetChargeInner().time = time;
         return LglsFwd(self);
      }
      
      /// Get the precedence component                                        
      template<class SELF>
      auto SetPrecedence(this SELF&& self, Real precedence) noexcept -> SELF&& {
         ThisCom::GetChargeInner().precedence = precedence;
         return LglsFwd(self);
      }
      
   protected:
      /// MARK: Protected                                                     
      /// Get the contained charge (inner)                                    
      template<Cid SID = ID>
      constexpr auto& GetChargeInner(this auto&& self) noexcept {
         return self.template AccessStack<ChargedStack>();
      }

      /// Set the contained charge (inner)                                    
      template<Cid SID = ID>
      constexpr void SetChargeInner(this auto& self, Langulus::Charge const& c) noexcept {
         ThisCom::GetChargeInner() = c;
      }

      /// Transfer from any kind of container, respecting intents.            
      /// Do it for a particular dimension.                                   
      ///   @param intent The intent and container to transfer from.          
      template<Cid D, class SELF, CT::Intent I> 
      void SliceFrom(this SELF& self, I&& intent) 
      requires (CT::Container<I> and requires { intent->template GetCharge<D>(); }) {
         static_assert(CT::Disowned<I>);
         ThisCom::SetCharge(intent->template GetCharge<D>());
      }

      /// Transfer from any kind of container, respecting intents             
      ///   @param intent the intent and container to transfer from           
      template<class SELF, CT::Intent I> 
      void ConstructFrom(this SELF& self, I&& intent)
      requires (CT::Container<I> and requires { intent->template GetCharge<ID>(); }) {
         ThisCom::SetCharge(intent->template GetCharge<ID>());
      }
   };
}

#undef ThisCom
