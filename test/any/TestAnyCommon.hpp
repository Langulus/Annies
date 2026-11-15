///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include <Langulus/Testing/CommonTypes.hpp>
#include <Langulus/Testing/ScopedElement.hpp>
#include <Langulus/Testing/PackedPointers.hpp>
#include "Langulus/Logger.hpp"
#include <Langulus/Handle.hpp>
#include <Langulus/Any.hpp>
#include <Langulus/TAny.hpp>
#include <Langulus/SerializeText.hpp>
#include <Langulus/SerializeBytes.hpp>
#include <ranges>
#include <any>

#include <Langulus/Annies/Functional/Describe.hpp>
#include <Langulus/Annies/Functional/Compare.hpp>
#include <Langulus/Annies/Functional/Assign.hpp>
#include <Langulus/Annies/Functional/Insert.hpp>
#include <Langulus/Annies/Functional/Convert.hpp>
#include <Langulus/Annies/Functional/Deepen.hpp>
#include <Langulus/Annies/Functional/ForEach.hpp>
#include <Langulus/Annies/Functional/Merge.hpp>
#include <Langulus/Annies/Functional/Remove.hpp>

using namespace Langulus::Annies;

#if LANGULUS(BENCHMARK)
   /// MARK: Benchmark                                                        
   /// Perform a persistent benchmark across build and verify performance     
   #define BenchmarkAny(func, tolerance, my_init, my) { \
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
   #define BenchmarkAnyStd(func, tolerance_highscore, tolerance, my_init, my, theirs_init, theirs) { \
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
      const auto token_std = ::std::string("Test/") + func + "(" + NameOf<E>() + ") |std::any|"; \
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
   #define BenchmarkAny(func, tolerance, my_init, my)
   #define BenchmarkAnyStd(func, tolerance_highscore, tolerance, my_init, my, theirs_init, theirs)
#endif


struct SizeSummary {
   size_t size = 0;
   size_t stack_size = 0;
   size_t heap_size = 0;
   size_t heap_size_per_element = 0;
   size_t heap_size_per_indirection = 0;
   size_t heap_size_per_dimension = 0;
};

namespace Com = Annies::Component;

/// MARK: Gap test                                                            
template<class C, size_t DimensionsCount>
void Common_GapTest_Inner(SizeSummary& summary) {
   if constexpr (requires { typename C::Subcomponents; }) {
      ForEach(typename C::Subcomponents{}, [&]<class InnerC> {
         Common_GapTest_Inner<InnerC, DimensionsCount>(summary);
      });
   }
   else {
      if constexpr (requires { typename C::StackRequest; }) {
         // Scan all stack requests                                        
         using R = typename C::StackRequest;
         if constexpr (CT::NotVoid<R>) {
            Logger::Info(NameOf<C>(), " component reserves ",
               sizeof(R), " bytes on the stack");
            summary.stack_size += sizeof(R);
         }
      }

      if constexpr (requires { typename C::HeapRequest; }) {
         // Scan all heap requests                                         
         using R = typename C::HeapRequest;
         if constexpr (CT::NotVoid<R>) {
            if constexpr (Com::IsRequestModifier<R>) {
               constexpr size_t S = sizeof(TypeOf<R>);
               Logger::Info(NameOf<C>(), " component reserves ",
                  S, " bytes on the heap ");

               if constexpr (R::AllocatedPerElement) {
                  summary.heap_size_per_element += S;
                  Logger::Append("per element ");
               }

               if constexpr (R::AllocatedPerDimension) {
                  summary.heap_size_per_dimension += S;
                  Logger::Append("per dimension ");
                  if constexpr (not Com::IsFooterRequest<R>)
                     summary.heap_size += S * DimensionsCount;
               }

               if constexpr (R::AllocatedPerIndirection) {
                  summary.heap_size_per_indirection += S;
                  Logger::Append("per indirection ");
               }
            }
            else {
               constexpr size_t S = sizeof(R);
               Logger::Info(NameOf<C>(), " component reserves ",
                  S, " bytes on the heap");
               summary.heap_size += S;
            }
         }
      }
   }

   summary.size += sizeof(C);
}

