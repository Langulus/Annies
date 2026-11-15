///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "handle/TestHandleCommon.hpp"
#include <Langulus/TOwn.hpp>


TEST_CASE_TEMPLATE("Owned value", T
   , Own<RT>
   //, TOwn<const RT> //TODO why are these commented out?
   , Own<int>
   //, TOwn<const int>
   , Own<RT*>
   , Own<const RT*>
   , Own<int*>
   , Own<const int*>
) {
   static MemoryState memoryState;
   using E = TypeOf<T>;
   
   
   GIVEN("Default-initialized") {
      T pointer;
      T pointer2;

      REQUIRE(pointer.GetRaw());

      if constexpr (not ::std::is_trivially_default_constructible_v<E>) {
         REQUIRE(pointer == pointer2);
      }

      /*WHEN("Create an instance") {
         pointer.Emplace(5);

         REQUIRE(*pointer == 5);
         REQUIRE(pointer.GetAllocation());
         REQUIRE(pointer.GetUses() == 1);
         if constexpr (CT::Referenced<TT>)
            REQUIRE(pointer->GetReferences() == 1);
      }

      WHEN("Create and copy an instance") {
         pointer.Emplace(5);
         pointer2 = pointer;

         REQUIRE(pointer == pointer2);
         REQUIRE(*pointer == 5);
         REQUIRE(*pointer2 == 5);
         REQUIRE(pointer.GetAllocation());
         REQUIRE(pointer2.GetAllocation());
         REQUIRE(pointer.GetUses() == 2);
         REQUIRE(pointer2.GetUses() == 2);
         if constexpr (CT::Referenced<TT>)
            REQUIRE(pointer->GetReferences() == 1); //TODO major design change - TRef is no longer deep referenced for now, let's see how that goes
      }

      WHEN("Create and move an instance") {
         pointer.Emplace(5);
         pointer2 = ::std::move(pointer);

         REQUIRE_FALSE(pointer);
         REQUIRE(pointer2);
         REQUIRE(*pointer2 == 5);
         REQUIRE_FALSE(pointer.GetAllocation());
         REQUIRE(pointer2.GetAllocation());
         REQUIRE(pointer.GetUses() == 0);
         REQUIRE(pointer2.GetUses() == 1);
         if constexpr (CT::Referenced<TT>)
            REQUIRE(pointer2->GetReferences() == 1);
      }

      WHEN("Overwrite an instance") {
         pointer.Emplace(5);
         IF_LANGULUS_MANAGED_MEMORY(auto backup = pointer.GetRaw());
         pointer2.Emplace(6);
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
         if constexpr (CT::Referenced<TT>)
            REQUIRE(pointer->GetReferences() == 1); //TODO major design change - TRef is no longer deep referenced for now, let's see how that goes
      }*/

      ScopedElement<E> raw {3};

      WHEN("Given an xvalue pointer created via `new` statement") {
         pointer = ::std::move(*raw);

         REQUIRE(pointer == *raw);
         
         #if LANGULUS_FEATURE(NEWDELETE)
            REQUIRE(pointer.GetReferences() == 2);
         #else
            if constexpr (CT::Referenced<E>)
               REQUIRE(pointer->GetReferences() == 1);
         #endif
      }

      #if LANGULUS_FEATURE(NEWDELETE)
         WHEN("Given an immediate xvalue pointer created via `new` statement - a very bad practice, unless LANGULUS_FEATURE(NEWDELETE) is enabled!") {
            pointer = new Decay<E> {3};

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
         pointer = *raw;

         REQUIRE(pointer == *raw);
         
         #if LANGULUS_FEATURE(NEWDELETE)
            REQUIRE(pointer.GetReferences() == 2);
         #else
            if constexpr (CT::Referenced<E>)
               REQUIRE(pointer->GetReferences() == 1);
         #endif
      }

      WHEN("Compared") {
         //static_assert(not requires { static_cast<bool>(T{}); });

         static_assert(T{} == T{});
         static_assert(T{} == E{});
         static_assert(E{} == T{});
         static_assert(T{ E{} } == T{ E{} });
         static_assert(T{ E{} } == E{});
         static_assert(E{} == T{ E{} });

         if constexpr (CT::Dense<E>) {
            static_assert(T{} != static_cast<E>(1));
            static_assert(static_cast<E>(1) != T{});
            static_assert(T{ E{} } != static_cast<E>(1));
            static_assert(static_cast<E>(1) != T{ E{} });
         }
      }
      
      WHEN("GetHandle is called on mutable container") {
         auto h = pointer.GetHandle();
         static_assert(::std::same_as<decltype(h), THandleDisowned<E&>>);

         Handle_CheckState_OwnedFull<E>(h);
      }

      WHEN("GetHandle is called on constant container") {
         T const pack_constant = *raw;
         auto h = pack_constant.GetHandle();
         static_assert(::std::same_as<decltype(h), THandleDisowned<ConstAll<E&>>>);

         Handle_CheckState_OwnedFull<E const>(h);
      }
   }

   REQUIRE(memoryState.Assert());
   REQUIRE_FALSE(Allocator::CollectGarbage());
}
