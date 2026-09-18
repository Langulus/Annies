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
#include <Langulus/Annies/Many.hpp>
#include "../Common.hpp"


template<class T, class E>
decltype(auto) FromHelper() {
   if constexpr (not CT::Typed<T>) {
      if constexpr (CT::TraitBased<T>) {
         if constexpr (CT::Trait<T>)
            return T::template OfType<E>();
         else
            return T::template From<Traits::Count, E>();
      }
      else return T::template From<E>();
   }
   else return T {};
}


///                                                                           
/// Possible states:                                                          
///   - uninitialized                                                         
///   - default                                                               
template<class E>
void Any_CheckState_Default(const auto&);
///   - invariant                                                             
template<class E>
void Any_CheckState_Invariant(const auto&);
///   - owned-full                                                            
template<class E>
void Any_CheckState_OwnedFull(const auto&);
///   - owned-full-const                                                      
template<class E>
void Any_CheckState_OwnedFullConst(const auto&);
///   - owned-empty                                                           
template<class E>
void Any_CheckState_OwnedEmpty(const auto&);
///   - disowned-full                                                         
template<class E>
void Any_CheckState_DisownedFull(const auto&);
///   - disowned-full-const                                                   
template<class E>
void Any_CheckState_DisownedFullConst(const auto&);
///   - abandoned                                                             
template<class E>
void Any_CheckState_Abandoned(const auto&);

template<class E>
void Any_Helper_TestType(const auto& any) {
   REQUIRE      (any.IsTyped());
   REQUIRE_FALSE(any.IsUntyped());
   REQUIRE      (any.GetType() == MetaDataOf<E>());
   REQUIRE      (any.GetType()->template IsSimilar<const E>());
   REQUIRE      (any.GetType()->template IsExact<E>());
   REQUIRE      (any.GetType()->template Is<E*>());
   REQUIRE      (any.IsDense() == CT::Dense<E>);
   REQUIRE      (any.IsSparse() == CT::Sparse<E>);
   REQUIRE      (any.IsDeep() == CT::Deep<Decay<E>>);
}

template<CT::BlockBased LHS, CT::BlockBased RHS>
void Any_Helper_TestSame(const LHS& lhs, const RHS& rhs) {
   REQUIRE(lhs.GetRaw() == rhs.GetRaw());
   REQUIRE(lhs.IsExact(rhs.GetType()));
   REQUIRE(lhs == rhs);
   REQUIRE(lhs.IsDeep() == rhs.IsDeep());
   REQUIRE(lhs.IsConstant() == rhs.IsConstant());
   REQUIRE(lhs.GetUnconstrainedState() == rhs.GetUnconstrainedState());
}


///                                                                           
/// Possible actions for each state:                                          
///   - uninitialized                                                         
///      - constexpr-default-initialized                                      
///      - runtime-default-initialized                                        
///      - intent-initialized from container                                  
///      - intent-initialized from single dense element                       
///      - intent-initialized from multiple dense elements                    
///      - intent-initialized from dense element bounded array                

template<class E>
void Any_CheckState_Default(const auto& any) {
   using T = Decay<decltype(any)>;

   if constexpr (CT::Typed<T>) {
      static_assert(CT::Exact<TypeOf<T>, E>);
      Any_Helper_TestType<E>(any);
      REQUIRE      (any.GetState() == DataState::Typed);
   }
   else {
      REQUIRE_FALSE(any.IsTyped());
      REQUIRE      (any.IsUntyped());
      REQUIRE      (any.GetType() == nullptr);
      REQUIRE      (any.IsDense());
      REQUIRE_FALSE(any.IsSparse());
      REQUIRE      (any.GetState() == DataState::Default);
      REQUIRE_FALSE(any.IsDeep());
   }

   REQUIRE      (any.IsTypeConstrained() == CT::Typed<T>);
   REQUIRE_FALSE(any.IsCompressed());
   REQUIRE      (any.IsConstant() == CT::Constant<E>);
   REQUIRE_FALSE(any.IsEncrypted());
   REQUIRE_FALSE(any.IsMissing());
   REQUIRE_FALSE(any.IsOr());
   REQUIRE_FALSE(any.IsStatic());
   REQUIRE_FALSE(any.IsValid());
   REQUIRE      (any.IsInvalid());
   REQUIRE_FALSE(any.IsAllocated());
   REQUIRE_FALSE(any.GetAllocation());
   REQUIRE      (any.IsNow());
   REQUIRE_FALSE(any.IsFuture());
   REQUIRE_FALSE(any.IsPast());
   REQUIRE      (any.IsEmpty());
   REQUIRE      (any.GetCount() == 0);
   REQUIRE      (any.GetReserved() == 0);
   REQUIRE      (any.GetUses() == 0);
   REQUIRE      (any.GetRaw() == nullptr);
   REQUIRE_FALSE(any);
   REQUIRE      (not any);
}

