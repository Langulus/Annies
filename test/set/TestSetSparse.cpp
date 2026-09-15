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
   //constexpr bool MANAGED = TestType::Managed;
   
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

   GIVEN("A default-initialized set instance") {
      T set {};

      WHEN("Given a default-constructed set") {
         Set_CheckState_Default<K>(set);

         //TODO benchmark
      }

      WHEN("Assigned an element by move") {
         auto movable = element;
         set = ::std::move(movable);

         Set_CheckState_OwnedFull<K>(set);

         if constexpr (not ::std::is_trivial_v<K>)
            REQUIRE(movable != element);
         REQUIRE(set.GetCount() == 1);
         REQUIRE(set.GetUses() == 1);
         REQUIRE(set.Contains(element));
         REQUIRE_FALSE(set.Contains("missing"));
         
         //TODO benchmark
      }
   }
   
   GIVEN("An element copy-initialized set instance") {
      T set {element};

      WHEN("Given an element-constructed set") {
         Set_CheckState_OwnedFull<K>(set);

         REQUIRE(set.GetCount() == 1);
         REQUIRE(set.GetUses() == 1);
         REQUIRE(set.Contains(element));
         REQUIRE_FALSE(set.Contains("missing"));

         //TODO benchmark
      }
   }
   
   GIVEN("An element-array copy-initialized set instance") {
      T set {darray1};

      WHEN("Given a preinitialized set with 5 elements") {
         Set_CheckState_OwnedFull<K>(set);

         REQUIRE(set.GetCount() == 5);
         REQUIRE(set.GetUses() == 1);
         for (auto& comparer : darray1)
            REQUIRE(set.Contains(comparer));
         REQUIRE(set.GetReserved() >= 5);

         //TODO benchmark
      }
   }

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
      
      WHEN("More capacity is reserved") {
         set.Reserve(20);

         Set_CheckState_OwnedFull<K>(set);

         REQUIRE(set.GetUses() == 1);
         REQUIRE(set.GetCount() == 5);
         REQUIRE(set.GetReserved() >= 20);
      }

      WHEN("Less capacity is reserved") {
         set.Reserve(2);

         Set_CheckState_OwnedFull<K>(set);

         REQUIRE(set.GetUses() == 1);
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
         for (auto& comparer : darray1)
            REQUIRE(copy.Contains(comparer));
      }

      WHEN("Set is cloned") {
         if constexpr (CT::CloneMakable<K>) {
            T clone = Langulus::Clone(set);

            Set_CheckState_OwnedFull<K>(clone);
            Set_CheckState_OwnedFull<K>(set);

            REQUIRE(clone != set);
            REQUIRE(clone.GetUses() == 1);
            REQUIRE(clone.GetCount() == set.GetCount());
            REQUIRE(clone.GetCount() == 5);
            REQUIRE(clone.GetRawMemory() != set.GetRawMemory());

            for (auto& comparer : darray1) {
               REQUIRE(not clone.Contains(comparer));
            }
         }
         else if constexpr (CT::Untyped<T>) {
            T clone;
            REQUIRE_THROWS(new (&clone) T {Langulus::Clone(set)});
         }
      }

      WHEN("Set is move-constructed") {
         T movable = set;
         T moved = ::std::move(movable);

         Set_CheckState_OwnedFull<K>(moved);
         Set_CheckState_Default<K>(movable);

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
         T copiedSet {set};
         T differentSet1;
         differentSet1 << darray1[0] << darray1[0] << darray1[2] << darray1[3] << darray1[4];

         REQUIRE(set == sameSet);
         REQUIRE(set == copiedSet);
         REQUIRE(set != differentSet1);

         if constexpr (CT::CloneMakable<K>) {
            T clonedSet {Clone(set)};
            REQUIRE(set != clonedSet);
         }
      }

      WHEN("Sets are iterated with ranged-for") {
         unsigned i = 0;
         for (auto& item : set) {
            // Pointers are always random, can't ensure order           
            (void) item;
            ++i;
         }

         REQUIRE(i == set.GetCount());
      }

      WHEN("ForEach flat dense key (immutable)") {
         unsigned i = 0;
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