/// Reports on a container, compares it to its std equivalent, makes sure it  
/// is tightly packed, properly sized, etc.                                   
template<class T, class COMPARE_WITH>
void Common_GapTest() {
   // Make sure the container is tightly packed with no padding         
   alignas(T) char unininitialized[sizeof(T)];
   memset(unininitialized, 0xff, sizeof(unininitialized));
   new (unininitialized) T {};
   size_t matched_bytes = 0;
   for (auto b : unininitialized) {
      if (b == 0xff)
         break;
      ++matched_bytes;
   }

   WARN(matched_bytes == sizeof(T));

   auto s = Logger::InfoSection(NameOf<T>(), " gap test:");
   if (matched_bytes != sizeof(T)) {
      Logger::Warning("Padding mask (FF corresponds to padding): ");
      Logger::Warning(" ");
      for (auto b : unininitialized)
         Logger::Append(Logger::Hex(b));
   }
   Logger::Info("Size of ", NameOf<COMPARE_WITH>(), " container is: ", sizeof(COMPARE_WITH), " bytes");
   Logger::Info("Size of ", NameOf<T>(), " container is: ", sizeof(T), " bytes");

   SizeSummary summary;
   ForEach(typename T::ComponentList{}, [&]<class C> {
      Common_GapTest_Inner<C, 1>(summary);
   });

   Logger::Info("-----------------------------------------");
   Logger::Info("For a total of ", summary.size,
      " bytes in components (should be optimized-out as empty bases)");
   Logger::Info("For a total of ", summary.stack_size,
      " bytes on the stack");

   if (summary.heap_size) {
      Logger::Info("For a total of ", summary.heap_size,
         " bytes on the heap header");
   }
   if (summary.heap_size_per_element) {
      Logger::Info("For a total of ", summary.heap_size_per_element,
         " bytes per element");
   }
   if (summary.heap_size_per_indirection) {
      Logger::Info("For a total of ", summary.heap_size_per_indirection,
         " bytes per indirection");
   }
   if (summary.heap_size_per_dimension) {
      Logger::Info("For a total of ", summary.heap_size_per_dimension,
         " bytes per dimension");
   }
}


namespace doctest
{
   /// MARK: {doctest}                                                        
   /// doctest stringifiers for _any_ container serializable to text.         
   /// This is used for all containers, inclusing sets, maps, pairs, handles..
   template<Langulus::CT::Container T>
   struct StringMaker<T> {
      static String convert(T const& value) {
         Langulus::Annies  ::Text as_text;
         ::Langulus::Serialize(value, as_text);
         return toString(
            static_cast<::std::string>(NameOf<T>() + "(" + as_text + ")")
         );
      }
   };
}


/// MARK: TestType                                                            
/// Tests if a container is of a particular type                              
template<class E, CT::Container C> requires CT::NoIntent<C>
void Any_Helper_TestType(const C& any) {
   if constexpr (CT::Void<E>) {
      REQUIRE(    any.template IsSame<int>());
      REQUIRE(    any.template IsExact<int>());
      REQUIRE(    any.template Is<int>());
      REQUIRE(not any.IsSparse());
      REQUIRE(not any.IsDeep());
      REQUIRE(    any.GetType() == MetaDataOf<int>());
   }
   else {
      REQUIRE(    any.template IsSame<E>());
      REQUIRE(    any.template IsExact<E>());
      REQUIRE(    any.template Is<E>());
      REQUIRE(    any.IsSparse() == CT::Sparse<E>);
      REQUIRE(    any.IsDeep() == CT::Deep<E>);
      REQUIRE(    any.GetType() == MetaDataOf<E>());      
   }
   
   REQUIRE(any.IsTyped());
}

