///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "TestMapCommon.hpp"

#define MAP_TESTS(MANAGED) \
   (MapTest<TMapUnsorted<Text, Tag*>, Text, Tag*, MANAGED>), \
   (MapTest<UnorderedMap, Text, RT*, MANAGED>), \
   (MapTest<TMapUnsorted<Tag*, RT*>, Tag*, RT*, MANAGED>), \
   (MapTest<TMapUnsorted<Text, RT*>, Text, RT*, MANAGED>), \
 \
   (MapTest<UnorderedMap, Tag*, RT*, MANAGED>), \
   (MapTest<UnorderedMap, Text, int*, MANAGED>), \
 \
   (MapTest<TMapUnsorted<Text, int*>, Text, int*, MANAGED>), \
   (MapTest<TMapUnsorted<Text, Tags::Count*>, Text, Tags::Count*, MANAGED>), \
   (MapTest<TMapUnsorted<Text, Many*>, Text, Many*, MANAGED>), \
 \
   (MapTest<TMapUnsorted<Tags::Count*, RT*>, Tags::Count*, RT*, MANAGED>), \
   (MapTest<TMapUnsorted<Many*, RT*>, Many*, RT*, MANAGED>), \
   (MapTest<TMapUnsorted<RT*, RT*>, RT*, RT*, MANAGED>), \
 \
   (MapTest<TOrderedMap<Text, int*>, Text, int*, MANAGED>), \
   (MapTest<TOrderedMap<Text, Tag*>, Text, Tag*, MANAGED>), \
   (MapTest<TOrderedMap<Text, Tags::Count*>, Text, Tags::Count*, MANAGED>), \
   (MapTest<TOrderedMap<Text, Many*>, Text, Many*, MANAGED>), \
   (MapTest<TOrderedMap<Text, RT*>, Text, RT*, MANAGED>), \
 \
   (MapTest<TOrderedMap<Tag*, RT*>, Tag*, RT*, MANAGED>), \
   (MapTest<TOrderedMap<Tags::Count*, RT*>, Tags::Count*, RT*, MANAGED>), \
   (MapTest<TOrderedMap<Many*, RT*>, Many*, RT*, MANAGED>), \
   (MapTest<TOrderedMap<RT*, RT*>, RT*, RT*, MANAGED>), \
 \
   (MapTest<UnorderedMap, Text, Tag*, MANAGED>), \
   (MapTest<UnorderedMap, Text, Tags::Count*, MANAGED>), \
   (MapTest<UnorderedMap, Text, Many*, MANAGED>), \
 \
   (MapTest<UnorderedMap, Tags::Count*, RT*, MANAGED>), \
   (MapTest<UnorderedMap, Many*, RT*, MANAGED>), \
   (MapTest<UnorderedMap, RT*, RT*, MANAGED>), \
 \
   (MapTest<OrderedMap, Text, int*, MANAGED>), \
   (MapTest<OrderedMap, Text, Tag*, MANAGED>), \
   (MapTest<OrderedMap, Text, Tags::Count*, MANAGED>), \
   (MapTest<OrderedMap, Text, Many*, MANAGED>), \
   (MapTest<OrderedMap, Text, RT*, MANAGED>), \
 \
   (MapTest<OrderedMap, Tag*, RT*, MANAGED>), \
   (MapTest<OrderedMap, Tags::Count*, RT*, MANAGED>), \
   (MapTest<OrderedMap, Many*, RT*, MANAGED>), \
   (MapTest<OrderedMap, RT*, RT*, MANAGED>)


