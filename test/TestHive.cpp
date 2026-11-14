///                                                                           
/// Langulus::Annies                                                           
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include <Langulus/Anyness/THive.hpp>
#include "Common.hpp"


struct Producible : Referenced {
   int v;

   Producible(int vv) : v {vv} {}

   bool operator == (const Producible& a) const noexcept {
      return v == a.v;
   }

   explicit operator bool() const noexcept { return v != 0; }
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
         auto p1 = hive.Emplace(1);
         auto p2 = hive.Emplace(2);

         REQUIRE(p1);
         REQUIRE(p2);

         REQUIRE(hive.GetCount() == 2);
         REQUIRE(hive.GetFrames() == 1);
         REQUIRE(hive.GetReusable() == hive.GetFrame(0) + 2*sizeof(Producible));
         REQUIRE(hive.GetType() == MetaOf<Producible>());
         REQUIRE(*reinterpret_cast<Producible*>(hive.GetFrame(0)) == one);
         REQUIRE(*reinterpret_cast<Producible*>(hive.GetFrame(0) + sizeof(Producible)) == two);
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
