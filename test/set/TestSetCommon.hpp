///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../many/TestManyCommon.hpp"
#include <Langulus/Set.hpp>
#include <Langulus/TSet.hpp>
#include <unordered_set>
#include <set>


#if LANGULUS(BENCHMARK)
   /// Perform a persistent benchmark across builds and verify performance    
   #define BenchmarkSet(func, tolerance, my_init, my) { \
      const auto token = ::std::string("Test/") + func + "(" + NameOf<E>() ") |" + NameOf<T>() + "|"; \
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

   /// Perform two persistent benchmarks across builds - one for Set and      
   /// one for std::unordered_set. Make sure they don't deviate in a bad way. 
   #define BenchmarkSetStd(func, tolerance_highscore, tolerance, my_init, my, theirs_init, theirs) { \
      const auto token = ::std::string("Test/") + func + "(" + NameOf<E>() ") |" + NameOf<T>() + "|"; \
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
      i = 0; \
      const auto token_std = ::std::string("Test/") + func + "(" + NameOf<E>() ") |std::unordered_set|"; \
      for (; i < BenchmarkWarmupCycles; i += 1) { \
         theirs_init; \
         theirs; \
      } \
      for (; i < BenchmarkWarmupCycles + BenchmarkMeasureCycles; i += 1) { \
         theirs_init; \
         { \
            CTRACK_NAME(token_std.c_str()); \
            theirs; \
         } \
      } \
      auto results = ctrack::result_get_detail_table(); \
      results.check_highscore(tolerance_highscore); \
      REQUIRE(results.check_same(token.c_str(), token_std.c_str(), tolerance)); \
   }
#else
   #define BenchmarkSet(func, tolerance, my_init, my)
   #define BenchmarkSetStd(func, tolerance_highscore, tolerance, my_init, my, theirs_init, theirs)
#endif


template<class E, CT::Container C> requires CT::NoIntent<C>
void Set_Helper_TestType(const C& set) {
   Many_Helper_TestType<E>(set);
}

template<class LHS, class RHS> requires (CT::Container<LHS, RHS> and CT::NoIntent<LHS, RHS>)
void Set_Helper_TestSame(const LHS& lhs, const RHS& rhs, bool match_constness = true) {
   Many_Helper_TestSame(lhs, rhs, match_constness);
}

///                                                                           
/// Possible state test implementations                                       
template<class E, CT::Container C> requires CT::NoIntent<C>
void Set_CheckState_Default(const C& set, bool typed = false) {
   Common_CheckState_Default<E>(set, typed);

   REQUIRE_FALSE(set.IsCompressed());
   REQUIRE_FALSE(set.IsEncrypted());
   REQUIRE_FALSE(set.IsSorted());
}

template<class E, CT::Container C> requires CT::NoIntent<C>
void Set_CheckState_OwnedEmpty(const C& set) {
   Many_CheckState_OwnedEmpty<E>(set);
}

template<class E, CT::Container C> requires CT::NoIntent<C>
void Set_CheckState_OwnedFull(const C& set) {
   Many_CheckState_OwnedFull<E>(set);
}

template<class E, CT::Container C> requires CT::NoIntent<C>
void Set_CheckState_DisownedFull(const C& set) {
   Many_CheckState_DisownedFull<E>(set);
}

template<class E, CT::Container C> requires CT::NoIntent<C>
void Set_CheckState_Abandoned(const C& set) {
   Many_CheckState_Abandoned<E>(set);
}

template<CT::Container T, CT::Intent I> requires CT::NoIntent<T>
void Set_CheckState_ContainsOne(T const& set, I&& e_with_intent, int uses = 1) {
   Many_VerifyAccessorInterface(set, LglsFwd(e_with_intent));

   auto& e = e_with_intent.what;
   using E = typename Decay<Deint<I>>::CTTI_Typed;

   if constexpr (CT::DeepDense<E>)
      REQUIRE(set.template AsAt<E*>(0)->template IsSame<int>());

   REQUIRE(set.GetCount() == 1);
   REQUIRE(set.GetUses() == uses);
   REQUIRE(set.GetReserved() >= (uses ? 1 : 0));

   if constexpr (not CT::CustomPointer<E>)
      REQUIRE(set.template AsAt<Decay<E>>(0) == DenseCast(*e));

   if constexpr (CT::Cloned<I> and CT::Sparse<E>) {
      REQUIRE(set.template AsAt<E>(0) != *e);
      REQUIRE((*set.template AsAt<E*>(0)) != *e);
   }
   else {
      REQUIRE(set.template AsAt<E>(0) == *e);
      REQUIRE((*set.template AsAt<E*>(0)) == *e);
   }

   if constexpr (CT::OwnedDeep<T>) {
      if constexpr (CT::Dense<E>)
         REQUIRE(set.GetEntries() == nullptr);
      else if (uses) {
         REQUIRE(set.GetEntries() != nullptr);

         if constexpr (not CT::Disowned<I>) {
            for (size_t i = 0; i < IndirectsOf<E>; ++i) {
               if constexpr (CT::Cloned<I>)
                  REQUIRE(set.GetEntriesAt(0)[i] != e.entries[i + 1]);
               else
                  REQUIRE(set.GetEntriesAt(0)[i] == e.entries[i + 1]);
            }
         }
         else {
            for (size_t i = 0; i < IndirectsOf<E>; ++i)
               REQUIRE(set.GetEntriesAt(0)[i] == nullptr);
         }
      }
   }

   if constexpr (CT::TypeErased<T>) {
      REQUIRE_THROWS(set.template AsAt<float>(0) == 0.0f);
      REQUIRE_THROWS(set.template AsAt<float*>(0) == nullptr);
   }

   if constexpr (CT::Cloned<I> and CT::Sparse<E>) {
      //TODO test all kinds of ranged modifiers??
      for (auto it : set) {
         if constexpr (CT::TypeErased<T>)
            REQUIRE(not it.CompareOneEqual(*e));
         else
            REQUIRE(it != *e);
      }
   }
   else {
      //TODO test all kinds of ranged modifiers??
      for (auto it : set) {
         if constexpr (CT::TypeErased<T>)
            REQUIRE(it.CompareOneEqual(*e));
         else
            REQUIRE(it == *e);
      }
   }
}

