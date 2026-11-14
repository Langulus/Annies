///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../any/TestAnyCommon.hpp"
#include <Langulus/Many.hpp>
#include <Langulus/TMany.hpp>
#include <vector>


#if LANGULUS(BENCHMARK)
   /// Perform a persistent benchmark across build and verify performance     
   #define BenchmarkMany(func, tolerance, my_init, my) { \
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

   /// Perform two persistent benchmarks across builds - one for Any and      
   /// one for std::any. Make sure they don't deviate a lot.                  
   #define BenchmarkManyStd(func, tolerance_highscore, tolerance, my_init, my, theirs_init, theirs) { \
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
      i = 0; \
      const auto token_std = ::std::string("Test/") + func + "(" + NameOf<E>() + ") |std::vector|"; \
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
   #define BenchmarkMany(func, tolerance, my_init, my)
   #define BenchmarkManyStd(func, tolerance_highscore, tolerance, my_init, my, theirs_init, theirs)
#endif


template<class E, CT::Container C> requires CT::NoIntent<C>
void Many_Helper_TestType(const C& many) {
   Any_Helper_TestType<E>(many);
}

template<class LHS, class RHS> requires (CT::Container<LHS, RHS> and CT::NoIntent<LHS, RHS>)
void Many_Helper_TestSame(const LHS& lhs, const RHS& rhs, bool match_constness = true) {
   Any_Helper_TestSame(lhs, rhs, match_constness);
}

///                                                                           
/// Possible state test implementations                                       
template<class E, CT::Container C> requires CT::NoIntent<C>
void Many_CheckState_Default(const C& many, bool typed = false) {
   Any_CheckState_Default<E>(many, typed);

   if constexpr (requires { many.IsCompressed(); })
      REQUIRE_FALSE(many.IsCompressed());
   if constexpr (requires { many.IsEncrypted(); })
      REQUIRE_FALSE(many.IsEncrypted());
   if constexpr (requires { many.IsOr(); })
      REQUIRE_FALSE(many.IsOr());
}

template<class E, CT::Container C> requires CT::NoIntent<C>
void Many_CheckState_OwnedEmpty(const C& many) {
   Any_CheckState_OwnedEmpty<E>(many);
}

template<class E, CT::Container C> requires CT::NoIntent<C>
void Many_CheckState_OwnedFull(const C& many) {
   Any_CheckState_OwnedFull<E>(many);
}

template<class E, CT::Container C> requires CT::NoIntent<C>
void Many_CheckState_DisownedFull(const C& many) {
   Any_CheckState_DisownedFull<E>(many);
}

template<class E, CT::Container C> requires CT::NoIntent<C>
void Many_CheckState_Abandoned(const C& many) {
   Any_CheckState_Abandoned<E>(many);
}

