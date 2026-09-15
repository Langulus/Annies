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
#include <Langulus/Annies/TMap.hpp>
#include <Langulus/Annies/Map.hpp>
#include <unordered_map>
#include "../Common.hpp"


///                                                                           
/// Possible states:                                                          
///   - uninitialized                                                         
///   - default                                                               
template<class K, class V>
void Map_CheckState_Default(const auto&);
///   - invariant                                                             
template<class K, class V>
void Map_CheckState_Invariant(const auto&);
///   - owned-full                                                            
template<class K, class V>
void Map_CheckState_OwnedFull(const auto&);
///   - owned-full-const                                                      
template<class K, class V>
void Map_CheckState_OwnedFullConst(const auto&);
///   - owned-empty                                                           
template<class K, class V>
void Map_CheckState_OwnedEmpty(const auto&);
///   - disowned-full                                                         
template<class K, class V>
void Map_CheckState_DisownedFull(const auto&);
///   - disowned-full-const                                                   
template<class K, class V>
void Map_CheckState_DisownedFullConst(const auto&);
///   - abandoned                                                             
template<class K, class V>
void Map_CheckState_Abandoned(const auto&);



template<class K, class V>
void Map_Helper_TestType(const auto& map) {
   REQUIRE      (map.IsKeyTyped());
   REQUIRE      (map.IsValueTyped());
   REQUIRE_FALSE(map.IsKeyUntyped());
   REQUIRE_FALSE(map.IsValueUntyped());

   REQUIRE      (map.GetKeyType() == MetaDataOf<K>());
   REQUIRE      (map.GetKeyType()->template IsSimilar<const K>());
   REQUIRE      (map.GetKeyType()->template IsExact<K>());
   REQUIRE      (map.GetKeyType()->template Is<K*>());
   REQUIRE      (map.IsKeyDense() == CT::Dense<K>);
   REQUIRE      (map.IsKeySparse() == CT::Sparse<K>);
   REQUIRE      (map.IsKeyDeep() == CT::Deep<Decay<K>>);

   REQUIRE      (map.GetValueType() == MetaDataOf<V>());
   REQUIRE      (map.GetValueType()->template IsSimilar<const V>());
   REQUIRE      (map.GetValueType()->template IsExact<V>());
   REQUIRE      (map.GetValueType()->template Is<V*>());
   REQUIRE      (map.IsValueDense() == CT::Dense<V>);
   REQUIRE      (map.IsValueSparse() == CT::Sparse<V>);
   REQUIRE      (map.IsValueDeep() == CT::Deep<Decay<V>>);
}

template<CT::Map LHS, CT::Map RHS>
void Map_Helper_TestSame(const LHS& lhs, const RHS& rhs) {
   REQUIRE(lhs.GetRaw() == rhs.GetRaw());
   REQUIRE(lhs.IsKeyExact(rhs.GetKeyType()));
   REQUIRE(lhs.IsValueExact(rhs.GetValueType()));
   REQUIRE(lhs == rhs);
   REQUIRE(lhs.IsDeep() == rhs.IsDeep());
   REQUIRE(lhs.IsConstant() == rhs.IsConstant());
   REQUIRE(lhs.GetUnconstrainedState() == rhs.GetUnconstrainedState());
}



template<class K, class V>
void Map_CheckState_Default(const auto& map) {
   using T = Decay<decltype(map)>;

   if constexpr (CT::Typed<T>) {
      static_assert(CT::Exact<typename T::Key, K>);
      static_assert(CT::Exact<typename T::Value, V>);
      Map_Helper_TestType<K, V>(map);
      REQUIRE      (map.GetKeyState() == DataState::Typed);
      REQUIRE      (map.GetValueState() == DataState::Typed);
   }
   else {
      REQUIRE_FALSE(map.IsKeyTyped());
      REQUIRE_FALSE(map.IsValueTyped());
      REQUIRE      (map.IsKeyUntyped());
      REQUIRE      (map.IsValueUntyped());
      REQUIRE      (map.GetKeyType() == nullptr);
      REQUIRE      (map.GetValueType() == nullptr);
      REQUIRE      (map.IsKeyDense());
      REQUIRE      (map.IsValueDense());
      REQUIRE_FALSE(map.IsKeySparse());
      REQUIRE_FALSE(map.IsValueSparse());
      REQUIRE      (map.GetKeyState() == DataState::Default);
      REQUIRE      (map.GetValueState() == DataState::Default);
      REQUIRE_FALSE(map.IsKeyDeep());
      REQUIRE_FALSE(map.IsValueDeep());
   }

   REQUIRE      (map.IsKeyTypeConstrained() == CT::Typed<T>);
   REQUIRE      (map.IsValueTypeConstrained() == CT::Typed<T>);
   REQUIRE_FALSE(map.IsKeyCompressed());
   REQUIRE_FALSE(map.IsValueCompressed());
   REQUIRE      (map.IsKeyConstant() == CT::Constant<K>);
   REQUIRE      (map.IsValueConstant() == CT::Constant<V>);
   REQUIRE_FALSE(map.IsKeyEncrypted());
   REQUIRE_FALSE(map.IsValueEncrypted());
   REQUIRE_FALSE(map.IsKeyMissing());
   REQUIRE_FALSE(map.IsValueMissing());
   REQUIRE_FALSE(map.IsValid());
   REQUIRE      (map.IsInvalid());
   REQUIRE_FALSE(map.IsAllocated());
   REQUIRE_FALSE(map.GetKeys().GetAllocation());
   REQUIRE_FALSE(map.GetVals().GetAllocation());
   REQUIRE      (map.IsEmpty());
   REQUIRE      (map.GetCount() == 0);
   REQUIRE      (map.GetReserved() == 0);
   REQUIRE      (map.GetKeys().GetUses() == 0);
   REQUIRE      (map.GetVals().GetUses() == 0);
   REQUIRE      (map.GetRawKeysMemory() == nullptr);
   REQUIRE      (map.GetRawValsMemory() == nullptr);
   REQUIRE_FALSE(map);
   REQUIRE      (not map);
}

