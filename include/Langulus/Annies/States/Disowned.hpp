///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Container.hpp"


namespace Langulus::Annies::Component::State
{
   /// Refers back to this particular component instance through the deduced  
   /// 'this'. Just for convenience. It is #undef-ed at the end of this file. 
   #define ThisCom self.Disowned<V, ID, SHARED...>

   ///                                                                        
   /// If enabled, allocations and entries will never be referenced or        
   /// dereferenced, neither at construction/destruction, nor on assignment.  
   /// Enabled when a container is absorbed using the Disown intent. Useful   
   /// for creating data views, temporary containers, or handles.             
   ///   @tparam V decides whether state is dynamic or static                 
   ///   @tparam ID, SHARED - affected dimensions                             
   template<StateValue V, Cid ID, Cid...SHARED>
   struct Disowned {
      using CTTI_Component = Yup;
      using CTTI_State     = Yup;
      using CTTI_ReflectAs = void;
      using Id             = Values<ID, SHARED...>;

      static constexpr int  ComponentPrecedence = -4000;
      static constexpr bool Static  = V != StateValue::Variable;
      static constexpr bool Dynamic = not Static;
      static constexpr bool Enable  = V == StateValue::Enabled;
      template<Cid SID>
      static constexpr bool Relevant = Id::template Contains<SID>;
      
      using StateRequest = Tif<Dynamic, Disowned, void>;

      // Every state needs a unique ID in order to find matches even    
      // when template arguments are different                          
      static constexpr StateUid UID = StateUid::Disowned;

   protected:
      LglsComHeapMovable(friend);
      
      template<CT::Component...COMPONENTS> requires ValidComponentOrder<COMPONENTS...>
      friend struct Component::Container;

      /// Enable the dynamic disowned state                                   
      template<Cid SID = ID, CT::Container C> requires Relevant<SID>
      constexpr auto EnableDisowned(this C&& self) noexcept -> C&& requires Dynamic {
         self.GetStateInner() += Disowned<V, ID, SHARED...> {};
         return LglsFwd(self);
      }

      /// Disable the dynamic disowned state                                  
      template<Cid SID = ID, CT::Container C> requires Relevant<SID>
      constexpr auto DisableDisowned(this C&& self) noexcept -> C&& requires Dynamic {
         self.GetStateInner() -= Disowned<V, ID, SHARED...> {};
         return LglsFwd(self);
      }
      
      /// Enable the state when transferring using Disown intent              
      template<class SELF, CT::Disowned I> requires CT::Container<I>
      constexpr void ConstructFrom(this SELF& self, I&&) noexcept {
         ThisCom::EnableDisowned();
      }
   };

   #undef ThisCom
}