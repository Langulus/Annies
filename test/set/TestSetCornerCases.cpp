///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "TestSetCommon.hpp"


/// Testing some corner cases encountered during the use of the container     
TEMPLATE_TEST_CASE("Set corner cases", "[set]",
   (SetTest<UnorderedSet, Text>),
   (SetTest<TUnorderedSet<Text>, Text>),
   (SetTest<TOrderedSet<Text>, Text>),
   (SetTest<OrderedSet, Text>)
) {
   using T = typename TestType::Container;
   using K = typename TestType::Key;

   GIVEN("Set instance initialized with 10 specific elements for the corner case") {
      const K keys[10] = {
         {"VulkanLayer"},
         {"VulkanRenderer"},
         {"VulkanCamera"},
         {"Platform"},
         {"Vulkan"},
         {"Window"},
         {"VulkanLight"},
         {"Monitor"},
         {"VulkanRenderable"},
         {"Cursor"}
      };

      T set {keys};

      WHEN("Removing around-the-end elements by value (corner case)") {
         size_t removed = 0;
         removed += set.Remove("VulkanRenderer");
         removed += set.Remove("VulkanCamera");
         removed += set.Remove("Vulkan");
         removed += set.Remove("VulkanRenderable");
         removed += set.Remove("VulkanLight");
         removed += set.Remove("VulkanLayer");

         REQUIRE(removed == 6);
         REQUIRE(set.GetCount() == 4);

         REQUIRE(set.Contains("Platform"));
         REQUIRE(set.Contains("Window"));
         REQUIRE(set.Contains("Monitor"));
         REQUIRE(set.Contains("Cursor"));
         REQUIRE_FALSE(set.Contains("VulkanLayer"));
         REQUIRE_FALSE(set.Contains("VulkanRenderer"));
         REQUIRE_FALSE(set.Contains("VulkanCamera"));
         REQUIRE_FALSE(set.Contains("Vulkan"));
         REQUIRE_FALSE(set.Contains("VulkanLight"));
         REQUIRE_FALSE(set.Contains("VulkanRenderable"));
      }
   }

   // Destroy BANK before static data - otherwise problems happen if    
   // not using managed reflection                                      
   BANK.Reset();

   REQUIRE_FALSE(Allocator::CollectGarbage());
}

/// Testing some corner cases encountered during the use of the container     
TEMPLATE_TEST_CASE("Set of outside-referenced elements", "[set]",
   TUnorderedSet<RT*>,
   TOrderedSet<RT*>,
   UnorderedSet,
   OrderedSet
) {
   static Allocator::State memoryState;
   using T = TestType;
   TMany<RT> factory {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

   for (int iii = 0; iii < 100; ++iii) {
   GIVEN(std::string("A set of externally produced elements #") + std::to_string(iii)) {
      T set;

      for (auto& element : factory) {
         REQUIRE(element.GetReferences() == 1);
         set << &element;
         REQUIRE(element.GetReferences() == (LANGULUS_FEATURE_MANAGED_MEMORY() ? 2 : 1));
      }

      WHEN("Elements are inserted") {
         for (auto& element : factory) {
            REQUIRE(set.Contains(&element));
            REQUIRE(element.GetReferences() == (LANGULUS_FEATURE_MANAGED_MEMORY() ? 2 : 1));
            REQUIRE(factory.GetUses() == (LANGULUS_FEATURE_MANAGED_MEMORY() ? 11 : 1));
         }
      }

      WHEN("An element is removed from the set") {
         REQUIRE(set.Remove(&factory[5]) == 1);

         for (auto& element : factory) {
            if (element == RT {6}) {
               REQUIRE_FALSE(set.Contains(&element));
               REQUIRE(element.GetReferences() == 1);
            }
            else {
               REQUIRE(set.Contains(&element));
               REQUIRE(element.GetReferences() == (LANGULUS_FEATURE_MANAGED_MEMORY() ? 2 : 1));
            }

            REQUIRE(factory.GetUses() == (LANGULUS_FEATURE_MANAGED_MEMORY() ? 10 : 1));
         }
      }

      WHEN("The set is reset") {
         set.Reset();

         for (auto& element : factory) {
            REQUIRE_FALSE(set.Contains(&element));
            REQUIRE(element.GetReferences() == 1);
            REQUIRE(factory.GetUses() == 1);
         }
      }
   }}

   factory.Reset();

   REQUIRE(memoryState.Assert());

   // Destroy BANK before static data - otherwise problems happen if    
   // not using managed reflection                                      
   BANK.Reset();

   REQUIRE_FALSE(Allocator::CollectGarbage());
}
