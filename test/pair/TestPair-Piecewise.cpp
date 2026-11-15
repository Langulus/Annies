///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "TestPairCommon.hpp"


namespace Langulus::Annies  
{
   // Reuses definitions from TestPair-Empty.cpp. Reduces compile time. 
   extern template struct TPair<Text,   Text>;
   extern template struct TPair<int,    int>;
   extern template struct TPair<Any,    Any>;
   extern template struct TPair<RT,     RT>;
   extern template struct TPair<char,   char>;
   
   extern template struct TPair<Text*,  Text*>;
   extern template struct TPair<int*,   int*>;
   extern template struct TPair<Any*,   Any*>;
   extern template struct TPair<RT*,    RT*>;
   extern template struct TPair<char*,  char*>;
   
   extern template struct TPair<Text**, Text**>;
   extern template struct TPair<int**,  int**>;
   extern template struct TPair<Any**,  Any**>;
   extern template struct TPair<RT**,   RT**>;
   extern template struct TPair<char**, char**>;

#if LANGULUS_FEATURE(MANAGED_MEMORY)
   extern template struct TPair<pptr8,  pptr8>;
   extern template struct TPair<pptr16, pptr16>;
   extern template struct TPair<pptr32, pptr32>;
#endif
}


TEST_CASE_TEMPLATE("Test piecewise-constructed Pair/TPair", TestType
   // Elements are not allocated by the memory manager                  
   , Types<Pair, ScopedElement<Text>,    ScopedElement<Text>>
   , Types<Pair, ScopedElement<int>,     ScopedElement<int>>
   , Types<Pair, ScopedElement<Any>,     ScopedElement<Any>>
   , Types<Pair, ScopedElement<RT>,      ScopedElement<RT>>
   , Types<Pair, ScopedElement<char>,    ScopedElement<char>>

   , Types<Pair, ScopedElement<Text*>,   ScopedElement<Text*>>
   , Types<Pair, ScopedElement<int*>,    ScopedElement<int*>>
   , Types<Pair, ScopedElement<Any*>,    ScopedElement<Any*>>
   , Types<Pair, ScopedElement<RT*>,     ScopedElement<RT*>>
   , Types<Pair, ScopedElement<char*>,   ScopedElement<char*>>

   , Types<Pair, ScopedElement<Text**>,  ScopedElement<Text**>>
   , Types<Pair, ScopedElement<int**>,   ScopedElement<int**>>
   , Types<Pair, ScopedElement<Any**>,   ScopedElement<Any**>>
   , Types<Pair, ScopedElement<RT**>,    ScopedElement<RT**>>
   , Types<Pair, ScopedElement<char**>,  ScopedElement<char**>>

   , Types<TPair<Text,   Text>,   ScopedElement<Text>,    ScopedElement<Text>>
   , Types<TPair<int,    int>,    ScopedElement<int>,     ScopedElement<int>>
   , Types<TPair<Any,    Any>,    ScopedElement<Any>,     ScopedElement<Any>>
   , Types<TPair<RT,     RT>,     ScopedElement<RT>,      ScopedElement<RT>>
   , Types<TPair<char,   char>,   ScopedElement<char>,    ScopedElement<char>>

   , Types<TPair<Text*,  Text*>,  ScopedElement<Text*>,   ScopedElement<Text*>>
   , Types<TPair<int*,   int*>,   ScopedElement<int*>,    ScopedElement<int*>>
   , Types<TPair<Any*,   Any*>,   ScopedElement<Any*>,    ScopedElement<Any*>>
   , Types<TPair<RT*,    RT*>,    ScopedElement<RT*>,     ScopedElement<RT*>>
   , Types<TPair<char*,  char*>,  ScopedElement<char*>,   ScopedElement<char*>>

   , Types<TPair<Text**, Text**>, ScopedElement<Text**>,  ScopedElement<Text**>>
   , Types<TPair<int**,  int**>,  ScopedElement<int**>,   ScopedElement<int**>>
   , Types<TPair<Any**,  Any**>,  ScopedElement<Any**>,   ScopedElement<Any**>>
   , Types<TPair<RT**,   RT**>,   ScopedElement<RT**>,    ScopedElement<RT**>>
   , Types<TPair<char**, char**>, ScopedElement<char**>,  ScopedElement<char**>>

   #if LANGULUS_FEATURE(MANAGED_MEMORY)
   // Elements are allocated by the memory manager                      
   , Types<Pair, ScopedElement<Text, true>,    ScopedElement<Text, true>>
   , Types<Pair, ScopedElement<int,  true>,    ScopedElement<int,  true>>
   , Types<Pair, ScopedElement<Any,  true>,    ScopedElement<Any,  true>>
   , Types<Pair, ScopedElement<RT,   true>,    ScopedElement<RT,   true>>
   , Types<Pair, ScopedElement<char, true>,    ScopedElement<char, true>>

   , Types<Pair, ScopedElement<Text*, true>,   ScopedElement<Text*, true>>
   , Types<Pair, ScopedElement<int*,  true>,   ScopedElement<int*,  true>>
   , Types<Pair, ScopedElement<Any*,  true>,   ScopedElement<Any*,  true>>
   , Types<Pair, ScopedElement<RT*,   true>,   ScopedElement<RT*,   true>>
   , Types<Pair, ScopedElement<char*, true>,   ScopedElement<char*, true>>

   , Types<Pair, ScopedElement<Text**, true>,  ScopedElement<Text**, true>>
   , Types<Pair, ScopedElement<int**,  true>,  ScopedElement<int**,  true>>
   , Types<Pair, ScopedElement<Any**,  true>,  ScopedElement<Any**,  true>>
   , Types<Pair, ScopedElement<RT**,   true>,  ScopedElement<RT**,   true>>
   , Types<Pair, ScopedElement<char**, true>,  ScopedElement<char**, true>>

   , Types<TPair<Text,   Text>,   ScopedElement<Text, true>,    ScopedElement<Text, true>>
   , Types<TPair<int,    int>,    ScopedElement<int,  true>,    ScopedElement<int,  true>>
   , Types<TPair<Any,    Any>,    ScopedElement<Any,  true>,    ScopedElement<Any,  true>>
   , Types<TPair<RT,     RT>,     ScopedElement<RT,   true>,    ScopedElement<RT,   true>>
   , Types<TPair<char,   char>,   ScopedElement<char, true>,    ScopedElement<char, true>>

   , Types<TPair<Text*,  Text*>,  ScopedElement<Text*, true>,   ScopedElement<Text*, true>>
   , Types<TPair<int*,   int*>,   ScopedElement<int*,  true>,   ScopedElement<int*,  true>>
   , Types<TPair<Any*,   Any*>,   ScopedElement<Any*,  true>,   ScopedElement<Any*,  true>>
   , Types<TPair<RT*,    RT*>,    ScopedElement<RT*,   true>,   ScopedElement<RT*,   true>>
   , Types<TPair<char*,  char*>,  ScopedElement<char*, true>,   ScopedElement<char*, true>>

   , Types<TPair<Text**, Text**>, ScopedElement<Text**, true>,  ScopedElement<Text**, true>>
   , Types<TPair<int**,  int**>,  ScopedElement<int**,  true>,  ScopedElement<int**,  true>>
   , Types<TPair<Any**,  Any**>,  ScopedElement<Any**,  true>,  ScopedElement<Any**,  true>>
   , Types<TPair<RT**,   RT**>,   ScopedElement<RT**,   true>,  ScopedElement<RT**,   true>>
   , Types<TPair<char**, char**>, ScopedElement<char**, true>,  ScopedElement<char**, true>>

   // Packed pointers                                                   
   , Types<Pair, ScopedElementPacked<pptr8>,   ScopedElementPacked<pptr8>>
   , Types<Pair, ScopedElementPacked<pptr16>,  ScopedElementPacked<pptr16>>
   , Types<Pair, ScopedElementPacked<pptr32>,  ScopedElementPacked<pptr32>>

   , Types<TPair<pptr8,  pptr8>,  ScopedElementPacked<pptr8>,   ScopedElementPacked<pptr8>>
   , Types<TPair<pptr16, pptr16>, ScopedElementPacked<pptr16>,  ScopedElementPacked<pptr16>>
   , Types<TPair<pptr32, pptr32>, ScopedElementPacked<pptr32>,  ScopedElementPacked<pptr32>>
   #endif
) {
   static MemoryState memoryState;
   using T = typename TestType::First;
   using ScopedE1 = typename TestType::template At<1>;
   using ScopedE2 = typename TestType::template At<2>;
   using E1 = TypeOf<ScopedE1>;
   using E2 = TypeOf<ScopedE2>;

   constexpr bool Managed = ScopedE1::Managed;
   static_assert(ScopedE1::Managed == ScopedE2::Managed);

   #if LANGULUS(BENCHMARK)
      using stdpair = ::std::pair<E1, E2>;
   #endif

   GIVEN("Piecewise-constructed container") {
      const ScopedE1 originalElement1{556};
      const ScopedE2 originalElement2{112};
      const ScopedE1 element1{555};
      const ScopedE2 element2{111};

      auto originalElement_movable1_1 = *originalElement1;
      auto originalElement_movable1_2 = *originalElement1;
      auto originalElement_movable1_3 = *originalElement1;

      auto originalElement_movable2_1 = *originalElement2;
      auto originalElement_movable2_2 = *originalElement2;
      auto originalElement_movable2_3 = *originalElement2;

      T pack_referred1{Piecewise,           *originalElement1,                      *originalElement2 };
      T pack_referred2{Piecewise,     Refer(*originalElement1),               Refer(*originalElement2)};
      T pack_copied   {Piecewise,      Copy(*originalElement1),                Copy(*originalElement2)};
      T pack_cloned   {Piecewise,     Clone(*originalElement1),               Clone(*originalElement2)};
      T pack_moved1   {Piecewise, std::move( originalElement_movable1_1), std::move( originalElement_movable2_1)};
      T pack_moved2   {Piecewise,      Move( originalElement_movable1_2),      Move( originalElement_movable2_2)};
      T pack_abandoned{Piecewise,   Abandon( originalElement_movable1_3),   Abandon( originalElement_movable2_3)};
      T pack_disowned {Piecewise,    Disown(*originalElement1),              Disown(*originalElement2)};

      WHEN("Value-constructed") {
         Pair_CheckState_OwnedFull<E1, E2>(pack_referred1);
         Pair_CheckState_OwnedFull<E1, E2>(pack_referred2);
         Pair_CheckState_OwnedFull<E1, E2>(pack_copied);
         Pair_CheckState_OwnedFull<E1, E2>(pack_cloned);
         Pair_CheckState_OwnedFull<E1, E2>(pack_moved1);
         Pair_CheckState_OwnedFull<E1, E2>(pack_moved2);
         Pair_CheckState_OwnedFull<E1, E2>(pack_abandoned);
         Pair_CheckState_OwnedFull<E1, E2>(pack_disowned);

         Pair_CheckState_ContainsOne(pack_referred1,  Refer(originalElement1),  Refer(originalElement2), 1);
         Pair_CheckState_ContainsOne(pack_referred2,  Refer(originalElement1),  Refer(originalElement2), 1);
         Pair_CheckState_ContainsOne(pack_copied,     Refer(originalElement1),  Refer(originalElement2), 1);
         Pair_CheckState_ContainsOne(pack_cloned,     Clone(originalElement1),  Clone(originalElement2), 1);
         Pair_CheckState_ContainsOne(pack_moved1,     Refer(originalElement1),  Refer(originalElement2), 1);
         Pair_CheckState_ContainsOne(pack_abandoned,  Refer(originalElement1),  Refer(originalElement2), 1);
         Pair_CheckState_ContainsOne(pack_disowned,  Disown(originalElement1), Disown(originalElement2), 1);

         if constexpr (CT::Referenced<Decay<E1>>)
            REQUIRE(DenseCast(*originalElement1).GetReferences() == (CT::Sparse<E1> ? 8 : 1));
         if constexpr (CT::Referenced<Decay<E2>>)
            REQUIRE(DenseCast(*originalElement2).GetReferences() == (CT::Sparse<E2> ? 8 : 1));

         BenchmarkPairStd("Empty/PiecewiseConstructor", 30, 400,
            T temp,            (new (&temp)     T      (Piecewise, *originalElement1, *originalElement2)),
            stdpair temp_std,   new (&temp_std) stdpair(*originalElement1, *originalElement2)
         );
      }

      WHEN("Assigned compatible referred value") {
         auto assign_refer = [&](T& a, [[maybe_unused]] const char* intent) {
            a.Assign(*element1, *element2);

            if constexpr (CT::DeepDense<E1>)
               Many_CheckState_OwnedFull<TypeOf<E1>>(*element1);
            if constexpr (CT::DeepDense<E2>)
               Many_CheckState_OwnedFull<TypeOf<E2>>(*element2);

            Pair_CheckState_OwnedFull<E1, E2>(a);
            Pair_CheckState_ContainsOne(a, Refer(element1), Refer(element2));

            BenchmarkPairStd("Piecewise/" + intent + "/Assign/Refer", 30, 100,
               a.Assign(*element1, *element2),               a.Assign(*originalElement1, *originalElement2),
               stdpair temp_std (*element1, *element2),      temp_std = stdpair(*originalElement1, *originalElement2)
            );
         };

         assign_refer(pack_referred1, "Refer");
         assign_refer(pack_copied,    "Copy");
         assign_refer(pack_cloned,    "Clone");
         assign_refer(pack_moved1,    "Move");
         assign_refer(pack_abandoned, "Abandon");
         assign_refer(pack_disowned,  "Disown");
      }

      if constexpr (CT::Pair<E1>) { //TODO not tested yet
         WHEN("Assigned and absorbed referred container") {
            if (not pack_referred1.IsSame(element1->GetType())) {
               auto misabsorb_refer = [&](T& a) {
                  REQUIRE_THROWS(a.AssignAbsorb(*element1));
                  Pair_CheckState_OwnedFull<E1, E2>(a);
                  Pair_CheckState_ContainsOne(a, Refer(originalElement1), Refer(originalElement2));
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
               a.AssignAbsorb(*element1);

               Pair_Helper_TestSame(a, *element1);
               REQUIRE(a.GetUses() == element1->GetUses());
               REQUIRE(a.GetUses() == 2);
               REQUIRE(a.GetAllocation() == element1->GetAllocation());

               BenchmarkPairStd("Piecewise/" + intent + "/AssignAbsorb/Refer", 30, 100,
                  a.AssignAbsorb(*element1),                                  a.AssignAbsorb(*originalElement1),
                  stdpair temp_std1 (*element1, *element2);
                  stdpair temp_std2 (*originalElement1, *originalElement2),   temp_std1 = temp_std2
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
            a.Assign(Clone(*element1), Clone(*element2));

            if constexpr (CT::DeepDense<E1>)
               Many_CheckState_OwnedFull<TypeOf<E1>>(*element1);
            if constexpr (CT::DeepDense<E2>)
               Many_CheckState_OwnedFull<TypeOf<E2>>(*element2);

            Pair_CheckState_OwnedFull<E1, E2>(a);
            Pair_CheckState_ContainsOne(a, Clone(element1), Clone(element2));

            BenchmarkPairStd("Piecewise/" + intent + "/Assign/Clone", 30, 100,
               a.Assign(Clone(*element1), Clone(*element2)),     a.Assign(Clone(*originalElement1), Clone(*originalElement2)),
               stdpair temp_std (*element1, *element2),          temp_std = stdpair (*originalElement1, *originalElement2)
            );
         };

         assign_clone(pack_referred1, "Refer");
         assign_clone(pack_copied,    "Copy");
         assign_clone(pack_cloned,    "Clone");
         assign_clone(pack_moved1,    "Move");
         assign_clone(pack_abandoned, "Abandon");
         assign_clone(pack_disowned,  "Disown");
      }

      if constexpr (CT::Pair<E1>) {
         WHEN("Assigned and absorbed cloned container") {
            if (not pack_referred1.IsSame(element1->GetType())) {
               auto misabsorb_clone = [&](T& a) {
                  REQUIRE_THROWS(a.AssignAbsorb(Clone(*element1)));
                  Pair_CheckState_OwnedFull<E1, E2>(a);
                  Pair_CheckState_ContainsOne(a, Clone(originalElement1), Clone(originalElement2));
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
               a.AssignAbsorb(Clone(*element1));

               Pair_CheckState_OwnedFull<TypeOf<E1, 0>, TypeOf<E1, 1>>(*element1);
               Pair_Helper_TestSame(a, *element1);
               REQUIRE(a.GetUses() == 2);
               REQUIRE(a.GetAllocation() == element1->GetAllocation());

               BenchmarkPairStd(
                  std::string("Piecewise/") + intent + "/AssignAbsorb/Clone", 30, 100,
                  a.AssignAbsorb(Clone(*element1)),         a.AssignAbsorb(Clone(*originalElement1)),
                  stdpair temp_std1 (*element);
                  stdpair temp_std2 (*originalElement),     temp_std1 = temp_std2
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
            a.Assign(Copy(*element1), Copy(*element2));

            if constexpr (CT::DeepDense<E1>)
               Many_CheckState_OwnedFull<TypeOf<E1>>(*element1);
            if constexpr (CT::DeepDense<E2>)
               Many_CheckState_OwnedFull<TypeOf<E2>>(*element2);

            Pair_CheckState_OwnedFull<E1, E2>(a);
            Pair_CheckState_ContainsOne(a, Refer(element1), Refer(element2));

            BenchmarkPairStd("Piecewise/" + intent + "/Assign/Copy", 30, 100,
               a.Assign(Copy(*element1), Copy(*element2)),    a.Assign(Copy(*originalElement1), Copy(*originalElement2)),
               stdpair temp_std (*element1, *element2),       temp_std = stdpair (*originalElement1, *originalElement2)
            );
         };

         assign_copy(pack_referred1, "Refer");
         assign_copy(pack_copied,    "Copy");
         assign_copy(pack_cloned,    "Clone");
         assign_copy(pack_moved1,    "Move");
         assign_copy(pack_abandoned, "Abandon");
         assign_copy(pack_disowned,  "Disown");
      }

      if constexpr (CT::Pair<E1>) {
         WHEN("Assigned and absorbed copied container") {
            if (not pack_referred1.IsSame(element1->GetType())) {
               auto misabsorb_copy = [&](T& a) {
                  REQUIRE_THROWS(a.AssignAbsorb(Copy(*element1)));
                  Pair_CheckState_OwnedFull<E1>(a);
                  Pair_CheckState_ContainsOne(a, Refer(originalElement1));
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
               a.AssignAbsorb(Copy(*element1));

               Pair_CheckState_OwnedFull<TypeOf<E1, 0>, TypeOf<E1, 1>>(*element1);
               Pair_Helper_TestSame(a, *element1);
               REQUIRE(a.GetUses() == 2);
               REQUIRE(a.GetAllocation() == element1->GetAllocation());

               BenchmarkPairStd("Piecewise/" + intent + "/AssignAbsorb/Copy", 30, 100,
                  a.AssignAbsorb(Copy(*element1)),         a.AssignAbsorb(Copy(*originalElement1)),
                  stdpair temp_std1 (*element1);
                  stdpair temp_std2 (*originalElement1),   temp_std1 = temp_std2
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
            auto movable1 = *element1;
            auto movable2 = *element2;
            a.Assign(::std::move(movable1), ::std::move(movable2));

            if constexpr (CT::DeepDense<E1>)
               Any_CheckState_Default<TypeOf<E1>>(movable1);
            if constexpr (CT::DeepDense<E2>)
               Any_CheckState_Default<TypeOf<E2>>(movable2);

            Pair_CheckState_OwnedFull<E1, E2>(a);
            Pair_CheckState_ContainsOne(a, Refer(element1), Refer(element2));

            BenchmarkPairStd("Piecewise/" + intent + "/Assign/Move", 30, 100,
               auto movable1_1 = *element1;
               auto movable1_2 = *element2;
               auto movable2_1 = *originalElement1;
               auto movable2_2 = *originalElement2;
               a.Assign(Move(movable1_1), Move(movable1_2)),                            a.Assign(Move(movable2_1), Move(movable2_2)),
               auto movable1_1 = *element1;
               auto movable1_2 = *element2;
               auto movable2_1 = *originalElement1;
               auto movable2_2 = *originalElement2;
               stdpair temp_std ({::std::move(movable1_1), ::std::move(movable1_2)}),   temp_std = stdpair(::std::move(movable2_1), ::std::move(movable2_2))
            );
         };

         assign_move(pack_referred1, "Refer");
         assign_move(pack_copied,    "Copy");
         assign_move(pack_cloned,    "Clone");
         assign_move(pack_moved1,    "Move");
         assign_move(pack_abandoned, "Abandon");
         assign_move(pack_disowned,  "Disown");
      }

      if constexpr (CT::Pair<E1>) {
         WHEN("Assigned and absorbed moved container") {
            if (not pack_referred1.IsSame(element1->GetType())) {
               auto misabsorb_move = [&](T& a) {
                  auto movable1 = *element1;
                  REQUIRE_THROWS(a.AssignAbsorb(::std::move(movable1)));

                  Pair_CheckState_OwnedFull<E1, E2>(a);
                  Pair_CheckState_ContainsOne(a, Refer(originalElement1));
                  Pair_CheckState_OwnedFull<int>(movable1);
                  REQUIRE(movable1.GetUses() == 2);
                  REQUIRE(movable1.template As<int>() == 555);
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
               auto movable = *element1;
               a.AssignAbsorb(::std::move(movable));

               Pair_CheckState_Default<TypeOf<E1, 0>, TypeOf<E1, 1>>(movable);
               Pair_Helper_TestSame(a, *element1);
               REQUIRE(a.GetUses() == 2);
               REQUIRE(a.GetAllocation() == element1->GetAllocation());

               BenchmarkPairStd("Piecewise/" + intent + "/AssignAbsorb/Move", 30, 100,
                  T movable1 = *element;
                  T movable2 = *originalElement;
                  a.AssignAbsorb(Move(movable1)),            a.AssignAbsorb(Move(movable2)),
                  stdpair movable1 (*element);
                  stdpair movable2 (*originalElement),       movable1 = ::std::move(movable2)
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
            a.Assign(Disown(*element1), Disown(*element2));

            Pair_CheckState_OwnedFull<E1, E2>(a);
            Pair_CheckState_ContainsOne(a, Disown(element1), Disown(element2));

            BenchmarkPairStd("Piecewise/" + intent + "/Assign/Disown", 30, 100,
               a.Assign(Disown(*element1), Disown(*element2)),    a.Assign(Disown(*originalElement1), Disown(*originalElement2)),
               stdpair temp_std(*element1, *element2),            temp_std = stdpair(*originalElement1, *originalElement2)
            );
         };

         assign_disown(pack_referred1, "Refer");
         assign_disown(pack_copied,    "Copy");
         assign_disown(pack_cloned,    "Clone");
         assign_disown(pack_moved1,    "Move");
         assign_disown(pack_abandoned, "Abandon");
         assign_disown(pack_disowned,  "Disown");
      }

      if constexpr (CT::Pair<E1>) {
         WHEN("Assigned and absorbed disowned container") {
            if (not pack_referred1.IsSame(element1->GetType())) {
               auto misabsorb_disown = [&](T& a) {
                  REQUIRE_THROWS(a.AssignAbsorb(Disown(*element1)));
                  Pair_CheckState_OwnedFull<E1, E2>(a);
                  Pair_CheckState_ContainsOne(a, Disown(originalElement1));
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
               a.AssignAbsorb(Disown(*element1));

               REQUIRE(a.GetRaw() == element1->GetRaw());
               REQUIRE(a.IsExact(element1->GetType()));
               REQUIRE(a == *element1);
               REQUIRE(a.IsDeep() == element1->IsDeep());
               REQUIRE(a.IsConstant() != element1->IsConstant());
               REQUIRE(a.GetUnconstrainedState() == element1->GetUnconstrainedState());
               REQUIRE(a.GetUses() == 0);
               REQUIRE_FALSE(a.GetAllocation());

               BenchmarkPairStd("Piecewise/" + intent + "/AssignAbsorb/Disown", 30, 100,
                  a.AssignAbsorb(Disown(*element)),          a.AssignAbsorb(Disown(*originalElement)),
                  stdpair temp_std1 (*element);
                  stdpair temp_std2 (*originalElement),      temp_std1 = temp_std2
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
            auto movable1 = *element1;
            auto movable2 = *element2;
            a.Assign(Abandon(movable1), Abandon(movable2));

            if constexpr (CT::DeepDense<E1>)
               Many_CheckState_Abandoned<TypeOf<E1>>(movable1);
            if constexpr (CT::DeepDense<E2>)
               Many_CheckState_Abandoned<TypeOf<E2>>(movable2);

            Pair_CheckState_OwnedFull<E1, E2>(a);
            Pair_CheckState_ContainsOne(a, Refer(element1), Refer(element2));

            BenchmarkPairStd("Piecewise/" + intent + "/Assign/Abandon", 30, 100,
               auto movable11 = *element1;
               auto movable21 = *originalElement1;
               auto movable12 = *element2;
               auto movable22 = *originalElement2;
               a.Assign(Abandon(movable11), Abandon(movable12)),                   a.Assign(Abandon(movable21), Abandon(movable22)),
               auto movable11 = *element1;
               auto movable21 = *originalElement1;
               auto movable12 = *element2;
               auto movable22 = *originalElement2;
               stdpair temp_std(::std::move(movable11), ::std::move(movable12)),   temp_std = stdpair(::std::move(movable21), :std::move(movable22))
            );
         };

         assign_abandon(pack_referred1, "Refer");
         assign_abandon(pack_copied,    "Copy");
         assign_abandon(pack_cloned,    "Clone");
         assign_abandon(pack_moved1,    "Move");
         assign_abandon(pack_abandoned, "Abandon");
         assign_abandon(pack_disowned,  "Disown");
      }

      if constexpr (CT::Pair<E1>) {
         WHEN("Assigned and absorbed abandoned container") {
            if (not pack_referred1.IsSame(element1->GetType())) {
               auto misabsorb_abandon = [&](T& a) {
                  auto movable = *element1;
                  REQUIRE_THROWS(a.AssignAbsorb(Abandon(movable)));

                  Pair_CheckState_OwnedFull<E1>(a);
                  Pair_CheckState_ContainsOne(a, Refer(originalElement1));
                  Pair_CheckState_OwnedFull<int>(movable);
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
               auto movable = *element1;
               a.AssignAbsorb(Abandon(movable));

               Pair_CheckState_Abandoned<TypeOf<E1, 0>, TypeOf<E1, 1>>(movable);
               Pair_Helper_TestSame(a, *element1);
               REQUIRE(a.GetUses() == 2);
               REQUIRE(a.GetAllocation() == element1->GetAllocation());

               BenchmarkPairStd("Piecewise/" + intent + "/AssignAbsorb/Abandon", 30, 100,
                  T movable1 = *element;
                  T movable2 = *originalElement;
                  a.AssignAbsorb(Abandon(movable1)),          a.AssignAbsorb(Abandon(movable2)),
                  stdpair movable1 (*element);
                  stdpair movable2 (*originalElement);
                  stdpair temp_std = ::std::move(movable1),   temp_std = ::std::move(movable2)
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
            Pair_CheckState_Default<E1, E2>(a);
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
               a = a;
            LglsDisableWarningPop
            Pair_Helper_TestSame(a, backup, not allow_change_in_constness);
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

            Pair_Helper_TestSame(absorbed1, compare_against);
            Pair_Helper_TestSame(absorbed2, compare_against);
            REQUIRE(absorbed1.GetUses() == uses);
            REQUIRE(absorbed2.GetUses() == uses);
         };

         absorb_construct_refer(pack_referred1, pack_referred1, 3);
         absorb_construct_refer(pack_referred2, pack_referred2, 3);
         absorb_construct_refer(pack_copied,    pack_copied,    3);
         absorb_construct_refer(pack_cloned,    pack_cloned,    3);
         absorb_construct_refer(pack_moved1,    pack_moved1,    3);
         absorb_construct_refer(pack_moved2,    pack_moved2,    3);
         absorb_construct_refer(pack_abandoned, pack_abandoned, 3);
         absorb_construct_refer(pack_disowned,  pack_disowned,  3);
      }
      
      WHEN("Absorbed by move") {
         auto absorb_construct_move1 = [&](T& a, int uses) {
            T backup = a;
            T absorbed {::std::move(a)};

            Pair_CheckState_Default<E1, E2>(a);
            Pair_CheckState_OwnedFull<E1, E2>(absorbed);
            Pair_Helper_TestSame(absorbed, backup);
            REQUIRE(absorbed.GetUses() == uses);
         };

         absorb_construct_move1(pack_referred1, 2);
         absorb_construct_move1(pack_referred2, 2);
         absorb_construct_move1(pack_copied,    2);
         absorb_construct_move1(pack_cloned,    2);
         absorb_construct_move1(pack_moved1,    2);
         absorb_construct_move1(pack_moved2,    2);
         absorb_construct_move1(pack_abandoned, 2);
         absorb_construct_move1(pack_disowned,  2);
      }
      
      WHEN("Absorbed by move (alt)") {
         auto absorb_construct_move2 = [&](T& a, int uses) {
            T backup = a;
            T absorbed {Move(a)};

            Pair_CheckState_Default<E1, E2>(a);
            Pair_CheckState_OwnedFull<E1, E2>(absorbed);
            Pair_Helper_TestSame(absorbed, backup);
            REQUIRE(absorbed.GetUses() == uses);
         };

         absorb_construct_move2(pack_referred1, 2);
         absorb_construct_move2(pack_referred2, 2);
         absorb_construct_move2(pack_copied,    2);
         absorb_construct_move2(pack_cloned,    2);
         absorb_construct_move2(pack_moved1,    2);
         absorb_construct_move2(pack_moved2,    2);
         absorb_construct_move2(pack_abandoned, 2);
         absorb_construct_move2(pack_disowned,  2);
      }
      
      WHEN("Absorbed by abandon") {
         auto absorb_construct_abandon = [&](T& a, int uses) {
            T backup = a;
            T absorbed {Abandon {a}};

            Pair_CheckState_Abandoned<E1, E2>(a);
            Pair_CheckState_OwnedFull<E1, E2>(absorbed);
            Pair_Helper_TestSame(absorbed, backup);
            REQUIRE(absorbed.GetUses() == uses);
         };

         absorb_construct_abandon(pack_referred1,  2);
         absorb_construct_abandon(pack_referred2,  2);
         absorb_construct_abandon(pack_copied,     2);
         absorb_construct_abandon(pack_cloned,     2);
         absorb_construct_abandon(pack_moved1,     2);
         absorb_construct_abandon(pack_moved2,     2);
         absorb_construct_abandon(pack_abandoned,  2);
         absorb_construct_abandon(pack_disowned,   2);
      }
      
      WHEN("Absorbed by disown") {
         auto absorb_construct_disown = [&](T& a, int uses) {
            T absorbed {Disown {a}};

            Pair_CheckState_OwnedFull<E1, E2>(a);
            Pair_CheckState_DisownedFull<E1, E2>(absorbed);
            Pair_Helper_TestSame(absorbed, a, false);
            REQUIRE(absorbed.GetUses() == uses);
         };

         absorb_construct_disown(pack_referred1, 1);
         absorb_construct_disown(pack_referred2, 1);
         absorb_construct_disown(pack_copied,    1);
         absorb_construct_disown(pack_cloned,    1);
         absorb_construct_disown(pack_moved1,    1);
         absorb_construct_disown(pack_moved2,    1);
         absorb_construct_disown(pack_abandoned, 1);
         absorb_construct_disown(pack_disowned,  1);
      }
      
      WHEN("Absorbed by copy") {
         const bool managed_sparse = CT::Sparse<E1, E2> and Managed;
         auto absorb_construct_copy = [&](T& a, int uses, int entry_refs, int indi_refs) {
            T absorbed {Copy {a}};

            Pair_CheckState_OwnedFull<E1, E2>(a);
            REQUIRE(a.GetUses() == uses);
            Pair_CheckState_OwnedFull<E1, E2>(absorbed);
            REQUIRE(absorbed.GetUses() == 1);
            REQUIRE(absorbed == a);
            REQUIRE(absorbed.GetRaw() != a.GetRaw());
            REQUIRE(absorbed.template KeyAs<E1>() == a.template KeyAs<E1>());
            REQUIRE(absorbed.template ValAs<E2>() == a.template ValAs<E2>());

            if constexpr (CT::Sparse<E1>) {
               auto entry = *absorbed.GetKeyEntries();
               
               if (entry)
                  REQUIRE(entry->GetUses() == entry_refs);

               if constexpr (CT::Referenced<Decay<E1>>) {
                  auto e = absorbed.template KeyAs<E1>();
                  REQUIRE(DenseCast(e).GetReferences() == indi_refs);
               }
            }

            if constexpr (CT::Sparse<E2>) {
               auto entry = *absorbed.GetValEntries();
               
               if (entry)
                  REQUIRE(entry->GetUses() == entry_refs);

               if constexpr (CT::Referenced<Decay<E2>>) {
                  auto e = absorbed.template ValAs<E2>();
                  REQUIRE(DenseCast(e).GetReferences() == indi_refs);
               }
            }
         };

         absorb_construct_copy(pack_referred1, 1, managed_sparse ? 8 : 3, 9);
         absorb_construct_copy(pack_referred2, 1, managed_sparse ? 8 : 3, 9);
         absorb_construct_copy(pack_copied,    1, managed_sparse ? 8 : 3, 9);
         absorb_construct_copy(pack_cloned,    1, 2, 2);
         absorb_construct_copy(pack_moved1,    1, managed_sparse ? 8 : 1, 9);
         absorb_construct_copy(pack_moved2,    1, managed_sparse ? 8 : 1, 9);
         absorb_construct_copy(pack_abandoned, 1, managed_sparse ? 8 : 1, 9);
         absorb_construct_copy(pack_disowned,  1, managed_sparse ? 8 : 1, 9);
      }
      
      WHEN("Absorbed by clone") {
         auto absorb_construct_clone = [&](T& a) {
            T absorbed {Clone {a}};

            Pair_CheckState_OwnedFull<E1, E2>(a);
            Pair_CheckState_OwnedFull<E1, E2>(absorbed);
            REQUIRE((absorbed == a) == CT::Dense<E1, E2>);
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
            Many descriptor {Piecewise, ::std::move(*i666)};

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
            BenchmarkPairStd("Piecewise/" + intent + "/Clear", 30, 100,
               T temp = a,                       temp.Clear(),
               stdpair temp_std (*element),      temp_std.clear()
            );

            a.Clear();

            Pair_CheckState_OwnedEmpty<E1, E2>(a);
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
            BenchmarkPairStd("Piecewise/" + intent + "/Reset", 30, 100,
               T temp = a,                       temp.Reset(),
               stdpair temp_std{*element},       temp_std.reset()
            );

            a.Reset();

            Pair_CheckState_Default<E1, E2>(a);
         };

         reset_full(pack_referred1, "Refer");
         reset_full(pack_copied,    "Copy");
         reset_full(pack_cloned,    "Clone");
         reset_full(pack_moved1,    "Move");
         reset_full(pack_abandoned, "Abandon");
         reset_full(pack_disowned,  "Disown");
      }

      if constexpr (LANGULUS_FEATURE(MANAGED_MEMORY) and CT::NotContainer<E1, E2>) {
         // Works only if E doesn't move entries around                 
         WHEN("Reset, and then immediately allocated again") {
            auto reset_and_reallocate = [&](T& a) {
               const auto memory = a.GetRaw();
               a.Reset();
               a = TPair<E1 const&, E2 const&> {*element1, *element2};
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

      WHEN("Compared") {
         ScopedE1 e1 {1};
         ScopedE2 e2 {3};
         T another_pack1 {Piecewise, *e1, *e2};
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
            BenchmarkPairStd("Piecewise/" + intent + "/operator==", 30, 100,
               (void) 0,                                      dont_optimize |= (a == same_pack),
               const stdpair a_std (*element1, *element2);
               const stdpair another_pack1_std (*e1, *e2),    dont_optimize |= (a_std == another_pack1_std)
            );
            BenchmarkPairStd("Piecewise/" + intent + "/operator!=", 30, 100,
               (void) 0,                                      dont_optimize |= (a != same_pack),
               const stdpair a_std (*element1, *element2);
               const stdpair another_pack1_std (*e1, *e2),    dont_optimize |= (a_std != another_pack1_std)
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
         ScopedE1 e1 {1};
         ScopedE2 e2 {2};
         
         auto contains_full = [&](T& a) {
            REQUIRE      (a.Contains(*originalElement1));
            REQUIRE      (a.ContainsEx(TPair                       {*originalElement1, *originalElement2}));
            REQUIRE      (a.ContainsEx(TPair<E1 const&, E2 const&> {*originalElement1, *originalElement2}));
            REQUIRE_FALSE(a.ContainsEx(TPair<E1 const&, E2 const&> {*e1, *originalElement2}));
            REQUIRE_FALSE(a.ContainsEx(TPair<E1 const&, E2 const&> {*originalElement1, *e2}));
            REQUIRE_FALSE(a.Contains(*e1));
            REQUIRE_FALSE(a.Contains(*originalElement2));
            REQUIRE_FALSE(a.ContainsEx(TPair           {*e1, *e2}));
            REQUIRE_FALSE(a.ContainsEx(TPair<E1&, E2&> {*e1, *e2}));
         };

         contains_full(pack_referred1);
         contains_full(pack_referred2);
         contains_full(pack_copied);

         if constexpr (CT::Sparse<E1, E2>) {
            REQUIRE      (*pack_cloned.template GetDense<0>().template As<Decay<E1>>() == DenseCast(*originalElement1));
            REQUIRE      (*pack_cloned.template GetDense<1>().template As<Decay<E2>>() == DenseCast(*originalElement1));
            REQUIRE_FALSE(*pack_cloned.template GetDense<0>().template As<E1>() == *originalElement1);
            REQUIRE_FALSE(*pack_cloned.template GetDense<1>().template As<E2>() == *originalElement1);
            REQUIRE_FALSE(*pack_cloned.template GetDense<0>().template As<E1>() == *e1);
            REQUIRE_FALSE(*pack_cloned.template GetDense<1>().template As<E2>() == *e2);
         }
         else contains_full(pack_cloned);

         contains_full(pack_moved1);
         contains_full(pack_moved2);
         contains_full(pack_abandoned);
         contains_full(pack_disowned);

         [[maybe_unused]] volatile bool dont_optimize = false;
         BenchmarkPair("Piecewise/Contains", 30,
            (void) 0, dont_optimize |= pack_referred1.Contains(*element)
         );
      }
   }

   GIVEN("Two full containers") {
      const ScopedE1 e1 {555};
      const ScopedE2 e2 {666};
      T pack1 {Piecewise, *e1, *e2};
      const T memory1 = pack1;

      const ScopedE1 e3 {5};
      const ScopedE2 e4 {6};
      T pack2 {Piecewise, *e3, *e4};
      const T memory2 = pack2;

      WHEN("Copy-assign pack1 to pack2") {
         pack2 = Copy(pack1);
         
         Pair_CheckState_OwnedFull<E1, E2>(pack1);
         Pair_CheckState_OwnedFull<E1, E2>(pack2);
         Pair_CheckState_ContainsOne(pack2, Refer(e1), Refer(e2));

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

         Pair_CheckState_OwnedFull<E1, E2>(pack1);
         Pair_CheckState_OwnedFull<E1, E2>(pack2);
         
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

         Pair_CheckState_OwnedFull<E1, E2>(pack1);
         Pair_CheckState_OwnedFull<E1, E2>(pack2);
         
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

         Pair_CheckState_Default<E1, E2>(movable);
         Pair_CheckState_OwnedFull<E1, E2>(pack1);
         Pair_CheckState_OwnedFull<E1, E2>(pack2);
         Pair_Helper_TestSame(pack1, pack2);

         REQUIRE(pack1.GetUses() == 3);
         REQUIRE(pack2.GetUses() == 3);
         REQUIRE(memory2.GetUses() == 1);
      }

      WHEN("Move-assign pack1 in pack2 (alt)") {
         T movable = pack1;
         pack2 = Move {movable};

         Pair_CheckState_Default<E1, E2>(movable);
         Pair_CheckState_OwnedFull<E1, E2>(pack1);
         Pair_CheckState_OwnedFull<E1, E2>(pack2);
         Pair_Helper_TestSame(pack1, pack2);

         REQUIRE(pack1.GetUses() == 3);
         REQUIRE(pack2.GetUses() == 3);
         REQUIRE(memory2.GetUses() == 1);
      }

      WHEN("Disown-assign pack1 in pack2") {
         pack2 = Disown(pack1);
         
         Pair_CheckState_OwnedFull<E1, E2>(pack1);
         Pair_CheckState_DisownedFull<E1, E2>(pack2);
         Pair_Helper_TestSame(pack1, pack2, false);

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

         Pair_CheckState_Abandoned<E1, E2>(movable);
         Pair_CheckState_OwnedFull<E1, E2>(pack1);
         Pair_CheckState_OwnedFull<E1, E2>(pack2);

         REQUIRE(pack1.GetUses() == 3);
         REQUIRE(pack2.GetUses() == 3);
         REQUIRE(memory2.GetUses() == 1);
         REQUIRE(pack1 == pack2);
      }

      WHEN("Clone-assign pack1 in pack2") {
         pack2 = Clone(pack1);

         REQUIRE(pack1.GetUses() == 2);
         REQUIRE(pack2.GetUses() == 1);
         REQUIRE((pack1 == pack2) == CT::Dense<E1>);
         REQUIRE((pack2 == memory1) == CT::Dense<E1>);
         REQUIRE(pack2 != memory2);
      }

      WHEN("Copy-assign pack1 in pack2, then reset pack1") {
         pack2 = Copy(pack1);
         pack1.Reset();

         Pair_CheckState_Default<E1, E2>(pack1);
         Pair_CheckState_OwnedFull<E1, E2>(pack2);

         REQUIRE(pack2.GetUses() == 1);
         REQUIRE(pack2 == memory1);
      }

      WHEN("Clone-assign pack1 in pack2, then reset pack1") {
         pack2 = Clone(pack1);
         REQUIRE(pack1.GetUses() == 2);
         REQUIRE(pack2.GetUses() == 1);

         if constexpr (CT::Sparse<E1>)
            REQUIRE((*pack2.GetKeyEntries())->GetUses() == 1);
         if constexpr (CT::Sparse<Deptr<E1>>)
            REQUIRE((*(pack2.GetKeyEntries()+1))->GetUses() == 1);

         if constexpr (CT::Sparse<E2>)
            REQUIRE((*pack2.GetValEntries())->GetUses() == 1);
         if constexpr (CT::Sparse<Deptr<E2>>)
            REQUIRE((*(pack2.GetValEntries()+1))->GetUses() == 1);

         const T memory3 = pack2;
         REQUIRE(pack2.GetUses() == 2);

         if constexpr (CT::Sparse<E1>)
            REQUIRE((*pack2.GetKeyEntries())->GetUses() == 2);
         if constexpr (CT::Sparse<Deptr<E1>>)
            REQUIRE((*(pack2.GetKeyEntries() + 1))->GetUses() == 2);

         if constexpr (CT::Sparse<E2>)
            REQUIRE((*pack2.GetValEntries())->GetUses() == 2);
         if constexpr (CT::Sparse<Deptr<E2>>)
            REQUIRE((*(pack2.GetValEntries() + 1))->GetUses() == 2);

         pack1.Reset();

         REQUIRE_FALSE(pack1.GetAllocation());
         REQUIRE(pack2.GetUses() == 2);
         REQUIRE(memory3.GetUses() == 2);
      }

      WHEN("Refer-assign pack1 in pack2, then reset pack1") {
         pack2 = pack1;
         pack1.Reset();
         
         Pair_CheckState_Default<E1, E2>(pack1);
         Pair_CheckState_OwnedFull<E1, E2>(pack2);

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

         if constexpr (CT::Pair<E1> or CT::Map<E1>) {
            static_assert(     T {} != E1{} );
            static_assert(not (T {} == E1{}));
            static_assert(     E1{} != T {} );
            static_assert(not (E1{} == T {}));
         }
            
         if constexpr (CT::Pair<E2> or CT::Map<E2>) {
            static_assert(     T {} != E2{} );
            static_assert(not (T {} == E2{}));
            static_assert(     E2{} != T {} );
            static_assert(not (E2{} == T {}));
         }
      }
   }

   REQUIRE(memoryState.Assert());
   REQUIRE_FALSE(Allocator::CollectGarbage());
}
