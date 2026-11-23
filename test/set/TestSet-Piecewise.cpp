///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "TestSetCommon.hpp"


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


TEST_CASE_TEMPLATE("Test piecewise-constructed Set/TSet", TestType
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
   constexpr bool Ambiguous = not Same<T, E> and CT::Set<E> and LANGULUS(SAFE);

   #if LANGULUS(BENCHMARK)
      using stdset = ::std::unordered_set<E>;
   #endif

   if constexpr (Ambiguous) {
      GIVEN("Piecewise-constructed container (ambiguously)") {
         ScopedE element {555};
         REQUIRE_THROWS(T {*element});      
         REQUIRE_THROWS(T {Refer(*element)});
         REQUIRE_THROWS(T {Copy(*element)});
         REQUIRE_THROWS(T {Clone(*element)});
         REQUIRE_THROWS(T {::std::move(*element)});
         REQUIRE_THROWS(T {Move(*element)});
         REQUIRE_THROWS(T {Abandon(*element)});
         REQUIRE_THROWS(T {Disown(*element)});
      }
   }

   GIVEN("Piecewise-constructed container") {
      const ScopedE originalElement {556};
      const ScopedE element {555};
      auto originalElement_movable1 = *originalElement;
      auto originalElement_movable2 = *originalElement;
      auto originalElement_movable3 = *originalElement;

      T pack_referred1{Piecewise,             *originalElement };
      T pack_referred2{Piecewise,       Refer(*originalElement)};
      T pack_copied   {Piecewise,        Copy(*originalElement)};
      T pack_cloned   {Piecewise,       Clone(*originalElement)};
      T pack_moved1   {Piecewise, ::std::move( originalElement_movable1)};
      T pack_moved2   {Piecewise,        Move( originalElement_movable2)};
      T pack_abandoned{Piecewise,     Abandon( originalElement_movable3)};
      T pack_disowned {Piecewise,      Disown(*originalElement)};

      WHEN("Value-constructed") {
         Set_CheckState_OwnedFull<E>(pack_referred1);
         Set_CheckState_OwnedFull<E>(pack_referred2);
         Set_CheckState_OwnedFull<E>(pack_copied);
         Set_CheckState_OwnedFull<E>(pack_cloned);
         Set_CheckState_OwnedFull<E>(pack_moved1);
         Set_CheckState_OwnedFull<E>(pack_moved2);
         Set_CheckState_OwnedFull<E>(pack_abandoned);
         Set_CheckState_OwnedFull<E>(pack_disowned);

         Set_CheckState_ContainsOne(pack_referred1, Refer(originalElement));
         Set_CheckState_ContainsOne(pack_referred2, Refer(originalElement));
         Set_CheckState_ContainsOne(pack_copied,    Refer(originalElement));
         Set_CheckState_ContainsOne(pack_cloned,    Clone(originalElement));
         Set_CheckState_ContainsOne(pack_moved1,    Refer(originalElement));
         Set_CheckState_ContainsOne(pack_abandoned, Refer(originalElement));
         Set_CheckState_ContainsOne(pack_disowned,  Disown(originalElement));

         BenchmarkSetStd("Empty/PiecewiseConstructor", 30, 400,
            T temp,                 (new (&temp)     T{Piecewise, *originalElement}),
            stdset temp_std,         new (&temp_std) stdset{*originalElement}
         );
      }

      WHEN("Assigned compatible referred value") {
         auto assign_refer = [&](T& a, [[maybe_unused]] const char* intent) {
            a.Assign(*element);

            if constexpr (CT::DeepDense<E>)
               Set_CheckState_OwnedFull<TypeOf<E>>(*element);
            Set_CheckState_OwnedFull<E>(a);
            Set_CheckState_ContainsOne(a, Refer(element));

            BenchmarkSetStd("Piecewise/" + intent + "/Assign/Refer", 30, 100,
               a.Assign(*element),              a.Assign(*originalElement),
               stdset temp_std (*element),      temp_std[0] = *originalElement
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
               auto misabsorb_refer = [&](T& a) {
                  REQUIRE_THROWS(a.AssignAbsorb(*element));
                  Set_CheckState_OwnedFull<E>(a);
                  Set_CheckState_ContainsOne(a, Refer(originalElement));
               };

               misabsorb_refer(pack_referred1);
               misabsorb_refer(pack_referred2);
               misabsorb_refer(pack_copied);
               misabsorb_refer(pack_cloned);
               misabsorb_refer(pack_moved1);
               misabsorb_refer(pack_moved2);
               misabsorb_refer(pack_abandoned);
               misabsorb_refer(pack_disowned);
               return;
            }

            auto absorb_refer = [&](T& a, [[maybe_unused]] const char* intent) {
               a.AssignAbsorb(*element);

               Set_Helper_TestSame(a, *element);
               REQUIRE(a.GetUses() == element->GetUses());
               REQUIRE(a.GetUses() == 2);
               REQUIRE(a.GetAllocation() == element->GetAllocation());

               BenchmarkSetStd("Piecewise/" + intent + "/AssignAbsorb/Refer", 30, 100,
                  a.AssignAbsorb(*element),              a.AssignAbsorb(*originalElement),
                  stdset temp_std1 (*element);
                  stdset temp_std2 (*originalElement),   temp_std1 = temp_std2
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
            a.Assign(Clone(*element));

            if constexpr (CT::DeepDense<E>)
               Set_CheckState_OwnedFull<TypeOf<E>>(*element);
            Set_CheckState_OwnedFull<E>(a);
            Set_CheckState_ContainsOne(a, Clone(element));

            BenchmarkSetStd("Piecewise/" + intent + "/Assign/Clone", 30, 100,
               a.Assign(Clone(*element)),    a.Assign(Clone(*originalElement)),
               stdset temp_std (*element),   temp_std[0] = *originalElement
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
               a.AssignAbsorb(Clone(*element));

               Set_CheckState_OwnedFull<TypeOf<E>>(*element);
               Set_Helper_TestSame(a, *element);
               REQUIRE(a.GetUses() == 2);
               REQUIRE(a.GetAllocation() == element->GetAllocation());

               BenchmarkSetStd("Piecewise/" + intent + "/AssignAbsorb/Clone", 30, 100,
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
            a.Assign(Copy(*element));

            if constexpr (CT::DeepDense<E>)
               Set_CheckState_OwnedFull<TypeOf<E>>(*element);
            Set_CheckState_OwnedFull<E>(a);
            Set_CheckState_ContainsOne(a, Refer(element));

            BenchmarkSetStd("Piecewise/" + intent + "/Assign/Copy", 30, 100,
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
               a.AssignAbsorb(Copy(*element));

               Set_CheckState_OwnedFull<TypeOf<E>>(*element);
               Set_Helper_TestSame(a, *element);
               REQUIRE(a.GetUses() == 2);
               REQUIRE(a.GetAllocation() == element->GetAllocation());

               BenchmarkSetStd("Piecewise/" + intent + "/AssignAbsorb/Copy", 30, 100,
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
            a.Assign(::std::move(movable));

            if constexpr (CT::DeepDense<E>)
               Many_CheckState_Default<TypeOf<E>>(movable);
            Set_CheckState_OwnedFull<E>(a);
            Set_CheckState_ContainsOne(a, Refer(element));

            BenchmarkSetStd("Piecewise/" + intent + "/Assign/Move", 30, 100,
               auto movable1 = *element;
               auto movable2 = *originalElement;
               a.Assign(Move(movable1)),                  a.Assign(Move(movable2)),
               auto movable1 = *element;
               auto movable2 = *originalElement;
               stdset temp_std (::std::move(movable1)),   temp_std[0] = ::std::move(movable2)
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
                  Set_CheckState_OwnedFull<int>(movable);
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
               a.AssignAbsorb(::std::move(movable));

               Set_CheckState_Default<TypeOf<E>>(movable);
               Set_Helper_TestSame(a, *element);
               REQUIRE(a.GetUses() == 2);
               REQUIRE(a.GetAllocation() == element->GetAllocation());

               BenchmarkSetStd("Piecewise/" + intent + "/AssignAbsorb/Move", 30, 100,
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
            a.Assign(Disown(*element));

            Set_CheckState_OwnedFull<E>(a);
            Set_CheckState_ContainsOne(a, Disown(element));

            BenchmarkSetStd("Piecewise/" + intent + "/Assign/Disown", 30, 100,
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
               a.AssignAbsorb(Disown(*element));

               REQUIRE(a.GetRaw() == element->GetRaw());
               REQUIRE(a.IsExact(element->GetType()));
               REQUIRE(a == *element);
               REQUIRE(a.IsDeep() == element->IsDeep());
               REQUIRE(a.IsConstant() != element->IsConstant());
               REQUIRE(a.GetUnconstrainedState() == element->GetUnconstrainedState());
               REQUIRE(a.GetUses() == 0);
               REQUIRE_FALSE(a.GetAllocation());

               BenchmarkSetStd("Piecewise/" + intent + "/AssignAbsorb/Disown", 30, 100,
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
            a.Assign(Abandon(movable));

            if constexpr (CT::DeepDense<E>)
               Set_CheckState_Abandoned<TypeOf<E>>(movable);
            Set_CheckState_OwnedFull<E>(a);
            Set_CheckState_ContainsOne(a, Refer(element));

            BenchmarkSetStd("Piecewise/" + intent + "/Assign/Abandon", 30, 100,
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
                  Set_CheckState_OwnedFull<int>(movable);
                  REQUIRE(movable.GetUses() == 2);
                  REQUIRE(movable.template As<int>() == 555);
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
               a.AssignAbsorb(Abandon(movable));

               Set_CheckState_Abandoned<TypeOf<E>>(movable);
               Set_Helper_TestSame(a, *element);
               REQUIRE(a.GetUses() == 2);
               REQUIRE(a.GetAllocation() == element->GetAllocation());

               BenchmarkSetStd("Piecewise/" + intent + "/AssignAbsorb/Abandon", 30, 100,
                  T movable1 = *element;
                  T movable2 = *originalElement;
                  a.AssignAbsorb(Abandon(movable1)),           a.AssignAbsorb(Abandon(movable2)),
                  stdset movable1 (*element);
                  stdset movable2 (*originalElement);
                  stdset temp_std = ::std::move(movable1),     temp_std = ::std::move(movable2)
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
            a = T{};
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
         auto assign_full_self = [&](T& a) {
            auto backup = a;
            const auto uses_before = a.GetUses();
            LglsDisableWarningPush
            LglsDisableWarning_SelfAssign
               a = a;
            LglsDisableWarningPop
            Set_Helper_TestSame(a, backup);
            REQUIRE(a.GetUses() == uses_before);
         };

         assign_full_self(pack_referred1);
         assign_full_self(pack_referred2);
         assign_full_self(pack_copied);
         assign_full_self(pack_cloned);
         assign_full_self(pack_moved1);
         assign_full_self(pack_moved2);
         assign_full_self(pack_abandoned);
         assign_full_self(pack_disowned);
      }

      WHEN("Absorbed by referral") {
         auto absorb_construct_refer = [&](T& a) {
            T absorbed1 {a};
            T absorbed2 {Refer {a}};

            Set_Helper_TestSame(absorbed1, a);
            Set_Helper_TestSame(absorbed2, a);
            REQUIRE(absorbed1.GetUses() == 3);
            REQUIRE(absorbed2.GetUses() == 3);
         };

         absorb_construct_refer(pack_referred1);
         absorb_construct_refer(pack_referred2);
         absorb_construct_refer(pack_copied);
         absorb_construct_refer(pack_cloned);
         absorb_construct_refer(pack_moved1);
         absorb_construct_refer(pack_moved2);
         absorb_construct_refer(pack_abandoned);
         absorb_construct_refer(pack_disowned);
      }
      
      WHEN("Absorbed by move") {
         auto absorb_construct_move1 = [&](T& a) {
            T backup = a;
            T absorbed {::std::move(a)};

            Set_CheckState_Default<E>(a);
            Set_CheckState_OwnedFull<E>(absorbed);
            Set_Helper_TestSame(absorbed, backup);
            REQUIRE(absorbed.GetUses() == 2);
         };

         absorb_construct_move1(pack_referred1);
         absorb_construct_move1(pack_referred2);
         absorb_construct_move1(pack_copied);
         absorb_construct_move1(pack_cloned);
         absorb_construct_move1(pack_moved1);
         absorb_construct_move1(pack_moved2);
         absorb_construct_move1(pack_abandoned);
         absorb_construct_move1(pack_disowned);
      }
      
      WHEN("Absorbed by move (alt)") {
         auto absorb_construct_move2 = [&](T& a) {
            T backup = a;
            T absorbed {Move(a)};

            Set_CheckState_Default<E>(a);
            Set_CheckState_OwnedFull<E>(absorbed);
            Set_Helper_TestSame(absorbed, backup);
            REQUIRE(absorbed.GetUses() == 2);
         };

         absorb_construct_move2(pack_referred1);
         absorb_construct_move2(pack_referred2);
         absorb_construct_move2(pack_copied);
         absorb_construct_move2(pack_cloned);
         absorb_construct_move2(pack_moved1);
         absorb_construct_move2(pack_moved2);
         absorb_construct_move2(pack_abandoned);
         absorb_construct_move2(pack_disowned);
      }
      
      WHEN("Absorbed by abandon") {
         auto absorb_construct_abandon = [&](T& a) {
            T backup = a;
            T absorbed {Abandon {a}};

            Set_CheckState_Abandoned<E>(a);
            Set_CheckState_OwnedFull<E>(absorbed);
            Set_Helper_TestSame(absorbed, backup);
            REQUIRE(absorbed.GetUses() == 2);
         };

         absorb_construct_abandon(pack_referred1);
         absorb_construct_abandon(pack_referred2);
         absorb_construct_abandon(pack_copied);
         absorb_construct_abandon(pack_cloned);
         absorb_construct_abandon(pack_moved1);
         absorb_construct_abandon(pack_moved2);
         absorb_construct_abandon(pack_abandoned);
         absorb_construct_abandon(pack_disowned);
      }
      
      WHEN("Absorbed by disown") {
         auto absorb_construct_disown = [&](T& a) {
            T absorbed {Disown {a}};

            Set_CheckState_OwnedFull<E>(a);
            Set_CheckState_DisownedFull<E>(absorbed);
            Set_Helper_TestSame(absorbed, a, false);
            REQUIRE(absorbed.IsConstant());
         };

         absorb_construct_disown(pack_referred1);
         absorb_construct_disown(pack_referred2);
         absorb_construct_disown(pack_copied);
         absorb_construct_disown(pack_cloned);
         absorb_construct_disown(pack_moved1);
         absorb_construct_disown(pack_moved2);
         absorb_construct_disown(pack_abandoned);
         absorb_construct_disown(pack_disowned);
      }
      
      WHEN("Absorbed by copy") {
         const bool managed_sparse = CT::Sparse<E> and Managed;
         auto absorb_construct_copy = [&](T& a, int entry_refs, int indi_refs) {
            T absorbed {Copy {a}};

            Set_CheckState_OwnedFull<E>(a);
            REQUIRE(a.GetUses() == 1);

            Set_CheckState_OwnedFull<E>(absorbed);
            REQUIRE(absorbed.GetUses() == 1);
            REQUIRE(absorbed == a);
            REQUIRE(absorbed.GetRaw() != a.GetRaw());
            REQUIRE(absorbed.template AsAt<E>(0) == a.template AsAt<E>(0));

            if constexpr (CT::Sparse<E>) {
               auto entry = *absorbed.GetEntries();
               if (entry)
                  REQUIRE(entry->GetUses() == entry_refs);
               if constexpr (CT::Referenced<Decay<E>>) {
                  auto e = absorbed.template AsAt<E>(0);
                  REQUIRE(DenseCast(e).GetReferences() == indi_refs);
               }
            }
         };

         absorb_construct_copy(pack_referred1, managed_sparse ? 8 : 3, 9);
         absorb_construct_copy(pack_referred2, managed_sparse ? 8 : 3, 9);
         absorb_construct_copy(pack_copied,    managed_sparse ? 8 : 3, 9);
         absorb_construct_copy(pack_cloned,    2, 2);
         absorb_construct_copy(pack_moved1,    managed_sparse ? 8 : 1, 9);
         absorb_construct_copy(pack_moved2,    managed_sparse ? 8 : 1, 9);
         absorb_construct_copy(pack_abandoned, managed_sparse ? 8 : 1, 9);
         absorb_construct_copy(pack_disowned,  managed_sparse ? 8 : 1, 9);
      }
      
      WHEN("Absorbed by clone") {
         auto absorb_construct_clone = [&](T& a) {
            T absorbed {Clone {a}};

            Set_CheckState_OwnedFull<E>(a);
            Set_CheckState_OwnedFull<E>(absorbed);
            REQUIRE((absorbed == a) == CT::Dense<E>);
            REQUIRE(absorbed.GetUses() == 1);
         };

         absorb_construct_clone(pack_referred1);
         absorb_construct_clone(pack_referred2);
         absorb_construct_clone(pack_copied);
         absorb_construct_clone(pack_cloned);
         absorb_construct_clone(pack_moved1);
         absorb_construct_clone(pack_moved2);
         absorb_construct_clone(pack_abandoned);
         absorb_construct_clone(pack_disowned);
      }
      
      /*WHEN("Emplace (overwrite)") {
         auto emplace_overwrite = [&](T& a, [[maybe_unused]] const char* intent) {
            ScopedE i666{666};
            const auto i666backup = *i666;
            decltype(auto) instance = a.Emplace(::std::move(*i666));

            Set_CheckState_OwnedFull<E>(a);
            if constexpr (CT::Handle<decltype(instance)>)
               REQUIRE(instance.CompareOneEqual(i666backup));
            else
               REQUIRE(instance == i666backup);
            REQUIRE(a.GetCount() == 1);
            REQUIRE(a.GetReserved() >= 1);
            if constexpr (CT::Typed<T>) {
               REQUIRE(*a == i666backup);
               if constexpr (CT::Handle<decltype(instance)>)
                  REQUIRE(&*a == &*instance);
               else
                  REQUIRE(&*a == &instance);
            }

            BenchmarkSet(
               std::string("Piecewise/") + intent + "/Emplace(" + static_cast<std::string>(NameOf<E>()) + ")", 30,
               auto movable1 = *element;
               auto movable2 = *originalElement;
               a.Emplace(::std::move(movable1)),      a.Emplace(::std::move(movable2))
            );

            if constexpr (not Managed) {
               // On unmanaged tests i666 will be destroyed at the end of this scope,
               // and the container will be left with a dangling pointer.
               // Make sure this isn't happening. When inserting raw unmanaged pointers, 
               // safety is solely in the hands of the user.
               a.Reset();
            }
         };

         emplace_overwrite(pack_referred1, "Refer");
         emplace_overwrite(pack_copied,    "Copy");
         emplace_overwrite(pack_cloned,    "Clone");
         emplace_overwrite(pack_moved1,    "Move");
         emplace_overwrite(pack_abandoned, "Abandon");
         emplace_overwrite(pack_disowned,  "Disown");
      }

      WHEN("Emplace (overwrite, describe)") {
         auto emplace_overwrite_describe = [&](T& a, [[maybe_unused]] const char* intent) {
            ScopedE i666{666};
            const auto i666backup = *i666;
            Neat descriptor {::std::move(*i666)};

            if constexpr (CT::DescribeConstructible<E> and not CT::Container<T>) {
               decltype(auto) instance = a.Emplace(Describe{descriptor});

               Set_CheckState_OwnedFull<E>(a);
               REQUIRE(instance.CompareOneEqual(i666backup));
               REQUIRE(a.GetCount() == 1);
               REQUIRE(a.GetReserved() >= 1);

               BenchmarkSet(
                  std::string("Piecewise/") + intent + "/Emplace(Describe(" + static_cast<std::string>(NameOf<E>()) + "))", 30,
                  auto movable1 = *element;
                  a.Emplace(::std::move(movable1)),      a.Emplace(Describe{descriptor})
               );
            }
            else if constexpr (CT::TypeErased<T>) {
               REQUIRE_THROWS(a.Emplace(Describe{descriptor}));

               Set_CheckState_Default<E>(a, true);
            }
         };

         emplace_overwrite_describe(pack_referred1, "Refer");
         emplace_overwrite_describe(pack_copied,    "Copy");
         emplace_overwrite_describe(pack_cloned,    "Clone");
         emplace_overwrite_describe(pack_moved1,    "Move");
         emplace_overwrite_describe(pack_abandoned, "Abandon");
         emplace_overwrite_describe(pack_disowned,  "Disown");
      }*/
      
      WHEN("Cleared") {
         auto clear_full = [&](T& a, [[maybe_unused]] const char* intent) {
            BenchmarkSetStd("Piecewise/" + intent + "/Clear", 30, 100,
               T temp = a,                      temp.Clear(),
               stdset temp_std (*element),      temp_std.clear()
            );

            a.Clear();

            Set_CheckState_OwnedEmpty<E>(a);
         };

         clear_full(pack_referred1, "Refer");
         clear_full(pack_copied,    "Copy");
         clear_full(pack_cloned,    "Clone");
         clear_full(pack_moved1,    "Move");
         clear_full(pack_abandoned, "Abandon");
         clear_full(pack_disowned,  "Disown");
      }

      WHEN("Reset") {
         auto reset_full = [&](T& a, [[maybe_unused]] const char* intent) {
            BenchmarkSetStd("Piecewise/" + intent + "/Reset", 30, 100,
               T temp = a,                      temp.Reset(),
               stdset temp_std{*element},       temp_std.reset()
            );

            a.Reset();

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

            reset_and_reallocate(pack_referred1);
            reset_and_reallocate(pack_referred2);
            reset_and_reallocate(pack_copied);
            reset_and_reallocate(pack_cloned);
            reset_and_reallocate(pack_moved1);
            reset_and_reallocate(pack_moved2);
            reset_and_reallocate(pack_abandoned);
            //reset_and_reallocate(pack_disowned); // likely to be reallocated in a new place due to lack of authority on the original memory
         }
      }

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
            BenchmarkSetStd("Piecewise/" + intent + "/operator==", 30, 100,
               (void) 0,                               dont_optimize |= (a == same_pack),
               const stdset a_std (*element);
               const stdset another_pack1_std (*e1),   dont_optimize |= (a_std == another_pack1_std)
            );
            BenchmarkSetStd("Piecewise/" + intent + "/operator!=", 30, 100,
               (void) 0,                               dont_optimize |= (a != same_pack),
               const stdset a_std (*element);
               const stdset another_pack1_std (*e1),   dont_optimize |= (a_std != another_pack1_std)
            );
         };

         compared_full(pack_referred1, "Refer");
         compared_full(pack_copied,    "Copy");
         compared_full(pack_cloned,    "Clone");
         compared_full(pack_moved1,    "Move");
         compared_full(pack_abandoned, "Abandon");
         compared_full(pack_disowned,  "Disown");
      }

      WHEN("Contains when full") {
         ScopedE e1 {1};
         
         auto contains_full = [&](T& a) {
            REQUIRE      (a.Contains(*originalElement));
            REQUIRE_FALSE(a.Contains(*e1));
         };

         contains_full(pack_referred1);
         contains_full(pack_referred2);
         contains_full(pack_copied);

         if constexpr (CT::Sparse<E>) {
            REQUIRE      (*pack_cloned.GetDenseAt(0).template As<Decay<E>>() == DenseCast(*originalElement));
            REQUIRE_FALSE(pack_cloned.Contains(*originalElement));
            REQUIRE_FALSE(pack_cloned.Contains(*e1));
         }
         else contains_full(pack_cloned);

         contains_full(pack_moved1);
         contains_full(pack_moved2);
         contains_full(pack_abandoned);
         contains_full(pack_disowned);

         [[maybe_unused]] volatile bool dont_optimize = false;
         BenchmarkSet("Piecewise/Contains", 30,
            (void) 0, dont_optimize |= pack_referred1.Contains(*element)
         );
      }
   }

   GIVEN("Two full containers") {
      const ScopedE e1 {555};
      const ScopedE e2 {666};
      T pack1 {Piecewise, *e1};
      T pack2 {Piecewise, *e2};
      const T memory1 = pack1;
      const T memory2 = pack2;

      WHEN("Copy-assign pack1 to pack2") {
         pack2 = Copy(pack1);
         
         Set_CheckState_OwnedFull<E>(pack1);
         Set_CheckState_OwnedFull<E>(pack2);
         Set_CheckState_ContainsOne(pack2, Refer(e1));

         REQUIRE(pack1.GetUses() == 2);
         REQUIRE(pack2.GetUses() == 1);
         REQUIRE(memory2.GetUses() == 1);
         
         REQUIRE(    pack1.CompareEqual(pack1));
         REQUIRE(    pack1.CompareEqual(pack2));
         REQUIRE(    pack2.CompareEqual(memory1));
         REQUIRE(not pack2.CompareEqual(memory2));
         //REQUIRE(    pack2.CompareOneEqual(*e1));
         //REQUIRE(not pack2.CompareOneEqual(*e2));
      }
      
      WHEN("Refer-assign pack1 in pack2") {
         pack2 = pack1;

         Set_CheckState_OwnedFull<E>(pack1);
         Set_CheckState_OwnedFull<E>(pack2);
         
         REQUIRE(pack1.GetUses() == 3);
         REQUIRE(pack2.GetUses() == 3);
         REQUIRE(memory2.GetUses() == 1);
         REQUIRE(pack1.CompareEqual(pack2));
         REQUIRE(pack2.CompareEqual(memory1));
         REQUIRE(not pack2.CompareEqual(memory2));
         //REQUIRE(pack2.CompareOneEqual(*e1));
      }

      WHEN("Refer-assign pack1 in pack2 (alt)") {
         pack2 = Refer {pack1};

         Set_CheckState_OwnedFull<E>(pack1);
         Set_CheckState_OwnedFull<E>(pack2);
         
         REQUIRE(pack1.GetUses() == 3);
         REQUIRE(pack2.GetUses() == 3);
         REQUIRE(memory2.GetUses() == 1);
         REQUIRE(pack1.CompareEqual(pack2));
         REQUIRE(pack2.CompareEqual(memory1));
         REQUIRE(not pack2.CompareEqual(memory2));
         //REQUIRE(pack2.CompareOneEqual(*e1));
      }

      WHEN("Move-assign pack1 in pack2") {
         T movable = pack1;
         pack2 = ::std::move(movable);

         Set_CheckState_Default<E>(movable);
         Set_CheckState_OwnedFull<E>(pack1);
         Set_CheckState_OwnedFull<E>(pack2);
         Set_Helper_TestSame(pack1, pack2);

         REQUIRE(pack1.GetUses() == 3);
         REQUIRE(pack2.GetUses() == 3);
         REQUIRE(memory2.GetUses() == 1);
      }

      WHEN("Move-assign pack1 in pack2 (alt)") {
         T movable = pack1;
         pack2 = Move {movable};

         Set_CheckState_Default<E>(movable);
         Set_CheckState_OwnedFull<E>(pack1);
         Set_CheckState_OwnedFull<E>(pack2);
         Set_Helper_TestSame(pack1, pack2);

         REQUIRE(pack1.GetUses() == 3);
         REQUIRE(pack2.GetUses() == 3);
         REQUIRE(memory2.GetUses() == 1);
      }

      WHEN("Disown-assign pack1 in pack2") {
         pack2 = Disown(pack1);
         
         Set_CheckState_OwnedFull<E>(pack1);
         Set_CheckState_DisownedFull<E>(pack2);
         Set_Helper_TestSame(pack1, pack2, false);

         REQUIRE(pack1.GetUses() == 2);
         REQUIRE(pack2.GetUses() == 2);
         REQUIRE(memory2.GetUses() == 1);
         REQUIRE(pack2 == memory1);
         REQUIRE(pack2 != memory2);
         //REQUIRE(pack2.CompareOneEqual(*e1));
      }

      WHEN("Abandon-assign pack1 in pack2") {
         T movable = pack1;
         pack2 = Abandon(movable);

         Set_CheckState_Abandoned<E>(movable);
         Set_CheckState_OwnedFull<E>(pack1);
         Set_CheckState_OwnedFull<E>(pack2);

         REQUIRE(pack1.GetUses() == 3);
         REQUIRE(pack2.GetUses() == 3);
         REQUIRE(memory2.GetUses() == 1);
         REQUIRE(pack1 == pack2);
      }

      WHEN("Clone-assign pack1 in pack2") {
         pack2 = Clone(pack1);

         REQUIRE(pack1.GetUses() == 2);
         REQUIRE(pack2.GetUses() == 1);
         REQUIRE((pack1 == pack2) == CT::Dense<E>);
         REQUIRE((pack2 == memory1) == CT::Dense<E>);
         REQUIRE(pack2 != memory2);
      }

      WHEN("Copy-assign pack1 in pack2, then reset pack1") {
         pack2 = Copy(pack1);
         pack1.Reset();

         Set_CheckState_Default<E>(pack1);
         Set_CheckState_OwnedFull<E>(pack2);

         REQUIRE(pack2.GetUses() == 1);
         REQUIRE(pack2 == memory1);
      }

      WHEN("Clone-assign pack1 in pack2, then reset pack1") {
         pack2 = Clone(pack1);
         REQUIRE(pack1.GetUses() == 2);
         REQUIRE(pack2.GetUses() == 1);
         if constexpr (CT::Sparse<E>)
            REQUIRE((*pack2.GetEntriesAt(0))->GetUses() == 1);
         if constexpr (CT::Sparse<Deptr<E>>)
            REQUIRE((*(pack2.GetEntriesAt(0)+1))->GetUses() == 1);

         const T memory3 = pack2;
         REQUIRE(pack2.GetUses() == 2);
         if constexpr (CT::Sparse<E>)
            REQUIRE((*pack2.GetEntriesAt(0))->GetUses() == 2);
         if constexpr (CT::Sparse<Deptr<E>>)
            REQUIRE((*(pack2.GetEntriesAt(0) + 1))->GetUses() == 2);

         pack1.Reset();

         REQUIRE_FALSE(pack1.GetAllocation());
         REQUIRE(pack2.GetUses() == 2);
         REQUIRE(memory3.GetUses() == 2);
      }

      WHEN("Refer-assign pack1 in pack2, then reset pack1") {
         pack2 = pack1;
         pack1.Reset();
         
         Set_CheckState_Default<E>(pack1);
         Set_CheckState_OwnedFull<E>(pack2);

         REQUIRE(pack2.GetUses() == 2);
         REQUIRE(pack2 == memory1);
      }

      WHEN("Compared") {
         T defaulted_pack;

         REQUIRE      (pack1 != pack2);
         REQUIRE_FALSE(pack1 == pack2);
         REQUIRE      (pack1 != defaulted_pack);
         REQUIRE_FALSE(pack1 == defaulted_pack);
         REQUIRE      (pack2 != defaulted_pack);
         REQUIRE_FALSE(pack2 == defaulted_pack);

         static_assert(not static_cast<bool>(T{}));

         if constexpr (CT::DeepDense<E>) {
            static_assert(     T{} == E{} );
            static_assert(not (T{} != E{}));
            static_assert(     E{} == T{} );
            static_assert(not (E{} != T{}));
         }
         else {
            static_assert(     T{} != E{} );
            static_assert(not (T{} == E{}));
            static_assert(     E{} != T{} );
            static_assert(not (E{} == T{}));
         }
      }
   }

   REQUIRE(memoryState.Assert());
   REQUIRE_FALSE(Allocator::CollectGarbage());
}
