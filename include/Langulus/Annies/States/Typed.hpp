///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Component.hpp"


namespace Langulus::Annies::Component::State
{
   ///                                                                        
   /// If enabled, data won't ever change type. Very useful when a type-      
   /// erased container has to represent a templated counterpart.             
   /// Needed to constrain the memory manipulations for safety.               
   ///   @tparam V decides whether state is dynamic or static                 
   template<StateValue V, Cid ID, Cid...SHARED>
   struct Typed {
      using CTTI_Component = Yup;
      using CTTI_State     = Yup;
      using CTTI_ReflectAs = void;
      using Id = Values<ID, SHARED...>;

      static constexpr int  ComponentPrecedence = 3000;
      static constexpr bool Static  = V != StateValue::Variable;
      static constexpr bool Dynamic = not Static;
      static constexpr bool Enable  = V == StateValue::Enabled;
      template<Cid SID>
      static constexpr bool Relevant = Id::template Contains<SID>;
      
      using StateRequest = Tif<Dynamic, Typed, void>;

      // Every state needs a unique ID in order to find matches even    
      // when template arguments are different                          
      static constexpr Annies::State UID = Annies::State::Typed;

      template<Cid SID = ID> requires Relevant<SID>
      constexpr bool IsTypeConstrained() const requires Static {
         return Enable;
      }

      template<Cid SID = ID, CT::Container C> requires Relevant<SID>
      constexpr bool IsTypeConstrained(this const C& self) noexcept requires Dynamic {
         return self.GetStateInner() & Typed<V, ID, SHARED...> {};
      }

      template<Cid SID = ID, CT::Container C> requires Relevant<SID>
      auto EnableTypeConstrained(this C& self) noexcept -> C& requires Dynamic {
         self.GetStateInner() += Typed<V, ID, SHARED...> {};
         return self;
      }

      template<Cid SID = ID, CT::Container C> requires Relevant<SID>
      auto DisableTypeConstrained(this C& self) noexcept -> C& requires Dynamic {
         self.GetStateInner() -= Typed<V, ID, SHARED...> {};
         return self;
      }
   };
}