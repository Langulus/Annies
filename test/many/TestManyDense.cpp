///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "TestManyCommon.hpp"


TEMPLATE_TEST_CASE("Dense Many/TMany", "[many]",
   (TypePair<Many, Tags::Count>),
   (TypePair<Tag, Text>),
   (TypePair<Tags::Name, Text>),

   (TypePair<TMany<int>, int>),
   (TypePair<TMany<Tag>, Tag>),
   (TypePair<TMany<Tags::Count>, Tags::Count>),
   (TypePair<TMany<Many>, Many>),
   (TypePair<TMany<Text>, Text>),

   (TypePair<Many, int>),
   (TypePair<Many, Tag>),
   (TypePair<Many, Many>),
   (TypePair<Many, Text>)
) {
   static Allocator::State memoryState;

   using T = typename TestType::LHS;
   using E = typename TestType::RHS;
      
   const E element = CreateElement<E>(555);

   const E darray1[5] {
      CreateElement<E>(1),
      CreateElement<E>(2),
      CreateElement<E>(3),
      CreateElement<E>(4),
      CreateElement<E>(5)
   };
   const E darray2[5] {
      CreateElement<E>(6),
      CreateElement<E>(7),
      CreateElement<E>(8),
      CreateElement<E>(9),
      CreateElement<E>(10)
   };

   GIVEN("Default constructed container") {
      T pack;

      WHEN("Emplace item at the front") {
         auto i666 = CreateElement<E>(666);
         const auto i666backup = i666;

         if constexpr (CT::Typed<T>) {
            auto& instance = pack.EmplaceAt(Index::Front, ::std::move(i666));

            Many_CheckState_OwnedFull<E>(pack);
            REQUIRE(pack.GetCount() == 1);
            REQUIRE(pack.GetReserved() >= 1);
            REQUIRE(pack[0] == i666backup);
            REQUIRE(&pack[0] == &instance);
         }
         else {
            REQUIRE_THROWS(pack.EmplaceAt(Index::Front, ::std::move(i666)));
            Many_CheckState_Default<E>(pack);
         }
      }
      
      WHEN("Emplace item at the back") {
         auto i666 = CreateElement<E>(666);
         const auto i666backup = i666;
         if constexpr (CT::Typed<T>) {
            auto& instance = pack.EmplaceAt(Index::Back, ::std::move(i666));

            Many_CheckState_OwnedFull<E>(pack);
            REQUIRE(pack.GetCount() == 1);
            REQUIRE(pack.GetReserved() >= 1);
            REQUIRE(pack[0] == i666backup);
            REQUIRE(&pack[0] == &instance);
         }
         else {
            REQUIRE_THROWS(pack.EmplaceAt(Index::Back, ::std::move(i666)));
            Many_CheckState_Default<E>(pack);
         }
      }

      WHEN("Removing non-available elements") {
      }

      WHEN("Empty pack with state is shallow-copied") {
         pack.EnableOr();
         auto copy = pack;

         Many_Helper_TestSame(copy, pack);
         REQUIRE(copy.GetState() == pack.GetState());
         REQUIRE(copy.GetUses() == 0);
      }

      WHEN("Packs are compared") {
         T another_pack1;
         another_pack1  << CreateElement<E>(1) 
                        << CreateElement<E>(2)
                        << CreateElement<E>(3)
                        << CreateElement<E>(4)
                        << CreateElement<E>(5);

         T another_pack2;
         another_pack2  << CreateElement<E>(2)
                        << CreateElement<E>(2)
                        << CreateElement<E>(3)
                        << CreateElement<E>(4)
                        << CreateElement<E>(5);

         T another_pack3;
         another_pack3  << CreateElement<E>(1)
                        << CreateElement<E>(2)
                        << CreateElement<E>(3)
                        << CreateElement<E>(4)
                        << CreateElement<E>(5)
                        << CreateElement<E>(6);
         T defaulted_pack1;

         TMany<uint> another_pack4;
         another_pack4  << uint(1) << uint(2) << uint(3) << uint(4) << uint(5);

         Many another_pack5;
         another_pack5  << CreateElement<E>(1)
                        << CreateElement<E>(2)
                        << CreateElement<E>(3)
                        << CreateElement<E>(4)
                        << CreateElement<E>(5);

         Many defaulted_pack2;

         REQUIRE(pack != another_pack1);
         REQUIRE(pack != another_pack2);
         REQUIRE(pack != another_pack3);
         //REQUIRE(pack != another_pack4);
         REQUIRE(pack != another_pack5);
         REQUIRE(pack == defaulted_pack1);
         REQUIRE(pack == defaulted_pack2);
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
            [&](const int&)    {FAIL();},
            [&](const Tag&)    {FAIL();},
            [&](const Many&)   {FAIL();}
         );

         REQUIRE(0 == foreachit);
      }

      WHEN("ForEach flat dense element (mutable)") {
         const auto foreachit = const_cast<T&>(pack).ForEach(
            [&](int&)         {FAIL(); },
            [&](Tag&)         {FAIL(); },
            [&](Many&)        {FAIL(); }
         );

         REQUIRE(0 == foreachit);
      }

      WHEN("ForEach flat sparse element (immutable)") {
         const auto foreachit = const_cast<const T&>(pack).ForEach(
            [&](const int*)   {FAIL(); },
            [&](const Tag*)   {FAIL(); },
            [&](const Many*)  {FAIL(); }
         );

         REQUIRE(0 == foreachit);
      }

      WHEN("ForEach flat sparse element (mutable)") {
         const auto foreachit = const_cast<T&>(pack).ForEach(
            [&](int*)         {FAIL(); },
            [&](Tag*)         {FAIL(); },
            [&](Many*)        {FAIL(); }
         );

         REQUIRE(0 == foreachit);
      }

      WHEN("ForEachRev flat dense element (immutable)") {
         const auto foreachit = const_cast<const T&>(pack).ForEachRev(
            [&](const int&)   {FAIL(); },
            [&](const Tag&)   {FAIL(); },
            [&](const Many&)  {FAIL(); }
         );

         REQUIRE(0 == foreachit);
      }

      WHEN("ForEachRev flat dense element (mutable)") {
         const auto foreachit = pack.ForEachRev(
            [&](const int&)   {FAIL(); },
            [&](const Tag&)   {FAIL(); },
            [&](const Many&)  {FAIL(); }
         );

         REQUIRE(0 == foreachit);
      }

      WHEN("ForEachRev flat sparse element (immutable)") {
         const auto foreachit = const_cast<const T&>(pack).ForEachRev(
            [&](const int*)   {FAIL(); },
            [&](const Tag*)   {FAIL(); },
            [&](const Many*)  {FAIL(); }
         );

         REQUIRE(0 == foreachit);
      }

      WHEN("ForEachRev flat sparse element (mutable)") {
         const auto foreachit = pack.ForEachRev(
            [&](const int*)   {FAIL(); },
            [&](const Tag*)   {FAIL(); },
            [&](const Many*)  {FAIL(); }
         );

         REQUIRE(0 == foreachit);
      }

      if constexpr (CT::Exact<E, Text>) {
         WHEN("Given an element that will be destroyed before the pack") {
            Text owned_text = "666";
            pack << Text(owned_text.operator Token());
         }
      }
   }

   GIVEN("Container constructed by static list of exactly the same shallow-copied elements") {
      if constexpr (CT::Untyped<T>) {
         const T pack {element, element};

         Many_CheckState_OwnedFull<E>(pack);
         REQUIRE(pack.GetCount() == 2);
         REQUIRE(pack.GetReserved() >= 2);
         for (auto& e : pack)
            REQUIRE(e == element);
      }
   }

   GIVEN("Container constructed by static list of somewhat different shallow-copied elements") {
      if constexpr (CT::Untyped<T>) {
         const T pack {element, &element};

         Many_CheckState_OwnedFull<Many>(pack);
         REQUIRE(pack.GetCount() == 2);
         REQUIRE(pack.GetReserved() >= 2);
         REQUIRE(pack[0] == Many {element});
         REQUIRE(pack[1] == Many {&element});
      }
   }

   GIVEN("Container with some items") {
      T pack {};
      pack << darray1[0] << darray1[1] << darray1[2] << darray1[3] << darray1[4];
      const auto previousReserved = pack.GetReserved();
      const auto memory = pack.GetRaw();
      
      Many_CheckState_OwnedFull<E>(pack);
      REQUIRE(pack.GetCount() == 5);
      REQUIRE(pack.GetReserved() >= 5);
      REQUIRE(pack.GetRaw());
      for (uint i = 0; i < pack.GetCount(); ++i)
         REQUIRE(pack[i] == darray1[i]);

      WHEN("Shallow-copy more of the same stuff to the back (<<)") {
         pack << darray2[0] << darray2[1] << darray2[2] << darray2[3] << darray2[4];

         Many_CheckState_OwnedFull<E>(pack);
         REQUIRE(pack.GetCount() == 10);
         REQUIRE(pack.GetReserved() >= 10);

         for (uint i = 0; i < 5; ++i)
            REQUIRE(pack[i] == darray1[i]);
         for (uint i = 5; i < pack.GetCount(); ++i)
            REQUIRE(pack[i] == darray2[i-5]);

         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            if constexpr (CT::Same<E, int>)
               REQUIRE(pack.GetRaw() == memory);
         #endif
      }

      WHEN("Shallow-copy more of the same stuff to the front (>>)") {
         pack >> darray2[0] >> darray2[1] >> darray2[2] >> darray2[3] >> darray2[4];

         Many_CheckState_OwnedFull<E>(pack);
         REQUIRE(pack.GetCount() == 10);
         REQUIRE(pack.GetReserved() >= 10);

         for (uint i = 5; i > 0; --i)
            REQUIRE(pack[5 - i] == darray2[i - 1]);
         for (uint i = 5; i < pack.GetCount(); ++i)
            REQUIRE(pack[i] == darray1[i-5]);

         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            if constexpr (CT::Same<E, int>)
               REQUIRE(pack.GetRaw() == memory);
         #endif
      }

      WHEN("Shallow-copy an array to the back") {
         pack.InsertAt(Index::Back, darray2);

         Many_CheckState_OwnedFull<E>(pack);
         REQUIRE(pack.GetCount() == 10);
         REQUIRE(pack.GetReserved() >= 10);

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

      WHEN("Shallow-copy an array to the front") {
         pack.InsertAt(Index::Front, darray2);

         Many_CheckState_OwnedFull<E>(pack);
         REQUIRE(pack.GetCount() == 10);
         REQUIRE(pack.GetReserved() >= 10);

         for (uint i = 0; i < 5; ++i)
            REQUIRE(pack[i] == darray2[i]);
         for (uint i = 5; i < pack.GetCount(); ++i)
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

         Many_CheckState_OwnedFull<E>(pack);
         REQUIRE(pack.GetCount() == 10);
         REQUIRE(pack.GetReserved() >= 10);

         for (uint i = 0; i < 5; ++i)
            REQUIRE(pack[i] == darray1[i]);
         for (uint i = 5; i < pack.GetCount(); ++i)
            REQUIRE(pack[i] == darray3backup[i - 5]);

         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            REQUIRE(pack.GetRaw() == memory);
         #endif
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

         Many_CheckState_OwnedFull<E>(pack);
         REQUIRE(pack.GetCount() == 10);
         REQUIRE(pack.GetReserved() >= 10);

         for (uint i = 5; i > 0; --i)
            REQUIRE(pack[5 - i] == darray3backup[i - 1]);
         for (uint i = 5; i < pack.GetCount(); ++i)
            REQUIRE(pack[i] == darray1[i - 5]);

         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            REQUIRE(pack.GetRaw() == memory);
         #endif
      }
      
      WHEN("Insert single item at a specific place by shallow-copy") {
         const auto i666 = CreateElement<E>(666);
         pack.InsertAt(3, i666);

         Many_CheckState_OwnedFull<E>(pack);
         REQUIRE(pack.GetCount() == 6);
         REQUIRE(pack.GetReserved() >= 6);
         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            REQUIRE(pack.GetRaw() == memory);
         #endif
         REQUIRE(pack[0] == darray1[0]);
         REQUIRE(pack[1] == darray1[1]);
         REQUIRE(pack[2] == darray1[2]);
         REQUIRE(pack[3] == i666);
         REQUIRE(pack[4] == darray1[3]);
         REQUIRE(pack[5] == darray1[4]);
      }

      WHEN("Insert multiple items at a specific place by shallow-copy") {
         pack.InsertAt(3, darray2);

         Many_CheckState_OwnedFull<E>(pack);
         REQUIRE(pack.GetCount() == 10);
         REQUIRE(pack.GetReserved() >= 10);
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

         Many_CheckState_OwnedFull<E>(pack);
         REQUIRE(pack.GetCount() == 6);
         REQUIRE(pack.GetReserved() >= 6);
         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            REQUIRE(pack.GetRaw() == memory);
         #endif
         REQUIRE(pack[0] == darray1[0]);
         REQUIRE(pack[1] == darray1[1]);
         REQUIRE(pack[2] == darray1[2]);
         REQUIRE(pack[3] == i666backup);
         REQUIRE(pack[4] == darray1[3]);
         REQUIRE(pack[5] == darray1[4]);
      }

      WHEN("Emplace item at a specific place") {
         auto i666 = CreateElement<E>(666);
         const auto i666backup = i666;
         decltype(auto) instance = pack.EmplaceAt(3, ::std::move(i666));

         Many_CheckState_OwnedFull<E>(pack);
         REQUIRE(pack.GetCount() == 6);
         REQUIRE(pack.GetReserved() >= 6);
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
            REQUIRE(&pack[3] == &instance);
         else {
            REQUIRE(pack[3].GetRaw() == instance.GetRaw());
            REQUIRE(pack[3].GetCount() == 1);
         }
      }

      WHEN("Emplace item at the front") {
         auto i666 = CreateElement<E>(666);
         const auto i666backup = i666;
         decltype(auto) instance = pack.EmplaceAt(Index::Front, ::std::move(i666));

         Many_CheckState_OwnedFull<E>(pack);
         REQUIRE(pack.GetCount() == 6);
         REQUIRE(pack.GetReserved() >= 6);
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
            REQUIRE(&pack[0] == &instance);
         else {
            REQUIRE(pack[0].GetRaw() == instance.GetRaw());
            REQUIRE(pack[0].GetCount() == 1);
         }
      }
      
      WHEN("Emplace item at the back") {
         auto i666 = CreateElement<E>(666);
         const auto i666backup = i666;
         decltype(auto) instance = pack.EmplaceAt(Index::Back, ::std::move(i666));

         Many_CheckState_OwnedFull<E>(pack);
         REQUIRE(pack.GetCount() == 6);
         REQUIRE(pack.GetReserved() >= 6);
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
            REQUIRE(&pack[5] == &instance);
         else {
            REQUIRE(pack[5].GetRaw() == instance.GetRaw());
            REQUIRE(pack[5].GetCount() == 1);
         }
      }

      WHEN("The size is reduced by finding and removing elements, but reserved memory should remain the same on shrinking") {
         const auto removed2 = pack.Remove(darray1[1]);
         const auto removed4 = pack.Remove(darray1[3]);

         REQUIRE(removed2 == 1);
         REQUIRE(removed4 == 1);
         REQUIRE(pack[0] == darray1[0]);
         REQUIRE(pack[1] == darray1[2]);
         REQUIRE(pack[2] == darray1[4]);
         REQUIRE_THROWS(pack[3] == CreateElement<E>(666));
         REQUIRE(pack.GetCount() == 3);
         REQUIRE(pack.GetReserved() >= 5);
         REQUIRE(pack.GetRaw() == memory);
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

      WHEN("Packs are compared") {
         T another_pack1;
         another_pack1 << CreateElement<E>(1)
                       << CreateElement<E>(2)
                       << CreateElement<E>(3)
                       << CreateElement<E>(4)
                       << CreateElement<E>(5);
         T another_pack2;
         another_pack2 << CreateElement<E>(2)
                       << CreateElement<E>(2)
                       << CreateElement<E>(3)
                       << CreateElement<E>(4)
                       << CreateElement<E>(5);
         T another_pack3;
         another_pack3 << CreateElement<E>(1)
                       << CreateElement<E>(2)
                       << CreateElement<E>(3)
                       << CreateElement<E>(4)
                       << CreateElement<E>(5)
                       << CreateElement<E>(6);
         TMany<uint> another_pack4;
         another_pack4 << uint(1) << uint(2) << uint(3) << uint(4) << uint(5);
         Many another_pack5;
         another_pack5 << CreateElement<E>(1)
                       << CreateElement<E>(2)
                       << CreateElement<E>(3)
                       << CreateElement<E>(4)
                       << CreateElement<E>(5);

         REQUIRE(pack == another_pack1);
         REQUIRE(pack != another_pack2);
         REQUIRE(pack != another_pack3);
         //REQUIRE(pack != another_pack4);
         REQUIRE(pack == another_pack5);
      }

      WHEN("A forward value-based search is performed on existent value") {
         const auto found = pack.Find(darray1[2]);

         REQUIRE(found);
         REQUIRE(found == 2);
      }

      WHEN("A forward value-based search is performed on non-exitent value") {
         const auto found = pack.Find(darray2[2]);

         REQUIRE(found == IndexNone);
         REQUIRE_FALSE(found);
      }

      WHEN("A backward value-based search is performed on existent value") {
         const auto found = pack.template Find<true>(darray1[2]);

         REQUIRE(found);
         REQUIRE(found == 2);
      }

      WHEN("A backward value-based search is performed on non-exitent value") {
         const auto found = pack.template Find<true>(darray2[2]);

         REQUIRE(found == IndexNone);
         REQUIRE_FALSE(found);
      }
      
      WHEN("Merge-copy an element to the back, if not found (<<=)") {
         pack <<= darray2[3];

         Many_CheckState_OwnedFull<E>(pack);
         REQUIRE(pack.GetCount() == 6);
         REQUIRE(pack.GetReserved() >= 6);
         for (uint i = 0; i < 5; ++i)
            REQUIRE(pack[i] == darray1[i]);
         REQUIRE(pack[5] == darray2[3]);

         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            if constexpr (CT::Same<E, int>)
               REQUIRE(pack.GetRaw() == memory);
         #endif
         
      }

      WHEN("Merge-copy an element to the front, if not found (>>=)") {
         pack >>= darray2[3];

         Many_CheckState_OwnedFull<E>(pack);
         REQUIRE(pack.GetCount() == 6);
         REQUIRE(pack.GetReserved() >= 6);
         REQUIRE(pack[0] == darray2[3]);
         for (uint i = 1; i < 6; ++i)
            REQUIRE(pack[i] == darray1[i-1]);

         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            if constexpr (CT::Same<E, int>)
               REQUIRE(pack.GetRaw() == memory);
         #endif
         
      }

      WHEN("Merge-move an element to the back, if not found (<<=)") {
         auto moved = darray2[3];
         pack <<= ::std::move(moved);

         Many_CheckState_OwnedFull<E>(pack);
         REQUIRE(pack.GetCount() == 6);
         REQUIRE(pack.GetReserved() >= 6);
         for (uint i = 0; i < 5; ++i)
            REQUIRE(pack[i] == darray1[i]);
         REQUIRE(pack[5] == darray2[3]);

         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            if constexpr (CT::Same<E, int>)
               REQUIRE(pack.GetRaw() == memory);
         #endif
      }

      WHEN("Merge-move an element to the front, if not found (>>=)") {
         auto moved = darray2[3];
         pack >>= ::std::move(moved);

         Many_CheckState_OwnedFull<E>(pack);
         REQUIRE(pack.GetCount() == 6);
         REQUIRE(pack.GetReserved() >= 6);
         REQUIRE(pack[0] == darray2[3]);
         for (uint i = 1; i < 6; ++i)
            REQUIRE(pack[i] == darray1[i-1]);

         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            if constexpr (CT::Same<E, int>)
               REQUIRE(pack.GetRaw() == memory);
         #endif
      }

      WHEN("ForEach flat dense element (immutable)") {
         int it = 0;
         const auto foreachit = const_cast<const T&>(pack).ForEach(
            [&](const int& i) {
               REQUIRE(i == it + 1);
               ++it;
            },
            [&](const Trait& i) {
               REQUIRE(i == it + 1);
               ++it;
            },
            [&](const Many& i) {
               const auto temp = CreateElement<E>(it + 1);
               REQUIRE(i == static_cast<const Many&>(temp));
               ++it;
            }
         );

         REQUIRE(static_cast<uint>(it) == foreachit);
         if constexpr (CT::Text<E>)
            REQUIRE(it == 0);
         else
            REQUIRE(static_cast<uint>(it) == pack.GetCount());
      }

      WHEN("ForEach flat dense element (mutable)") {
         int it = 0;
         const auto foreachit = const_cast<T&>(pack).ForEach(
            [&](int& i) {
               REQUIRE(i == it + 1);
               ++it;
            },
            [&](const Trait& i) {
               REQUIRE(i == it + 1);
               ++it;
            },
            [&](const Many& i) {
               const auto temp = CreateElement<E>(it + 1);
               REQUIRE(i == static_cast<const Many&>(temp));
               ++it;
            }
         );

         REQUIRE(static_cast<uint>(it) == foreachit);

         if constexpr (CT::Same<E, Text>)
            REQUIRE(it == 0);
         else
            REQUIRE(static_cast<uint>(it) == pack.GetCount());
      }

      WHEN("ForEachRev flat dense element (immutable)") {
         int it = 0;
         const auto foreachit = const_cast<const T&>(pack).template ForEach<true>(
            [&](const int& i) {
               REQUIRE(i == 5 - it);
               ++it;
            },
            [&](const Trait& i) {
               REQUIRE(i == 5 - it);
               ++it;
            },
            [&](const Many& i) {
               const auto temp = CreateElement<E>(5 - it);
               REQUIRE(i == static_cast<const Many&>(temp));
               ++it;
            }
         );

         REQUIRE(static_cast<uint>(it) == foreachit);

         if constexpr (CT::Same<E, Text>)
            REQUIRE(it == 0);
         else
            REQUIRE(static_cast<uint>(it) == pack.GetCount());
      }

      WHEN("ForEachRev flat dense element (mutable)") {
         int it = 0;
         const auto foreachit = pack.template ForEach<true>(
            [&](int& i) {
               REQUIRE(i == 5 - it);
               ++it;
            },
            [&](const Trait& i) {
               REQUIRE(i == 5 - it);
               ++it;
            },
            [&](const Many& i) {
               const auto temp = CreateElement<E>(5 - it);
               REQUIRE(i == static_cast<const Many&>(temp));
               ++it;
            }
         );

         REQUIRE(static_cast<uint>(it) == foreachit);

         if constexpr (CT::Same<E, Text>)
            REQUIRE(it == 0);
         else
            REQUIRE(static_cast<uint>(it) == pack.GetCount());
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

      WHEN("Clone-assign pack1 in pack2") {
         pack2 = Clone(pack1);

         REQUIRE(pack1.GetUses() == 2);
         REQUIRE(pack2.GetUses() == 1);
         REQUIRE(pack1 == pack2);
         REQUIRE(pack2 == memory1);
         REQUIRE(pack2 != memory2);
      }

      WHEN("Clone-assign pack1 in pack2, then reset pack1") {
         pack2 = Clone(pack1);
         const T memory3 = pack2;
         pack1.Reset();

         REQUIRE_FALSE(pack1.GetAllocation());
         REQUIRE(pack2.GetUses() == 2);
         REQUIRE(memory3.GetUses() == 2);
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
