///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "TestManyCommon.hpp"


SCENARIO("Pushing one sparse container, and then two more, one being the first") {
   static MemoryState memoryState;

   GIVEN("An empty container") {
      ScopedElement<Many*, true> p1 {1};
      ScopedElement<Many*, true> p2 {1};
   
      auto entry1 = p1.entries[1];
      auto entry2 = p2.entries[1];
      REQUIRE(entry1->GetUses() == 1);
      REQUIRE(entry2->GetUses() == 1);
   
      Many pack;

      WHEN("Pushed the first pointer") {
         REQUIRE_NOTHROW(pack << *p1);

         REQUIRE(pack == *p1);
         REQUIRE(pack.GetCount() == 1);
         REQUIRE(pack.IsExact<Many*>());
         REQUIRE(DenseCast(*p1).GetUses() == 1);
         REQUIRE(DenseCast(*p2).GetUses() == 1);

         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            REQUIRE(entry1->GetUses() == 2);
            REQUIRE(entry2->GetUses() == 1);
         #else
            REQUIRE(entry1->GetUses() == 1);
            REQUIRE(entry2->GetUses() == 1);
         #endif

         THEN("Push-back the first again and then the second") {
            REQUIRE_NOTHROW(pack << *p1);

            #if LANGULUS_FEATURE(MANAGED_MEMORY)
               REQUIRE(pack.GetEntries()[0] == entry1);
               REQUIRE(pack.GetEntries()[1] == entry1);
               REQUIRE(entry1->GetUses() == 3);
               REQUIRE(entry2->GetUses() == 1);
            #else
               REQUIRE(pack.GetEntries()[0] == nullptr);
               REQUIRE(pack.GetEntries()[1] == nullptr);
               REQUIRE(entry1->GetUses() == 1);
               REQUIRE(entry2->GetUses() == 1);
            #endif

            REQUIRE_NOTHROW(pack << *p2);

            #if LANGULUS_FEATURE(MANAGED_MEMORY)
               REQUIRE(pack.GetEntries()[0] == entry1);
               REQUIRE(pack.GetEntries()[1] == entry1);
               REQUIRE(pack.GetEntries()[2] == entry2);
               REQUIRE(entry1->GetUses() == 3);
               REQUIRE(entry2->GetUses() == 2);
            #else
               REQUIRE(pack.GetEntries()[0] == nullptr);
               REQUIRE(pack.GetEntries()[1] == nullptr);
               REQUIRE(pack.GetEntries()[2] == nullptr);
               REQUIRE(entry1->GetUses() == 1);
               REQUIRE(entry2->GetUses() == 1);
            #endif

            REQUIRE(pack.GetCount() == 3);
            REQUIRE(pack.IsExact<Many*>());
            REQUIRE(DenseCast(*p1).GetUses() == 1);
            REQUIRE(DenseCast(*p2).GetUses() == 1);
         }

         THEN("Push-front the first again and then the second") {
            REQUIRE_NOTHROW(pack >> *p1);

            #if LANGULUS_FEATURE(MANAGED_MEMORY)
               REQUIRE(pack.GetEntries()[0] == entry1);
               REQUIRE(pack.GetEntries()[1] == entry1);
               REQUIRE(entry1->GetUses() == 3);
               REQUIRE(entry2->GetUses() == 1);
            #else
               REQUIRE(pack.GetEntries()[0] == nullptr);
               REQUIRE(pack.GetEntries()[1] == nullptr);
               REQUIRE(entry1->GetUses() == 1);
               REQUIRE(entry2->GetUses() == 1);
            #endif

            REQUIRE_NOTHROW(pack >> *p2);

            #if LANGULUS_FEATURE(MANAGED_MEMORY)
               REQUIRE(pack.GetEntries()[0] == entry2);
               REQUIRE(pack.GetEntries()[1] == entry1);
               REQUIRE(pack.GetEntries()[2] == entry1);
               REQUIRE(entry1->GetUses() == 3);
               REQUIRE(entry2->GetUses() == 2);
            #else
               REQUIRE(pack.GetEntries()[0] == nullptr);
               REQUIRE(pack.GetEntries()[1] == nullptr);
               REQUIRE(pack.GetEntries()[2] == nullptr);
               REQUIRE(entry1->GetUses() == 1);
               REQUIRE(entry2->GetUses() == 1);
            #endif

            REQUIRE(pack.GetCount() == 3);
            REQUIRE(pack.IsExact<Many*>());
            REQUIRE(DenseCast(*p1).GetUses() == 1);
            REQUIRE(DenseCast(*p2).GetUses() == 1);
         }

         THEN("Compose-back the first again and then the second, but packed together") {
            REQUIRE_NOTHROW(pack.Compose(Many {*p1, *p2}));

            REQUIRE(pack.GetCount() == 3);
            REQUIRE(pack.IsExact<Many*>());
            REQUIRE(DenseCast(*p1).GetUses() == 1);
            REQUIRE(DenseCast(*p2).GetUses() == 1);

            #if LANGULUS_FEATURE(MANAGED_MEMORY)
               REQUIRE(entry1->GetUses() == 3);
               REQUIRE(entry2->GetUses() == 2);
            #else
               REQUIRE(entry1->GetUses() == 1);
               REQUIRE(entry2->GetUses() == 1);
            #endif
         }

         THEN("Compose-front the first again and then the second, but packed together") {
            REQUIRE_NOTHROW(pack.ComposeAt(Index::Front, Many {*p1, *p2}));

            REQUIRE(pack.GetCount() == 3);
            REQUIRE(pack.IsExact<Many*>());
            REQUIRE(DenseCast(*p1).GetUses() == 1);
            REQUIRE(DenseCast(*p2).GetUses() == 1);

            #if LANGULUS_FEATURE(MANAGED_MEMORY)
               REQUIRE(entry1->GetUses() == 3);
               REQUIRE(entry2->GetUses() == 2);
            #else
               REQUIRE(entry1->GetUses() == 1);
               REQUIRE(entry2->GetUses() == 1);
            #endif
         }
      }
   }

   REQUIRE(memoryState.Assert());
   REQUIRE_FALSE(Allocator::CollectGarbage());
}
