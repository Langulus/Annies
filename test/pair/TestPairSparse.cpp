///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "TestPairCommon.hpp"

#define PAIR_TESTS(MANAGED) \
   (MapTest<Pair, Trait*, RT*, MANAGED>), \
 \
   (MapTest<TPair<Trait*, RT*>, Trait*, RT*, MANAGED>), \
   (MapTest<TPair<Traits::Count*, RT*>, Traits::Count*, RT*, MANAGED>), \
   (MapTest<TPair<Many*, RT*>, Many*, RT*, MANAGED>), \
   (MapTest<TPair<RT*, RT*>, RT*, RT*, MANAGED>), \
 \
   (MapTest<Pair, Traits::Count*, RT*, MANAGED>), \
   (MapTest<Pair, Many*, RT*, MANAGED>), \
   (MapTest<Pair, RT*, RT*, MANAGED>)


/// The main test for TPair/Pair containers, with all kinds of items, from    
/// sparse to dense, from trivial to complex, from flat to deep               
#if LANGULUS_FEATURE(MANAGED_MEMORY)
TEMPLATE_TEST_CASE("Sparse TPair/Pair", "[pair]",
   //TODO PAIR_TESTS(true),
   PAIR_TESTS(false)
) {
#else
TEMPLATE_TEST_CASE("Sparse TPair/Pair", "[pair]",
   PAIR_TESTS(false)
) {
#endif
   static Allocator::State memoryState;

   using T = typename TestType::Container;
   using K = typename TestType::Key;
   using V = typename TestType::Value;
   using stdT = ::std::pair<K, V>;
   constexpr bool MANAGED = TestType::Managed;

   const auto lp = CreatePair<T,    K, V, MANAGED>("five hundred", 555);
   const auto sp = CreatePair<stdT, K, V, MANAGED>("five hundred", 555);

   if constexpr (CT::Untyped<T>) {
      // All type-erased containers should have all intent              
      // constructors and assigners available, and errors will instead  
      // be thrown as exceptions at runtime                             
      static_assert(CT::CopyMakable<T>);
      static_assert(CT::ReferMakable<T>);
      static_assert(CT::AbandonMakable<T>);
      static_assert(CT::MoveMakable<T>);
      static_assert(CT::CloneMakable<T>);
      static_assert(CT::DisownMakable<T>);

      static_assert(CT::CopyAssignable<T>);
      static_assert(CT::ReferAssignable<T>);
      static_assert(CT::AbandonAssignable<T>);
      static_assert(CT::MoveAssignable<T>);
      static_assert(CT::CloneAssignable<T>);
      static_assert(CT::DisownAssignable<T>);
   }

   GIVEN("A default-initialized pair instance") {
      T pair {};

      WHEN("Given a default-constructed pair") {
         Pair_CheckState_Default<K, V>(pair);

         #ifdef LANGULUS_STD_BENCHMARK
            BENCHMARK_ADVANCED("Annies::pair::default construction") (timer meter) {
               some<uninitialized<T>> storage(meter.runs());
               meter.measure([&](int i) {
                  return storage[i].construct();
               });
            };

            BENCHMARK_ADVANCED("std::pair::default construction") (timer meter) {
               some<uninitialized<stdT>> storage(meter.runs());
               meter.measure([&](int i) {
                  return storage[i].construct();
               });
            };
         #endif
      }

      WHEN("Assigned a pair by move") {
         auto movablePair = lp;
         pair = ::std::move(movablePair);

         Pair_CheckState_Default<K, V>(movablePair);
         Pair_CheckState_OwnedFull<K, V>(pair);

         REQUIRE(pair.GetKey() == lp.GetKey());
         REQUIRE(pair.GetValue() == lp.GetValue());
         REQUIRE(pair == lp);

         #ifdef LANGULUS_STD_BENCHMARK
            BENCHMARK_ADVANCED("Annies::TPair::operator = (single pair copy)") (timer meter) {
               some<T> source(meter.runs());
               for (auto& i : source)
                  i = CreatePair<Pair, K, V>("five hundred", 555);
                  
               some<T> storage(meter.runs());
               meter.measure([&](int i) {
                  return storage[i] = ::std::move(source[i]);
               });
            };

            BENCHMARK_ADVANCED("std::pair::insert(single pair copy)") (timer meter) {
               some<stdT> source(meter.runs());
               for(auto& i : source)
                  i = valueStd;

               some<stdT> storage(meter.runs());
               meter.measure([&](int i) {
                  return storage[i].emplace(::std::move(source[i]));
               });
            };
         #endif
      }
   }
   
   GIVEN("A copy-initialized pair instance") {
      T pair {lp};

      Pair_CheckState_OwnedFull<K, V>(pair);

      REQUIRE(pair.GetKey() == lp.GetKey());
      REQUIRE(pair.GetValue() == lp.GetValue());
      REQUIRE(pair == lp);
      //TODO benchmark
   }
   
   GIVEN("Map with some items") {
      T pair {lp};

      WHEN("Pair is cleared") {
         pair.Clear();

         Pair_CheckState_Default<K, V>(pair);
      }

      WHEN("Pair is reset") {
         pair.Reset();

         Pair_CheckState_Default<K, V>(pair);
      }

      WHEN("Pair is shallow-copied") {
         auto copy = pair;

         Pair_CheckState_OwnedFull<K, V>(pair);
         Pair_CheckState_OwnedFull<K, V>(copy);

         REQUIRE(copy.GetKey() == pair.GetKey());
         REQUIRE(copy.GetValue() == pair.GetValue());
         REQUIRE(copy == pair);
         REQUIRE(copy == lp);
         //TODO benchmark  
      }

      WHEN("Pair is cloned") {
         if constexpr (CT::CloneMakable<K, V>) {
            T clone = Clone(pair);

            Pair_CheckState_OwnedFull<K, V>(pair);
            Pair_CheckState_OwnedFull<K, V>(clone);

            REQUIRE(clone.GetKey() != pair.GetKey());
            REQUIRE(clone.GetValue() != pair.GetValue());
            REQUIRE(clone != pair);
         }
         else if constexpr (CT::Untyped<T>) {
            T clone;
            REQUIRE_THROWS(new (&clone) T {Clone(pair)});
         }
      }

      WHEN("Pair is move-constructed") {
         T movable = pair;
         T moved = ::std::move(movable);

         Pair_CheckState_Default<K, V>(movable);
         Pair_CheckState_OwnedFull<K, V>(moved);

         REQUIRE(moved.GetKey() == pair.GetKey());
         REQUIRE(moved.GetValue() == pair.GetValue());
         REQUIRE(moved != movable);
         REQUIRE(moved == pair);
         //TODO benchmark  
      }

      WHEN("Pairs are compared") {
         T samePair = lp;
         T copiedPair = pair;
         T differentPair = CreatePair<T, K, V, MANAGED>("not five hundred", -555);

         REQUIRE(pair == samePair);
         REQUIRE(pair == copiedPair);
         REQUIRE(pair != differentPair);

         DestroyPair<MANAGED>(differentPair);
      }
   }

   DestroyPair<MANAGED>(lp);
   DestroyPair<MANAGED>(sp);

   REQUIRE(memoryState.Assert());

   // Destroy BANK before static data - otherwise problems happen if    
   // not using managed reflection                                      
   BANK.Reset();

   REQUIRE_FALSE(Allocator::CollectGarbage());
}