template<CT::Container T, CT::Intent I> requires CT::NoIntent<T>
void Many_VerifyAccessorInterface(T const& many, I&&) {
   using E = typename Decay<Deint<I>>::CTTI_Typed;

   // The Get method always adds a pointer, because it interfaces the   
   // heap directly                                                     
   static_assert(requires {
      {many.template GetAt<Decay<E>      >(0)} -> ::std::same_as<const Decay<E>*>;
      {many.template GetAt<Decay<E> const>(0)} -> ::std::same_as<const Decay<E>*>;
   });
   static_assert(requires {
      {many.template GetAt<E      >(0)} -> ::std::same_as<ConstAll<E> const*>;
      {many.template GetAt<E const>(0)} -> ::std::same_as<ConstAll<E> const*>;
   });

   // AsAt dereferences that pointer, doing only pointer arithmetic     
   /*if constexpr (CT::DeepDense<E> and (not Same<TypeOf<T>, E> or CT::TypeErased<T>)) {
      static_assert(requires {
         {many.template AsAt<E>(0)} -> ::std::same_as<Decay<E>>;
      });
   }
   else {*/
      using innerT = Tif<(CT::Sparse<E> and not CT::CustomPointer<E>), ConstAll<E>, ConstAll<E> const&>;
      static_assert(requires {
         {many.template AsAt<E>(0)} -> ::std::same_as<innerT>;
      });
   //}

   if constexpr (CT::Dense<E> and CT::Typed<T>) {
      // One additional indirection is always acceptable                
      // A static container will static_assert if too many indirects    
      static_assert(requires {
         {many.template GetAt<Decay<E>      *>(0)} -> ::std::same_as<Decay<E> const*>;
         {many.template GetAt<Decay<E> const*>(0)} -> ::std::same_as<Decay<E> const*>;
      });
      static_assert(requires {
         {many.template AsAt<Decay<E>*>(0)} -> ::std::same_as<Decay<E> const*>;
      });

      /* shouldn't compile, too many indirections
      static_assert(requires {
         {many.template GetAt<Decay<E>      **>(0)} -> ::std::same_as<Decay<E> const* const*>;
         {many.template GetAt<Decay<E> const**>(0)} -> ::std::same_as<Decay<E> const* const*>;
      });
      static_assert(requires {
         {many.template AsAt<Decay<E>**>(0)} -> ::std::same_as<Decay<E> const* const* const&>;
      });
      */
   }
   else if constexpr (not CT::CustomPointer<E>) {                             //TODO access via custom pointers needs more rigorous testing
      // One additional indirection is always acceptible                
      // Type-erased containers will throw an exception at runtime, if  
      // too many indirects were requested                              
      static_assert(requires {
         {many.template GetAt<Decay<E>      *>(0)} -> ::std::same_as<Decay<E> const* const*>;
         {many.template GetAt<Decay<E> const*>(0)} -> ::std::same_as<Decay<E> const* const*>;
      });
      static_assert(requires {
         {many.template AsAt<Decay<E>*>(0)} -> ::std::same_as<Decay<E> const*>;
      });

      if constexpr (IndirectsOf<E> >= 2 or CT::TypeErased<T>) {
         static_assert(requires {
            {many.template GetAt<Decay<E>      **>(0)} -> ::std::same_as<Decay<E> const* const* const*>;
            {many.template GetAt<Decay<E> const**>(0)} -> ::std::same_as<Decay<E> const* const* const*>;
         });

         static_assert(requires {
            {many.template AsAt<Decay<E>**>(0)} -> ::std::same_as<Decay<E> const* const*>;
         });
      }
      else {
         static_assert(requires {
            {many.template GetAt<Decay<E>      **>(0)} -> ::std::same_as<Decay<E> const* const*>;
            {many.template GetAt<Decay<E> const**>(0)} -> ::std::same_as<Decay<E> const* const*>;
         });

         static_assert(requires {
            {many.template AsAt<Decay<E>**>(0)} -> ::std::same_as<Decay<E> const* const*>;
         });
      }
   }
}

template<CT::Container T, CT::Intent I> requires CT::NoIntent<T>
void Many_CheckState_ContainsOne(T const& many, I&& e_with_intent, int uses = 1) {
   Any_CheckState_ContainsOne(many, LglsFwd(e_with_intent), uses);
   Many_VerifyAccessorInterface(many, LglsFwd(e_with_intent));

   auto& e = e_with_intent.what;
   using E = typename Decay<Deint<I>>::CTTI_Typed;

   if constexpr (CT::Cloned<I> and CT::Sparse<E>) {
      //TODO test all kinds of ranged modifiers??
      for (auto& it : many) {
         if constexpr (CT::TypeErased<T>)
            REQUIRE(not it.CompareOneEqual(*e));
         else
            REQUIRE(it != *e);
      }
   }
   else {
      //TODO test all kinds of ranged modifiers??
      for (auto& it : many) {
         if constexpr (CT::TypeErased<T>)
            REQUIRE(it.CompareOneEqual(*e));
         else
            REQUIRE(it == *e);
      }
   }
}