/// MARK: TestSame                                                            
/// Tests if two containers point to the same memory the same way             
template<class LHS, class RHS> requires (CT::Container<LHS, RHS> and CT::NoIntent<LHS, RHS>)
void Any_Helper_TestSame(const LHS& lhs, const RHS& rhs, bool match_constness = true) {
   REQUIRE(lhs.GetCount() == rhs.GetCount());
   REQUIRE(lhs.IsExact(rhs.GetType()));
   REQUIRE(lhs == rhs);
   REQUIRE(lhs.IsDeep() == rhs.IsDeep());

   if (not lhs.IsEmpty())
      REQUIRE(lhs.GetRaw() == rhs.GetRaw());

   if (match_constness)
      REQUIRE(lhs.IsConstant() == rhs.IsConstant());

   if constexpr (requires { lhs.GetUnconstrainedState(); rhs.GetUnconstrainedState(); })
      REQUIRE(lhs.GetUnconstrainedState() == rhs.GetUnconstrainedState());
   else if constexpr (requires { lhs.GetUnconstrainedState(); })
      REQUIRE(lhs.IsDefaultState());
   else if constexpr (requires { rhs.GetUnconstrainedState(); })
      REQUIRE(rhs.IsDefaultState());
}

/// MARK: Default                                                             
/// Tests whether a container is in its default-constructed state             
template<class E, CT::Container C> requires CT::NoIntent<C>
void Common_CheckState_Default(const C& any, bool typed = false) {
   if constexpr (CT::Typed<C>) {
      static_assert(Exact<TypeOf<C>, E>);
      Any_Helper_TestType<E>(any);
   }
   else if (not typed) {
      REQUIRE_FALSE(any.IsTyped());
      REQUIRE      (any.GetType() == nullptr);
      REQUIRE_FALSE(any.IsSparse());
      REQUIRE_FALSE(any.IsDeep());
   }
   else Any_Helper_TestType<E>(any);

   REQUIRE      (any.IsDefaultState());
   REQUIRE      (any.IsTypeConstrained() == CT::Typed<C>);
   REQUIRE_FALSE(any.IsConstant());
   REQUIRE_FALSE(any.IsDisowned());
   REQUIRE_FALSE(any.IsValid());
   REQUIRE_FALSE(any.GetAllocation());
   REQUIRE      (any.IsEmpty());
   REQUIRE      (any.GetCount() == 0);
   REQUIRE      (any.GetReserved() == 0);
   REQUIRE      (any.GetUses() == 0);
   REQUIRE_FALSE(any);
   REQUIRE      (not any);
   REQUIRE      (any == C{});
}

