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
   ///                                                                        
   /// If enabled, container won't change to assignment.                      
   ///   @tparam V decides whether state is dynamic or static                 
   template<StateValue V, Cid ID, Cid...SHARED>
   struct Pinned {
      using CTTI_Component = Yup;
      using CTTI_State     = Yup;
      using CTTI_ReflectAs = void;
      using Id             = Values<ID, SHARED...>;

      static constexpr int  ComponentPrecedence = 3000;
      static constexpr bool Static  = V != StateValue::Variable;
      static constexpr bool Dynamic = not Static;
      static constexpr bool Enable  = V == StateValue::Enabled;
      template<Cid SID>
      static constexpr bool Relevant = Id::template Contains<SID>;

      using StateRequest = Tif<Dynamic, Pinned, void>;

      // Every state needs a unique ID in order to find matches even    
      // when template arguments are different                          
      static constexpr Annies::State UID = Annies::State::Pinned;

      template<Cid SID = ID> requires Relevant<SID>
      constexpr bool IsPinned() const requires Static {
         return Enable;
      }

      template<Cid SID = ID, CT::Container C> requires Relevant<SID>
      constexpr bool IsPinned(this const C& self) noexcept requires Dynamic {
         return self.GetStateInner() & Pinned<V, ID, SHARED...> {};
      }

      template<Cid SID = ID, CT::Container C> requires Relevant<SID>
      auto Pin(this C& self) noexcept -> C& requires Dynamic {
         self.GetStateInner() += Pinned<V, ID, SHARED...> {};
         return self;
      }

      template<Cid SID = ID, CT::Container C> requires Relevant<SID>
      auto Unpin(this C& self) noexcept -> C& requires Dynamic {
         self.GetStateInner() -= Pinned<V, ID, SHARED...> {};
         return self;
      }
   };
}