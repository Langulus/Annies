///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           

/// INTENTIONALLY NOT GUARDED                                                 
/// Include this file once in each cpp file, after all other headers          
#include <Langulus/Annies/Text.hpp>
#include <Langulus/Annies/Trait.hpp>
#include <Langulus/Annies/TSet.hpp>
#include <Langulus/Annies/Set.hpp>
#include <unordered_set>
#include "../Common.hpp"


///                                                                           
/// Possible states:                                                          
///   - uninitialized                                                         
///   - default                                                               
template<class K>
void Set_CheckState_Default(const auto&);
///   - invariant                                                             
template<class K>
void Set_CheckState_Invariant(const auto&);
///   - owned-full                                                            
template<class K>
void Set_CheckState_OwnedFull(const auto&);
///   - owned-full-const                                                      
template<class K>
void Set_CheckState_OwnedFullConst(const auto&);
///   - owned-empty                                                           
template<class K>
void Set_CheckState_OwnedEmpty(const auto&);
///   - disowned-full                                                         
template<class K>
void Set_CheckState_DisownedFull(const auto&);
///   - disowned-full-const                                                   
template<class K>
void Set_CheckState_DisownedFullConst(const auto&);
///   - abandoned                                                             
template<class K>
void Set_CheckState_Abandoned(const auto&);



template<class K>
void Set_Helper_TestType(const auto& set) {
   REQUIRE      (set.IsTyped());
   REQUIRE_FALSE(set.IsUntyped());

   REQUIRE      (set.GetType() == MetaDataOf<K>());
   REQUIRE      (set.GetType()->template IsSimilar<const K>());
   REQUIRE      (set.GetType()->template IsExact<K>());
   REQUIRE      (set.GetType()->template Is<K*>());
   REQUIRE      (set.IsDense() == CT::Dense<K>);
   REQUIRE      (set.IsSparse() == CT::Sparse<K>);
   REQUIRE      (set.IsDeep() == CT::Deep<Decay<K>>);
}

template<CT::Set LHS, CT::Set RHS>
void Set_Helper_TestSame(const LHS& lhs, const RHS& rhs) {
   REQUIRE(lhs.GetRaw() == rhs.GetRaw());
   REQUIRE(lhs.IsExact(rhs.GetType()));
   REQUIRE(lhs == rhs);
   REQUIRE(lhs.IsDeep() == rhs.IsDeep());
   REQUIRE(lhs.IsConstant() == rhs.IsConstant());
   REQUIRE(lhs.GetUnconstrainedState() == rhs.GetUnconstrainedState());
}



template<class K>
void Set_CheckState_Default(const auto& set) {
   using T = Decay<decltype(set)>;

   if constexpr (CT::Typed<T>) {
      static_assert(CT::Exact<TypeOf<T>, K>);
      Set_Helper_TestType<K>(set);
      REQUIRE      (set.GetState() == DataState::Typed);
   }
   else {
      REQUIRE_FALSE(set.IsTyped());
      REQUIRE      (set.IsUntyped());
      REQUIRE      (set.GetType() == nullptr);
      REQUIRE      (set.IsDense());
      REQUIRE_FALSE(set.IsSparse());
      REQUIRE      (set.GetState() == DataState::Default);
      REQUIRE_FALSE(set.IsDeep());
   }

   REQUIRE      (set.IsTypeConstrained() == CT::Typed<T>);
   REQUIRE_FALSE(set.IsCompressed());
   REQUIRE      (set.IsConstant() == CT::Constant<K>);
   REQUIRE_FALSE(set.IsEncrypted());
   REQUIRE_FALSE(set.IsMissing());
   REQUIRE_FALSE(set.IsValid());
   REQUIRE      (set.IsInvalid());
   REQUIRE_FALSE(set.IsAllocated());
   REQUIRE_FALSE(set.GetAllocation());
   REQUIRE      (set.IsEmpty());
   REQUIRE      (set.GetCount() == 0);
   REQUIRE      (set.GetReserved() == 0);
   REQUIRE      (set.GetUses() == 0);
   REQUIRE      (set.GetRawMemory() == nullptr);
   REQUIRE_FALSE(set);
   REQUIRE      (not set);
}

template<class K>
void Set_CheckState_OwnedEmpty(const auto& set) {
   using T = Decay<decltype(set)>;

   Set_Helper_TestType<K>(set);

   REQUIRE      (set.IsTypeConstrained() == CT::Typed<T>);
   REQUIRE_FALSE(set.IsCompressed());
   REQUIRE      (set.IsConstant() == CT::Constant<K>);
   REQUIRE_FALSE(set.IsEncrypted());
   REQUIRE_FALSE(set.IsMissing());
   REQUIRE_FALSE(set.IsValid());
   REQUIRE      (set.IsInvalid());
   REQUIRE      (set.IsAllocated());
   REQUIRE      (set.GetAllocation());
   REQUIRE      (set.IsEmpty());
   REQUIRE      (set.GetCount() == 0);
   REQUIRE      (set.GetReserved() > 0);
   REQUIRE      (set.GetUses() == 1);
   REQUIRE      (set.GetRawMemory());
   REQUIRE_FALSE(set);
   REQUIRE      (not set);
}

template<class K>
void Set_CheckState_OwnedFull(const auto& set) {
   using T = Decay<decltype(set)>;

   Set_Helper_TestType<K>(set);

   REQUIRE      (set.IsTypeConstrained() == CT::Typed<T>);
   REQUIRE_FALSE(set.IsCompressed());
   REQUIRE      (set.IsConstant() == CT::Constant<K>);
   REQUIRE_FALSE(set.IsEncrypted());
   REQUIRE_FALSE(set.IsMissing());
   REQUIRE      (set.IsValid());
   REQUIRE_FALSE(set.IsInvalid());
   REQUIRE      (set.IsAllocated());
   REQUIRE      (set.GetAllocation());
   REQUIRE_FALSE(set.IsEmpty());
   REQUIRE      (set.GetCount() > 0);
   REQUIRE      (set.GetReserved() > 0);
   REQUIRE      (set.GetUses() > 0);
   REQUIRE      (set.GetRawMemory());
   REQUIRE      (set);
   REQUIRE_FALSE(not set);
}