///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../many/TestManyCommon.hpp"
#include <Langulus/Text.hpp>
#include <string>


#if LANGULUS(BENCHMARK)
   /// Perform a persistent benchmark across build and verify performance     
   #define BenchmarkText(func, tolerance, my_init, my) { \
      const auto token = ::std::string("Test/") + func + "(char) |Text|"; \
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

   /// Perform two persistent benchmarks across builds - one for Any and      
   /// one for std::any. Make sure they don't deviate a lot.                  
   #define BenchmarkTextStd(func, tolerance_highscore, tolerance, my_init, my, theirs_init, theirs) { \
      const auto token = ::std::string("Test/") + func + "(char) |Text|"; \
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
      const auto token_std = ::std::string("Test/") + func + "(char) |std::string|"; \
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
   #define BenchmarkText(func, tolerance, my_init, my)
   #define BenchmarkTextStd(func, tolerance_highscore, tolerance, my_init, my, theirs_init, theirs)
#endif


template<CT::Container C> requires CT::NoIntent<C>
void Text_Helper_TestType(const C& text) {
   Many_Helper_TestType<char>(text);
}

template<class LHS, class RHS> requires (CT::Container<LHS, RHS> and CT::NoIntent<LHS, RHS>)
void Text_Helper_TestSame(const LHS& lhs, const RHS& rhs, bool match_constness = true) {
   Many_Helper_TestSame(lhs, rhs, match_constness);
}

template<CT::Container C> requires CT::NoIntent<C>
void Text_CheckState_Default(const C& text, bool typed = false) {
   Many_CheckState_Default<char>(text, typed);

   REQUIRE      (text == nullptr);
   REQUIRE_FALSE(text != nullptr);
   REQUIRE      (text == (char*)nullptr);
   REQUIRE_FALSE(text != (char*)nullptr);
   REQUIRE      (text == "");
   REQUIRE_FALSE(text != "");
   REQUIRE_FALSE(text == "no match");
}

template<CT::Container C> requires CT::NoIntent<C>
void Text_CheckState_OwnedEmpty(const C& text) {
   Many_CheckState_OwnedEmpty<char>(text);

   REQUIRE      (text == nullptr);
   REQUIRE_FALSE(text != nullptr);
   REQUIRE      (text == (char*)nullptr);
   REQUIRE_FALSE(text != (char*)nullptr);
   REQUIRE      (text == "");
   REQUIRE_FALSE(text != "");
   REQUIRE_FALSE(text == "no match");
}

template<CT::Container C> requires CT::NoIntent<C>
void Text_CheckState_OwnedFull(const C& text) {
   Many_CheckState_OwnedFull<char>(text);

   REQUIRE      (text != nullptr);
   REQUIRE_FALSE(text == nullptr);
   REQUIRE      (text != (char*)nullptr);
   REQUIRE_FALSE(text == (char*)nullptr);
   REQUIRE      (text != "");
   REQUIRE_FALSE(text == "");
   REQUIRE_FALSE(text == "no match");
}

template<CT::Container C> requires CT::NoIntent<C>
void Text_CheckState_DisownedFull(const C& text) {
   Many_CheckState_DisownedFull<char>(text);
}

template<CT::Container C> requires CT::NoIntent<C>
void Text_CheckState_Abandoned(const C& text) {
   Many_CheckState_Abandoned<char>(text);
}

template<CT::Container T, CT::Intent I> requires CT::NoIntent<T>
void Text_VerifyAccessorInterface(T const& text, I&& arg) {
   Many_VerifyAccessorInterface(text, LglsFwd(arg));
}

/*template<CT::Container T, CT::Intent I> requires CT::NoIntent<T>
void Text_CheckState_ContainsOne(T const& text, I&& e_with_intent, int uses = 1) {
   Many_CheckState_ContainsOne(text, LglsFwd(e_with_intent), uses);
}

template<CT::Container T, CT::Intent I> requires CT::NoIntent<T>
void Text_CheckState_ContainsN(size_t n, const T& many, I&& e_scoped_with_intent, int uses = 1) {
   Many_CheckState_ContainsN(n, many, LglsFwd(e_scoped_with_intent), uses);
}*/

template<CT::Container T, CT::Array I> requires CT::NoIntent<I>
void Text_CheckState_ContainsString(const T& many, I&& e) {
   constexpr size_t n = ExtentOf<I> - 1;

   REQUIRE(many.GetCount() == n);
   REQUIRE(many.GetUses() == 1);
   REQUIRE(many.GetReserved() >= n);

   int index = 0;
   for (auto& it : many)
      REQUIRE(it == e[index++]);
   REQUIRE(index == n);

   REQUIRE(*many.Get() == e[0]);
   REQUIRE(many.template As<char>() == e[0]);

   for (size_t i = 0; i < n; ++i) {
      REQUIRE(many.GetRaw()[i] == e[i]);
      REQUIRE(many.template AsAt<char>(i) == e[i]);
      REQUIRE(many.template GetRawAs<char>()[i] == e[i]);

      if constexpr (T::TypeErased) {
         REQUIRE_THROWS(many.template As<float>(i) == 0.0f);
         REQUIRE_THROWS(many.template As<float*>(i) == nullptr);
      }
   }
}

template<CT::Container T, class I> requires CT::NoIntent<T, I>
void Text_CheckState_ContainsOne(T const& pack, I const& e, int uses = 1) {
   if constexpr (CT::Character<I>) {
      REQUIRE(pack.GetCount() == 1);
      REQUIRE(pack.GetUses() == uses);
      REQUIRE(pack.GetReserved() >= pack.GetCount());
      REQUIRE(*pack.Get() == e);
   }
   else {
      T converted;
      Langulus::Serialize(e, converted);
      REQUIRE(pack.GetCount() > 0);
      REQUIRE(pack.GetCount() == converted.GetCount());
      REQUIRE(pack.GetUses() == uses);
      REQUIRE(pack.GetReserved() >= pack.GetCount());

      for (size_t i = 0; i < converted.GetCount(); ++i) {
         REQUIRE(pack.GetRaw()[i] == converted[i]);
         REQUIRE(pack.template GetRawAs<char>()[i] == converted[i]);
         REQUIRE(pack.template As<char>() == converted[0]);
         REQUIRE(pack.template AsAt<char>(i) == converted[i]);
         REQUIRE(*pack.Get() == *converted.GetRaw());
         REQUIRE(*pack.template Get<char>() == *converted.GetRaw());
         REQUIRE(pack.template GetRawAs<char>()[i] == converted.GetRaw()[i]);
      }
   }
}

template<CT::Container T, class E> requires CT::NoIntent<T>
void Text_Helper_CompareOne(const T& many, const E& e) {
   Many_Helper_CompareOne(many, e);
}
