///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include <Langulus/IntentOf.hpp>
#include <ranges>


namespace Langulus::Annies
{
   ///                                                                        
   ///   Reverse iteration adapter                                            
   ///                                                                        
   /// Use like this: for(auto i : IterateInReverse(container)), where        
   /// 'container' can be any range, including std one.                       
   template<class C>
   struct IterateInReverse {
      using CTTI_ReflectAs = void;
      static_assert(CT::NoIntent<C>,         "C can't have an intent");
      static_assert(CT::NotReference<C>,     "C can't be a reference");
      static_assert(::std::ranges::range<C>, "C is not a range");

      C& range;

      constexpr IterateInReverse(C& a) noexcept
         : range {a} {}

      decltype(auto) begin() noexcept { return range.rbegin(); }
      decltype(auto) end()   noexcept { return range.rend();   }
   };

   template<class C>
   IterateInReverse(C&) -> IterateInReverse<C>;
}