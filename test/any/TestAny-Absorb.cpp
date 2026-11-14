///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "TestAnyCommon.hpp"
#include <Langulus/Many.hpp>


namespace Langulus::Annies  
{
   // Reuses definitions from TestAny-Empty.cpp. Reduces compile time.  
   extern template struct TAny<Text>;
   extern template struct TAny<int>;
   extern template struct TAny<Any>;
   extern template struct TAny<RT>;
   extern template struct TAny<char>;

   extern template struct TAny<Text*>;
   extern template struct TAny<int*>;
   extern template struct TAny<Any*>;
   extern template struct TAny<RT*>;
   extern template struct TAny<char*>;

   extern template struct TAny<Text**>;
   extern template struct TAny<int**>;
   extern template struct TAny<Any**>;
   extern template struct TAny<RT**>;
   extern template struct TAny<char**>;

   #if LANGULUS_FEATURE(MANAGED_MEMORY)
      extern template struct TAny<pptr8>;
      extern template struct TAny<pptr16>;
      extern template struct TAny<pptr32>;
   #endif
}


TEST_CASE_TEMPLATE("Test absorb-constructed Any/TAny", TestType
   // Elements are not allocated by the memory manager                  
   , Types<Any, ScopedElement<Text>>
   , Types<Any, ScopedElement<int>>
   , Types<Any, ScopedElement<Any>>
   , Types<Any, ScopedElement<RT>>
   , Types<Any, ScopedElement<char>>
                
   , Types<Any, ScopedElement<Text*>>
   , Types<Any, ScopedElement<int*>>
   , Types<Any, ScopedElement<Any*>>
   , Types<Any, ScopedElement<RT*>>
   , Types<Any, ScopedElement<char*>>

   , Types<Any, ScopedElement<Text**>>
   , Types<Any, ScopedElement<int**>>
   , Types<Any, ScopedElement<Any**>>
   , Types<Any, ScopedElement<RT**>>
   , Types<Any, ScopedElement<char**>>

   , Types<TAny<Text>,   ScopedElement<Text>>
   , Types<TAny<int>,    ScopedElement<int>>
   , Types<TAny<Any>,    ScopedElement<Any>>
   , Types<TAny<RT>,     ScopedElement<RT>>
   , Types<TAny<char>,   ScopedElement<char>>
   
   , Types<TAny<Text*>,  ScopedElement<Text*>>
   , Types<TAny<int*>,   ScopedElement<int*>>
   , Types<TAny<Any*>,   ScopedElement<Any*>>
   , Types<TAny<RT*>,    ScopedElement<RT*>>
   , Types<TAny<char*>,  ScopedElement<char*>>

   , Types<TAny<Text**>, ScopedElement<Text**>>
   , Types<TAny<int**>,  ScopedElement<int**>>
   , Types<TAny<Any**>,  ScopedElement<Any**>>
   , Types<TAny<RT**>,   ScopedElement<RT**>>
   , Types<TAny<char**>, ScopedElement<char**>>

   #if LANGULUS_FEATURE(MANAGED_MEMORY)
   // Elements are allocated by the memory manager                      
   , Types<Any, ScopedElement<Text, true>>
   , Types<Any, ScopedElement<int, true>>
   , Types<Any, ScopedElement<Any, true>>
   , Types<Any, ScopedElement<RT, true>>
   , Types<Any, ScopedElement<char, true>>
                
   , Types<Any, ScopedElement<Text*, true>>
   , Types<Any, ScopedElement<int*, true>>
   , Types<Any, ScopedElement<Any*, true>>
   , Types<Any, ScopedElement<RT*, true>>
   , Types<Any, ScopedElement<char*, true>>

   , Types<Any, ScopedElement<Text**, true>>
   , Types<Any, ScopedElement<int**, true>>
   , Types<Any, ScopedElement<Any**, true>>
   , Types<Any, ScopedElement<RT**, true>>
   , Types<Any, ScopedElement<char**, true>>

   , Types<TAny<Text>,   ScopedElement<Text, true>>
   , Types<TAny<int>,    ScopedElement<int, true>>
   , Types<TAny<Any>,    ScopedElement<Any, true>>
   , Types<TAny<RT>,     ScopedElement<RT, true>>
   , Types<TAny<char>,   ScopedElement<char, true>>
                         
   , Types<TAny<Text*>,  ScopedElement<Text*, true>>
   , Types<TAny<int*>,   ScopedElement<int*, true>>
   , Types<TAny<Any*>,   ScopedElement<Any*, true>>
   , Types<TAny<RT*>,    ScopedElement<RT*, true>>
   , Types<TAny<char*>,  ScopedElement<char*, true>>

   , Types<TAny<Text**>, ScopedElement<Text**, true>>
   , Types<TAny<int**>,  ScopedElement<int**, true>>
   , Types<TAny<Any**>,  ScopedElement<Any**, true>>
   , Types<TAny<RT**>,   ScopedElement<RT**, true>>
   , Types<TAny<char**>, ScopedElement<char**, true>>

   // Packed pointers                                                   
   , Types<Any, ScopedElementPacked<pptr8>>
   , Types<Any, ScopedElementPacked<pptr16>>
   , Types<Any, ScopedElementPacked<pptr32>>

   , Types<TAny<pptr8>,  ScopedElementPacked<pptr8>>
   , Types<TAny<pptr16>, ScopedElementPacked<pptr16>>
   , Types<TAny<pptr32>, ScopedElementPacked<pptr32>>
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
      using stdany = ::std::any;
   #endif

   auto testtype1 = MetaDataOf<E>();
   auto testtype2 = MetaDataOf<E const*>();
   auto testtype3 = MetaDataOf<Deptr<E> const*>();
   LglsAssumeDev(testtype1.IsExact(testtype1.GetDecvq()), testtype1.GetName(), " != ", testtype1.GetDecvq().GetName());
   REQUIRE(testtype1.IsExact(testtype1.GetDecvq()));
   REQUIRE(testtype2.IsExact(testtype2.GetDecvq()));
   REQUIRE(testtype3.IsExact(testtype3.GetDecvq()));
   
   GIVEN("Piecewise-constructed container, assigned (refer), and then destroyed") {
      const ScopedE element1{555};
      const ScopedE element2{111};
      T piecewise1{Piecewise, *element1};
      piecewise1.Assign(*element2);
   }

   GIVEN("Piecewise-constructed container, assigned (refer using intent), and then destroyed") {
      const ScopedE element1{555};
      const ScopedE element2{111};
      T piecewise1{Piecewise, *element1};
      piecewise1.Assign(Refer(*element2));
   }

   GIVEN("Piecewise-constructed container, assigned (copied), and then destroyed") {
      const ScopedE element1{555};
      const ScopedE element2{111};
      T piecewise1{Piecewise, *element1};
      piecewise1.Assign(Copy(*element2));
   }

   GIVEN("Piecewise-constructed container, assigned (cloned), and then destroyed") {
      const ScopedE element1{555};
      const ScopedE element2{111};
      T piecewise1{Piecewise, *element1};
      piecewise1.Assign(Clone(*element2));
   }

   GIVEN("Piecewise-constructed container, assigned (move), and then destroyed") {
      const ScopedE element1{555};
      ScopedE element2{111};
      T piecewise1{Piecewise, *element1};
      piecewise1.Assign(::std::move(*element2));
   }

   GIVEN("Piecewise-constructed container, assigned (move using intent), and then destroyed") {
      const ScopedE element1{555};
      ScopedE element2{112};
      T piecewise1{Piecewise, *element1};
      piecewise1.Assign(Move(*element2));
   }

   GIVEN("Piecewise-constructed container, assigned (abandon), and then destroyed") {
      const ScopedE element1{555};
      ScopedE element2{112};
      T piecewise1{Piecewise, *element1};
      piecewise1.Assign(Abandon(*element2));
   }

   GIVEN("Piecewise-constructed container, assigned (disown), and then destroyed") {
      const ScopedE element1{555};
      const ScopedE element2{111};
      T piecewise1{Piecewise, *element1};
      piecewise1.Assign(Disown(*element2));
   }

   GIVEN("Absorb-constructed container") {
      const ScopedE originalElement {556};
      const ScopedE element {555};

      if constexpr (Reffed) {
         REQUIRE(DenseCast(*originalElement).GetReferences() == 1);
      }

      T piecewise1{Piecewise, *originalElement};
      T piecewise2{Piecewise, *originalElement};
      T piecewise3{Piecewise, *originalElement};
      T piecewise4{Piecewise, *originalElement};

      if constexpr (Reffed) {
         REQUIRE(DenseCast(*originalElement).GetReferences() == (Sparse ? 5 : 1));
      }

      T pack_referred1{Absorb,             piecewise1};
      T pack_referred2{Absorb,       Refer(piecewise1)};

      if constexpr (Reffed) {
         REQUIRE(DenseCast(*originalElement).GetReferences() == (Sparse ? 7 : 1));
      }

      T pack_copied   {Absorb,        Copy(piecewise1)};

      if constexpr (Reffed) {
         REQUIRE(DenseCast(*originalElement).GetReferences() == (Sparse ? 8 : 1));
      }

      T pack_cloned   {Absorb,       Clone(piecewise1)};
      T pack_moved1   {Absorb, ::std::move(piecewise2)};
      T pack_moved2   {Absorb,        Move(piecewise3)};
      T pack_abandoned{Absorb,     Abandon(piecewise4)};
      T pack_disowned {Absorb,      Disown(piecewise1)};

      if constexpr (Reffed) {
         REQUIRE(DenseCast(*originalElement).GetReferences() == (Sparse ? 8 : 1));
      }

      WHEN("Absorb-constructed") {
         Any_CheckState_OwnedFull<E>(pack_referred1);
         Any_CheckState_OwnedFull<E>(pack_referred2);
         Any_CheckState_OwnedFull<E>(pack_copied);
         Any_CheckState_OwnedFull<E>(pack_cloned);
         Any_CheckState_OwnedFull<E>(pack_moved1);
         Any_CheckState_OwnedFull<E>(pack_moved2);
         Any_CheckState_OwnedFull<E>(pack_abandoned);
         Any_CheckState_DisownedFull<E>(pack_disowned);

         Any_CheckState_ContainsOne(pack_referred1,  Refer(originalElement), 3);
         Any_CheckState_ContainsOne(pack_referred2,  Refer(originalElement), 3);
         Any_CheckState_ContainsOne(pack_copied,     Refer(originalElement), 1);
         Any_CheckState_ContainsOne(pack_cloned,     Clone(originalElement), 1);
         Any_CheckState_ContainsOne(pack_moved1,     Refer(originalElement), 1);
         Any_CheckState_ContainsOne(pack_abandoned,  Refer(originalElement), 1);

         if constexpr (Managed) {
            // Entries are still propagated when absorbed
            Any_CheckState_ContainsOne(pack_disowned,  Refer(originalElement), 3);
         }
         else Any_CheckState_ContainsOne(pack_disowned,  Disown(originalElement), 3);

         if constexpr (Reffed) {
            REQUIRE(DenseCast(*originalElement).GetReferences() == (CT::Sparse<E> ? 8 : 1));
         }

         BenchmarkAnyStd("Empty/AbsorbConstructor", 30, 100,
            T temp,                                new (&temp) T(Absorb, piecewise1),
            stdany temp_std1 = *originalElement;
            stdany temp_std2,                      new (&temp_std2) stdany{temp_std1}
         );
      }

      WHEN("Assigned compatible referred value") {
         auto assign_refer = [&](T& a, [[maybe_unused]] const char* intent) {
            a.Assign(*element);

            if constexpr (CT::Container<E>)
               Any_CheckState_OwnedFull<TypeOf<E>>(*element);
            Any_CheckState_OwnedFull<E>(a);
            Any_CheckState_ContainsOne(a, Refer(element));

            BenchmarkAnyStd("Absorb/" + intent + "/Assign/Refer", 30, 100,
               a.Assign(*element),              a.Assign(*originalElement),
               stdany temp_std = *element,      temp_std = *originalElement
            );
         };

         assign_refer(pack_referred1, "Refer");
         assign_refer(pack_copied,    "Copy");
         assign_refer(pack_cloned,    "Clone");
         assign_refer(pack_moved1,    "Move");
         assign_refer(pack_abandoned, "Abandon");
         assign_refer(pack_disowned,  "Disown");
      }

      if constexpr (CT::ContainsOne<E>) {
         WHEN("Assigned and absorbed referred container") {
            if (not pack_referred1.IsSame(element->GetType())) {
               auto misabsorb_refer = [&](T& a, int uses) {
                  REQUIRE_THROWS(a.AssignAbsorb(*element));

                  Any_CheckState_ContainsOne(a, Refer(originalElement), uses);
               };

               misabsorb_refer(pack_referred1, 3);
               Any_CheckState_OwnedFull<E>(pack_referred1);

               misabsorb_refer(pack_referred2, 3);
               Any_CheckState_OwnedFull<E>(pack_referred2);

               misabsorb_refer(pack_copied,    1);
               Any_CheckState_OwnedFull<E>(pack_copied);

               misabsorb_refer(pack_cloned,    1);
               Any_CheckState_OwnedFull<E>(pack_cloned);

               misabsorb_refer(pack_moved1,    1);
               Any_CheckState_OwnedFull<E>(pack_moved1);

               misabsorb_refer(pack_moved2,    1);
               Any_CheckState_OwnedFull<E>(pack_moved2);

               misabsorb_refer(pack_abandoned, 1);
               Any_CheckState_OwnedFull<E>(pack_abandoned);

               misabsorb_refer(pack_disowned,  3);
               Any_CheckState_DisownedFull<E>(pack_disowned);
               return;
            }

            auto absorb_refer = [&](T& a, [[maybe_unused]] const char* intent) {
               a.AssignAbsorb(*element);

               Any_Helper_TestSame(a, *element);
               REQUIRE(a.GetUses() == element->GetUses());
               REQUIRE(a.GetUses() == 2);
               REQUIRE(a.GetAllocation() == element->GetAllocation());

               BenchmarkAnyStd("Absorb/" + intent + "/AssignAbsorb/Refer", 30, 100,
                  a.AssignAbsorb(*element),                 a.AssignAbsorb(*originalElement),
                  stdany temp_std1 = *element;
                  stdany temp_std2 = *originalElement,      temp_std1 = temp_std2
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

            if constexpr (CT::Container<E>)
               Any_CheckState_OwnedFull<TypeOf<E>>(*element);
            Any_CheckState_OwnedFull<E>(a);
            Any_CheckState_ContainsOne(a, Clone(element));

            BenchmarkAnyStd("Absorb/" + intent + "/Assign/Clone", 30, 100,
               a.Assign(Clone(*element)),          a.Assign(Clone(*originalElement)),
               stdany temp_std = *element,         temp_std = *originalElement
            );
         };

         assign_clone(pack_referred1, "Refer");
         assign_clone(pack_copied,    "Copy");
         assign_clone(pack_cloned,    "Clone");
         assign_clone(pack_moved1,    "Move");
         assign_clone(pack_abandoned, "Abandon");
         assign_clone(pack_disowned,  "Disown");
      }

      if constexpr (CT::ContainsOne<E>) {
         WHEN("Assigned and absorbed cloned container") {
            if (not pack_referred1.IsSame(element->GetType())) {
               auto misabsorb_clone = [&](T& a) {
                  REQUIRE_THROWS(a.AssignAbsorb(Clone(*element)));
                  Any_CheckState_OwnedFull<E>(a);
                  Any_CheckState_ContainsOne(a, Clone(originalElement));
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

               if constexpr (CT::Container<E>)
                  Any_CheckState_OwnedFull<TypeOf<E>>(*element);
               Any_Helper_TestSame(a, *element);
               REQUIRE(a.GetUses() == 2);
               REQUIRE(a.GetAllocation() == element->GetAllocation());

               BenchmarkAnyStd("Absorb/" + intent + "/AssignAbsorb/Clone", 30, 100,
                  a.AssignAbsorb(Clone(*element)),          a.AssignAbsorb(Clone(*originalElement)),
                  stdany temp_std1 = *element;
                  stdany temp_std2 = *originalElement,      temp_std1 = temp_std2
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

            if constexpr (CT::Container<E>)
               Any_CheckState_OwnedFull<TypeOf<E>>(*element);
            Any_CheckState_OwnedFull<E>(a);
            Any_CheckState_ContainsOne(a, Refer(element));

            BenchmarkAnyStd("Absorb/" + intent + "/Assign/Copy", 30, 100,
               a.Assign(Copy(*element)),        a.Assign(Copy(*originalElement)),
               stdany temp_std = *element,      temp_std = *originalElement
            );
         };

         assign_copy(pack_referred1, "Refer");
         assign_copy(pack_copied,    "Copy");
         assign_copy(pack_cloned,    "Clone");
         assign_copy(pack_moved1,    "Move");
         assign_copy(pack_abandoned, "Abandon");
         assign_copy(pack_disowned,  "Disown");
      }

      if constexpr (CT::ContainsOne<E>) {
         WHEN("Assigned and absorbed copied container") {
            if (not pack_referred1.IsSame(element->GetType())) {
               auto misabsorb_copy = [&](T& a) {
                  REQUIRE_THROWS(a.AssignAbsorb(Copy(*element)));
                  Any_CheckState_OwnedFull<E>(a);
                  Any_CheckState_ContainsOne(a, Refer(originalElement));
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

               if constexpr (CT::Container<E>)
                  Any_CheckState_OwnedFull<TypeOf<E>>(*element);
               Any_Helper_TestSame(a, *element);
               REQUIRE(a.GetUses() == 2);
               REQUIRE(a.GetAllocation() == element->GetAllocation());

               BenchmarkAnyStd("Absorb/" + intent + "/AssignAbsorb/Copy", 30, 100,
                  a.AssignAbsorb(Copy(*element)),           a.AssignAbsorb(Copy(*originalElement)),
                  stdany temp_std1 = *element;
                  stdany temp_std2 = *originalElement,      temp_std1 = temp_std2
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

            if constexpr (CT::Container<E>)
               Any_CheckState_Default<TypeOf<E>>(movable);
            Any_CheckState_OwnedFull<E>(a);
            Any_CheckState_ContainsOne(a, Refer(element));

            BenchmarkAnyStd("Absorb/" + intent + "/Assign/Move", 30, 100,
               auto movable1 = *element;
               auto movable2 = *originalElement;
               a.Assign(Move(movable1)),                   a.Assign(Move(movable2)),
               auto movable1 = *element;
               auto movable2 = *originalElement;
               stdany temp_std = ::std::move(movable1),    temp_std = ::std::move(movable2)
            );
         };

         assign_move(pack_referred1, "Refer");
         assign_move(pack_copied,    "Copy");
         assign_move(pack_cloned,    "Clone");
         assign_move(pack_moved1,    "Move");
         assign_move(pack_abandoned, "Abandon");
         assign_move(pack_disowned,  "Disown");
      }

      if constexpr (CT::ContainsOne<E>) {
         WHEN("Assigned and absorbed moved container") {
            if (not pack_referred1.IsSame(element->GetType())) {
               auto misabsorb_move = [&](T& a) {
                  auto movable = *element;
                  REQUIRE_THROWS(a.AssignAbsorb(::std::move(movable)));

                  Any_CheckState_OwnedFull<E>(a);
                  Any_CheckState_ContainsOne(a, Refer(originalElement));
                  Any_CheckState_OwnedFull<int>(movable);
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

               if constexpr (CT::Container<E>)
                  Any_CheckState_Default<TypeOf<E>>(movable);
               Any_Helper_TestSame(a, *element);
               REQUIRE(a.GetUses() == 2);
               REQUIRE(a.GetAllocation() == element->GetAllocation());

               BenchmarkAnyStd("Absorb/" + intent + "/AssignAbsorb/Move", 30, 100,
                  T movable1 = *element;
                  T movable2 = *originalElement;
                  a.AssignAbsorb(Move(movable1)),           a.AssignAbsorb(Move(movable2)),
                  stdany movable1 = *element;
                  stdany movable2 = *originalElement,       movable1 = ::std::move(movable2)
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

            Any_CheckState_OwnedFull<E>(a);
            Any_CheckState_ContainsOne(a, Disown(element));

            BenchmarkAnyStd("Absorb/" + intent + "/Assign/Disown", 30, 100,
               a.Assign(Disown(*element)),         a.Assign(Disown(*originalElement)),
               stdany temp_std = *element,         temp_std = *originalElement
            );
         };

         assign_disown(pack_referred1, "Refer");
         assign_disown(pack_copied,    "Copy");
         assign_disown(pack_cloned,    "Clone");
         assign_disown(pack_moved1,    "Move");
         assign_disown(pack_abandoned, "Abandon");
         assign_disown(pack_disowned,  "Disown");
      }

      if constexpr (CT::ContainsOne<E>) {
         WHEN("Assigned and absorbed disowned container") {
            if (not pack_referred1.IsSame(element->GetType())) {
               auto misabsorb_disown = [&](T& a) {
                  REQUIRE_THROWS(a.AssignAbsorb(Disown(*element)));
                  Any_CheckState_OwnedFull<E>(a);
                  Any_CheckState_ContainsOne(a, Disown(originalElement));
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

               BenchmarkAnyStd("Absorb/" + intent + "/AssignAbsorb/Disown", 30, 100,
                  a.AssignAbsorb(Disown(*element)),         a.AssignAbsorb(Disown(*originalElement)),
                  stdany temp_std1 = *element;
                  stdany temp_std2 = *originalElement,      temp_std1 = temp_std2
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

            if constexpr (CT::Container<E>)
               Any_CheckState_Abandoned<TypeOf<E>>(movable);
            Any_CheckState_OwnedFull<E>(a);
            Any_CheckState_ContainsOne(a, Refer(element));

            BenchmarkAnyStd("Absorb/" + intent + "/Assign/Abandon", 30, 100,
               auto movable1 = *element;
               auto movable2 = *originalElement;
               a.Assign(Abandon(movable1)),                 a.Assign(Abandon(movable2)),
               auto movable1 = *element;
               auto movable2 = *originalElement;
               stdany temp_std = ::std::move(movable1),     temp_std = ::std::move(movable2)
            );
         };

         assign_abandon(pack_referred1, "Refer");
         assign_abandon(pack_copied,    "Copy");
         assign_abandon(pack_cloned,    "Clone");
         assign_abandon(pack_moved1,    "Move");
         assign_abandon(pack_abandoned, "Abandon");
         assign_abandon(pack_disowned,  "Disown");
      }

      if constexpr (CT::ContainsOne<E>) {
         WHEN("Assigned and absorbed abandoned container") {
            if (not pack_referred1.IsSame(element->GetType())) {
               auto misabsorb_abandon = [&](T& a) {
                  auto movable = *element;
                  REQUIRE_THROWS(a.AssignAbsorb(Abandon(movable)));

                  Any_CheckState_OwnedFull<E>(a);
                  Any_CheckState_ContainsOne(a, Refer(originalElement));
                  Any_CheckState_OwnedFull<int>(movable);
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

               if constexpr (CT::Container<E>)
                  Any_CheckState_Abandoned<TypeOf<E>>(movable);
               Any_Helper_TestSame(a, *element);
               REQUIRE(a.GetUses() == 2);
               REQUIRE(a.GetAllocation() == element->GetAllocation());

               BenchmarkAnyStd("Absorb/" + intent + "/AssignAbsorb/Abandon", 30, 100,
                  T movable1 = *element;
                  T movable2 = *originalElement;
                  a.AssignAbsorb(Abandon(movable1)),           a.AssignAbsorb(Abandon(movable2)),
                  stdany movable1 = *element;
                  stdany movable2 = *originalElement;
                  stdany temp_std = ::std::move(movable1),     temp_std = ::std::move(movable2)
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
            Any_CheckState_Default<E>(a);
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
            T backup = a;
            const auto uses_before = a.GetUses();
            LglsDisableWarningPush
            LglsDisableWarning_SelfAssign
               a = a;
            LglsDisableWarningPop
            Any_Helper_TestSame(a, backup, not allow_change_in_constness);
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

            Any_Helper_TestSame(absorbed1, compare_against);
            Any_Helper_TestSame(absorbed2, compare_against);
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
         absorb_construct_refer(pack_disowned,  pack_referred1, 5); // referencing a disowned container basically reverses disownment, on managed memory builds at least
      }
      
      WHEN("Absorbed by move") {
         auto absorb_construct_move = [&](T& a, int uses) {
            T backup = a;
            T absorbed {::std::move(a)};

            Any_CheckState_Default<E>(a);
            Any_CheckState_OwnedFull<E>(absorbed);
            Any_Helper_TestSame(absorbed, backup);
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

            Any_CheckState_Default<E>(a);
            Any_CheckState_OwnedFull<E>(absorbed);
            Any_Helper_TestSame(absorbed, backup);
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

            Any_CheckState_Abandoned<E>(a);
            Any_CheckState_OwnedFull<E>(absorbed);
            Any_Helper_TestSame(absorbed, backup);
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

            Any_CheckState_OwnedFull<E>(a);
            Any_CheckState_DisownedFull<E>(absorbed);
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
         Any_CheckState_DisownedFull<E>(pack_disowned);
         Any_CheckState_DisownedFull<E>(absorbed);
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
         auto absorb_construct_copy = [&](T& a, int uses, int entry_refs) {
            T absorbed {Copy {a}};

            REQUIRE(a.GetUses() == uses);
            Any_CheckState_OwnedFull<E>(absorbed);
            REQUIRE(absorbed.GetUses() == 1);
            REQUIRE(absorbed == a);
            REQUIRE(absorbed.GetRaw() != a.GetRaw());
            REQUIRE(absorbed.template As<E>() == a.template As<E>());

            if constexpr (Sparse) {
               auto entry = *absorbed.GetEntries();
               if (entry_refs == 0)
                  REQUIRE(entry == nullptr);

               if (entry) {
                  REQUIRE(entry->GetUses() == entry_refs);
                  if constexpr (Reffed) {
                     auto e = absorbed.template As<E>();
                     REQUIRE(DenseCast(e).GetReferences() == entry_refs);
                  }
               }
               else {
                  if constexpr (Reffed) {
                     auto e = absorbed.template As<E>();
                     REQUIRE(DenseCast(e).GetReferences() == 9);
                  }
               }
            }
         };

         absorb_construct_copy(pack_referred1, 3, managed_sparse ? 9 : 3);
         Any_CheckState_OwnedFull<E>(pack_referred1);

         absorb_construct_copy(pack_referred2, 3, managed_sparse ? 9 : 3);
         Any_CheckState_OwnedFull<E>(pack_referred2);

         absorb_construct_copy(pack_copied,    1, managed_sparse ? 9 : 3);
         Any_CheckState_OwnedFull<E>(pack_copied);

         absorb_construct_copy(pack_cloned,    1, 2);
         Any_CheckState_OwnedFull<E>(pack_cloned);

         absorb_construct_copy(pack_moved1,    1, managed_sparse ? 9 : 1);
         Any_CheckState_OwnedFull<E>(pack_moved1);

         absorb_construct_copy(pack_moved2,    1, managed_sparse ? 9 : 1);
         Any_CheckState_OwnedFull<E>(pack_moved2);

         absorb_construct_copy(pack_abandoned, 1, managed_sparse ? 9 : 1);
         Any_CheckState_OwnedFull<E>(pack_abandoned);

         absorb_construct_copy(pack_disowned,  3, managed_sparse ? 9 : 0);
         Any_CheckState_DisownedFull<E>(pack_disowned);
      }
      
      WHEN("Absorbed by clone") {
         auto absorb_construct_clone = [&](T& a, int uses) {
            T absorbed {Clone {a}};

            if (uses == 0)
               Any_CheckState_DisownedFull<E>(a);
            else
               Any_CheckState_OwnedFull<E>(a);

            Any_CheckState_OwnedFull<E>(absorbed);
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
      
      /// MARK: Emplace                                                       
      WHEN("Emplace (overwrite)") {
         auto emplace_overwrite = [&](T& a, [[maybe_unused]] const char* intent) {
            ScopedE i666{666};
            const auto i666backup = *i666;
            decltype(auto) instance = a.Emplace(::std::move(*i666));

            Any_CheckState_OwnedFull<E>(a);
            if constexpr (CT::Handle<decltype(instance)>)
               REQUIRE(instance.CompareOneEqual(i666backup));
            else
               REQUIRE(instance == i666backup);
            REQUIRE(a.GetCount() == 1);
            REQUIRE(a.GetReserved() >= 1);

            if constexpr (not CT::TypeErased<T>) {
               REQUIRE(*a == i666backup);
               if constexpr (CT::Handle<decltype(instance)>)
                  REQUIRE(&*a == &*instance);
               else
                  REQUIRE(&*a == &instance);
            }

            BenchmarkAny("Absorb/" + intent + "/Emplace", 30,
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

      /// MARK: Emplace (describe)                                            
      WHEN("Emplace (overwrite, describe)") {
         auto emplace_overwrite_describe = [&](T& a, [[maybe_unused]] const char* intent) {
            ScopedE i666{666};
            const auto i666backup = *i666;
            Many descriptor {Piecewise, ::std::move(*i666)};

            if constexpr (CT::DescribeConstructible<E> and not CT::Container<T>) {
               decltype(auto) instance = a.Emplace(Describe{descriptor});

               Any_CheckState_OwnedFull<E>(a);
               REQUIRE(instance.CompareOneEqual(i666backup));
               REQUIRE(a.GetCount() == 1);
               REQUIRE(a.GetReserved() >= 1);

               BenchmarkAny("Absorb/" + intent + "/Emplace/Describe", 30,
                  auto movable1 = *element;
                  a.Emplace(::std::move(movable1)),      a.Emplace(Describe{descriptor})
               );
            }
            else if constexpr (CT::TypeErased<T>) {
               REQUIRE_THROWS(a.Emplace(Describe{descriptor}));

               Any_CheckState_Default<E>(a, true);
            }
         };

         emplace_overwrite_describe(pack_referred1, "Refer");
         emplace_overwrite_describe(pack_copied,    "Copy");
         emplace_overwrite_describe(pack_cloned,    "Clone");
         emplace_overwrite_describe(pack_moved1,    "Move");
         emplace_overwrite_describe(pack_abandoned, "Abandon");
         emplace_overwrite_describe(pack_disowned,  "Disown");
      }
      
      /// MARK: Clear                                                         
      WHEN("Cleared") {
         auto clear_full = [&](T& a, [[maybe_unused]] const char* intent) {
            BenchmarkAnyStd("Absorb/" + intent + "/Clear", 30, 100,
               T temp = a,                      temp.Clear(),
               stdany temp_std = *element,      temp_std.reset()
            );

            a.Clear();
         };

         clear_full(pack_referred1, "Refer");
         Any_CheckState_Default<E>(pack_referred1, true);

         clear_full(pack_copied,    "Copy");
         Any_CheckState_OwnedEmpty<E>(pack_copied);

         clear_full(pack_cloned,    "Clone");
         Any_CheckState_OwnedEmpty<E>(pack_cloned);

         clear_full(pack_moved1,    "Move");
         Any_CheckState_OwnedEmpty<E>(pack_moved1);

         clear_full(pack_abandoned, "Abandon");
         Any_CheckState_OwnedEmpty<E>(pack_abandoned);

         clear_full(pack_disowned,  "Disown");
         Any_CheckState_Default<E>(pack_disowned, true);
      }

      /// MARK: Reset                                                         
      WHEN("Reset") {
         auto reset_full = [&](T& a, [[maybe_unused]] const char* intent) {
            BenchmarkAnyStd("Absorb/" + intent + "/Reset", 30, 100,
               T temp = a,                      temp.Reset(),
               stdany temp_std = *element,      temp_std.reset()
            );

            a.Reset();

            Any_CheckState_Default<E>(a);
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
            BenchmarkAnyStd("Absorb/" + intent + "/operator==", 30, 100,
               (void) 0,                                dont_optimize |= (a == same_pack),
               const stdany a_std = *element;
               const stdany another_pack1_std = *e1,    dont_optimize |= (std::any_cast<E const&>(a_std) == std::any_cast<E const&>(another_pack1_std))
            );
            BenchmarkAnyStd("Absorb/" + intent + "/operator!=", 30, 100,
               (void) 0,                                dont_optimize |= (a != same_pack),
               const stdany a_std = *element;
               const stdany another_pack1_std = *e1,    dont_optimize |= (std::any_cast<E const&>(a_std) != std::any_cast<E const&>(another_pack1_std))
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
         
         auto contains_full = [&](T& a) {
            REQUIRE      (a.Contains(*originalElement));
            REQUIRE_FALSE(a.Contains(*e1));
         };

         contains_full(pack_referred1);
         contains_full(pack_referred2);
         contains_full(pack_copied);

         if constexpr (Sparse) {
            REQUIRE      (pack_cloned.GetDense().Contains(DenseCast(*originalElement)));
            REQUIRE_FALSE(pack_cloned.Contains(*originalElement));
            REQUIRE_FALSE(pack_cloned.Contains(*e1));
         }
         else contains_full(pack_cloned);

         contains_full(pack_moved1);
         contains_full(pack_moved2);
         contains_full(pack_abandoned);
         contains_full(pack_disowned);

         [[maybe_unused]] volatile bool dont_optimize = false;
         BenchmarkAny("Absorb/Contains", 30,
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
         auto src_handle = src.GetHandle();

         #if not LANGULUS(FORCE_TYPE_ERASURE)
            if constexpr (CT::TypeErased<T>)
               static_assert(::std::same_as<decltype(src_handle), HandleMut>);
            else
               static_assert(::std::same_as<decltype(src_handle), THandle<E&>>);
         #else
            static_assert(::std::same_as<decltype(src_handle), HandleMut>);
         #endif

         auto src_data   = src_handle.template Get<E>();
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

         auto dst_handle = dst.GetHandle();
         auto dst_data   = dst_handle.template Get<E>();
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

         #if not LANGULUS(FORCE_TYPE_ERASURE)
            if constexpr (CT::TypeErased<T>)
               static_assert(::std::same_as<decltype(handle), Handle>);
            else
               static_assert(::std::same_as<decltype(handle), THandle<ConstAll<E&>>>);
         #else
            static_assert(::std::same_as<decltype(handle), Handle>);
         #endif

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
