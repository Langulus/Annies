///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "TestAnyCommon.hpp"


SCENARIO("Converting Any/TAny") {
   static MemoryState memoryState;

   GIVEN("An empty container") {
      Any         pack1;
      TAny<DMeta> pack2;

      WHEN("Converted to texts using a templated destination") {
         TAny<Text> converted1, converted2;
         pack1.ConvertTo(converted1);
         pack2.ConvertTo(converted2);

         Any_CheckState_Default<Text>(converted1);
         Any_CheckState_Default<Text>(converted2);
      }
   }

   GIVEN("A container with meta data") {
      Any         pack1 {MetaDataOf<double>()};
      TAny<DMeta> pack2 {MetaDataOf<double>()};

      WHEN("Converted to texts using a templated destination") {
         TAny<Text> converted1, converted2;
         pack1.ConvertTo(converted1);
         pack2.ConvertTo(converted2);

         Any_CheckState_OwnedFull<Text>(converted1);
         Any_CheckState_OwnedFull<Text>(converted2);
         REQUIRE(converted1 == converted2);
         REQUIRE(converted1.GetCount() == 1);
         REQUIRE(*converted1 == "Double");
      }
   }

   GIVEN("A container with ConvertibleToInt") {
      Any                    pack1 = ConvertibleToInt{};
      TAny<ConvertibleToInt> pack2 = ConvertibleToInt{};

      WHEN("Converted to integers using a templated destination") {
         TAny<int> converted1, converted2;
         pack1.ConvertTo(converted1);
         pack2.ConvertTo(converted2);

         Any_CheckState_OwnedFull<int>(converted1);
         Any_CheckState_OwnedFull<int>(converted2);
         REQUIRE(converted1 == converted2);
         REQUIRE(converted1.GetCount() == 1);
         REQUIRE(*converted1 == 666);
      }
   }

   REQUIRE(memoryState.Assert());
   REQUIRE_FALSE(Allocator::CollectGarbage());
}
