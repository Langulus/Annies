///                                                                           
/// Langulus::Annies                                                           
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "TestSetCommon.hpp"


#define SET_TESTS(MANAGED) \
   (SetTest<TUnorderedSet<Text*>, Text*, MANAGED>), \
   (SetTest<TUnorderedSet<int*>, int*, MANAGED>), \
   (SetTest<TUnorderedSet<Trait*>, Trait*, MANAGED>), \
   (SetTest<TUnorderedSet<Traits::Count*>, Traits::Count*, MANAGED>), \
   (SetTest<TUnorderedSet<Many*>, Many*, MANAGED>), \
   (SetTest<TUnorderedSet<RT*>, RT*, MANAGED>), \
 \
   (SetTest<TOrderedSet<Text*>, Text*, MANAGED>), \
   (SetTest<TOrderedSet<int*>, int*, MANAGED>), \
   (SetTest<TOrderedSet<Trait*>, Trait*, MANAGED>), \
   (SetTest<TOrderedSet<Traits::Count*>, Traits::Count*, MANAGED>), \
   (SetTest<TOrderedSet<Many*>, Many*, MANAGED>), \
   (SetTest<TOrderedSet<RT*>, RT*, MANAGED>), \
 \
   (SetTest<UnorderedSet, Text*, MANAGED>), \
   (SetTest<UnorderedSet, int*, MANAGED>), \
   (SetTest<UnorderedSet, Trait*, MANAGED>), \
   (SetTest<UnorderedSet, Traits::Count*, MANAGED>), \
   (SetTest<UnorderedSet, Many*, MANAGED>), \
   (SetTest<UnorderedSet, RT*, MANAGED>), \
 \
   (SetTest<OrderedSet, Text*, MANAGED>), \
   (SetTest<OrderedSet, int*, MANAGED>), \
   (SetTest<OrderedSet, Trait*, MANAGED>), \
   (SetTest<OrderedSet, Traits::Count*, MANAGED>), \
   (SetTest<OrderedSet, Many*, MANAGED>), \
   (SetTest<OrderedSet, RT*, MANAGED>)


