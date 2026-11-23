///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "TestMapCommon.hpp"


namespace Langulus::Annies  
{
   // Reuses definitions from TestMap-Empty.cpp. Reduces compile time.  
   extern template struct TMap<Text,   Text>;
   extern template struct TMap<int,    int>;
   extern template struct TMap<Any,    Any>;
   extern template struct TMap<RT,     RT>;
   extern template struct TMap<char,   char>;

   extern template struct TMap<Text*,  Text*>;
   extern template struct TMap<int*,   int*>;
   extern template struct TMap<Any*,   Any*>;
   extern template struct TMap<RT*,    RT*>;
   extern template struct TMap<char*,  char*>;

   extern template struct TMap<Text**, Text**>;
   extern template struct TMap<int**,  int**>;
   extern template struct TMap<Any**,  Any**>;
   extern template struct TMap<RT**,   RT**>;
   extern template struct TMap<char**, char**>;

#if LANGULUS_FEATURE(MANAGED_MEMORY)
   extern template struct TMap<pptr8,  pptr8>;
   extern template struct TMap<pptr16, pptr16>;
   extern template struct TMap<pptr32, pptr32>;
#endif
}


TEST_CASE_TEMPLATE("Test absorb-constructed Map/TMap", TestType
   // Elements are not allocated by the memory manager                  
   , Types<Map, ScopedElement<Text>,    ScopedElement<Text>>
   , Types<Map, ScopedElement<int>,     ScopedElement<int>>
   , Types<Map, ScopedElement<Any>,     ScopedElement<Any>>
   , Types<Map, ScopedElement<RT>,      ScopedElement<RT>>
   , Types<Map, ScopedElement<char>,    ScopedElement<char>>

   , Types<Map, ScopedElement<Text*>,   ScopedElement<Text*>>
   , Types<Map, ScopedElement<int*>,    ScopedElement<int*>>
   , Types<Map, ScopedElement<Any*>,    ScopedElement<Any*>>
   , Types<Map, ScopedElement<RT*>,     ScopedElement<RT*>>
   , Types<Map, ScopedElement<char*>,   ScopedElement<char*>>

   , Types<Map, ScopedElement<Text**>,  ScopedElement<Text**>>
   , Types<Map, ScopedElement<int**>,   ScopedElement<int**>>
   , Types<Map, ScopedElement<Any**>,   ScopedElement<Any**>>
   , Types<Map, ScopedElement<RT**>,    ScopedElement<RT**>>
   , Types<Map, ScopedElement<char**>,  ScopedElement<char**>>

   , Types<TMap<Text,   Text>,   ScopedElement<Text>,    ScopedElement<Text>>
   , Types<TMap<int,    int>,    ScopedElement<int>,     ScopedElement<int>>
   , Types<TMap<Any,    Any>,    ScopedElement<Any>,     ScopedElement<Any>>
   , Types<TMap<RT,     RT>,     ScopedElement<RT>,      ScopedElement<RT>>
   , Types<TMap<char,   char>,   ScopedElement<char>,    ScopedElement<char>>

   , Types<TMap<Text*,  Text*>,  ScopedElement<Text*>,   ScopedElement<Text*>>
   , Types<TMap<int*,   int*>,   ScopedElement<int*>,    ScopedElement<int*>>
   , Types<TMap<Any*,   Any*>,   ScopedElement<Any*>,    ScopedElement<Any*>>
   , Types<TMap<RT*,    RT*>,    ScopedElement<RT*>,     ScopedElement<RT*>>
   , Types<TMap<char*,  char*>,  ScopedElement<char*>,   ScopedElement<char*>>

   , Types<TMap<Text**, Text**>, ScopedElement<Text**>,  ScopedElement<Text**>>
   , Types<TMap<int**,  int**>,  ScopedElement<int**>,   ScopedElement<int**>>
   , Types<TMap<Any**,  Any**>,  ScopedElement<Any**>,   ScopedElement<Any**>>
   , Types<TMap<RT**,   RT**>,   ScopedElement<RT**>,    ScopedElement<RT**>>
   , Types<TMap<char**, char**>, ScopedElement<char**>,  ScopedElement<char**>>

   #if LANGULUS_FEATURE(MANAGED_MEMORY)
   // Elements are allocated by the memory manager                      
   , Types<Map, ScopedElement<Text, true>,    ScopedElement<Text, true>>
   , Types<Map, ScopedElement<int,  true>,    ScopedElement<int,  true>>
   , Types<Map, ScopedElement<Any,  true>,    ScopedElement<Any,  true>>
   , Types<Map, ScopedElement<RT,   true>,    ScopedElement<RT,   true>>
   , Types<Map, ScopedElement<char, true>,    ScopedElement<char, true>>

   , Types<Map, ScopedElement<Text*, true>,   ScopedElement<Text*, true>>
   , Types<Map, ScopedElement<int*,  true>,   ScopedElement<int*,  true>>
   , Types<Map, ScopedElement<Any*,  true>,   ScopedElement<Any*,  true>>
   , Types<Map, ScopedElement<RT*,   true>,   ScopedElement<RT*,   true>>
   , Types<Map, ScopedElement<char*, true>,   ScopedElement<char*, true>>

   , Types<Map, ScopedElement<Text**, true>,  ScopedElement<Text**, true>>
   , Types<Map, ScopedElement<int**,  true>,  ScopedElement<int**,  true>>
   , Types<Map, ScopedElement<Any**,  true>,  ScopedElement<Any**,  true>>
   , Types<Map, ScopedElement<RT**,   true>,  ScopedElement<RT**,   true>>
   , Types<Map, ScopedElement<char**, true>,  ScopedElement<char**, true>>

   , Types<TMap<Text,   Text>,   ScopedElement<Text, true>,    ScopedElement<Text, true>>
   , Types<TMap<int,    int>,    ScopedElement<int,  true>,    ScopedElement<int,  true>>
   , Types<TMap<Any,    Any>,    ScopedElement<Any,  true>,    ScopedElement<Any,  true>>
   , Types<TMap<RT,     RT>,     ScopedElement<RT,   true>,    ScopedElement<RT,   true>>
   , Types<TMap<char,   char>,   ScopedElement<char, true>,    ScopedElement<char, true>>

   , Types<TMap<Text*,  Text*>,  ScopedElement<Text*, true>,   ScopedElement<Text*, true>>
   , Types<TMap<int*,   int*>,   ScopedElement<int*,  true>,   ScopedElement<int*,  true>>
   , Types<TMap<Any*,   Any*>,   ScopedElement<Any*,  true>,   ScopedElement<Any*,  true>>
   , Types<TMap<RT*,    RT*>,    ScopedElement<RT*,   true>,   ScopedElement<RT*,   true>>
   , Types<TMap<char*,  char*>,  ScopedElement<char*, true>,   ScopedElement<char*, true>>

   , Types<TMap<Text**, Text**>, ScopedElement<Text**, true>,  ScopedElement<Text**, true>>
   , Types<TMap<int**,  int**>,  ScopedElement<int**,  true>,  ScopedElement<int**,  true>>
   , Types<TMap<Any**,  Any**>,  ScopedElement<Any**,  true>,  ScopedElement<Any**,  true>>
   , Types<TMap<RT**,   RT**>,   ScopedElement<RT**,   true>,  ScopedElement<RT**,   true>>
   , Types<TMap<char**, char**>, ScopedElement<char**, true>,  ScopedElement<char**, true>>

   // Packed pointers                                                   
   , Types<Map, ScopedElementPacked<pptr8>,   ScopedElementPacked<pptr8>>
   , Types<Map, ScopedElementPacked<pptr16>,  ScopedElementPacked<pptr16>>
   , Types<Map, ScopedElementPacked<pptr32>,  ScopedElementPacked<pptr32>>

   , Types<TMap<pptr8,  pptr8>,  ScopedElementPacked<pptr8>,   ScopedElementPacked<pptr8>>
   , Types<TMap<pptr16, pptr16>, ScopedElementPacked<pptr16>,  ScopedElementPacked<pptr16>>
   , Types<TMap<pptr32, pptr32>, ScopedElementPacked<pptr32>,  ScopedElementPacked<pptr32>>
   #endif
) {
   static MemoryState memoryState;
   using T = typename TestType::First;
   using ScopedE1 = typename TestType::template At<1>;
   using ScopedE2 = typename TestType::template At<2>;
   using E1 = TypeOf<ScopedE1>;
   using E2 = TypeOf<ScopedE2>;

   constexpr bool Managed = ScopedE1::Managed;
   constexpr bool Sparse1 = CT::Sparse<E1>;
   constexpr bool Sparse2 = CT::Sparse<E2>;
   constexpr bool Reffed1 = CT::Referenced<Decay<E1>>;
   constexpr bool Reffed2 = CT::Referenced<Decay<E2>>;
   static_assert(ScopedE1::Managed == ScopedE2::Managed);

   #if LANGULUS(BENCHMARK)
      using stdmap = ::std::unordered_map<E1, E2>;
   #endif
   
   GIVEN("Piecewise-constructed container, assigned (refer), and then destroyed") {
      const ScopedE1 element1{555};
      const ScopedE2 element2{111};
      const ScopedE1 element3{556};
      const ScopedE2 element4{112};
      T piecewise1{Piecewise, TPair {*element1, *element2}};
      piecewise1.Assign(*element3, *element4);
   }

   GIVEN("Piecewise-constructed container, assigned (refer using intent), and then destroyed") {
      const ScopedE1 element1{555};
      const ScopedE2 element2{111};
      const ScopedE1 element3{556};
      const ScopedE2 element4{112};
      T piecewise1{Piecewise, TPair {*element1, *element2}};
      piecewise1.Assign(Refer(*element3), Refer(*element4));
   }

   GIVEN("Piecewise-constructed container, assigned (copied), and then destroyed") {
      const ScopedE1 element1{555};
      const ScopedE2 element2{111};
      const ScopedE1 element3{556};
      const ScopedE2 element4{112};
      T piecewise1{Piecewise, TPair {*element1, *element2}};
      piecewise1.Assign(Copy(*element3), Copy(*element4));
   }

   GIVEN("Piecewise-constructed container, assigned (cloned), and then destroyed") {
      const ScopedE1 element1{555};
      const ScopedE2 element2{111};
      const ScopedE1 element3{556};
      const ScopedE2 element4{112};
      T piecewise1{Piecewise, TPair {*element1, *element2}};
      piecewise1.Assign(Clone(*element3), Clone(*element4));
   }

   GIVEN("Piecewise-constructed container, assigned (move), and then destroyed") {
      const ScopedE1 element1{555};
      const ScopedE2 element2{111};
      ScopedE1 element3{556};
      ScopedE2 element4{112};
      T piecewise1{Piecewise, TPair {*element1, *element2}};
      piecewise1.Assign(::std::move(*element3), ::std::move(*element4));
   }

   GIVEN("Piecewise-constructed container, assigned (move using intent), and then destroyed") {
      const ScopedE1 element1{555};
      const ScopedE2 element2{111};
      ScopedE1 element3{556};
      ScopedE2 element4{112};
      T piecewise1{Piecewise, TPair {*element1, *element2}};
      piecewise1.Assign(Move(*element3), Move(*element4));
   }

   GIVEN("Piecewise-constructed container, assigned (abandon), and then destroyed") {
      const ScopedE1 element1{555};
      const ScopedE2 element2{111};
      ScopedE1 element3{556};
      ScopedE2 element4{112};
      T piecewise1{Piecewise, TPair {*element1, *element2}};
      piecewise1.Assign(Abandon(*element3), Abandon(*element4));
   }

   GIVEN("Piecewise-constructed container, assigned (disown), and then destroyed") {
      const ScopedE1 element1{555};
      const ScopedE2 element2{111};
      const ScopedE1 element3{556};
      const ScopedE2 element4{112};
      T piecewise1{Piecewise, TPair {*element1, *element2}};
      piecewise1.Assign(Disown(*element3), Disown(*element4));
   }

   GIVEN("Absorb-constructed container") {
      const ScopedE1 originalElement1{556};
      const ScopedE2 originalElement2{112};
      const ScopedE1 element1{555};
      const ScopedE2 element2{111};

      T piecewise1{Piecewise, TPair {*originalElement1, *originalElement2}};
      T piecewise2{Piecewise, TPair {*originalElement1, *originalElement2}};
      T piecewise3{Piecewise, TPair {*originalElement1, *originalElement2}};
      T piecewise4{Piecewise, TPair {*originalElement1, *originalElement2}};

      T pack_referred1{Absorb,             piecewise1};
      T pack_referred2{Absorb,       Refer(piecewise1)};
      T pack_copied   {Absorb,        Copy(piecewise1)};
      T pack_cloned   {Absorb,       Clone(piecewise1)};
      T pack_moved1   {Absorb, ::std::move(piecewise2)};
      T pack_moved2   {Absorb,        Move(piecewise3)};
      T pack_abandoned{Absorb,     Abandon(piecewise4)};
      T pack_disowned {Absorb,      Disown(piecewise1)};

      WHEN("Absorb-constructed") {
         Map_CheckState_OwnedFull<E1, E2>(pack_referred1);
         Map_CheckState_OwnedFull<E1, E2>(pack_referred2);
         Map_CheckState_OwnedFull<E1, E2>(pack_copied);
         Map_CheckState_OwnedFull<E1, E2>(pack_cloned);
         Map_CheckState_OwnedFull<E1, E2>(pack_moved1);
         Map_CheckState_OwnedFull<E1, E2>(pack_moved2);
         Map_CheckState_OwnedFull<E1, E2>(pack_abandoned);
         Map_CheckState_DisownedFull<E1, E2>(pack_disowned);

         Map_CheckState_ContainsOne(pack_referred1,  Refer(originalElement1),  Refer(originalElement2), 3);
         Map_CheckState_ContainsOne(pack_referred2,  Refer(originalElement1),  Refer(originalElement2), 3);
         Map_CheckState_ContainsOne(pack_copied,     Refer(originalElement1),  Refer(originalElement2), 1);
         Map_CheckState_ContainsOne(pack_cloned,     Clone(originalElement1),  Clone(originalElement2), 1);
         Map_CheckState_ContainsOne(pack_moved1,     Refer(originalElement1),  Refer(originalElement2), 1);
         Map_CheckState_ContainsOne(pack_abandoned,  Refer(originalElement1),  Refer(originalElement2), 1);

         if constexpr (Managed) {
            // Entries are still propagated when absorbed               
            Map_CheckState_ContainsOne(pack_disowned, Refer(originalElement1), Refer(originalElement2), 3);
         }
         else Map_CheckState_ContainsOne(pack_disowned, Disown(originalElement1), Disown(originalElement2), 3);

         if constexpr (Reffed1)
            REQUIRE(DenseCast(*originalElement1).GetReferences() == (Sparse1 ? 8 : 1));
         if constexpr (Reffed2)
            REQUIRE(DenseCast(*originalElement2).GetReferences() == (Sparse2 ? 8 : 1));

         BenchmarkMapStd("Empty/AbsorbConstructor", 30, 100,
            T temp,                                                     (new (&temp) T{Absorb, piecewise1}),
            stdmap temp_std1({*originalElement1, *originalElement2});
            stdmap temp_std2,                                           new (&temp_std2) stdmap{temp_std1}
         );
      }

      WHEN("Assigned compatible referred value") {
         auto assign_refer = [&](T& a, [[maybe_unused]] const char* intent) {
            a.Assign(*element1, *element2);

            if constexpr (CT::DeepDense<E1>)
               Many_CheckState_OwnedFull<TypeOf<E1>>(*element1);
            if constexpr (CT::DeepDense<E2>)
               Many_CheckState_OwnedFull<TypeOf<E2>>(*element2);

            Map_CheckState_OwnedFull<E1, E2>(a);
            Map_CheckState_ContainsOne(a, Refer(element1), Refer(element2));

            BenchmarkMapStd("Absorb/" + intent + "/Assign/Refer", 30, 100,
               a.Assign(*element1, *element2),            a.Assign(*originalElement1, *originalElement2),
               stdmap temp_std({*element1, *element2}),   temp_std[0] = *originalElement
            );
         };

         assign_refer(pack_referred1, "Refer");
         assign_refer(pack_copied,    "Copy");
         assign_refer(pack_cloned,    "Clone");
         assign_refer(pack_moved1,    "Move");
         assign_refer(pack_abandoned, "Abandon");
         assign_refer(pack_disowned,  "Disown");
      }

      if constexpr (CT::Map<E1>) { //TODO not tested yet
         WHEN("Assigned and absorbed referred container") {
            if (not pack_referred1.IsSame(element1->GetType())) {
               auto misabsorb_refer = [&](auto& a, int uses) {
                  REQUIRE_THROWS(a.AssignAbsorb(*element1));

                  Map_CheckState_OwnedFull<E1>(a);
                  Map_CheckState_ContainsOne(a, Refer(originalElement1), uses);
               };

               misabsorb_refer(pack_referred1, 3);
               misabsorb_refer(pack_referred2, 3);
               misabsorb_refer(pack_copied,    1);
               misabsorb_refer(pack_cloned,    1);
               misabsorb_refer(pack_moved1,    1);
               misabsorb_refer(pack_moved2,    1);
               misabsorb_refer(pack_abandoned, 1);
               misabsorb_refer(pack_disowned,  3);
               return;
            }

            auto absorb_refer = [&](auto& a, [[maybe_unused]] const char* intent) {
               a.AssignAbsorb(*element1);

               Map_Helper_TestSame(a, *element1);
               REQUIRE(a.GetUses() == element1->GetUses());
               REQUIRE(a.GetUses() == 2);
               REQUIRE(a.GetAllocation() == element1->GetAllocation());

               BenchmarkMapStd("Absorb/" + intent + "/AssignAbsorb/Refer", 30, 100,
                  a.AssignAbsorb(*element),                 a.AssignAbsorb(*originalElement),
                  stdmap temp_std1 (*element);
                  stdmap temp_std2 (*originalElement),      temp_std1 = temp_std2
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

            Map_CheckState_OwnedFull<E1, E2>(a);
            Map_CheckState_ContainsOne(a, Clone(element1), Clone(element2));

            BenchmarkMapStd("Absorb/" + intent + "/Assign/Clone", 30, 100,
               a.Assign(Clone(*element1), Clone(*element2)),      a.Assign(Clone(*originalElement1), Clone(*originalElement2)),
               stdmap temp_std({{*element1, *element2}}),         temp_std[0] = *originalElement
            );
         };

         assign_clone(pack_referred1, "Refer");
         assign_clone(pack_copied,    "Copy");
         assign_clone(pack_cloned,    "Clone");
         assign_clone(pack_moved1,    "Move");
         assign_clone(pack_abandoned, "Abandon");
         assign_clone(pack_disowned,  "Disown");
      }

      if constexpr (CT::Map<E1>) {
         WHEN("Assigned and absorbed cloned container") {
            if (not pack_referred1.IsSame(element1->GetType())) {
               auto misabsorb_clone = [&](auto& a) {
                  REQUIRE_THROWS(a.AssignAbsorb(Clone(*element1)));
                  Map_CheckState_OwnedFull<E1>(a);
                  Map_CheckState_ContainsOne(a, Clone(originalElement1));
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

            auto absorb_clone = [&](auto& a, [[maybe_unused]] const char* intent) {
               a.AssignAbsorb(Clone(*element1));

               Many_CheckState_OwnedFull<TypeOf<E1>>(*element1);
               Map_Helper_TestSame(a, *element1);
               REQUIRE(a.GetUses() == 2);
               REQUIRE(a.GetAllocation() == element1->GetAllocation());

               BenchmarkMapStd("Absorb/" + intent + "/AssignAbsorb/Clone", 30, 100,
                  a.AssignAbsorb(Clone(*element)),          a.AssignAbsorb(Clone(*originalElement)),
                  stdmap temp_std1 (*element);
                  stdmap temp_std2 (*originalElement),      temp_std1 = temp_std2
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

            Map_CheckState_OwnedFull<E1, E2>(a);
            Map_CheckState_ContainsOne(a, Refer(element1), Refer(element2));

            BenchmarkMapStd("Absorb/" + intent + "/Assign/Copy", 30, 100,
               a.Assign(Copy(*element1), Copy(*element2)),     a.Assign(Copy(*originalElement1), Copy(*originalElement2)),
               stdmap temp_std({{*element1, *element2}}),      temp_std[0] = *originalElement
            );
         };

         assign_copy(pack_referred1, "Refer");
         assign_copy(pack_copied,    "Copy");
         assign_copy(pack_cloned,    "Clone");
         assign_copy(pack_moved1,    "Move");
         assign_copy(pack_abandoned, "Abandon");
         assign_copy(pack_disowned,  "Disown");
      }

      if constexpr (CT::Map<E1>) {
         WHEN("Assigned and absorbed copied container") {
            if (not pack_referred1.IsSame(element1->GetType())) {
               auto misabsorb_copy = [&](auto& a) {
                  REQUIRE_THROWS(a.AssignAbsorb(Copy(*element1)));
                  Map_CheckState_OwnedFull<E1>(a);
                  Map_CheckState_ContainsOne(a, Refer(originalElement1));
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

            auto absorb_copy = [&](auto& a, [[maybe_unused]] const char* intent) {
               a.AssignAbsorb(Copy(*element1));

               Many_CheckState_OwnedFull<TypeOf<E1>>(*element1);
               Map_Helper_TestSame(a, *element1);
               REQUIRE(a.GetUses() == 2);
               REQUIRE(a.GetAllocation() == element1->GetAllocation());

               BenchmarkMapStd("Absorb/" + intent + "/AssignAbsorb/Copy", 30, 100,
                  a.AssignAbsorb(Copy(*element)),           a.AssignAbsorb(Copy(*originalElement)),
                  stdmap temp_std1 (*element);
                  stdmap temp_std2 (*originalElement),      temp_std1 = temp_std2
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

            Map_CheckState_OwnedFull<E1, E2>(a);
            Map_CheckState_ContainsOne(a, Refer(element1), Refer(element2));

            BenchmarkMapStd("Absorb/" + intent + "/Assign/Move", 30, 100,
               auto movable11 = *element1;
               auto movable21 = *originalElement1;
               auto movable12 = *element2;
               auto movable22 = *originalElement2;
               a.Assign(Move(movable11), Move(movable12)),                             a.Assign(Move(movable21), Move(movable22)),
               auto movable11 = *element1;
               auto movable21 = *originalElement1;
               auto movable12 = *element2;
               auto movable22 = *originalElement2;
               stdmap temp_std (::std::move(movable11), ::std::move(movable12)),       temp_std[0] = ::std::move(movable21)
            );
         };

         assign_move(pack_referred1, "Refer");
         assign_move(pack_copied,    "Copy");
         assign_move(pack_cloned,    "Clone");
         assign_move(pack_moved1,    "Move");
         assign_move(pack_abandoned, "Abandon");
         assign_move(pack_disowned,  "Disown");
      }

      if constexpr (CT::Map<E1>) {
         WHEN("Assigned and absorbed moved container") {
            if (not pack_referred1.IsSame(*element1)) {
               auto misabsorb_move = [&](T& a) {
                  auto movable1 = *element1;
                  REQUIRE_THROWS(a.AssignAbsorb(::std::move(movable1)));

                  Map_CheckState_OwnedFull<E1, E2>(a);
                  Map_CheckState_ContainsOne(a, Refer(originalElement1), Refer(originalElement2));
                  Map_CheckState_OwnedFull<int, int>(movable1);
                  Map_Helper_TestSame(movable1, 555, 111);
                  REQUIRE(movable1.GetUses() == 2);
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
               auto movable1 = *element1;
               a.AssignAbsorb(::std::move(movable1));

               Map_CheckState_Default<int, int>(movable1);
               Map_Helper_TestSame(a, 555, 111);
               REQUIRE(a.GetUses() == 2);
               REQUIRE(a.GetAllocation() == element1->GetAllocation());

               BenchmarkMapStd("Absorb/" + intent + "/AssignAbsorb/Move", 30, 100,
                  T movable1(*element1);
                  T movable2(*originalElement1);
                  a.AssignAbsorb(Move(movable1)),                                a.AssignAbsorb(Move(movable2)),
                  stdmap movable1({{*element1, *element2}});
                  stdmap movable2({{*originalElement1, *originalElement2}}),     movable1 = ::std::move(movable2)
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

            Map_CheckState_OwnedFull<E1, E2>(a);
            Map_CheckState_ContainsOne(a, Disown(element1), Disown(element2));

            BenchmarkMapStd("Absorb/" + intent + "/Assign/Disown", 30, 100,
               a.Assign(Disown(*element1), Disown(*element2)),          a.Assign(Disown(*originalElement1), Disown(*originalElement2)),
               stdmap temp_std({{*element1, *element2}}),               temp_std[0] = *originalElement
            );
         };

         assign_disown(pack_referred1, "Refer");
         assign_disown(pack_copied,    "Copy");
         assign_disown(pack_cloned,    "Clone");
         assign_disown(pack_moved1,    "Move");
         assign_disown(pack_abandoned, "Abandon");
         assign_disown(pack_disowned,  "Disown");
      }

      if constexpr (CT::Map<E1>) {
         WHEN("Assigned and absorbed disowned container") {
            if (not pack_referred1.IsSame(*element1)) {
               auto misabsorb_disown = [&](T& a) {
                  REQUIRE_THROWS(a.AssignAbsorb(Disown(*element1)));
                  Map_CheckState_OwnedFull<E1, E2>(a);
                  Map_CheckState_ContainsOne(a, Disown(originalElement1), Disown(originalElement2));
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
               REQUIRE(a.IsKeyExact(element1->GetKeyType()));
               REQUIRE(a.IsValExact(element2->GetValType()));
               REQUIRE(a == *element1);
               REQUIRE(a.IsKeyDeep() == element1->IsKeyDeep());
               REQUIRE(a.IsValDeep() == element2->IsValDeep());
               REQUIRE(a.IsKeyConstant());
               REQUIRE(a.IsValConstant() != element2->IsConstant());
               REQUIRE(a.GetUnconstrainedState() == element1->GetUnconstrainedState());
               REQUIRE(a.GetUses() == 0);
               REQUIRE_FALSE(a.GetAllocation());

               BenchmarkMapStd("Absorb/" + intent + "/AssignAbsorb/Disown", 30, 100,
                  a.AssignAbsorb(Disown(*element1)),       a.AssignAbsorb(Disown(*originalElement1)),
                  stdmap temp_std1({*element1});
                  stdmap temp_std2({*originalElement1}),   temp_std1 = temp_std2
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

            Map_CheckState_OwnedFull<E1, E2>(a);
            Map_CheckState_ContainsOne(a, Refer(element1), Refer(element2));

            BenchmarkMapStd("Absorb/" + intent + "/Assign/Abandon", 30, 100,
               auto movable11 = *element1;
               auto movable21 = *originalElement1;
               auto movable12 = *element2;
               auto movable22 = *originalElement2;
               a.Assign(Abandon(movable11), Abandon(movable12)),     a.Assign(Abandon(movable21), Abandon(movable22)),
               auto movable11 = *element1;
               auto movable21 = *originalElement1;
               auto movable12 = *element2;
               auto movable22 = *originalElement2;
               stdmap temp_std(::std::move(movable11)),              temp_std[0] = ::std::move(movable21)
            );
         };

         assign_abandon(pack_referred1, "Refer");
         assign_abandon(pack_copied,    "Copy");
         assign_abandon(pack_cloned,    "Clone");
         assign_abandon(pack_moved1,    "Move");
         assign_abandon(pack_abandoned, "Abandon");
         assign_abandon(pack_disowned,  "Disown");
      }

      if constexpr (CT::Map<E1>) {
         WHEN("Assigned and absorbed abandoned container") {
            if (not pack_referred1.IsSame(element1->GetType())) {
               auto misabsorb_abandon = [&](auto& a) {
                  auto movable = *element1;
                  REQUIRE_THROWS(a.AssignAbsorb(Abandon(movable)));

                  Map_CheckState_OwnedFull<E1>(a);
                  Map_CheckState_ContainsOne(a, Refer(originalElement1));
                  Many_CheckState_OwnedFull<int>(movable);
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

            auto absorb_abandon = [&](auto& a, [[maybe_unused]] const char* intent) {
               auto movable = *element1;
               a.AssignAbsorb(Abandon(movable));

               Many_CheckState_Abandoned<TypeOf<E1>>(movable);
               Map_Helper_TestSame(a, *element1);
               REQUIRE(a.GetUses() == 2);
               REQUIRE(a.GetAllocation() == element1->GetAllocation());

               BenchmarkMapStd("Absorb/" + intent + "/AssignAbsorb/Abandon", 30, 100,
                  T movable1 = *element;
                  T movable2 = *originalElement;
                  a.AssignAbsorb(Abandon(movable1)),         a.AssignAbsorb(Abandon(movable2)),
                  stdmap movable1 (*element);
                  stdmap movable2 (*originalElement);
                  stdmap temp_std = ::std::move(movable1),   temp_std = ::std::move(movable2)
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
            Map_CheckState_Default<E1, E2>(a);
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
            Map_Helper_TestSame(a, backup, not allow_change_in_constness);
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

            Map_Helper_TestSame(absorbed1, compare_against);
            Map_Helper_TestSame(absorbed2, compare_against);
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

            Map_CheckState_Default<E1, E2>(a);
            Map_CheckState_OwnedFull<E1, E2>(absorbed);
            Map_Helper_TestSame(absorbed, backup);
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

            Map_CheckState_Default<E1, E2>(a);
            Map_CheckState_OwnedFull<E1, E2>(absorbed);
            Map_Helper_TestSame(absorbed, backup);
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

            Map_CheckState_Abandoned<E1, E2>(a);
            Map_CheckState_OwnedFull<E1, E2>(absorbed);
            Map_Helper_TestSame(absorbed, backup);
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

            Map_CheckState_OwnedFull<E1, E2>(a);
            Map_CheckState_DisownedFull<E1, E2>(absorbed);
            Map_Helper_TestSame(absorbed, a, false);
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
         Map_CheckState_DisownedFull<E1, E2>(pack_disowned);
         Map_CheckState_DisownedFull<E1, E2>(absorbed);
         REQUIRE(absorbed.GetRaw() == pack_disowned.GetRaw());
         REQUIRE(absorbed.IsKeyExact(pack_disowned.GetKeyType()));
         REQUIRE(absorbed.IsValExact(pack_disowned.GetValType()));
         REQUIRE(absorbed == pack_disowned);
         REQUIRE(absorbed.IsKeyDeep() == pack_disowned.IsKeyDeep());
         REQUIRE(absorbed.IsValDeep() == pack_disowned.IsValDeep());
         REQUIRE(absorbed.IsKeyConstant() == pack_disowned.IsKeyConstant());
         REQUIRE(absorbed.IsValConstant() == pack_disowned.IsValConstant());
         REQUIRE(absorbed.GetUnconstrainedState() == pack_disowned.GetUnconstrainedState());
         REQUIRE(absorbed.GetUses() == 3);
      }
      
      WHEN("Absorbed by copy") {
         const bool managed_sparse = CT::Sparse<E1, E2> and Managed;
         auto absorb_construct_copy = [&](T& a, int uses, int entry_refs, int indi_refs) {
            T absorbed {Copy {a}};

            REQUIRE(a.GetUses() == uses);
            Map_CheckState_OwnedFull<E1, E2>(absorbed);
            REQUIRE(absorbed.GetUses() == 1);
            REQUIRE(absorbed == a);
            REQUIRE(absorbed.GetRaw() != a.GetRaw());
            REQUIRE(absorbed.template KeyAsAt<E1>(0) == a.template KeyAsAt<E1>(0));
            REQUIRE(absorbed.template ValAsAt<E2>(0) == a.template ValAsAt<E2>(0));

            if constexpr (CT::Sparse<E1>) {
               auto entry = *absorbed.GetKeyEntries();
               
               if (entry)
                  REQUIRE(entry->GetUses() == entry_refs);

               if constexpr (CT::Referenced<Decay<E1>>) {
                  auto e = absorbed.template KeyAsAt<E1>(0);
                  REQUIRE(DenseCast(e).GetReferences() == indi_refs);
               }
            }

            if constexpr (CT::Sparse<E2>) {
               auto entry = *absorbed.GetValEntries();
               
               if (entry)
                  REQUIRE(entry->GetUses() == entry_refs);

               if constexpr (CT::Referenced<Decay<E2>>) {
                  auto e = absorbed.template ValAsAt<E2>(0);
                  REQUIRE(DenseCast(e).GetReferences() == indi_refs);
               }
            }
         };

         absorb_construct_copy(pack_referred1, 3, managed_sparse ? 9 : 3, 9);
         Map_CheckState_OwnedFull<E1, E2>(pack_referred1);

         absorb_construct_copy(pack_referred2, 3, managed_sparse ? 9 : 3, 9);
         Map_CheckState_OwnedFull<E1, E2>(pack_referred2);

         absorb_construct_copy(pack_copied,    1, managed_sparse ? 9 : 3, 9);
         Map_CheckState_OwnedFull<E1, E2>(pack_copied);

         absorb_construct_copy(pack_cloned,    1, 2, 2);
         Map_CheckState_OwnedFull<E1, E2>(pack_cloned);

         absorb_construct_copy(pack_moved1,    1, managed_sparse ? 9 : 1, 9);
         Map_CheckState_OwnedFull<E1, E2>(pack_moved1);

         absorb_construct_copy(pack_moved2,    1, managed_sparse ? 9 : 1, 9);
         Map_CheckState_OwnedFull<E1, E2>(pack_moved2);

         absorb_construct_copy(pack_abandoned, 1, managed_sparse ? 9 : 1, 9);
         Map_CheckState_OwnedFull<E1, E2>(pack_abandoned);

         absorb_construct_copy(pack_disowned,  3, managed_sparse ? 9 : 0, 9);
         Map_CheckState_DisownedFull<E1, E2>(pack_disowned);
      }
      
      WHEN("Absorbed by clone") {
         auto absorb_construct_clone = [&](T& a) {
            T absorbed {Clone {a}};

            Map_CheckState_OwnedFull<E1, E2>(absorbed);
            REQUIRE((absorbed == a) == CT::Dense<E1, E2>);
            REQUIRE(absorbed.GetUses() == 1);
         };

         absorb_construct_clone(pack_referred1);
         Map_CheckState_OwnedFull<E1, E2>(pack_referred1);

         absorb_construct_clone(pack_referred2);
         Map_CheckState_OwnedFull<E1, E2>(pack_referred2);

         absorb_construct_clone(pack_copied);
         Map_CheckState_OwnedFull<E1, E2>(pack_copied);

         absorb_construct_clone(pack_cloned);
         Map_CheckState_OwnedFull<E1, E2>(pack_cloned);

         absorb_construct_clone(pack_moved1);
         Map_CheckState_OwnedFull<E1, E2>(pack_moved1);

         absorb_construct_clone(pack_moved2);
         Map_CheckState_OwnedFull<E1, E2>(pack_moved2);

         absorb_construct_clone(pack_abandoned);
         Map_CheckState_OwnedFull<E1, E2>(pack_abandoned);

         absorb_construct_clone(pack_disowned);
         Map_CheckState_DisownedFull<E1, E2>(pack_disowned);
      }
      
      /*WHEN("Emplace (overwrite)") {
         auto emplace_overwrite = [&](auto& a, [[maybe_unused]] const char* intent) {
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
               std::string("Absorb/") + intent + "/Emplace(" + static_cast<std::string>(NameOf<E>()) + ")", 30,
               auto movable1 = *element;
               auto movable2 = *originalElement;
               a.Emplace(::std::move(movable1)),   a.Emplace(::std::move(movable2))
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
         auto emplace_overwrite_describe = [&](auto& a, [[maybe_unused]] const char* intent) {
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
                  std::string("Absorb/") + intent + "/Emplace(Describe(" + static_cast<std::string>(NameOf<E>()) + "))", 30,
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
         auto clear_full = [&](T& a, [[maybe_unused]] const char* intent, int uses = 1) {
            BenchmarkMapStd("Absorb/" + intent + "/Clear", 30, 100,
               T temp = a,                                  temp.Clear(),
               stdmap temp_std({{*element1, *element2}}),   temp_std.clear()
            );

            a.Clear();

            if (uses != 1)
               Map_CheckState_Default<E1, E2>(a, true);
            else
               Map_CheckState_OwnedEmpty<E1, E2>(a);
         };

         clear_full(pack_referred1, "Refer", 3);
         clear_full(pack_copied,    "Copy");
         clear_full(pack_cloned,    "Clone");
         clear_full(pack_moved1,    "Move");
         clear_full(pack_abandoned, "Abandon");
         clear_full(pack_disowned,  "Disown", 0);
      }

      WHEN("Reset") {
         auto reset_full = [&](T& a, [[maybe_unused]] const char* intent) {
            BenchmarkMapStd("Absorb/" + intent + "/Reset", 30, 100,
               T temp = a,                                temp.Reset(),
               stdmap temp_std({{*element1, *element2}}), temp_std.clear()
            );

            a.Reset();

            Map_CheckState_Default<E1, E2>(a);
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
         ScopedE2 e2 {2};
         T another_pack1{Piecewise, TPair {*e1, *e2}};
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
            BenchmarkMapStd("Absorb/" + intent + "/operator==", 30, 100,
               (void) 0,                                       dont_optimize |= (a == same_pack),
               const stdmap a_std (*element1, *element2);
               const stdmap another_pack1_std (*e1, *e2),      dont_optimize |= (a_std == another_pack1_std)
            );
            BenchmarkMapStd("Absorb/" + intent + "/operator!=", 30, 100,
               (void) 0,                                       dont_optimize |= (a != same_pack),
               const stdmap a_std (*element1, *element2);
               const stdmap another_pack1_std (*e1, *e2),      dont_optimize |= (a_std != another_pack1_std)
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
            REQUIRE      (*pack_cloned.template GetDenseAt<0>(0).template As<Decay<E1>>() == DenseCast(*originalElement1));
            REQUIRE      (*pack_cloned.template GetDenseAt<1>(0).template As<Decay<E2>>() == DenseCast(*originalElement2));
            REQUIRE_FALSE(*pack_cloned.template GetDenseAt<0>(0).template As<E1>() == *originalElement1);
            REQUIRE_FALSE(*pack_cloned.template GetDenseAt<1>(0).template As<E2>() == *originalElement2);
            REQUIRE_FALSE(*pack_cloned.template GetDenseAt<0>(0).template As<E1>() == *e1);
            REQUIRE_FALSE(*pack_cloned.template GetDenseAt<1>(0).template As<E2>() == *e2);
         }
         else contains_full(pack_cloned);

         contains_full(pack_moved1);
         contains_full(pack_moved2);
         contains_full(pack_abandoned);
         contains_full(pack_disowned);

         [[maybe_unused]] volatile bool dont_optimize = false;
         BenchmarkMap("Absorb/Contains", 30,
            (void) 0, dont_optimize |= pack_referred1.Contains(TPair {*element1, *element2})
         );
      }
   }
   
   GIVEN("Two absorb-constructed containers") {
      const ScopedE1 e556 {556};
      const ScopedE2 e557 {557};
      const ScopedE1 e6   {6};
      const ScopedE2 e7   {7};

      T piecewise1{Piecewise, TPair {*e556, *e557}};
      T piecewise2{Piecewise, TPair {*e6,   *e7  }};
      T src {Absorb, Abandon(piecewise1)};
      T dst {Absorb, Abandon(piecewise2)};

      /// MARK: GetHandle                                                     
      WHEN("GetHandle is called on mutable container") {
         auto src_handle_default = src.GetHandle();
         if constexpr (CT::Untyped<T>)
            static_assert(::std::same_as<decltype(src_handle_default), THandlePair<Handle, HandleMut>>);
         else
            static_assert(::std::same_as<decltype(src_handle_default), THandlePair<THandle<ConstAll<E1&>>, THandle<E2&>>>);

         auto src_handle = src_handle_default.ForceMutable();
         if constexpr (CT::Untyped<T>)
            static_assert(::std::same_as<decltype(src_handle), THandlePair<HandleMut, HandleMut>>);
         else
            static_assert(::std::same_as<decltype(src_handle), THandlePair<THandle<E1&>, THandle<E2&>>>);

         auto src_valid_element = src.GetAt(0);
         while(src_handle.Get() != src_valid_element)
            ++src_handle;

         auto src_data1 = src_handle.template Get<E1, 0>();
         auto src_data2 = src_handle.template Get<E2, 1>();
         AllocationPtr const* src_entries1 = nullptr;
         AllocationPtr const* src_entries2 = nullptr;

         HandlePair_CheckState_OwnedFull<E1, E2>(src_handle);

         if constexpr (Sparse1) {
            src_entries1 = src_handle.template GetEntries<0>();
            REQUIRE(*src_entries1 == e556.entries[1]);
            if constexpr (Managed)
               REQUIRE(e556.entries[1]->GetUses() == 2);
         }

         if constexpr (Sparse2) {
            src_entries2 = src_handle.template GetEntries<1>();
            REQUIRE(*src_entries2 == e557.entries[1]);
            if constexpr (Managed)
               REQUIRE(e557.entries[1]->GetUses() == 2);
         }

         if constexpr (Reffed1) {
            REQUIRE(DenseCast(src_data1).GetReferences() == (Sparse1 ? 2 : 1));
            REQUIRE(DenseCast(src_data1).destroyed == false);
         }

         if constexpr (Reffed2) {
            REQUIRE(DenseCast(src_data2).GetReferences() == (Sparse2 ? 2 : 1));
            REQUIRE(DenseCast(src_data2).destroyed == false);
         }

         auto dst_handle = dst.GetHandle().ForceMutable();
         auto dst_valid_element = dst.GetAt(0);
         while(dst_handle.Get() != dst_valid_element)
            ++dst_handle;

         auto dst_data1 = dst_handle.template Get<E1, 0>();
         auto dst_data2 = dst_handle.template Get<E2, 1>();
         AllocationPtr const* dst_entries1 = nullptr;
         AllocationPtr const* dst_entries2 = nullptr;

         HandlePair_CheckState_OwnedFull<E1, E2>(dst_handle);

         if constexpr (Sparse1) {
            dst_entries1 = dst_handle.template GetEntries<0>();
            REQUIRE(*dst_entries1 == e6.entries[1]);
            if constexpr (Managed)
               REQUIRE(e6.entries[1]->GetUses() == 2);
            REQUIRE(dst_entries1 != src_entries1);
         }

         if constexpr (Sparse2) {
            dst_entries2 = dst_handle.template GetEntries<1>();
            REQUIRE(*dst_entries2 == e7.entries[1]);
            if constexpr (Managed)
               REQUIRE(e7.entries[1]->GetUses() == 2);
            REQUIRE(dst_entries2 != src_entries2);
         }

         REQUIRE(dst_data1 != src_data1);
         REQUIRE(dst_data2 != src_data2);

         if constexpr (Reffed1) {
            REQUIRE(DenseCast(dst_data1).GetReferences() == (Sparse1 ? 2 : 1));
            REQUIRE(DenseCast(dst_data1).destroyed == false);
         }

         if constexpr (Reffed2) {
            REQUIRE(DenseCast(dst_data2).GetReferences() == (Sparse2 ? 2 : 1));
            REQUIRE(DenseCast(dst_data2).destroyed == false);
         }

         THEN("Handle assigned to another container") {
            REQUIRE_NOTHROW(dst_handle.Assign(Move(src_handle)));

            HandlePair_CheckState_OwnedFull<E1, E2>(src_handle);
            HandlePair_CheckState_OwnedFull<E1, E2>(dst_handle);
            REQUIRE(src_handle.template Get<E1, 0>() == src_data1);
            REQUIRE(src_handle.template Get<E2, 1>() == src_data2);
            REQUIRE(dst_handle.template Get<E1, 0>() == dst_data1);
            REQUIRE(dst_handle.template Get<E2, 1>() == dst_data2);
            
            auto& moved_in1 = DenseCast(dst_data1);
            if constexpr (Sparse1) {
               REQUIRE(src_handle.template GetEntries<0>() == src_entries1);
               REQUIRE(*src_data1 == nullptr);
               REQUIRE(*src_entries1 == nullptr);

               REQUIRE(dst_handle.template GetEntries<0>() == dst_entries1);
               REQUIRE(*dst_data1 == *e556);
               REQUIRE(*dst_entries1 == e556.entries[1]);
               if constexpr (Managed) {
                  REQUIRE(e556.entries[1]->GetUses() == 2);
                  REQUIRE(e6.entries[1]->GetUses() == 1);
               }

               if constexpr (Reffed1) {
                  REQUIRE(DenseCast(*e6).GetReferences() == 1);
                  REQUIRE(moved_in1.GetReferences() == 2);
                  REQUIRE(moved_in1.data == DenseCast(*e556).data);
                  REQUIRE(moved_in1.destroyed == false);
                  REQUIRE(moved_in1.moved_in == false);
                  REQUIRE(moved_in1.moved_out == false);
               }
            }
            else {
               auto& moved_out = DenseCast(src_data1);
               if constexpr (Reffed1) {
                  REQUIRE(moved_out.GetReferences() == 1);
                  REQUIRE(moved_out.destroyed == false);
                  REQUIRE(moved_out.moved_in == false);
                  REQUIRE(moved_out.moved_out == true);

                  REQUIRE(moved_in1.GetReferences() == 1);
                  REQUIRE(moved_in1.data == DenseCast(*e556).data);
                  REQUIRE(moved_in1.destroyed == false);
                  REQUIRE(moved_in1.moved_in == true);
                  REQUIRE(moved_in1.moved_out == false);
               }
            }

            auto& moved_in2 = DenseCast(dst_data2);
            if constexpr (Sparse2) {
               REQUIRE(src_handle.template GetEntries<1>() == src_entries2);
               REQUIRE(*src_data2 == nullptr);
               REQUIRE(*src_entries2 == nullptr);

               REQUIRE(dst_handle.template GetEntries<1>() == dst_entries2);
               REQUIRE(*dst_data2 == *e557);
               REQUIRE(*dst_entries2 == e557.entries[1]);
               if constexpr (Managed) {
                  REQUIRE(e557.entries[1]->GetUses() == 2);
                  REQUIRE(e7.entries[1]->GetUses() == 1);
               }

               if constexpr (Reffed2) {
                  REQUIRE(DenseCast(*e7).GetReferences() == 1);
                  REQUIRE(moved_in2.GetReferences() == 2);
                  REQUIRE(moved_in2.data == DenseCast(*e557).data);
                  REQUIRE(moved_in2.destroyed == false);
                  REQUIRE(moved_in2.moved_in == false);
                  REQUIRE(moved_in2.moved_out == false);
               }
            }
            else {
               auto& moved_out = DenseCast(src_data2);
               if constexpr (Reffed2) {
                  REQUIRE(moved_out.GetReferences() == 1);
                  REQUIRE(moved_out.destroyed == false);
                  REQUIRE(moved_out.moved_in == false);
                  REQUIRE(moved_out.moved_out == true);

                  REQUIRE(moved_in2.GetReferences() == 1);
                  REQUIRE(moved_in2.data == DenseCast(*e557).data);
                  REQUIRE(moved_in2.destroyed == false);
                  REQUIRE(moved_in2.moved_in == true);
                  REQUIRE(moved_in2.moved_out == false);
               }
            }
         }
         
         THEN("Handle is swapped with another container's handle") {
            REQUIRE_NOTHROW(dst_handle.SwapContents(src_handle));

            HandlePair_CheckState_OwnedFull<E1, E2>(src_handle);
            HandlePair_CheckState_OwnedFull<E1, E2>(dst_handle);
            REQUIRE(src_handle.template Get<E1, 0>() == src_data1);
            REQUIRE(src_handle.template Get<E2, 1>() == src_data2);
            REQUIRE(dst_handle.template Get<E1, 0>() == dst_data1);
            REQUIRE(dst_handle.template Get<E2, 1>() == dst_data2);
            
            auto& moved_in1  = DenseCast(dst_data1);
            auto& moved_out1 = DenseCast(src_data1);
            REQUIRE(moved_in1  == DenseCast(*e556));
            REQUIRE(moved_out1 == DenseCast(*e6));

            if constexpr (Sparse1) {
               REQUIRE(src_handle.template GetEntries<0>() == src_entries1);
               REQUIRE(dst_handle.template GetEntries<0>() == dst_entries1);
   
               REQUIRE(*dst_entries1 == e556.entries[1]);
               REQUIRE(*src_entries1 == e6.entries[1]);
               if constexpr (Managed) {
                  REQUIRE(e556.entries[1]->GetUses() == 2);
                  REQUIRE(e6.entries[1]->GetUses() == 2);
               }

               if constexpr (Reffed1) {
                  REQUIRE(moved_out1.GetReferences() == 2);
                  REQUIRE(moved_out1.data == DenseCast(*e6).data);
                  REQUIRE(moved_out1.destroyed == false);
                  REQUIRE(moved_out1.moved_in == false);
                  REQUIRE(moved_out1.moved_out == false);

                  REQUIRE(moved_in1.GetReferences() == 2);
                  REQUIRE(moved_in1.data == DenseCast(*e556).data);
                  REQUIRE(moved_in1.destroyed == false);
                  REQUIRE(moved_in1.moved_in == false);
                  REQUIRE(moved_in1.moved_out == false);
               }
            }
            else {
               if constexpr (Reffed1) {
                  REQUIRE(moved_out1.GetReferences() == 1);
                  REQUIRE(moved_out1.destroyed == false);
                  REQUIRE(moved_out1.moved_in == true);
                  REQUIRE(moved_out1.moved_out == false);

                  REQUIRE(moved_in1.GetReferences() == 1);
                  REQUIRE(moved_in1.destroyed == false);
                  REQUIRE(moved_in1.moved_in == true);
                  REQUIRE(moved_in1.moved_out == false);
               }
            }

            auto& moved_in2  = DenseCast(dst_data2);
            auto& moved_out2 = DenseCast(src_data2);
            REQUIRE(moved_in2  == DenseCast(*e557));
            REQUIRE(moved_out2 == DenseCast(*e7));

            if constexpr (Sparse2) {
               REQUIRE(src_handle.template GetEntries<1>() == src_entries2);
               REQUIRE(dst_handle.template GetEntries<1>() == dst_entries2);
   
               REQUIRE(*dst_entries2 == e557.entries[1]);
               REQUIRE(*src_entries2 == e7.entries[1]);
               if constexpr (Managed) {
                  REQUIRE(e557.entries[1]->GetUses() == 2);
                  REQUIRE(e7.entries[1]->GetUses() == 2);
               }

               if constexpr (Reffed2) {
                  REQUIRE(moved_out2.GetReferences() == 2);
                  REQUIRE(moved_out2.data == DenseCast(*e7).data);
                  REQUIRE(moved_out2.destroyed == false);
                  REQUIRE(moved_out2.moved_in == false);
                  REQUIRE(moved_out2.moved_out == false);

                  REQUIRE(moved_in2.GetReferences() == 2);
                  REQUIRE(moved_in2.data == DenseCast(*e557).data);
                  REQUIRE(moved_in2.destroyed == false);
                  REQUIRE(moved_in2.moved_in == false);
                  REQUIRE(moved_in2.moved_out == false);
               }
            }
            else {
               if constexpr (Reffed2) {
                  REQUIRE(moved_out2.GetReferences() == 1);
                  REQUIRE(moved_out2.destroyed == false);
                  REQUIRE(moved_out2.moved_in == true);
                  REQUIRE(moved_out2.moved_out == false);

                  REQUIRE(moved_in2.GetReferences() == 1);
                  REQUIRE(moved_in2.destroyed == false);
                  REQUIRE(moved_in2.moved_in == true);
                  REQUIRE(moved_in2.moved_out == false);
               }
            }

            // We should be able to do this indefinitely                
            for(int i = 0; i < 101; ++i)
               dst_handle.SwapContents(src_handle);
         }
         
         THEN("Handle moved into a local handle") {
            THandlePair<THandle<E1>, THandle<E2>> local {Absorb, Move(src_handle)};

            HandlePair_CheckState_OwnedFull<E1, E2>(src_handle);
            HandlePair_CheckState_OwnedFull<E1, E2>(local, CT::Dense<E1, E2>);
            REQUIRE(src_handle.template Get<E1, 0>() == src_data1);
            REQUIRE(src_handle.template Get<E2, 1>() == src_data2);
            REQUIRE(local.template Get<E1, 0>() != src_data1);
            REQUIRE(local.template Get<E2, 1>() != src_data2);
            
            auto& moved_in1 = DenseCast(local.template Get<E1, 0>());
            REQUIRE(moved_in1 == DenseCast(*e556));
            if constexpr (Sparse1) {
               REQUIRE(src_handle.template GetEntries<0>() == src_entries1);
               REQUIRE(local.template GetEntries<0>() != src_entries1);

               REQUIRE(*src_data1 == nullptr);
               REQUIRE(*src_entries1 == nullptr);

               REQUIRE(local.template GetEntries<0>()[0] == e556.entries[1]);
               if constexpr (Managed) {
                  REQUIRE(e556.entries[1]->GetUses() == 2);
                  REQUIRE(e6.entries[1]->GetUses() == 2);
               }

               if constexpr (Reffed1) {
                  REQUIRE(DenseCast(*e6).GetReferences() == 2);
                  REQUIRE(moved_in1.GetReferences() == 2);
                  REQUIRE(moved_in1.data == DenseCast(*e556).data);
                  REQUIRE(moved_in1.destroyed == false);
                  REQUIRE(moved_in1.moved_in == false);
                  REQUIRE(moved_in1.moved_out == false);
               }
            }
            else {
               auto& moved_out = DenseCast(src_data1);

               if constexpr (Reffed1) {
                  REQUIRE(moved_out.GetReferences() == 1);
                  REQUIRE(moved_out.destroyed == false);
                  REQUIRE(moved_out.moved_in == false);
                  REQUIRE(moved_out.moved_out == true);

                  REQUIRE(moved_in1.GetReferences() == 1);
                  REQUIRE(moved_in1.data == DenseCast(*e556).data);
                  REQUIRE(moved_in1.destroyed == false);
                  REQUIRE(moved_in1.moved_in == true);
                  REQUIRE(moved_in1.moved_out == false);
               }
            }

            auto& moved_in2 = DenseCast(local.template Get<E2, 1>());
            REQUIRE(moved_in2 == DenseCast(*e557));
            if constexpr (Sparse2) {
               REQUIRE(src_handle.template GetEntries<1>() == src_entries2);
               REQUIRE(local.template GetEntries<1>() != src_entries2);

               REQUIRE(*src_data2 == nullptr);
               REQUIRE(*src_entries2 == nullptr);

               REQUIRE(local.template GetEntries<1>()[0] == e557.entries[1]);
               if constexpr (Managed) {
                  REQUIRE(e557.entries[1]->GetUses() == 2);
                  REQUIRE(e7.entries[1]->GetUses() == 2);
               }

               if constexpr (Reffed2) {
                  REQUIRE(DenseCast(*e7).GetReferences() == 2);
                  REQUIRE(moved_in2.GetReferences() == 2);
                  REQUIRE(moved_in2.data == DenseCast(*e557).data);
                  REQUIRE(moved_in2.destroyed == false);
                  REQUIRE(moved_in2.moved_in == false);
                  REQUIRE(moved_in2.moved_out == false);
               }
            }
            else {
               auto& moved_out = DenseCast(src_data2);

               if constexpr (Reffed2) {
                  REQUIRE(moved_out.GetReferences() == 1);
                  REQUIRE(moved_out.destroyed == false);
                  REQUIRE(moved_out.moved_in == false);
                  REQUIRE(moved_out.moved_out == true);

                  REQUIRE(moved_in2.GetReferences() == 1);
                  REQUIRE(moved_in2.data == DenseCast(*e557).data);
                  REQUIRE(moved_in2.destroyed == false);
                  REQUIRE(moved_in2.moved_in == true);
                  REQUIRE(moved_in2.moved_out == false);
               }
            }
         }

         THEN("Handle is swapped with local handle, and then back to container") {
            THandlePair<THandle<E1>, THandle<E2>> local;
            REQUIRE_NOTHROW(local.SwapContents(src_handle));
            auto local_data1 = local.template Get<E1, 0>();
            auto local_data2 = local.template Get<E2, 1>();
            AllocationPtr const* local_entries1 = nullptr;
            AllocationPtr const* local_entries2 = nullptr;

            HandlePair_CheckState_OwnedFull<E1, E2>(src_handle);
            HandlePair_CheckState_OwnedFull<E1, E2>(local, CT::Dense<E1, E2>);
            REQUIRE(src_handle.template Get<E1, 0>() == src_data1);
            REQUIRE(src_handle.template Get<E2, 1>() == src_data2);
            REQUIRE(local_data1);
            REQUIRE(local_data2);
            REQUIRE(local_data1 != src_data1);
            REQUIRE(local_data2 != src_data2);

            auto& moved_in1 = DenseCast(local_data1);
            REQUIRE(moved_in1 == DenseCast(*e556));
            if constexpr (Sparse1) {
               REQUIRE(src_handle.template GetEntries<0>() == src_entries1);
               local_entries1 = local.template GetEntries<0>();
               REQUIRE(local_entries1 != nullptr);
               REQUIRE(local_entries1 != src_entries1);

               REQUIRE(*src_data1 == nullptr);
               REQUIRE(*src_entries1 == nullptr);

               REQUIRE(*local_entries1 == e556.entries[1]);
               if constexpr (Managed)
                  REQUIRE(e556.entries[1]->GetUses() == 2);

               if constexpr (Reffed1) {
                  REQUIRE(moved_in1.GetReferences() == 2);
                  REQUIRE(moved_in1.data == DenseCast(*e556).data);
                  REQUIRE(moved_in1.destroyed == false);
                  REQUIRE(moved_in1.moved_in == false);
                  REQUIRE(moved_in1.moved_out == false);
               }
            }
            else {
               auto& moved_out = DenseCast(src_data1);

               if constexpr (Reffed1) {
                  REQUIRE(moved_out.GetReferences() == 1);
                  REQUIRE(moved_out.destroyed == false);
                  REQUIRE(moved_out.moved_in == true);
                  REQUIRE(moved_out.moved_out == false);

                  REQUIRE(moved_in1.GetReferences() == 1);
                  REQUIRE(moved_in1.data == DenseCast(*e556).data);
                  REQUIRE(moved_in1.destroyed == false);
                  REQUIRE(moved_in1.moved_in == true);
                  REQUIRE(moved_in1.moved_out == false);
               }
            }

            auto& moved_in2 = DenseCast(local_data2);
            REQUIRE(moved_in2 == DenseCast(*e557));
            if constexpr (Sparse2) {
               REQUIRE(src_handle.template GetEntries<1>() == src_entries2);
               local_entries2 = local.template GetEntries<1>();
               REQUIRE(local_entries2 != nullptr);
               REQUIRE(local_entries2 != src_entries2);

               REQUIRE(*src_data2 == nullptr);
               REQUIRE(*src_entries2 == nullptr);

               REQUIRE(*local_entries2 == e557.entries[1]);
               if constexpr (Managed)
                  REQUIRE(e557.entries[1]->GetUses() == 2);

               if constexpr (Reffed2) {
                  REQUIRE(moved_in2.GetReferences() == 2);
                  REQUIRE(moved_in2.data == DenseCast(*e557).data);
                  REQUIRE(moved_in2.destroyed == false);
                  REQUIRE(moved_in2.moved_in == false);
                  REQUIRE(moved_in2.moved_out == false);
               }
            }
            else {
               auto& moved_out = DenseCast(src_data2);

               if constexpr (Reffed2) {
                  REQUIRE(moved_out.GetReferences() == 1);
                  REQUIRE(moved_out.destroyed == false);
                  REQUIRE(moved_out.moved_in == true);
                  REQUIRE(moved_out.moved_out == false);

                  REQUIRE(moved_in2.GetReferences() == 1);
                  REQUIRE(moved_in2.data == DenseCast(*e557).data);
                  REQUIRE(moved_in2.destroyed == false);
                  REQUIRE(moved_in2.moved_in == true);
                  REQUIRE(moved_in2.moved_out == false);
               }
            }

            REQUIRE_NOTHROW(local.SwapContents(src_handle));
            REQUIRE(src_handle.template Get<E1, 0>() == src_data1);
            REQUIRE(src_handle.template Get<E2, 1>() == src_data2);
            REQUIRE(local.template Get<E1, 0>() == local_data1);
            REQUIRE(local.template Get<E2, 1>() == local_data2);

            if constexpr (Sparse1) {
               REQUIRE(src_handle.template GetEntries<0>() == src_entries1);
               REQUIRE(local.template GetEntries<0>() != src_entries1);
               REQUIRE(local.template GetEntries<0>() == local_entries1);
               REQUIRE(*local_entries1 == nullptr);
               REQUIRE(*src_data1 != nullptr);

               REQUIRE(*src_entries1 == e556.entries[1]);
               if constexpr (Managed)
                  REQUIRE(e556.entries[1]->GetUses() == 2);

               if constexpr (Reffed1) {
                  auto& moved_out = DenseCast(src_data1);
                  REQUIRE(moved_out.GetReferences() == 2);
                  REQUIRE(moved_out.data == DenseCast(*e556).data);
                  REQUIRE(moved_out.destroyed == false);
                  REQUIRE(moved_out.moved_in == false);
                  REQUIRE(moved_out.moved_out == false);
               }
            }
            else {
               auto& moved_out = DenseCast(src_data1);

               if constexpr (Reffed1) {
                  REQUIRE(moved_in1.GetReferences() == 1);
                  REQUIRE(moved_in1.destroyed == false);
                  REQUIRE(moved_in1.moved_in == true);
                  REQUIRE(moved_in1.moved_out == false);

                  REQUIRE(moved_out.GetReferences() == 1);
                  REQUIRE(moved_out.data == DenseCast(*e556).data);
                  REQUIRE(moved_out.destroyed == false);
                  REQUIRE(moved_out.moved_in == true);
                  REQUIRE(moved_out.moved_out == false);
               }
            }

            if constexpr (Sparse2) {
               REQUIRE(src_handle.template GetEntries<1>() == src_entries2);
               REQUIRE(local.template GetEntries<1>() != src_entries2);
               REQUIRE(local.template GetEntries<1>() == local_entries2);
               REQUIRE(*local_entries2 == nullptr);
               REQUIRE(*src_data2 != nullptr);

               REQUIRE(*src_entries2 == e557.entries[1]);
               if constexpr (Managed)
                  REQUIRE(e557.entries[1]->GetUses() == 2);

               if constexpr (Reffed2) {
                  auto& moved_out = DenseCast(src_data2);
                  REQUIRE(moved_out.GetReferences() == 2);
                  REQUIRE(moved_out.data == DenseCast(*e557).data);
                  REQUIRE(moved_out.destroyed == false);
                  REQUIRE(moved_out.moved_in == false);
                  REQUIRE(moved_out.moved_out == false);
               }
            }
            else {
               auto& moved_out = DenseCast(src_data2);

               if constexpr (Reffed2) {
                  REQUIRE(moved_in2.GetReferences() == 1);
                  REQUIRE(moved_in2.destroyed == false);
                  REQUIRE(moved_in2.moved_in == true);
                  REQUIRE(moved_in2.moved_out == false);

                  REQUIRE(moved_out.GetReferences() == 1);
                  REQUIRE(moved_out.data == DenseCast(*e557).data);
                  REQUIRE(moved_out.destroyed == false);
                  REQUIRE(moved_out.moved_in == true);
                  REQUIRE(moved_out.moved_out == false);
               }
            }

            // We should be able to do this indefinitely                
            for(int i = 0; i < 101; ++i)
               local.SwapContents(src_handle);
         }
      }

      WHEN("GetHandle is called on constant container") {
         T const& pack_constant = src;
         auto handle = pack_constant.GetHandle();
         auto first_valid_element = pack_constant.GetAt(0);
         while(handle.Get() != first_valid_element)
            ++handle;

         if constexpr (CT::Untyped<T>)
            static_assert(::std::same_as<decltype(handle), THandlePair<Handle, Handle>>);
         else
            static_assert(::std::same_as<decltype(handle), THandlePair<THandle<ConstAll<E1&>>, THandle<ConstAll<E2&>>>>);
            
         HandlePair_CheckState_OwnedFull<E1 const, E2 const>(handle);
         
         if constexpr (Sparse1) {
            auto entries = handle.template GetEntries<0>();
            REQUIRE(entries);
            REQUIRE(*entries == e556.entries[1]);
            if constexpr (Managed)
               REQUIRE(e556.entries[1]->GetUses() == 2);
         }

         if constexpr (Sparse2) {
            auto entries = handle.template GetEntries<1>();
            REQUIRE(entries);
            REQUIRE(*entries == e557.entries[1]);
            if constexpr (Managed)
               REQUIRE(e557.entries[1]->GetUses() == 2);
         }

         if constexpr (Reffed1) {
            auto& data = DenseCast(handle.template Get<E1, 0>());
            REQUIRE(data.GetReferences() == (Sparse1 ? 2 : 1));
            REQUIRE(data.destroyed == false);
         }

         if constexpr (Reffed2) {
            auto& data = DenseCast(handle.template Get<E2, 1>());
            REQUIRE(data.GetReferences() == (Sparse2 ? 2 : 1));
            REQUIRE(data.destroyed == false);
         }
      }
   }

   REQUIRE(memoryState.Assert());
   REQUIRE_FALSE(Allocator::CollectGarbage());
}
