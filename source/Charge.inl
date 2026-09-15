///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Charge.hpp"


namespace Langulus::Annies
{
   
   /// Charge construction                                                    
   ///   @param mass - the mass charge                                        
   ///   @param freq - the frequency charge                                   
   ///   @param time - the time charge                                        
   ///   @param prio - the priority charge                                    
   LANGULUS(INLINED)
   constexpr Charge::Charge(Real mass, Real rate, Real time, Real prio) noexcept
      : mMass {mass}
      , mRate {rate}
      , mTime {time}
      , mPriority {prio} {}

   /// Compare charges                                                        
   ///   @param rhs - the charge to compare against                           
   ///   @return true if both charges match exactly                           
   LANGULUS(INLINED)
   constexpr bool Charge::operator == (const Charge& rhs) const noexcept {
      return mMass == rhs.mMass
         and mRate == rhs.mRate
         and mTime == rhs.mTime
         and mPriority == rhs.mPriority;
   }

   /// Check if charge is default                                             
   ///   @return true if charge is default                                    
   LANGULUS(INLINED)
   constexpr bool Charge::IsDefault() const noexcept {
      return *this == Charge {};
   }

   /// Check if charge is default                                             
   ///   @return true if charge is default                                    
   LANGULUS(INLINED)
   constexpr bool Charge::IsFlowDependent() const noexcept {
      return mRate != DefaultRate
          or mTime != DefaultTime
          or mPriority != DefaultPriority;
   }

   /// Get the hash of the charge                                             
   ///   @return the hash of the charge                                       
   LANGULUS(INLINED)
   Hash Charge::GetHash() const noexcept {
      return HashOf(mMass, mRate, mTime, mPriority);
   }

   /// Reset the charge to the default                                        
   LANGULUS(INLINED)
   void Charge::Reset() noexcept {
      mMass = DefaultMass;
      mRate = DefaultRate;
      mTime = DefaultTime;
      mPriority = DefaultPriority;
   }

   /// Scale the mass of a charge                                             
   ///   @param scalar - the scalar to multiply by                            
   ///   @return a new charge instance with changed mass                      
   LANGULUS(INLINED)
   constexpr Charge Charge::operator * (const Real& scalar) const noexcept {
      return {mMass * scalar, mRate, mTime, mPriority};
   }

   /// Scale the rate of a charge                                             
   ///   @param scalar - the scalar to multiply by                            
   ///   @return a new charge instance with changed rate                      
   LANGULUS(INLINED)
   constexpr Charge Charge::operator ^ (const Real& scalar) const noexcept {
      return {mMass, mRate * scalar, mTime, mPriority};
   }

   /// Scale the mass of a charge (destructive)                               
   ///   @param scalar - the scalar to multiply by                            
   ///   @return a reference to this charge                                   
   LANGULUS(INLINED)
   constexpr Charge& Charge::operator *= (const Real& scalar) noexcept {
      mMass *= scalar;
      return *this;
   }

   /// Scale the rate of a charge (destructive)                               
   ///   @param scalar - the scalar to multiply by                            
   ///   @return a reference to this charge                                   
   LANGULUS(INLINED)
   constexpr Charge& Charge::operator ^= (const Real& scalar) noexcept {
      mRate *= scalar;
      return *this;
   }

}
