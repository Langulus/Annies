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

   (MapTest<TMapUnsorted <Text, int>,         Text, int>),
   (MapTest<TMapUnsorted <Text, Tag>,         Text, Tag>),
   (MapTest<TMapUnsorted <Text, Tags::Count>, Text, Tags::Count>),
   (MapTest<TMapUnsorted <Text, Many>,        Text, Many>),
                         
   (MapTest<TMapSorted   <Text, int>,         Text, int>),
   (MapTest<TMapSorted   <Text, Tag>,         Text, Tag>),
   (MapTest<TMapSorted   <Text, Tags::Count>, Text, Tags::Count>),
   (MapTest<TMapSorted   <Text, Many>,        Text, Many>),

   (MapTest<MapUnsorted, Text, int>),
   (MapTest<MapUnsorted, Text, Tag>),
   (MapTest<MapUnsorted, Text, Tags::Count>),
   (MapTest<MapUnsorted, Text, Many>),

   (MapTest<MapSorted,   Text, int>),
   (MapTest<MapSorted,   Text, Tag>),
   (MapTest<MapSorted,   Text, Tags::Count>),
   (MapTest<MapSorted,   Text, Many>)
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
      static_assert(CT::CopyConstructible<T>);
      static_assert(CT::ReferConstructible<T>);
      static_assert(CT::AbandonConstructible<T>);
      static_assert(CT::MoveConstructible<T>);
      static_assert(CT::CloneConstructible<T>);
      static_assert(CT::DisownConstructible<T>);

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
      
      WHEN("Maps are iterated with ranged-for") {
         for (auto& comparer : darray1)
            REQUIRE(map[comparer.GetKey()] == comparer.GetValue());

         uint i = 0;
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

         uint i = 0;
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
}

TEMPLATE_TEST_CASE("Dense templated map stress test", "[map]",
   (MapTest<TMapUnsorted<int, int>, int, int>),
   (MapTest<TMapUnsorted<int, Tag>, int, Tag>),
   (MapTest<TMapUnsorted<int, Tags::Count>, int, Tags::Count>),
   (MapTest<TMapUnsorted<int, Many>, int, Many>),

   (MapTest<TMapSorted<int, int>, int, int>),
   (MapTest<TMapSorted<int, Tag>, int, Tag>),
   (MapTest<TMapSorted<int, Tags::Count>, int, Tags::Count>),
   (MapTest<TMapSorted<int, Many>, int, Many>)
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
         size_t iterated = 0;
         for (auto pair : map) {
            (void) pair;
            ++iterated;
         }

         REQUIRE(iterated == 2'000);
      }
   }
}