template<class E>
void Any_CheckState_OwnedEmpty(const auto& any) {
   using T = Decay<decltype(any)>;

   Any_Helper_TestType<E>(any);

   REQUIRE      (any.IsTypeConstrained() == CT::Typed<T>);
   REQUIRE_FALSE(any.IsCompressed());
   REQUIRE      (any.IsConstant() == CT::Constant<E>);
   REQUIRE_FALSE(any.IsEncrypted());
   REQUIRE_FALSE(any.IsStatic());
   REQUIRE_FALSE(any.IsValid());
   REQUIRE      (any.IsInvalid());
   REQUIRE      (any.IsAllocated());
   REQUIRE      (any.GetAllocation());
   REQUIRE      (any.IsEmpty());
   REQUIRE      (any.GetCount() == 0);
   REQUIRE      (any.GetReserved() > 0);
   REQUIRE      (any.GetUses() == 1);
   REQUIRE      (any.GetRaw());
   REQUIRE_FALSE(any);
   REQUIRE      (not any);
}

template<class E>
void Any_CheckState_OwnedFull(const auto& any) {
   using T = Decay<decltype(any)>;

   Any_Helper_TestType<E>(any);

   REQUIRE      (any.IsTypeConstrained() == CT::Typed<T>);
   REQUIRE_FALSE(any.IsCompressed());
   REQUIRE      (any.IsConstant() == CT::Constant<E>);
   REQUIRE_FALSE(any.IsEncrypted());
   REQUIRE      (any.IsValid());
   REQUIRE_FALSE(any.IsInvalid());
   REQUIRE_FALSE(any.IsStatic());
   REQUIRE      (any.IsAllocated());
   REQUIRE      (any.GetAllocation());
   REQUIRE_FALSE(any.IsEmpty());
   REQUIRE      (any.GetCount() > 0);
   REQUIRE      (any.GetReserved() > 0);
   REQUIRE      (any.GetUses() > 0);
   REQUIRE      (any.GetRaw());
   REQUIRE      (any);
   REQUIRE_FALSE(not any);
}

template<class E>
void Any_CheckState_DisownedFull(const auto& any) {
   using T = Decay<decltype(any)>;

   Any_Helper_TestType<E>(any);

   REQUIRE      (any.IsTypeConstrained() == CT::Typed<T>);
   REQUIRE_FALSE(any.IsCompressed());
   REQUIRE      (any.IsConstant() == CT::Constant<E>);
   REQUIRE_FALSE(any.IsEncrypted());
   REQUIRE      (any.IsValid());
   REQUIRE_FALSE(any.IsInvalid());
   REQUIRE      (any.IsStatic());
   REQUIRE      (any.IsAllocated());
   REQUIRE_FALSE(any.GetAllocation());
   REQUIRE_FALSE(any.IsEmpty());
   REQUIRE      (any.GetCount() > 0);
   REQUIRE      (any.GetReserved() > 0);
   REQUIRE      (any.GetUses() == 0);
   REQUIRE      (any.GetRaw());
   REQUIRE      (any);
   REQUIRE_FALSE(not any);
}

template<class E>
void Any_CheckState_DisownedFullConst(const auto& any) {
   using T = Decay<decltype(any)>;

   Any_Helper_TestType<E>(any);

   REQUIRE      (any.IsTypeConstrained() == CT::Typed<T>);
   REQUIRE_FALSE(any.IsCompressed());
   REQUIRE      (any.IsConstant());
   REQUIRE_FALSE(any.IsEncrypted());
   REQUIRE      (any.IsValid());
   REQUIRE_FALSE(any.IsInvalid());
   REQUIRE      (any.IsStatic());
   REQUIRE      (any.IsAllocated());
   REQUIRE_FALSE(any.GetAllocation());
   REQUIRE_FALSE(any.IsEmpty());
   REQUIRE      (any.GetCount() > 0);
   REQUIRE      (any.GetReserved() > 0);
   REQUIRE      (any.GetUses() == 0);
   REQUIRE      (any.GetRaw());
   REQUIRE      (any);
   REQUIRE_FALSE(not any);
}

