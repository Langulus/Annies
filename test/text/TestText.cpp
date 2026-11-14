///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "../Main.hpp"

using namespace Langulus;
using Anyness::Text;
using Anyness::operator""_text;


TEST_CASE_TEMPLATE("Testing text containers", T,
   Text
   //TODO Path
) {
   static MemoryState memoryState;

   GIVEN("Uninitialized text container") {
      T* text = nullptr;
      prevent_optimization(text);

      WHEN("Constructed with a count-terminated string") {
         text = new T {Text::FromText("test2", 2)};

         Text_CheckState_DisownedFullConst(*text);
         REQUIRE((*text).GetCount() == 2);
         REQUIRE((*text) == "te");
         REQUIRE((*text)[0] == 't');
         REQUIRE((*text)[1] == 'e');
         IF_SAFE(REQUIRE_THROWS((*text)[2] == 's'));
         IF_SAFE(REQUIRE_THROWS((*text)[3] == 't'));
         IF_SAFE(REQUIRE_THROWS((*text)[4] == '2'));
         IF_SAFE(REQUIRE_THROWS((*text)[5] == '?'));
      }

      WHEN("Constructed with a bounded array string") {
         char test1[] = "test3";
         text = new T {test1};

         Text_CheckState_OwnedFull(*text);
         REQUIRE((*text).GetCount() == 5);
         REQUIRE((*text).GetReserved() >= 5);
         REQUIRE((*text) == "test3");
         REQUIRE((*text)[0] == 't');
         REQUIRE((*text)[1] == 'e');
         REQUIRE((*text)[2] == 's');
         REQUIRE((*text)[3] == 't');
         REQUIRE((*text)[4] == '3');
         IF_SAFE(REQUIRE_THROWS((*text)[5] == '?'));
      }

      WHEN("Constructed with a nullptr_t") {
         text = new T {nullptr};

         Text_CheckState_Default(*text);
      }

      WHEN("Constructed with a nullptr c-string") {
         text = new T {(char*)nullptr};

         Text_CheckState_Default(*text);
      }

      WHEN("Constructed with empty c-string") {
         text = new T {""};

         Text_CheckState_Default(*text);
      }

      delete text;
   }

   REQUIRE(memoryState.Assert());
   REQUIRE_FALSE(Allocator::CollectGarbage());
}

TEST_CASE_TEMPLATE("Unsigned number stringification", TestType,
   uint8_t, uint16_t, uint32_t, uint64_t
) {
   static MemoryState memoryState;

   WHEN("Constructed Text with a number") {
      Text* text = new Text {Text::FromNumber(TestType{66})};

      Text_CheckState_OwnedFull(*text);
      REQUIRE((*text).GetCount() == 2);
      REQUIRE((*text).GetReserved() >= 2);
      REQUIRE((*text) == "66");

      delete text;
   }

   /*WHEN("Constructed Path with a number") {
      Path* text = new Path {TestType{66}};

      REQUIRE((*text).GetCount() == 2);
      REQUIRE((*text).GetReserved() >= 2);
      REQUIRE((*text).Is<char>());
      REQUIRE((*text).GetRaw());
      REQUIRE((*text).GetAllocation());
      REQUIRE((*text) == "66");

      delete text;
   }*/

   REQUIRE(memoryState.Assert());
   REQUIRE_FALSE(Allocator::CollectGarbage());
}

TEST_CASE_TEMPLATE("Signed number stringification", TestType,
   int8_t, int16_t, int32_t, int64_t
) {
   static MemoryState memoryState;

   WHEN("Constructed Text with a number") {
      Text* text = new Text {Text::FromNumber(TestType{-66})};
      
      Text_CheckState_OwnedFull(*text);
      REQUIRE((*text).GetCount() == 3);
      REQUIRE((*text).GetReserved() >= 3);
      REQUIRE((*text) == "-66");

      delete text;
   }

   /*WHEN("Constructed Path with a number") {
      Path* text = new Path {TestType{-66}};

      REQUIRE((*text).GetCount() == 3);
      REQUIRE((*text).GetReserved() >= 3);
      REQUIRE((*text).Is<char>());
      REQUIRE((*text).GetRaw());
      REQUIRE((*text).GetAllocation());
      REQUIRE((*text) == "-66");

      delete text;
   }*/

   REQUIRE(memoryState.Assert());
   REQUIRE_FALSE(Allocator::CollectGarbage());
}

TEST_CASE_TEMPLATE("Real number stringification", TestType,
   float, double
) {
   static MemoryState memoryState;

   WHEN("Constructed Text with a number") {
      Text* text = new Text {Text::FromNumber(TestType{-66.666}, 2)};

      Text_CheckState_OwnedFull(*text);
      REQUIRE((*text).GetCount() == 7);
      REQUIRE((*text).GetReserved() >= 7);
      REQUIRE((*text) == "~-66.67");

      delete text;
   }

   /*WHEN("Constructed Path with a number") {
      Path* text = new Path {TestType{-66}};

      REQUIRE((*text).GetCount() == 3);
      REQUIRE((*text).GetReserved() >= 3);
      REQUIRE((*text).Is<char>());
      REQUIRE((*text).GetRaw());
      REQUIRE((*text).GetAllocation());
      REQUIRE((*text) == "-66");

      delete text;
   }*/

   REQUIRE(memoryState.Assert());
   REQUIRE_FALSE(Allocator::CollectGarbage());
}

