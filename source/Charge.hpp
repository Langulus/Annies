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
   ///   Charge, carrying the four verb dimensions                            
   ///                                                                        
   class Charge {
      LANGULUS(POD) true;
      LANGULUS(NULLIFIABLE) false;
      LANGULUS_CONVERTS_TO(Text);

      // Mass of the verb                                               
      Real mMass = DefaultMass;
      // Frequency of the verb                                          
      Real mRate = DefaultRate;
      // Time of the verb                                               
      Real mTime = DefaultTime;
      // Priority of the verb                                           
      Real mPriority = DefaultPriority;

   public:
      static constexpr Real DefaultMass {1};
      static constexpr Real DefaultRate {0};
      static constexpr Real DefaultTime {0};

      static constexpr Real DefaultPriority {0};
      static constexpr Real MinPriority {-10000};
      static constexpr Real MaxPriority {+10000};

      constexpr Charge(
         Real = DefaultMass,
         Real = DefaultRate,
         Real = DefaultTime,
         Real = DefaultPriority
      ) noexcept;

      constexpr bool    operator == (const Charge&) const noexcept;

      constexpr Charge  operator *  (const Real&) const noexcept;
      constexpr Charge  operator ^  (const Real&) const noexcept;

      constexpr Charge& operator *= (const Real&) noexcept;
      constexpr Charge& operator ^= (const Real&) noexcept;

      constexpr bool IsDefault() const noexcept;
      constexpr bool IsFlowDependent() const noexcept;
      Hash GetHash() const noexcept;
      void Reset() noexcept;

      operator Text() const;
   };

}