template<class E>
void Any_CheckState_Abandoned(const auto& any) {
   REQUIRE_FALSE(any.GetAllocation());
}


void Any_CheckState_ContainsOne(const auto& pack, const auto& e, Allocation* entry = nullptr) {
   using T = Deref<decltype(pack)>;
   using E = Deref<decltype(e)>;
   (void) entry;

   REQUIRE(pack.GetCount() == 1);
   REQUIRE(pack.GetUses() == 1);
   REQUIRE(pack.GetReserved() >= 1);

   for (auto& it : pack)
      REQUIRE(it == e);

   if constexpr (CT::Sparse<E>) {
      REQUIRE(&pack.template As<Deptr<E>>() ==  e);
      REQUIRE( pack.template As<Deptr<E>>() == *e);
      REQUIRE(*pack.template As<E>() == *e);
      REQUIRE(*pack.template GetRaw<E>() == e);
   }
   else if constexpr (T::TypeErased or CT::Same<TypeOf<T>, E>) {
      REQUIRE(pack.template As<E>() == e);
   }

   IF_LANGULUS_MANAGED_MEMORY(REQUIRE(*pack.GetEntries() == entry));

   if constexpr (T::TypeErased) {
      REQUIRE_THROWS(pack.template As<float>() == 0.0f);
      REQUIRE_THROWS(pack.template As<float*>() == nullptr);
   }
}

void Any_CheckState_ContainsN(size_t n, const auto& pack, const CT::Sparse auto& e, Allocation* entry = nullptr) {
   using T = Deref<decltype(pack)>;
   using E = Deref<decltype(e)>;
   (void)entry;

   REQUIRE(pack.GetCount() == n);
   REQUIRE(pack.GetUses() == 1);
   REQUIRE(pack.GetReserved() >= n);

   for (auto& it : pack)
      REQUIRE(it == e);

   for (size_t i = 0; i < n; ++i) {
      REQUIRE(&pack.template As<Deptr<E>>(i) == e);
      REQUIRE(pack.template As<Deptr<E>>(i) == *e);
      REQUIRE(*pack.template As<E>(i) == *e);
      REQUIRE(pack.template GetRaw<E>()[i] == e);
      IF_LANGULUS_MANAGED_MEMORY(REQUIRE(pack.GetEntries()[i] == entry));

      if constexpr (T::TypeErased) {
         REQUIRE_THROWS(pack.template As<float>(i) == 0.0f);
         REQUIRE_THROWS(pack.template As<float*>(i) == nullptr);
      }
   }
}

void Any_CheckState_ContainsArray(const auto& pack, const CT::Array auto& e, Allocation* entry = nullptr) {
   using T = Deref<decltype(pack)>;
   using E = Deext<decltype(e)>;
   (void)entry;
   constexpr int n = ExtentOf<decltype(e)>;

   REQUIRE(pack.GetCount() == n);
   REQUIRE(pack.GetUses() == 1);
   REQUIRE(pack.GetReserved() >= n);

   int index = 0;
   for (auto& it : pack)
      REQUIRE(it == e[index++]);
   REQUIRE(index == n);

   for (size_t i = 0; i < n; ++i) {
      REQUIRE(&pack.template As<Deptr<E>>(i) == e[i]);
      REQUIRE(pack.template As<Deptr<E>>(i) == *e[i]);
      REQUIRE(*pack.template As<E>(i) == *e[i]);
      REQUIRE(pack.template GetRaw<E>()[i] == e[i]);
      IF_LANGULUS_MANAGED_MEMORY(REQUIRE(pack.GetEntries()[i] == entry));

      if constexpr (T::TypeErased) {
         REQUIRE_THROWS(pack.template As<float>(i) == 0.0f);
         REQUIRE_THROWS(pack.template As<float*>(i) == nullptr);
      }
   }
}