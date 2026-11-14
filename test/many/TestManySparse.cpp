///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "TestManyCommon.hpp"


/// The main test for Many/TMany containers, with all kinds of items, from    
/// sparse to dense, from trivial to complex, from flat to deep               
TEMPLATE_TEST_CASE("Sparse Many/TMany", "[many]",
   (TypePair<Tag, RT*>),

   (TypePair<TMany<Tag*>, Tag*>),
   (TypePair<Many, Tags::Count*>),
   (TypePair<Tag, Text*>),

   (TypePair<Tags::Name, Text*>),
   (TypePair<Tags::Name, RT*>),

   (TypePair<TMany<int*>, int*>),
   (TypePair<TMany<Tags::Count*>, Tags::Count*>),
   (TypePair<TMany<Many*>, Many*>),
   (TypePair<TMany<Text*>, Text*>),
   (TypePair<TMany<RT*>, RT*>),

   (TypePair<Many, int*>),
   (TypePair<Many, Tag*>),
   (TypePair<Many, Many*>),
   (TypePair<Many, Text*>),
   (TypePair<Many, RT*>)
) {
   static Allocator::State memoryState;

   using T = typename TestType::LHS;
   using E = typename TestType::RHS;
   using DenseE = Decay<E>;
      
   E element = CreateElement<E>(555);
   const DenseE& denseValue {DenseCast(element)};
   const DenseE* const sparseValue {SparseCast(element)};


   GIVEN("Default constructed container") {
      T pack;

      WHEN("Shallow-copy more of the same stuff to the back (<<)") {
         pack << darray2[0]
              << darray2[1]
              << darray2[2]
              << darray2[3]
              << darray2[4];

         Many_CheckState_OwnedFull<E>(pack);
         Many_CheckState_ContainsArray(pack, darray2);
      }

      WHEN("Shallow-copy more of the same stuff to the front (>>)") {
         pack >> darray2[4]
              >> darray2[3]
              >> darray2[2]
              >> darray2[1]
              >> darray2[0];

         Many_CheckState_OwnedFull<E>(pack);
         Many_CheckState_ContainsArray(pack, darray2);
      }

      WHEN("Shallow-copy an array to the back") {
         pack.InsertAt(Index::Back, darray2);

         Many_CheckState_OwnedFull<E>(pack);
         Many_CheckState_ContainsArray(pack, darray2);
      }

      WHEN("Shallow-copy an array to the front") {
         pack.InsertAt(Index::Front, darray2);

         Many_CheckState_OwnedFull<E>(pack);
         Many_CheckState_ContainsArray(pack, darray2);
      }

      WHEN("Move more of the same stuff to the back (<<)") {
         E darray3[5] {
            CreateElement<E>(6),
            CreateElement<E>(7),
            CreateElement<E>(8),
            CreateElement<E>(9),
            CreateElement<E>(10)
         };

         const E darray3backup[5] {
            darray3[0],
            darray3[1],
            darray3[2],
            darray3[3],
            darray3[4],
         };

         pack
            << ::std::move(darray3[0])
            << ::std::move(darray3[1])
            << ::std::move(darray3[2])
            << ::std::move(darray3[3])
            << ::std::move(darray3[4]);

         Many_CheckState_OwnedFull<E>(pack);
         Many_CheckState_ContainsArray(pack, darray3backup);
         
         for (auto i : darray3)
            DestroyElement(i);
      }

      WHEN("Move more of the same stuff to the front (>>)") {
         E darray3[5] {
            CreateElement<E>(6),
            CreateElement<E>(7),
            CreateElement<E>(8),
            CreateElement<E>(9),
            CreateElement<E>(10)
         };

         const E darray3backup[5] {
            darray3[0],
            darray3[1],
            darray3[2],
            darray3[3],
            darray3[4],
         };

         pack
            >> ::std::move(darray3[4])
            >> ::std::move(darray3[3])
            >> ::std::move(darray3[2])
            >> ::std::move(darray3[1])
            >> ::std::move(darray3[0]);

         Many_CheckState_OwnedFull<E>(pack);
         Many_CheckState_ContainsArray(pack, darray3backup);
         
         for (auto i : darray3)
            DestroyElement(i);
      }

      WHEN("Emplace item at the front") {
         auto i666 = CreateElement<E>(666);
         const auto i666backup = i666;

         if constexpr (CT::Typed<T>) {
            auto instance = pack.EmplaceAt(Index::Front, ::std::move(i666));

            Many_CheckState_OwnedFull<E>(pack);
            REQUIRE(pack.GetCount() == 1);
            REQUIRE(pack.GetReserved() >= 1);
            REQUIRE(pack[0] == i666backup);
            REQUIRE(pack[0] == instance);

         }
         else {
            REQUIRE_THROWS(pack.EmplaceAt(Index::Front, ::std::move(i666)));
            Many_CheckState_Default<E>(pack);
         }

         DestroyElement(i666);
      }
      
      WHEN("Emplace item at the back") {
         auto i666 = CreateElement<E>(666);
         const auto i666backup = i666;

         if constexpr (CT::Typed<T>) {
            auto instance = pack.EmplaceAt(Index::Back, ::std::move(i666));

            Many_CheckState_OwnedFull<E>(pack);
            REQUIRE(pack.GetCount() == 1);
            REQUIRE(pack.GetReserved() >= 1);
            REQUIRE(pack[0] == i666backup);
            REQUIRE(pack[0] == instance);

         }
         else {
            REQUIRE_THROWS(pack.EmplaceAt(Index::Back, ::std::move(i666)));
            Many_CheckState_Default<E>(pack);
         }

         DestroyElement(i666);
      }

      WHEN("Removing non-available elements") {
         const auto removed9 = pack.Remove(darray2[3]);

         REQUIRE(removed9 == 0);
         Many_CheckState_Default<E>(pack);
      }    

      WHEN("Empty pack with state is shallow-copied") {
         pack.EnableOr();
         auto copy = pack;

         Many_Helper_TestSame(copy, pack);
         REQUIRE(copy.GetState() == pack.GetState());
         REQUIRE(copy.GetUses() == 0);
      }

      if constexpr (CT::CloneConstructible<T>) {
         WHEN("Empty pack with state is cloned") {
            pack.EnableOr();
            T clone = Clone(pack);

            Many_Helper_TestSame(clone, pack);
            REQUIRE(clone.GetState() == pack.GetState());
            REQUIRE(clone.GetUses() == 0);
         }
      }

      WHEN("Empty pack with state is moved") {
         pack.EnableOr();
         T movable = pack;
         const T moved = ::std::move(movable);

         Many_CheckState_Default<E>(movable);
         Many_Helper_TestSame(moved, pack);
      }

      WHEN("A forward value-based search is performed on non-exitent value") {
         const auto found = pack.Find(darray2[2]);

         REQUIRE(found == Index::None);
         REQUIRE_FALSE(found);
      }

      WHEN("A backward value-based search is performed on non-exitent value") {
         const auto found = pack.template Find<true>(darray2[2]);

         REQUIRE(found == Index::None);
         REQUIRE_FALSE(found);
      }
      
      WHEN("Merge-copy an element to the back, if not found (<<=)") {
         pack <<= darray2[3];

         Many_CheckState_OwnedFull<E>(pack);
         REQUIRE(pack.GetCount() == 1);
         REQUIRE(pack.GetReserved() >= 1);
         REQUIRE(pack[0] == darray2[3]);
      }

      WHEN("Merge-copy an element to the front, if not found (>>=)") {
         pack >>= darray2[3];

         Many_CheckState_OwnedFull<E>(pack);
         REQUIRE(pack.GetCount() == 1);
         REQUIRE(pack.GetReserved() >= 1);
         REQUIRE(pack[0] == darray2[3]);
      }

      WHEN("Merge-move an element to the back, if not found (<<=)") {
         auto moved = darray2[3];
         pack <<= ::std::move(moved);

         Many_CheckState_OwnedFull<E>(pack);
         REQUIRE(pack.GetCount() == 1);
         REQUIRE(pack.GetReserved() >= 1);
         REQUIRE(pack[0] == darray2[3]);
      }

      WHEN("Merge-move an element to the front, if not found (>>=)") {
         auto moved = darray2[3];
         pack >>= ::std::move(moved);

         Many_CheckState_OwnedFull<E>(pack);
         REQUIRE(pack.GetCount() == 1);
         REQUIRE(pack.GetReserved() >= 1);
         REQUIRE(pack[0] == darray2[3]);
      }

      WHEN("ForEach flat dense element (immutable)") {
         const auto foreachit = const_cast<const T&>(pack).ForEach(
            [&](const int&)  {FAIL();},
            [&](const Tag&)  {FAIL();},
            [&](const Many&) {FAIL();}
         );

         REQUIRE(0 == foreachit);
      }

      WHEN("ForEach flat dense element (mutable)") {
         const auto foreachit = const_cast<T&>(pack).ForEach(
            [&](int&)       {FAIL(); },
            [&](Tag&)       {FAIL(); },
            [&](Many&)      {FAIL(); }
         );

         REQUIRE(0 == foreachit);
      }

      WHEN("ForEach flat sparse element (immutable)") {
         const auto foreachit = const_cast<const T&>(pack).ForEach(
            [&](const int*)  {FAIL(); },
            [&](const Tag*)  {FAIL(); },
            [&](const Many*) {FAIL(); }
         );

         REQUIRE(0 == foreachit);
      }

      WHEN("ForEach flat sparse element (mutable)") {
         const auto foreachit = const_cast<T&>(pack).ForEach(
            [&](int*)    {FAIL(); },
            [&](Tag*)    {FAIL(); },
            [&](Many*)   {FAIL(); }
         );

         REQUIRE(0 == foreachit);
      }

      WHEN("ForEachRev flat dense element (immutable)") {
         const auto foreachit = const_cast<const T&>(pack).ForEachRev(
            [&](const int&)  {FAIL(); },
            [&](const Tag&)  {FAIL(); },
            [&](const Many&) {FAIL(); }
         );

         REQUIRE(0 == foreachit);
      }

      WHEN("ForEachRev flat dense element (mutable)") {
         const auto foreachit = pack.ForEachRev(
            [&](const int&)  {FAIL(); },
            [&](const Tag&)  {FAIL(); },
            [&](const Many&) {FAIL(); }
         );

         REQUIRE(0 == foreachit);
      }

      WHEN("ForEachRev flat sparse element (immutable)") {
         const auto foreachit = const_cast<const T&>(pack).ForEachRev(
            [&](const int*)  {FAIL(); },
            [&](const Tag*)  {FAIL(); },
            [&](const Many*) {FAIL(); }
         );

         REQUIRE(0 == foreachit);
      }

      WHEN("ForEachRev flat sparse element (mutable)") {
         const auto foreachit = pack.ForEachRev(
            [&](const int*)  {FAIL(); },
            [&](const Tag*)  {FAIL(); },
            [&](const Many*) {FAIL(); }
         );

         REQUIRE(0 == foreachit);
      }
   }

   GIVEN("Container constructed by static list of exactly the same shallow-copied elements") {
      if constexpr (not CT::Typed<T>) {
         const T pack {element, element};

         Many_CheckState_OwnedFull<E>(pack);
         REQUIRE(pack.GetCount() == 2);
         REQUIRE(pack.GetReserved() >= 2);
         for (auto& e : pack) {
            REQUIRE(e == element);
         }
      }
   }

   GIVEN("Container constructed by static list of somewhat different shallow-copied elements") {
      if constexpr (not CT::Typed<T>) {
         const T pack {denseValue, sparseValue};

         Many_CheckState_OwnedFull<Many>(pack);
         REQUIRE(pack.GetCount() == 2);
         REQUIRE(pack.GetReserved() >= 2);
         REQUIRE(pack[0] == Many {denseValue});
         REQUIRE(pack[1] == Many {sparseValue});
      }
   }

   GIVEN("Container with some items") {
      T pack {};
      pack << darray1[0] << darray1[1] << darray1[2] << darray1[3] << darray1[4];
      const auto previousReserved = pack.GetReserved();
      const auto memory = pack.GetRaw();
      
      WHEN("Given a preinitialized container with 5 elements") {
         REQUIRE(pack.GetCount() == 5);
         REQUIRE(pack.GetReserved() >= 5);
         REQUIRE(pack.template IsExact<E>());
         REQUIRE(pack.GetRaw());
         for (uint i = 0; i < pack.GetCount(); ++i)
            REQUIRE(pack[i] == darray1[i]);
         REQUIRE_FALSE(pack.IsConstant());
      }

      WHEN("Shallow-copy more of the same stuff to the back (<<)") {
         pack << darray2[0] << darray2[1] << darray2[2] << darray2[3] << darray2[4];

         REQUIRE(pack.GetCount() == 10);
         REQUIRE(pack.GetReserved() >= 10);
         REQUIRE(pack.template IsExact<E>());

         for (uint i = 0; i < 5; ++i)
            REQUIRE(pack[i] == darray1[i]);
         for (uint i = 5; i < pack.GetCount(); ++i)
            REQUIRE(pack[i] == darray2[i-5]);

         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            if constexpr (CT::Same<E, int>) {
               REQUIRE(pack.GetRaw() == memory);
            }
         #endif
      }

      WHEN("Shallow-copy more of the same stuff to the front (>>)") {
         pack >> darray2[0] >> darray2[1] >> darray2[2] >> darray2[3] >> darray2[4];

         REQUIRE(pack.GetCount() == 10);
         REQUIRE(pack.GetReserved() >= 10);
         REQUIRE(pack.template IsExact<E>());

         for (unsigned i = 5; i > 0; --i)
            REQUIRE(pack[5 - i] == darray2[i - 1]);
         for (unsigned i = 5; i < pack.GetCount(); ++i)
            REQUIRE(pack[i] == darray1[i-5]);

         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            if constexpr (CT::Same<E, int>) {
               REQUIRE(pack.GetRaw() == memory);
            }
         #endif
      }

      WHEN("Shallow-copy an array to the back") {
         pack.InsertAt(Index::Back, darray2);

         REQUIRE(pack.GetCount() == 10);
         REQUIRE(pack.GetReserved() >= 10);
         REQUIRE(pack.template IsExact<E>());

         for (unsigned i = 0; i < 5; ++i)
            REQUIRE(pack[i] == darray1[i]);
         for (unsigned i = 5; i < pack.GetCount(); ++i)
            REQUIRE(pack[i] == darray2[i-5]);

         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            if constexpr (CT::Same<E, int>) {
               REQUIRE(pack.GetRaw() == memory);
            }
         #endif
      }

      WHEN("Shallow-copy an array to the front") {
         pack.InsertAt(Index::Front, darray2);

         REQUIRE(pack.GetCount() == 10);
         REQUIRE(pack.GetReserved() >= 10);
         REQUIRE(pack.template IsExact<E>());

         for (unsigned i = 0; i < 5; ++i)
            REQUIRE(pack[i] == darray2[i]);
         for (unsigned i = 5; i < pack.GetCount(); ++i)
            REQUIRE(pack[i] == darray1[i-5]);

         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            if constexpr (CT::Same<E, int>) {
               REQUIRE(pack.GetRaw() == memory);
            }
         #endif
      }

      WHEN("Move more of the same stuff to the back (<<)") {
         E darray3[5] {
            CreateElement<E>(6),
            CreateElement<E>(7),
            CreateElement<E>(8),
            CreateElement<E>(9),
            CreateElement<E>(10)
         };

         const E darray3backup[5] {
            darray3[0],
            darray3[1],
            darray3[2],
            darray3[3],
            darray3[4],
         };

         pack
            << ::std::move(darray3[0])
            << ::std::move(darray3[1])
            << ::std::move(darray3[2])
            << ::std::move(darray3[3])
            << ::std::move(darray3[4]);

         REQUIRE(pack.GetCount() == 10);
         REQUIRE(pack.GetReserved() >= 10);
         REQUIRE(pack.template IsExact<E>());

         for (unsigned i = 0; i < 5; ++i)
            REQUIRE(pack[i] == darray1[i]);

         for (unsigned i = 5; i < pack.GetCount(); ++i)
            REQUIRE(pack[i] == darray3backup[i - 5]);

         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            REQUIRE(pack.GetRaw() == memory);
         #endif

         for (auto i : darray3)
            DestroyElement(i);
      }

      WHEN("Move more of the same stuff to the front (>>)") {
         E darray3[5] {
            CreateElement<E>(6),
            CreateElement<E>(7),
            CreateElement<E>(8),
            CreateElement<E>(9),
            CreateElement<E>(10)
         };

         const E darray3backup[5] {
            darray3[0],
            darray3[1],
            darray3[2],
            darray3[3],
            darray3[4],
         };

         pack
            >> ::std::move(darray3[0])
            >> ::std::move(darray3[1])
            >> ::std::move(darray3[2])
            >> ::std::move(darray3[3])
            >> ::std::move(darray3[4]);

         REQUIRE(pack.GetCount() == 10);
         REQUIRE(pack.GetReserved() >= 10);
         REQUIRE(pack.template IsExact<E>());

         for (unsigned i = 5; i > 0; --i)
            REQUIRE(pack[5 - i] == darray3backup[i - 1]);

         for (unsigned i = 5; i < pack.GetCount(); ++i)
            REQUIRE(pack[i] == darray1[i - 5]);

         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            REQUIRE(pack.GetRaw() == memory);
         #endif

         for (auto i : darray3)
            DestroyElement(i);
      }
      
      WHEN("Insert single item at a specific place by shallow-copy") {
         const auto i666 = CreateElement<E>(666);
         pack.InsertAt(3, i666);

         REQUIRE(pack.GetCount() == 6);
         REQUIRE(pack.GetReserved() >= 6);
         REQUIRE(pack.template IsExact<E>());
         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            REQUIRE(pack.GetRaw() == memory);
         #endif
         REQUIRE(pack[0] == darray1[0]);
         REQUIRE(pack[1] == darray1[1]);
         REQUIRE(pack[2] == darray1[2]);
         REQUIRE(pack[3] == i666);
         REQUIRE(pack[4] == darray1[3]);
         REQUIRE(pack[5] == darray1[4]);

         DestroyElement(i666);
      }

      WHEN("Insert multiple items at a specific place by shallow-copy") {
         pack.InsertAt(3, darray2);

         REQUIRE(pack.GetCount() == 10);
         REQUIRE(pack.GetReserved() >= 10);
         REQUIRE(pack.template IsExact<E>());
         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            REQUIRE(pack.GetRaw() == memory);
         #endif
         REQUIRE(pack[0] == darray1[0]);
         REQUIRE(pack[1] == darray1[1]);
         REQUIRE(pack[2] == darray1[2]);
         REQUIRE(pack[3] == darray2[0]);
         REQUIRE(pack[4] == darray2[1]);
         REQUIRE(pack[5] == darray2[2]);
         REQUIRE(pack[6] == darray2[3]);
         REQUIRE(pack[7] == darray2[4]);
         REQUIRE(pack[8] == darray1[3]);
         REQUIRE(pack[9] == darray1[4]);
      }

      WHEN("Insert single item at a specific place by move") {
         auto i666 = CreateElement<E>(666);
         const auto i666backup = i666;
         pack.InsertAt(3, ::std::move(i666));

         REQUIRE(pack.GetCount() == 6);
         REQUIRE(pack.GetReserved() >= 6);
         REQUIRE(pack.template IsExact<E>());
         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            REQUIRE(pack.GetRaw() == memory);
         #endif
         REQUIRE(pack[0] == darray1[0]);
         REQUIRE(pack[1] == darray1[1]);
         REQUIRE(pack[2] == darray1[2]);
         REQUIRE(pack[3] == i666backup);
         REQUIRE(pack[4] == darray1[3]);
         REQUIRE(pack[5] == darray1[4]);

         DestroyElement(i666);
      }

      WHEN("Emplace item at a specific place") {
         auto i666 = CreateElement<E>(666);
         const auto i666backup = i666;
         auto instance = pack.EmplaceAt(3, ::std::move(i666));

         REQUIRE(pack.GetCount() == 6);
         REQUIRE(pack.GetReserved() >= 6);
         REQUIRE(pack.template IsExact<E>());
         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            REQUIRE(pack.GetRaw() == memory);
         #endif
         REQUIRE(pack[0] == darray1[0]);
         REQUIRE(pack[1] == darray1[1]);
         REQUIRE(pack[2] == darray1[2]);
         REQUIRE(pack[3] == i666backup);
         REQUIRE(pack[4] == darray1[3]);
         REQUIRE(pack[5] == darray1[4]);

         if constexpr (CT::Typed<T>)
            REQUIRE(pack[3] == instance);
         else {
            REQUIRE(pack[3].GetRaw() == instance.GetRaw());
            REQUIRE(pack[3].GetCount() == 1);
         }

         DestroyElement(i666);
      }

      WHEN("Emplace item at the front") {
         auto i666 = CreateElement<E>(666);
         const auto i666backup = i666;
         auto instance = pack.EmplaceAt(Index::Front, ::std::move(i666));

         REQUIRE(pack.GetCount() == 6);
         REQUIRE(pack.GetReserved() >= 6);
         REQUIRE(pack.template IsExact<E>());
         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            REQUIRE(pack.GetRaw() == memory);
         #endif
         REQUIRE(pack[0] == i666backup);
         REQUIRE(pack[1] == darray1[0]);
         REQUIRE(pack[2] == darray1[1]);
         REQUIRE(pack[3] == darray1[2]);
         REQUIRE(pack[4] == darray1[3]);
         REQUIRE(pack[5] == darray1[4]);

         if constexpr (CT::Typed<T>)
            REQUIRE(pack[0] == instance);
         else {
            REQUIRE(pack[0].GetRaw() == instance.GetRaw());
            REQUIRE(pack[0].GetCount() == 1);
         }

         DestroyElement(i666);
      }
      
      WHEN("Emplace item at the back") {
         auto i666 = CreateElement<E>(666);
         const auto i666backup = i666;
         auto instance = pack.EmplaceAt(Index::Back, ::std::move(i666));

         REQUIRE(pack.GetCount() == 6);
         REQUIRE(pack.GetReserved() >= 6);
         REQUIRE(pack.template IsExact<E>());
         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            REQUIRE(pack.GetRaw() == memory);
         #endif
         REQUIRE(pack[0] == darray1[0]);
         REQUIRE(pack[1] == darray1[1]);
         REQUIRE(pack[2] == darray1[2]);
         REQUIRE(pack[3] == darray1[3]);
         REQUIRE(pack[4] == darray1[4]);
         REQUIRE(pack[5] == i666backup);

         if constexpr (CT::Typed<T>)
            REQUIRE(pack[5] == instance);
         else {
            REQUIRE(pack[5].GetRaw() == instance.GetRaw());
            REQUIRE(pack[5].GetCount() == 1);
         }

         DestroyElement(i666);
      }

      WHEN("The size is reduced by finding and removing elements") {
         const auto removed2 = pack.Remove(darray1[1]);
         const auto removed4 = pack.Remove(darray1[3]);
         const auto temp = CreateElement<E>(666);

         REQUIRE(removed2 == 1);
         REQUIRE(removed4 == 1);
         REQUIRE(pack[0] == darray1[0]);
         REQUIRE(pack[1] == darray1[2]);
         REQUIRE(pack[2] == darray1[4]);
         REQUIRE_THROWS(pack[3] == temp);
         REQUIRE(pack.GetCount() == 3);
         REQUIRE(pack.GetReserved() >= 5);
         REQUIRE(pack.GetRaw() == memory);

         DestroyElement(temp);
      }

      WHEN("Removing non-available elements") {
         const auto removed9 = pack.Remove(darray2[3]);

         REQUIRE(removed9 == 0);
         REQUIRE(pack[0] == darray1[0]);
         REQUIRE(pack[1] == darray1[1]);
         REQUIRE(pack[2] == darray1[2]);
         REQUIRE(pack[3] == darray1[3]);
         REQUIRE(pack[4] == darray1[4]);
         REQUIRE(pack.GetCount() == 5);
         REQUIRE(pack.GetReserved() >= 5);
         REQUIRE(pack.GetRaw() == memory);
      }    

      WHEN("Empty pack with state is shallow-copied") {
         pack.MakeOr();
         auto copy = pack;

         REQUIRE(copy.GetRaw() == pack.GetRaw());
         REQUIRE(copy.GetCount() == pack.GetCount());
         REQUIRE(copy.GetReserved() == pack.GetReserved());
         REQUIRE(copy.GetState() == pack.GetState());
         REQUIRE(copy.GetType() == pack.GetType());
         REQUIRE(copy.GetUses() == 2);
      }

      WHEN("Pack is cloned") {
         pack.MakeOr();

         if constexpr (CT::CloneConstructible<E>) {
            T clone = Clone(pack);

            REQUIRE(clone.GetRaw() != pack.GetRaw());
            REQUIRE(clone.GetCount() == pack.GetCount());
            REQUIRE(clone.GetReserved() >= clone.GetCount());
            REQUIRE(clone.GetState() == pack.GetState());
            REQUIRE(clone.GetType() == pack.GetType());
            REQUIRE(clone.GetUses() == 1);
            REQUIRE(pack.GetUses() == 1);

            for (unsigned i = 0; i < 5; ++i) {
               REQUIRE(pack[i] == darray1[i]);
               REQUIRE(clone[i] != darray1[i]);
               REQUIRE(*clone[i] == *darray1[i]);
            }
         }
         else if constexpr (CT::Untyped<T>) {
            T clone;
            REQUIRE_THROWS(new (&clone) T {Langulus::Clone(pack)});
         }
      }

      WHEN("A forward value-based search is performed on existent value") {
         const auto found = pack.Find(darray1[2]);

         REQUIRE(found);
         REQUIRE(found == 2);
      }

      WHEN("A forward value-based search is performed on non-exitent value") {
         const auto found = pack.Find(darray2[2]);

         REQUIRE(found == Index::None);
         REQUIRE_FALSE(found);
      }

      WHEN("A backward value-based search is performed on existent value") {
         const auto found = pack.template Find<true>(darray1[2]);

         REQUIRE(found);
         REQUIRE(found == 2);
      }

      WHEN("A backward value-based search is performed on non-exitent value") {
         const auto found = pack.template Find<true>(darray2[2]);

         REQUIRE(found == Index::None);
         REQUIRE_FALSE(found);
      }
      
      WHEN("Merge-copy an element to the back, if not found (<<=)") {
         pack <<= darray2[3];

         REQUIRE(pack.GetCount() == 6);
         REQUIRE(pack.GetReserved() >= 6);
         REQUIRE(pack.template IsExact<E>());
         for (unsigned i = 0; i < 5; ++i)
            REQUIRE(pack[i] == darray1[i]);
         REQUIRE(pack[5] == darray2[3]);

         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            if constexpr (CT::Same<E, int>) {
               REQUIRE(pack.GetRaw() == memory);
            }
         #endif
      }

      WHEN("Merge-copy an element to the front, if not found (>>=)") {
         pack >>= darray2[3];

         REQUIRE(pack.GetCount() == 6);
         REQUIRE(pack.GetReserved() >= 6);
         REQUIRE(pack.template IsExact<E>());
         REQUIRE(pack[0] == darray2[3]);
         for (unsigned i = 1; i < 6; ++i)
            REQUIRE(pack[i] == darray1[i-1]);

         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            if constexpr (CT::Same<E, int>) {
               REQUIRE(pack.GetRaw() == memory);
            }
         #endif
      }

      WHEN("Merge-move an element to the back, if not found (<<=)") {
         auto moved = darray2[3];
         pack <<= ::std::move(moved);

         REQUIRE(pack.GetCount() == 6);
         REQUIRE(pack.GetReserved() >= 6);
         REQUIRE(pack.template IsExact<E>());
         for (unsigned i = 0; i < 5; ++i)
            REQUIRE(pack[i] == darray1[i]);
         REQUIRE(pack[5] == darray2[3]);

         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            if constexpr (CT::Same<E, int>) {
               REQUIRE(pack.GetRaw() == memory);
            }
         #endif
      }

      WHEN("Merge-move an element to the front, if not found (>>=)") {
         auto moved = darray2[3];
         pack >>= ::std::move(moved);

         REQUIRE(pack.GetCount() == 6);
         REQUIRE(pack.GetReserved() >= 6);
         REQUIRE(pack.template IsExact<E>());
         REQUIRE(pack[0] == darray2[3]);
         for (unsigned i = 1; i < 6; ++i)
            REQUIRE(pack[i] == darray1[i-1]);

         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            if constexpr (CT::Same<E, int>) {
               REQUIRE(pack.GetRaw() == memory);
            }
         #endif
      }

      WHEN("ForEach flat dense element (immutable)") {
         int it = 0;
         const auto foreachit = const_cast<const T&>(pack).ForEach(
            [&](const int& i) {
               REQUIRE(i == it + 1);
               ++it;
            },
            [&](const Tag& i) {
               REQUIRE(i == it + 1);
               ++it;
            },
            [&](const RT& i) {
               REQUIRE(i == it + 1);
               ++it;
            },
            [&](const Many& i) {
               const auto temp = CreateElement<DenseE>(it + 1);
               REQUIRE(i == static_cast<const Many&>(temp));
               ++it;
            }
         );

         REQUIRE(static_cast<unsigned>(it) == foreachit);
         if constexpr (CT::Text<Decay<E>>)
            REQUIRE(it == 0);
         else
            REQUIRE(static_cast<unsigned>(it) == pack.GetCount());
      }

      WHEN("ForEach flat dense element (mutable)") {
         int it = 0;
         const auto foreachit = const_cast<T&>(pack).ForEach(
            [&](int& i) {
               REQUIRE(i == it + 1);
               ++it;
            },
            [&](Tag& i) {
               REQUIRE(i == it + 1);
               ++it;
            },
            [&](RT& i) {
               REQUIRE(i == it + 1);
               ++it;
            },
            [&](Many& i) {
               const auto temp = CreateElement<DenseE>(it + 1);
               REQUIRE(i == static_cast<const Many&>(temp));
               ++it;
            }
         );

         REQUIRE(static_cast<unsigned>(it) == foreachit);
         if constexpr (CT::Text<Decay<E>>)
            REQUIRE(it == 0);
         else
            REQUIRE(static_cast<unsigned>(it) == pack.GetCount());
      }

      WHEN("ForEach flat sparse element (immutable)") {
         int it = 0;
         const auto foreachit = const_cast<const T&>(pack).ForEach(
            [&](const int* i) {
               REQUIRE(*i == it + 1);
               ++it;
            },
            [&](const Tag* i) {
               REQUIRE(*i == it + 1);
               ++it;
            },
            [&](const RT* i) {
               REQUIRE(*i == it + 1);
               ++it;
            },
            [&](const Many* i) {
               const auto temp = CreateElement<DenseE>(it + 1);
               REQUIRE(*i == static_cast<const Many&>(temp));
               ++it;
            }
         );

         REQUIRE(static_cast<unsigned>(it) == foreachit);
         if constexpr (CT::Text<Decay<E>>)
            REQUIRE(it == 0);
         else
            REQUIRE(static_cast<unsigned>(it) == pack.GetCount());
      }

      WHEN("ForEach flat sparse element (mutable)") {
         int it = 0;
         const auto foreachit = const_cast<T&>(pack).ForEach(
            [&](int* i) {
               REQUIRE(*i == it + 1);
               ++it;
            },
            [&](Tag* i) {
               REQUIRE(*i == it + 1);
               ++it;
            },
            [&](RT* i) {
               REQUIRE(*i == it + 1);
               ++it;
            },
            [&](Many* i) {
               const auto temp = CreateElement<DenseE>(it + 1);
               REQUIRE(*i == static_cast<const Many&>(temp));
               ++it;
            }
         );

         REQUIRE(static_cast<unsigned>(it) == foreachit);
         if constexpr (CT::Text<Decay<E>>)
            REQUIRE(it == 0);
         else
            REQUIRE(static_cast<unsigned>(it) == pack.GetCount());
      }

      WHEN("ForEachRev flat dense element (immutable)") {
         int it = 0;
         const auto foreachit = const_cast<const T&>(pack).template ForEach<true>(
            [&](const int& i) {
               REQUIRE(i == 5 - it);
               ++it;
            },
            [&](const Tag& i) {
               REQUIRE(i == 5 - it);
               ++it;
            },
            [&](const RT& i) {
               REQUIRE(i == 5 - it);
               ++it;
            },
            [&](const Many& i) {
               const auto temp = CreateElement<DenseE>(5 - it);
               REQUIRE(i == static_cast<const Many&>(temp));
               ++it;
            }
         );

         REQUIRE(static_cast<unsigned>(it) == foreachit);
         if constexpr (CT::Text<Decay<E>>)
            REQUIRE(it == 0);
         else
            REQUIRE(static_cast<unsigned>(it) == pack.GetCount());
      }

      WHEN("ForEachRev flat dense element (mutable)") {
         int it = 0;
         const auto foreachit = pack.template ForEach<true>(
            [&](int& i) {
               REQUIRE(i == 5 - it);
               ++it;
            },
            [&](Tag& i) {
               REQUIRE(i == 5 - it);
               ++it;
            },
            [&](RT& i) {
               REQUIRE(i == 5 - it);
               ++it;
            },
            [&](Many& i) {
               const auto temp = CreateElement<DenseE>(5 - it);
               REQUIRE(i == static_cast<const Many&>(temp));
               ++it;
            }
         );

         REQUIRE(static_cast<unsigned>(it) == foreachit);
         if constexpr (CT::Text<Decay<E>>)
            REQUIRE(it == 0);
         else
            REQUIRE(static_cast<unsigned>(it) == pack.GetCount());
      }

      WHEN("ForEachRev flat sparse element (immutable)") {
         int it = 0;
         const auto foreachit = const_cast<const T&>(pack).template ForEach<true>(
            [&](const int* i) {
               REQUIRE(*i == 5 - it);
               ++it;
            },
            [&](const Tag* i) {
               REQUIRE(*i == 5 - it);
               ++it;
            },
            [&](const RT* i) {
               REQUIRE(*i == 5 - it);
               ++it;
            },
            [&](const Many* i) {
               const auto temp = CreateElement<DenseE>(5 - it);
               REQUIRE(*i == static_cast<const Many&>(temp));
               ++it;
            }
         );

         REQUIRE(static_cast<unsigned>(it) == foreachit);
         if constexpr (CT::Text<Decay<E>>)
            REQUIRE(it == 0);
         else
            REQUIRE(static_cast<unsigned>(it) == pack.GetCount());
      }

      WHEN("ForEachRev flat sparse element (mutable)") {
         int it = 0;
         const auto foreachit = pack.template ForEach<true>(
            [&](int* i) {
               REQUIRE(*i == 5 - it);
               ++it;
            },
            [&](Tag* i) {
               REQUIRE(*i == 5 - it);
               ++it;
            },
            [&](RT* i) {
               REQUIRE(*i == 5 - it);
               ++it;
            },
            [&](Many* i) {
               const auto temp = CreateElement<DenseE>(5 - it);
               REQUIRE(*i == static_cast<const Many&>(temp));
               ++it;
            }
         );

         REQUIRE(static_cast<unsigned>(it) == foreachit);
         if constexpr (CT::Text<Decay<E>>)
            REQUIRE(it == 0);
         else
            REQUIRE(static_cast<unsigned>(it) == pack.GetCount());
      }
   }

   GIVEN("Two containers with some items") {
      T pack1 {darray1[0], darray1[1], darray1[2], darray1[3], darray1[4]};
      T pack2 {darray2[0], darray2[1], darray2[2], darray2[3], darray2[4]};
      const T memory1 = pack1;
      const T memory2 = pack2;

      WHEN("Copy-assign pack1 in pack2") {
         pack2 = Copy(pack1);

         REQUIRE(pack1.GetUses() == 2);
         REQUIRE(pack2.GetUses() == 1);
         REQUIRE(memory2.GetUses() == 1);
         REQUIRE(pack1 == pack2);
         REQUIRE(pack2 == memory1);
         REQUIRE(pack2 != memory2);
         for (int i = 0; i < 5; ++i)
            REQUIRE(pack2[i] == darray1[i]);
      }
      
      WHEN("Refer-assign pack1 in pack2") {
         pack2 = pack1;

         REQUIRE(pack1.GetUses() == 3);
         REQUIRE(pack2.GetUses() == 3);
         REQUIRE(memory2.GetUses() == 1);
         REQUIRE(pack1 == pack2);
         REQUIRE(pack2 == memory1);
         REQUIRE(pack2 != memory2);
         for (int i = 0; i < 5; ++i)
            REQUIRE(pack2[i] == darray1[i]);
      }

      WHEN("Move-assign pack1 in pack2") {
         auto movable = pack1;
         pack2 = ::std::move(movable);

         REQUIRE(pack1.GetUses() == 3);
         REQUIRE(pack2.GetUses() == 3);
         REQUIRE(memory2.GetUses() == 1);
         REQUIRE(pack1 == pack2);
         REQUIRE(movable != pack1);
         REQUIRE(movable == T {});
      }

      WHEN("Disown-assign pack1 in pack2") {
         pack2 = Disown(pack1);

         REQUIRE(pack1.GetUses() == 2);
         REQUIRE(pack2.GetUses() == 0);
         REQUIRE(memory2.GetUses() == 1);
         REQUIRE(pack1 == pack2);
         REQUIRE(pack2 == memory1);
         REQUIRE(pack2 != memory2);
         REQUIRE(pack2.GetAllocation() == nullptr);
         for (int i = 0; i < 5; ++i)
            REQUIRE(pack2[i] == darray1[i]);
      }

      WHEN("Abandon-assign pack1 in pack2") {
         auto movable = pack1;
         pack2 = Abandon(movable);

         REQUIRE(pack1.GetUses() == 3);
         REQUIRE(pack2.GetUses() == 3);
         REQUIRE(memory2.GetUses() == 1);
         REQUIRE(pack1 == pack2);
         REQUIRE(movable.GetAllocation() == nullptr);
      }

      WHEN("Copy-assign pack1 in pack2, then reset pack1") {
         pack2 = Copy(pack1);
         pack1.Reset();

         REQUIRE_FALSE(pack1.GetAllocation());
         REQUIRE(pack2.GetUses() == 1);
         REQUIRE_FALSE(pack1.GetRaw());
         REQUIRE(pack1.GetReserved() == 0);
         REQUIRE(pack2 == memory1);
      }
      
      WHEN("Refer-assign pack1 in pack2, then reset pack1") {
         pack2 = pack1;
         pack1.Reset();

         REQUIRE_FALSE(pack1.GetAllocation());
         REQUIRE(pack2.GetUses() == 2);
         REQUIRE_FALSE(pack1.GetRaw());
         REQUIRE(pack1.GetReserved() == 0);
         REQUIRE(pack2 == memory1);
      }

      if constexpr (CT::CloneMakable<E>) {
         WHEN("Clone-assign pack1 in pack2") {
            pack2 = Langulus::Clone(pack1);

            REQUIRE(pack1.GetUses() == 2);
            REQUIRE(pack2.GetUses() == 1);
            REQUIRE(pack1 != pack2);
            REQUIRE(pack2 != memory1);
            REQUIRE(pack2 != memory2);
         }

         WHEN("Clone-assign pack1 in pack2, then reset pack1") {
            pack2 = Langulus::Clone(pack1);
            const T memory3 = pack2;
            pack1.Reset();

            REQUIRE_FALSE(pack1.GetAllocation());
            REQUIRE(pack2.GetUses() == 2);
            REQUIRE(memory3.GetUses() == 2);
         }
      }
      else if constexpr (CT::Untyped<T>) {
         WHEN("Clone-assign pack1 in pack2") {
            REQUIRE_THROWS(pack2 = Langulus::Clone(pack1));
         }
      }

      WHEN("Concatenate both packs to a third pack") {
         const auto pack3 = pack1 + pack2;

         for (int i = 0; i < 5; ++i)
            REQUIRE(pack3[i] == darray1[i]);
         for (int i = 5; i < 10; ++i)
            REQUIRE(pack3[i] == darray2[i - 5]);
      }
   }
}
