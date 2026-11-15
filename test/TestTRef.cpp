///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "handle/TestHandleCommon.hpp"
#include <Langulus/TRef.hpp>

#include <Langulus/Annies/Functional/Compare.hpp>


TEST_CASE_TEMPLATE("Shared pointer", TestType
   , Types<Ref<RT>,        ScopedElement<RT>>
   , Types<Ref<const RT>,  ScopedElement<RT>>
   , Types<Ref<int>,       ScopedElement<int>>
   , Types<Ref<const int>, ScopedElement<int>>
   
   , Types<Ref<RT>,        ScopedElement<RT, true>>
   , Types<Ref<const RT>,  ScopedElement<RT, true>>
   , Types<Ref<int>,       ScopedElement<int, true>>
   , Types<Ref<const int>, ScopedElement<int, true>>

   //TODO multiple indirections
) {
   static MemoryState memoryState;
   using T = typename TestType::First;
   using E = TypeOf<T>;
   using ScopedE = typename TestType::Second;
   constexpr bool Managed = ScopedE::Managed;

   static_assert(T::CountHeapProviders() == 0);
   static_assert(not CT::HeapAllocated<T>);
   static_assert(not CT::Multiheap<T>);
   static_assert(not CT::OwnedStrong<T>);
   static_assert(not CT::Owned<T>);
   static_assert(    CT::OwnedDeep<T>);
   static_assert(    CT::OwnedDeepStrong<T>);
   static_assert(not CT::HasVariableCount<T>);
   static_assert(not CT::ContainsMany<T>);
   static_assert(    CT::ContainsOne<T>);
   static_assert(not CT::TypeErased<T>);
   static_assert(not CT::Indexed<T>);
   static_assert(    CT::IndexedLinearly<T>);
   static_assert(not CT::IndexedTable<T>);

   GIVEN("Nullptr-initialized") {
      T pointer  {nullptr};
      T pointer2 {nullptr};

      REQUIRE(pointer.GetRaw());
      REQUIRE(pointer.IsEmpty());
      REQUIRE_FALSE(pointer);
      REQUIRE(pointer == pointer2);
   }

   GIVEN("Default-initialized") {
      const ScopedE raw3 {3};
      const ScopedE raw5 {5};
      
      T pointer;
      T pointer2;

      REQUIRE(pointer.IsEmpty());
      REQUIRE_FALSE(pointer);
      REQUIRE(pointer == pointer2);

      WHEN("Create an instance") {
         pointer.Emplace(raw3.element); //TODO apparently Emplace accepts just plain int, maybe constrain it?

         REQUIRE(*pointer == raw3.element);

         if constexpr (Managed)
            REQUIRE(raw3.entries[0]->GetUses() == 2);

         //REQUIRE(pointer.GetEntries());
         //REQUIRE(pointer.GetEntries()[0]->GetUses() == 1);
         if constexpr (CT::Referenced<E>)
            REQUIRE(pointer->GetReferences() == 2);
      }

      WHEN("Create and refer an instance") {
         pointer.Emplace(raw3.element); //TODO apparently Emplace accepts just plain int, maybe constrain it?
         pointer2 = pointer;

         REQUIRE(pointer == pointer2);
         REQUIRE(*pointer == raw3.element);
         REQUIRE(*pointer2 == raw3.element);

         if constexpr (Managed)
            REQUIRE(raw3.entries[0]->GetUses() == 3);

         /*REQUIRE(pointer.GetEntries());
         REQUIRE(pointer2.GetEntries());
         REQUIRE(pointer.GetEntries()[0]->GetUses() == 2);
         REQUIRE(pointer2.GetEntries()[0]->GetUses() == 2);*/
         if constexpr (CT::Referenced<E>)
            REQUIRE(pointer->GetReferences() == 3);
      }

      WHEN("Create and move an instance") {
         pointer.Emplace(raw3.element); //TODO apparently Emplace accepts just plain int, maybe constrain it?
         pointer2 = ::std::move(pointer);

         REQUIRE_FALSE(pointer);
         REQUIRE(pointer2);
         REQUIRE(*pointer2 == raw3.element);

         if constexpr (Managed)
            REQUIRE(raw3.entries[0]->GetUses() == 2);

         /*REQUIRE_FALSE(pointer.GetEntries());
         REQUIRE(pointer2.GetEntries());
         REQUIRE(pointer.GetEntries()[0]->GetUses() == 0);
         REQUIRE(pointer2.GetEntries()[0]->GetUses() == 1);*/
         if constexpr (CT::Referenced<E>)
            REQUIRE(pointer2->GetReferences() == 2);
      }

      WHEN("Overwrite an instance") {
         pointer.Emplace(raw3.element); //TODO apparently Emplace accepts just plain int, maybe constrain it?

         /*#if LANGULUS_FEATURE(MANAGED_MEMORY)
            auto backup = pointer.GetRaw();
         #endif*/

         pointer2.Emplace(raw5.element); //TODO apparently Emplace accepts just plain int, maybe constrain it?
         pointer = pointer2;

         REQUIRE(pointer == pointer2);
         REQUIRE(*pointer == raw5.element);
         REQUIRE(*pointer2 == raw5.element);
         /*#if LANGULUS_FEATURE(MANAGED_MEMORY)
            REQUIRE(Allocator::CheckAuthority(backup));
            REQUIRE_FALSE(Allocator::Find(backup));
         #endif*/

         if constexpr (Managed) {
            REQUIRE(raw3.entries[0]->GetUses() == 1);
            REQUIRE(raw5.entries[0]->GetUses() == 3);
         }

         /*REQUIRE(pointer2.GetEntries());
         REQUIRE(pointer.GetEntries());
         REQUIRE(pointer.GetEntries()[0]->GetUses() == 2);*/
         if constexpr (CT::Referenced<E>)
            REQUIRE(pointer->GetReferences() == 3);
      }

      //TODO test New
      
      WHEN("Given an xvalue raw pointer") {
         auto movable = raw5.element;
         pointer = ::std::move(movable);

         REQUIRE(pointer == raw5.element);
         REQUIRE(*pointer == raw5.element);

         if constexpr (Managed) {
            REQUIRE(raw3.entries[0]->GetUses() == 1);
            REQUIRE(raw5.entries[0]->GetUses() == 2);
         }

         /*#if LANGULUS_FEATURE(MANAGED_MEMORY)
            REQUIRE(pointer.GetEntries() == raw5.entries);
         #else
            REQUIRE(pointer.GetEntries() == nullptr);
         #endif
         REQUIRE(pointer.GetEntries()[0]->GetUses() == 2);*/
         if constexpr (CT::Referenced<E>)
            REQUIRE(pointer->GetReferences() == 2);
      }

      WHEN("Given an lvalue pointer") {
         pointer = raw5.element;

         REQUIRE(pointer == raw5.element);
         REQUIRE(*pointer == raw5.element);

         if constexpr (Managed) {
            REQUIRE(raw3.entries[0]->GetUses() == 1);
            REQUIRE(raw5.entries[0]->GetUses() == 2);
         }

         /*#if LANGULUS_FEATURE(MANAGED_MEMORY)
            REQUIRE(pointer.GetEntries() == raw5.entries);
         #else
            REQUIRE(pointer.GetEntries() == nullptr);
         #endif
         REQUIRE(pointer.GetEntries()[0]->GetUses() ==  2);*/
         if constexpr (CT::Referenced<E>)
            REQUIRE(pointer->GetReferences() == 2);
      }

      WHEN("Compared") {
         static_assert(not static_cast<bool>(T{}));

         static_assert(T{} == T{});
         static_assert(T{} == nullptr);
         static_assert(nullptr == T{});
         static_assert(T{} == (E) {});
         static_assert((E) {} == T{});
         static_assert(T{ nullptr } == T{ nullptr });
         static_assert(T{ (E) {} } == T{ (E) {} });
         static_assert(T{ nullptr } == nullptr);
         static_assert(nullptr == T{ nullptr });
         static_assert(T{ (E) {} } == (E) {});
         static_assert((E) {} == T{ (E) {} });
      }
      
      WHEN("GetHandle is called on mutable container") {
         auto h = pointer.GetHandle();
         static_assert(::std::same_as<decltype(h), THandleEmergent<E&>>);

         Handle_CheckState_OwnedFull<E>(h);
      }

      WHEN("GetHandle is called on constant container") {
         T const pack_constant;
         auto h = pack_constant.GetHandle();
         static_assert(::std::same_as<decltype(h), THandleEmergent<ConstAll<E&>>>);

         Handle_CheckState_OwnedFull<E const>(h);
      }
   }

   REQUIRE(memoryState.Assert());
   REQUIRE_FALSE(Allocator::CollectGarbage());
}
