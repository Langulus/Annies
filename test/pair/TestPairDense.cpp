///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "TestPairCommon.hpp"


/// The main test for TPair/Pair containers, with all kinds of items, from    
/// sparse to dense, from trivial to complex, from flat to deep               
TEMPLATE_TEST_CASE("Dense TPair/Pair", "[pair]",
   (MapTest<Pair, Text, int>),

   (MapTest<TPair<Text, int>, Text, int>),
   (MapTest<TPair<Text, Trait>, Text, Trait>),
   (MapTest<TPair<Text, Traits::Count>, Text, Traits::Count>),
   (MapTest<TPair<Text, Many>, Text, Many>),

   (MapTest<Pair, Text, Trait>),
   (MapTest<Pair, Text, Traits::Count>),
   (MapTest<Pair, Text, Many>)
) {
   static Allocator::State memoryState;

   using T = typename TestType::Container;
   using K = typename TestType::Key;
   using V = typename TestType::Value;
   using stdT = ::std::pair<K, V>;

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

   const auto lp = CreatePair<T, K, V>("five hundred", 555);
   Any_Helper_TestType<K>(lp.GetKeyBlock());
   Any_Helper_TestType<V>(lp.GetValueBlock());

   [[maybe_unused]] const auto sp = CreatePair<stdT, K, V>("five hundred", 555);


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

         REQUIRE(movablePair != lp);
         REQUIRE(pair == lp);
         REQUIRE(pair.GetKey() == lp.GetKey());
         REQUIRE(pair.GetValue() == lp.GetValue());

         #ifdef LANGULUS_STD_BENCHMARK
            BENCHMARK_ADVANCED("Annies::pair::operator = (single pair copy)") (timer meter) {
               some<T> source(meter.runs());
               for (auto& i : source)
                  i = CreatePair<Pair, K, V>("five hundred"_text, 555);
                  
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

      REQUIRE(pair == lp);
      REQUIRE(pair.GetKey() == lp.GetKey());
      REQUIRE(pair.GetValue() == lp.GetValue());
   }
   
   GIVEN("Pair with some items") {
      T pair {lp};

      WHEN("Pair is cleared") {
         pair.Clear();

         Pair_CheckState_Default<K, V>(pair);

         REQUIRE(pair != lp);
         REQUIRE(((pair.GetKey() != lp.GetKey()) or (pair.GetValue() != lp.GetValue())));
      }

      WHEN("Pair is reset") {
         pair.Reset();

         Pair_CheckState_Default<K, V>(pair);
      }

      WHEN("Pair is shallow-copied") {
         auto copy = pair;

         Pair_CheckState_OwnedFull<K, V>(pair);
         Pair_CheckState_OwnedFull<K, V>(copy);

         REQUIRE(copy == pair);
         REQUIRE(copy.GetKey() == pair.GetKey());
         REQUIRE(copy.GetValue() == pair.GetValue());
      }

      WHEN("Pair is cloned") {
         T clone = Clone(pair);

         Pair_CheckState_OwnedFull<K, V>(pair);
         Pair_CheckState_OwnedFull<K, V>(clone);

         REQUIRE((clone != pair) == (CT::Sparse<K> or CT::Sparse<V>));
         REQUIRE(clone.GetKey() == pair.GetKey());
         REQUIRE(clone.GetValue() == pair.GetValue());
      }

      WHEN("Pair is move-constructed") {
         T movable = pair;
         T moved = ::std::move(movable);

         Pair_CheckState_Default<K, V>(movable);
         Pair_CheckState_OwnedFull<K, V>(moved);

         REQUIRE(moved == pair);
         REQUIRE(moved != movable);
      }

      WHEN("Pairs are compared") {
         T samePair   {lp};
         T clonedPair {Clone(pair)};
         T copiedPair {pair};
         T differentPair1 {CreatePair<T, K, V>("not five hundred", -555)};

         REQUIRE(pair == samePair);
         REQUIRE((pair != clonedPair) == (CT::Sparse<K> or CT::Sparse<V>));
         REQUIRE(pair == copiedPair);
         REQUIRE(pair != differentPair1);
      }
   }

   DestroyPair(lp);
   DestroyPair(sp);

   REQUIRE(memoryState.Assert());

   // Destroy BANK before static data - otherwise problems happen if    
   // not using managed reflection                                      
   BANK.Reset();

   REQUIRE_FALSE(Allocator::CollectGarbage());
}