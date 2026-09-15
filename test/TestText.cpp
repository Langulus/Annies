///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include <Langulus/Annies/Text.hpp>
#include <Langulus/Annies/Path.hpp>
#include <Langulus/Annies/Trait.hpp>
#include "Common.hpp"


/// A type that is reflected, as convertible to Text                          
/*struct Stringifiable {
   LANGULUS_CONVERTS_TO(Text);
   explicit operator Text() { return "Stringifiable converted to Text"; }
};*/
// TODO: this causes MSVC to ICE since 19.40.33811.0 :(
// Cast operators must ALWAYS be const!

/// A type that is reflected, as convertible to Text                          
struct StringifiableConst {
   LANGULUS_CONVERTS_TO(Text);
   
   explicit operator Text() const {
      return "Stringifiable converted to Text";
   }
};


///                                                                           
/// Possible states:                                                          
///   - uninitialized                                                         
///   - default                                                               
void Text_CheckState_Default(const Text&);
///   - invariant                                                             
void Text_CheckState_Invariant(const Text&);
///   - owned-full                                                            
void Text_CheckState_OwnedFull(const Text&);
///   - owned-full-const                                                      
void Text_CheckState_OwnedFullConst(const Text&);
///   - owned-empty                                                           
void Text_CheckState_OwnedEmpty(const Text&);
///   - disowned-full                                                         
void Text_CheckState_DisownedFull(const Text&);
///   - disowned-full-const                                                   
void Text_CheckState_DisownedFullConst(const Text&);
///   - abandoned                                                             
void Text_CheckState_Abandoned(const Text&);

///                                                                           
/// Possible actions for each state:                                          
///   - uninitialized                                                         
///      - default-initialized                                                
///      - intent-initialized from container                                  
///      - intent-initialized from dense letter                               
///      - intent-initialized from dense std::string                          
///      - intent-initialized from dense std::string_view                     
///      - intent-initialized from dense number (stringification)             
///      - intent-initialized from sparse meta (stringification)              
///      - intent-initialized from exception (stringification)                
///      - intent-initialized from sparse element, zero-terminated            
///      - intent-initialized from sparse element, bound-terminated           
///      - intent-initialized from sparse element, count-terminated           

