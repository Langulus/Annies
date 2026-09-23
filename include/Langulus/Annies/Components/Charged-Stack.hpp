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
   /// Refers back to this particular component instance through the deduced  
   /// 'this'. Just for convenience. It is #undef-ed at the end of this file. 
   #define ThisCom self.ChargedStack<ID>

   ///                                                                        
   /// MARK: Charge                                                           
   /// Carrying the four dimensions *ominous music*                           
   template<Cid ID>
   struct ChargedStack {
      using CTTI_ReflectAs = void;
      using CTTI_Component = Yup;
      using CTTI_Nullable  = No;
      using CTTI_Charged   = Yup;
      using StackRequest   = Langulus::Charge;
      using Id             = Values<ID>;

      static constexpr int ComponentPrecedence = -100;
      
      /// Get the contained charge                                            
      template<Cid SID = ID>
      constexpr auto GetCharge(this auto const& self) noexcept -> Langulus::Charge const& {
         return ThisCom::GetChargeInner();
      }
      
      /// Set the charge                                                      
      ///   @param c the new charge                                           
      template<Cid SID = ID, CT::Container C>
      void SetCharge(this C& self, Langulus::Charge const& c) {
         ThisCom::GetChargeInner() = c;
      }
      
      /// Copy/move charge from another container                             
      ///   @param other the container to copy charge from                    
      template<Cid SID = ID, CT::Container I, class SELF> requires CT::NoIntent<I>
      void AbsorbCharge(this SELF& self, I const& other) {
         ThisCom::SetCharge(other.template GetCharge<SID>());
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
      void SetMass(this auto& self, Real mass) noexcept {
         ThisCom::GetChargeInner().mass = mass;
      }
      
      /// Get the rate component                                              
      void SetRate(this auto& self, Real rate) noexcept {
         ThisCom::GetChargeInner().rate = rate;
      }
      
      /// Get the time component                                              
      void SetTime(this auto& self, Real time) noexcept {
         ThisCom::GetChargeInner().time = time;
      }
      
      /// Get the precedence component                                        
      void SetPrecedence(this auto& self, Real precedence) noexcept {
         ThisCom::GetChargeInner().precedence = precedence;
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
      template<Cid D, class SELF, CT::Intent I> requires CT::Container<I>
      void SliceFrom(this SELF& self, I&& intent) {
         static_assert(CT::Disowned<I>);
         ThisCom::template AbsorbCharge<D>(LglsFwd(intent));
      }

      /// Transfer from any kind of container, respecting intents             
      ///   @param intent the intent and container to transfer from           
      template<class SELF, CT::Intent I> requires CT::Container<I>
      void ConstructFrom(this SELF& self, I&& intent) {
         ThisCom::AbsorbCharge(LglsFwd(intent));
         
         if constexpr (CT::Moved<I>)
            intent->template SetChargeInner<ID>({});
      }
   };
}

#undef ThisCom
