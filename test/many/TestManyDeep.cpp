///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "TestManyCommon.hpp"


TEMPLATE_TEST_CASE("Deep sequential containers 1", "[any]", RT*, int, RT, int*) {
   static Allocator::State memoryState;
   static_assert(sizeof(A::Block) == sizeof(Block<>));
   using E = TestType;

   const E darray[10] {
      CreateElement<E, true>(1),
      CreateElement<E, true>(2),
      CreateElement<E, true>(3),
      CreateElement<E, true>(4),
      CreateElement<E, true>(5),
      CreateElement<E, true>(6),
      CreateElement<E, true>(7),
      CreateElement<E, true>(8),
      CreateElement<E, true>(9),
      CreateElement<E, true>(10)
   };

   GIVEN("Any with some deep items") {
      Many pack;
      Many subpack1;
      Many subpack2;
      Many subpack3;
      subpack1 << darray[0] << darray[1] << darray[2] << darray[3] << darray[4];
      REQUIRE(subpack1.GetUses() == 1);

      subpack2 << darray[5] << darray[6] << darray[7] << darray[8] << darray[9];
      REQUIRE(subpack2.GetUses() == 1);

      subpack3 << subpack1 << subpack2;
      REQUIRE(subpack1.GetUses() == 2);
      REQUIRE(subpack2.GetUses() == 2);
      REQUIRE(subpack3.GetUses() == 1);

      pack << subpack1 << subpack2 << subpack3;
      REQUIRE(pack.GetUses() == 1);
      REQUIRE(subpack1.GetUses() == 3);
      REQUIRE(subpack2.GetUses() == 3);
      REQUIRE(subpack3.GetUses() == 2);

      pack.MakeTypeConstrained();

      auto memory = pack.GetRaw<Many>();

      REQUIRE(pack.GetCount() == 3);
      REQUIRE(pack.GetReserved() >= 3);
      REQUIRE(pack.Is<Many>());
      REQUIRE(pack.GetRaw<Many>());

      WHEN("Getting deep elements") {
         REQUIRE(pack.GetCountDeep() == 6);
         REQUIRE(pack.GetCountElementsDeep() == 20);
         REQUIRE(pack.GetBlockDeep(0));
         REQUIRE(pack.GetBlockDeep(1));
         REQUIRE(pack.GetBlockDeep(2));
         REQUIRE(pack.GetBlockDeep(3));
         REQUIRE(pack.GetBlockDeep(4));
         REQUIRE(pack.GetBlockDeep(5));
         REQUIRE(pack.GetBlockDeep(666) == nullptr);
         REQUIRE(*pack.GetBlockDeep(0) == pack);
         REQUIRE(*pack.GetBlockDeep(1) == subpack1);
         REQUIRE(*pack.GetBlockDeep(2) == subpack2);
         REQUIRE(*pack.GetBlockDeep(3) == subpack3);
         REQUIRE(*pack.GetBlockDeep(4) == subpack1);
         REQUIRE(*pack.GetBlockDeep(5) == subpack2);
         for (int i = 0; i < 10; ++i) {
            REQUIRE(pack.GetElementDeep(i) == darray[i]);
            REQUIRE(pack.GetElementDeep(i + 10) == darray[i]);
         }
         REQUIRE(pack.GetElementDeep(666).IsEmpty());
      }

      WHEN("Push more stuff") {
         REQUIRE_THROWS_AS(pack << int(6), Except::Mutate);

         REQUIRE(pack.GetCount() == 3);
         REQUIRE(pack.GetReserved() >= 3);
         REQUIRE(pack.Is<Many>());
         REQUIRE(pack.GetRaw<Many>());
      }

      WHEN("Element 0 is removed") {
         const auto refsBefore = pack.GetUses();
         pack.RemoveIndex(0);

         REQUIRE(pack.GetCount() == 2);
         REQUIRE(pack.As<Many>(0) == subpack2);
         REQUIRE(pack.As<Many>(1) == subpack3);
         REQUIRE(pack.GetReserved() >= 3);
         REQUIRE(pack.Is<Many>());
         REQUIRE(pack.GetRaw<Many>() == memory);
         REQUIRE(pack.GetUses() == refsBefore);
         REQUIRE(subpack1.GetUses() == 2);
         REQUIRE(subpack2.GetUses() == 3);
         REQUIRE(subpack3.GetUses() == 2);
      }

      WHEN("Element 1 is removed") {
         const auto refsBefore = pack.GetUses();
         pack.RemoveIndex(1);

         REQUIRE(pack.GetCount() == 2);
         REQUIRE(pack.As<Many>(0) == subpack1);
         REQUIRE(pack.As<Many>(1) == subpack3);
         REQUIRE(pack.GetReserved() >= 3);
         REQUIRE(pack.Is<Many>());
         REQUIRE(pack.GetRaw<Many>() == memory);
         REQUIRE(pack.GetUses() == refsBefore);
         REQUIRE(subpack1.GetUses() == 3);
         REQUIRE(subpack2.GetUses() == 2);
         REQUIRE(subpack3.GetUses() == 2);
      }

      WHEN("Element 2 is removed") {
         const auto refsBefore = pack.GetUses();
         pack.RemoveIndex(2);

         REQUIRE(pack.GetCount() == 2);
         REQUIRE(pack.As<Many>(0) == subpack1);
         REQUIRE(pack.As<Many>(1) == subpack2);
         REQUIRE(pack.GetReserved() >= 3);
         REQUIRE(pack.Is<Many>());
         REQUIRE(pack.GetRaw<Many>() == memory);
         REQUIRE(pack.GetUses() == refsBefore);
         REQUIRE(subpack1.GetUses() == 3);
         REQUIRE(subpack2.GetUses() == 3);
         REQUIRE(subpack3.GetUses() == 1);
      }

      WHEN("All element are removed one by one") {
         pack.RemoveIndex(0);
         pack.RemoveIndex(0);
         pack.RemoveIndex(0);

         REQUIRE(!pack);
         REQUIRE(pack.GetReserved() > 0);
         REQUIRE(pack.Is<Many>());
         REQUIRE(pack.IsTypeConstrained());
         REQUIRE(pack.GetRaw<Many>() != nullptr);
         REQUIRE(pack.GetUses() > 0);
         REQUIRE(subpack1.GetUses() == 2);
         REQUIRE(subpack2.GetUses() == 2);
         REQUIRE(subpack3.GetUses() == 1);
      }

      WHEN("The size is reduced, by finding and removing") {
         pack.RemoveIndex(pack.Find(subpack1));

         REQUIRE(pack.GetCount() == 2);
         REQUIRE(pack.As<Many>(0) == subpack2);
         REQUIRE(pack.As<Many>(1) == subpack3);
         REQUIRE(pack.GetReserved() >= 3);
         REQUIRE(pack.Is<Many>());
         REQUIRE(pack.GetRaw<Many>() != nullptr);
      }

      WHEN("Pack is cleared") {
         pack.Clear();

         REQUIRE(pack.GetCount() == 0);
         REQUIRE(pack.GetReserved() >= 3);
         REQUIRE(pack.GetRaw<Many>() == memory);
         REQUIRE(pack.Is<Many>());
      }

      WHEN("Pack is reset") {
         pack.Reset();

         REQUIRE(pack.GetCount() == 0);
         REQUIRE(pack.GetReserved() == 0);
         REQUIRE(pack.GetRaw<Many>() == nullptr);
         REQUIRE(pack.Is<Many>());
         REQUIRE(pack.IsTypeConstrained());
      }

      WHEN("Pack is shallow-copied") {
         pack.As<Many>(2).As<Many>(1).MakeOr();
         pack.As<Many>(0).MakeOr();

         auto copy = pack;

         REQUIRE(copy.GetRaw<Many>() == pack.GetRaw<Many>());
         REQUIRE(copy.GetCount() == pack.GetCount());
         REQUIRE(copy.GetReserved() == pack.GetReserved());
         REQUIRE(copy.GetState() == pack.GetState());
         REQUIRE(copy.GetType() == pack.GetType());
         REQUIRE(copy.GetUses() == 2);
         REQUIRE(copy.As<Many>(0).GetRaw<Many>() == subpack1.GetRaw<Many>());
         REQUIRE(copy.As<Many>(0).IsOr());
         REQUIRE(copy.As<Many>(0).GetCount() == subpack1.GetCount());
         REQUIRE(copy.As<Many>(0).GetUses() == 3);
         REQUIRE(copy.As<Many>(1).GetRaw<Many>() == subpack2.GetRaw<Many>());
         REQUIRE(copy.As<Many>(1).GetState() == DataState::Default);
         REQUIRE(copy.As<Many>(1).GetCount() == subpack2.GetCount());
         REQUIRE(copy.As<Many>(1).GetUses() == 3);
         REQUIRE(copy.As<Many>(2).GetRaw<Many>() == subpack3.GetRaw<Many>());
         REQUIRE(copy.As<Many>(2).GetState() == DataState::Default);
         REQUIRE(copy.As<Many>(2).GetCount() == subpack3.GetCount());
         REQUIRE(copy.As<Many>(2).GetUses() == 2);
         REQUIRE(copy.As<Many>(2).As<Many>(0).GetRaw<Many>() == subpack1.GetRaw<Many>());
         REQUIRE(copy.As<Many>(2).As<Many>(0).GetState() == DataState::Default);
         REQUIRE(copy.As<Many>(2).As<Many>(0).GetCount() == subpack1.GetCount());
         REQUIRE(copy.As<Many>(2).As<Many>(1).GetRaw<Many>() == subpack2.GetRaw<Many>());
         REQUIRE(copy.As<Many>(2).As<Many>(1).IsOr());
         REQUIRE(copy.As<Many>(2).As<Many>(1).GetCount() == subpack2.GetCount());
      }

      WHEN("Pack is cloned") {
         pack.As<Many>(2).As<Many>(1).MakeOr();
         pack.As<Many>(0).MakeOr();

         Many clone = Clone(pack);

         REQUIRE(clone.GetRaw<Many>() != pack.GetRaw<Many>());
         REQUIRE(clone.GetCount() == pack.GetCount());
         REQUIRE(clone.GetReserved() >= clone.GetCount());
         REQUIRE(clone.GetState() == pack.GetState());
         REQUIRE(clone.GetType() == pack.GetType());
         REQUIRE(clone.GetUses() == 1);
         REQUIRE(pack.GetUses() == 1);
         REQUIRE(clone.As<Many>(0).GetRaw<Many>() != subpack1.GetRaw<Many>());
         REQUIRE(clone.As<Many>(0).IsOr());
         REQUIRE(clone.As<Many>(0).GetCount() == subpack1.GetCount());
         REQUIRE(clone.As<Many>(0).GetUses() == 1);
         REQUIRE( pack.As<Many>(0).GetUses() == 3);
         REQUIRE(clone.As<Many>(1).GetRaw<Many>() != subpack2.GetRaw<Many>());
         REQUIRE(clone.As<Many>(1).GetState() == DataState::Default);
         REQUIRE(clone.As<Many>(1).GetCount() == subpack2.GetCount());
         REQUIRE(clone.As<Many>(1).GetUses() == 1);
         REQUIRE( pack.As<Many>(1).GetUses() == 3);
         REQUIRE(clone.As<Many>(2).GetRaw<Many>() != subpack3.GetRaw<Many>());
         REQUIRE(clone.As<Many>(2).GetState() == DataState::Default);
         REQUIRE(clone.As<Many>(2).GetCount() == subpack3.GetCount());
         REQUIRE(clone.As<Many>(2).GetUses() == 1);
         REQUIRE( pack.As<Many>(2).GetUses() == 2);
         REQUIRE(clone.As<Many>(2).As<Many>(0).GetRaw<Many>() != subpack1.GetRaw<Many>());
         REQUIRE(clone.As<Many>(2).As<Many>(0).GetState() == DataState::Default);
         REQUIRE(clone.As<Many>(2).As<Many>(0).GetCount() == subpack1.GetCount());
         REQUIRE(clone.As<Many>(2).As<Many>(0).GetUses() == 1);
         REQUIRE( pack.As<Many>(2).As<Many>(0).GetUses() == 3);
         REQUIRE(clone.As<Many>(2).As<Many>(1).GetRaw<Many>() != subpack2.GetRaw<Many>());
         REQUIRE(clone.As<Many>(2).As<Many>(1).IsOr());
         REQUIRE(clone.As<Many>(2).As<Many>(1).GetCount() == subpack2.GetCount());
         REQUIRE(clone.As<Many>(2).As<Many>(1).GetUses() == 1);
         REQUIRE( pack.As<Many>(2).As<Many>(1).GetUses() == 3);
      }

      WHEN("Smart pushing different type without retainment") {
         auto result = subpack1.SmartPush<true, void>(IndexBack, '?');

         REQUIRE(result == 0);
         REQUIRE(subpack1.GetCount() == 5);
      }

      WHEN("Smart pushing with retainment") {
         Many deepened;
         deepened << int(1) << int(2) << int(3) << int(4) << int(5);
         auto result = deepened.SmartPush<false>(IndexBack, '?');

         REQUIRE(result == 1);
         REQUIRE(deepened.IsDeep());
         REQUIRE(deepened.GetCount() == 2);
         REQUIRE(deepened.As<Many>(0).GetCount() == 5);
         REQUIRE(deepened.As<Many>(1).GetCount() == 1);
      }

      WHEN("Smart pushing an empty container (but not stateless) with retainment") {
         Many deepened;
         deepened << int(1) << int(2) << int(3) << int(4) << int(5);
         auto pushed = Many::FromMeta(nullptr, DataState::Missing);
         auto result = deepened.SmartPush(IndexBack, pushed);

         REQUIRE(result == 1);
         REQUIRE(deepened.IsDeep());
         REQUIRE(deepened.GetCount() == 2);
         REQUIRE(deepened.As<Many>(0).GetCount() == 5);
         REQUIRE(deepened.As<Many>(1).GetCount() == 0);
         REQUIRE(deepened.As<Many>(1).GetState() == DataState::Missing);
      }

      WHEN("Smart pushing an empty container (but not stateless) with retainment to another empty container") {
         auto pushed  = Many::FromMeta(nullptr, DataState::Missing);
         auto pushed2 = Many::FromMeta(nullptr, DataState {});
         auto result  = pushed2.SmartPush(IndexBack, pushed);

         REQUIRE(result == 1);
         REQUIRE(pushed2.GetCount() == 0);
         REQUIRE(pushed2.GetState() == DataState::Missing);
      }

      WHEN("Smart pushing to an empty container (concat & retain enabled)") {
         Many pushed;
         auto result = pushed.SmartPush(IndexBack, pack);

         REQUIRE(pushed == pack);
         REQUIRE(result == 1);
      }

      WHEN("Smart pushing to a different container with retain enabled") {
         Many pushed;
         pushed << 666;
         pushed.MakeOr();
         auto result = pushed.SmartPush(IndexBack, '?');

         REQUIRE(result == 1);
         REQUIRE(pushed.IsOr());
         REQUIRE(!pushed.As<Many>(0).IsOr());
         REQUIRE(!pushed.As<Many>(1).IsOr());
      }

      WHEN("ForEachDeep with dense flat element (immutable, skipping)") {
         int it = 1;
         Count total = 0;
         const auto iterated = pack.ForEachDeep(
            [&](Conditional<CT::Sparse<E>, E, const E&> i) {
               REQUIRE(DenseCast(i) == it);
               ++total;
               if (++it == 11)
                  it = 1;
            }
         );

         REQUIRE(it == 1);
         REQUIRE(total == 20);
         REQUIRE(total == iterated);
      }

      WHEN("ForEachDeep with dense flat element (mutable, skipping)") {
         int it = 1;
         Count total = 0;
         const auto iterated = pack.ForEachDeep(
            [&](Conditional<CT::Sparse<E>, E, E&> i) {
               REQUIRE(DenseCast(i) == it);
               ++total;
               if (++it == 11)
                  it = 1;
            }
         );

         REQUIRE(it == 1);
         REQUIRE(total == 20);
         REQUIRE(total == iterated);
      }

      WHEN("ForEachDeep with dense flat element (immutable, non-skipping)") {
         int it = 1;
         Count total = 0;
         const auto iterated = pack.template ForEachDeep<false, false>(
            [&](Conditional<CT::Sparse<E>, E, const E&> i) {
               REQUIRE(DenseCast(i) == it);
               ++total;
               if (++it == 11)
                  it = 1;
            }
         );

         REQUIRE(it == 1);
         REQUIRE(total == 20);
         REQUIRE(total == iterated);
      }

      WHEN("ForEachDeep with dense flat element (mutable, non-skipping)") {
         int it = 1;
         Count total = 0;
         const auto iterated = pack.template ForEachDeep<false, false>(
            [&](Conditional<CT::Sparse<E>, E, E&> i) {
               REQUIRE(DenseCast(i) == it);
               ++total;
               if (++it == 11)
                  it = 1;
            }
         );

         REQUIRE(it == 1);
         REQUIRE(total == 20);
         REQUIRE(total == iterated);
      }

      WHEN("ForEachDeep with dense Block element (immutable, skipping)") {
         Count total = 0;
         const auto iterated = pack.ForEachDeep(
            [&](const Block<>& i) {
               (void)i;
               ++total;
            }
         );

         REQUIRE(total == 4);
         REQUIRE(total == iterated);
      }

      WHEN("ForEachDeep with dense Block element (mutable, skipping)") {
         Count total = 0;
         const auto iterated = pack.ForEachDeep(
            [&](Block<>& i) {
               (void)i;
               ++total;
            }
         );

         REQUIRE(total == 4);
         REQUIRE(total == iterated);
      }

      WHEN("ForEachDeep with dense Block element (immutable, non-skipping)") {
         Count total = 0;
         const auto iterated = pack.template ForEachDeep<false, false>(
            [&](const Block<>& i) {
               (void)i;
               ++total;
            }
         );

         REQUIRE(total == 6);
         REQUIRE(total == iterated);
      }

      WHEN("ForEachDeep with dense Block element (mutable, non-skipping)") {
         Count total = 0;
         const auto iterated = pack.template ForEachDeep<false, false>(
            [&](Block<>& i) {
               (void)i;
               ++total;
            }
         );

         REQUIRE(total == 6);
         REQUIRE(total == iterated);
      }
   }

   GIVEN("Any with some deep items for the purpose of optimization") {
      Many pack;
      Many subpack1;
      Many subpack2;
      Many subpack3;
      subpack1 << darray[0] << darray[1] << darray[2] << darray[3] << darray[4];
      subpack2 << darray[5] << darray[6] << darray[7] << darray[8] << darray[9];
      subpack3 << subpack1;
      subpack3.MakeOr();
      pack << subpack1 << subpack2 << subpack3;

      WHEN("The container is optimized") {
         pack.Optimize();

         REQUIRE(pack.GetCount() == 3);
         REQUIRE(pack.As<Many>(0) == subpack1);
         REQUIRE(pack.As<Many>(1) == subpack2);
         REQUIRE(pack.As<Many>(2) == subpack1);
         REQUIRE(pack.GetUses() == 1);
         REQUIRE(subpack1.GetUses() == 3);
         REQUIRE(subpack2.GetUses() == 2);
         REQUIRE(subpack3.GetUses() == 1);
      }
   }

   for (auto& i : darray)
      DestroyElement<true>(i);

   REQUIRE(memoryState.Assert());

   // Destroy BANK before static data - otherwise problems happen if    
   // not using managed reflection                                      
   BANK.Reset();

   REQUIRE_FALSE(Allocator::CollectGarbage());
}