template<CT::Container T> requires CT::NoIntent<T>
void Set_CheckState_ContainsN(const T& set, size_t n) {
   REQUIRE(set.GetCount() == n);

   // Check reserve                                                     
   size_t total  = T::InitialSize;
   size_t growth = T::InitialSize;
   while (n > total) {
      growth *= T::GrowthFactor;
      total += growth;
   }
   REQUIRE(set.GetReserved() == total);

   // Check table integrity                                             
   auto start     = set.GetHashTable();
   const auto end = set.GetHashTableEnd();
   REQUIRE((end - start) == set.GetReserved());

   size_t found = 0;
   while (start != end) {
      if (*start)
         ++found;
      ++start;
   }
   REQUIRE(found == n);
}

/// MARK: ContainsArray                                                       
template<CT::Container T, CT::Array...A> requires CT::NoIntent<T, A...>
void Set_CheckState_ContainsArray(const T& set, A const&...array) {
   // Check count                                                       
   constexpr size_t size = (ExtentOf<A> + ...);
   REQUIRE(set.GetCount() == size);

   // Check reserve                                                     
   size_t total = T::InitialSize;
   size_t growth = T::InitialSize;
   while (size > total) {
      growth *= T::GrowthFactor;
      total += growth;
   }
   REQUIRE(set.GetReserved() == total);

   // Check if all elements are present                                 
   auto find = []<class H>(H const& e, CT::Array auto const& a) -> size_t {
      size_t found = 0;
      for (auto& a_e : a) {
         if constexpr (CT::Handle<H>) {
            if (e.CompareOneEqual(a_e))
               ++found;
         }
         else if constexpr (CT::Container<H>) {
            if (e.CompareEqual(a_e))
               ++found;
         }
         else {
            if (e == a_e)
               ++found;
         }
      }
      return found;
   };

   size_t iterated = 0;
   size_t found = 0;
   for (auto it : set) {
      found += (find(it, array) + ...);
      ++iterated;
   }
   REQUIRE(iterated == size);
   REQUIRE(found == size);

   // Check table integrity                                             
   auto start     = set.GetHashTable();
   const auto end = set.GetHashTableEnd();
   REQUIRE((end - start) == set.GetReserved());

   found = 0;
   while (start != end) {
      if (*start)
         ++found;
      ++start;
   }
   REQUIRE(found == size);
}

template<CT::Container T, class E> requires CT::NoIntent<T>
void Set_Helper_CompareOne(const T& set, const E& e) {
   Many_Helper_CompareOne(set, e);
}

/// MARK: DumpSet                                                             
template<CT::Container T> requires CT::NoIntent<T>
void DumpSet(const T& set) {
   auto table = set.GetHashTable();
   const auto tableEnd = set.GetHashTableEnd();
   auto handle = set.GetHandle();
   auto _ = Logger::SpecialSection("Set dump:");
   Logger::Line("-------------- table #0 --------------");

   size_t growth = T::InitialSize;
   size_t cascade = T::InitialSize;
   size_t table_idx = 0;
   size_t counter = 0;
   while(table != tableEnd) {
      if (counter == cascade) {
         growth *= T::GrowthFactor;
         cascade += growth;
         ++table_idx;
         Logger::Line("--------------------------------------");
         Logger::Line("-------------- table #", table_idx, " --------------");
      }

      if (*table) {
         if (*table == 1)
            Logger::Line("");
         else
            Logger::Line("^-");

         for (int i = 2; i < *table; ++i)
            Logger::Append("--");

         Logger::Append("[", counter, "] ", handle);
      }
      else Logger::Line("[", counter, "] -");

      ++table;
      ++handle;
      ++counter;
   }
}