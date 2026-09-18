///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Verb.hpp"
#include "VerbState.inl"
#include "../Charge.inl"
#include "../text/Text.hpp"


namespace Langulus::A
{
   
   /// Refer constructor                                                      
   ///   @param other - the verb to refer to                                  
   LANGULUS(INLINED)
   Verb::Verb(const Verb& other)
      : Verb {Refer(other)} {}

   /// Move constructor                                                       
   ///   @param other - the verb to move                                      
   LANGULUS(INLINED)
   Verb::Verb(Verb&& other)
      : Verb {Move(other)} {}
   
   /// Move constructor                                                       
   ///   @param other - the verb to move                                      
   template<template<class> class S>
   requires CT::Intent<S<Verb>> LANGULUS(INLINED)
   Verb::Verb(S<Verb>&& other) {
      Many::operator = (other.template Forward<Many>());
      Charge::operator = (*other);
      mVerb = other->mVerb;
      mState = other->mState;
      mSource = S<Many> {other->mSource};
      mOutput = S<Many> {other->mOutput};
   }

   /// Refer assignment                                                       
   ///   @param rhs - the verb to refer to                                    
   ///   @return a reference to this verb                                     
   LANGULUS(INLINED)
   Verb& Verb::operator = (const Verb& rhs) {
      return operator = (Refer(rhs));
   }

   /// Verb move-assignment                                                   
   ///   @param rhs - the verb to move-assign                                 
   ///   @return a reference to this verb                                     
   LANGULUS(INLINED)
   Verb& Verb::operator = (Verb&& rhs) {
      return operator = (Move(rhs));
   }

   /// Generic assignment                                                     
   ///   @param rhs - the verb/argument and intent to assign by               
   ///   @return a reference to this verb                                     
   template<template<class> class S>
   requires CT::Intent<S<Verb>> LANGULUS(INLINED)
   Verb& Verb::operator = (S<Verb>&& rhs) {
      Many::operator = (rhs.template Forward<Many>());
      Charge::operator = (*rhs);
      mVerb   = rhs->mVerb;
      mState  = rhs->mState;
      mSource = S<Many> {rhs->mSource};
      mOutput = S<Many> {rhs->mOutput};
      return *this;
   }
   
   /// Reset all verb members and energy                                      
   LANGULUS(INLINED)
   void Verb::Reset() {
      mVerb = {};
      Many::Reset();
      Charge::Reset();
      mSource.Reset();
      mOutput.Reset();
      mSuccesses = {};
   };

   /// Check if a verb is valid for the given priority                        
   ///   @param priority - the priority to check                              
   ///   @return true if this verb's priority matches the provided one        
   LANGULUS(INLINED)
   bool Verb::Validate(Annies::Index priority) const noexcept {
      return int(mPriority) == priority.mIndex
          or priority == Annies::IndexAll;
   }
   
   /// Check if verb has been satisfied at least once                         
   ///   @return true if verb has been satisfied at least once                
   LANGULUS(INLINED)
   bool Verb::IsDone() const noexcept {
      return mSuccesses > 0;
   }

   /// Check if verb is multicast                                             
   ///   @return true if verb is multicast                                    
   /*LANGULUS(INLINED)
   constexpr bool Verb::IsMulticast() const noexcept {
      return mState.IsMulticast();
   }

   /// Check if verb is monocast                                              
   ///   @return true if verb is monocast                                     
   LANGULUS(INLINED)
   constexpr bool Verb::IsMonocast() const noexcept {
      return mState.IsMonocast();
   }*/

   /// Check if verb is short-circuited                                       
   ///   @return true if verb is short-circuited                              
   LANGULUS(INLINED)
   constexpr bool Verb::IsShortCircuited() const noexcept {
      return mState.IsShortCircuited();
   }

   /// Check if verb is long-circuited                                        
   ///   @return true if verb is long-circuited                               
   LANGULUS(INLINED)
   constexpr bool Verb::IsLongCircuited() const noexcept {
      return mState.IsLongCircuited();
   }

   /// Get the verb state                                                     
   ///   @return the verb state                                               
   LANGULUS(INLINED)
   auto Verb::GetVerbState() const noexcept -> VerbState {
      return mState;
   }
   
   /// Get the number of successful execution of the verb                     
   ///   @return the number of successful executions                          
   LANGULUS(INLINED)
   size_t Verb::GetSuccesses() const noexcept {
      return mSuccesses;
   }

