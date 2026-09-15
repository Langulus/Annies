///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Config.hpp"


namespace Langulus::Annies
{

   ///                                                                        
   ///   Data comparison results                                              
   ///                                                                        
   struct Compared {
      LANGULUS(POD) true;
      LANGULUS(NULLIFIABLE) true;

      enum Enum : ::std::uint8_t {
         // Elements were not compared                                  
         Unknown = 0,

         // LHS == RHS                                                  
         Equal = 1,

         // LHS != RHS                                                  
         Unequal = 2,

         // LHS > RHS                                                   
         Greater = 3,

         // LHS < RHS                                                   
         Lower = 4
      };

      using Type = TypeOf<Enum>;

      Type mResult {Unknown};

   public:
      constexpr Compared() noexcept = default;
      constexpr Compared(const Type& a) noexcept
         : mResult {a} {}

      explicit constexpr operator bool() const noexcept {
         return mResult != Unknown;
      }
      constexpr bool operator == (const Compared::Enum& a) const noexcept {
         return mResult == a;
      }
   };
   
}
