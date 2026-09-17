///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Construct.hpp"
#include "../text/Text.hpp"


namespace Langulus::Annies
{
   
   /// Refer constructor                                                      
   ///   @param other - construct to refer to                                 
   LANGULUS(INLINED)
   Construct::Construct(const Construct& other) noexcept
      : Construct    {Refer(other)} {}

   /// Move constructor                                                       
   ///   @param other - construct to move                                     
   LANGULUS(INLINED)
   Construct::Construct(Construct&& other) noexcept
      : Construct    {Move(other)} {}

   /// Intent constructor                                                     
   ///   @param other - the construct and intent                              
   template<template<class> class S> requires CT::Intent<S<Construct>>
   LANGULUS(INLINED) Construct::Construct(S<Construct>&& other)   
      : mType        {other->mType}
      , mHash        {other->mHash}
      , mCharge      {other->mCharge}
      , mDescriptor  {other.Nest(other->mDescriptor)} {
      if constexpr (ResetsOnMove<S>()) {
         other->mType = {};
         other->mHash = {};
         other->ResetCharge();
      }
   }

   /// Construct from a type                                                  
   ///   @param type - the type of the content                                
   LANGULUS(INLINED)
   Construct::Construct(DMeta type)
      : mType        {type ? type->mOrigin : nullptr} {}

   /// Manual constructor                                                     
   ///   @param type - the type of the construct                              
   ///   @param args - the arguments for construction, with or without intent 
   ///   @param charge - the charge for the construction                      
   LANGULUS(INLINED)
   Construct::Construct(DMeta type, auto&& args, const Charge& charge)
      : mType        {type ? type->mOrigin : nullptr}
      , mCharge      {charge}
      , mDescriptor  {Forward<Deref<decltype(args)>>(args)} {}

#if LANGULUS_FEATURE(MANAGED_REFLECTION)
   /// Construct from a type token                                            
   ///   @param token - the type name of the content                          
   LANGULUS(INLINED)
   Construct::Construct(const Token& token)
      : mType        {RTTI::GetMetaData(token)->mOrigin} {}

   LANGULUS(INLINED)
   /// Construct from a type token                                            
   ///   @param token - the type name of the content                          
   ///   @param args - the arguments for construction, with or without intent 
   ///   @param charge - the charge for the construction                      
   Construct::Construct(const Token& token, auto&& args, const Charge& charge)
      : Construct {
         RTTI::GetMetaData(token),
         Forward<Deref<decltype(args)>>(args),
         charge
      } {}
#endif

   /// Refer-assignment                                                       
   ///   @param rhs - the construct to shallow-copy                           
   ///   @return a reference to this construct                                
   LANGULUS(INLINED)
   Construct& Construct::operator = (const Construct& rhs) noexcept {
      return operator = (Refer(rhs));
   }

   /// Move-assignment                                                        
   ///   @param rhs - the construct to move                                   
   ///   @return a reference to this construct                                
   LANGULUS(INLINED)
   Construct& Construct::operator = (Construct&& rhs) noexcept {
      return operator = (Move(rhs));
   }

   /// Intent-assignment                                                      
   ///   @param rhs - the right hand side                                     
   ///   @return a reference to this construct                                
   template<template<class> class S> requires CT::Intent<S<Construct>>
   LANGULUS(INLINED) Construct& Construct::operator = (S<Construct>&& rhs) {
      mType       = rhs->mType;
      mHash       = rhs->mHash;
      mCharge     = rhs->mCharge;
      mDescriptor = rhs.Nest(rhs->mDescriptor);

      if constexpr (ResetsOnMove<S>()) {
         rhs->mType = {};
         rhs->mHash = {};
         rhs->ResetCharge();
      }

      return *this;
   }

   /// Create content descriptor from a static type and arguments with intent 
   ///   @tparam T - type of the construct                                    
   ///   @param t1, tn  - the constructor arguments                           
   ///   @return the request                                                  
   template<CT::NotVoid T, CT::NotVoid T1, CT::NotVoid...TN> LANGULUS(INLINED)
   Construct Construct::From(T1&& t1, TN&&...tn) {
      static_assert(CT::Decayed<T>, "T must be fully decayed");
      return {
         MetaDataOf<T>(),
         Many {Forward<T1>(t1), Forward<TN>(tn)...}
      };
   }