template<CT::Container T, CT::Intent I> requires CT::NoIntent<T>
void Many_CheckState_ContainsN(size_t n, const T& many, I&& e_scoped_with_intent, int uses = 1) {
   auto& e = e_scoped_with_intent.what;
   using E = typename Decay<Deint<I>>::CTTI_Typed;

   REQUIRE(many.GetCount() == n);
   REQUIRE(many.GetUses() == 1);
   REQUIRE(many.GetReserved() >= n);

   for (auto& it : many)
      REQUIRE(it == e);

   for (size_t i = 0; i < n; ++i) {
      REQUIRE(&many.template As<Deptr<E>>(i) ==  e);
      REQUIRE( many.template As<Deptr<E>>(i) == *e);
      REQUIRE(*many.template As<E>(i) == *e);
      REQUIRE( many.template GetRaw<E>()[i] == e);

      if constexpr (CT::Dense<E>)
         REQUIRE(many.GetEntries() == nullptr);
      else if (uses) {
         REQUIRE(many.GetEntries() != nullptr);

         if constexpr (not CT::Disowned<I>) {
            for (size_t indirection = 0; indirection < IndirectsOf<E>; ++indirection) {
               size_t entry_idx = indirection + i * IndirectsOf<E>;
               if constexpr (CT::Cloned<I>)
                  REQUIRE(many.GetEntries()[entry_idx] != e.entries[indirection + 1]);
               else
                  REQUIRE(many.GetEntries()[entry_idx] == e.entries[indirection + 1]);
            }
         }
         else {
            for (size_t indirection = 0; indirection < IndirectsOf<E>; ++indirection) {
               size_t entry_idx = indirection + i * IndirectsOf<E>;
               REQUIRE(many.GetEntries()[entry_idx] == nullptr);
            }
         }
      }

      if constexpr (CT::TypeErased<T>) {
         REQUIRE_THROWS(many.template As<float>(i) == 0.0f);
         REQUIRE_THROWS(many.template As<float*>(i) == nullptr);
      }
   }
}

template<CT::Container T, CT::Intent I> requires (CT::NoIntent<T> and CT::Array<I>)
void Many_CheckState_ContainsArray(const T& many, I&& e_scoped_array_with_intent) {
   auto  e = e_scoped_array_with_intent.what;
   using E = typename Decay<Deint<I>>::CTTI_Typed;
   constexpr size_t n = ExtentOf<decltype(e_scoped_array_with_intent.what)>;

   REQUIRE(many.GetCount() == n);
   REQUIRE(many.GetUses() == 1);
   REQUIRE(many.GetReserved() >= n);

   int index = 0;
   for (auto& it : many)
      REQUIRE(it == *(e[index++]));
   REQUIRE(index == n);

   for (int i = 0; i < n; ++i) {
      REQUIRE(&many.template As<Deptr<E>>(i) == e[i]);
      REQUIRE( many.template As<Deptr<E>>(i) == *e[i]);
      REQUIRE(*many.template As<E>(i) == *e[i]);
      REQUIRE( many.template GetRaw<E>()[i] == e[i]);

      if constexpr (not CT::Disowned<I>) {
         for (size_t indirection = 0; indirection < IndirectsOf<E>; ++indirection) {
            size_t entry_idx = indirection + i * IndirectsOf<E>;
            if constexpr (CT::Cloned<I>)
               REQUIRE(many.GetEntries()[entry_idx] != e[i].entries[indirection + 1]);
            else
               REQUIRE(many.GetEntries()[entry_idx] == e[i].entries[indirection + 1]);
         }
      }
      else {
         for (size_t indirection = 0; indirection < IndirectsOf<E>; ++indirection) {
            size_t entry_idx = indirection + i * IndirectsOf<E>;
            REQUIRE(many.GetEntries()[entry_idx] == nullptr);
         }
      }

      if constexpr (T::TypeErased) {
         REQUIRE_THROWS(many.template As<float>(i) == 0.0f);
         REQUIRE_THROWS(many.template As<float*>(i) == nullptr);
      }
   }
}

template<CT::Container T, class E> requires CT::NoIntent<T>
void Many_Helper_CompareOne(const T& many, const E& e) {
   Any_Helper_CompareOne(many, e);
}
