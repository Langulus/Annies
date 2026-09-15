///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "TestMapCommon.hpp"


/// The main test for TOrderedMap/TUnorderedMap/OrderedMap/UnorderedMap       
/// containers, with all kinds of items, from sparse to dense, from trivial   
/// to complex, from flat to deep                                             
TEMPLATE_TEST_CASE(
   "Dense TOrderedMap/TUnorderedMap/OrderedMap/UnorderedMap", "[map]",
   (MapTest<UnorderedMap, Text, int>),

   (MapTest<TUnorderedMap<Text, int>, Text, int>),
   (MapTest<TUnorderedMap<Text, Trait>, Text, Trait>),
   (MapTest<TUnorderedMap<Text, Traits::Count>, Text, Traits::Count>),
   (MapTest<TUnorderedMap<Text, Many>, Text, Many>),

   (MapTest<TOrderedMap<Text, int>, Text, int>),
   (MapTest<TOrderedMap<Text, Trait>, Text, Trait>),
   (MapTest<TOrderedMap<Text, Traits::Count>, Text, Traits::Count>),
   (MapTest<TOrderedMap<Text, Many>, Text, Many>),

   (MapTest<UnorderedMap, Text, Trait>),
   (MapTest<UnorderedMap, Text, Traits::Count>),
   (MapTest<UnorderedMap, Text, Many>),

   (MapTest<OrderedMap, Text, int>),
   (MapTest<OrderedMap, Text, Trait>),
   (MapTest<OrderedMap, Text, Traits::Count>),
   (MapTest<OrderedMap, Text, Many>)
) {
   static Allocator::State memoryState;

   using T = typename TestType::Container;
   using K = typename TestType::Key;
   using V = typename TestType::Value;
   using Pair = TPair<K, V>;
   using StdPair = ::std::pair<K, V>;

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

   const auto pair = CreatePair<Pair, K, V>("five hundred", 555);
   [[maybe_unused]] const auto stdpair = CreatePair<StdPair, K, V>("five hundred", 555);

   const Pair darray1[5] {
      CreatePair<Pair, K, V>("one", 1),
      CreatePair<Pair, K, V>("two", 2),
      CreatePair<Pair, K, V>("three", 3),
      CreatePair<Pair, K, V>("four", 4),
      CreatePair<Pair, K, V>("five", 5)
   };
   const Pair darray2[5] {
      CreatePair<Pair, K, V>("six", 6),
      CreatePair<Pair, K, V>("seven", 7),
      CreatePair<Pair, K, V>("eight", 8),
      CreatePair<Pair, K, V>("nine", 9),
      CreatePair<Pair, K, V>("ten", 10)
   };
   
   [[maybe_unused]] const StdPair darray1std[5] {
      CreatePair<StdPair, K, V>("one", 1),
      CreatePair<StdPair, K, V>("two", 2),
      CreatePair<StdPair, K, V>("three", 3),
      CreatePair<StdPair, K, V>("four", 4),
      CreatePair<StdPair, K, V>("five", 5)
   };
   [[maybe_unused]] const StdPair darray2std[5] {
      CreatePair<StdPair, K, V>("six", 6),
      CreatePair<StdPair, K, V>("seven", 7),
      CreatePair<StdPair, K, V>("eight", 8),
      CreatePair<StdPair, K, V>("nine", 9),
      CreatePair<StdPair, K, V>("ten", 10)
   };


   GIVEN("A default-initialized map instance") {
      T map {};

      WHEN("Given a default-constructed map") {
         Map_CheckState_Default<K, V>(map);

         #ifdef LANGULUS_STD_BENCHMARK
            BENCHMARK_ADVANCED("Annies::map::default construction") (timer meter) {
               some<uninitialized<MapType>> storage(meter.runs());
               meter.measure([&](int i) {
                  return storage[i].construct();
               });
            };

            BENCHMARK_ADVANCED("std::map::default construction") (timer meter) {
               some<uninitialized<MapTypeStd>> storage(meter.runs());
               meter.measure([&](int i) {
                  return storage[i].construct();
               });
            };
         #endif
      }

      WHEN("Assigned a pair by move") {
         auto movablePair = pair;
         map = ::std::move(movablePair);

         Map_CheckState_OwnedFull<K, V>(map);

         REQUIRE(movablePair != pair);
         REQUIRE(map.GetCount() == 1);
         REQUIRE(map[pair.GetKey()] == pair.GetValue());
         REQUIRE(map["five hundred"] == pair.GetValue());
         REQUIRE_THROWS(map["missing"] != pair.GetValue());

         #ifdef LANGULUS_STD_BENCHMARK
            BENCHMARK_ADVANCED("Annies::TUnorderedMap::operator = (single pair copy)") (timer meter) {
               some<Pair> source(meter.runs());
               for (auto& i : source)
                  i = CreatePair<Pair, K, V>("five hundred"_text, 555);
                  
               some<MapType> storage(meter.runs());
               meter.measure([&](int i) {
                  return storage[i] = ::std::move(source[i]);
               });
            };

            BENCHMARK_ADVANCED("std::unordered_map::insert(single pair copy)") (timer meter) {
               some<StdPair> source(meter.runs());
               for(auto& i : source)
                  i = valueStd;

               some<MapTypeStd> storage(meter.runs());
               meter.measure([&](int i) {
                  return storage[i].emplace(::std::move(source[i]));
               });
            };
         #endif
      }
   }
   
   GIVEN("A pair copy-initialized map instance") {
      T map {pair};

      Map_CheckState_OwnedFull<K, V>(map);

      REQUIRE(map.GetCount() == 1);
      REQUIRE(map[pair.GetKey()] == pair.GetValue());
      REQUIRE(map["five hundred"] == pair.GetValue());
      REQUIRE_THROWS(map["missing"] != pair.GetValue());
   }
   
   GIVEN("A pair array copy-initialized map instance") {
      T map {darray1};

      Map_CheckState_OwnedFull<K, V>(map);

      REQUIRE(map.GetCount() == 5);
      for (auto& comparer : darray1)
         REQUIRE(map[comparer.GetKey()] == comparer.GetValue());
      REQUIRE(map.GetReserved() >= 5);
   }

   GIVEN("Map with some items") {
      T map {};
      map << darray1[0];
      map << darray1[1];
      map << darray1[2];
      map << darray1[3];
      map << darray1[4];

      auto keyMemory = map.GetRawKeysMemory();
      auto valueMemory = map.GetRawValsMemory();

      Map_CheckState_OwnedFull<K, V>(map);

      REQUIRE(map.GetCount() == 5);
      REQUIRE_FALSE(map.template IsKey<int>());
      REQUIRE_FALSE(map.template IsKey<char>());
      REQUIRE_FALSE(map.template IsValue<float>());
      REQUIRE_FALSE(map.template IsValue<unsigned char>());
      for (auto& comparer : darray1)
         REQUIRE(map[comparer.GetKey()] == comparer.GetValue());
      REQUIRE(map.GetReserved() >= 5);

      WHEN("Shallow-copy more of the same stuff") {
         for (auto& comparer : darray1)
            REQUIRE(map[comparer.GetKey()] == comparer.GetValue());

         map << darray2[0];
         for (auto& comparer : darray1)
            REQUIRE(map[comparer.GetKey()] == comparer.GetValue());

         map << darray2[1];
         for (auto& comparer : darray1)
            REQUIRE(map[comparer.GetKey()] == comparer.GetValue());

         map << darray2[2];
         for (auto& comparer : darray1)
            REQUIRE(map[comparer.GetKey()] == comparer.GetValue());

         /*Logger::SpecialTab("Map before: ");
         for (auto p : map)
            Logger::Append(p.mKey.As<Text>(), ", ");*/

         map << darray2[3];

         /*Logger::SpecialTab("Map after: ");
         for (auto p : map)
            Logger::Append(p.mKey.As<Text>(), ", ");*/

         for (auto& comparer : darray1)
            REQUIRE(map[comparer.GetKey()] == comparer.GetValue());

         map << darray2[4];
         for (auto& comparer : darray1)
            REQUIRE(map[comparer.GetKey()] == comparer.GetValue());

         Map_CheckState_OwnedFull<K, V>(map);

         REQUIRE(map.GetCount() == 10);
         for (auto& comparer : darray1)
            REQUIRE(map[comparer.GetKey()] == comparer.GetValue());
         for (auto& comparer : darray2)
            REQUIRE(map[comparer.GetKey()] == comparer.GetValue());
         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            REQUIRE(map.GetRawKeysMemory() == keyMemory);
            REQUIRE(map.GetRawValsMemory() == valueMemory);
         #endif
         REQUIRE(map.GetReserved() >= 10);

         #ifdef LANGULUS_STD_BENCHMARK
            BENCHMARK_ADVANCED("Annies::TUnorderedMap::operator << (5 consecutive pair copies)") (timer meter) {
               some<MapType> storage(meter.runs());
               for (auto& i : storage)
                  i << darray1[0] << darray1[1] << darray1[2] << darray1[3] << darray1[4];

               meter.measure([&](int i) {
                  return storage[i] << darray2[0] << darray2[1] << darray2[2] << darray2[3] << darray2[4];
               });
            };

            BENCHMARK_ADVANCED("std::unordered_map::insert(5 consecutive pair copies)") (timer meter) {
               some<MapTypeStd> storage(meter.runs());
               for (auto& i : storage) {
                  i.insert(darray1std[0]);
                  i.insert(darray1std[1]);
                  i.insert(darray1std[2]);
                  i.insert(darray1std[3]);
                  i.insert(darray1std[4]);
               }

               meter.measure([&](int i) {
                  storage[i].insert(darray2std[0]);
                  storage[i].insert(darray2std[1]);
                  storage[i].insert(darray2std[2]);
                  storage[i].insert(darray2std[3]);
                  return storage[i].insert(darray2std[4]);
               });
            };

            // Last result: 1:1, slightly slower than STD, can be further improved
            BENCHMARK_ADVANCED("Annies::TUnorderedMap::operator [] (retrieval by key from a map with 10 pairs)") (timer meter) {
               some<MapType> storage(meter.runs());
               for (auto& i : storage) {
                  i << darray1[0] << darray1[1] << darray1[2] << darray1[3] << darray1[4];
                  i << darray2[0] << darray2[1] << darray2[2] << darray2[3] << darray2[4];
               }

               meter.measure([&](int i) {
                  return storage[i]["seven"];
               });
            };

            BENCHMARK_ADVANCED("std::unordered_map::operator [] (retrieval by key from a map with 10 pairs)") (timer meter) {
               some<MapTypeStd> storage(meter.runs());
               for (auto& i : storage) {
                  i.insert(darray1std[0]);
                  i.insert(darray1std[1]);
                  i.insert(darray1std[2]);
                  i.insert(darray1std[3]);
                  i.insert(darray1std[4]);
                  i.insert(darray2std[0]);
                  i.insert(darray2std[1]);
                  i.insert(darray2std[2]);
                  i.insert(darray2std[3]);
                  i.insert(darray2std[4]);
               }

               meter.measure([&](int i) {
                  return storage[i]["seven"];
               });
            };
         #endif
      }

      WHEN("Move more of the same stuff") {
         Pair movableDarray2[5] {
            darray2[0],
            darray2[1],
            darray2[2],
            darray2[3],
            darray2[4]
         };

         map << ::std::move(movableDarray2[0])
             << ::std::move(movableDarray2[1])
             << ::std::move(movableDarray2[2])
             << ::std::move(movableDarray2[3])
             << ::std::move(movableDarray2[4]);

         Map_CheckState_OwnedFull<K, V>(map);

         REQUIRE(map.GetCount() == 10);
         for (auto& comparer : darray1)
            REQUIRE(map[comparer.GetKey()] == comparer.GetValue());
         for (auto& comparer : darray2)
            REQUIRE(map[comparer.GetKey()] == comparer.GetValue());
         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            REQUIRE(map.GetRawKeysMemory() == keyMemory);
            REQUIRE(map.GetRawValsMemory() == valueMemory);
         #endif
         REQUIRE(map.GetReserved() >= 10);

         #ifdef LANGULUS_STD_BENCHMARK
            BENCHMARK_ADVANCED("Annies::TUnorderedMap::operator << (5 consecutive trivial moves)") (timer meter) {
               some<MapType> storage(meter.runs());
               meter.measure([&](int i) {
                  return storage[i] 
                     << ::std::move(darray2[0]) 
                     << ::std::move(darray2[1]) 
                     << ::std::move(darray2[2]) 
                     << ::std::move(darray2[3]) 
                     << ::std::move(darray2[4]);
               });
            };

            BENCHMARK_ADVANCED("std::unordered_map::emplace_back(5 consecutive trivial moves)") (timer meter) {
               some<MapTypeStd> storage(meter.runs());
               meter.measure([&](int i) {
                  storage[i].emplace(::std::move(darray2std[0]));
                  storage[i].emplace(::std::move(darray2std[1]));
                  storage[i].emplace(::std::move(darray2std[2]));
                  storage[i].emplace(::std::move(darray2std[3]));
                  return storage[i].emplace(::std::move(darray2std[4]));
               });
            };
         #endif
      }

      WHEN("Removing elements by value") {
         const auto removed2 = map.RemoveValue(darray1[1].GetValue());
         const auto removed4 = map.RemoveValue(darray1[3].GetValue());

         Map_CheckState_OwnedFull<K, V>(map);

         REQUIRE(removed2 == 1);
         REQUIRE(removed4 == 1);
         REQUIRE(map.GetCount() == 3);
         REQUIRE(map.GetRawKeysMemory() == keyMemory);
         REQUIRE(map.GetRawValsMemory() == valueMemory);
         REQUIRE(map.GetReserved() >= 5);

         REQUIRE(map.ContainsKey(darray1[0].GetKey()));
         REQUIRE_FALSE(map.ContainsKey(darray1[1].GetKey()));
         REQUIRE(map.ContainsKey(darray1[2].GetKey()));
         REQUIRE_FALSE(map.ContainsKey(darray1[3].GetKey()));
         REQUIRE(map.ContainsKey(darray1[4].GetKey()));

         REQUIRE(map.ContainsValue(darray1[0].GetValue()));
         REQUIRE_FALSE(map.ContainsValue(darray1[1].GetValue()));
         REQUIRE(map.ContainsValue(darray1[2].GetValue()));
         REQUIRE_FALSE(map.ContainsValue(darray1[3].GetValue()));
         REQUIRE(map.ContainsValue(darray1[4].GetValue()));

         #ifdef LANGULUS_STD_BENCHMARK
            BENCHMARK_ADVANCED("Annies::TUnorderedMap::RemoveValue") (timer meter) {
               some<MapType> storage(meter.runs());
               for (auto&& o : storage)
                  o << darray1[0] << darray1[1] << darray1[2] << darray1[3] << darray1[4];

               meter.measure([&](int i) {
                  return storage[i].RemoveValue(2);
               });
            };

            BENCHMARK_ADVANCED("std::unordered_map::erase(by value)") (timer meter) {
               some<MapTypeStd> storage(meter.runs());
               for (auto&& i : storage) {
                  i.insert(darray1std[0]);
                  i.insert(darray1std[1]);
                  i.insert(darray1std[2]);
                  i.insert(darray1std[3]);
                  i.insert(darray1std[4]);
               }

               meter.measure([&](int i) {
                  auto it = storage[i].begin();
                  while (it != storage[i].end()) {
                     if (it->second == 2) {
                        it = storage[i].erase(it);
                        continue;
                     }
                     it++;
                  }
                  return it;
               });
            };
         #endif
      }

      for (int iii = 0; iii < 10; ++iii) {
      WHEN(std::string("Removing elements by key #") + std::to_string(iii)) {
         const auto removed2 = map.RemoveKey(darray1[1].GetKey());
         const auto removed4 = map.RemoveKey(darray1[3].GetKey());

         Map_CheckState_OwnedFull<K, V>(map);

         REQUIRE(removed2 == 1);
         REQUIRE(removed4 == 1);
         REQUIRE(map.GetCount() == 3);
         REQUIRE(map.GetRawKeysMemory() == keyMemory);
         REQUIRE(map.GetRawValsMemory() == valueMemory);
         REQUIRE(map.GetReserved() >= 5);

         REQUIRE(map.ContainsKey(darray1[0].GetKey()));
         REQUIRE_FALSE(map.ContainsKey(darray1[1].GetKey()));
         REQUIRE(map.ContainsKey(darray1[2].GetKey()));
         REQUIRE_FALSE(map.ContainsKey(darray1[3].GetKey()));
         REQUIRE(map.ContainsKey(darray1[4].GetKey()));

         REQUIRE(map.ContainsValue(darray1[0].GetValue()));
         REQUIRE_FALSE(map.ContainsValue(darray1[1].GetValue()));
         REQUIRE(map.ContainsValue(darray1[2].GetValue()));
         REQUIRE_FALSE(map.ContainsValue(darray1[3].GetValue()));
         REQUIRE(map.ContainsValue(darray1[4].GetValue()));

         #ifdef LANGULUS_STD_BENCHMARK
            BENCHMARK_ADVANCED("Annies::TUnorderedMap::RemoveKey") (timer meter) {
               some<MapType> storage(meter.runs());
               for (auto&& o : storage)
                  o << darray1[0] << darray1[1] << darray1[2] << darray1[3] << darray1[4];

               meter.measure([&](int i) {
                  return storage[i].RemoveKey("two");
               });
            };

            BENCHMARK_ADVANCED("std::unordered_map::erase(by key)") (timer meter) {
               some<MapTypeStd> storage(meter.runs());
               for (auto&& i : storage) {
                  i.insert(darray1std[0]);
                  i.insert(darray1std[1]);
                  i.insert(darray1std[2]);
                  i.insert(darray1std[3]);
                  i.insert(darray1std[4]);
               }

               meter.measure([&](int i) {
                  return storage[i].erase("two");
               });
            };
         #endif
      }
      }

      WHEN("Removing non-available elements by value") {
         const auto removed9 = map.RemoveValue(darray2[3].GetValue());

         Map_CheckState_OwnedFull<K, V>(map);

         REQUIRE(removed9 == 0);
         for (auto& comparer : darray1)
            REQUIRE(map[comparer.GetKey()] == comparer.GetValue());
         REQUIRE(map.GetCount() == 5);
         REQUIRE(map.GetRawKeysMemory() == keyMemory);
         REQUIRE(map.GetRawValsMemory() == valueMemory);
         REQUIRE(map.GetReserved() >= 5);

         REQUIRE(map.ContainsKey(darray1[0].GetKey()));
         REQUIRE(map.ContainsKey(darray1[1].GetKey()));
         REQUIRE(map.ContainsKey(darray1[2].GetKey()));
         REQUIRE(map.ContainsKey(darray1[3].GetKey()));
         REQUIRE(map.ContainsKey(darray1[4].GetKey()));

         REQUIRE(map.ContainsValue(darray1[0].GetValue()));
         REQUIRE(map.ContainsValue(darray1[1].GetValue()));
         REQUIRE(map.ContainsValue(darray1[2].GetValue()));
         REQUIRE(map.ContainsValue(darray1[3].GetValue()));
         REQUIRE(map.ContainsValue(darray1[4].GetValue()));
      }
      
      WHEN("Removing non-available elements by key") {
         const auto removed9 = map.RemoveKey(darray2[3].GetKey());

         Map_CheckState_OwnedFull<K, V>(map);

         REQUIRE(removed9 == 0);
         for (auto& comparer : darray1)
            REQUIRE(map[comparer.GetKey()] == comparer.GetValue());
         REQUIRE(map.GetCount() == 5);
         REQUIRE(map.GetRawKeysMemory() == keyMemory);
         REQUIRE(map.GetRawValsMemory() == valueMemory);
         REQUIRE(map.GetReserved() >= 5);

         REQUIRE(map.ContainsKey(darray1[0].GetKey()));
         REQUIRE(map.ContainsKey(darray1[1].GetKey()));
         REQUIRE(map.ContainsKey(darray1[2].GetKey()));
         REQUIRE(map.ContainsKey(darray1[3].GetKey()));
         REQUIRE(map.ContainsKey(darray1[4].GetKey()));

         REQUIRE(map.ContainsValue(darray1[0].GetValue()));
         REQUIRE(map.ContainsValue(darray1[1].GetValue()));
         REQUIRE(map.ContainsValue(darray1[2].GetValue()));
         REQUIRE(map.ContainsValue(darray1[3].GetValue()));
         REQUIRE(map.ContainsValue(darray1[4].GetValue()));
      }
      
      WHEN("More capacity is reserved") {
         map.Reserve(20);

         Map_CheckState_OwnedFull<K, V>(map);

         REQUIRE(map.GetCount() == 5);
         REQUIRE(map.GetReserved() >= 20);
      }

      WHEN("Less capacity is reserved") {
         map.Reserve(2);

         Map_CheckState_OwnedFull<K, V>(map);

         REQUIRE(map.GetCount() == 5);
         REQUIRE(map.GetRawKeysMemory() == keyMemory);
         REQUIRE(map.GetRawValsMemory() == valueMemory);
         REQUIRE(map.GetReserved() >= 5);
      }

      WHEN("Map is cleared") {
         map.Clear();

         Map_CheckState_OwnedEmpty<K, V>(map);

         REQUIRE(map.GetRawKeysMemory() == keyMemory);
         REQUIRE(map.GetRawValsMemory() == valueMemory);
         REQUIRE(map.GetReserved() >= 5);
      }

      WHEN("Map is reset") {
         map.Reset();

         Map_CheckState_Default<K, V>(map);
      }

      WHEN("Map is shallow-copied") {
         auto copy = map;

         Map_CheckState_OwnedFull<K, V>(copy);
         Map_CheckState_OwnedFull<K, V>(map);

         REQUIRE(copy == map);
         REQUIRE(copy.GetKeys().GetUses() == 2);
         REQUIRE(copy.GetVals().GetUses() == 2);
         REQUIRE(copy.GetCount() == map.GetCount());
         REQUIRE(copy.GetCount() == 5);
         REQUIRE(copy.GetRawKeysMemory() == map.GetRawKeysMemory());
         REQUIRE(copy.GetRawValsMemory() == map.GetRawValsMemory());
         for (auto& comparer : darray1)
            REQUIRE(copy[comparer.GetKey()] == comparer.GetValue());

         if constexpr (CT::Typed<T>) {
            for (auto& comparer : darray1)
               REQUIRE(&map[comparer.GetKey()] == &copy[comparer.GetKey()]);
         }
      }

      WHEN("Map is cloned") {
         T clone = Langulus::Clone(map);

         Map_CheckState_OwnedFull<K, V>(clone);
         Map_CheckState_OwnedFull<K, V>(map);

         REQUIRE((clone != map) == (CT::Sparse<K> or CT::Sparse<V>));
         REQUIRE(clone.GetKeys().GetUses() == 1);
         REQUIRE(clone.GetVals().GetUses() == 1);
         REQUIRE(clone.GetCount() == map.GetCount());
         REQUIRE(clone.GetCount() == 5);
         REQUIRE(clone.GetRawKeysMemory() != map.GetRawKeysMemory());
         REQUIRE(clone.GetRawValsMemory() != map.GetRawValsMemory());
         for (auto& comparer : darray1) {
            if constexpr (CT::Sparse<V>) {
               REQUIRE(clone[comparer.GetKey()] != comparer.GetValue());
               REQUIRE(map[comparer.GetKey()] != clone[comparer.GetKey()]);
            }
            else {
               REQUIRE(clone[comparer.GetKey()] == comparer.GetValue());
               REQUIRE(map[comparer.GetKey()] == clone[comparer.GetKey()]);
            }
               
            REQUIRE(map[comparer.GetKey()] == comparer.GetValue());
               
            if constexpr (CT::Typed<T>)
               REQUIRE(&map[comparer.GetKey()] != &clone[comparer.GetKey()]);
            else
               REQUIRE(map[comparer.GetKey()].GetRaw() != clone[comparer.GetKey()].GetRaw());
         }
      }

      WHEN("Map is move-constructed") {
         T movable = map;
         T moved = ::std::move(movable);

         Map_CheckState_Default<K, V>(movable);
         Map_CheckState_OwnedFull<K, V>(moved);

         REQUIRE(moved == map);
         REQUIRE(moved != movable);
         REQUIRE(moved.GetRawKeysMemory() == keyMemory);
         REQUIRE(moved.GetRawValsMemory() == valueMemory);
         REQUIRE(moved.GetCount() == 5);
         REQUIRE(moved.GetKeys().GetUses() == 2);
         REQUIRE(moved.GetVals().GetUses() == 2);
         for (auto& comparer : darray1)
            REQUIRE(moved[comparer.GetKey()] == comparer.GetValue());
      }

      WHEN("Maps are compared") {
         T sameMap;
         sameMap << darray1[0] << darray1[1] << darray1[2] << darray1[3] << darray1[4];
         T clonedMap {Clone(map)};
         T copiedMap {map};
         T differentMap1;
         differentMap1 << darray1[0] << darray1[0] << darray1[2] << darray1[3] << darray1[4];

         REQUIRE(map == sameMap);
         REQUIRE(map == clonedMap);
         REQUIRE(map == copiedMap);
         REQUIRE(map != differentMap1);
      }

      WHEN("Maps are iterated with ranged-for") {
         for (auto& comparer : darray1)
            REQUIRE(map[comparer.GetKey()] == comparer.GetValue());

         unsigned i = 0;
         for (auto pair : map) {
            static_assert(not CT::Typed<T> or ::std::is_reference_v<decltype(pair.GetKey())>,
               "Pair key type is not a reference for statically optimized map");
            static_assert(not CT::Typed<T> or ::std::is_reference_v<decltype(pair.GetValue())>,
               "Pair value type is not a reference for statically optimized map");

            // Different architectures result in different hashes       
            if constexpr (Bitness == 32) {
               switch (i) {
               case 0:
                  REQUIRE(pair.GetKey() == darray1[2].GetKey());
                  REQUIRE(pair.GetValue() == darray1[2].GetValue());
                  break;
               case 1:
                  REQUIRE(pair.GetKey() == darray1[3].GetKey());
                  REQUIRE(pair.GetValue() == darray1[3].GetValue());
                  break;
               case 2:
                  REQUIRE(pair.GetKey() == darray1[1].GetKey());
                  REQUIRE(pair.GetValue() == darray1[1].GetValue());
                  break;
               case 3:
                  REQUIRE(pair.GetKey() == darray1[4].GetKey());
                  REQUIRE(pair.GetValue() == darray1[4].GetValue());
                  break;
               case 4:
                  REQUIRE(pair.GetKey() == darray1[0].GetKey());
                  REQUIRE(pair.GetValue() == darray1[0].GetValue());
                  break;
               default:
                  FAIL("Index out of bounds in ranged-for");
                  break;
               }
            }
            else if constexpr (Bitness == 64) {
               switch (i) {
               case 0:
                  REQUIRE(pair.GetKey() == darray1[1].GetKey());
                  REQUIRE(pair.GetValue() == darray1[1].GetValue());
                  break;
               case 1:
                  REQUIRE(pair.GetKey() == darray1[2].GetKey());
                  REQUIRE(pair.GetValue() == darray1[2].GetValue());
                  break;
               case 2:
                  REQUIRE(pair.GetKey() == darray1[3].GetKey());
                  REQUIRE(pair.GetValue() == darray1[3].GetValue());
                  break;
               case 3:
                  REQUIRE(pair.GetKey() == darray1[4].GetKey());
                  REQUIRE(pair.GetValue() == darray1[4].GetValue());
                  break;
               case 4:
                  REQUIRE(pair.GetKey() == darray1[0].GetKey());
                  REQUIRE(pair.GetValue() == darray1[0].GetValue());
                  break;
               default:
                  FAIL("Index out of bounds in ranged-for");
                  break;
               }
            }
            else break;

            ++i;
         }

         REQUIRE(i == map.GetCount());
      }

      WHEN("ForEach flat dense key (immutable)") {
         for (auto& comparer : darray1)
            REQUIRE(map[comparer.GetKey()] == comparer.GetValue());

         unsigned i = 0;
         const auto done = map.ForEachKey([&](const K& key) {
            // Different architectures result in different hashes       
            if constexpr (Bitness == 32) {
               switch (i) {
               case 0:
                  REQUIRE(key == darray1[2].GetKey());
                  break;
               case 1:
                  REQUIRE(key == darray1[3].GetKey());
                  break;
               case 2:
                  REQUIRE(key == darray1[1].GetKey());
                  break;
               case 3:
                  REQUIRE(key == darray1[4].GetKey());
                  break;
               case 4:
                  REQUIRE(key == darray1[0].GetKey());
                  break;
               default:
                  FAIL("Index out of bounds in ranged-for");
                  break;
               }
            }
            else if constexpr (Bitness == 64) {
               switch (i) {
               case 0:
                  REQUIRE(key == darray1[1].GetKey());
                  break;
               case 1:
                  REQUIRE(key == darray1[2].GetKey());
                  break;
               case 2:
                  REQUIRE(key == darray1[3].GetKey());
                  break;
               case 3:
                  REQUIRE(key == darray1[4].GetKey());
                  break;
               case 4:
                  REQUIRE(key == darray1[0].GetKey());
                  break;
               default:
                  FAIL("Index out of bounds in ranged-for");
                  break;
               }
            }
            else return false;

            ++i;
            return true;
         });

         REQUIRE(i == map.GetCount());
         REQUIRE(i == done);
      }
   }
   
   DestroyPair(pair);
   DestroyPair(stdpair);

   for (auto& i : darray1)
      DestroyPair(i);
   for (auto& i : darray2)
      DestroyPair(i);

   for (auto& i : darray1std)
      DestroyPair(i);
   for (auto& i : darray2std)
      DestroyPair(i);

   REQUIRE(memoryState.Assert());

   // Destroy BANK before static data - otherwise problems happen if    
   // not using managed reflection                                      
   BANK.Reset();

   REQUIRE_FALSE(Allocator::CollectGarbage());
}

