///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "DataState.hpp"


namespace Langulus
{

   /// Manual construction                                                    
   ///   @param state - the state                                             
   LANGULUS(INLINED)
   constexpr DataState::DataState(const Type& state) noexcept
      : mState {state} {}

   /// Explicit convertion to bool                                            
   ///   @return true if state is not default                                 
   LANGULUS(INLINED)
   constexpr DataState::operator bool() const noexcept {
      return not IsDefault();
   }

   /// Combine two states                                                     
   ///   @param rhs - the other state                                         
   ///   @return a new combined state                                         
   LANGULUS(INLINED)
   constexpr DataState DataState::operator + (const DataState& rhs) const noexcept {
      return mState | rhs.mState;
   }

   /// Remove rhs state from this state                                       
   ///   @param rhs - the other state                                         
   ///   @return a new leftover state                                         
   LANGULUS(INLINED)
   constexpr DataState DataState::operator - (const DataState& rhs) const noexcept {
      return mState & (~rhs.mState);
   }

   /// Destructively add state                                                
   ///   @param rhs - the other state                                         
   ///   @return a reference to this state                                    
   LANGULUS(INLINED)
   constexpr DataState& DataState::operator += (const DataState& rhs) noexcept {
      mState |= rhs.mState;
      return *this;
   }

   /// Destructively remove state                                             
   ///   @param rhs - the other state                                         
   ///   @return a reference to this state                                    
   LANGULUS(INLINED)
   constexpr DataState& DataState::operator -= (const DataState& rhs) noexcept {
      mState &= ~rhs.mState;
      return *this;
   }

   /// Destructively AND state                                                
   ///   @param rhs - the other state                                         
   ///   @return a reference to this state                                    
   LANGULUS(INLINED)
   constexpr DataState& DataState::operator &= (const DataState& rhs) noexcept {
      mState &= rhs.mState;
      return *this;
   }

   LANGULUS(INLINED)
   constexpr bool DataState::operator & (const DataState& rhs) const noexcept {
      return (mState & rhs.mState) == rhs.mState;
   }

   LANGULUS(INLINED)
   constexpr bool DataState::operator % (const DataState& rhs) const noexcept {
      return (mState & rhs.mState) == 0;
   }

   /// Check if default data state                                            
   /// Default state is inclusive, mutable, nonpolar, nonvacuum, nonstatic,   
   /// nonencrypted, noncompressed, untyped, and dense                        
   LANGULUS(INLINED)
   constexpr bool DataState::IsDefault() const noexcept {
      return mState == DataState::Default;
   }

   /// Check if state is marked missing                                       
   LANGULUS(INLINED)
   constexpr bool DataState::IsMissing() const noexcept {
      return mState & DataState::Missing;
   }

   /// Check if data is compressed                                            
   LANGULUS(INLINED)
   constexpr bool DataState::IsCompressed() const noexcept {
      return mState & DataState::Compressed;
   }

   /// Check if data is encrypted                                             
   LANGULUS(INLINED)
   constexpr bool DataState::IsEncrypted() const noexcept {
      return mState & DataState::Encrypted;
   }

   /// Check if data is marked exlusive (OR)                                  
   LANGULUS(INLINED)
   constexpr bool DataState::IsOr() const noexcept {
      return mState & DataState::Or;
   }

   /// Check if data is not missing                                           
   LANGULUS(INLINED)
   constexpr bool DataState::IsNow() const noexcept {
      return not IsMissing();
   }

   /// Check if data is future-phased                                         
   LANGULUS(INLINED)
   constexpr bool DataState::IsFuture() const noexcept {
      return IsMissing() and 0 != (mState & DataState::Future);
   }

   /// Check if data is past-phased                                           
   LANGULUS(INLINED)
   constexpr bool DataState::IsPast() const noexcept {
      return IsMissing() and 0 == (mState & DataState::Future);
   }

   /// Check if data is static (size-constrained)                             
   /*LANGULUS(INLINED)
   constexpr bool DataState::IsStatic() const noexcept {
      return mState & DataState::Static;
   }*/

   /// Check if data is constant (change-constrained)                         
   LANGULUS(INLINED)
   constexpr bool DataState::IsConstant() const noexcept {
      return mState & DataState::Constant;
   }

   /// Check if data is type-constrained                                      
   LANGULUS(INLINED)
   constexpr bool DataState::IsTyped() const noexcept {
      return mState & DataState::Typed;
   }

   /// Check if data is either size-, change- or type-constrained             
   LANGULUS(INLINED)
   constexpr bool DataState::IsConstrained() const noexcept {
      return mState & DataState::Constrained;
   }

   /// Reset the state to default                                             
   LANGULUS(INLINED)
   constexpr void DataState::Reset() noexcept {
      mState = DataState::Default;
   }

} // namespace Langulus