/// The main test for TOrderedSet/TUnorderedSet/OrderedSet/UnorderedSet       
/// containers, with all kinds of sparse items - from trivial to complex,     
/// from flat to deep                                                         
#if LANGULUS_FEATURE(MANAGED_MEMORY)
TEMPLATE_TEST_CASE(
   "Sparse TOrderedSet/TUnorderedSet/OrderedSet/UnorderedSet", "[set]",
   //TODO SET_TESTS(true),
   SET_TESTS(false)
) {
#else
TEMPLATE_TEST_CASE(
   "Sparse TOrderedSet/TUnorderedSet/OrderedSet/UnorderedSet", "[set]",
   SET_TESTS(false)
) {
#endif
   static Allocator::State memoryState;

   using T = typename TestType::Container;
   using K = typename TestType::Key;

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

   GIVEN("Set with some items") {
      T set {};
      set << darray1[0];
      set << darray1[1];
      set << darray1[2];
      set << darray1[3];
      set << darray1[4];

      auto memory = set.GetRawMemory();

      WHEN("Given a preinitialized set with 5 elements") {
         Set_CheckState_OwnedFull<K>(set);

         REQUIRE(set.GetCount() == 5);
         REQUIRE(set.GetUses() == 1);
         for (auto& comparer : darray1)
            REQUIRE(set.Contains(comparer));
         REQUIRE(set.GetReserved() >= 5);
      }

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

         REQUIRE(set.GetUses() == 1);
         REQUIRE(set.GetCount() == 10);
         for (auto& comparer : darray1)
            REQUIRE(set.Contains(comparer));
         for (auto& comparer : darray2)
            REQUIRE(set.Contains(comparer));

         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            REQUIRE(set.GetRawMemory() == memory);
         #endif

         REQUIRE(set.GetReserved() >= 10);

         //TODO benchmark
      }

      WHEN("Move more of the same stuff") {
         K movableDarray2[5] {
            darray2[0],
            darray2[1],
            darray2[2],
            darray2[3],
            darray2[4]
         };

         set
            << ::std::move(movableDarray2[0])
            << ::std::move(movableDarray2[1])
            << ::std::move(movableDarray2[2])
            << ::std::move(movableDarray2[3])
            << ::std::move(movableDarray2[4]);

         Set_CheckState_OwnedFull<K>(set);

         REQUIRE(set.GetUses() == 1);
         REQUIRE(set.GetCount() == 10);

         for (auto& comparer : darray1)
            REQUIRE(set.Contains(comparer));
         for (auto& comparer : darray2)
            REQUIRE(set.Contains(comparer));

         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            REQUIRE(set.GetRawMemory() == memory);
         #endif

         REQUIRE(set.GetReserved() >= 10);

         //TODO benchmark
      }

      for (int iii = 0; iii < 10; ++iii) {
      WHEN(std::string("Removing elements by value #") + std::to_string(iii)) {
         static_assert(CT::Owned<Own<Trait*>>);
         static_assert(CT::Owned<Ref<Trait>>);
         static_assert(CT::NotOwned<Trait*>);
         static_assert(CT::NotOwned<Trait>);
         static_assert(CT::Comparable<Trait*, Own<Trait*>>);
         static_assert(CT::Comparable<Trait*, Ref<Trait>>);

         const auto removed2 = set.Remove(darray1[1]);
         const auto removed4 = set.Remove(darray1[3]);

         Set_CheckState_OwnedFull<K>(set);

         REQUIRE(set.GetUses() == 1);
         REQUIRE(removed2 == 1);
         REQUIRE(removed4 == 1);
         REQUIRE(set.GetCount() == 3);
         REQUIRE(set.GetRawMemory() == memory);
         REQUIRE(set.GetReserved() >= 5);

         REQUIRE      (set.Contains(darray1[0]));
         REQUIRE_FALSE(set.Contains(darray1[1]));
         REQUIRE      (set.Contains(darray1[2]));
         REQUIRE_FALSE(set.Contains(darray1[3]));
         REQUIRE      (set.Contains(darray1[4]));

         const auto removed3 = set.Remove(darray1[2]);
         REQUIRE(removed3 == 1);
         REQUIRE(set.GetCount() == 2);

         REQUIRE      (set.Contains(darray1[0]));
         REQUIRE_FALSE(set.Contains(darray1[1]));
         REQUIRE_FALSE(set.Contains(darray1[2]));
         REQUIRE_FALSE(set.Contains(darray1[3]));
         REQUIRE      (set.Contains(darray1[4]));

         const auto removed1 = set.Remove(darray1[0]);
         REQUIRE(removed1 == 1);
         REQUIRE(set.GetCount() == 1);

         REQUIRE_FALSE(set.Contains(darray1[0]));
         REQUIRE_FALSE(set.Contains(darray1[1]));
         REQUIRE_FALSE(set.Contains(darray1[2]));
         REQUIRE_FALSE(set.Contains(darray1[3]));
         REQUIRE      (set.Contains(darray1[4]));

         const auto removed5 = set.Remove(darray1[4]);
         REQUIRE(removed5 == 1);
         REQUIRE(set.GetCount() == 0);

         REQUIRE_FALSE(set.Contains(darray1[0]));
         REQUIRE_FALSE(set.Contains(darray1[1]));
         REQUIRE_FALSE(set.Contains(darray1[2]));
         REQUIRE_FALSE(set.Contains(darray1[3]));
         REQUIRE_FALSE(set.Contains(darray1[4]));

         //TODO benchmark
      }
      }

      for (int iii = 0; iii < 10; ++iii) {
      WHEN(std::string("Removing elements by key #") + std::to_string(iii)) {
         const auto removed2 = set.Remove(darray1[1]);
         const auto removed4 = set.Remove(darray1[3]);

         Set_CheckState_OwnedFull<K>(set);

         REQUIRE(set.GetUses() == 1);
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

         //TODO benchmark
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
         REQUIRE(set.GetUses() == 1);
         REQUIRE(set.GetReserved() >= 5);

         REQUIRE(set.Contains(darray1[0]));
         REQUIRE(set.Contains(darray1[1]));
         REQUIRE(set.Contains(darray1[2]));
         REQUIRE(set.Contains(darray1[3]));
         REQUIRE(set.Contains(darray1[4]));
      }
      
      WHEN("Sets are iterated with ranged-for") {
         uint i = 0;
         for (auto& item : set) {
            // Pointers are always random, can't ensure order           
            (void) item;
            ++i;
         }

         REQUIRE(i == set.GetCount());
      }

      WHEN("ForEach flat dense key (immutable)") {
         uint i = 0;
         const auto done = set.ForEach([&](const K& key) {
            // Pointers are always random, can't ensure order           
            (void) key;
            ++i;
            return true;
         });

         REQUIRE(i == set.GetCount());
         REQUIRE(i == done);
      }
   }
}
