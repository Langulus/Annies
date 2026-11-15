///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "TestSetCommon.hpp"
#include "../handle/TestHandleCommon.hpp"


namespace Langulus::Annies  
{
   // Reuses definitions from TestSet-Empty.cpp. Reduces compile time.  
   extern template struct TSet<Text>;
   extern template struct TSet<int>;
   extern template struct TSet<Any>;
   extern template struct TSet<RT>;
   extern template struct TSet<char>;

   extern template struct TSet<Text*>;
   extern template struct TSet<int*>;
   extern template struct TSet<Any*>;
   extern template struct TSet<RT*>;
   extern template struct TSet<char*>;

   extern template struct TSet<Text**>;
   extern template struct TSet<int**>;
   extern template struct TSet<Any**>;
   extern template struct TSet<RT**>;
   extern template struct TSet<char**>;

#if LANGULUS_FEATURE(MANAGED_MEMORY)
   extern template struct TSet<pptr8>;
   extern template struct TSet<pptr16>;
   extern template struct TSet<pptr32>;
#endif
}


TEST_CASE_TEMPLATE("Test absorb-constructed Set/TSet", TestType
   // Elements are not allocated by the memory manager                  
   , Types<Set, ScopedElement<Text>>
   , Types<Set, ScopedElement<int>>
   , Types<Set, ScopedElement<Many>>
   , Types<Set, ScopedElement<RT>>
   , Types<Set, ScopedElement<char>>

   , Types<Set, ScopedElement<Text*>>
   , Types<Set, ScopedElement<int*>>
   , Types<Set, ScopedElement<Many*>>
   , Types<Set, ScopedElement<RT*>>
   , Types<Set, ScopedElement<char*>>

   , Types<Set, ScopedElement<Text**>>
   , Types<Set, ScopedElement<int**>>
   , Types<Set, ScopedElement<Many**>>
   , Types<Set, ScopedElement<RT**>>
   , Types<Set, ScopedElement<char**>>

   , Types<TSet<Text>,   ScopedElement<Text>>
   , Types<TSet<int>,    ScopedElement<int>>
   , Types<TSet<Many>,   ScopedElement<Many>>
   , Types<TSet<RT>,     ScopedElement<RT>>
   , Types<TSet<char>,   ScopedElement<char>>

   , Types<TSet<Text*>,  ScopedElement<Text*>>
   , Types<TSet<int*>,   ScopedElement<int*>>
   , Types<TSet<Many*>,  ScopedElement<Many*>>
   , Types<TSet<RT*>,    ScopedElement<RT*>>
   , Types<TSet<char*>,  ScopedElement<char*>>

   , Types<TSet<Text**>, ScopedElement<Text**>>
   , Types<TSet<int**>,  ScopedElement<int**>>
   , Types<TSet<Many**>, ScopedElement<Many**>>
   , Types<TSet<RT**>,   ScopedElement<RT**>>
   , Types<TSet<char**>, ScopedElement<char**>>

   #if LANGULUS_FEATURE(MANAGED_MEMORY)
   // Elements are allocated by the memory manager                      
   , Types<Set, ScopedElement<Text, true>>
   , Types<Set, ScopedElement<int, true>>
   , Types<Set, ScopedElement<Many, true>>
   , Types<Set, ScopedElement<RT, true>>
   , Types<Set, ScopedElement<char, true>>

   , Types<Set, ScopedElement<Text*, true>>
   , Types<Set, ScopedElement<int*, true>>
   , Types<Set, ScopedElement<Many*, true>>
   , Types<Set, ScopedElement<RT*, true>>
   , Types<Set, ScopedElement<char*, true>>

   , Types<Set, ScopedElement<Text**, true>>
   , Types<Set, ScopedElement<int**, true>>
   , Types<Set, ScopedElement<Many**, true>>
   , Types<Set, ScopedElement<RT**, true>>
   , Types<Set, ScopedElement<char**, true>>

   , Types<TSet<Text>,   ScopedElement<Text, true>>
   , Types<TSet<int>,    ScopedElement<int, true>>
   , Types<TSet<Many>,   ScopedElement<Many, true>>
   , Types<TSet<RT>,     ScopedElement<RT, true>>
   , Types<TSet<char>,   ScopedElement<char, true>>

   , Types<TSet<Text*>,  ScopedElement<Text*, true>>
   , Types<TSet<int*>,   ScopedElement<int*, true>>
   , Types<TSet<Many*>,  ScopedElement<Many*, true>>
   , Types<TSet<RT*>,    ScopedElement<RT*, true>>
   , Types<TSet<char*>,  ScopedElement<char*, true>>

   , Types<TSet<Text**>, ScopedElement<Text**, true>>
   , Types<TSet<int**>,  ScopedElement<int**, true>>
   , Types<TSet<Many**>, ScopedElement<Many**, true>>
   , Types<TSet<RT**>,   ScopedElement<RT**, true>>
   , Types<TSet<char**>, ScopedElement<char**, true>>

   // Packed pointers                                                   
   , Types<Set, ScopedElementPacked<pptr8>>
   , Types<Set, ScopedElementPacked<pptr16>>
   , Types<Set, ScopedElementPacked<pptr32>>

   , Types<TSet<pptr8>,  ScopedElementPacked<pptr8>>
   , Types<TSet<pptr16>, ScopedElementPacked<pptr16>>
   , Types<TSet<pptr32>, ScopedElementPacked<pptr32>>
   #endif
) {
   static MemoryState memoryState;
   using T = typename TestType::First;
   using ScopedE = typename TestType::Second;
   using E = TypeOf<ScopedE>;

   constexpr bool Managed = ScopedE::Managed;
   constexpr bool Sparse  = CT::Sparse<E>;
   constexpr bool Reffed  = CT::Referenced<Decay<E>>;

   #if LANGULUS(BENCHMARK)
      using stdset = ::std::unordered_set<E>;
   #endif

   GIVEN("Piecewise-constructed container, assigned (refer), and then destroyed") {
      const ScopedE element1{555};
      const ScopedE element2{111};
      T piecewise1{Piecewise, *element1};
      REQUIRE_NOTHROW(piecewise1.Assign(*element2));
   }

   GIVEN("Piecewise-constructed container, assigned (refer using intent), and then destroyed") {
      const ScopedE element1{555};
      const ScopedE element2{111};
      T piecewise1{Piecewise, *element1};
      REQUIRE_NOTHROW(piecewise1.Assign(Refer(*element2)));
   }

   GIVEN("Piecewise-constructed container, assigned (copied), and then destroyed") {
      const ScopedE element1{555};
      const ScopedE element2{111};
      T piecewise1{Piecewise, *element1};
      REQUIRE_NOTHROW(piecewise1.Assign(Copy(*element2)));
   }

   GIVEN("Piecewise-constructed container, assigned (cloned), and then destroyed") {
      const ScopedE element1{555};
      const ScopedE element2{111};
      T piecewise1{Piecewise, *element1};
      REQUIRE_NOTHROW(piecewise1.Assign(Clone(*element2)));
   }

   GIVEN("Piecewise-constructed container, assigned (move), and then destroyed") {
      const ScopedE element1{555};
      ScopedE element2{111};
      T piecewise1{Piecewise, *element1};
      REQUIRE_NOTHROW(piecewise1.Assign(::std::move(*element2)));
   }

   GIVEN("Piecewise-constructed container, assigned (move using intent), and then destroyed") {
      const ScopedE element1{555};
      ScopedE element2{112};
      T piecewise1{Piecewise, *element1};
      REQUIRE_NOTHROW(piecewise1.Assign(Move(*element2)));
   }

   GIVEN("Piecewise-constructed container, assigned (abandon), and then destroyed") {
      const ScopedE element1{555};
      ScopedE element2{112};
      T piecewise1{Piecewise, *element1};
      REQUIRE_NOTHROW(piecewise1.Assign(Abandon(*element2)));
   }

   GIVEN("Piecewise-constructed container, assigned (disown), and then destroyed") {
      const ScopedE element1{555};
      const ScopedE element2{111};
      T piecewise1{Piecewise, *element1};
      REQUIRE_NOTHROW(piecewise1.Assign(Disown(*element2)));
   }

   GIVEN("Absorb-constructed container") {
      const ScopedE originalElement {556};
      const ScopedE element {555};

      T piecewise1{Piecewise, *originalElement};
      T piecewise2{Piecewise, *originalElement};
      T piecewise3{Piecewise, *originalElement};
      T piecewise4{Piecewise, *originalElement};

      T pack_referred1{Absorb,             piecewise1};
      T pack_referred2{Absorb,       Refer(piecewise1)};
      T pack_copied   {Absorb,        Copy(piecewise1)};
      T pack_cloned   {Absorb,       Clone(piecewise1)};
      T pack_moved1   {Absorb, ::std::move(piecewise2)};
      T pack_moved2   {Absorb,        Move(piecewise3)};
      T pack_abandoned{Absorb,     Abandon(piecewise4)};
      T pack_disowned {Absorb,      Disown(piecewise1)};

      WHEN("Absorb-constructed") {
         Set_CheckState_OwnedFull<E>(pack_referred1);
         Set_CheckState_OwnedFull<E>(pack_referred2);
         Set_CheckState_OwnedFull<E>(pack_copied);
         Set_CheckState_OwnedFull<E>(pack_cloned);
         Set_CheckState_OwnedFull<E>(pack_moved1);
         Set_CheckState_OwnedFull<E>(pack_moved2);
         Set_CheckState_OwnedFull<E>(pack_abandoned);
         Set_CheckState_DisownedFull<E>(pack_disowned);

         Set_CheckState_ContainsOne(pack_referred1,  Refer(originalElement), 3);
         Set_CheckState_ContainsOne(pack_referred2,  Refer(originalElement), 3);
         Set_CheckState_ContainsOne(pack_copied,     Refer(originalElement), 1);
         Set_CheckState_ContainsOne(pack_cloned,     Clone(originalElement), 1);
         Set_CheckState_ContainsOne(pack_moved1,     Refer(originalElement), 1);
         Set_CheckState_ContainsOne(pack_abandoned,  Refer(originalElement), 1);

         if constexpr (Managed) {
            // Entries are still propagated when absorbed
            Set_CheckState_ContainsOne(pack_disowned,  Refer(originalElement), 3);
         }
         else Set_CheckState_ContainsOne(pack_disowned,  Disown(originalElement), 3);

         if constexpr (Reffed) {
            REQUIRE(DenseCast(*originalElement).GetReferences() == (CT::Sparse<E> ? 8 : 1));
         }

         BenchmarkSetStd("Empty/AbsorbConstructor", 30, 100,
            T temp,                                (new (&temp) T{Absorb, piecewise1}),
            stdset temp_std1 (*originalElement);
            stdset temp_std2,                      new (&temp_std2) stdset {temp_std1}
         );
      }

      WHEN("Assigned compatible referred value") {
         auto assign_refer = [&](auto& a, [[maybe_unused]] const char* intent) {
            REQUIRE_NOTHROW(a.Assign(*element));

            if constexpr (CT::DeepDense<E>) {
               static_assert(CT::Deep<E> and CT::Dense<E>);
               static_assert(not ::std::same_as<E, int>);
               static_assert(not ::std::same_as<E, RT>);
               Many_CheckState_OwnedFull<TypeOf<E>>(*element);
            }

            Set_CheckState_OwnedFull<E>(a);
            Set_CheckState_ContainsOne(a, Refer(element));

            BenchmarkSetStd("Absorb/" + intent + "/Assign/Refer", 30, 100,
               a.Assign(*element),                 a.Assign(*originalElement),
               stdset temp_std (1, *element),      temp_std[0] = *originalElement
            );
         };

         assign_refer(pack_referred1, "Refer");
         assign_refer(pack_copied,    "Copy");
         assign_refer(pack_cloned,    "Clone");
         assign_refer(pack_moved1,    "Move");
         assign_refer(pack_abandoned, "Abandon");
         assign_refer(pack_disowned,  "Disown");
      }

      if constexpr (CT::Set<E>) { //TODO not tested yet
         WHEN("Assigned and absorbed referred container") {
            if (not pack_referred1.IsSame(element->GetType())) {
               auto misabsorb_refer = [&](auto& a, int uses) {
                  REQUIRE_THROWS(a.AssignAbsorb(*element));

                  Set_CheckState_ContainsOne(a, Refer(originalElement), uses);
               };

               misabsorb_refer(pack_referred1, 3);
               Set_CheckState_OwnedFull<E>(pack_referred1);

               misabsorb_refer(pack_referred2, 3);
               Set_CheckState_OwnedFull<E>(pack_referred2);

               misabsorb_refer(pack_copied,    1);
               Set_CheckState_OwnedFull<E>(pack_copied);

               misabsorb_refer(pack_cloned,    1);
               Set_CheckState_OwnedFull<E>(pack_cloned);

               misabsorb_refer(pack_moved1,    1);
               Set_CheckState_OwnedFull<E>(pack_moved1);

               misabsorb_refer(pack_moved2,    1);
               Set_CheckState_OwnedFull<E>(pack_moved2);

               misabsorb_refer(pack_abandoned, 1);
               Set_CheckState_OwnedFull<E>(pack_abandoned);

               misabsorb_refer(pack_disowned,  3);
               Set_CheckState_DisownedFull<E>(pack_disowned);
               return;
            }

            auto absorb_refer = [&](auto& a, [[maybe_unused]] const char* intent) {
               REQUIRE_NOTHROW(a.AssignAbsorb(*element));

               Set_Helper_TestSame(a, *element);
               REQUIRE(a.GetUses() == element->GetUses());
               REQUIRE(a.GetUses() == 2);
               REQUIRE(a.GetAllocation() == element->GetAllocation());

               BenchmarkSetStd("Absorb/" + intent + "/AssignAbsorb/Refer", 30, 100,
                  a.AssignAbsorb(*element),                 a.AssignAbsorb(*originalElement),
                  stdset temp_std1 (*element);
                  stdset temp_std2 (*originalElement),      temp_std1 = temp_std2
               );
            };

            absorb_refer(pack_referred1, "Refer");
            absorb_refer(pack_copied,    "Copy");
            absorb_refer(pack_cloned,    "Clone");
            absorb_refer(pack_moved1,    "Move");
            absorb_refer(pack_abandoned, "Abandon");
            absorb_refer(pack_disowned,  "Disown");
         }
      }
      
      WHEN("Assigned compatible cloned value") {
         auto assign_clone = [&](T& a, [[maybe_unused]] const char* intent) {
            REQUIRE_NOTHROW(a.Assign(Clone(*element)));

            if constexpr (CT::DeepDense<E>)
               Many_CheckState_OwnedFull<TypeOf<E>>(*element);
            Set_CheckState_OwnedFull<E>(a);
            Set_CheckState_ContainsOne(a, Clone(element));

            BenchmarkSetStd("Absorb/" + intent + "/Assign/Clone", 30, 100,
               a.Assign(Clone(*element)),       a.Assign(Clone(*originalElement)),
               stdset temp_std (*element),      temp_std[0] = *originalElement
            );
         };

         assign_clone(pack_referred1, "Refer");
         assign_clone(pack_copied,    "Copy");
         assign_clone(pack_cloned,    "Clone");
         assign_clone(pack_moved1,    "Move");
         assign_clone(pack_abandoned, "Abandon");
         assign_clone(pack_disowned,  "Disown");
      }

      if constexpr (CT::Set<E>) {
         WHEN("Assigned and absorbed cloned container") {
            if (not pack_referred1.IsSame(element->GetType())) {
               auto misabsorb_clone = [&](T& a) {
                  REQUIRE_THROWS(a.AssignAbsorb(Clone(*element)));
                  Set_CheckState_OwnedFull<E>(a);
                  Set_CheckState_ContainsOne(a, Clone(originalElement));
               };

               misabsorb_clone(pack_referred1);
               misabsorb_clone(pack_referred2);
               misabsorb_clone(pack_copied);
               misabsorb_clone(pack_cloned);
               misabsorb_clone(pack_moved1);
               misabsorb_clone(pack_moved2);
               misabsorb_clone(pack_abandoned);
               misabsorb_clone(pack_disowned);
               return;
            }

            auto absorb_clone = [&](T& a, [[maybe_unused]] const char* intent) {
               REQUIRE_NOTHROW(a.AssignAbsorb(Clone(*element)));

               Many_CheckState_OwnedFull<TypeOf<E>>(*element);
               Set_Helper_TestSame(a, *element);
               REQUIRE(a.GetUses() == 2);
               REQUIRE(a.GetAllocation() == element->GetAllocation());

               BenchmarkSetStd("Absorb/" + intent + "/AssignAbsorb/Clone", 30, 100,
                  a.AssignAbsorb(Clone(*element)),          a.AssignAbsorb(Clone(*originalElement)),
                  stdset temp_std1 (*element);
                  stdset temp_std2 (*originalElement),      temp_std1 = temp_std2
               );
            };

            absorb_clone(pack_referred1, "Refer");
            absorb_clone(pack_copied,    "Copy");
            absorb_clone(pack_cloned,    "Clone");
            absorb_clone(pack_moved1,    "Move");
            absorb_clone(pack_abandoned, "Abandon");
            absorb_clone(pack_disowned,  "Disown");
         }
      }

      WHEN("Assigned compatible copied value") {
         auto assign_copy = [&](T& a, [[maybe_unused]] const char* intent) {
            REQUIRE_NOTHROW(a.Assign(Copy(*element)));

            if constexpr (CT::DeepDense<E>)
               Many_CheckState_OwnedFull<TypeOf<E>>(*element);
            Set_CheckState_OwnedFull<E>(a);
            Set_CheckState_ContainsOne(a, Refer(element));

            BenchmarkSetStd("Absorb/" + intent + "/Assign/Copy", 30, 100,
               a.Assign(Copy(*element)),        a.Assign(Copy(*originalElement)),
               stdset temp_std (*element),      temp_std[0] = *originalElement
            );
         };

         assign_copy(pack_referred1, "Refer");
         assign_copy(pack_copied,    "Copy");
         assign_copy(pack_cloned,    "Clone");
         assign_copy(pack_moved1,    "Move");
         assign_copy(pack_abandoned, "Abandon");
         assign_copy(pack_disowned,  "Disown");
      }

      if constexpr (CT::Set<E>) {
         WHEN("Assigned and absorbed copied container") {
            if (not pack_referred1.IsSame(element->GetType())) {
               auto misabsorb_copy = [&](T& a) {
                  REQUIRE_THROWS(a.AssignAbsorb(Copy(*element)));
                  Set_CheckState_OwnedFull<E>(a);
                  Set_CheckState_ContainsOne(a, Refer(originalElement));
               };

               misabsorb_copy(pack_referred1);
               misabsorb_copy(pack_referred2);
               misabsorb_copy(pack_copied);
               misabsorb_copy(pack_cloned);
               misabsorb_copy(pack_moved1);
               misabsorb_copy(pack_moved2);
               misabsorb_copy(pack_abandoned);
               misabsorb_copy(pack_disowned);
               return;
            }

            auto absorb_copy = [&](T& a, [[maybe_unused]] const char* intent) {
               REQUIRE_NOTHROW(a.AssignAbsorb(Copy(*element)));

               Many_CheckState_OwnedFull<TypeOf<E>>(*element);
               Set_Helper_TestSame(a, *element);
               REQUIRE(a.GetUses() == 2);
               REQUIRE(a.GetAllocation() == element->GetAllocation());

               BenchmarkSetStd("Absorb/" + intent + "/AssignAbsorb/Copy", 30, 100,
                  a.AssignAbsorb(Copy(*element)),           a.AssignAbsorb(Copy(*originalElement)),
                  stdset temp_std1 (*element);
                  stdset temp_std2 (*originalElement),      temp_std1 = temp_std2
               );
            };

            absorb_copy(pack_referred1, "Refer");
            absorb_copy(pack_copied,    "Copy");
            absorb_copy(pack_cloned,    "Clone");
            absorb_copy(pack_moved1,    "Move");
            absorb_copy(pack_abandoned, "Abandon");
            absorb_copy(pack_disowned,  "Disown");
         }
      }

      WHEN("Assigned compatible moved value") {
         auto assign_move = [&](T& a, [[maybe_unused]] const char* intent) {
            auto movable = *element;
            REQUIRE_NOTHROW(a.Assign(::std::move(movable)));

            if constexpr (CT::DeepDense<E>)
               Many_CheckState_Default<TypeOf<E>>(movable);
            Set_CheckState_OwnedFull<E>(a);
            Set_CheckState_ContainsOne(a, Refer(element));

            BenchmarkSetStd("Absorb/" + intent + "/Assign/Move", 30, 100,
               auto movable1 = *element;
               auto movable2 = *originalElement;
               a.Assign(Move(movable1)),                    a.Assign(Move(movable2)),
               auto movable1 = *element;
               auto movable2 = *originalElement;
               stdset temp_std (::std::move(movable1)),     temp_std[0] = ::std::move(movable2)
            );
         };

         assign_move(pack_referred1, "Refer");
         assign_move(pack_copied,    "Copy");
         assign_move(pack_cloned,    "Clone");
         assign_move(pack_moved1,    "Move");
         assign_move(pack_abandoned, "Abandon");
         assign_move(pack_disowned,  "Disown");
      }

      if constexpr (CT::Set<E>) {
         WHEN("Assigned and absorbed moved container") {
            if (not pack_referred1.IsSame(element->GetType())) {
               auto misabsorb_move = [&](T& a) {
                  auto movable = *element;
                  REQUIRE_THROWS(a.AssignAbsorb(::std::move(movable)));

                  Set_CheckState_OwnedFull<E>(a);
                  Set_CheckState_ContainsOne(a, Refer(originalElement));
                  Many_CheckState_OwnedFull<int>(movable);
                  REQUIRE(movable.GetUses() == 2);
                  REQUIRE(movable.template As<int>() == 555);
               };

               misabsorb_move(pack_referred1);
               misabsorb_move(pack_referred2);
               misabsorb_move(pack_copied);
               misabsorb_move(pack_cloned);
               misabsorb_move(pack_moved1);
               misabsorb_move(pack_moved2);
               misabsorb_move(pack_abandoned);
               misabsorb_move(pack_disowned);
               return;
            };

            auto absorb_move = [&](T& a, [[maybe_unused]] const char* intent) {
               auto movable = *element;
               REQUIRE_NOTHROW(a.AssignAbsorb(::std::move(movable)));

               Many_CheckState_Default<TypeOf<E>>(movable);
               Set_Helper_TestSame(a, *element);
               REQUIRE(a.GetUses() == 2);
               REQUIRE(a.GetAllocation() == element->GetAllocation());

               BenchmarkSetStd("Absorb/" + intent + "/AssignAbsorb/Move", 30, 100,
                  T movable1 = *element;
                  T movable2 = *originalElement;
                  a.AssignAbsorb(Move(movable1)),        a.AssignAbsorb(Move(movable2)),
                  stdset movable1 (*element);
                  stdset movable2 (*originalElement),    movable1 = ::std::move(movable2)
               );
            };

            absorb_move(pack_referred1, "Refer");
            absorb_move(pack_copied,    "Copy");
            absorb_move(pack_cloned,    "Clone");
            absorb_move(pack_moved1,    "Move");
            absorb_move(pack_abandoned, "Abandon");
            absorb_move(pack_disowned,  "Disown");
         }
      }

      WHEN("Assigned compatible disowned value") {
         auto assign_disown = [&](T& a, [[maybe_unused]] const char* intent) {
            REQUIRE_NOTHROW(a.Assign(Disown(*element)));

            Set_CheckState_OwnedFull<E>(a);
            Set_CheckState_ContainsOne(a, Disown(element));

            BenchmarkSetStd("Absorb/" + intent + "/Assign/Disown", 30, 100,
               a.Assign(Disown(*element)),      a.Assign(Disown(*originalElement)),
               stdset temp_std (*element),      temp_std[0] = *originalElement
            );
         };

         assign_disown(pack_referred1, "Refer");
         assign_disown(pack_copied,    "Copy");
         assign_disown(pack_cloned,    "Clone");
         assign_disown(pack_moved1,    "Move");
         assign_disown(pack_abandoned, "Abandon");
         assign_disown(pack_disowned,  "Disown");
      }

      if constexpr (CT::Set<E>) {
         WHEN("Assigned and absorbed disowned container") {
            if (not pack_referred1.IsSame(element->GetType())) {
               auto misabsorb_disown = [&](T& a) {
                  REQUIRE_THROWS(a.AssignAbsorb(Disown(*element)));
                  Set_CheckState_OwnedFull<E>(a);
                  Set_CheckState_ContainsOne(a, Disown(originalElement));
               };

               misabsorb_disown(pack_referred1);
               misabsorb_disown(pack_referred2);
               misabsorb_disown(pack_copied);
               misabsorb_disown(pack_cloned);
               misabsorb_disown(pack_moved1);
               misabsorb_disown(pack_moved2);
               misabsorb_disown(pack_abandoned);
               misabsorb_disown(pack_disowned);
               return;
            }

            auto absorb_disown = [&](T& a, [[maybe_unused]] const char* intent) {
               REQUIRE_NOTHROW(a.AssignAbsorb(Disown(*element)));

               REQUIRE(a.GetRaw() == element->GetRaw());
               REQUIRE(a.IsExact(element->GetType()));
               REQUIRE(a == *element);
               REQUIRE(a.IsDeep() == element->IsDeep());
               REQUIRE(a.IsConstant() != element->IsConstant());
               REQUIRE(a.GetUnconstrainedState() == element->GetUnconstrainedState());
               REQUIRE(a.GetUses() == 0);
               REQUIRE_FALSE(a.GetAllocation());

               BenchmarkSetStd("Absorb/" + intent + "/AssignAbsorb/Disown", 30, 100,
                  a.AssignAbsorb(Disown(*element)),         a.AssignAbsorb(Disown(*originalElement)),
                  stdset temp_std1 (*element);
                  stdset temp_std2 (*originalElement),      temp_std1 = temp_std2
               );
            };

            absorb_disown(pack_referred1, "Refer");
            absorb_disown(pack_copied,    "Copy");
            absorb_disown(pack_cloned,    "Clone");
            absorb_disown(pack_moved1,    "Move");
            absorb_disown(pack_abandoned, "Abandon");
            absorb_disown(pack_disowned,  "Disown");
         }
      }
      
      WHEN("Assigned compatible abandoned value") {
         auto assign_abandon = [&](T& a, [[maybe_unused]] const char* intent) {
            auto movable = *element;
            REQUIRE_NOTHROW(a.Assign(Abandon(movable)));

            if constexpr (CT::DeepDense<E>)
               Many_CheckState_Abandoned<TypeOf<E>>(movable);
            Set_CheckState_OwnedFull<E>(a);
            Set_CheckState_ContainsOne(a, Refer(element));

            BenchmarkSetStd("Absorb/" + intent + "/Assign/Abandon", 30, 100,
               auto movable1 = *element;
               auto movable2 = *originalElement;
               a.Assign(Abandon(movable1)),                 a.Assign(Abandon(movable2)),
               auto movable1 = *element;
               auto movable2 = *originalElement;
               stdset temp_std (::std::move(movable1)),     temp_std[0] = ::std::move(movable2)
            );
         };

         assign_abandon(pack_referred1, "Refer");
         assign_abandon(pack_copied,    "Copy");
         assign_abandon(pack_cloned,    "Clone");
         assign_abandon(pack_moved1,    "Move");
         assign_abandon(pack_abandoned, "Abandon");
         assign_abandon(pack_disowned,  "Disown");
      }

      if constexpr (CT::Set<E>) {
         WHEN("Assigned and absorbed abandoned container") {
            if (not pack_referred1.IsSame(element->GetType())) {
               auto misabsorb_abandon = [&](T& a) {
                  auto movable = *element;
                  REQUIRE_THROWS(a.AssignAbsorb(Abandon(movable)));

                  Set_CheckState_OwnedFull<E>(a);
                  Set_CheckState_ContainsOne(a, Refer(originalElement));
                  Many_CheckState_OwnedFull<int>(movable);
                  REQUIRE(movable.GetUses() == 2);
               };

               misabsorb_abandon(pack_referred1);
               misabsorb_abandon(pack_referred2);
               misabsorb_abandon(pack_copied);
               misabsorb_abandon(pack_cloned);
               misabsorb_abandon(pack_moved1);
               misabsorb_abandon(pack_moved2);
               misabsorb_abandon(pack_abandoned);
               misabsorb_abandon(pack_disowned);
               return;
            }

            auto absorb_abandon = [&](T& a, [[maybe_unused]] const char* intent) {
               auto movable = *element;
               REQUIRE_NOTHROW(a.AssignAbsorb(Abandon(movable)));

               Many_CheckState_Abandoned<TypeOf<E>>(movable);
               Set_Helper_TestSame(a, *element);
               REQUIRE(a.GetUses() == 2);
               REQUIRE(a.GetAllocation() == element->GetAllocation());

               BenchmarkSetStd("Absorb/" + intent + "/AssignAbsorb/Abandon", 30, 100,
                  T movable1 = *element;
                  T movable2 = *originalElement;
                  a.AssignAbsorb(Abandon(movable1)),         a.AssignAbsorb(Abandon(movable2)),
                  stdset movable1 (*element);
                  stdset movable2 (*originalElement);
                  stdset temp_std = ::std::move(movable1),   temp_std = ::std::move(movable2)
               );
            };

            absorb_abandon(pack_referred1, "Refer");
            absorb_abandon(pack_copied,    "Copy");
            absorb_abandon(pack_cloned,    "Clone");
            absorb_abandon(pack_moved1,    "Move");
            absorb_abandon(pack_abandoned, "Abandon");
            absorb_abandon(pack_disowned,  "Disown");
         }
      }

      WHEN("Assigned compatible empty self") {
         auto assign_empty_self = [&](T& a) {
            REQUIRE_NOTHROW(a = T{});
            Set_CheckState_Default<E>(a);
         };

         assign_empty_self(pack_referred1);
         assign_empty_self(pack_referred2);
         assign_empty_self(pack_copied);
         assign_empty_self(pack_cloned);
         assign_empty_self(pack_moved1);
         assign_empty_self(pack_moved2);
         assign_empty_self(pack_abandoned);
         assign_empty_self(pack_disowned);
      }

      WHEN("Assigned compatible full self") {
         auto assign_full_self = [&](T& a, bool allow_change_in_constness = false) {
            auto backup = a;
            const auto uses_before = a.GetUses();
            LglsDisableWarningPush
            LglsDisableWarning_SelfAssign
               REQUIRE_NOTHROW(a = a);
            LglsDisableWarningPop
            Set_Helper_TestSame(a, backup, not allow_change_in_constness);
            REQUIRE(a.GetUses() == uses_before);
         };

         assign_full_self(pack_referred1);
         assign_full_self(pack_referred2);
         assign_full_self(pack_copied);
         assign_full_self(pack_cloned);
         assign_full_self(pack_moved1);
         assign_full_self(pack_moved2);
         assign_full_self(pack_abandoned);
         assign_full_self(pack_disowned, true);
      }

      WHEN("Absorbed by referral") {
         auto absorb_construct_refer = [&](T& a, T& compare_against, int uses) {
            T absorbed1 {a};
            T absorbed2 {Refer {a}};

            Set_Helper_TestSame(absorbed1, compare_against);
            Set_Helper_TestSame(absorbed2, compare_against);
            REQUIRE(absorbed1.GetUses() == uses);
            REQUIRE(absorbed2.GetUses() == uses);
         };

         absorb_construct_refer(pack_referred1, pack_referred1, 5);
         absorb_construct_refer(pack_referred2, pack_referred1, 5);
         absorb_construct_refer(pack_copied,    pack_copied,    3);
         absorb_construct_refer(pack_cloned,    pack_cloned,    3);
         absorb_construct_refer(pack_moved1,    pack_moved1,    3);
         absorb_construct_refer(pack_moved2,    pack_moved2,    3);
         absorb_construct_refer(pack_abandoned, pack_abandoned, 3);
         absorb_construct_refer(pack_disowned,  pack_referred1, 5);
      }
      
      WHEN("Absorbed by move") {
         auto absorb_construct_move = [&](T& a, int uses) {
            T backup = a;
            T absorbed {::std::move(a)};

            Set_CheckState_Default<E>(a);
            Set_CheckState_OwnedFull<E>(absorbed);
            Set_Helper_TestSame(absorbed, backup);
            REQUIRE(absorbed.GetUses() == uses);
         };

         absorb_construct_move(pack_referred1, 4);
         absorb_construct_move(pack_referred2, 3); // piecewise1 has been dereferenced twice in the prior call, because pack_referred1 was moved away
         absorb_construct_move(pack_copied,    2);
         absorb_construct_move(pack_cloned,    2);
         absorb_construct_move(pack_moved1,    2);
         absorb_construct_move(pack_moved2,    2);
         absorb_construct_move(pack_abandoned, 2);
         absorb_construct_move(pack_disowned,  3); // moving from a disowned container acts as referencing - nothing was owned prior
      }
      
      WHEN("Absorbed by move (alt)") {
         auto absorb_construct_move = [&](T& a, int uses) {
            T backup = a;
            T absorbed {Move(a)};

            Set_CheckState_Default<E>(a);
            Set_CheckState_OwnedFull<E>(absorbed);
            Set_Helper_TestSame(absorbed, backup);
            REQUIRE(absorbed.GetUses() == uses);
         };

         absorb_construct_move(pack_referred1, 4);
         absorb_construct_move(pack_referred2, 3);
         absorb_construct_move(pack_copied,    2);
         absorb_construct_move(pack_cloned,    2);
         absorb_construct_move(pack_moved1,    2);
         absorb_construct_move(pack_moved2,    2);
         absorb_construct_move(pack_abandoned, 2);
         absorb_construct_move(pack_disowned,  3);
      }
      
      WHEN("Absorbed by abandon") {
         auto absorb_construct_abandon = [&](T& a, int uses) {
            T backup = a;
            T absorbed {Abandon {a}};

            Set_CheckState_Abandoned<E>(a);
            Set_CheckState_OwnedFull<E>(absorbed);
            Set_Helper_TestSame(absorbed, backup);
            REQUIRE(absorbed.GetUses() == uses);
         };

         absorb_construct_abandon(pack_referred1, 4);
         absorb_construct_abandon(pack_referred2, 3);
         absorb_construct_abandon(pack_copied,    2);
         absorb_construct_abandon(pack_cloned,    2);
         absorb_construct_abandon(pack_moved1,    2);
         absorb_construct_abandon(pack_moved2,    2);
         absorb_construct_abandon(pack_abandoned, 2);
         absorb_construct_abandon(pack_disowned,  3); // abandoning from a disowned container acts as referencing - nothing was owned prior
      }
      
      WHEN("Absorbed by disown") {
         auto absorb_construct_disown = [&](T& a, int uses) {
            T absorbed {Disown {a}};

            Set_CheckState_OwnedFull<E>(a);
            Set_CheckState_DisownedFull<E>(absorbed);
            REQUIRE(absorbed.GetRaw() == a.GetRaw());
            REQUIRE(absorbed.IsExact(a.GetType()));
            REQUIRE(absorbed == a);
            REQUIRE(absorbed.IsDeep() == a.IsDeep());
            REQUIRE(absorbed.IsConstant() != a.IsConstant());
            REQUIRE(absorbed.GetUnconstrainedState() == a.GetUnconstrainedState());
            REQUIRE(absorbed.GetUses() == uses);
         };

         absorb_construct_disown(pack_referred1, 3);
         absorb_construct_disown(pack_referred2, 3);
         absorb_construct_disown(pack_copied,    1);
         absorb_construct_disown(pack_cloned,    1);
         absorb_construct_disown(pack_moved1,    1);
         absorb_construct_disown(pack_moved2,    1);
         absorb_construct_disown(pack_abandoned, 1);

         T absorbed{Disown {pack_disowned}};
         Set_CheckState_DisownedFull<E>(pack_disowned);
         Set_CheckState_DisownedFull<E>(absorbed);
         REQUIRE(absorbed.GetRaw() == pack_disowned.GetRaw());
         REQUIRE(absorbed.IsExact(pack_disowned.GetType()));
         REQUIRE(absorbed == pack_disowned);
         REQUIRE(absorbed.IsDeep() == pack_disowned.IsDeep());
         REQUIRE(absorbed.IsConstant() == pack_disowned.IsConstant());
         REQUIRE(absorbed.GetUnconstrainedState() == pack_disowned.GetUnconstrainedState());
         REQUIRE(absorbed.GetUses() == 3);
      }
      
      WHEN("Absorbed by copy") {
         const bool managed_sparse = Sparse and Managed;
         auto absorb_construct_copy = [&](T& a, int uses, int entry_refs, int indi_refs) {
            T absorbed {Copy {a}};

            REQUIRE(a.GetUses() == uses);
            Set_CheckState_OwnedFull<E>(absorbed);
            REQUIRE(absorbed.GetUses() == 1);
            REQUIRE(absorbed == a);
            REQUIRE(absorbed.GetRaw() != a.GetRaw());
            REQUIRE(absorbed.template AsAt<E>(0) == a.template AsAt<E>(0));

            if constexpr (Sparse) {
               auto entry = *absorbed.GetEntries();

               if (entry)
                  REQUIRE(entry->GetUses() == entry_refs);

               if constexpr (Reffed) {
                  auto e = absorbed.template AsAt<E>(0);
                  REQUIRE(DenseCast(e).GetReferences() == indi_refs);
               }
            }
         };

         absorb_construct_copy(pack_referred1, 3, managed_sparse ? 9 : 3, 9);
         Set_CheckState_OwnedFull<E>(pack_referred1);

         absorb_construct_copy(pack_referred2, 3, managed_sparse ? 9 : 3, 9);
         Set_CheckState_OwnedFull<E>(pack_referred2);

         absorb_construct_copy(pack_copied,    1, managed_sparse ? 9 : 3, 9);
         Set_CheckState_OwnedFull<E>(pack_copied);

         absorb_construct_copy(pack_cloned,    1, 2, 2);
         Set_CheckState_OwnedFull<E>(pack_cloned);

         absorb_construct_copy(pack_moved1,    1, managed_sparse ? 9 : 1, 9);
         Set_CheckState_OwnedFull<E>(pack_moved1);

         absorb_construct_copy(pack_moved2,    1, managed_sparse ? 9 : 1, 9);
         Set_CheckState_OwnedFull<E>(pack_moved2);

         absorb_construct_copy(pack_abandoned, 1, managed_sparse ? 9 : 1, 9);
         Set_CheckState_OwnedFull<E>(pack_abandoned);

         absorb_construct_copy(pack_disowned,  3, managed_sparse ? 9 : 0, 9);
         Set_CheckState_DisownedFull<E>(pack_disowned);
      }
      
      WHEN("Absorbed by clone") {
         auto absorb_construct_clone = [&](T& a, int uses) {
            T absorbed {Clone {a}};

            if (uses == 0)
               Set_CheckState_DisownedFull<E>(a);
            else
               Set_CheckState_OwnedFull<E>(a);

            Set_CheckState_OwnedFull<E>(absorbed);
            REQUIRE((absorbed == a) == CT::Dense<E>);
            REQUIRE(absorbed.GetUses() == 1);
         };

         absorb_construct_clone(pack_referred1, 1);
         absorb_construct_clone(pack_referred2, 1);
         absorb_construct_clone(pack_copied,    1);
         absorb_construct_clone(pack_cloned,    1);
         absorb_construct_clone(pack_moved1,    1);
         absorb_construct_clone(pack_moved2,    1);
         absorb_construct_clone(pack_abandoned, 1);
         absorb_construct_clone(pack_disowned,  0);
      }
      
      /// MARK: Clear                                                         
      WHEN("Cleared") {
         auto clear_full = [&](T& a, [[maybe_unused]] const char* intent, int uses = 1) {
            BenchmarkSetStd("Absorb/" + intent + "/Clear", 30, 100,
               T temp = a,                      temp.Clear(),
               stdset temp_std (*element),      temp_std.clear()
            );

            REQUIRE_NOTHROW(a.Clear());

            if (uses != 1)
               Set_CheckState_Default<E>(a, true);
            else
               Set_CheckState_OwnedEmpty<E>(a);
         };

         clear_full(pack_referred1, "Refer", 3);
         clear_full(pack_copied,    "Copy");
         clear_full(pack_cloned,    "Clone");
         clear_full(pack_moved1,    "Move");
         clear_full(pack_abandoned, "Abandon");
         clear_full(pack_disowned,  "Disown", 0);
      }

      /// MARK: Reset                                                         
      WHEN("Reset") {
         auto reset_full = [&](T& a, [[maybe_unused]] const char* intent) {
            BenchmarkSetStd("Absorb/" + intent + "/Reset", 30, 100,
               T temp = a,                         temp.Reset(),
               stdset temp_std (*element),         temp_std.clear()
            );

            REQUIRE_NOTHROW(a.Reset());

            Set_CheckState_Default<E>(a);
         };

         reset_full(pack_referred1, "Refer");
         reset_full(pack_copied,    "Copy");
         reset_full(pack_cloned,    "Clone");
         reset_full(pack_moved1,    "Move");
         reset_full(pack_abandoned, "Abandon");
         reset_full(pack_disowned,  "Disown");
      }

      if constexpr (LANGULUS_FEATURE(MANAGED_MEMORY) and not CT::Container<E>) {
         // Works only if E doesn't move entries around                 
         WHEN("Reset, and then immediately allocated again") {
            auto reset_and_reallocate = [&](T& a) {
               const auto memory = a.GetRaw();
               a.Reset();
               a = *element;
               REQUIRE(a.GetRaw() == memory);
            };

            //reset_and_reallocate(pack_referred1); // referred too many times to be deallocated
            //reset_and_reallocate(pack_referred2); // referred too many times to be deallocated
            reset_and_reallocate(pack_copied);
            reset_and_reallocate(pack_cloned);
            reset_and_reallocate(pack_moved1);
            reset_and_reallocate(pack_moved2);
            reset_and_reallocate(pack_abandoned);
            //reset_and_reallocate(pack_disowned); // likely to be reallocated in a new place due to lack of authority on the original memory
         }
      }

      /// MARK: Compare                                                       
      WHEN("Compared") {
         ScopedE e1 {1};
         T another_pack1 {Piecewise, *e1};
         T defaulted_pack;

         auto compared_full = [&](T& a, [[maybe_unused]] const char* intent) {
            T same_pack {a};

            REQUIRE      (a != another_pack1);
            REQUIRE_FALSE(a == another_pack1);
            REQUIRE      (a != defaulted_pack);
            REQUIRE_FALSE(a == defaulted_pack);
            REQUIRE      (a == same_pack);
            REQUIRE_FALSE(a != same_pack);

            [[maybe_unused]] volatile bool dont_optimize = false;
            BenchmarkSetStd("Absorb/" + intent + "/operator==", 30, 100,
               (void) 0,                           dont_optimize |= (a == same_pack),
               stdset a_std (*element);
               stdset another_pack1_std (*e1),     dont_optimize |= (a_std == another_pack1_std)
            );
            BenchmarkSetStd("Absorb/" + intent + "/operator!=", 30, 100,
               (void) 0,                           dont_optimize |= (a != same_pack),
               stdset a_std (*element);
               stdset another_pack1_std (*e1),     dont_optimize |= (a_std != another_pack1_std)
            );
         };

         compared_full(pack_referred1, "Refer");
         compared_full(pack_copied,    "Copy");
         compared_full(pack_cloned,    "Clone");
         compared_full(pack_moved1,    "Move");
         compared_full(pack_abandoned, "Abandon");
         compared_full(pack_disowned,  "Disown");
      }

      /// MARK: Contains                                                      
      WHEN("Contains when full") {
         ScopedE e1 {1};
         
         auto contains_full = [&](auto& a) {
            DumpSet(a);
            REQUIRE      (a.Contains(*originalElement));
            REQUIRE_FALSE(a.Contains(*e1));
         };

         contains_full(pack_referred1);
         contains_full(pack_referred2);
         contains_full(pack_copied);

         if constexpr (Sparse) {
            REQUIRE      (*pack_cloned.GetDenseAt(0).template As<Decay<E>>() == DenseCast(*originalElement));
            REQUIRE_FALSE(*pack_cloned.GetDenseAt(0).template As<E>() == *originalElement);
            REQUIRE_FALSE(*pack_cloned.GetDenseAt(0).template As<E>() == *e1);
         }
         else contains_full(pack_cloned);

         contains_full(pack_moved1);
         contains_full(pack_moved2);
         contains_full(pack_abandoned);
         contains_full(pack_disowned);

         [[maybe_unused]] volatile bool dont_optimize = false;
         BenchmarkSet("Absorb/Contains", 30,
            (void) 0, dont_optimize |= pack_referred1.Contains(*element)
         );
      }
   }

   GIVEN("Two absorb-constructed containers") {
      const ScopedE e556 {556};
      const ScopedE e6   {6};

      T piecewise1{Piecewise, *e556};
      T piecewise2{Piecewise, *e6};
      T src {Absorb, Abandon(piecewise1)};
      T dst {Absorb, Abandon(piecewise2)};

      /// MARK: GetHandle                                                     
      WHEN("GetHandle is called on mutable container") {
         auto src_handle_default = src.GetHandle();
         if constexpr (CT::TypeErased<T>)
            static_assert(::std::same_as<decltype(src_handle_default), Handle>);
         else
            static_assert(::std::same_as<decltype(src_handle_default), THandle<ConstAll<E&>>>);

         auto src_handle = src_handle_default.ForceMutable();
         if constexpr (CT::TypeErased<T>)
            static_assert(::std::same_as<decltype(src_handle), HandleMut>);
         else
            static_assert(::std::same_as<decltype(src_handle), THandle<E&>>);

         auto src_valid_element = src.GetAt(0);
         while(src_handle.Get() != src_valid_element)
            ++src_handle;

         auto src_data = src_handle.template Get<E>();
         AllocationPtr const* src_entries = nullptr;

         Handle_CheckState_OwnedFull<E>(src_handle);

         if constexpr (Sparse) {
            src_entries = src_handle.GetEntries();
            REQUIRE(*src_entries == e556.entries[1]);
            if constexpr (Managed)
               REQUIRE(e556.entries[1]->GetUses() == 2);
         }

         if constexpr (Reffed) {
            REQUIRE(DenseCast(src_data).GetReferences() == (Sparse ? 2 : 1));
            REQUIRE(DenseCast(src_data).destroyed == false);
         }

         auto dst_handle = dst.GetHandle().ForceMutable();
         auto dst_valid_element = dst.GetAt(0);
         while(dst_handle.Get() != dst_valid_element)
            ++dst_handle;

         auto dst_data = dst_handle.template Get<E>();
         AllocationPtr const* dst_entries = nullptr;

         Handle_CheckState_OwnedFull<E>(dst_handle);

         if constexpr (Sparse) {
            dst_entries = dst_handle.GetEntries();
            REQUIRE(*dst_entries == e6.entries[1]);
            if constexpr (Managed)
               REQUIRE(e6.entries[1]->GetUses() == 2);
            REQUIRE(dst_entries != src_entries);
         }

         REQUIRE(dst_data != src_data);

         if constexpr (Reffed) {
            REQUIRE(DenseCast(dst_data).GetReferences() == (Sparse ? 2 : 1));
            REQUIRE(DenseCast(dst_data).destroyed == false);
         }

         THEN("Handle assigned to another container") {
            REQUIRE_NOTHROW(dst_handle.Assign(Move(src_handle)));

            Handle_CheckState_OwnedFull<E>(src_handle);
            Handle_CheckState_OwnedFull<E>(dst_handle);
            REQUIRE(src_handle.template Get<E>() == src_data);
            REQUIRE(dst_handle.template Get<E>() == dst_data);
            
            auto& moved_in = DenseCast(dst_data);
            if constexpr (Sparse) {
               REQUIRE(src_handle.GetEntries() == src_entries);
               REQUIRE(*src_data == nullptr);
               REQUIRE(*src_entries == nullptr);

               REQUIRE(dst_handle.GetEntries() == dst_entries);
               REQUIRE(*dst_data == *e556);
               REQUIRE(*dst_entries == e556.entries[1]);
               if constexpr (Managed) {
                  REQUIRE(e556.entries[1]->GetUses() == 2);
                  REQUIRE(e6.entries[1]->GetUses() == 1);
               }

               if constexpr (Reffed) {
                  REQUIRE(DenseCast(*e6).GetReferences() == 1);
                  REQUIRE(moved_in.GetReferences() == 2);
                  REQUIRE(moved_in.data == DenseCast(*e556).data);
                  REQUIRE(moved_in.destroyed == false);
                  REQUIRE(moved_in.moved_in == false);
                  REQUIRE(moved_in.moved_out == false);
               }
            }
            else {
               auto& moved_out = DenseCast(src_data);
               if constexpr (Reffed) {
                  REQUIRE(moved_out.GetReferences() == 1);
                  REQUIRE(moved_out.destroyed == false);
                  REQUIRE(moved_out.moved_in == false);
                  REQUIRE(moved_out.moved_out == true);

                  REQUIRE(moved_in.GetReferences() == 1);
                  REQUIRE(moved_in.data == DenseCast(*e556).data);
                  REQUIRE(moved_in.destroyed == false);
                  REQUIRE(moved_in.moved_in == true);
                  REQUIRE(moved_in.moved_out == false);
               }
            }
         }
         
         THEN("Handle is swapped with another container's handle") {
            REQUIRE_NOTHROW(dst_handle.SwapContents(src_handle));

            Handle_CheckState_OwnedFull<E>(src_handle);
            Handle_CheckState_OwnedFull<E>(dst_handle);
            REQUIRE(src_handle.template Get<E>() == src_data);
            REQUIRE(dst_handle.template Get<E>() == dst_data);
            
            auto& moved_in  = DenseCast(dst_data);
            auto& moved_out = DenseCast(src_data);

            REQUIRE(moved_in  == DenseCast(*e556));
            REQUIRE(moved_out == DenseCast(*e6));

            if constexpr (Sparse) {
               REQUIRE(src_handle.GetEntries() == src_entries);
               REQUIRE(dst_handle.GetEntries() == dst_entries);
   
               REQUIRE(*dst_entries == e556.entries[1]);
               REQUIRE(*src_entries == e6.entries[1]);
               if constexpr (Managed) {
                  REQUIRE(e556.entries[1]->GetUses() == 2);
                  REQUIRE(e6.entries[1]->GetUses() == 2);
               }

               if constexpr (Reffed) {
                  REQUIRE(moved_out.GetReferences() == 2);
                  REQUIRE(moved_out.data == DenseCast(*e6).data);
                  REQUIRE(moved_out.destroyed == false);
                  REQUIRE(moved_out.moved_in == false);
                  REQUIRE(moved_out.moved_out == false);

                  REQUIRE(moved_in.GetReferences() == 2);
                  REQUIRE(moved_in.data == DenseCast(*e556).data);
                  REQUIRE(moved_in.destroyed == false);
                  REQUIRE(moved_in.moved_in == false);
                  REQUIRE(moved_in.moved_out == false);
               }
            }
            else {
               if constexpr (Reffed) {
                  REQUIRE(moved_out.GetReferences() == 1);
                  REQUIRE(moved_out.destroyed == false);
                  REQUIRE(moved_out.moved_in == true);
                  REQUIRE(moved_out.moved_out == false);

                  REQUIRE(moved_in.GetReferences() == 1);
                  REQUIRE(moved_in.destroyed == false);
                  REQUIRE(moved_in.moved_in == true);
                  REQUIRE(moved_in.moved_out == false);
               }
            }

            // We should be able to do this indefinitely                
            for(int i = 0; i < 101; ++i)
               dst_handle.SwapContents(src_handle);
         }
         
         #if not LANGULUS(FORCE_TYPE_ERASURE)
         THEN("Handle moved into a local handle") {
            THandle<E> local {Absorb, Move(src_handle)};

            Handle_CheckState_OwnedFull<E>(src_handle);
            Handle_CheckState_OwnedFull<E>(local);
            REQUIRE(src_handle.template Get<E>() == src_data);
            REQUIRE(local.template Get<E>() != src_data);
            
            auto& moved_in = DenseCast(local.template Get<E>());
            REQUIRE(moved_in == DenseCast(*e556));

            if constexpr (Sparse) {
               REQUIRE(src_handle.GetEntries() == src_entries);
               REQUIRE(local.GetEntries() != src_entries);

               REQUIRE(*src_data == nullptr);
               REQUIRE(*src_entries == nullptr);

               REQUIRE(local.GetEntries()[0] == e556.entries[1]);
               if constexpr (Managed) {
                  REQUIRE(e556.entries[1]->GetUses() == 2);
                  REQUIRE(e6.entries[1]->GetUses() == 2);
               }

               if constexpr (Reffed) {
                  REQUIRE(DenseCast(*e6).GetReferences() == 2);
                  REQUIRE(moved_in.GetReferences() == 2);
                  REQUIRE(moved_in.data == DenseCast(*e556).data);
                  REQUIRE(moved_in.destroyed == false);
                  REQUIRE(moved_in.moved_in == false);
                  REQUIRE(moved_in.moved_out == false);
               }
            }
            else {
               auto& moved_out = DenseCast(src_data);

               if constexpr (Reffed) {
                  REQUIRE(moved_out.GetReferences() == 1);
                  REQUIRE(moved_out.destroyed == false);
                  REQUIRE(moved_out.moved_in == false);
                  REQUIRE(moved_out.moved_out == true);

                  REQUIRE(moved_in.GetReferences() == 1);
                  REQUIRE(moved_in.data == DenseCast(*e556).data);
                  REQUIRE(moved_in.destroyed == false);
                  REQUIRE(moved_in.moved_in == true);
                  REQUIRE(moved_in.moved_out == false);
               }
            }
         }

         THEN("Handle is swapped with local handle, and then back to container") {
            THandle<E> local;
            REQUIRE_NOTHROW(local.SwapContents(src_handle));
            auto local_data = local.template Get<E>();
            AllocationPtr const* local_entries = nullptr;

            Handle_CheckState_OwnedFull<E>(src_handle);
            Handle_CheckState_OwnedFull<E>(local);
            REQUIRE(src_handle.template Get<E>() == src_data);
            REQUIRE(local_data);
            REQUIRE(local_data != src_data);

            auto& moved_in = DenseCast(local_data);
            REQUIRE(moved_in == DenseCast(*e556));

            if constexpr (Sparse) {
               REQUIRE(src_handle.GetEntries() == src_entries);
               local_entries = local.GetEntries();
               REQUIRE(local_entries != nullptr);
               REQUIRE(local_entries != src_entries);

               REQUIRE(*src_data == nullptr);
               REQUIRE(*src_entries == nullptr);

               REQUIRE(*local_entries == e556.entries[1]);
               if constexpr (Managed)
                  REQUIRE(e556.entries[1]->GetUses() == 2);

               if constexpr (Reffed) {
                  REQUIRE(moved_in.GetReferences() == 2);
                  REQUIRE(moved_in.data == DenseCast(*e556).data);
                  REQUIRE(moved_in.destroyed == false);
                  REQUIRE(moved_in.moved_in == false);
                  REQUIRE(moved_in.moved_out == false);
               }
            }
            else {
               auto& moved_out = DenseCast(src_data);

               if constexpr (Reffed) {
                  REQUIRE(moved_out.GetReferences() == 1);
                  REQUIRE(moved_out.destroyed == false);
                  REQUIRE(moved_out.moved_in == true);
                  REQUIRE(moved_out.moved_out == false);

                  REQUIRE(moved_in.GetReferences() == 1);
                  REQUIRE(moved_in.data == DenseCast(*e556).data);
                  REQUIRE(moved_in.destroyed == false);
                  REQUIRE(moved_in.moved_in == true);
                  REQUIRE(moved_in.moved_out == false);
               }
            }

            REQUIRE_NOTHROW(local.SwapContents(src_handle));
            REQUIRE(src_handle.template Get<E>() == src_data);
            REQUIRE(local.template Get<E>() == local_data);

            if constexpr (Sparse) {
               REQUIRE(src_handle.GetEntries() == src_entries);
               REQUIRE(local.GetEntries() != src_entries);
               REQUIRE(local.GetEntries() == local_entries);
               REQUIRE(*local_entries == nullptr);
               REQUIRE(*src_data != nullptr);

               REQUIRE(*src_entries == e556.entries[1]);
               if constexpr (Managed)
                  REQUIRE(e556.entries[1]->GetUses() == 2);

               if constexpr (Reffed) {
                  auto& moved_out = DenseCast(src_data);
                  REQUIRE(moved_out.GetReferences() == 2);
                  REQUIRE(moved_out.data == DenseCast(*e556).data);
                  REQUIRE(moved_out.destroyed == false);
                  REQUIRE(moved_out.moved_in == false);
                  REQUIRE(moved_out.moved_out == false);
               }
            }
            else {
               auto& moved_out = DenseCast(src_data);

               if constexpr (Reffed) {
                  REQUIRE(moved_in.GetReferences() == 1);
                  REQUIRE(moved_in.destroyed == false);
                  REQUIRE(moved_in.moved_in == true);
                  REQUIRE(moved_in.moved_out == false);

                  REQUIRE(moved_out.GetReferences() == 1);
                  REQUIRE(moved_out.data == DenseCast(*e556).data);
                  REQUIRE(moved_out.destroyed == false);
                  REQUIRE(moved_out.moved_in == true);
                  REQUIRE(moved_out.moved_out == false);
               }
            }

            // We should be able to do this indefinitely                
            for(int i = 0; i < 101; ++i)
               local.SwapContents(src_handle);
         }
         #endif
      }

      WHEN("GetHandle is called on constant container") {
         T const& pack_constant = src;
         auto handle = pack_constant.GetHandle();
         auto first_valid_element = pack_constant.GetAt(0);
         while(handle.Get() != first_valid_element)
            ++handle;

         if constexpr (CT::TypeErased<T>)
            static_assert(::std::same_as<decltype(handle), Handle>);
         else
            static_assert(::std::same_as<decltype(handle), THandle<ConstAll<E&>>>);
            
         Handle_CheckState_OwnedFull<E const>(handle);
         
         if constexpr (Sparse) {
            auto entries = handle.GetEntries();
            REQUIRE(entries);
            REQUIRE(*entries == e556.entries[1]);
            if constexpr (Managed)
               REQUIRE(e556.entries[1]->GetUses() == 2);
         }

         if constexpr (Reffed) {
            auto& data = DenseCast(handle.template Get<E>());
            REQUIRE(data.GetReferences() == (Sparse ? 2 : 1));
            REQUIRE(data.destroyed == false);
         }
      }
   }

   REQUIRE(memoryState.Assert());
   REQUIRE_FALSE(Allocator::CollectGarbage());
}
