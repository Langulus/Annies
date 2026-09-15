///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include <Langulus/Annies/THive.hpp>
#include "Common.hpp"


struct Producible : Referenced {
   int v;

   Producible(int vv) : v {vv} {}

   bool operator == (const Producible& a) const noexcept {
      return v == a.v;
   }
};

SCENARIO("Test hives", "[hive]") {
   static Allocator::State memoryState;

   const Producible one {1};
   const Producible two {2};

	GIVEN("A hive instance") {
		THive<Producible> hive;

		WHEN("Default-constructed") {
			REQUIRE(hive.GetReusable() == nullptr);
			REQUIRE(hive.IsEmpty());
			REQUIRE(hive.GetType() == MetaOf<Producible>());
			REQUIRE(hive.GetCount() == 0);
		}

		WHEN("Two elements produced") {
         auto p1 = hive.New(1);
         auto p2 = hive.New(2);

         REQUIRE(p1);
         REQUIRE(p2);

			REQUIRE(hive.GetCount() == 2);
			REQUIRE(hive.GetFrames().GetCount() == 1);
         REQUIRE(hive.GetReusable() == hive.GetFrames()[0].GetRaw() + 2);
         REQUIRE(hive.GetType() == MetaOf<Producible>());
			REQUIRE(hive.GetFrames()[0].GetRaw()[0].mData == one);
			REQUIRE(hive.GetFrames()[0].GetRaw()[1].mData == two);
		}

		WHEN("30 elements produced") {
         for (int i = 1; i <= 30; ++i)
            REQUIRE(hive.New(i));

			REQUIRE(hive.GetCount() == 30);
			REQUIRE(hive.GetFrames().GetCount() == 2);
         //REQUIRE(hive.GetReusable() == nullptr);
         REQUIRE(hive.GetType() == MetaOf<Producible>());

         for (Count i = 0; i < hive.GetFrames()[0].GetReserved(); ++i) {
            auto v = hive.GetFrames()[0].GetRaw()[i].mData;
            REQUIRE(v.v == i + 1);
         }

         for (Count i = hive.GetFrames()[0].GetReserved(); i < hive.GetFrames()[0].GetReserved() + hive.GetFrames()[1].GetReserved() - 1; ++i) {
            auto v = hive.GetFrames()[1].GetRaw()[i - hive.GetFrames()[0].GetReserved()].mData;
            REQUIRE(v.v == i + 1);
         }

         THEN("Iterated using range") {
            int counter = 0;
            for (auto& i : hive) {
               ++counter;
               REQUIRE(i == counter);
            }
            REQUIRE(counter == 30);
         }
		}
   }

   const_cast<Producible&>(one).Reference(-1);
   const_cast<Producible&>(two).Reference(-1);

   REQUIRE(memoryState.Assert());

   // Destroy BANK before static data - otherwise problems happen if    
   // not using managed reflection                                      
   BANK.Reset();

   REQUIRE_FALSE(Allocator::CollectGarbage());
}
