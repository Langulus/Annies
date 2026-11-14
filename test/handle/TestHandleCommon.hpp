///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../any/TestAnyCommon.hpp"


/// MARK: TestType                                                            
/// Tests if a container is of a particular type                              
template<class E, CT::Container C> requires CT::NoIntent<C>
void Handle_Helper_TestType(const C& any) {
   /// @attention constness for type-erased handles is achieved         
   /// through either the lack or presence of an Assignment component,  
   /// in order to avoid the overhead of navigating to the const type.  
   REQUIRE(any.template IsSame<E>());
   REQUIRE(any.template Is<E>());
   REQUIRE(any.IsSparse() == CT::Sparse<E>);
   REQUIRE(any.IsDeep() == CT::Deep<E>);
   REQUIRE(any.GetType().GetOrigin() == MetaDataOf<Decay<E>>());
   REQUIRE(any.IsTyped());
}

/// MARK: TestSame                                                            
/// Tests if two containers point to the same memory the same way             
template<class LHS, class RHS> requires (CT::Container<LHS, RHS> and CT::NoIntent<LHS, RHS>)
void Handle_Helper_TestSame(const LHS& lhs, const RHS& rhs, bool match_constness = true) {
   Any_Helper_TestSame(lhs, rhs, match_constness);
}

/// MARK: Default                                                             
template<class E, CT::Container C> requires CT::NoIntent<C>
void Handle_CheckState_Default(const C& any/*, bool typed = false*/) {
   if constexpr (CT::Typed<C>) {
      Handle_Helper_TestType<E>(any);
   }
   /*else if (not typed) {
      REQUIRE_FALSE(any.IsTyped());
      REQUIRE      (any.GetType() == nullptr);
      REQUIRE_FALSE(any.IsSparse());
      REQUIRE_FALSE(any.IsDeep());
   }*/
   else {
      REQUIRE_FALSE(any.IsTyped());
   }

   REQUIRE      (any.IsDefaultState());
   REQUIRE      (any.IsTypeConstrained()/* == CT::Typed<C>*/);
   REQUIRE      (any.IsConstant() == CT::Constant<E>);
   REQUIRE_FALSE(any.IsDisowned());
   REQUIRE_FALSE(any.IsValid());

   if constexpr (requires { any.GetAllocation(); }) {
      REQUIRE   (any.GetAllocation() == nullptr);
   }
   
   if constexpr (requires { any.GetEntries(); }) {
      REQUIRE   (any.GetEntries() == nullptr);
   }

   REQUIRE      (any.IsEmpty());
   REQUIRE      (any.GetCount() == 0);
   REQUIRE_FALSE(any);
   REQUIRE      (not any);
   REQUIRE      (any == C{});

   if constexpr (requires { any.IsMissing(); }
   or            requires { any.IsFuture();  }
   or            requires { any.IsPast();    }) {
      REQUIRE_FALSE(any.IsMissing());
      REQUIRE_FALSE(any.IsFuture());
      REQUIRE_FALSE(any.IsPast());
   }
}

/// MARK: OwnedEmpty                                                          
template<class E, CT::Container C> requires CT::NoIntent<C>
void Handle_CheckState_OwnedEmpty(const C& h) {
   Any_CheckState_OwnedEmpty<E>(h);
}

/// MARK: OwnedFull                                                           
template<class E, CT::Container C> requires CT::NoIntent<C>
void Handle_CheckState_OwnedFull(const C& any, bool onTheStack = false) {
   Handle_Helper_TestType<E>(any);

   REQUIRE      (any.IsTypeConstrained()/* == CT::Typed<C>*/);
   REQUIRE      (any.IsConstant() == CT::Constant<E>);
   REQUIRE      (any.IsValid());

   if constexpr (requires { any.GetAllocation(); }) {
      if (onTheStack) {
         REQUIRE(any.GetAllocation() == nullptr);
         REQUIRE(any.GetUses() == 0);
      }
      else {
         REQUIRE(any.GetAllocation());
         REQUIRE(any.GetUses() > 0);
      }
   }
   
   if constexpr (requires { any.GetEntries(); }) {
      if constexpr (CT::Sparse<E>)
         REQUIRE(any.GetEntries());
      else
         REQUIRE(any.GetEntries() == nullptr);
   }

   REQUIRE_FALSE(any.IsDisowned());
   REQUIRE_FALSE(any.IsEmpty());
   REQUIRE      (any.GetCount() > 0);

   REQUIRE      (any.GetRaw());
   REQUIRE      (any);
   REQUIRE_FALSE(not any);

   if constexpr (C::CountHeapProviders() > 0) {
      REQUIRE(any != C{}); // can be true for stack-based handles that are default-initialized to zero, when 'any' contains zero as well
   }
}

/// MARK: DisownedFull                                                        
template<class E, CT::Container C> requires CT::NoIntent<C>
void Handle_CheckState_DisownedFull(const C& h) {
   Any_CheckState_DisownedFull<E>(h);
}

/// MARK: Abandoned                                                           
template<class E, CT::Container C> requires CT::NoIntent<C>
void Handle_CheckState_Abandoned(const C& h) {
   Any_CheckState_Abandoned<E>(h);
}

/// MARK: ContainsOne                                                         
template<CT::Container T, CT::Intent I> requires CT::NoIntent<T>
void Handle_CheckState_ContainsOne(T const& h, I&& e_with_intent, int uses = 1) {
   Any_CheckState_ContainsOne(h, e_with_intent, uses);
}

/// MARK: CompareOne                                                          
template<CT::Container T, class E> requires CT::NoIntent<T>
void Handle_Helper_CompareOne(const T& h, const E& e) {
   Any_Helper_CompareOne(h, e);
}