   /// Create content descriptor from a static type (without arguments)       
   ///   @tparam T - type of the construct                                    
   ///   @return the request                                                  
   template<CT::NotVoid T> LANGULUS(INLINED)
   Construct Construct::From() {
      static_assert(CT::Decayed<T>, "T must be fully decayed");
      return Construct {MetaDataOf<T>()};
   }

#if LANGULUS_FEATURE(MANAGED_REFLECTION)
   /// Create content descriptor from a type token and arguments with intents 
   ///   @param token - the type name for the construct                       
   ///   @param t1, tn  - the constructor arguments                           
   ///   @return the request                                                  
   template<CT::NotVoid T1, CT::NotVoid...TN> LANGULUS(INLINED)
   Construct Construct::FromToken(const Token& token, T1&& t1, TN&&...tn) {
      return {
         RTTI::DisambiguateMeta(token),
         Many {Forward<T1>(t1), Forward<TN>(tn)...}
      };
   }

   /// Create content descriptor from a type token (without arguments)        
   ///   @param token - type of the construct                                 
   ///   @return the request                                                  
   LANGULUS(INLINED)
   Construct Construct::FromToken(const Token& token) {
      return Construct {RTTI::DisambiguateMeta(token)};
   }
#endif
   
   /// Rehash the construct                                                   
   /// The hash is (mostly) cached, so this is a (relatively) cheap function  
   ///   @return the hash of the content                                      
   LANGULUS(INLINED)
   Hash Construct::GetHash() const {
      // Hash is the same as the Neat base, but with the type on top    
      if (mHash)
         return mHash;

      if (mType) {
         if (mDescriptor.IsEmpty())
            mHash = HashOf(mType->mDecvq);
         else
            mHash = HashOf(mType->mDecvq, mDescriptor);
      }
      else mHash = mDescriptor.GetHash();
      return mHash;
   }

   /// Clears arguments and charge, but doesn't deallocate                    
   LANGULUS(INLINED)
   void Construct::Clear() {
      mHash = {};
      mCharge.Reset();
      mDescriptor.Clear();
   }
   
   /// Clears and deallocates arguments and charge                            
   LANGULUS(INLINED)
   void Construct::Reset() {
      mHash = {};
      mCharge.Reset();
      mDescriptor.Reset();
   }

   /// Reset charge                                                           
   LANGULUS(INLINED)
   void Construct::ResetCharge() noexcept {
      mCharge.Reset();
   }

   /// Compare constructs                                                     
   ///   @param rhs - descriptor to compare with                              
   ///   @return true if both constructs are the same                         
   LANGULUS(INLINED)
   bool Construct::operator == (const Construct& rhs) const {
      VERBOSE_COMPARE_TAB("Comparing Construct of type ",
         Logger::PushWhite, mType, Logger::Pop, " with construct of type ",
         Logger::PushWhite, rhs.mType
      );

      if (GetHash() != rhs.GetHash())
         VERBOSE_COMPARE(Logger::Red, "Hashes differ: ", GetHash(), " != ", rhs.GetHash());
      if (!(mType & rhs.mType))
         VERBOSE_COMPARE(Logger::Red, "Types not compatible");
      if (mDescriptor != rhs.mDescriptor)
         VERBOSE_COMPARE(Logger::Red, "Contents differ");

      return GetHash() == rhs.GetHash()
         and mType & rhs.mType
         and mDescriptor == rhs.mDescriptor;
   }
   
   /// Check if construct type can be interpreted as a given static type      
   ///   @tparam T - type of the construct to compare against                 
   template<CT::NotVoid T> LANGULUS(INLINED)
   bool Construct::CastsTo() const {
      return mType ? CastsTo(MetaDataOf<T>()) : false;
   }

   /// Check if construct type can be interpreted as another type             
   ///   @param type - the type check if current type interprets to           
   ///   @return true if able to interpret current header to 'type'           
   LANGULUS(INLINED)
   bool Construct::CastsTo(DMeta type) const {
      return mType & type or mType->CastsTo(type);
   }
   
   /// Check if construct type is similar to another type                     
   ///   @tparam T - the type to check for                                    
   template<CT::NotVoid T> LANGULUS(INLINED)
   bool Construct::Is() const {
      return mType ? Is(MetaDataOf<T>()) : false;
   }

   /// Check if construct type is similar to another type                     
   ///   @param type - the type to check for                                  
   ///   @return true if current header is similar to 'type'                  
   LANGULUS(INLINED)
   bool Construct::Is(DMeta type) const {
      return mType & type;
   }
   
   /// Change the type of the construct                                       
   ///   @attention will cause a rehash if type differs                       
   ///   @tparam T - the new type of the construct                            
   template<CT::NotVoid T> LANGULUS(INLINED)
   void Construct::SetType() {
      SetType(MetaDataOf<T>());
   }