TEMPLATE_TEST_CASE("Dense templated map stress test", "[map]",
   (MapTest<TUnorderedMap<int, int>, int, int>),
   (MapTest<TUnorderedMap<int, Trait>, int, Trait>),
   (MapTest<TUnorderedMap<int, Traits::Count>, int, Traits::Count>),
   (MapTest<TUnorderedMap<int, Many>, int, Many>),

   (MapTest<TOrderedMap<int, int>, int, int>),
   (MapTest<TOrderedMap<int, Trait>, int, Trait>),
   (MapTest<TOrderedMap<int, Traits::Count>, int, Traits::Count>),
   (MapTest<TOrderedMap<int, Many>, int, Many>)
) {
   static Allocator::State memoryState;

   using T = typename TestType::Container;
   //using K = typename TestType::Key;
   using V = typename TestType::Value;

   const V darray[5] {
      CreateElement<V>(111),
      CreateElement<V>(222),
      CreateElement<V>(333),
      CreateElement<V>(444),
      CreateElement<V>(555)
   };

   GIVEN("Map with some items") {
      T map {};

      // Insert 5,000,000 elements at random places                     
      // Tested with up to that many, but takes a lot of time, so i've  
      // lowered the number                                             
      for (int i = 0; i < 2'000; ++i) {
         for (auto& item : darray)
            map.Insert(i, item);
      }

      WHEN("Iterated") {
         Count iterated = 0;
         for (auto pair : map) {
            (void) pair;
            ++iterated;
         }

         REQUIRE(iterated == 2'000);
      }
   }

   for (auto& i : darray)
      DestroyElement(i);

   REQUIRE(memoryState.Assert());

   // Destroy BANK before static data - otherwise problems happen if    
   // not using managed reflection                                      
   BANK.Reset();

   REQUIRE_FALSE(Allocator::CollectGarbage());
}