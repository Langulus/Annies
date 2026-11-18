///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../many/TestManyCommon.hpp"
#include <Langulus/THive.hpp>


#if LANGULUS(BENCHMARK)
   /// Perform a persistent benchmark across build and verify performance     
   #define BenchmarkHive(func, tolerance, my_init, my) { \
      const auto token = ::std::string("Test/") + func + "(" + NameOf<E>() + ") |" + NameOf<T>() + "|"; \
      volatile int i = 0; \
      for (; i < BenchmarkWarmupCycles; i += 1) { \
         my_init; \
         my; \
      } \
      for (; i < BenchmarkWarmupCycles + BenchmarkMeasureCycles; i += 1) { \
         my_init; \
         { \
            CTRACK_NAME_PERSIST(token.c_str()); \
            my; \
         } \
      } \
      auto results = ctrack::result_get_detail_table(); \
      results.check_highscore(tolerance); \
   }
#else
   #define BenchmarkHive(func, tolerance, my_init, my)
#endif


template<class E, CT::Container C> requires CT::NoIntent<C>
void Hive_Helper_TestType(const C& many) {
   Many_Helper_TestType<E>(many);
}

template<class LHS, class RHS> requires (CT::Container<LHS, RHS> and CT::NoIntent<LHS, RHS>)
void Hive_Helper_TestSame(const LHS& lhs, const RHS& rhs, bool match_constness = true) {
   Many_Helper_TestSame(lhs, rhs, match_constness);
}

///                                                                           
/// Possible state test implementations                                       
template<class E, CT::Container C> requires CT::NoIntent<C>
void Hive_CheckState_Default(const C& many, bool typed = false) {
   Many_CheckState_Default<E>(many, typed);
}

template<class E, CT::Container C> requires CT::NoIntent<C>
void Hive_CheckState_OwnedEmpty(const C& many) {
   Many_CheckState_OwnedEmpty<E>(many);
}

template<class E, CT::Container C> requires CT::NoIntent<C>
void Hive_CheckState_OwnedFull(const C& many) {
   Many_CheckState_OwnedFull<E>(many);
}

template<class E, CT::Container C> requires CT::NoIntent<C>
void Hive_CheckState_DisownedFull(const C& many) {
   Many_CheckState_DisownedFull<E>(many);
}

template<class E, CT::Container C> requires CT::NoIntent<C>
void Hive_CheckState_Abandoned(const C& many) {
   Many_CheckState_Abandoned<E>(many);
}

template<CT::Container T, CT::Intent I> requires CT::NoIntent<T>
void Hive_VerifyAccessorInterface(T const& many, I&& intent) {
   Many_VerifyAccessorInterface(many, LglsFwd(intent));
}

template<CT::Container T, CT::Intent I> requires CT::NoIntent<T>
void Hive_CheckState_ContainsOne(T const& many, I&& e_with_intent, int uses = 1) {
   Hive_VerifyAccessorInterface(many, LglsFwd(e_with_intent));

   auto& e1 = e_with_intent.what;
   using E1 = typename Decay<Deint<I>>::CTTI_Typed;

   if constexpr (CT::DeepDense<E1>) {
      REQUIRE(many.template AsAt<E1 >(0).template IsSame<int>());
      REQUIRE(many.template AsAt<E1*>(0)->template IsSame<int>());
   }

   REQUIRE(many.GetCount() == 1);
   REQUIRE(many.GetUses() == uses);
   REQUIRE(many.GetReserved() >= (uses ? 1 : 0));

   if constexpr (not CT::CustomPointer<E1>)
      REQUIRE(many.template AsAt<Decay<E1>>(0) == DenseCast(*e1));

   if constexpr (CT::Cloned<I> and CT::Sparse<E1>) {
      REQUIRE(many.template AsAt<E1>(0) != *e1);
      REQUIRE((*many.template AsAt<E1*>(0)) != *e1);
   }
   else {
      REQUIRE(many.template AsAt<E1>(0) == *e1);
      REQUIRE((*many.template AsAt<E1*>(0)) == *e1);
   }

   if constexpr (CT::OwnedDeep<T>) {
      if constexpr (CT::Dense<E1>)
         REQUIRE(many.GetEntries() == nullptr);
      else if (uses) {
         REQUIRE(many.GetEntries() != nullptr);

         if constexpr (not CT::Disowned<I>) {
            for (size_t i = 0; i < IndirectsOf<E1>; ++i) {
               if constexpr (CT::Cloned<I>) {
                  REQUIRE(many.GetEntriesAt(0)[i] != nullptr);
                  REQUIRE(many.GetEntriesAt(0)[i] != e1.entries[i + 1]);
               }
               else {
                  #if LANGULUS_FEATURE(MANAGED_MEMORY)
                     REQUIRE(many.GetEntriesAt(0)[i] == e1.entries[i + 1]);
                  #else
                     REQUIRE(many.GetEntriesAt(0)[i] == nullptr);
                  #endif
               }
            }
         }
         else {
            for (size_t i = 0; i < IndirectsOf<E1>; ++i)
               REQUIRE(many.GetEntriesAt(0)[i] == nullptr);
         }
      }
   }

   //TODO test all kinds of ranged modifiers??
   for (auto& it : many) {
      static_assert(Exact<ConstAll<TypeOf<T>&>, decltype(it)>);
      REQUIRE((it != *e1) == (CT::Cloned<I> and CT::Sparse<E1>));
   }
}

template<CT::Container T, CT::Intent I> requires CT::NoIntent<T>
void Hive_CheckState_ContainsN(size_t n, const T& many, I&& e_scoped_with_intent, int uses = 1) {
   Many_CheckState_ContainsN(n, many, LglsFwd(e_scoped_with_intent), uses);
}

template<CT::Container T, CT::Intent I> requires (CT::NoIntent<T> and CT::Array<I>)
void Hive_CheckState_ContainsArray(const T& many, I&& e_scoped_array_with_intent) {
   Many_CheckState_ContainsArray(many, LglsFwd(e_scoped_array_with_intent));
}

template<CT::Container T, class E> requires CT::NoIntent<T>
void Hive_Helper_CompareOne(const T& many, const E& e) {
   Many_Helper_CompareOne(many, e);
}