TEMPLATE_TEST_CASE("Testing text containers", "[text]",
   Text, Path
) {
   static Allocator::State memoryState;

   GIVEN("Default text container") {
      TestType text;

      Text_CheckState_Default(text);
      REQUIRE_FALSE(text.IsConstant());

      WHEN("Capacity is reserved") {
         text.Reserve(500);

         Text_CheckState_OwnedEmpty(text);
         REQUIRE_FALSE(text.IsConstant());
         REQUIRE(text.GetReserved() >= 500);
      }

      WHEN("Directly assigned to itself") {
         text = text;

         Text_CheckState_Default(text);
         REQUIRE_FALSE(text.IsConstant());
      }

      WHEN("Indirectly assigned to itself") {
         const auto anothertext = text;
         text = anothertext;

         Text_CheckState_Default(text);
         REQUIRE_FALSE(text.IsConstant());
      }
   }

   GIVEN("Uninitialized text container") {
      TestType* text = nullptr;

      WHEN("Constructed with a null-terminated literal") {
         text = new TestType {"test1"};

         Text_CheckState_OwnedFull(*text);
         REQUIRE((*text).GetCount() == 5);
         REQUIRE((*text).GetReserved() >= 5);
         REQUIRE((*text) == "test1");
         REQUIRE((*text)[0] == 't');
         REQUIRE((*text)[1] == 'e');
         REQUIRE((*text)[2] == 's');
         REQUIRE((*text)[3] == 't');
         REQUIRE((*text)[4] == '1');
         REQUIRE_THROWS((*text)[5] == '?');
      }

      WHEN("Constructed with a count-terminated literal") {
         text = new TestType {Text::From("test2", 5)};

         Text_CheckState_DisownedFullConst(*text);
         REQUIRE((*text).GetCount() == 5);
         REQUIRE((*text).GetReserved() >= 5);
         REQUIRE((*text) == "test2");
         REQUIRE((*text)[0] == 't');
         REQUIRE((*text)[1] == 'e');
         REQUIRE((*text)[2] == 's');
         REQUIRE((*text)[3] == 't');
         REQUIRE((*text)[4] == '2');
         REQUIRE_THROWS((*text)[5] == '?');
      }

      WHEN("Constructed with a bounded literal") {
         char test1[] = "test3";
         text = new TestType {test1};

         Text_CheckState_OwnedFull(*text);
         REQUIRE((*text).GetCount() == 5);
         REQUIRE((*text).GetReserved() >= 5);
         REQUIRE((*text) == "test3");
         REQUIRE((*text)[0] == 't');
         REQUIRE((*text)[1] == 'e');
         REQUIRE((*text)[2] == 's');
         REQUIRE((*text)[3] == 't');
         REQUIRE((*text)[4] == '3');
         REQUIRE_THROWS((*text)[5] == '?');
      }

      WHEN("Constructed with a nullptr_t") {
         text = new TestType {nullptr};

         Text_CheckState_Default(*text);
      }

      WHEN("Constructed with a nullptr c-array") {
         text = new TestType {(char*)nullptr};

         Text_CheckState_Default(*text);
      }

      WHEN("Constructed with empty c-array") {
         text = new TestType {""};

         Text_CheckState_Default(*text);
      }

      WHEN("Constructed with a single character") {
         text = new TestType {'?'};

         Text_CheckState_OwnedFull(*text);
         REQUIRE((*text).GetCount() == 1);
         REQUIRE((*text).GetReserved() >= 1);
         REQUIRE((*text)[0] == '?');
         REQUIRE_THROWS((*text)[1] == '?');
      }

      if (text)
         delete text;
   }

   GIVEN("Reserved text container") {
      TestType text;
      text.Reserve(500);
      auto memory = text.GetRaw();

      WHEN("Text is extended") {
         auto region = text.Extend(10);

         REQUIRE(text.GetCount() == 10);
         REQUIRE(text.GetReserved() >= 500);
         REQUIRE(text.GetRaw() == memory);
         REQUIRE(text.GetAllocation());
         REQUIRE(region.GetCount() == 10);
         REQUIRE(region.GetRaw() == memory);
      }

      WHEN("Text is concatenated") {
         text += "test";

         REQUIRE(text.GetCount() == 4);
         REQUIRE(text.GetReserved() >= 500);
         REQUIRE(text.GetRaw() == memory);
         REQUIRE(text.GetAllocation());
         REQUIRE(text == "test");
      }

      WHEN("Text is cleared") {
         text += "test";
         text.Clear();

         REQUIRE(text.GetCount() == 0);
         REQUIRE(text.GetReserved() >= 500);
         REQUIRE(text.GetRaw() == memory);
         REQUIRE(text.GetAllocation());
         REQUIRE(text != "test");
      }

      WHEN("Text is reset") {
         text += "test";
         text.Reset();

         REQUIRE(text.GetCount() == 0);
         REQUIRE(text.GetReserved() == 0);
         REQUIRE(text.GetRaw() == nullptr);
         REQUIRE(text.GetType() == MetaOf<Letter>());
         REQUIRE_FALSE(text.GetAllocation());
         REQUIRE(text != "test");
      }
   }

   GIVEN("Full text container") {
      TestType text {"test1"};
      auto memory = text.GetRaw();

      WHEN("Add more text") {
         text += "test2";

         REQUIRE(text == "test1test2");
         REQUIRE(text.GetCount() == 10);
         REQUIRE(text.GetReserved() >= 10);
         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            REQUIRE(text.GetRaw() == memory);
         #endif
         REQUIRE(text.GetAllocation());
         REQUIRE(text.template Is<Letter>());
      }

      WHEN("More capacity is reserved") {
         text.Reserve(20);

         REQUIRE(text.GetCount() == 5);
         REQUIRE(text.GetReserved() >= 20);
         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            REQUIRE(text.GetRaw() == memory);
         #endif
         REQUIRE(text.GetAllocation());
      }

      WHEN("More capacity is reserved, via Extend()") {
         auto region = text.Extend(10);

         REQUIRE(text.GetCount() == 15);
         REQUIRE(text.GetReserved() >= 15);
         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            REQUIRE(text.GetRaw() == memory);
         #endif
         REQUIRE(text.GetAllocation());
         REQUIRE(region.GetCount() == 10);
         REQUIRE(region.GetRaw() == text.GetRaw() + 5);
      }

      WHEN("Less capacity is reserved") {
         text.Reserve(2);

         REQUIRE(text.GetCount() == 2);
         REQUIRE(text.GetReserved() >= 5);
         REQUIRE(text.GetRaw() == memory);
         REQUIRE(text.GetAllocation());
      }

      WHEN("Text is cleared") {
         text.Clear();

         REQUIRE(text.GetCount() == 0);
         REQUIRE(text.GetReserved() >= 5);
         REQUIRE(text.GetRaw() == memory);
         REQUIRE(text.GetAllocation());
         REQUIRE(text.template Is<Letter>());
      }

      WHEN("Text is reset") {
         text.Reset();

         REQUIRE(text.GetCount() == 0);
         REQUIRE(text.GetReserved() == 0);
         REQUIRE_FALSE(text.GetRaw());
         REQUIRE(text.template Is<Letter>());
      }

      WHEN("Text is copied shallowly") {
         TestType copy = text;

         REQUIRE(text.GetCount() == copy.GetCount());
         REQUIRE(text.GetReserved() == copy.GetReserved());
         REQUIRE(text.GetRaw() == copy.GetRaw());
         REQUIRE(text.GetType() == copy.GetType());
         REQUIRE(text.GetAllocation());
         REQUIRE(copy.GetAllocation());
         REQUIRE(copy.GetUses() == 2);
         REQUIRE(text.GetUses() == 2);
      }

      WHEN("Text is cloned (deep copy)") {
         TestType copy = Clone(text);

         REQUIRE(text.GetCount() == copy.GetCount());
         REQUIRE(text.GetReserved() >= copy.GetReserved());
         REQUIRE(text.GetRaw() != copy.GetRaw());
         REQUIRE(text.GetType() == copy.GetType());
         REQUIRE(text.GetAllocation());
         REQUIRE(copy.GetAllocation());
         REQUIRE(copy.GetUses() == 1);
         REQUIRE(text.GetUses() == 1);
      }

      WHEN("Text is reset, then allocated again") {
         text.Reset();
         text += "kurec";

         REQUIRE(text.GetCount() == 5);
         REQUIRE(text.GetReserved() >= 5);
         REQUIRE(text.GetAllocation());
         REQUIRE(text.template Is<Letter>());
      }

      WHEN("Texts are compared") {
         REQUIRE(text == "test1");
         REQUIRE(text != "Tests");
      }
   }

   REQUIRE(memoryState.Assert());

   // Destroy BANK before static data - otherwise problems happen if    
   // not using managed reflection                                      
   BANK.Reset();

   REQUIRE_FALSE(Allocator::CollectGarbage());
}