   /// Check if anything inside the verb is missing on the surface level      
   ///   @return true if anything is missing                                  
   LANGULUS(INLINED)
   bool Verb::IsMissing() const noexcept {
      return mSource.IsMissing()
          or Many::IsMissing()
          or mOutput.IsMissing();
   }

   /// Check if anything inside the verb is missing deeply                    
   ///   @return true if anything is missing                                  
   LANGULUS(INLINED)
   bool Verb::IsMissingDeep() const noexcept {
      return mSource.IsMissingDeep()
          or Many::IsMissingDeep()
          or mOutput.IsMissingDeep();
   }

   /// Satisfy verb a number of times                                         
   LANGULUS(INLINED)
   void Verb::Done(size_t c) noexcept {
      mSuccesses = c;
   }

   /// Satisfy verb once                                                      
   LANGULUS(INLINED)
   void Verb::Done() noexcept {
      ++mSuccesses;
   }

   /// Reset verb satisfaction, clear output                                  
   LANGULUS(INLINED)
   void Verb::Undo() noexcept {
      mSuccesses = 0;
      mOutput.Reset();
   }

   /// Compare verb priorities                                                
   ///   @param rhs - the verb to compare against                             
   ///   @return true if rhs has larger or equal priority                     
   LANGULUS(INLINED)
   bool Verb::operator < (const Verb& ext) const noexcept {
      return mPriority < ext.mPriority;
   }

   /// Compare verb priorities                                                
   ///   @param rhs - the verb to compare against                             
   ///   @return true if rhs has smaller or equal priority                    
   LANGULUS(INLINED)
   bool Verb::operator > (const Verb& ext) const noexcept {
      return mPriority > ext.mPriority;
   }

   /// Compare verb priorities                                                
   ///   @param rhs - the verb to compare against                             
   ///   @return true if rhs has smaller priority                             
   LANGULUS(INLINED)
   bool Verb::operator >= (const Verb& ext) const noexcept {
      return mPriority >= ext.mPriority;
   }

   /// Compare verb priorities                                                
   ///   @param rhs - the verb to compare against                             
   ///   @return true if rhs has larger priority                              
   LANGULUS(INLINED)
   bool Verb::operator <= (const Verb& rhs) const noexcept {
      return mPriority <= rhs.mPriority;
   }
   
   /// Get the verb type                                                      
   ///   @attention verb might not be set yet, if taken from A::Verb          
   ///   @return the hash of the content                                      
   LANGULUS(INLINED)
   auto Verb::GetVerb() const noexcept -> VMeta {
      return mVerb;
   }

   /// Hash the verb                                                          
   ///   @return the hash of the content                                      
   LANGULUS(INLINED)
   auto Verb::GetHash() const -> Hash {
      return HashOf(mVerb, mSource, GetArgument(), mOutput);
   }

   /// Get the verb id and charge                                             
   ///   @return verb charge                                                  
   LANGULUS(INLINED)
   auto Verb::GetCharge() const noexcept -> const Charge& {
      return static_cast<const Annies::Charge&>(*this);
   }

   /// Get the verb mass (a.k.a. magnitude)                                   
   ///   @return the current mass                                             
   LANGULUS(INLINED)
   auto Verb::GetMass() const noexcept -> Real {
      return Charge::mMass;
   }

   /// Get the verb frequency                                                 
   ///   @return the current frequency                                        
   LANGULUS(INLINED)
   auto Verb::GetRate() const noexcept -> Real {
      return Charge::mRate;
   }

   /// Get the verb time                                                      
   ///   @return the current time                                             
   LANGULUS(INLINED)
   auto Verb::GetTime() const noexcept -> Real {
      return Charge::mTime;
   }

   /// Get the verb priority                                                  
   ///   @return the current priority                                         
   LANGULUS(INLINED)
   auto Verb::GetPriority() const noexcept -> Real {
      return Charge::mPriority;
   }

   /// Get verb source                                                        
   ///   @return the verb source                                              
   LANGULUS(INLINED)
   auto Verb::GetSource() noexcept -> Many& {
      return mSource;
   }

   /// Get verb source (constant)                                             
   ///   @return the verb source                                              
   LANGULUS(INLINED)
   auto Verb::GetSource() const noexcept -> const Many& {
      return mSource;
   }

   /// Get verb argument                                                      
   ///   @return the verb argument                                            
   LANGULUS(INLINED)
   auto Verb::GetArgument() noexcept -> Many& {
      return static_cast<Many&>(*this);
   }

