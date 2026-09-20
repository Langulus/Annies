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
   /// If enabled, data is considered disjunct instead of conjunct.           
   /// Useful to encode alternative arguments or branched execution.          
   /// This simple bit is crucial for handling ambiguity.                     
   ///   @tparam V decides whether state is dynamic or static                 
   template<StateValue V, Cid ID, Cid...SHARED>
   struct Or {
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

      using StateRequest = Tif<Dynamic, Or, void>;

      // Every state needs a unique ID in order to find matches even    
      // when template arguments are different                          
      static constexpr Annies::State UID = Annies::State::Or;

      template<Cid SID = ID> requires Relevant<SID>
      constexpr bool IsOr() const requires Static {
         return Enable;
      }

      template<Cid SID = ID, CT::Container C> requires Relevant<SID>
      constexpr bool IsOr(this const C& self) noexcept requires Dynamic {
         return self.GetStateInner() & Or<V, ID, SHARED...> {};
      }

      template<Cid SID = ID, CT::Container C> requires Relevant<SID>
      auto EnableOr(this C& self) noexcept -> C& requires Dynamic {
         self.GetStateInner() += Or<V, ID, SHARED...> {};
         return self;
      }

      template<Cid SID = ID, CT::Container C> requires Relevant<SID>
      auto DisableOr(this C& self) noexcept -> C& requires Dynamic {
         self.GetStateInner() -= Or<V, ID, SHARED...> {};
         return self;
      }
   };
}