TEMPLATE_TEST_CASE("Unsigned number stringification", "[text]",
   /*uint8_t,*/ uint16_t, uint32_t, uint64_t
) {
   static Allocator::State memoryState;

   WHEN("Constructed Text with a number") {
      Text* text = new Text {TestType{66}};

      REQUIRE((*text).GetCount() == 2);
      REQUIRE((*text).GetReserved() >= 2);
      REQUIRE((*text).template Is<Letter>());
      REQUIRE((*text).GetRaw());
      REQUIRE((*text).GetAllocation());
      REQUIRE((*text) == "66");

      delete text;
   }

   WHEN("Constructed Path with a number") {
      Path* text = new Path {TestType{66}};

      REQUIRE((*text).GetCount() == 2);
      REQUIRE((*text).GetReserved() >= 2);
      REQUIRE((*text).template Is<Letter>());
      REQUIRE((*text).GetRaw());
      REQUIRE((*text).GetAllocation());
      REQUIRE((*text) == "66");

      delete text;
   }

   REQUIRE(memoryState.Assert());

   // Destroy BANK before static data - otherwise problems happen if    
   // not using managed reflection                                      
   BANK.Reset();

   REQUIRE_FALSE(Allocator::CollectGarbage());
}

TEMPLATE_TEST_CASE("Signed number stringification", "[text]", int8_t, int16_t, int32_t, int64_t) {
   static Allocator::State memoryState;

   WHEN("Constructed Text with a number") {
      Text* text = new Text {TestType{-66}};

      REQUIRE((*text).GetCount() == 3);
      REQUIRE((*text).GetReserved() >= 3);
      REQUIRE((*text).template Is<Letter>());
      REQUIRE((*text).GetRaw());
      REQUIRE((*text).GetAllocation());
      REQUIRE((*text) == "-66");

      delete text;
   }

   WHEN("Constructed Path with a number") {
      Path* text = new Path {TestType{-66}};

      REQUIRE((*text).GetCount() == 3);
      REQUIRE((*text).GetReserved() >= 3);
      REQUIRE((*text).template Is<Letter>());
      REQUIRE((*text).GetRaw());
      REQUIRE((*text).GetAllocation());
      REQUIRE((*text) == "-66");

      delete text;
   }

   REQUIRE(memoryState.Assert());

   // Destroy BANK before static data - otherwise problems happen if    
   // not using managed reflection                                      
   BANK.Reset();

   REQUIRE_FALSE(Allocator::CollectGarbage());
}

