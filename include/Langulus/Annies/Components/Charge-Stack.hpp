///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Component.hpp"


namespace Langulus::Annies::Component
{
   ///                                                                        
   /// Charge, carrying the four dimensions *ominous music*                   
   ///                                                                        
   struct Charge {
      using CTTI_Component = Yup;
      using CTTI_ReflectAs = void;

      static constexpr int ComponentPrecedence = -1000;

      static constexpr Real DefaultMass = 1;
      static constexpr Real DefaultRate = 0;
      static constexpr Real DefaultTime = 0;

      static constexpr Real DefaultPriority = 0;
      static constexpr Real MinPriority     = -10000;
      static constexpr Real MaxPriority     =  10000;

   protected:
      Real mMass     = DefaultMass;
      Real mRate     = DefaultRate;
      Real mTime     = DefaultTime;
      Real mPriority = DefaultPriority;

   public:
      constexpr Charge(
         Real mass = DefaultMass,
         Real rate = DefaultRate,
         Real time = DefaultTime,
         Real prio = DefaultPriority
      ) noexcept
         : mMass     {mass}
         , mRate     {rate}
         , mTime     {time}
         , mPriority {prio} {}

      constexpr bool operator == (const Charge& rhs) const noexcept {
         return mMass == rhs.mMass
            and mRate == rhs.mRate
            and mTime == rhs.mTime
            and mPriority == rhs.mPriority;
      }

      constexpr Charge operator * (const Real& scalar) const noexcept {
         return {mMass * scalar, mRate, mTime, mPriority};
      }

      constexpr Charge operator ^ (const Real& scalar) const noexcept {
         return {mMass, mRate * scalar, mTime, mPriority};
      }

      constexpr Charge& operator *= (const Real& scalar) noexcept {
         mMass *= scalar;
         return *this;
      }

      constexpr Charge& operator ^= (const Real& scalar) noexcept {
         mRate *= scalar;
         return *this;
      }

      constexpr bool IsCharged() const noexcept {
         return *this != Charge {};
      }

      constexpr bool IsFlowDependent() const noexcept {
         return mRate != DefaultRate
             or mTime != DefaultTime
             or mPriority != DefaultPriority;
      }

      void Reset() noexcept {
         mMass = DefaultMass;
         mRate = DefaultRate;
         mTime = DefaultTime;
         mPriority = DefaultPriority;
      }
   };
}