template<class K, class V>
void Map_CheckState_OwnedEmpty(const auto& map) {
   using T = Decay<decltype(map)>;

   Map_Helper_TestType<K, V>(map);

   REQUIRE      (map.IsKeyTypeConstrained() == CT::Typed<T>);
   REQUIRE      (map.IsValueTypeConstrained() == CT::Typed<T>);
   REQUIRE_FALSE(map.IsKeyCompressed());
   REQUIRE_FALSE(map.IsValueCompressed());
   REQUIRE      (map.IsKeyConstant() == CT::Constant<K>);
   REQUIRE      (map.IsValueConstant() == CT::Constant<V>);
   REQUIRE_FALSE(map.IsKeyEncrypted());
   REQUIRE_FALSE(map.IsValueEncrypted());
   REQUIRE_FALSE(map.IsKeyMissing());
   REQUIRE_FALSE(map.IsValueMissing());
   REQUIRE_FALSE(map.IsValid());
   REQUIRE      (map.IsInvalid());
   REQUIRE      (map.IsAllocated());
   REQUIRE      (map.GetKeys().GetAllocation());
   REQUIRE      (map.GetVals().GetAllocation());
   REQUIRE      (map.IsEmpty());
   REQUIRE      (map.GetCount() == 0);
   REQUIRE      (map.GetReserved() > 0);
   REQUIRE      (map.GetKeys().GetUses() == 1);
   REQUIRE      (map.GetVals().GetUses() == 1);
   REQUIRE      (map.GetRawKeysMemory());
   REQUIRE      (map.GetRawValsMemory());
   REQUIRE_FALSE(map);
   REQUIRE      (not map);
}

template<class K, class V>
void Map_CheckState_OwnedFull(const auto& map) {
   using T = Decay<decltype(map)>;

   Map_Helper_TestType<K, V>(map);

   REQUIRE      (map.IsKeyTypeConstrained() == CT::Typed<T>);
   REQUIRE      (map.IsValueTypeConstrained() == CT::Typed<T>);
   REQUIRE_FALSE(map.IsKeyCompressed());
   REQUIRE_FALSE(map.IsValueCompressed());
   REQUIRE      (map.IsKeyConstant() == CT::Constant<K>);
   REQUIRE      (map.IsValueConstant() == CT::Constant<V>);
   REQUIRE_FALSE(map.IsKeyEncrypted());
   REQUIRE_FALSE(map.IsValueEncrypted());
   REQUIRE_FALSE(map.IsKeyMissing());
   REQUIRE_FALSE(map.IsValueMissing());
   REQUIRE      (map.IsValid());
   REQUIRE_FALSE(map.IsInvalid());
   REQUIRE      (map.IsAllocated());
   REQUIRE      (map.GetKeys().GetAllocation());
   REQUIRE      (map.GetVals().GetAllocation());
   REQUIRE_FALSE(map.IsEmpty());
   REQUIRE      (map.GetCount() > 0);
   REQUIRE      (map.GetReserved() > 0);
   REQUIRE      (map.GetKeys().GetUses() > 0);
   REQUIRE      (map.GetVals().GetUses() > 0);
   REQUIRE      (map.GetRawKeysMemory());
   REQUIRE      (map.GetRawValsMemory());
   REQUIRE      (map);
   REQUIRE_FALSE(not map);
}