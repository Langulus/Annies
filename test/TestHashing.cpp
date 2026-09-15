///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include <Langulus/Annies/Text.hpp>
#include <Langulus/Annies/Path.hpp>
#include <Langulus/Annies/TMap.hpp>
#include <Langulus/Annies/TSet.hpp>
#include <Langulus/Annies/Trait.hpp>
#include "Common.hpp"


SCENARIO("Hashing different kinds of containers", "[hash]") {
   WHEN("Hashing all kinds of containers") {
      Token same1 = "Same1"; 
      std::string same1str = "Same1";
      Text same1txt = "Same1";
      Path same1pat = "Same1";
      Many same1any {'S', 'a', 'm', 'e', '1'};
      std::vector<char> same1vec = {'S', 'a', 'm', 'e', '1'};
      //std::span<char, 5> sape1spa = "Same1";
      std::array<char, 5> sape1arr = {'S', 'a', 'm', 'e', '1'};

      REQUIRE(HashOf(same1) == HashOf(same1str));
      REQUIRE(HashOf(same1) == HashOf(same1any));
      REQUIRE(HashOf(same1) == HashOf(same1txt));
      REQUIRE(HashOf(same1) == HashOf(same1pat));
      REQUIRE(HashOf(same1) == HashOf(same1vec));
      //REQUIRE(HashOf(same1) == HashOf(sape1spa));
      REQUIRE(HashOf(same1) == HashOf(sape1arr));
      REQUIRE(HashOf(same1) == HashBytes("Same1", 5));
   }

   // Destroy BANK before static data - otherwise problems happen if    
   // not using managed reflection                                      
   BANK.Reset();

   REQUIRE_FALSE(Allocator::CollectGarbage());
}


/// Cross-container consistency tests                                         
TEMPLATE_TEST_CASE(
   "Cross-container consistency tests for TOrderedMap/TUnorderedMap/OrderedMap/UnorderedMap", "[map]",
   (HashTest<Text, Trait*>),

   (HashTest<Text, int>),
   (HashTest<Text, Trait>),
   (HashTest<Text, Traits::Count>),
   (HashTest<Text, Many>),

   (HashTest<Text, int*>),
   (HashTest<Text, Traits::Count*>),
   (HashTest<Text, Many*>)
) {
   using K = typename TestType::Key;
   using V = typename TestType::Value;

   Allocator::State memoryState;

   GIVEN("A single element initialized maps of all kinds") {
      const auto pair = CreatePair<TPair<K, V>, K, V>(
         "five hundred", 555);

      TUnorderedMap<K, V> uset1 {pair};
      UnorderedMap uset2 {pair};
      TOrderedMap<K, V> oset1 {pair};
      OrderedMap oset2 {pair};

      WHEN("Their hashes are taken") {
         const auto elementHash = HashOf(pair);

         const auto uhash1 = uset1.GetHash();
         const auto uhash2 = uset2.GetHash();
         const auto ohash1 = oset1.GetHash();
         const auto ohash2 = oset2.GetHash();

         REQUIRE(uhash1 == uhash2);
         REQUIRE(ohash1 == ohash2);
         REQUIRE(uhash1 == ohash1);
         REQUIRE(uhash1 == elementHash);
      }

      // Check for memory leaks after each cycle                        
      REQUIRE(memoryState.Assert());

      DestroyPair(pair);
   }

   // Destroy BANK before static data - otherwise problems happen if    
   // not using managed reflection                                      
   BANK.Reset();

   REQUIRE_FALSE(Allocator::CollectGarbage());
}

/// Cross-container consistency tests                                         
TEMPLATE_TEST_CASE(
   "Cross-container consistency tests for TOrderedSet/TUnorderedSet/OrderedSet/UnorderedSet", "[set]",
   int,  Trait,  Traits::Count,  Many,
   int*, Trait*, Traits::Count*, Many*
) {
   Allocator::State memoryState;

   GIVEN("A single element initialized sets of all kinds") {
      const auto element = CreateElement<TestType>(555);
      const auto elementHash = HashOf(element);

      TUnorderedSet<TestType> uset1 {element};
      UnorderedSet uset2 {element};
      TOrderedSet<TestType> oset1 {element};
      OrderedSet oset2 {element};

      WHEN("Their hashes are taken") {
         REQUIRE(uset1.template IsExact<TestType>());
         REQUIRE(uset2.template IsExact<TestType>());
         REQUIRE(oset1.template IsExact<TestType>());
         REQUIRE(oset2.template IsExact<TestType>());

         const auto uhash1 = uset1.GetHash();
         const auto uhash2 = uset2.GetHash();
         const auto ohash1 = oset1.GetHash();
         const auto ohash2 = oset2.GetHash();

         REQUIRE(uhash1 == uhash2);
         REQUIRE(ohash1 == ohash2);
         REQUIRE(uhash1 == ohash1);
         REQUIRE(uhash1 == elementHash);
      }

      // Check for memory leaks after each cycle                        
      REQUIRE(memoryState.Assert());

      DestroyElement(element);
   }

   // Destroy BANK before static data - otherwise problems happen if    
   // not using managed reflection                                      
   BANK.Reset();

   REQUIRE_FALSE(Allocator::CollectGarbage());
}
