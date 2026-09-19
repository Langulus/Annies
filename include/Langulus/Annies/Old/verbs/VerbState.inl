///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "VerbState.hpp"


namespace Langulus::Annies
{
   
   /// Manual construction                                                    
   ///   @param state - the state                                             
   LANGULUS(INLINED)
   constexpr VerbState::VerbState(const Type& state) noexcept
      : mState {state} {}

   /// Explicit convertion to bool                                            
   ///   @return true if state is not default                                 
   LANGULUS(INLINED)
   constexpr VerbState::operator bool() const noexcept {
      return not IsDefault();
   }
   
   /// Combine two states                                                     
   ///   @param rhs - the other state                                         
   ///   @return a new combined state                                         
   LANGULUS(INLINED)
   constexpr VerbState VerbState::operator + (const VerbState& rhs) const noexcept {
      return mState | rhs.mState;
   }
   
   /// Remove rhs state from this state                                       
   ///   @param rhs - the other state                                         
   ///   @return a new leftover state                                         
   LANGULUS(INLINED)
   constexpr VerbState VerbState::operator - (const VerbState& rhs) const noexcept {
      return mState & (~rhs.mState);
   }
   
   /// Destructively add state                                                
   ///   @param rhs - the other state                                         
   ///   @return a reference to this state                                    
   LANGULUS(INLINED)
   constexpr VerbState& VerbState::operator += (const VerbState& rhs) noexcept {
      mState |= rhs.mState;
      return *this;
   }
   
   /// Destructively remove state                                             
   ///   @param rhs - the other state                                         
   ///   @return a reference to this state                                    
   LANGULUS(INLINED)
   constexpr VerbState& VerbState::operator -= (const VerbState& rhs) noexcept {
      mState &= ~rhs.mState;
      return *this;
   }
   
   LANGULUS(INLINED)
   constexpr bool VerbState::operator & (const VerbState& rhs) const noexcept {
      return (mState & rhs.mState) == rhs.mState;
   }
   
   LANGULUS(INLINED)
   constexpr bool VerbState::operator % (const VerbState& rhs) const noexcept {
      return (mState & rhs.mState) == 0;
   }
   
   /// Check if default data state                                            
   /// Default state is inclusive, mutable, nonpolar, nonvacuum, nonstatic,   
   /// nonencrypted, noncompressed, untyped, and dense                        
   LANGULUS(INLINED)
   constexpr bool VerbState::IsDefault() const noexcept {
      return mState == VerbState::Default;
   }
   
   /// Check if state is multicast                                            
   /*LANGULUS(INLINED)
   constexpr bool VerbState::IsMulticast() const noexcept {
      return (mState & VerbState::Monocast) == 0;
   }
   
   /// Check if state is monocast                                             
   LANGULUS(INLINED)
   constexpr bool VerbState::IsMonocast() const noexcept {
      return mState & VerbState::Monocast;
   }*/
   
   /// Check if state is long-circuited                                       
   LANGULUS(INLINED)
   constexpr bool VerbState::IsLongCircuited() const noexcept {
      return mState & VerbState::LongCircuited;
   }
   
   /// Check if state is short-circuited                                      
   LANGULUS(INLINED)
   constexpr bool VerbState::IsShortCircuited() const noexcept {
      return (mState & VerbState::LongCircuited) == 0;
   }

} // namespace Langulus::Annies
