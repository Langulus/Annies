///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "TestSetCommon.hpp"


/// The main test for TOrderedSet/TUnorderedSet/OrderedSet/UnorderedSet       
/// containers, with all kinds of dense items - from trivial to complex,      
/// from flat to deep                                                         
TEMPLATE_TEST_CASE(
   "Dense TOrderedSet/TUnorderedSet/OrderedSet/UnorderedSet", "[set]",
   //(SetTest<TUnorderedSet<Text>, Text>),
   (SetTest<TUnorderedSet<int>, int>),
   (SetTest<TUnorderedSet<Trait>, Trait>),
   (SetTest<TUnorderedSet<Traits::Count>, Traits::Count>),
   (SetTest<TUnorderedSet<Many>, Many>),

   //(SetTest<TOrderedSet<Text>, Text>),
   (SetTest<TOrderedSet<int>, int>),
   (SetTest<TOrderedSet<Trait>, Trait>),
   (SetTest<TOrderedSet<Traits::Count>, Traits::Count>),
   (SetTest<TOrderedSet<Many>, Many>),

   //(SetTest<UnorderedSet, Text>),
   (SetTest<UnorderedSet, int>),
   (SetTest<UnorderedSet, Trait>),
   (SetTest<UnorderedSet, Traits::Count>),
   (SetTest<UnorderedSet, Many>),

   //(SetTest<OrderedSet, Text>),
   (SetTest<OrderedSet, int>),
   (SetTest<OrderedSet, Trait>),
   (SetTest<OrderedSet, Traits::Count>),
   (SetTest<OrderedSet, Many>)
) {
   static Allocator::State memoryState;

   using T = typename TestType::Container;
   using K = typename TestType::Key;

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

   K element = CreateElement<K>(555);

   const K darray1[5] {
      CreateElement<K>(1),
      CreateElement<K>(2),
      CreateElement<K>(3),
      CreateElement<K>(4),
      CreateElement<K>(5)
   };
   const K darray2[5] {
      CreateElement<K>(6),
      CreateElement<K>(7),
      CreateElement<K>(8),
      CreateElement<K>(9),
      CreateElement<K>(10)
   };
   

   GIVEN("A default-initialized set instance") {
      T set {};

      WHEN("Given a default-constructed set") {
         Set_CheckState_Default<K>(set);

         #ifdef LANGULUS_STD_BENCHMARK
            BENCHMARK_ADVANCED("Annies::set::default construction") (timer meter) {
               some<uninitialized<SetType>> storage(meter.runs());
               meter.measure([&](int i) {
                  return storage[i].construct();
               });
            };

            BENCHMARK_ADVANCED("std::set::default construction") (timer meter) {
               some<uninitialized<SetTypeStd>> storage(meter.runs());
               meter.measure([&](int i) {
                  return storage[i].construct();
               });
            };
         #endif
      }

      WHEN("Assigned a value by move") {
         auto movable = element;
         set = ::std::move(movable);

         Set_CheckState_OwnedFull<K>(set);

         if constexpr (not ::std::is_trivial_v<K>)
            REQUIRE(movable != element);
         REQUIRE(set.GetCount() == 1);
         REQUIRE(set.Contains(element));
         REQUIRE_FALSE(set.Contains("missing"));

         #ifdef LANGULUS_STD_BENCHMARK
            BENCHMARK_ADVANCED("Annies::TUnorderedSet::operator = (single pair copy)") (timer meter) {
               some<E> source(meter.runs());
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
   
   GIVEN("An element copy-initialized set instance") {
      T set {element};

      Set_CheckState_OwnedFull<K>(set);

      REQUIRE(set.GetCount() == 1);
      REQUIRE(set.Contains(element));
      REQUIRE_FALSE(set.Contains("missing"));
   }
   
   GIVEN("An element array copy-initialized set instance") {
      T set {darray1};

      Set_CheckState_OwnedFull<K>(set);

      REQUIRE(set.GetCount() == 5);
      for (auto& comparer : darray1)
         REQUIRE(set.Contains(comparer));
      REQUIRE(set.GetReserved() >= 5);
   }

   GIVEN("Set with some items") {
      T set {};
      set << darray1[0];
      set << darray1[1];
      set << darray1[2];
      set << darray1[3];
      set << darray1[4];

      auto memory = set.GetRawMemory();

      Set_CheckState_OwnedFull<K>(set);

      REQUIRE(set.GetCount() == 5);
      for (auto& comparer : darray1)
         REQUIRE(set.Contains(comparer));
      REQUIRE(set.GetReserved() >= 5);

      WHEN("Shallow-copy more of the same stuff") {
         for (auto& comparer : darray1)
            REQUIRE(set.Contains(comparer));

         set << darray2[0];
         for (auto& comparer : darray1)
            REQUIRE(set.Contains(comparer));

         set << darray2[1];
         for (auto& comparer : darray1)
            REQUIRE(set.Contains(comparer));

         set << darray2[2];
         for (auto& comparer : darray1)
            REQUIRE(set.Contains(comparer));

         set << darray2[3];
         for (auto& comparer : darray1)
            REQUIRE(set.Contains(comparer));

         set << darray2[4];
         for (auto& comparer : darray1)
            REQUIRE(set.Contains(comparer));

         Set_CheckState_OwnedFull<K>(set);

         REQUIRE(set.GetCount() == 10);

         for (auto& comparer : darray1)
            REQUIRE(set.Contains(comparer));
         for (auto& comparer : darray2)
            REQUIRE(set.Contains(comparer));

         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            REQUIRE(set.GetRawMemory() == memory);
         #endif

         REQUIRE(set.GetReserved() >= 10);

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
         K movableDarray2[5] {
            darray2[0],
            darray2[1],
            darray2[2],
            darray2[3],
            darray2[4]
         };

         set << ::std::move(movableDarray2[0])
             << ::std::move(movableDarray2[1])
             << ::std::move(movableDarray2[2])
             << ::std::move(movableDarray2[3])
             << ::std::move(movableDarray2[4]);

         Set_CheckState_OwnedFull<K>(set);

         REQUIRE(set.GetCount() == 10);

         for (auto& comparer : darray1)
            REQUIRE(set.Contains(comparer));

         for (auto& comparer : darray2)
            REQUIRE(set.Contains(comparer));

         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            REQUIRE(set.GetRawMemory() == memory);
         #endif

         REQUIRE(set.GetReserved() >= 10);

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
         const auto removed2 = set.Remove(darray1[1]);
         const auto removed4 = set.Remove(darray1[3]);

         Set_CheckState_OwnedFull<K>(set);

         REQUIRE(removed2 == 1);
         REQUIRE(removed4 == 1);
         REQUIRE(set.GetCount() == 3);
         REQUIRE(set.GetRawMemory() == memory);
         REQUIRE(set.GetReserved() >= 5);

         REQUIRE(set.Contains(darray1[0]));
         REQUIRE_FALSE(set.Contains(darray1[1]));
         REQUIRE(set.Contains(darray1[2]));
         REQUIRE_FALSE(set.Contains(darray1[3]));
         REQUIRE(set.Contains(darray1[4]));

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
         const auto removed2 = set.Remove(darray1[1]);
         const auto removed4 = set.Remove(darray1[3]);

         Set_CheckState_OwnedFull<K>(set);

         REQUIRE(removed2 == 1);
         REQUIRE(removed4 == 1);
         REQUIRE(set.GetCount() == 3);
         REQUIRE(set.GetRawMemory() == memory);
         REQUIRE(set.GetReserved() >= 5);

         REQUIRE(set.Contains(darray1[0]));
         REQUIRE_FALSE(set.Contains(darray1[1]));
         REQUIRE(set.Contains(darray1[2]));
         REQUIRE_FALSE(set.Contains(darray1[3]));
         REQUIRE(set.Contains(darray1[4]));

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
         const auto removed9 = set.Remove(darray2[3]);

         Set_CheckState_OwnedFull<K>(set);

         REQUIRE(removed9 == 0);
         for (auto& comparer : darray1)
            REQUIRE(set.Contains(comparer));
         REQUIRE(set.GetCount() == 5);
         REQUIRE(set.GetRawMemory() == memory);
         REQUIRE(set.GetReserved() >= 5);

         REQUIRE(set.Contains(darray1[0]));
         REQUIRE(set.Contains(darray1[1]));
         REQUIRE(set.Contains(darray1[2]));
         REQUIRE(set.Contains(darray1[3]));
         REQUIRE(set.Contains(darray1[4]));
      }
      
      WHEN("Removing non-available elements by key") {
         const auto removed9 = set.Remove(darray2[3]);

         Set_CheckState_OwnedFull<K>(set);

         REQUIRE(removed9 == 0);
         for (auto& comparer : darray1)
            REQUIRE(set.Contains(comparer));
         REQUIRE(set.GetCount() == 5);
         REQUIRE(set.GetRawMemory() == memory);
         REQUIRE(set.GetReserved() >= 5);

         REQUIRE(set.Contains(darray1[0]));
         REQUIRE(set.Contains(darray1[1]));
         REQUIRE(set.Contains(darray1[2]));
         REQUIRE(set.Contains(darray1[3]));
         REQUIRE(set.Contains(darray1[4]));
      }
      
      WHEN("More capacity is reserved") {
         set.Reserve(20);

         Set_CheckState_OwnedFull<K>(set);

         REQUIRE(set.GetCount() == 5);
         REQUIRE(set.GetReserved() >= 20);
      }

      WHEN("Less capacity is reserved") {
         set.Reserve(2);

         Set_CheckState_OwnedFull<K>(set);

         REQUIRE(set.GetCount() == 5);
         REQUIRE(set.GetRawMemory() == memory);
         REQUIRE(set.GetReserved() >= 5);
      }

      WHEN("Set is cleared") {
         set.Clear();

         Set_CheckState_OwnedEmpty<K>(set);

         REQUIRE(set.GetRawMemory() == memory);
         REQUIRE(set.GetReserved() >= 5);
      }

      WHEN("Set is reset") {
         set.Reset();

         Set_CheckState_Default<K>(set);
      }

      WHEN("Set is shallow-copied") {
         auto copy = set;

         Set_CheckState_OwnedFull<K>(copy);
         Set_CheckState_OwnedFull<K>(set);

         REQUIRE(copy == set);
         REQUIRE(copy.GetUses() == 2);
         REQUIRE(copy.GetCount() == set.GetCount());
         REQUIRE(copy.GetCount() == 5);
         REQUIRE(copy.GetRawMemory() == set.GetRawMemory());

         for (auto& comparer : darray1) {
            REQUIRE(copy.Contains(comparer));
            REQUIRE(set.Contains(comparer));
         }
      }

      WHEN("Set is cloned") {
         T clone = Langulus::Clone(set);

         Set_CheckState_OwnedFull<K>(clone);
         Set_CheckState_OwnedFull<K>(set);

         REQUIRE(clone == set);
         REQUIRE(clone.GetUses() == 1);
         REQUIRE(clone.GetCount() == set.GetCount());
         REQUIRE(clone.GetCount() == 5);
         REQUIRE(clone.GetRawMemory() != set.GetRawMemory());

         for (auto& comparer : darray1) {
            REQUIRE(clone.Contains(comparer));
            REQUIRE(set.Contains(comparer));
         }
      }

      WHEN("Set is move-constructed") {
         T movable = set;
         T moved = ::std::move(movable);

         Set_CheckState_Default<K>(movable);
         Set_CheckState_OwnedFull<K>(moved);

         REQUIRE(moved == set);
         REQUIRE(moved != movable);
         REQUIRE(moved.GetRawMemory() == memory);
         REQUIRE(moved.GetCount() == 5);
         REQUIRE(moved.GetUses() == 2);

         for (auto& comparer : darray1) {
            REQUIRE(moved.Contains(comparer));
            REQUIRE_FALSE(movable.Contains(comparer));
         }
      }

      WHEN("Sets are compared") {
         T sameSet;
         sameSet << darray1[0] << darray1[1] << darray1[2] << darray1[3] << darray1[4];
         T clonedSet {Clone(set)};
         T copiedSet {set};
         T differentSet1;
         differentSet1 << darray1[0] << darray1[0] << darray1[2] << darray1[3] << darray1[4];

         REQUIRE(set == sameSet);
         REQUIRE(set == clonedSet);
         REQUIRE(set == copiedSet);
         REQUIRE(set != differentSet1);
      }

      WHEN("Sets are iterated with ranged-for") {
         unsigned i = 0;
         for (auto& item : set) {
            // Different architectures result in different hashes       
            /*if constexpr (Bitness == 32) {
               switch (i) {
               case 0:
                  REQUIRE(item == darray1[2]);
                  break;
               case 1:
                  REQUIRE(item == darray1[3]);
                  break;
               case 2:
                  REQUIRE(item == darray1[1]);
                  break;
               case 3:
                  REQUIRE(item == darray1[4]);
                  break;
               case 4:
                  REQUIRE(item == darray1[0]);
                  break;
               default:
                  FAIL("Index out of bounds in ranged-for");
                  break;
               }
            }
            else if constexpr (Bitness == 64) {
               switch (i) {
               case 0:
                  REQUIRE(item == darray1[1]);
                  break;
               case 1:
                  REQUIRE(item == darray1[2]);
                  break;
               case 2:
                  REQUIRE(item == darray1[3]);
                  break;
               case 3:
                  REQUIRE(item == darray1[4]);
                  break;
               case 4:
                  REQUIRE(item == darray1[0]);
                  break;
               default:
                  FAIL("Index out of bounds in ranged-for");
                  break;
               }
            }
            else break;*/
            (void) item;
            ++i;
         }

         REQUIRE(i == set.GetCount());
      }

      WHEN("ForEach flat dense (immutable)") {
         unsigned i = 0;
         const auto done = set.ForEach([&](const K& key) {
            // Different architectures result in different hashes       
            /*if constexpr (Bitness == 32) {
               switch (i) {
               case 0:
                  REQUIRE(key == darray1[2]);
                  break;
               case 1:
                  REQUIRE(key == darray1[3]);
                  break;
               case 2:
                  REQUIRE(key == darray1[1]);
                  break;
               case 3:
                  REQUIRE(key == darray1[4]);
                  break;
               case 4:
                  REQUIRE(key == darray1[0]);
                  break;
               default:
                  FAIL("Index out of bounds in ranged-for");
                  break;
               }
            }
            else if constexpr (Bitness == 64) {
               switch (i) {
               case 0:
                  REQUIRE(key == darray1[1]);
                  break;
               case 1:
                  REQUIRE(key == darray1[2]);
                  break;
               case 2:
                  REQUIRE(key == darray1[3]);
                  break;
               case 3:
                  REQUIRE(key == darray1[4]);
                  break;
               case 4:
                  REQUIRE(key == darray1[0]);
                  break;
               default:
                  FAIL("Index out of bounds in ranged-for");
                  break;
               }
            }
            else return false;*/
            (void) key;
            ++i;
            return true;
         });

         REQUIRE(i == set.GetCount());
         REQUIRE(i == done);
      }
   }

   DestroyElement(element);

   for (auto& i : darray1)
      DestroyElement(i);
   for (auto& i : darray2)
      DestroyElement(i);

   REQUIRE(memoryState.Assert());

   // Destroy BANK before static data - otherwise problems happen if    
   // not using managed reflection                                      
   BANK.Reset();

   REQUIRE_FALSE(Allocator::CollectGarbage());
}