   /// Check if constructor header is exactly another type                    
   ///   @param type - the type to check for (must be a dense type)           
   ///   @return true if current header is 'type'                             
   LANGULUS(INLINED)
   void Construct::SetType(DMeta type) noexcept {
      mType = type;
      mHash = {};
   }

   /// Get the argument for the construct                                     
   ///   @return the constant arguments container                             
   LANGULUS(INLINED)
   auto Construct::GetDescriptor() const noexcept -> const Many& {
      return mDescriptor;
   }

   LANGULUS(INLINED)
   auto Construct::operator -> () const -> const Many* {
      return &mDescriptor;
   }

   /// Get the argument for the construct                                     
   ///   @attention this will force a rehash to guarantee that any changes    
   ///      made through the mutable Neat reference are accounted for on next 
   ///      GetHash() request                                                 
   ///   @return the mutable arguments container                              
   LANGULUS(INLINED)
   auto Construct::GetDescriptor() noexcept -> Many& {
      // Reset hash preventively, because we're exposing a mutable ref, 
      // which is likely to change, and thus a rehash will be needed    
      mHash = {};
      return mDescriptor;
   }

   LANGULUS(INLINED)
   auto Construct::operator -> () -> Many* {
      return &mDescriptor;
   }

   /// Get construct's charge                                                 
   ///   @return the charge                                                   
   LANGULUS(INLINED)
   auto Construct::GetCharge() noexcept -> Charge& {
      return mCharge;
   }

   /// Get construct's charge (const)                                         
   ///   @return the charge                                                   
   LANGULUS(INLINED)
   auto Construct::GetCharge() const noexcept -> const Charge& {
      return mCharge;
   }

   /// Get the type of the construct                                          
   ///   @return the type                                                     
   LANGULUS(INLINED)
   DMeta Construct::GetType() const noexcept {
      return mType;
   }
   
   /// Get the token of the construct's type                                  
   ///   @return the token, if type is set, or default token if not           
   LANGULUS(INLINED)
   Token Construct::GetToken() const noexcept {
      return mType.GetToken();
   }

   /// Get the producer of the construct                                      
   ///   @return the type of the producer                                     
   LANGULUS(INLINED)
   DMeta Construct::GetProducer() const noexcept {
      if (mType and mType->mProducerRetriever)
         return mType->mProducerRetriever();
      return nullptr;
   }

   /// Check if construct contains executable elements                        
   LANGULUS(INLINED)
   bool Construct::IsExecutable() const noexcept {
      return mDescriptor.IsExecutable();
   }
   
   /// Check if construct is typed                                            
   LANGULUS(INLINED)
   bool Construct::IsTyped() const noexcept {
      return not not mType;
   }
   
   /// Check if construct is untyped                                          
   LANGULUS(INLINED)
   bool Construct::IsUntyped() const noexcept {
      return not mType;
   }

   /// Push anything to the descriptor                                        
   ///   @attention resets hash                                               
   ///   @param rhs - stuff to push                                           
   ///   @return a reference to this construct for chaining                   
   template<class T> LANGULUS(INLINED)
   Construct& Construct::operator << (T&& rhs) {
      if constexpr (requires {mDescriptor << Forward<T>(rhs); }) {
         mDescriptor << Forward<T>(rhs);
         mHash = {};
         return *this;
      }
      else static_assert(false, "Can't insert that into descriptor");
   }

   /// Merge anything to the descriptor                                       
   ///   @attention resets hash                                               
   ///   @param rhs - stuff to merge                                          
   ///   @return a reference to this construct for chaining                   
   template<class T> LANGULUS(INLINED)
   Construct& Construct::operator <<= (T&& rhs) {
      if constexpr (requires {mDescriptor <<= Forward<T>(rhs); }) {
         mDescriptor <<= Forward<T>(rhs);
         mHash = {};
         return *this;
      }
      else static_assert(false, "Can't merge that into descriptor");
   }

   /// Serialize the construct to anything text-based                         
   ///   @param to - [in/out] container to serialize into                     
   ///   @return the number of elements that were written to 'to'             
   Count Construct::Serialize(CT::Serial auto& to) const {
      const auto initial = to.GetCount();
      using OUT = Deref<decltype(to)>;
      to += GetType();
      to += static_cast<Text>(GetCharge());
      to += OUT::Operator::OpenScope;
      GetDescriptor().Serialize(to);
      to += OUT::Operator::CloseScope;
      return to.GetCount() - initial;
   }

} // namespace Langulus::Annies