TEMPLATE_TEST_CASE("Deep sequential containers 2", "[any]", int, RT, int*, RT*) {
   static Allocator::State memoryState;
   static_assert(sizeof(A::Block) == sizeof(Block<>));
   using E = TestType;

   const E darray[10] {
      CreateElement<E, true>(1),
      CreateElement<E, true>(2),
      CreateElement<E, true>(3),
      CreateElement<E, true>(4),
      CreateElement<E, true>(5),
      CreateElement<E, true>(6),
      CreateElement<E, true>(7),
      CreateElement<E, true>(8),
      CreateElement<E, true>(9),
      CreateElement<E, true>(10)
   };

   GIVEN("Any with some deep items, and their Blocks coalesced") {
      Many pack;
      Many subpack1;
      Many subpack2;
      Many subpack3;
      subpack1 << darray[0] << darray[1] << darray[2] << darray[3] << darray[4];
      subpack2 << darray[5] << darray[6] << darray[7] << darray[8] << darray[9];
      subpack3 << subpack1;
      subpack3.MakeOr();
      pack << subpack1 << subpack2 << subpack3;

      auto baseRange = Many::From<Block<>>();
      baseRange.Reserve(3);

      for (Count e = 0; e < pack.GetCount(); ++e) {
         auto element = pack.GetElement(e);
         RTTI::Base base;
         REQUIRE(element.GetType()->GetBase<Block<>>(0, base));
         auto baseBlock = element.GetBaseMemory(MetaOf<Block<>>(), base);
         baseRange.InsertBlock(IndexBack, baseBlock);
      }

      WHEN("The Block bases from the subpacks are coalesced in a single container") {
         REQUIRE(pack.GetUses() == 1);
         REQUIRE(subpack1.GetUses() == 3);
         REQUIRE(subpack2.GetUses() == 2);
         REQUIRE(subpack3.GetUses() == 2);
      }

      WHEN("The coalesced Block bases are freed") {
         baseRange.Reset();

         REQUIRE(pack.GetUses() == 1);
         REQUIRE(subpack1.GetUses() == 3);
         REQUIRE(subpack2.GetUses() == 2);
         REQUIRE(subpack3.GetUses() == 2);
      }

      WHEN("The master pack is freed") {
         pack.Reset();

         REQUIRE(pack.GetUses() == 0);
         REQUIRE(subpack1.GetUses() == 2);
         REQUIRE(subpack2.GetUses() == 1);
         REQUIRE(subpack3.GetUses() == 1);
      }
   }

   for (auto& i : darray)
      DestroyElement<true>(i);

   REQUIRE(memoryState.Assert());

   // Destroy BANK before static data - otherwise problems happen if    
   // not using managed reflection                                      
   BANK.Reset();

   REQUIRE_FALSE(Allocator::CollectGarbage());
}

SCENARIO("Test BlockCast", "[block]") {
   Block<> from {};
   const Block<> fromc {};

   static_assert(CT::Exact<decltype(BlockCast<Text>(from)), Text&>);
   static_assert(CT::Exact<decltype(BlockCast<Text>(fromc)), const Text&>);
   static_assert(CT::Exact<decltype(BlockCast<Text>(Block<> {})), Text&>);

   static_assert(CT::Exact<decltype(BlockCast<const Text>(from)), Text&>);
   static_assert(CT::Exact<decltype(BlockCast<const Text>(fromc)), const Text&>);
   static_assert(CT::Exact<decltype(BlockCast<const Text>(Block<> {})), Text&>);
}