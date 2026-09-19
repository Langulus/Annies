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
   /// If enabled, data is marked as a missing past.                          
   /// Processing natural language involves incomplete information a lot of   
   /// the time. Missing past represents a linking point, which will get      
   /// filled with already available context.                                 
   ///   @tparam V decides whether state is dynamic or static                 
   template<StateValue V, Cid ID, Cid...SHARED>
   struct Past {
      using CTTI_Component = Yup;
      using CTTI_State     = Yup;
      using CTTI_ReflectAs = void;
      using Id = Values<ID, SHARED...>;

      static constexpr int  ComponentPrecedence = 3000;
      static constexpr bool Static  = V != StateValue::Variable;
      static constexpr bool Dynamic = not Static;
      static constexpr bool Enable  = V == StateValue::Enabled;
      //static constexpr bool CanBeMissing = Dynamic or Enable;
      template<Cid SID>
      static constexpr bool Relevant = Id::template Contains<SID>;

      using StateRequest = Tif<Dynamic, Past, void>;

      // Every state needs a unique ID in order to find matches even    
      // when template arguments are different                          
      static constexpr StateUid UID = StateUid::Past;

      template<Cid SID = ID> requires Relevant<SID>
      constexpr bool IsPast() const requires Static {
         return Enable;
      }

      template<Cid SID = ID, CT::Container C> requires Relevant<SID>
      constexpr bool IsPast(this C const& self) noexcept requires Dynamic {
         return self.GetStateInner() & Past<V, ID, SHARED...> {};
      }

      template<Cid SID = ID, CT::Container C> requires Relevant<SID>
      auto EnablePast(this C& self) noexcept -> C& requires Dynamic {
         self.GetStateInner() += Past<V, ID, SHARED...> {};
         return self;
      }

      template<Cid SID = ID, CT::Container C> requires Relevant<SID>
      auto DisablePast(this C& self) noexcept -> C& requires Dynamic {
         self.GetStateInner() -= Past<V, ID, SHARED...> {};
         return self;
      }
   };
}