TEMPLATE_TEST_CASE("Logging text containers", "[text]", Text, Path) {
   static Allocator::State memoryState;

   WHEN("Logging") {
      TestType text {"some text"};
      Logger::Info() << "You should see " << text;
      Logger::Info("You should also see ", text);
   }

   WHEN("Logging literal") {
      Logger::Info() << "You should see " << "some text"_text;
      Logger::Info("You should also see ", "some text"_text);
   }

   REQUIRE(memoryState.Assert());

   // Destroy BANK before static data - otherwise problems happen if    
   // not using managed reflection                                      
   BANK.Reset();

   REQUIRE_FALSE(Allocator::CollectGarbage());
}

TEMPLATE_TEST_CASE("Reflected coverters to text", "[text]", /*Stringifiable,*/ StringifiableConst) {
   static Allocator::State memoryState;

   GIVEN("A stringifiable type") {
      const auto debugMeta = MetaOf<Text>();
      const auto meta = MetaOf<TestType>();
      TestType instance;

      WHEN("Converted") {
         // Calling static_cast<Debug> here doesn't work, because of MSVC bug
         const auto staticallyConverted = instance.operator Text();
         
         Text rttiConverted;
         meta->GetConverter(debugMeta)(&instance, &rttiConverted);

         REQUIRE(staticallyConverted == rttiConverted);
         REQUIRE(staticallyConverted == "Stringifiable converted to Text");
      }
   }

   REQUIRE(memoryState.Assert());

   // Destroy BANK before static data - otherwise problems happen if    
   // not using managed reflection                                      
   BANK.Reset();

   REQUIRE_FALSE(Allocator::CollectGarbage());
}

TEMPLATE_TEST_CASE("Text container interoperability", "[text]",
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
}

TEMPLATE_TEST_CASE("Text container conversion at runtime", "[text]",
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
}