/// The main test for TOrderedMap/TMapUnsorted/OrderedMap/UnorderedMap       
/// containers, with all kinds of items, from sparse to dense, from trivial   
/// to complex, from flat to deep                                             
#if LANGULUS_FEATURE(MANAGED_MEMORY)
TEMPLATE_TEST_CASE(
   "Sparse TOrderedMap/TMapUnsorted/OrderedMap/UnorderedMap", "[map]",
   //TODO MAP_TESTS(true),
   MAP_TESTS(false)
) {
#else
TEMPLATE_TEST_CASE(
   "Sparse TOrderedMap/TMapUnsorted/OrderedMap/UnorderedMap", "[map]",
   MAP_TESTS(false)
) {
#endif
   static Allocator::State memoryState;

   using T = typename TestType::Container;
   using K = typename TestType::Key;
   using V = typename TestType::Value;
   using Pair = TPair<K, V>;
   using StdPair = ::std::pair<K, V>;
   constexpr bool MANAGED = TestType::Managed;

   const auto pair        = CreatePair<Pair,    K, V, MANAGED>("five hundred", 555);
   const auto pairMissing = CreatePair<Pair,    K, V, MANAGED>("missing",      554);
   const auto stdpair     = CreatePair<StdPair, K, V, MANAGED>("five hundred", 555);

   const Pair darray1[5] {
      CreatePair<Pair, K, V, MANAGED>("one", 1),
      CreatePair<Pair, K, V, MANAGED>("two", 2),
      CreatePair<Pair, K, V, MANAGED>("three", 3),
      CreatePair<Pair, K, V, MANAGED>("four", 4),
      CreatePair<Pair, K, V, MANAGED>("five", 5)
   };

   const Pair darray2[5] {
      CreatePair<Pair, K, V, MANAGED>("six", 6),
      CreatePair<Pair, K, V, MANAGED>("seven", 7),
      CreatePair<Pair, K, V, MANAGED>("eight", 8),
      CreatePair<Pair, K, V, MANAGED>("nine", 9),
      CreatePair<Pair, K, V, MANAGED>("ten", 10)
   };

   const StdPair darray1std[5] {
      CreatePair<StdPair, K, V, MANAGED>("one", 1),
      CreatePair<StdPair, K, V, MANAGED>("two", 2),
      CreatePair<StdPair, K, V, MANAGED>("three", 3),
      CreatePair<StdPair, K, V, MANAGED>("four", 4),
      CreatePair<StdPair, K, V, MANAGED>("five", 5)
   };

   const StdPair darray2std[5] {
      CreatePair<StdPair, K, V, MANAGED>("six", 6),
      CreatePair<StdPair, K, V, MANAGED>("seven", 7),
      CreatePair<StdPair, K, V, MANAGED>("eight", 8),
      CreatePair<StdPair, K, V, MANAGED>("nine", 9),
      CreatePair<StdPair, K, V, MANAGED>("ten", 10)
   };

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
         REQUIRE(map.GetKeys().GetUses() == 1);
         REQUIRE(map.GetVals().GetUses() == 1);
         REQUIRE(map[pair.GetKey()] == pair.GetVal());

         if constexpr (CT::Text<K>) {
            REQUIRE(map["five hundred"] == pair.GetVal());
            REQUIRE_THROWS(map["missing"] != pair.GetVal());
         }
         else {
            REQUIRE(map[pair.GetKey()] == pair.GetVal());
            REQUIRE_THROWS(map[pairMissing.GetKey()] != pair.GetVal());
         }

         #ifdef LANGULUS_STD_BENCHMARK
            BENCHMARK_ADVANCED("Annies::TMapUnsorted::operator = (single pair copy)") (timer meter) {
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

      WHEN("Given a pair-constructed map") {
         Map_CheckState_OwnedFull<K, V>(map);

         REQUIRE(map.GetCount() == 1);
         REQUIRE(map.GetKeys().GetUses() == 1);
         REQUIRE(map.GetVals().GetUses() == 1);
         REQUIRE(map[pair.GetKey()] == pair.GetVal());

         if constexpr (CT::Text<K>) {
            REQUIRE(map["five hundred"] == pair.GetVal());
            REQUIRE_THROWS(map["missing"] != pair.GetVal());
         }
         else {
            REQUIRE(map[pair.GetKey()] == pair.GetVal());
            REQUIRE_THROWS(map[pairMissing.GetKey()] != pair.GetVal());
         }

         //TODO benchmark
      }
   }
   
   GIVEN("A pair array copy-initialized map instance") {
      T map {darray1};

      WHEN("Given a preinitialized map with 5 elements") {
         Map_CheckState_OwnedFull<K, V>(map);

         REQUIRE(map.GetCount() == 5);
         REQUIRE(map.GetKeys().GetUses() == 1);
         REQUIRE(map.GetVals().GetUses() == 1);
         for (auto& comparer : darray1)
            REQUIRE(map[comparer.GetKey()] == comparer.GetVal());
         REQUIRE(map.GetReserved() >= 5);

         //TODO benchmark
      }
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

      WHEN("Given a preinitialized map with 5 elements") {
         Map_CheckState_OwnedFull<K, V>(map);

         REQUIRE(map.GetCount() == 5);
         REQUIRE(map.GetKeys().GetUses() == 1);
         REQUIRE(map.GetVals().GetUses() == 1);
         for (auto& comparer : darray1)
            REQUIRE(map[comparer.GetKey()] == comparer.GetVal());
         REQUIRE(map.GetReserved() >= 5);
      }

      WHEN("Shallow-copy more of the same stuff") {
         for (auto& comparer : darray1)
            REQUIRE(map[comparer.GetKey()] == comparer.GetVal());

         map << darray2[0];
         for (auto& comparer : darray1)
            REQUIRE(map[comparer.GetKey()] == comparer.GetVal());

         map << darray2[1];
         for (auto& comparer : darray1)
            REQUIRE(map[comparer.GetKey()] == comparer.GetVal());

         map << darray2[2];
         for (auto& comparer : darray1)
            REQUIRE(map[comparer.GetKey()] == comparer.GetVal());


         /*Logger::SpecialTab("Map before: ");
         for (auto p : map)
            Logger::Append(p.mKey->As<Text>(), ", ");*/

         map << darray2[3];

         /*Logger::SpecialTab("Map after: ");
         for (auto p : map)
            Logger::Append(p.mKey->As<Text>(), ", ");*/

         for (auto& comparer : darray1) {
            //Logger::Special("Searching for: ", comparer.mKey->As<Text>());
            REQUIRE(map[comparer.GetKey()] == comparer.GetVal());
         }

         map << darray2[4];
         for (auto& comparer : darray1)
            REQUIRE(map[comparer.GetKey()] == comparer.GetVal());

         Map_CheckState_OwnedFull<K, V>(map);

         REQUIRE(map.GetKeys().GetUses() == 1);
         REQUIRE(map.GetVals().GetUses() == 1);
         REQUIRE(map.GetCount() == 10);

         for (auto& comparer : darray1)
            REQUIRE(map[comparer.GetKey()] == comparer.GetVal());
         for (auto& comparer : darray2)
            REQUIRE(map[comparer.GetKey()] == comparer.GetVal());

         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            REQUIRE(map.GetRawKeysMemory() == keyMemory);
            REQUIRE(map.GetRawValsMemory() == valueMemory);
         #endif

         REQUIRE(map.GetReserved() >= 10);

         #ifdef LANGULUS_STD_BENCHMARK
            BENCHMARK_ADVANCED("Annies::TMapUnsorted::operator << (5 consecutive pair copies)") (timer meter) {
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
            BENCHMARK_ADVANCED("Annies::TMapUnsorted::operator [] (retrieval by key from a map with 10 pairs)") (timer meter) {
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

         map
            << ::std::move(movableDarray2[0])
            << ::std::move(movableDarray2[1])
            << ::std::move(movableDarray2[2])
            << ::std::move(movableDarray2[3])
            << ::std::move(movableDarray2[4]);

         Map_CheckState_OwnedFull<K, V>(map);

         REQUIRE(map.GetKeys().GetUses() == 1);
         REQUIRE(map.GetVals().GetUses() == 1);
         REQUIRE(map.GetCount() == 10);
         for (auto& comparer : darray1)
            REQUIRE(map[comparer.GetKey()] == comparer.GetVal());
         for (auto& comparer : darray2)
            REQUIRE(map[comparer.GetKey()] == comparer.GetVal());
         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            REQUIRE(map.GetRawKeysMemory() == keyMemory);
            REQUIRE(map.GetRawValsMemory() == valueMemory);
         #endif
         REQUIRE(map.GetReserved() >= 10);

         #ifdef LANGULUS_STD_BENCHMARK
            BENCHMARK_ADVANCED("Annies::TMapUnsorted::operator << (5 consecutive trivial moves)") (timer meter) {
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

      for (int iii = 0; iii < 10; ++iii) {
      WHEN(std::string("Removing elements by value #") + std::to_string(iii)) {
         static_assert(CT::Owned<Own<Tag*>>);
         static_assert(CT::Owned<Ref<Tag>>);
         static_assert(CT::NotOwned<Tag*>);
         static_assert(CT::NotOwned<Tag>);
         static_assert(CT::Comparable<Tag*, Own<Tag*>>);
         static_assert(CT::Comparable<Tag*, Ref<Tag>>);

         const auto removed2 = map.RemoveValue(darray1[1].GetVal());
         const auto removed4 = map.RemoveValue(darray1[3].GetVal());

         Map_CheckState_OwnedFull<K, V>(map);

         REQUIRE(map.GetKeys().GetUses() == 1);
         REQUIRE(map.GetVals().GetUses() == 1);
         REQUIRE(removed2 == 1);
         REQUIRE(removed4 == 1);
         REQUIRE(map.GetCount() == 3);
         REQUIRE(map.GetRawKeysMemory() == keyMemory);
         REQUIRE(map.GetRawValsMemory() == valueMemory);
         REQUIRE(map.GetReserved() >= 5);

         REQUIRE      (map.ContainsValue(darray1[0].GetVal()));
         REQUIRE_FALSE(map.ContainsValue(darray1[1].GetVal()));
         REQUIRE      (map.ContainsValue(darray1[2].GetVal()));
         REQUIRE_FALSE(map.ContainsValue(darray1[3].GetVal()));
         REQUIRE      (map.ContainsValue(darray1[4].GetVal()));

         REQUIRE      (map.ContainsKey(darray1[0].GetKey()));
         REQUIRE_FALSE(map.ContainsKey(darray1[1].GetKey()));
         REQUIRE      (map.ContainsKey(darray1[2].GetKey()));
         REQUIRE_FALSE(map.ContainsKey(darray1[3].GetKey()));
         REQUIRE      (map.ContainsKey(darray1[4].GetKey()));

         const auto removed3 = map.RemoveValue(darray1[2].GetVal());
         REQUIRE(removed3 == 1);
         REQUIRE(map.GetCount() == 2);

         REQUIRE      (map.ContainsValue(darray1[0].GetVal()));
         REQUIRE_FALSE(map.ContainsValue(darray1[1].GetVal()));
         REQUIRE_FALSE(map.ContainsValue(darray1[2].GetVal()));
         REQUIRE_FALSE(map.ContainsValue(darray1[3].GetVal()));
         REQUIRE      (map.ContainsValue(darray1[4].GetVal()));

         REQUIRE      (map.ContainsKey(darray1[0].GetKey()));
         REQUIRE_FALSE(map.ContainsKey(darray1[1].GetKey()));
         REQUIRE_FALSE(map.ContainsKey(darray1[2].GetKey()));
         REQUIRE_FALSE(map.ContainsKey(darray1[3].GetKey()));
         REQUIRE      (map.ContainsKey(darray1[4].GetKey()));

         const auto removed1 = map.RemoveValue(darray1[0].GetVal());
         REQUIRE(removed1 == 1);
         REQUIRE(map.GetCount() == 1);

         REQUIRE_FALSE(map.ContainsValue(darray1[0].GetVal()));
         REQUIRE_FALSE(map.ContainsValue(darray1[1].GetVal()));
         REQUIRE_FALSE(map.ContainsValue(darray1[2].GetVal()));
         REQUIRE_FALSE(map.ContainsValue(darray1[3].GetVal()));
         REQUIRE      (map.ContainsValue(darray1[4].GetVal()));

         REQUIRE_FALSE(map.ContainsKey(darray1[0].GetKey()));
         REQUIRE_FALSE(map.ContainsKey(darray1[1].GetKey()));
         REQUIRE_FALSE(map.ContainsKey(darray1[2].GetKey()));
         REQUIRE_FALSE(map.ContainsKey(darray1[3].GetKey()));
         REQUIRE      (map.ContainsKey(darray1[4].GetKey()));

         const auto removed5 = map.RemoveValue(darray1[4].GetVal());
         REQUIRE(removed5 == 1);
         REQUIRE(map.GetCount() == 0);

         REQUIRE_FALSE(map.ContainsValue(darray1[0].GetVal()));
         REQUIRE_FALSE(map.ContainsValue(darray1[1].GetVal()));
         REQUIRE_FALSE(map.ContainsValue(darray1[2].GetVal()));
         REQUIRE_FALSE(map.ContainsValue(darray1[3].GetVal()));
         REQUIRE_FALSE(map.ContainsValue(darray1[4].GetVal()));

         REQUIRE_FALSE(map.ContainsKey(darray1[0].GetKey()));
         REQUIRE_FALSE(map.ContainsKey(darray1[1].GetKey()));
         REQUIRE_FALSE(map.ContainsKey(darray1[2].GetKey()));
         REQUIRE_FALSE(map.ContainsKey(darray1[3].GetKey()));
         REQUIRE_FALSE(map.ContainsKey(darray1[4].GetKey()));


         #ifdef LANGULUS_STD_BENCHMARK
            BENCHMARK_ADVANCED("Annies::TMapUnsorted::RemoveValue") (timer meter) {
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
      }

      for (int iii = 0; iii < 10; ++iii) {
      WHEN(std::string("Removing elements by key #") + std::to_string(iii)) {
         const auto removed2 = map.RemoveKey(darray1[1].GetKey());
         const auto removed4 = map.RemoveKey(darray1[3].GetKey());

         Map_CheckState_OwnedFull<K, V>(map);

         REQUIRE(map.GetKeys().GetUses() == 1);
         REQUIRE(map.GetVals().GetUses() == 1);
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

         REQUIRE(map.ContainsValue(darray1[0].GetVal()));
         REQUIRE_FALSE(map.ContainsValue(darray1[1].GetVal()));
         REQUIRE(map.ContainsValue(darray1[2].GetVal()));
         REQUIRE_FALSE(map.ContainsValue(darray1[3].GetVal()));
         REQUIRE(map.ContainsValue(darray1[4].GetVal()));

         #ifdef LANGULUS_STD_BENCHMARK
            BENCHMARK_ADVANCED("Annies::TMapUnsorted::RemoveKey") (timer meter) {
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
         const auto removed9 = map.RemoveValue(darray2[3].GetVal());

         Map_CheckState_OwnedFull<K, V>(map);

         REQUIRE(removed9 == 0);
         for (auto& comparer : darray1)
            REQUIRE(map[comparer.GetKey()] == comparer.GetVal());
         REQUIRE(map.GetCount() == 5);
         REQUIRE(map.GetRawKeysMemory() == keyMemory);
         REQUIRE(map.GetRawValsMemory() == valueMemory);
         REQUIRE(map.GetReserved() >= 5);

         REQUIRE(map.ContainsKey(darray1[0].GetKey()));
         REQUIRE(map.ContainsKey(darray1[1].GetKey()));
         REQUIRE(map.ContainsKey(darray1[2].GetKey()));
         REQUIRE(map.ContainsKey(darray1[3].GetKey()));
         REQUIRE(map.ContainsKey(darray1[4].GetKey()));

         REQUIRE(map.ContainsValue(darray1[0].GetVal()));
         REQUIRE(map.ContainsValue(darray1[1].GetVal()));
         REQUIRE(map.ContainsValue(darray1[2].GetVal()));
         REQUIRE(map.ContainsValue(darray1[3].GetVal()));
         REQUIRE(map.ContainsValue(darray1[4].GetVal()));
      }
      
      WHEN("Removing non-available elements by key") {
         const auto removed9 = map.RemoveKey(darray2[3].GetKey());

         Map_CheckState_OwnedFull<K, V>(map);

         REQUIRE(removed9 == 0);
         for (auto& comparer : darray1)
            REQUIRE(map[comparer.GetKey()] == comparer.GetVal());
         REQUIRE(map.GetCount() == 5);
         REQUIRE(map.GetKeys().GetUses() == 1);
         REQUIRE(map.GetVals().GetUses() == 1);
         REQUIRE(map.GetReserved() >= 5);

         REQUIRE(map.ContainsKey(darray1[0].GetKey()));
         REQUIRE(map.ContainsKey(darray1[1].GetKey()));
         REQUIRE(map.ContainsKey(darray1[2].GetKey()));
         REQUIRE(map.ContainsKey(darray1[3].GetKey()));
         REQUIRE(map.ContainsKey(darray1[4].GetKey()));

         REQUIRE(map.ContainsValue(darray1[0].GetVal()));
         REQUIRE(map.ContainsValue(darray1[1].GetVal()));
         REQUIRE(map.ContainsValue(darray1[2].GetVal()));
         REQUIRE(map.ContainsValue(darray1[3].GetVal()));
         REQUIRE(map.ContainsValue(darray1[4].GetVal()));
      }
      
      WHEN("More capacity is reserved") {
         //TODO this causes the Leftover exception
         map.Reserve(20);

         Map_CheckState_OwnedFull<K, V>(map);

         REQUIRE(map.GetKeys().GetUses() == 1);
         REQUIRE(map.GetVals().GetUses() == 1);
         REQUIRE(map.GetCount() == 5);
         REQUIRE(map.GetReserved() >= 20);
      }

      WHEN("Less capacity is reserved") {
         map.Reserve(2);

         Map_CheckState_OwnedFull<K, V>(map);

         REQUIRE(map.GetKeys().GetUses() == 1);
         REQUIRE(map.GetVals().GetUses() == 1);
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
            REQUIRE(copy[comparer.GetKey()] == comparer.GetVal());

         if constexpr (CT::Typed<T>) {
            for (auto& comparer : darray1)
               REQUIRE(&map[comparer.GetKey()] == &copy[comparer.GetKey()]);
         }
      }

      WHEN("Map is cloned") {
         if constexpr (CT::CloneMakable<K> and CT::CloneMakable<V>) {
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
               if constexpr (CT::Sparse<K>) {
                  // Pointer changes, hence no longer findable          
                  REQUIRE_THROWS(clone[comparer.GetKey()]);
               }
               else {
                  REQUIRE(clone[comparer.GetKey()] != comparer.GetVal());
                  REQUIRE(map[comparer.GetKey()] != clone[comparer.GetKey()]);

                  if constexpr (CT::Typed<T>)
                     REQUIRE(&map[comparer.GetKey()] != &clone[comparer.GetKey()]);
                  else
                     REQUIRE(map[comparer.GetKey()].GetRaw() != clone[comparer.GetKey()].GetRaw());
               }

               REQUIRE(map[comparer.GetKey()] == comparer.GetVal());
            }
         }
         else if constexpr (CT::Untyped<T>) {
            T clone;
            REQUIRE_THROWS(new (&clone) T {Langulus::Clone(map)});
         }
      }

      WHEN("Map is move-constructed") {
         T movable = map;
         T moved = ::std::move(movable);

         Map_CheckState_OwnedFull<K, V>(moved);
         Map_CheckState_Default<K, V>(movable);

         REQUIRE(moved == map);
         REQUIRE(moved != movable);
         REQUIRE(moved.GetRawKeysMemory() == keyMemory);
         REQUIRE(moved.GetRawValsMemory() == valueMemory);
         REQUIRE(moved.GetCount() == 5);
         REQUIRE(moved.GetKeys().GetUses() == 2);
         REQUIRE(moved.GetVals().GetUses() == 2);
         for (auto& comparer : darray1)
            REQUIRE(moved[comparer.GetKey()] == comparer.GetVal());
      }

      WHEN("Maps are compared") {
         T sameMap;
         sameMap << darray1[0] << darray1[1] << darray1[2] << darray1[3] << darray1[4];
         T copiedMap {map};
         T differentMap1;
         differentMap1 << darray1[0] << darray1[0] << darray1[2] << darray1[3] << darray1[4];

         REQUIRE(map == sameMap);
         REQUIRE(map == copiedMap);
         REQUIRE(map != differentMap1);

         if constexpr (CT::CloneMakable<K> and CT::CloneMakable<V>) {
            T clonedMap {Clone(map)};
            REQUIRE(map != clonedMap);
         }
      }

      WHEN("Maps are iterated with ranged-for") {
         unsigned i = 0;
         for (auto pair : map) {
            static_assert(CT::Untyped<T> or CT::Reference<decltype(pair.GetKey())>,
               "Pair key type is not a reference for statically optimized map");
            static_assert(CT::Untyped<T> or CT::Reference<decltype(pair.GetVal())>,
               "Pair value type is not a reference for statically optimized map");

            // Pointers are always random, can't ensure order           
            (void) pair;
            ++i;
         }

         REQUIRE(i == map.GetCount());
      }

      WHEN("ForEach flat dense key (immutable)") {
         unsigned i = 0;
         const auto done = map.ForEachKey([&](const K& key) {
            // Pointers are always random, can't ensure order           
            (void) key;
            ++i;
            return true;
         });

         THEN("The comparisons should be adequate") {
            REQUIRE(i == map.GetCount());
            REQUIRE(i == done);
         }
      }
   }

   DestroyPair<MANAGED>(pair);
   DestroyPair<MANAGED>(pairMissing);
   DestroyPair<MANAGED>(stdpair);

   for (auto& i : darray1)
      DestroyPair<MANAGED>(i);
   for (auto& i : darray2)
      DestroyPair<MANAGED>(i);
   for (auto& i : darray1std)
      DestroyPair<MANAGED>(i);
   for (auto& i : darray2std)
      DestroyPair<MANAGED>(i);

   REQUIRE(memoryState.Assert());

   // Destroy BANK before static data - otherwise problems happen if    
   // not using managed reflection                                      
   BANK.Reset();

   REQUIRE_FALSE(Allocator::CollectGarbage());
}

TEMPLATE_TEST_CASE("Sparse templated map stress test", "[map]",
   (MapTest<TMapUnsorted<int*, Tag>, int*, Tag>),

   (MapTest<TMapUnsorted<int, int*>, int, int*>),
   (MapTest<TMapUnsorted<int, Tag*>, int, Tag*>),
   (MapTest<TMapUnsorted<int, Tags::Count*>, int, Tags::Count*>),
   (MapTest<TMapUnsorted<int, Many*>, int, Many*>),

   (MapTest<TOrderedMap<int, int*>, int, int*>),
   (MapTest<TOrderedMap<int, Tag*>, int, Tag*>),
   (MapTest<TOrderedMap<int, Tags::Count*>, int, Tags::Count*>),
   (MapTest<TOrderedMap<int, Many*>, int, Many*>),

   (MapTest<TMapUnsorted<int*, int>, int*, int>),
   (MapTest<TMapUnsorted<int*, Tags::Count>, int*, Tags::Count>),
   (MapTest<TMapUnsorted<int*, Many>, int*, Many>),

   (MapTest<TOrderedMap<int*, int>, int*, int>),
   (MapTest<TOrderedMap<int*, Tag>, int*, Tag>),
   (MapTest<TOrderedMap<int*, Tags::Count>, int*, Tags::Count>),
   (MapTest<TOrderedMap<int*, Many>, int*, Many>),

   (MapTest<TMapUnsorted<int*, int*>, int*, int*>),
   (MapTest<TMapUnsorted<int*, Tag*>, int*, Tag*>),
   (MapTest<TMapUnsorted<int*, Tags::Count*>, int*, Tags::Count*>),
   (MapTest<TMapUnsorted<int*, Many*>, int*, Many*>),

   (MapTest<TOrderedMap<int*, int*>, int*, int*>),
   (MapTest<TOrderedMap<int*, Tag*>, int*, Tag*>),
   (MapTest<TOrderedMap<int*, Tags::Count*>, int*, Tags::Count*>),
   (MapTest<TOrderedMap<int*, Many*>, int*, Many*>)
) {
   static Allocator::State memoryState;

   using T = typename TestType::Container;
   using K = typename TestType::Key;
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
      // lowered the number. 14'980 has shown historically to be        
      // associated with some bugs in the past, so it's a number of     
      // interest.                                                      
      for (int i = 0; i < 1'000; ++i) {
         for (auto& item : darray) {
            //if (i == 14'979)
            //   Logger::Fatal("Break to debug");

            map.Insert(CreateElement<K>(i), item);

            // Check integrity                                          
            size_t iterated = 0;
            for (auto pair : map) {
               (void) pair;
               ++iterated;
            }

            if (iterated != map.GetCount())
               Logger::Fatal("Map integrity check failure after inserting ", i);

            REQUIRE(iterated == map.GetCount());
         }
      }

      WHEN("Iterated") {
         size_t iterated = 0;
         for (auto pair : map) {
            (void) pair;
            ++iterated;
         }

         if constexpr (CT::Sparse<K>)
            REQUIRE(iterated == 1'000*5);
         else
            REQUIRE(iterated == 1'000);
      }

      for (auto i : map)
         DestroyElement(i.GetKey());
   }

   // Friendly note for a future Dimo: If you ever get memory manager   
   // integrity failures, its due to lack of these destruction calls    
   for (auto& i : darray)
      DestroyElement(i);

   REQUIRE(memoryState.Assert());

   // Destroy BANK before static data - otherwise problems happen if    
   // not using managed reflection                                      
   BANK.Reset();

   REQUIRE_FALSE(Allocator::CollectGarbage());
}