///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include <Langulus/Annies/Text.hpp>
#include <Langulus/Annies/Ref.hpp>
#include "Common.hpp"


///                                                                           
TEMPLATE_TEST_CASE("Shared pointer", "[Ref]",
   Ref<const RT>, Ref<Many>, Ref<int>, Ref<RT>,
   Ref<const Many>, Ref<const int>
) {
   static Allocator::State memoryState;

   using T = TestType;
   using TT = TypeOf<T>;

   GIVEN("A nullptr-initialized templated shared pointer") {
      T pointer {nullptr};
      T pointer2 {nullptr};

      constexpr T pointer_constexpr {nullptr};
      static_assert(not pointer_constexpr);

      constexpr T pointer2_constexpr {nullptr};
      static_assert(not pointer2_constexpr);

      static_assert(pointer_constexpr == pointer2_constexpr);

      REQUIRE_FALSE(pointer.Get());
      REQUIRE_FALSE(pointer);
      REQUIRE(pointer == pointer2);
   }

   GIVEN("A default-initialized templated shared pointer") {
      T pointer;
      T pointer2;

      constexpr T pointer_constexpr;
      static_assert(not pointer_constexpr);

      constexpr T pointer2_constexpr;
      static_assert(not pointer2_constexpr);

      static_assert(pointer_constexpr == pointer2_constexpr);

      REQUIRE_FALSE(pointer.Get());
      REQUIRE_FALSE(pointer);
      REQUIRE(pointer == pointer2);

      WHEN("Create an instance") {
         pointer.New(5);

         REQUIRE(*pointer == 5);
         REQUIRE(pointer.GetAllocation());
         REQUIRE(pointer.GetUses() == 1);
         if constexpr (CT::Referencable<TT>)
            REQUIRE(pointer->GetReferences() == 1);
      }

      WHEN("Create and copy an instance") {
         pointer.New(5);
         pointer2 = pointer;

         REQUIRE(pointer == pointer2);
         REQUIRE(*pointer == 5);
         REQUIRE(*pointer2 == 5);
         REQUIRE(pointer.GetAllocation());
         REQUIRE(pointer2.GetAllocation());
         REQUIRE(pointer.GetUses() == 2);
         REQUIRE(pointer2.GetUses() == 2);
         if constexpr (CT::Referencable<TT>)
            REQUIRE(pointer->GetReferences() == 2);
      }

      WHEN("Create and move an instance") {
         pointer.New(5);
         pointer2 = ::std::move(pointer);

         REQUIRE_FALSE(pointer);
         REQUIRE(pointer2);
         REQUIRE(*pointer2 == 5);
         REQUIRE_FALSE(pointer.GetAllocation());
         REQUIRE(pointer2.GetAllocation());
         REQUIRE(pointer.GetUses() == 0);
         REQUIRE(pointer2.GetUses() == 1);
         if constexpr (CT::Referencable<TT>)
            REQUIRE(pointer2->GetReferences() == 1);
      }

      WHEN("Overwrite an instance") {
         pointer.New(5);
         IF_LANGULUS_MANAGED_MEMORY(auto backup = pointer.Get());
         pointer2.New(6);
         pointer = pointer2;

         REQUIRE(pointer == pointer2);
         REQUIRE(*pointer == 6);
         REQUIRE(*pointer2 == 6);
         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            REQUIRE(Allocator::CheckAuthority(pointer.GetType(), backup));
            REQUIRE_FALSE(Allocator::Find(pointer.GetType(), backup));
         #endif
         REQUIRE(pointer2.GetAllocation());
         REQUIRE(pointer.GetAllocation());
         REQUIRE(pointer.GetUses() == 2);
         if constexpr (CT::Referencable<TT>)
            REQUIRE(pointer->GetReferences() == 2);
      }

      auto raw = new Decay<TT> {3};
      const auto rawBackUp = raw;

      WHEN("Given an xvalue pointer created via `new` statement") {
         pointer = ::std::move(raw);

         REQUIRE(pointer == rawBackUp);
         REQUIRE(*pointer == *rawBackUp);
         REQUIRE(raw == rawBackUp);
         #if LANGULUS_FEATURE(NEWDELETE)
            REQUIRE(pointer.GetAllocation());
            REQUIRE(pointer.GetReferences() == 2);
         #else
            REQUIRE_FALSE(pointer.GetAllocation());
            if constexpr (CT::Referencable<TT>)
               REQUIRE(pointer->GetReferences() == 1);
         #endif
      }

      #if LANGULUS_FEATURE(NEWDELETE)
         WHEN("Given an immediate xvalue pointer created via `new` statement - a very bad practice, unless LANGULUS_FEATURE(NEWDELETE) is enabled!") {
            pointer = new Decay<TT> {3};

            #if LANGULUS_FEATURE(NEWDELETE)
               REQUIRE(pointer.GetAllocation());
               REQUIRE(pointer.GetReferences() == 2);
            #endif
         }

         WHEN("Given an xvalue pointer and then reset") {
            pointer = ::std::move(raw);
            auto unused = Allocator::Free(pointer.GetType(), raw, 1);
            pointer = nullptr;

            REQUIRE_FALSE(raw->GetAllocation());
            REQUIRE(Allocator::CheckAuthority(pointer.GetType(), raw));
            REQUIRE_FALSE(Allocator::Find(pointer.GetType(), raw));
            REQUIRE_FALSE(pointer.GetAllocation());
         }
      #endif

      WHEN("Given an lvalue pointer") {
         pointer = raw;

         REQUIRE(pointer == raw);
         REQUIRE(*pointer == *raw);
         #if LANGULUS_FEATURE(NEWDELETE)
            REQUIRE(pointer.GetAllocation());
            REQUIRE(pointer.GetReferences() == 2);
         #else
            REQUIRE_FALSE(pointer.GetAllocation());
            if constexpr (CT::Referencable<TT>)
               REQUIRE(pointer->GetReferences() == 1);
         #endif
      }

      #if not LANGULUS_FEATURE(NEWDELETE)
         if constexpr (CT::Referencable<Deptr<TT>>)
            raw->Reference(-1);
         delete raw;
      #endif
   }

   REQUIRE(memoryState.Assert());

   // Destroy BANK before static data - otherwise problems happen if    
   // not using managed reflection                                      
   BANK.Reset();

   REQUIRE_FALSE(Allocator::CollectGarbage());
}