TEMPLATE_TEST_CASE("Containing literals", "[text]",
   Many, Trait
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
         REQUIRE(text.template As<Text>(0) == "one");
         REQUIRE(text.template As<Text>(1) == "two");
      }

      WHEN("Concatenated") {
         TestType text {"one"};
         TestType text2 = text + TestType {"two"};

         REQUIRE(text.GetCount() == 1);
         REQUIRE(text2.GetCount() == 2);
         REQUIRE(text.template IsExact<Text>());
         REQUIRE(text2.template IsExact<Text>());
         REQUIRE(text.template As<Text>() == "one");
         REQUIRE(text2.template As<Text>(0) == "one");
         REQUIRE(text2.template As<Text>(1) == "two");
      }
   }

   REQUIRE(memoryState.Assert());

   // Destroy BANK before static data - otherwise problems happen if    
   // not using managed reflection                                      
   BANK.Reset();

   REQUIRE_FALSE(Allocator::CollectGarbage());
}

void Text_CheckState_Default(const Text& text) {
   REQUIRE_FALSE(text.IsCompressed());
   REQUIRE_FALSE(text.IsConstant());
   REQUIRE_FALSE(text.IsDeep());
   REQUIRE_FALSE(text.IsSparse());
   REQUIRE_FALSE(text.IsEncrypted());
   REQUIRE_FALSE(text.IsMissing());
   REQUIRE_FALSE(text.IsOr());
   REQUIRE_FALSE(text.IsStatic());
   REQUIRE      (text.IsTyped());
   REQUIRE_FALSE(text.IsUntyped());
   REQUIRE_FALSE(text.IsValid());
   REQUIRE      (text.IsInvalid());
   REQUIRE_FALSE(text.IsAllocated());
   REQUIRE      (text.IsEmpty());
   REQUIRE_FALSE(text.GetAllocation());
   REQUIRE      (text.IsTypeConstrained());
   REQUIRE      (text.GetType() == MetaOf<Letter>());
   REQUIRE      (text.template Is<Letter>());
   REQUIRE      (text.IsNow());
   REQUIRE_FALSE(text.IsFuture());
   REQUIRE_FALSE(text.IsPast());
   REQUIRE      (text.IsDense());
   REQUIRE      (text.GetCount() == 0);
   REQUIRE      (text.GetReserved() == 0);
   REQUIRE      (text.GetUses() == 0);
   REQUIRE      (text.GetRaw() == nullptr);
   REQUIRE      (text == nullptr);
   REQUIRE_FALSE(text != nullptr);
   REQUIRE      (text == (char*)nullptr);
   REQUIRE_FALSE(text != (char*)nullptr);
   REQUIRE      (not text);
   REQUIRE_FALSE(text);
   REQUIRE      (text == "");
   REQUIRE_FALSE(text != "");
   REQUIRE_FALSE(text == "no match");
}

void Text_CheckState_OwnedEmpty(const Text& text) {
   REQUIRE_FALSE(text.IsCompressed());
   REQUIRE_FALSE(text.IsConstant());
   REQUIRE_FALSE(text.IsDeep());
   REQUIRE_FALSE(text.IsSparse());
   REQUIRE_FALSE(text.IsEncrypted());
   REQUIRE_FALSE(text.IsMissing());
   REQUIRE_FALSE(text.IsOr());
   REQUIRE_FALSE(text.IsStatic());
   REQUIRE      (text.IsTyped());
   REQUIRE_FALSE(text.IsUntyped());
   REQUIRE_FALSE(text.IsValid());
   REQUIRE      (text.IsInvalid());
   REQUIRE      (text.IsAllocated());
   REQUIRE      (text.IsEmpty());
   REQUIRE      (text.GetAllocation());
   REQUIRE      (text.IsTypeConstrained());
   REQUIRE      (text.GetType() == MetaOf<Letter>());
   REQUIRE      (text.template Is<Letter>());
   REQUIRE      (text.IsNow());
   REQUIRE_FALSE(text.IsFuture());
   REQUIRE_FALSE(text.IsPast());
   REQUIRE      (text.IsDense());
   REQUIRE      (text.GetCount() == 0);
   REQUIRE      (text.GetReserved() > 0);
   REQUIRE      (text.GetUses() == 1);
   REQUIRE      (text.GetRaw());
   REQUIRE      (text == nullptr);
   REQUIRE_FALSE(text != nullptr);
   REQUIRE      (text == (char*)nullptr);
   REQUIRE_FALSE(text != (char*)nullptr);
   REQUIRE      (not text);
   REQUIRE_FALSE(text);
   REQUIRE      (text == "");
   REQUIRE_FALSE(text != "");
   REQUIRE_FALSE(text == "no match");
}

