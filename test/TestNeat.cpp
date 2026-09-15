///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include <Langulus/Annies/Neat.hpp>
#include "Common.hpp"


SCENARIO("Data normalization", "[neat]") {
   static Allocator::State memoryState;

   static_assert(not CT::Abstract<Many>);
   static_assert(not CT::Abstract<TMany<Many>>);
   static_assert(not CT::Abstract<TMeta>);
   static_assert(not CT::Abstract<TPair<TMeta, TMany<Many>>>);

   Many test1aa;
   Many test2aa {Clone(test1aa)};
   Many test3aa {Copy(test1aa)};
   Many test4aa {Refer(test1aa)};

   IntentNew(&test3aa, Copy(test1aa));
   IntentNew(&test3aa, Clone(test1aa));
   IntentNew(&test3aa, Refer(test1aa));

   static_assert(CT::Complete<Many>);

   static_assert(CT::IntentMakable<Copied, Many>);
   static_assert(CT::IntentMakable<Referred, Many>);

   static_assert(CT::DeepMakable<Many, Cloned<TMany<Many>>>);
   static_assert(CT::CopyMakable<Many>);
   static_assert(CT::ReferMakable<Many>);
   static_assert(CT::CloneMakable<Many>);

   static_assert(CT::CopyMakable<TMeta>);
   static_assert(CT::ReferMakable<TMeta>);
   static_assert(CT::CloneMakable<TMeta>);

   static_assert( CT::CopyMakable<TMany<Many>>);
   static_assert(CT::ReferMakable<TMany<Many>>);
   static_assert(CT::CloneMakable<TMany<Many>>);

   TMany<Many> test1a;
   TMany<Many> test2a {Clone(test1a)};
   TMany<Many> test3a {Copy(test1a)};
   TMany<Many> test4a {Refer(test1a)};

   TPair<TMeta, TMany<Many>> test1;
   TPair<TMeta, TMany<Many>> test2 {Clone(test1)};
   TPair<TMeta, TMany<Many>> test3 {Copy(test1)};
   TPair<TMeta, TMany<Many>> test4 {Refer(test1)};



   static_assert(CT::Exact<typename IntentOf<Cloned<const int>>::template
      As<float>, Cloned<float>>);
   static_assert(CT::Exact<typename IntentOf<Cloned<const int>&>::template
      As<float>, Cloned<float>>);
   static_assert(CT::Exact<typename IntentOf<Cloned<const int>&&>::template
      As<float>, Cloned<float>>);
   static_assert(CT::Exact<typename IntentOf<const Cloned<const int>&>::template
      As<float>, Cloned<float>>);

   static_assert(CT::Pair<Deint<Cloned<TPair<TMeta, TMany<Many>>>>>);
   static_assert(CT::PairMakable<TMeta, TMany<Many>, Cloned<TPair<TMeta, TMany<Many>>>>);
   static_assert(CT::PairAssignable<TMeta, TMany<Many>, Cloned<TPair<TMeta, TMany<Many>>>>);

   static_assert(CT::IntentMakableAlt<Copied<TMeta>>);
   static_assert(CT::IntentMakableAlt<Referred<TMeta>>);
   static_assert(CT::IntentMakableAlt<Cloned<TMeta>>);

   static_assert(CT::IntentMakableAlt<Copied<TMany<Many>>>);
   static_assert(CT::IntentMakableAlt<Referred<TMany<Many>>>);
   static_assert(CT::IntentMakableAlt<Cloned<TMany<Many>>>);

   static_assert(CT::IntentMakableAlt<Copied<TPair<TMeta, TMany<Many>>>>);
   static_assert(CT::IntentMakableAlt<Referred<TPair<TMeta, TMany<Many>>>>);
   static_assert(CT::IntentMakableAlt<Cloned<TPair<TMeta, TMany<Many>>>>);

   static_assert(CT::CopyMakable<TUnorderedMap<TMeta, TMany<Many>>>);
   static_assert(CT::ReferMakable<TUnorderedMap<TMeta, TMany<Many>>>);
   static_assert(CT::CloneMakable<TUnorderedMap<TMeta, TMany<Many>>>);

   static_assert(CT::MoveMakable<TPair<TMeta, TMany<Many>>>);
   static_assert(CT::CopyMakable<TPair<TMeta, TMany<Many>>>);
   static_assert(CT::ReferMakable<TPair<TMeta, TMany<Many>>>);
   static_assert(CT::CloneMakable<TPair<TMeta, TMany<Many>>>);

	GIVEN("An empty messy descriptor") {
      Many descriptor;

		WHEN("Normalized") {
			Neat normalized {descriptor};
		}
	}

	GIVEN("A messy descriptor with byte contents") {
      TMany<Byte> data;
      data.New(8192);

      WHEN("Filled with contents") {
         Neat normalized {data};
      }
	}
   
	GIVEN("A messy descriptor constructed with text contents") {
      Many content;
      content << "test"_text;

      WHEN("Filled with abandoned contents") {
         Neat normalized {Abandon(content)};

         REQUIRE(normalized == Neat {"test"});
         REQUIRE(normalized != Neat {"test "});
      }

      WHEN("Filled with copied contents") {
         Neat normalized {Copy(content)};

         REQUIRE(normalized == Neat {"test"});
         REQUIRE(normalized != Neat {"test "});
      }

      WHEN("Filled with cloned contents") {
         Neat normalized {Clone(content)};

         REQUIRE(normalized == Neat {"test"});
         REQUIRE(normalized != Neat {"test "});
      }

      WHEN("Filled with moved contents") {
         Neat normalized {Move(content)};

         REQUIRE(normalized == Neat {"test"});
         REQUIRE(normalized != Neat {"test "});
      }

      WHEN("Filled with refering contents") {
         Neat normalized {Refer(content)};

         REQUIRE(normalized == Neat {"test"});
         REQUIRE(normalized != Neat {"test "});
      }
	}
   
	GIVEN("A messy descriptor with text contents pushed") {
      Neat normalized;
      Text owned_string = "test";
      Many content;
      content << Text {owned_string.operator Token()};

      WHEN("Filled with abandoned contents") {
         normalized << Abandon(content);

         REQUIRE(normalized == Neat {"test"});
         REQUIRE(normalized != Neat {"test "});
      }

      WHEN("Filled with copied contents") {
         normalized << Copy(content);

         REQUIRE(normalized == Neat {"test"});
         REQUIRE(normalized != Neat {"test "});
      }

      WHEN("Filled with cloned contents") {
         normalized << Clone(content);

         REQUIRE(normalized == Neat {"test"});
         REQUIRE(normalized != Neat {"test "});
      }

      WHEN("Filled with moved contents") {
         normalized << Move(content);

         REQUIRE(normalized == Neat {"test"});
         REQUIRE(normalized != Neat {"test "});
      }

      WHEN("Filled with refering contents") {
         normalized << Refer(content);

         REQUIRE(normalized == Neat {"test"});
         REQUIRE(normalized != Neat {"test "});
      }
	}
   
	GIVEN("A neat container full of many things") {
      struct ComplexStuff {
         int x = 1;
         float y = 2;
         double z = 3;
         std::string name;

         bool operator == (const ComplexStuff&) const = default;

         ~ComplexStuff() {
            x = 0;
            y = 1;
            z = 2;
         }
      };

      Neat neat {
         Traits::Name {"Root"},
         Construct::From<int>(),
         Construct::From<float>(),
         Construct::From<double>(),
         Construct::From<ComplexStuff>(
            Traits::Name {"Child1"},
            Construct::From<int>(),
            Construct::From<float>(),
            Construct::From<ComplexStuff>(Traits::Name {"GrandChild1"}),
            Construct::From<ComplexStuff>(Traits::Name {"GrandChild2"})
         ),
         Construct::From<ComplexStuff>(Traits::Name {"Child2"})
      };

      WHEN("Copied") {
         Neat copied = neat;

         REQUIRE(neat == copied);
      }
	}

   REQUIRE(memoryState.Assert());

   // Destroy BANK before static data - otherwise problems happen if    
   // not using managed reflection                                      
   BANK.Reset();

   REQUIRE_FALSE(Allocator::CollectGarbage());
}