TEST_CASE_TEMPLATE("Logging text containers", TestType,
   Text/*TODO , Path*/
) {
   static MemoryState memoryState;

   WHEN("Logging") {
      TestType text {"some text"};
      Logger::Info("You should also see ", text);
   }

   WHEN("Logging literal") {
      Logger::Info("You should also see ", "some text"_text);
   }

   REQUIRE(memoryState.Assert());
   REQUIRE_FALSE(Allocator::CollectGarbage());
}

TEST_CASE_TEMPLATE("Reflected coverters to text", TestType,
   Stringifiable, StringifiableConst
) {
   static MemoryState memoryState;

   GIVEN("A stringifiable type") {
      const auto debugMeta = MetaOf<Text>();
      const auto meta = MetaOf<TestType>();
      TestType instance;

      WHEN("Converted") {
         // @attention calling static_cast<Text>(Stringifiable) won't   
         // work on MSVC due to a compiler bug with mutable cast        
         // operators                                                   
         const auto staticallyConverted = instance.operator Text();
         Text rttiConverted;
         meta.GetMorphism(debugMeta).convert(&instance, &rttiConverted);

         REQUIRE(staticallyConverted == rttiConverted);
         if constexpr (Akin<Stringifiable, TestType>)
            REQUIRE(staticallyConverted == "Stringifiable converted to Text");
         else
            REQUIRE(staticallyConverted == "StringifiableConst converted to Text");            
      }
   }

   REQUIRE(memoryState.Assert());
   REQUIRE_FALSE(Allocator::CollectGarbage());
}

/*TEMPLATE_TEST_CASE("Text container interoperability", "[text]",
   (TypePair<Path, Text>),
   (TypePair<Text, Path>)
) {
   static Allocator::State memoryState;

   using LHS = typename TestType::LHS;
   using RHS = typename TestType::RHS;

   GIVEN("Two types of text containers") {
      WHEN("Constructed") {
         LHS text {RHS{"one"}};

         REQUIRE(text == "one");
      }

      WHEN("Assigned") {
         LHS text {"one"};
         text = RHS {"two"};

         REQUIRE(text == "two");
      }

      WHEN("Concatenated (destructively)") {
         LHS text {"one"};
         text += RHS {"two"};

         REQUIRE(text == "onetwo");
      }

      WHEN("Concatenated") {
         LHS text {"one"};
         LHS text2 = text + RHS {"two"};

         REQUIRE(text == "one");
         REQUIRE(text2 == "onetwo");
      }
   }

   REQUIRE(memoryState.Assert());

   // Destroy BANK before static data - otherwise problems happen if    
   // not using managed reflection                                      
   BANK.Reset();

   REQUIRE_FALSE(Allocator::CollectGarbage());
}*/

/*TEMPLATE_TEST_CASE("Text container conversion at runtime", "[text]",
   (TypePair<Text, Path>)
) {
   static Allocator::State memoryState;

   using LHS = typename TestType::LHS;
   using RHS = typename TestType::RHS;

   GIVEN("Two types of text containers") {
      WHEN("Reflected") {
         auto lhs = MetaDataOf<LHS>();
         auto rhs = MetaDataOf<RHS>();

         REQUIRE(lhs->GetConverter(rhs) != nullptr);
      }
   }

   REQUIRE(memoryState.Assert());

   // Destroy BANK before static data - otherwise problems happen if    
   // not using managed reflection                                      
   BANK.Reset();

   REQUIRE_FALSE(Allocator::CollectGarbage());
}*/

/*TEMPLATE_TEST_CASE("Containing literals", "[text]",
   Many, Tag
) {
   static Allocator::State memoryState;

   GIVEN("Two types of text containers") {
      WHEN("Constructed") {
         TestType text {"one"};

         REQUIRE(text.GetCount() == 1);
         REQUIRE(text.template IsExact<Text>());
         REQUIRE(text.template As<Text>() == "one");
      }

      WHEN("Assigned") {
         TestType text {"one"};
         text = "two";

         REQUIRE(text.GetCount() == 1);
         REQUIRE(text.template IsExact<Text>());
         REQUIRE(text.template As<Text>() == "two");
      }

      WHEN("Concatenated (destructively)") {
         TestType text {"one"};
         text += TestType {"two"};

         REQUIRE(text.GetCount() == 2);
         REQUIRE(text.template IsExact<Text>());
         REQUIRE(text.template AsAt<Text>(0) == "one");
         REQUIRE(text.template AsAt<Text>(1) == "two");
      }

      WHEN("Concatenated") {
         TestType text {"one"};
         TestType text2 = text + TestType {"two"};

         REQUIRE(text.GetCount() == 1);
         REQUIRE(text2.GetCount() == 2);
         REQUIRE(text.template IsExact<Text>());
         REQUIRE(text2.template IsExact<Text>());
         REQUIRE(text.template As<Text>() == "one");
         REQUIRE(text2.template AsAt<Text>(0) == "one");
         REQUIRE(text2.template AsAt<Text>(1) == "two");
      }
   }

   REQUIRE(memoryState.Assert());
   REQUIRE_FALSE(Allocator::CollectGarbage());
}*/