void Text_CheckState_OwnedFull(const Text& text) {
   REQUIRE_FALSE(text.IsCompressed());
   REQUIRE_FALSE(text.IsConstant());
   REQUIRE_FALSE(text.IsDeep());
   REQUIRE_FALSE(text.IsSparse());
   REQUIRE_FALSE(text.IsEncrypted());
   REQUIRE_FALSE(text.IsMissing());
   REQUIRE_FALSE(text.IsOr());
   REQUIRE      (text.IsTyped());
   REQUIRE_FALSE(text.IsUntyped());
   REQUIRE      (text.IsValid());
   REQUIRE_FALSE(text.IsInvalid());
   REQUIRE_FALSE(text.IsStatic());
   REQUIRE      (text.IsAllocated());
   REQUIRE_FALSE(text.IsEmpty());
   REQUIRE      (text.GetAllocation());
   REQUIRE      (text.IsTypeConstrained());
   REQUIRE      (text.GetType() == MetaOf<Letter>());
   REQUIRE      (text.template Is<Letter>());
   REQUIRE      (text.IsNow());
   REQUIRE_FALSE(text.IsFuture());
   REQUIRE_FALSE(text.IsPast());
   REQUIRE      (text.IsDense());
   REQUIRE      (text.GetCount() > 0);
   REQUIRE      (text.GetReserved() > 0);
   REQUIRE      (text.GetUses() > 0);
   REQUIRE      (text.GetRaw());
   REQUIRE      (text != nullptr);
   REQUIRE_FALSE(text == nullptr);
   REQUIRE      (text != (char*)nullptr);
   REQUIRE_FALSE(text == (char*)nullptr);
   REQUIRE      (text);
   REQUIRE_FALSE(not text);
   REQUIRE      (text != "");
   REQUIRE_FALSE(text == "");
   REQUIRE_FALSE(text == "no match");
}

void Text_CheckState_DisownedFullConst(const Text& text) {
   REQUIRE_FALSE(text.IsCompressed());
   REQUIRE      (text.IsConstant());
   REQUIRE_FALSE(text.IsDeep());
   REQUIRE_FALSE(text.IsSparse());
   REQUIRE_FALSE(text.IsEncrypted());
   REQUIRE_FALSE(text.IsMissing());
   REQUIRE_FALSE(text.IsOr());
   REQUIRE      (text.IsTyped());
   REQUIRE_FALSE(text.IsUntyped());
   REQUIRE      (text.IsValid());
   REQUIRE_FALSE(text.IsInvalid());
   REQUIRE      (text.IsStatic());
   REQUIRE      (text.IsAllocated());
   REQUIRE_FALSE(text.IsEmpty());
   REQUIRE_FALSE(text.GetAllocation());
   REQUIRE      (text.IsTypeConstrained());
   REQUIRE      (text.GetType() == MetaOf<Letter>());
   REQUIRE      (text.template Is<Letter>());
   REQUIRE      (text.IsNow());
   REQUIRE_FALSE(text.IsFuture());
   REQUIRE_FALSE(text.IsPast());
   REQUIRE      (text.IsDense());
   REQUIRE      (text.GetCount() > 0);
   REQUIRE      (text.GetReserved() > 0);
   REQUIRE      (text.GetUses() == 0);
   REQUIRE      (text.GetRaw());
   REQUIRE      (text != nullptr);
   REQUIRE_FALSE(text == nullptr);
   REQUIRE      (text != (char*)nullptr);
   REQUIRE_FALSE(text == (char*)nullptr);
   REQUIRE      (text);
   REQUIRE_FALSE(not text);
   REQUIRE      (text != "");
   REQUIRE_FALSE(text == "");
   REQUIRE_FALSE(text == "no match");
}