template<class E, CT::Container C> requires CT::NoIntent<C>
void Any_CheckState_Default(const C& any, bool typed = false) {
   Common_CheckState_Default<E>(any, typed);

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
void Any_CheckState_OwnedEmpty(const C& any) {
   Any_Helper_TestType<E>(any);

   REQUIRE      (any.IsTypeConstrained() == CT::Typed<C>);
   REQUIRE      (any.IsConstant() == CT::Constant<E>);
   REQUIRE_FALSE(any.IsValid());
   REQUIRE_FALSE(any.IsDisowned());
   REQUIRE      (any.GetAllocation());
   REQUIRE      (any.IsEmpty());
   REQUIRE      (any.GetCount() == 0);
   REQUIRE      (any.GetReserved() > 0);
   REQUIRE      (any.GetUses() == 1);
   REQUIRE_FALSE(any);
   REQUIRE      (not any);
   REQUIRE      (any == C{});
}

/// MARK: OwnedFull                                                           
template<class E, CT::Container C> requires CT::NoIntent<C>
void Any_CheckState_OwnedFull(const C& any) {
   Any_Helper_TestType<E>(any);

   REQUIRE      (any.IsTypeConstrained() == CT::Typed<C>);
   REQUIRE      (any.IsConstant() == CT::Constant<E>);
   REQUIRE      (any.IsValid());
   REQUIRE      (any.GetAllocation());
   REQUIRE_FALSE(any.IsDisowned());
   REQUIRE_FALSE(any.IsEmpty());
   REQUIRE      (any.GetCount() > 0);
   REQUIRE      (any.GetReserved() > 0);
   REQUIRE      (any.GetUses() > 0);
   REQUIRE      (any.GetRaw());
   REQUIRE      (any);
   REQUIRE_FALSE(not any);
   REQUIRE      (any != C{});
}

/// MARK: DisownedFull                                                        
template<class E, CT::Container C> requires CT::NoIntent<C>
void Any_CheckState_DisownedFull(const C& any) {
   Any_Helper_TestType<E>(any);

   REQUIRE      (any.IsDisowned());
   REQUIRE      (any.IsTypeConstrained() == CT::Typed<C>);
   REQUIRE      (any.IsConstant());
   REQUIRE      (any.IsValid());
   //REQUIRE      (any.GetAllocation()); // A disowned container may or may not have an allocation
   REQUIRE_FALSE(any.IsEmpty());
   REQUIRE      (any.GetCount() > 0);
   if (any.GetAllocation()) {
      REQUIRE   (any.GetReserved() > 0);
      REQUIRE   (any.GetUses() > 0);
   }
   REQUIRE      (any.GetRaw());
   REQUIRE      (any);
   REQUIRE_FALSE(not any);
   REQUIRE      (any != C{});
}

/// MARK: Abandoned                                                           
template<class E, CT::Container C> requires CT::NoIntent<C>
void Any_CheckState_Abandoned(const C& any) {
   REQUIRE(any.IsDisowned());
}

/// MARK: ContainsOne                                                         
template<CT::Container T, CT::Intent I> requires CT::NoIntent<T>
void Any_CheckState_ContainsOne(T const& pack, I&& e_with_intent, int uses = 1) {
   auto& e = e_with_intent.what;
   using E = typename Decay<Deint<I>>::CTTI_Typed;
   REQUIRE(pack.GetCount() == 1);
   REQUIRE(pack.GetUses() == uses);
   REQUIRE(pack.GetReserved() >= (uses ? 1 : 0));
   REQUIRE(pack.template As<Decay<E>>() == DenseCast(*e));
   REQUIRE(DenseCast(pack.template As<E>()) == DenseCast(*e));

   if constexpr (CT::Cloned<I> and CT::Sparse<E>) {
      REQUIRE(pack.template As<E>() != *e);
      REQUIRE((*pack.template As<E*>()) != *e);
      REQUIRE(*pack.template GetRawAs<E>() != *e);
   }
   else {
      REQUIRE(pack.template As<E>() == *e);
      REQUIRE((*pack.template As<E*>()) == *e);
      REQUIRE(*pack.template GetRawAs<E>() == *e);
   }

   if constexpr (CT::OwnedDeep<T>) {
      auto entries = pack.GetEntries();
      if constexpr (CT::Dense<E>)
         REQUIRE(entries == nullptr);
      else if (uses) {
         REQUIRE(entries != nullptr);

         if constexpr (not CT::Disowned<I>) {
            for (size_t i = 0; i < IndirectsOf<E>; ++i) {
               if constexpr (CT::Cloned<I>)
                  REQUIRE(entries[i] != e.entries[i + 1]);
               else
                  REQUIRE(entries[i] == e.entries[i + 1]);
            }
         }
         else {
            for (size_t i = 0; i < IndirectsOf<E>; ++i)
               REQUIRE(entries[i] == nullptr);
         }
      }
   }

   if constexpr (CT::TypeErased<T>) {
      REQUIRE_THROWS(pack.template As<float>() == 0.0f);
      REQUIRE_THROWS(pack.template As<float*>() == nullptr);
   }
}

/// MARK: CompareOne                                                          
template<CT::Container T, class E> requires CT::NoIntent<T>
void Any_Helper_CompareOne(const T& pack, const E& e) {
   if constexpr (CT::TypeErased<T>) {
      REQUIRE(pack.CompareOne(e) == Compared::Equal);
      REQUIRE(pack.CompareOneEqual(e) == true);
   }
   else {
      REQUIRE(pack.CompareOne(e) == ::std::partial_ordering::equivalent);
      REQUIRE(pack.CompareOneEqual(e) == true);
   }

   if constexpr (CT::Deep<E> and LANGULUS(SAFE))
      REQUIRE_THROWS(pack == e);
   else
      REQUIRE_NOTHROW(pack == e);
}
