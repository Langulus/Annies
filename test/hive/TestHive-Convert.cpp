///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "TestHiveCommon.hpp"


SCENARIO("Converting hive") {
   static MemoryState memoryState;

   GIVEN("An empty container") {
      THive<DMeta> pack2;

      WHEN("Converted to one Text using a templated destination") {
         TAny<Text> converted1, converted2;
         pack2.ConvertTo(converted2);

         Any_CheckState_Default<Text>(converted1);
         Any_CheckState_Default<Text>(converted2);
      }

      WHEN("Converted to many Texts using a templated destination") {
         TMany<Text> converted1, converted2;
         pack2.ConvertTo(converted2);

         Many_CheckState_Default<Text>(converted1);
         Many_CheckState_Default<Text>(converted2);
      }
   }

   GIVEN("A container with one meta data") {
      THive<DMeta> pack2 {MetaDataOf<double>()};

      WHEN("Converted to one Text using a templated destination") {
         TAny<Text> converted1, converted2;
         pack2.ConvertTo(converted2);

         Any_CheckState_OwnedFull<Text>(converted1);
         Any_CheckState_OwnedFull<Text>(converted2);
         REQUIRE(converted1 == converted2);
         REQUIRE(converted1.GetCount() == 1);
         REQUIRE(*converted1 == "Double");
      }

      WHEN("Converted to many Texts using a templated destination") {
         TMany<Text> converted1, converted2;
         pack2.ConvertTo(converted2);

         Many_CheckState_OwnedFull<Text>(converted1);
         Many_CheckState_OwnedFull<Text>(converted2);
         REQUIRE(converted1 == converted2);
         REQUIRE(converted1.GetCount() == 1);
         REQUIRE(*converted1 == "Double");
      }
   }

   GIVEN("A container with multiple meta datas") {
      THive<DMeta> pack2 {MetaDataOf<double>(), MetaDataOf<ConvertibleToInt>(), MetaDataOf<float>()};

      WHEN("Converted to one Text using a templated destination") {
         TAny<Text> converted1, converted2;
         pack2.ConvertTo(converted2);

         Any_CheckState_OwnedFull<Text>(converted1);
         Any_CheckState_OwnedFull<Text>(converted2);
         REQUIRE(converted1 == converted2);
         REQUIRE(converted1.GetCount() == 1);
         REQUIRE(*converted1 == "Double");
      }

      WHEN("Converted to many Texts using a templated destination") {
         TMany<Text> converted1, converted2;
         pack2.ConvertTo(converted2);

         Many_CheckState_OwnedFull<Text>(converted1);
         Many_CheckState_OwnedFull<Text>(converted2);
         REQUIRE(converted1 == converted2);
         REQUIRE(converted1.GetCount() == 3);
         REQUIRE(converted1[0] == "Double");
         REQUIRE(converted1[1] == "ConvertibleToInt");
         REQUIRE(converted1[2] == "Float");
      }
   }

   GIVEN("A container with a single ConvertibleToInt") {
      THive<ConvertibleToInt> pack2 = ConvertibleToInt{};

      WHEN("Converted to one integer using a templated destination") {
         TAny<int> converted1, converted2;
         pack2.ConvertTo(converted2);

         Any_CheckState_OwnedFull<int>(converted1);
         Any_CheckState_OwnedFull<int>(converted2);
         REQUIRE(converted1 == converted2);
         REQUIRE(converted1.GetCount() == 1);
         REQUIRE(*converted1 == 666);
      }

      WHEN("Converted to many integers using a templated destination") {
         TMany<int> converted1, converted2;
         pack2.ConvertTo(converted2);

         Many_CheckState_OwnedFull<int>(converted1);
         Many_CheckState_OwnedFull<int>(converted2);
         REQUIRE(converted1 == converted2);
         REQUIRE(converted1.GetCount() == 1);
         REQUIRE(*converted1 == 666);
      }
   }

   GIVEN("A container with multiple ConvertibleToInt") {
      THive<ConvertibleToInt> pack2 {ConvertibleToInt{1}, ConvertibleToInt{2}, ConvertibleToInt{3}};

      WHEN("Converted to one integer using a templated destination") {
         TAny<int> converted1, converted2;
         pack2.ConvertTo(converted2);

         Any_CheckState_OwnedFull<int>(converted1);
         Any_CheckState_OwnedFull<int>(converted2);
         REQUIRE(converted1 == converted2);
         REQUIRE(converted1.GetCount() == 1);
         REQUIRE(*converted1 == 1);
      }

      WHEN("Converted to many integers using a templated destination") {
         TMany<int> converted1, converted2;
         pack2.ConvertTo(converted2);

         Many_CheckState_OwnedFull<int>(converted1);
         Many_CheckState_OwnedFull<int>(converted2);
         REQUIRE(converted1 == converted2);
         REQUIRE(converted1.GetCount() == 3);
         REQUIRE(converted1[0] == 1);
         REQUIRE(converted1[1] == 2);
         REQUIRE(converted1[2] == 3);
      }
   }

   REQUIRE(memoryState.Assert());
   REQUIRE_FALSE(Allocator::CollectGarbage());
}