   /// Get verb argument (constant)                                           
   ///   @return the verb argument                                            
   LANGULUS(INLINED)
   auto Verb::GetArgument() const noexcept -> const Many& {
      return static_cast<const Many&>(*this);
   }

   /// Get verb output                                                        
   ///   @return the verb output                                              
   LANGULUS(INLINED)
   auto Verb::GetOutput() noexcept -> Many& {
      return mOutput;
   }

   /// Get verb output (constant)                                             
   ///   @return the verb output                                              
   LANGULUS(INLINED)
   auto Verb::GetOutput() const noexcept -> const Many& {
      return mOutput;
   }

   /// Convenience operator for accessing the output container inside verb    
   ///   @return the verb output                                              
   LANGULUS(INLINED)
   auto Verb::operator -> () const noexcept -> const Many* {
      return &mOutput;
   }

   /// Convenience operator for accessing the output container inside verb    
   ///   @return the verb output                                              
   LANGULUS(INLINED)
   auto Verb::operator -> () noexcept -> Many* {
      return &mOutput;
   }
   
   /// Serialize verb to any form of text                                     
   ///   @param out - [in/out] the serialized verb                            
   void Verb::SerializeVerb(CT::Serial auto& out) const {
      using OUT = Deref<decltype(out)>;
      using Rules = typename OUT::SerializationRules;
      if (mSuccesses and mOutput) {
         // If verb has been executed with output, just dump the output 
         mOutput.Serialize(out);
         return;
      }

      // If reached, then verb hasn't been executed yet                 
      // Let's check if there's a source in which verb is executed      
      if (mSource.IsValid()) {
         Rules::BeginScope(mSource, out);
         mSource.Serialize(out);
         Rules::EndScope(mSource, out);
      }

      // After the source, we decide whether to write verb token or     
      // verb operator, depending on the verb definition, state and     
      // charge                                                         
      bool writtenAsToken = false;
      const auto token = GetOperatorToken(writtenAsToken);
      if (writtenAsToken and mSource.IsValid())
         out += ' ';
      out += token;

      if (not IsValid())
         return;
      
      if (not Rules::BeginScope(GetArgument(), out) and writtenAsToken)
         out += ' ';

      GetArgument().Serialize(out);
      Rules::EndScope(GetArgument(), out);
   }

   /// Serialize verb for logger                                              
   LANGULUS(INLINED)
   Verb::operator Annies::Text() const {
      Annies::Text result;
      SerializeVerb(result);
      return result;
   }
   
   /// Compare verbs                                                          
   ///   @param rhs - the verb to compare against                             
   ///   @return true if verbs match                                          
   LANGULUS(INLINED)
   bool Verb::operator == (const Verb& rhs) const {
      return mVerb == rhs.mVerb
         and mSource == rhs.mSource
         and Many::operator == (rhs.GetArgument())
         and mOutput == rhs.mOutput
         and mState == rhs.mState;
   }

   /// Compare verb types for equality                                        
   ///   @param rhs - the verb to compare against                             
   ///   @return true if verbs match                                          
   LANGULUS(INLINED)
   bool Verb::operator == (VMeta rhs) const noexcept {
      return mVerb == rhs;
   }
   
   /// Get the verb token or operator depending on context and energy         
   ///   @return the token as a literal                                       
   LANGULUS(INLINED)
   auto Verb::GetOperatorToken(bool& tokenized) const -> Text {
      Text out;
      if (not mVerb) {
         // An invalid verb is always written as token                  
         out += NameOf<Verb>();
         tokenized = true;
         return out.Lowercase();
      }

      // A valid verb is written either as token, or as operator        
      if (mMass < 0) {
         if (not mVerb->mOperatorReverse.empty()
         and (GetCharge().operator*(-1)).IsDefault()) {
            // Write as operator                                        
            out += mVerb->mOperatorReverse;
         }
         else if (not mVerb->mTokenReverse.empty()) {
            // Write as token                                           
            out += mVerb->mTokenReverse;
            out += static_cast<Text>(GetCharge().operator*(-1));
            tokenized = true;
         }
         else {
            // Write as token                                           
            out += mVerb->mToken;
            out += static_cast<Text>(GetCharge());
            tokenized = true;
         }
      }
      else {
         if (not mVerb->mOperator.empty() and GetCharge().IsDefault()) {
            // Write as operator                                        
            out += mVerb->mOperator;
         }
         else {
            // Write as token                                           
            out += mVerb->mToken;
            out += static_cast<Text>(GetCharge());
            tokenized = true;
         }
      }

      return out.Lowercase();
   }

} // namespace Langulus::A