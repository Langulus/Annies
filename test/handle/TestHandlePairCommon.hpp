///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "TestHandleCommon.hpp"


/// MARK: TestType                                                            
/// Tests if a container is of a particular type                              
template<class E1, class E2, CT::Container C> requires CT::NoIntent<C>
void HandlePair_Helper_TestType(const C& any) {
   Handle_Helper_TestType<E1>(any.GetKeyHandle());
   Handle_Helper_TestType<E2>(any.GetValHandle());
}

/// MARK: TestSame                                                            
/// Tests if two containers point to the same memory the same way             
template<class LHS, class RHS> requires (CT::Container<LHS, RHS> and CT::NoIntent<LHS, RHS>)
void HandlePair_Helper_TestSame(const LHS& lhs, const RHS& rhs, bool match_constness = true) {
   Any_Helper_TestSame(lhs.GetKeyHandle(), rhs.GetKeyHandle(), match_constness);
   Any_Helper_TestSame(lhs.GetValHandle(), rhs.GetValHandle(), match_constness);
}

/// MARK: Default                                                             
template<class E1, class E2, CT::Container C> requires CT::NoIntent<C>
void HandlePair_CheckState_Default(const C& any) {
   Handle_CheckState_Default<E1>(any.GetKeyHandle());
   Handle_CheckState_Default<E2>(any.GetValHandle());
}

/// MARK: OwnedEmpty                                                          
template<class E1, class E2, CT::Container C> requires CT::NoIntent<C>
void HandlePair_CheckState_OwnedEmpty(const C& any) {
   Handle_CheckState_Default<E1>(any.GetKeyHandle());
   Handle_CheckState_Default<E2>(any.GetValHandle());
}

/// MARK: OwnedFull                                                           
template<class E1, class E2, CT::Container C> requires CT::NoIntent<C>
void HandlePair_CheckState_OwnedFull(const C& any, bool onTheStack = false) {
   Handle_CheckState_OwnedFull<E1>(any.GetKeyHandle(), onTheStack);
   Handle_CheckState_OwnedFull<E2>(any.GetValHandle(), onTheStack);
}

/// MARK: DisownedFull                                                        
template<class E1, class E2, CT::Container C> requires CT::NoIntent<C>
void HandlePair_CheckState_DisownedFull(const C& any) {
   Handle_CheckState_DisownedFull<E1>(any.GetKeyHandle());
   Handle_CheckState_DisownedFull<E2>(any.GetValHandle());
}

/// MARK: Abandoned                                                           
template<class E1, class E2, CT::Container C> requires CT::NoIntent<C>
void HandlePair_CheckState_Abandoned(const C& any) {
   Handle_CheckState_Abandoned<E1>(any.GetKeyHandle());
   Handle_CheckState_Abandoned<E2>(any.GetValHandle());
}

/// MARK: ContainsOne                                                         
template<CT::Container T, CT::Intent I1, CT::Intent I2> requires CT::NoIntent<T>
void HandlePair_CheckState_ContainsOne(T const& any, I1&& e1_with_intent, I2&& e2_with_intent, int uses = 1) {
   Handle_CheckState_ContainsOne(any.GetKeyHandle(), e1_with_intent, uses);
   Handle_CheckState_ContainsOne(any.GetValHandle(), e2_with_intent, uses);
}

/// MARK: CompareOne                                                          
template<CT::Container T, class E1, class E2> requires CT::NoIntent<T>
void HandlePair_Helper_CompareOne(const T& any, const E1& e1, const E2& e2) {
   Handle_Helper_CompareOne(any.GetKeyHandle(), e1);
   Handle_Helper_CompareOne(any.GetValHandle(), e2);
}
