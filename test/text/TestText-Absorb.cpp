///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "TestTextCommon.hpp"
#include "../handle/TestHandleCommon.hpp"


TEST_CASE_TEMPLATE("Test absorb-constructed Text", TestType
   // Elements are not allocated by the memory manager                  
   , Types<Text, ScopedElement<Text>>
   , Types<Text, ScopedElement<int>>
   , Types<Text, ScopedElement<Many>>
   , Types<Text, ScopedElement<RT>>
   , Types<Text, ScopedElement<char>>

   , Types<Text, ScopedElement<Text*>>
   , Types<Text, ScopedElement<int*>>
   , Types<Text, ScopedElement<Many*>>
   , Types<Text, ScopedElement<RT*>>
   , Types<Text, ScopedElement<char*>>

   , Types<Text, ScopedElement<Text**>>
   , Types<Text, ScopedElement<int**>>
   , Types<Text, ScopedElement<Many**>>
   , Types<Text, ScopedElement<RT**>>
   , Types<Text, ScopedElement<char**>>

   #if LANGULUS_FEATURE(MANAGED_MEMORY)
   // Elements are allocated by the memory manager                      
   , Types<Text, ScopedElement<Text,   true>>
   , Types<Text, ScopedElement<int,    true>>
   , Types<Text, ScopedElement<Many,   true>>
   , Types<Text, ScopedElement<RT,     true>>
   , Types<Text, ScopedElement<char,   true>>

   , Types<Text, ScopedElement<Text*,  true>>
   , Types<Text, ScopedElement<int*,   true>>
   , Types<Text, ScopedElement<Many*,  true>>
   , Types<Text, ScopedElement<RT*,    true>>
   , Types<Text, ScopedElement<char*,  true>>

   , Types<Text, ScopedElement<Text**, true>>
   , Types<Text, ScopedElement<int**,  true>>
   , Types<Text, ScopedElement<Many**, true>>
   , Types<Text, ScopedElement<RT**,   true>>
   , Types<Text, ScopedElement<char**, true>>

   // Packed pointers                                                   
   , Types<Text, ScopedElementPacked<pptr8>>
   , Types<Text, ScopedElementPacked<pptr16>>
   , Types<Text, ScopedElementPacked<pptr32>>
   #endif
) {
   static MemoryState memoryState;
   using T        = typename TestType::First;
   using ScopedE  = typename TestType::Second;
   using E        = TypeOf<ScopedE>;

   [[maybe_unused]] constexpr bool Managed = ScopedE::Managed;

   #if LANGULUS(BENCHMARK)
      using stdstr = ::std::string;
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
         Text_CheckState_OwnedFull(pack_referred1);
         Text_CheckState_OwnedFull(pack_referred2);
         Text_CheckState_OwnedFull(pack_copied);
         Text_CheckState_OwnedFull(pack_cloned);
         Text_CheckState_OwnedFull(pack_moved1);
         Text_CheckState_OwnedFull(pack_moved2);
         Text_CheckState_OwnedFull(pack_abandoned);
         Text_CheckState_DisownedFull(pack_disowned);

         Text_CheckState_ContainsOne(pack_referred1, *originalElement, 3);
         Text_CheckState_ContainsOne(pack_referred2, *originalElement, 3);
         Text_CheckState_ContainsOne(pack_copied,    *originalElement, 1);
         Text_CheckState_ContainsOne(pack_cloned,    *originalElement, 1);
         Text_CheckState_ContainsOne(pack_moved1,    *originalElement, 1);
         Text_CheckState_ContainsOne(pack_abandoned, *originalElement, 1);
         Text_CheckState_ContainsOne(pack_disowned,  *originalElement, 3);

         BenchmarkTextStd("Empty/AbsorbConstructor", 30, 100,
            T temp,                                   (new (&temp) T{Absorb, piecewise1}),
            stdstr temp_std1 (1, *originalElement);
            stdstr temp_std2,                         new (&temp_std2) stdstr {temp_std1}
         );
      }

      /// MARK: Assign/Refer                                                  
      WHEN("Assigned value by referral") {
         auto assign_refer = [&](auto& a, [[maybe_unused]] const char* intent) {
            REQUIRE_NOTHROW(a.Assign(*element));

            if constexpr (CT::DeepDense<E>)
               Many_CheckState_OwnedFull<TypeOf<E>>(*element);

            Text_CheckState_OwnedFull(a);
            Text_CheckState_ContainsOne(a, *element);

            BenchmarkTextStd("Absorb/" + intent + "/Assign/Refer", 30, 100,
               a.Assign(*element),                 a.Assign(*originalElement),
               stdstr temp_std (1, *element),      temp_std[0] = *originalElement
            );
         };

         assign_refer(pack_referred1, "Refer");
         assign_refer(pack_copied,    "Copy");
         assign_refer(pack_cloned,    "Clone");
         assign_refer(pack_moved1,    "Move");
         assign_refer(pack_abandoned, "Abandon");
         assign_refer(pack_disowned,  "Disown");
      }

      if constexpr (CT::DeepDense<E>) {
         WHEN("Assigned and misabsorbed by referral") {
            auto misabsorb_refer = [&](auto& a, int uses) {
               REQUIRE_THROWS(a.AssignAbsorb(*element));

               Text_CheckState_ContainsOne(a, *originalElement, uses);
            };

            misabsorb_refer(pack_referred1, 3);
            Text_CheckState_OwnedFull(pack_referred1);

            misabsorb_refer(pack_referred2, 3);
            Text_CheckState_OwnedFull(pack_referred2);

            misabsorb_refer(pack_copied,    1);
            Text_CheckState_OwnedFull(pack_copied);

            misabsorb_refer(pack_cloned,    1);
            Text_CheckState_OwnedFull(pack_cloned);

            misabsorb_refer(pack_moved1,    1);
            Text_CheckState_OwnedFull(pack_moved1);

            misabsorb_refer(pack_moved2,    1);
            Text_CheckState_OwnedFull(pack_moved2);

            misabsorb_refer(pack_abandoned, 1);
            Text_CheckState_OwnedFull(pack_abandoned);

            misabsorb_refer(pack_disowned,  3);
            Text_CheckState_DisownedFull(pack_disowned);
         }
      }

      if constexpr (CT::Container<E> and CT::Text<E>) {
         WHEN("Assigned and absorbed by referral") {
            auto absorb_refer = [&](auto& a, [[maybe_unused]] const char* intent, int uses) {
               REQUIRE_NOTHROW(a.AssignAbsorb(*element));

               Text_CheckState_OwnedFull(a);
               Text_CheckState_OwnedFull(*element);
               Text_Helper_TestSame(a, *element);
               REQUIRE(a.GetUses() == element->GetUses());
               REQUIRE(a.GetUses() == uses);
               REQUIRE(a.GetAllocation() == element->GetAllocation());

               BenchmarkTextStd("Absorb/" + intent + "/AssignAbsorb/Refer", 30, 100,
                  a.AssignAbsorb(*element),                 a.AssignAbsorb(*originalElement),
                  stdstr temp_std1 (1, *element);
                  stdstr temp_std2 (1, *originalElement),   temp_std1 = temp_std2
               );
            };

            absorb_refer(pack_referred1, "Refer",   2);
            absorb_refer(pack_copied,    "Copy",    3);
            absorb_refer(pack_cloned,    "Clone",   4);
            absorb_refer(pack_moved1,    "Move",    5);
            absorb_refer(pack_abandoned, "Abandon", 6);
            absorb_refer(pack_disowned,  "Disown",  7);
         }
      }
      
      /// MARK: Assign/Move                                                   
      WHEN("Assigned value by move") {
         auto assign_move = [&](T& a, [[maybe_unused]] const char* intent) {
            auto movable = *element;
            if constexpr (Same<E, RT>)
               movable.copied_in = false;

            REQUIRE_NOTHROW(a.Assign(::std::move(movable)));

            if constexpr (CT::DeepDense<E>) {
               Many_CheckState_OwnedFull<TypeOf<E>>(movable);
               Many_Helper_TestSame(movable, *element);
            }

            Text_CheckState_OwnedFull(a);
            Text_CheckState_ContainsOne(a, *element);

            BenchmarkTextStd("Absorb/" + intent + "/Assign/Move", 30, 100,
               auto movable1 = *element;
               auto movable2 = *originalElement;
               a.Assign(Move(movable1)),                       a.Assign(Move(movable2)),
               auto movable1 = *element;
               auto movable2 = *originalElement;
               stdstr temp_std (1, ::std::move(movable1)),     temp_std[0] = ::std::move(movable2)
            );
         };

         assign_move(pack_referred1, "Refer");
         assign_move(pack_copied,    "Copy");
         assign_move(pack_cloned,    "Clone");
         assign_move(pack_moved1,    "Move");
         assign_move(pack_abandoned, "Abandon");
         assign_move(pack_disowned,  "Disown");
      }

      if constexpr (CT::DeepDense<E>) {
         WHEN("Assigned and misabsorbed by move") {
            auto misabsorb_move = [&](T& a, int uses) {
               auto movable = *element;
               REQUIRE_THROWS(a.AssignAbsorb(::std::move(movable)));

               Text_CheckState_ContainsOne(a, *originalElement, uses);
               Many_CheckState_OwnedFull<TypeOf<E>>(movable);
               Many_Helper_TestSame(movable, *element);
            };

            misabsorb_move(pack_referred1, 3);
            Text_CheckState_OwnedFull(pack_referred1);

            misabsorb_move(pack_referred2, 3);
            Text_CheckState_OwnedFull(pack_referred2);

            misabsorb_move(pack_copied,    1);
            Text_CheckState_OwnedFull(pack_copied);

            misabsorb_move(pack_cloned,    1);
            Text_CheckState_OwnedFull(pack_cloned);

            misabsorb_move(pack_moved1,    1);
            Text_CheckState_OwnedFull(pack_moved1);

            misabsorb_move(pack_moved2,    1);
            Text_CheckState_OwnedFull(pack_moved2);

            misabsorb_move(pack_abandoned, 1);
            Text_CheckState_OwnedFull(pack_abandoned);

            misabsorb_move(pack_disowned,  3);
            Text_CheckState_DisownedFull(pack_disowned);
         }
      }

      if constexpr (CT::Container<E> and CT::Text<E>) {
         WHEN("Assigned and absorbed by move") {
            auto absorb_move = [&](T& a, [[maybe_unused]] const char* intent, int uses) {
               auto movable = *element;
               REQUIRE_NOTHROW(a.AssignAbsorb(::std::move(movable)));

               Text_CheckState_OwnedFull(a);
               Text_CheckState_Default(movable);
               Text_Helper_TestSame(a, *element);
               REQUIRE(a.GetUses() == uses);
               REQUIRE(a.GetAllocation() == element->GetAllocation());

               BenchmarkTextStd("Absorb/" + intent + "/AssignAbsorb/Move", 30, 100,
                  T movable1 = *element;
                  T movable2 = *originalElement;
                  a.AssignAbsorb(Move(movable1)),          a.AssignAbsorb(Move(movable2)),
                  stdstr movable1 (1, *element);
                  stdstr movable2 (1, *originalElement),   movable1 = ::std::move(movable2)
               );
            };

            absorb_move(pack_referred1, "Refer",   2);
            absorb_move(pack_copied,    "Copy",    3);
            absorb_move(pack_cloned,    "Clone",   4);
            absorb_move(pack_moved1,    "Move",    5);
            absorb_move(pack_abandoned, "Abandon", 6);
            absorb_move(pack_disowned,  "Disown",  7);
         }
      }
      
      /// MARK: Assign/Copy                                                   
      WHEN("Assigned value by copy") {
         auto assign_copy = [&](T& a, [[maybe_unused]] const char* intent) {
            REQUIRE_NOTHROW(a.Assign(Copy(*element)));

            if constexpr (CT::DeepDense<E>)
               Many_CheckState_OwnedFull<TypeOf<E>>(*element);
            Text_CheckState_OwnedFull(a);
            Text_CheckState_ContainsOne(a, *element);

            BenchmarkTextStd("Absorb/" + intent + "/Assign/Copy", 30, 100,
               a.Assign(Copy(*element)),           a.Assign(Copy(*originalElement)),
               stdstr temp_std (1, *element),      temp_std[0] = *originalElement
            );
         };

         assign_copy(pack_referred1, "Refer");
         assign_copy(pack_copied,    "Copy");
         assign_copy(pack_cloned,    "Clone");
         assign_copy(pack_moved1,    "Move");
         assign_copy(pack_abandoned, "Abandon");
         assign_copy(pack_disowned,  "Disown");
      }

      if constexpr (CT::DeepDense<E>) {
         WHEN("Assigned and misabsorbed by copy") {
            auto misabsorb_copy = [&](T& a, int uses) {
               REQUIRE_THROWS(a.AssignAbsorb(Copy(*element)));
               Text_CheckState_ContainsOne(a, *originalElement, uses);
            };

            misabsorb_copy(pack_referred1, 3);
            Text_CheckState_OwnedFull(pack_referred1);

            misabsorb_copy(pack_referred2, 3);
            Text_CheckState_OwnedFull(pack_referred2);

            misabsorb_copy(pack_copied,    1);
            Text_CheckState_OwnedFull(pack_copied);

            misabsorb_copy(pack_cloned,    1);
            Text_CheckState_OwnedFull(pack_cloned);

            misabsorb_copy(pack_moved1,    1);
            Text_CheckState_OwnedFull(pack_moved1);

            misabsorb_copy(pack_moved2,    1);
            Text_CheckState_OwnedFull(pack_moved2);

            misabsorb_copy(pack_abandoned, 1);
            Text_CheckState_OwnedFull(pack_abandoned);

            misabsorb_copy(pack_disowned,  3);
            Text_CheckState_DisownedFull(pack_disowned);
         }
      }

      if constexpr (CT::Container<E> and CT::Text<E>) {
         WHEN("Assigned and absorbed by copy") {
            auto absorb_copy = [&](T& a, [[maybe_unused]] const char* intent) {
               REQUIRE_NOTHROW(a.AssignAbsorb(Copy(*element)));

               Text_CheckState_OwnedFull(a);
               Text_CheckState_OwnedFull(*element);
               Text_CheckState_ContainsString(a, "555");
               Text_CheckState_ContainsString(*element, "555");
               REQUIRE(a.GetUses() == 1);
               REQUIRE(a.GetAllocation() != element->GetAllocation());

               BenchmarkTextStd("Absorb/" + intent + "/AssignAbsorb/Copy", 30, 100,
                  a.AssignAbsorb(Copy(*element)),           a.AssignAbsorb(Copy(*originalElement)),
                  stdstr temp_std1 (1, *element);
                  stdstr temp_std2 (1, *originalElement),   temp_std1 = temp_std2
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

      /// MARK: Assign/Clone                                                  
      WHEN("Assigned value by clone") {
         auto assign_clone = [&](T& a, [[maybe_unused]] const char* intent) {
            REQUIRE_NOTHROW(a.Assign(Clone(*element)));

            if constexpr (CT::DeepDense<E>)
               Many_CheckState_OwnedFull<TypeOf<E>>(*element);
            Text_CheckState_OwnedFull(a);
            Text_CheckState_ContainsOne(a, *element);

            BenchmarkTextStd("Absorb/" + intent + "/Assign/Clone", 30, 100,
               a.Assign(Clone(*element)),          a.Assign(Clone(*originalElement)),
               stdstr temp_std (1, *element),      temp_std[0] = *originalElement
            );
         };

         assign_clone(pack_referred1, "Refer");
         assign_clone(pack_copied,    "Copy");
         assign_clone(pack_cloned,    "Clone");
         assign_clone(pack_moved1,    "Move");
         assign_clone(pack_abandoned, "Abandon");
         assign_clone(pack_disowned,  "Disown");
      }

      if constexpr (CT::DeepDense<E>) {
         WHEN("Assigned and misabsorbed by clone") {
            auto misabsorb_clone = [&](T& a, int uses) {
               REQUIRE_THROWS(a.AssignAbsorb(Clone(*element)));
               Text_CheckState_ContainsOne(a, *originalElement, uses);
            };

            misabsorb_clone(pack_referred1, 3);
            Text_CheckState_OwnedFull(pack_referred1);

            misabsorb_clone(pack_referred2, 3);
            Text_CheckState_OwnedFull(pack_referred2);

            misabsorb_clone(pack_copied,    1);
            Text_CheckState_OwnedFull(pack_copied);

            misabsorb_clone(pack_cloned,    1);
            Text_CheckState_OwnedFull(pack_cloned);

            misabsorb_clone(pack_moved1,    1);
            Text_CheckState_OwnedFull(pack_moved1);

            misabsorb_clone(pack_moved2,    1);
            Text_CheckState_OwnedFull(pack_moved2);

            misabsorb_clone(pack_abandoned, 1);
            Text_CheckState_OwnedFull(pack_abandoned);

            misabsorb_clone(pack_disowned,  3);
            Text_CheckState_DisownedFull(pack_disowned);
         }
      }

      if constexpr (CT::Container<E> and CT::Text<E>) {
         WHEN("Assigned and absorbed by clone") {
            auto absorb_clone = [&](T& a, [[maybe_unused]] const char* intent) {
               REQUIRE_NOTHROW(a.AssignAbsorb(Clone(*element)));

               Text_CheckState_OwnedFull(a);
               Text_CheckState_OwnedFull(*element);
               Text_CheckState_ContainsString(a, "555");
               Text_CheckState_ContainsString(*element, "555");
               REQUIRE(a.GetUses() == 1);
               REQUIRE(a.GetAllocation() != element->GetAllocation());

               BenchmarkTextStd("Absorb/" + intent + "/AssignAbsorb/Clone", 30, 100,
                  a.AssignAbsorb(Clone(*element)),          a.AssignAbsorb(Clone(*originalElement)),
                  stdstr temp_std1 (1, *element);
                  stdstr temp_std2 (1, *originalElement),   temp_std1 = temp_std2
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

      /// MARK: Assign/Disown                                                 
      WHEN("Assigned value by disown") {
         auto assign_disown = [&](T& a, [[maybe_unused]] const char* intent) {
            REQUIRE_NOTHROW(a.Assign(Disown(*element)));

            Text_CheckState_OwnedFull(a);
            Text_CheckState_ContainsOne(a, *element);

            BenchmarkTextStd("Absorb/" + intent + "/Assign/Disown", 30, 100,
               a.Assign(Disown(*element)),      a.Assign(Disown(*originalElement)),
               stdstr temp_std (1, *element),   temp_std[0] = *originalElement
            );
         };

         assign_disown(pack_referred1, "Refer");
         assign_disown(pack_copied,    "Copy");
         assign_disown(pack_cloned,    "Clone");
         assign_disown(pack_moved1,    "Move");
         assign_disown(pack_abandoned, "Abandon");
         assign_disown(pack_disowned,  "Disown");
      }

      if constexpr (CT::DeepDense<E>) {
         WHEN("Assigned and misabsorbed by disown") {
            auto misabsorb_disown = [&](T& a, int uses) {
               REQUIRE_THROWS(a.AssignAbsorb(Disown(*element)));
               
               Text_CheckState_ContainsOne(a, *originalElement, uses);
            };

            misabsorb_disown(pack_referred1, 3);
            Text_CheckState_OwnedFull(pack_referred1);

            misabsorb_disown(pack_referred2, 3);
            Text_CheckState_OwnedFull(pack_referred2);

            misabsorb_disown(pack_copied,    1);
            Text_CheckState_OwnedFull(pack_copied);

            misabsorb_disown(pack_cloned,    1);
            Text_CheckState_OwnedFull(pack_cloned);

            misabsorb_disown(pack_moved1,    1);
            Text_CheckState_OwnedFull(pack_moved1);

            misabsorb_disown(pack_moved2,    1);
            Text_CheckState_OwnedFull(pack_moved2);

            misabsorb_disown(pack_abandoned, 1);
            Text_CheckState_OwnedFull(pack_abandoned);

            misabsorb_disown(pack_disowned,  3);
            Text_CheckState_DisownedFull(pack_disowned);
         }
      }

      if constexpr (CT::Container<E> and CT::Text<E>) {
         WHEN("Assigned and absorbed by disown") {
            auto absorb_disown = [&](T& a, [[maybe_unused]] const char* intent) {
               REQUIRE_NOTHROW(a.AssignAbsorb(Disown(*element)));

               Text_CheckState_DisownedFull(a);
               Text_CheckState_OwnedFull(*element);
               Text_Helper_TestSame(a, *element, false);
               REQUIRE(a.GetUses() == 1);
               REQUIRE(a.GetAllocation() == element->GetAllocation());

               BenchmarkTextStd("Absorb/" + intent + "/AssignAbsorb/Disown", 30, 100,
                  a.AssignAbsorb(Disown(*element)),         a.AssignAbsorb(Disown(*originalElement)),
                  stdstr temp_std1 (1, *element);
                  stdstr temp_std2 (1, *originalElement),   temp_std1 = temp_std2
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
      
      /// MARK: Assign/Abandon                                                
      WHEN("Assigned value by abandon") {
         auto assign_abandon = [&](T& a, [[maybe_unused]] const char* intent) {
            auto movable = *element;
            if constexpr (Same<E, RT>)
               movable.copied_in = false;

            REQUIRE_NOTHROW(a.Assign(Abandon(movable)));

            if constexpr (CT::DeepDense<E>) {
               Many_CheckState_OwnedFull<TypeOf<E>>(movable);
               Many_Helper_TestSame(movable, *element);
            }
            
            Text_CheckState_OwnedFull(a);
            Text_CheckState_ContainsOne(a, *element);

            BenchmarkTextStd("Absorb/" + intent + "/Assign/Abandon", 30, 100,
               auto movable1 = *element;
               auto movable2 = *originalElement;
               a.Assign(Abandon(movable1)),                   a.Assign(Abandon(movable2)),
               auto movable1 = *element;
               auto movable2 = *originalElement;
               stdstr temp_std (1, ::std::move(movable1)),    temp_std[0] = ::std::move(movable2)
            );
         };

         assign_abandon(pack_referred1, "Refer");
         assign_abandon(pack_copied,    "Copy");
         assign_abandon(pack_cloned,    "Clone");
         assign_abandon(pack_moved1,    "Move");
         assign_abandon(pack_abandoned, "Abandon");
         assign_abandon(pack_disowned,  "Disown");
      }

      if constexpr (CT::DeepDense<E>) {
         WHEN("Assigned and misabsorbed by abandon") {
            auto misabsorb_abandon = [&](T& a, int uses) {
               auto movable = *element;
               REQUIRE_THROWS(a.AssignAbsorb(Abandon(movable)));

               Text_CheckState_ContainsOne(a, *originalElement, uses);
               Many_CheckState_OwnedFull<TypeOf<E>>(movable);
               Many_Helper_TestSame(movable, *element);
            };

            misabsorb_abandon(pack_referred1, 3);
            Text_CheckState_OwnedFull(pack_referred1);

            misabsorb_abandon(pack_referred2, 3);
            Text_CheckState_OwnedFull(pack_referred2);

            misabsorb_abandon(pack_copied,    1);
            Text_CheckState_OwnedFull(pack_copied);

            misabsorb_abandon(pack_cloned,    1);
            Text_CheckState_OwnedFull(pack_cloned);

            misabsorb_abandon(pack_moved1,    1);
            Text_CheckState_OwnedFull(pack_moved1);

            misabsorb_abandon(pack_moved2,    1);
            Text_CheckState_OwnedFull(pack_moved2);

            misabsorb_abandon(pack_abandoned, 1);
            Text_CheckState_OwnedFull(pack_abandoned);

            misabsorb_abandon(pack_disowned,  3);
            Text_CheckState_DisownedFull(pack_disowned);
         }
      }

      if constexpr (CT::Container<E> and CT::Text<E>) {
         WHEN("Assigned and absorbed by abandon") {
            auto absorb_abandon = [&](T& a, [[maybe_unused]] const char* intent, int uses) {
               auto movable = *element;
               REQUIRE_NOTHROW(a.AssignAbsorb(Abandon(movable)));

               Text_CheckState_OwnedFull(a);
               Text_CheckState_Abandoned(movable);
               Text_Helper_TestSame(a, *element);
               REQUIRE(a.GetUses() == uses);
               REQUIRE(a.GetAllocation() == element->GetAllocation());

               BenchmarkTextStd("Absorb/" + intent + "/AssignAbsorb/Abandon", 30, 100,
                  T movable1 = *element;
                  T movable2 = *originalElement;
                  a.AssignAbsorb(Abandon(movable1)),          a.AssignAbsorb(Abandon(movable2)),
                  stdstr movable1 (1, *element);
                  stdstr movable2 (1, *originalElement);
                  stdstr temp_std = ::std::move(movable1),    temp_std = ::std::move(movable2)
               );
            };

            absorb_abandon(pack_referred1, "Refer",   2);
            absorb_abandon(pack_copied,    "Copy",    3);
            absorb_abandon(pack_cloned,    "Clone",   4);
            absorb_abandon(pack_moved1,    "Move",    5);
            absorb_abandon(pack_abandoned, "Abandon", 6);
            absorb_abandon(pack_disowned,  "Disown",  7);
         }
      }

      /// MARK: Assign empty                                                  
      WHEN("Ambigous assigned empty self") {
         auto assign_empty_self = [&](T& a) {
            REQUIRE_NOTHROW(a = T{});
            Text_CheckState_Default(a);
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
            Text_Helper_TestSame(a, backup, not allow_change_in_constness);
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

            Text_Helper_TestSame(absorbed1, compare_against);
            Text_Helper_TestSame(absorbed2, compare_against);
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

            Text_CheckState_Default(a);
            Text_CheckState_OwnedFull(absorbed);
            Text_Helper_TestSame(absorbed, backup);
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

            Text_CheckState_Default(a);
            Text_CheckState_OwnedFull(absorbed);
            Text_Helper_TestSame(absorbed, backup);
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

            Text_CheckState_Abandoned(a);
            Text_CheckState_OwnedFull(absorbed);
            Text_Helper_TestSame(absorbed, backup);
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

            Text_CheckState_OwnedFull(a);
            Text_CheckState_DisownedFull(absorbed);
            Text_Helper_TestSame(absorbed, a, false);
            REQUIRE(absorbed.IsConstant());
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
         Text_CheckState_DisownedFull(pack_disowned);
         Text_CheckState_DisownedFull(absorbed);
         Text_Helper_TestSame(absorbed, pack_disowned);
         REQUIRE(absorbed.IsConstant());
         REQUIRE(absorbed.GetUses() == 3);
   }
      
      WHEN("Absorbed by copy") {
         auto absorb_construct_copy = [&](T& a) {
            T absorbed {Copy {a}};

            Text_CheckState_OwnedFull(absorbed);
            Text_CheckState_ContainsOne(absorbed, *originalElement);

            REQUIRE(absorbed.GetUses() == 1);
            REQUIRE(absorbed == a);
            REQUIRE(absorbed.GetRaw() != a.GetRaw());
         };

         absorb_construct_copy(pack_referred1);
         Text_CheckState_OwnedFull(pack_referred1);

         absorb_construct_copy(pack_referred2);
         Text_CheckState_OwnedFull(pack_referred2);

         absorb_construct_copy(pack_copied);
         Text_CheckState_OwnedFull(pack_copied);

         absorb_construct_copy(pack_cloned);
         Text_CheckState_OwnedFull(pack_cloned);

         absorb_construct_copy(pack_moved1);
         Text_CheckState_OwnedFull(pack_moved1);

         absorb_construct_copy(pack_moved2);
         Text_CheckState_OwnedFull(pack_moved2);

         absorb_construct_copy(pack_abandoned);
         Text_CheckState_OwnedFull(pack_abandoned);

         absorb_construct_copy(pack_disowned);
         Text_CheckState_DisownedFull(pack_disowned);
      }
      
      WHEN("Absorbed by clone") {
         auto absorb_construct_clone = [&](T& a, int uses) {
            T absorbed {Clone {a}};

            if (uses == 0)
               Text_CheckState_DisownedFull(a);
            else
               Text_CheckState_OwnedFull(a);

            Text_CheckState_OwnedFull(absorbed);
            REQUIRE(absorbed == a);
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
            BenchmarkTextStd("Absorb/" + intent + "/Clear", 30, 100,
               T temp = a,                         temp.Clear(),
               stdstr temp_std (1, *element),      temp_std.clear()
            );

            REQUIRE_NOTHROW(a.Clear());

            if (uses != 1)
               Text_CheckState_Default(a, true);
            else
               Text_CheckState_OwnedEmpty(a);
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
            BenchmarkTextStd("Absorb/" + intent + "/Reset", 30, 100,
               T temp = a,                         temp.Reset(),
               stdstr temp_std (1, *element),      temp_std.clear()
            );

            REQUIRE_NOTHROW(a.Reset());

            Text_CheckState_Default(a);
         };

         reset_full(pack_referred1, "Refer");
         reset_full(pack_copied,    "Copy");
         reset_full(pack_cloned,    "Clone");
         reset_full(pack_moved1,    "Move");
         reset_full(pack_abandoned, "Abandon");
         reset_full(pack_disowned,  "Disown");
      }

      #if LANGULUS_FEATURE(MANAGED_MEMORY)
      if constexpr (Same<E, char>) {
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
      #endif

      /// MARK: Compare                                                       
      WHEN("Compared") {
         ScopedE e1 {1};
         T another_pack1 {Piecewise, *e1};
         T defaulted_pack;

         auto compared_full = [&](T& a, [[maybe_unused]] const char* intent) {
            T same_pack {a};

            if constexpr (Same<E, RT>) {
               // RT serializes to the same text regardless inner int   
               REQUIRE      (a == another_pack1);
               REQUIRE_FALSE(a != another_pack1);
            }
            else {
               REQUIRE      (a != another_pack1);
               REQUIRE_FALSE(a == another_pack1);
            }

            REQUIRE      (a != defaulted_pack);
            REQUIRE_FALSE(a == defaulted_pack);
            REQUIRE      (a == same_pack);
            REQUIRE_FALSE(a != same_pack);
            REQUIRE      (static_cast<bool>(a));

            REQUIRE      (         a != nullptr    );
            REQUIRE_FALSE(         a == nullptr    );
            REQUIRE      (   nullptr != a          );
            REQUIRE_FALSE(   nullptr == a          );
            REQUIRE      (         a != ""         );
            REQUIRE_FALSE(         a == ""         );
            REQUIRE      (        "" != a          );
            REQUIRE_FALSE(        "" == a          );

            REQUIRE      (         a != T{nullptr} );
            REQUIRE_FALSE(         a == T{nullptr} );
            REQUIRE      (T{nullptr} != a          );
            REQUIRE_FALSE(T{nullptr} == a          );
            REQUIRE      (         a != T{""}      );
            REQUIRE_FALSE(         a == T{""}      );
            REQUIRE      (     T{""} != a          );
            REQUIRE_FALSE(     T{""} == a          );
   
            [[maybe_unused]] volatile bool dont_optimize = false;
            BenchmarkTextStd("Absorb/" + intent + "/operator==", 30, 100,
               (void) 0,                                    dont_optimize |= (a == same_pack),
               const stdstr a_std (1, *element);
               const stdstr another_pack1_std (1, *e1),     dont_optimize |= (a_std == another_pack1_std)
            );
            BenchmarkTextStd("Absorb/" + intent + "/operator!=", 30, 100,
               (void) 0,                                    dont_optimize |= (a != same_pack),
               const stdstr a_std (1, *element);
               const stdstr another_pack1_std (1, *e1),     dont_optimize |= (a_std != another_pack1_std)
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
         auto contains_full = [&](auto& a) {
            if constexpr (CT::Sparse<E>) {
               //TODO pointers are always different
               REQUIRE_FALSE(a.Contains('?'));
            }
            else if constexpr (Same<E, Text>) {
               REQUIRE      (a.Contains('5'));
               REQUIRE      (a.Contains('6'));
               REQUIRE_FALSE(a.Contains('?'));
            }
            else if constexpr (Same<E, RT>) {
               REQUIRE      (a.Contains('R'));
               REQUIRE      (a.Contains('T'));
               REQUIRE      (a.Contains('('));
               REQUIRE      (a.Contains(')'));
               REQUIRE_FALSE(a.Contains('?'));
            }
            else if constexpr (Same<E, char>) {
               REQUIRE      (a.Contains(','));
               REQUIRE_FALSE(a.Contains('?'));
            }
            else {
               REQUIRE      (a.Contains('5'));
               REQUIRE      (a.Contains('6'));
               REQUIRE_FALSE(a.Contains('?'));
            }
         };

         contains_full(pack_referred1);
         contains_full(pack_referred2);
         contains_full(pack_copied);
         contains_full(pack_cloned);
         contains_full(pack_moved1);
         contains_full(pack_moved2);
         contains_full(pack_abandoned);
         contains_full(pack_disowned);

         [[maybe_unused]] volatile bool dont_optimize = false;
         BenchmarkText("Absorb/Contains", 30,
            (void) 0, dont_optimize |= pack_referred1.Contains(*element)
         );
      }

      /// MARK: Contains Range                                                
      WHEN("ContainsRange when full") {
         auto contains_full = [&](auto& a) {
            if constexpr (CT::Sparse<E>) {
               //TODO pointers are always different
            }
            else if constexpr (Same<E, RT>) {
               REQUIRE      (a.ContainsRange("RT("));
               REQUIRE_FALSE(a.ContainsRange("int"));
               REQUIRE_FALSE(a.ContainsRange(""));
            }
            else if constexpr (Same<E, char>) {
               REQUIRE      (a.ContainsRange(","));
               REQUIRE_FALSE(a.ContainsRange("?"));
               REQUIRE_FALSE(a.ContainsRange(""));
            }
            else {
               REQUIRE      (a.ContainsRange("55"));
               REQUIRE      (a.ContainsRange("556"));
               REQUIRE      (a.ContainsRange("56"));
               REQUIRE      (a.ContainsRange("6"));
               REQUIRE      (a.ContainsRange("5"));
               REQUIRE_FALSE(a.ContainsRange("?"));
               REQUIRE_FALSE(a.ContainsRange("57"));
               REQUIRE_FALSE(a.ContainsRange("557"));
               REQUIRE_FALSE(a.ContainsRange("5578"));
               REQUIRE_FALSE(a.ContainsRange(""));
            }
         };

         contains_full(pack_referred1);
         contains_full(pack_referred2);
         contains_full(pack_copied);
         contains_full(pack_cloned);
         contains_full(pack_moved1);
         contains_full(pack_moved2);
         contains_full(pack_abandoned);
         contains_full(pack_disowned);

         [[maybe_unused]] volatile bool dont_optimize = false;
         BenchmarkText("Absorb/ContainsRange", 30,
            (void) 0, dont_optimize |= pack_referred1.ContainsRange(*element)
         );
      }
      
      /// MARK: Range                                                         
      WHEN("Range-iterated (default)") {
         auto scan = [&](auto& pack) {
            IterateDefault strategy(pack);
            IterateDefault strategyConst(::std::as_const(pack));
            using Iterator      = decltype(strategy.begin());
            using IteratorConst = decltype(strategyConst.begin());

            static_assert(::std::same_as<Iterator,      decltype(strategy.end())>);
            static_assert(::std::same_as<IteratorConst, decltype(strategyConst.end())>);
            static_assert(::std::input_or_output_iterator<Iterator>);
            static_assert(::std::input_or_output_iterator<IteratorConst>);

            static_assert(::std::random_access_iterator<Iterator>);
            static_assert(::std::random_access_iterator<IteratorConst>);
            static_assert(::std::contiguous_iterator<Iterator>);
            static_assert(::std::contiguous_iterator<IteratorConst>);

            size_t counter = 0;
            for (auto& it : pack) {
               (void) it;
               ++counter;
               static_assert(Same<char, decltype(it)>);
            }
            REQUIRE(counter == pack.GetCount());

            counter = 0;
            for (auto& it : ::std::as_const(pack)) {
               (void) it;
               ++counter;
               static_assert(Same<char, decltype(it)>);
            }
            REQUIRE(counter == pack.GetCount());

            counter = 0;
            for (auto& it : strategy) {
               (void) it;
               ++counter;
               static_assert(Same<char, decltype(it)>);
            }
            REQUIRE(counter == pack.GetCount());

            counter = 0;
            for (auto& it : strategyConst) {
               (void) it;
               ++counter;
               static_assert(Same<char, decltype(it)>);
            }
            REQUIRE(counter == pack.GetCount());
         };

         scan(pack_referred1);
         scan(pack_referred2);
         scan(pack_copied);
         scan(pack_cloned);
         scan(pack_moved1);
         scan(pack_moved2);
         scan(pack_abandoned);
         scan(pack_disowned);
      }

      WHEN("Range-iterated (reverse)") {
         auto scan = [&](auto& pack) {
            IterateInReverse strategy(pack);
            IterateInReverse strategyConst(::std::as_const(pack));
            using Iterator      = decltype(strategy.begin());
            using IteratorConst = decltype(strategyConst.begin());

            static_assert(::std::same_as<Iterator,      decltype(strategy.end())>);
            static_assert(::std::same_as<IteratorConst, decltype(strategyConst.end())>);
            static_assert(::std::input_or_output_iterator<Iterator>);
            static_assert(::std::input_or_output_iterator<IteratorConst>);

            static_assert(::std::random_access_iterator<Iterator>);
            static_assert(::std::random_access_iterator<IteratorConst>);
            static_assert(::std::contiguous_iterator<Iterator>);
            static_assert(::std::contiguous_iterator<IteratorConst>);

            size_t counter = 0;
            for (auto& it : strategy) {
               (void) it;
               ++counter;
               static_assert(Exact<char, decltype(it)>);
            }
            REQUIRE(counter == pack.GetCount());

            counter = 0;
            for (auto& it : strategyConst) {
               (void) it;
               ++counter;
               static_assert(Exact<char const, decltype(it)>);
            }
            REQUIRE(counter == pack.GetCount());
         };

         scan(pack_referred1);
         scan(pack_referred2);
         scan(pack_copied);
         scan(pack_cloned);
         scan(pack_moved1);
         scan(pack_moved2);
         scan(pack_abandoned);
         scan(pack_disowned);
      }

      WHEN("Range-iterated (noderef)") {
         auto scan = [&](auto& pack) {
            IterateNoDeref strategy(pack);
            IterateNoDeref strategyConst(::std::as_const(pack));
            using Iterator      = decltype(strategy.begin());
            using IteratorConst = decltype(strategyConst.begin());

            static_assert(::std::same_as<Iterator,      decltype(strategy.end())>);
            static_assert(::std::same_as<IteratorConst, decltype(strategyConst.end())>);
            static_assert(::std::input_or_output_iterator<Iterator>);
            static_assert(::std::input_or_output_iterator<IteratorConst>);
            //static_assert(::std::random_access_iterator<Iterator>);
            //static_assert(::std::contiguous_iterator<Iterator>);

            size_t counter = 0;
            for (auto& it : strategy) {
               (void) it;
               ++counter;
               static_assert(Exact<typename IterateDefault<false, T>::Iterator const&, decltype(it)>);
            }
            REQUIRE(counter == pack.GetCount());

            counter = 0;
            for (auto& it : strategyConst) {
               (void) it;
               ++counter;
               static_assert(Exact<typename IterateDefault<false, T const>::Iterator const&, decltype(it)>);
            }
            REQUIRE(counter == pack.GetCount());
         };

         scan(pack_referred1);
         scan(pack_referred2);
         scan(pack_copied);
         scan(pack_cloned);
         scan(pack_moved1);
         scan(pack_moved2);
         scan(pack_abandoned);
         scan(pack_disowned);
      }

      WHEN("Range-iterated (handles)") {
         auto scan = [&](auto& pack) {
            IterateHandles strategy(pack);
            IterateHandles strategyConst(::std::as_const(pack));
            using Iterator      = decltype(strategy.begin());
            using IteratorConst = decltype(strategyConst.begin());

            static_assert(::std::same_as<Iterator,      decltype(strategy.end())>);
            static_assert(::std::same_as<IteratorConst, decltype(strategyConst.end())>);
            static_assert(::std::input_or_output_iterator<Iterator>);
            static_assert(::std::input_or_output_iterator<IteratorConst>);
            //static_assert(::std::random_access_iterator<Iterator>);
            //static_assert(::std::contiguous_iterator<Iterator>);

            size_t counter = 0;
            for (auto& it : strategy) {
               (void) it;
               ++counter;
               static_assert(CT::Handle<decltype(it)>);
            }
            REQUIRE(counter == pack.GetCount());

            counter = 0;
            for (auto& it : strategyConst) {
               (void) it;
               ++counter;
               static_assert(CT::Handle<decltype(it)>);
            }
            REQUIRE(counter == pack.GetCount());
         };

         scan(pack_referred1);
         scan(pack_referred2);
         scan(pack_copied);
         scan(pack_cloned);
         scan(pack_moved1);
         scan(pack_moved2);
         scan(pack_abandoned);
         scan(pack_disowned);
      }

      WHEN("Range-iterated (together)") {
         IterateTogether strategy(
            pack_referred1,
            pack_referred2,
            pack_copied,
            pack_cloned,
            pack_moved1,
            pack_moved2,
            pack_abandoned,
            pack_disowned
         );
         IterateTogether strategyConst(
            ::std::as_const(pack_referred1),
                            pack_referred2,
            ::std::as_const(pack_copied),
                            pack_cloned,
            ::std::as_const(pack_moved1),
                            pack_moved2,
            ::std::as_const(pack_abandoned),
            ::std::as_const(pack_disowned)
         );
         using Iterator      = decltype(strategy.begin());
         using IteratorConst = decltype(strategyConst.begin());

         static_assert(::std::same_as<Iterator,      decltype(strategy.end())>);
         static_assert(::std::same_as<IteratorConst, decltype(strategyConst.end())>);
         static_assert(::std::input_or_output_iterator<Iterator>);
         static_assert(::std::input_or_output_iterator<IteratorConst>);
         //static_assert(::std::random_access_iterator<Iterator>);
         //static_assert(::std::contiguous_iterator<Iterator>);

         size_t counter = 0;
         for (auto& it : strategy) {
            (void) it;
            ++counter;
            static_assert(Exact<char*, decltype(it.template Get<0>())>);
            static_assert(Exact<char*, decltype(it.template Get<1>())>);
            static_assert(Exact<char*, decltype(it.template Get<2>())>);
            static_assert(Exact<char*, decltype(it.template Get<3>())>);
            static_assert(Exact<char*, decltype(it.template Get<4>())>);
            static_assert(Exact<char*, decltype(it.template Get<5>())>);
            static_assert(Exact<char*, decltype(it.template Get<6>())>);
            static_assert(Exact<char*, decltype(it.template Get<7>())>);
         }
         REQUIRE(counter == pack_referred1.GetCount());

         counter = 0;
         for (auto& it : strategyConst) {
            (void) it;
            ++counter;
            static_assert(Exact<char const*, decltype(it.template Get<0>())>);
            static_assert(Exact<char*,       decltype(it.template Get<1>())>);
            static_assert(Exact<char const*, decltype(it.template Get<2>())>);
            static_assert(Exact<char*,       decltype(it.template Get<3>())>);
            static_assert(Exact<char const*, decltype(it.template Get<4>())>);
            static_assert(Exact<char*,       decltype(it.template Get<5>())>);
            static_assert(Exact<char const*, decltype(it.template Get<6>())>);
            static_assert(Exact<char const*, decltype(it.template Get<7>())>);
         }
         REQUIRE(counter == pack_referred1.GetCount());
      }
   }

   GIVEN("Two absorb-constructed containers") {
      const ScopedE e556 {556};
      const ScopedE e66  {66};

      T piecewise1{Piecewise, *e556};
      T piecewise2{Piecewise, *e66};
      T src {Absorb, Abandon(piecewise1)};
      T dst {Absorb, Abandon(piecewise2)};

      /// MARK: Handles                                                       
      WHEN("GetHandle is called on mutable container") {
         auto src_handle = src.GetHandle();

         #if not LANGULUS(FORCE_TYPE_ERASURE)
            static_assert(::std::same_as<decltype(src_handle), THandle<char&>>);
         #else
            static_assert(::std::same_as<decltype(src_handle), HandleMut>);
         #endif

         auto src_data = src_handle.template Get<char>();
         REQUIRE(src_handle.template Get<char>() == src_data);
         Handle_CheckState_OwnedFull<char>(src_handle);

         auto dst_handle = dst.GetHandle();
         auto dst_data   = dst_handle.template Get<char>();
         REQUIRE(dst_handle.template Get<char>() == dst_data);
         Handle_CheckState_OwnedFull<char>(dst_handle);

         REQUIRE(dst_data != src_data);

         THEN("Handle assigned to another container") {
            REQUIRE_NOTHROW(dst_handle.Assign(Move(src_handle)));

            Handle_CheckState_OwnedFull<char>(src_handle);
            Handle_CheckState_OwnedFull<char>(dst_handle);

            Text_CheckState_ContainsOne(src, *e556);

            if constexpr (CT::Sparse<E>)
               ; //TODO
            else if constexpr (Same<E, char>)
               Text_CheckState_ContainsString(dst, ",");
            else if constexpr (Same<E, RT>)
               Text_CheckState_ContainsString(dst, "RT(unknown)");
            else if constexpr (Same<E, Text>)
               Text_CheckState_ContainsString(dst, "\"66\"");
            else
               Text_CheckState_ContainsString(dst, "56");
         }
         
         THEN("Handle is swapped with another container's handle") {
            REQUIRE_NOTHROW(dst_handle.SwapContents(src_handle));

            Handle_CheckState_OwnedFull<char>(src_handle);
            Handle_CheckState_OwnedFull<char>(dst_handle);

            REQUIRE(src_handle.template Get<char>() == src_data);
            REQUIRE(dst_handle.template Get<char>() == dst_data);

            if constexpr (CT::Sparse<E>)
               ; //TODO
            else if constexpr (Same<E, char>) {
               Text_CheckState_ContainsString(src, "B");
               Text_CheckState_ContainsString(dst, ",");
            }
            else if constexpr (Same<E, RT>) {
               Text_CheckState_ContainsString(src, "RT(unknown)");
               Text_CheckState_ContainsString(dst, "RT(unknown)");
            }
            else if constexpr (Same<E, Text>) {
               Text_CheckState_ContainsString(src, "\"556\"");
               Text_CheckState_ContainsString(dst, "\"66\"");
            }
            else {
               Text_CheckState_ContainsOne(src, "656");
               Text_CheckState_ContainsOne(dst, "56");
            }

            // We should be able to do this indefinitely                
            for(int i = 0; i < 101; ++i)
               dst_handle.SwapContents(src_handle);
         }
         
         #if not LANGULUS(FORCE_TYPE_ERASURE)
         THEN("Handle moved into a local handle") {
            THandle<char> local {Absorb, Move(src_handle)};

            Handle_CheckState_OwnedFull<char>(src_handle);
            Handle_CheckState_OwnedFull<char>(local);
            REQUIRE(src_handle.template Get<char>() == src_data);
            REQUIRE(local.template Get<char>() != src_data);

            if constexpr (CT::Sparse<E>)
               ; //TODO
            else if constexpr (Same<E, char>) {
               Text_CheckState_ContainsString(src, ",");
               REQUIRE(*local.template Get<char>() == ',');
            }
            else if constexpr (Same<E, RT>) {
               Text_CheckState_ContainsString(src, "RT(unknown)");
               REQUIRE(*local.template Get<char>() == 'R');
            }
            else if constexpr (Same<E, Text>) {
               Text_CheckState_ContainsString(src, "\"556\"");
               REQUIRE(*local.template Get<char>() == '"');
            }
            else {
               Text_CheckState_ContainsString(src, "556");
               REQUIRE(*local.template Get<char>() == '5');
            }
         }

         THEN("Handle is swapped with local handle, and then back to container") {
            THandle<char> local = '1';
            REQUIRE_NOTHROW(local.SwapContents(src_handle));
            auto local_data = local.template Get<char>();

            Handle_CheckState_OwnedFull<char>(src_handle);
            Handle_CheckState_OwnedFull<char>(local);
            REQUIRE(src_handle.template Get<char>() == src_data);
            REQUIRE(local_data);
            REQUIRE(local_data != src_data);

            if constexpr (CT::Sparse<E>)
               ; //TODO
            else if constexpr (Same<E, char>) {
               Text_CheckState_ContainsString(src, "1");
               REQUIRE(*local.template Get<char>() == ',');
            }
            else if constexpr (Same<E, RT>) {
               Text_CheckState_ContainsString(src, "1T(unknown)");
               REQUIRE(*local.template Get<char>() == 'R');
            }
            else if constexpr (Same<E, Text>) {
               Text_CheckState_ContainsString(src, "1556\"");
               REQUIRE(*local.template Get<char>() == '"');
            }
            else {
               Text_CheckState_ContainsString(src, "156");
               REQUIRE(*local.template Get<char>() == '5');
            }

            REQUIRE_NOTHROW(local.SwapContents(src_handle));
            REQUIRE(src_handle.template Get<char>() == src_data);
            REQUIRE(local.template Get<char>() == local_data);

            if constexpr (CT::Sparse<E>)
               ; //TODO
            else if constexpr (Same<E, char>) {
               Text_CheckState_ContainsString(src, ",");
               REQUIRE(*local.template Get<char>() == '1');
            }
            else if constexpr (Same<E, RT>) {
               Text_CheckState_ContainsString(src, "RT(unknown)");
               REQUIRE(*local.template Get<char>() == '1');
            }
            else if constexpr (Same<E, Text>) {
               Text_CheckState_ContainsString(src, "\"556\"");
               REQUIRE(*local.template Get<char>() == '1');
            }
            else {
               Text_CheckState_ContainsString(src, "556");
               REQUIRE(*local.template Get<char>() == '1');
            }

            // We should be able to do this indefinitely                
            for(int i = 0; i < 101; ++i)
               local.SwapContents(src_handle);
         }
         #endif
      }

      WHEN("GetHandle is called on constant container") {
         T const& pack_constant = src;
         auto h = pack_constant.GetHandle();
         static_assert(::std::same_as<decltype(h), THandle<char const&>>);

         Handle_CheckState_OwnedFull<char const>(h);
      }
   }
   
   GIVEN("Absorbed container and a couple of arrays") {
      const ScopedE darray1[5] {49, 50, 51, 52, 53};
      const ScopedE darray2[5] {54, 55, 56, 57, 58};

      const E immovable[5] {
         *darray1[0], *darray1[1], *darray1[2], *darray1[3], *darray1[4]
      };
      E movable1[5] {
         *darray2[0], *darray2[1], *darray2[2], *darray2[3], *darray2[4]
      };
      E movable2[5] {
         *darray2[0], *darray2[1], *darray2[2], *darray2[3], *darray2[4]
      };
      E movable3[5] {
         *darray2[0], *darray2[1], *darray2[2], *darray2[3], *darray2[4]
      };

      const ScopedE e556 {556};
      T piecewise1{Piecewise, *e556};
      T pack {Absorb, Abandon(piecewise1)};

      /// MARK: Insert array                                                  
      WHEN("Insert an array to the back") {
         volatile size_t inserted = 0;
         REQUIRE_NOTHROW(inserted += pack.InsertAt(Index::Back,           immovable));
         REQUIRE_NOTHROW(inserted += pack.InsertAt(Index::Back, Refer    {immovable}));
         REQUIRE_NOTHROW(inserted += pack.InsertAt(Index::Back, Copy     {immovable}));
         REQUIRE_NOTHROW(inserted += pack.InsertAt(Index::Back, Disown   {immovable}));
         REQUIRE_NOTHROW(inserted += pack.InsertAt(Index::Back, std::move(movable1)));
         REQUIRE_NOTHROW(inserted += pack.InsertAt(Index::Back, Move     {movable2}));
         REQUIRE_NOTHROW(inserted += pack.InsertAt(Index::Back, Abandon  {movable3}));
         REQUIRE_NOTHROW(inserted += pack.InsertAt(Index::Back, Clone    {immovable}));

         Text_CheckState_OwnedFull(pack);

         if constexpr (CT::Container<E>) {
            for (int i = 0; i < 5; ++i) {
               Many_CheckState_OwnedFull<TypeOf<E>>(immovable[i]);
               Many_CheckState_OwnedFull<TypeOf<E>>(movable1[i]);
               Many_CheckState_OwnedFull<TypeOf<E>>(movable2[i]);
               Many_CheckState_OwnedFull<TypeOf<E>>(movable3[i]);
            }
         }

         if constexpr (CT::Sparse<E>) {
            //TODO pointers are always different
         }
         else if constexpr (Same<E, Text>) {
            REQUIRE(inserted == 4*5*8);
            Text_CheckState_ContainsString(pack,
               "\"556\"\"49\"\"50\"\"51\"\"52\"\"53\""
                      "\"49\"\"50\"\"51\"\"52\"\"53\""
                      "\"49\"\"50\"\"51\"\"52\"\"53\""
                      "\"49\"\"50\"\"51\"\"52\"\"53\""
                      "\"54\"\"55\"\"56\"\"57\"\"58\""
                      "\"54\"\"55\"\"56\"\"57\"\"58\""
                      "\"54\"\"55\"\"56\"\"57\"\"58\""
                      "\"49\"\"50\"\"51\"\"52\"\"53\""
            );
         }
         else if constexpr (Same<E, RT>) {
            REQUIRE(inserted == 10*5*8);
            Text_CheckState_ContainsString(pack,
               "RT(unknown)RT(copied)RT(copied)RT(copied)RT(copied)RT(copied)"
                          "RT(copied)RT(copied)RT(copied)RT(copied)RT(copied)"
                          "RT(copied)RT(copied)RT(copied)RT(copied)RT(copied)"
                          "RT(copied)RT(copied)RT(copied)RT(copied)RT(copied)"
                          "RT(copied)RT(copied)RT(copied)RT(copied)RT(copied)"
                          "RT(copied)RT(copied)RT(copied)RT(copied)RT(copied)"
                          "RT(copied)RT(copied)RT(copied)RT(copied)RT(copied)"
                          "RT(copied)RT(copied)RT(copied)RT(copied)RT(copied)"
            );
         }
         else if constexpr (Same<E, char>) {
            REQUIRE(inserted == 5*8);
            Text_CheckState_ContainsString(pack,
               ",12345"
                "12345"
                "12345"
                "12345"
                "6789:"
                "6789:"
                "6789:"
                "12345"
            );
         }
         else {
            REQUIRE(inserted == 2*5*8);
            Text_CheckState_ContainsString(pack,
               "5564950515253"
                  "4950515253"
                  "4950515253"
                  "4950515253"
                  "5455565758"
                  "5455565758"
                  "5455565758"
                  "4950515253"
            );
         }

         BenchmarkTextStd("Absorb/Insert/Array/Back", 30, 100,
            T temp,              temp.InsertAt(Index::Back, immovable),
            stdstr temp_std,     std::copy(immovable, immovable + 5, std::back_inserter(temp_std))
         );
      }

      WHEN("Insert an array to the front") {
         size_t inserted = 0;
         REQUIRE_NOTHROW(inserted += pack.InsertAt(Index::Front,           immovable));
         REQUIRE_NOTHROW(inserted += pack.InsertAt(Index::Front, Refer    {immovable}));
         REQUIRE_NOTHROW(inserted += pack.InsertAt(Index::Front, Copy     {immovable}));
         REQUIRE_NOTHROW(inserted += pack.InsertAt(Index::Front, Disown   {immovable}));
         REQUIRE_NOTHROW(inserted += pack.InsertAt(Index::Front, std::move(movable1)));
         REQUIRE_NOTHROW(inserted += pack.InsertAt(Index::Front, Move     {movable2}));
         REQUIRE_NOTHROW(inserted += pack.InsertAt(Index::Front, Abandon  {movable3}));
         REQUIRE_NOTHROW(inserted += pack.InsertAt(Index::Front, Clone    {immovable}));

         Text_CheckState_OwnedFull(pack);

         if constexpr (CT::Container<E>) {
            for (int i = 0; i < 5; ++i) {
               Many_CheckState_OwnedFull<TypeOf<E>>(immovable[i]);
               Many_CheckState_OwnedFull<TypeOf<E>>(movable1[i]);
               Many_CheckState_OwnedFull<TypeOf<E>>(movable2[i]);
               Many_CheckState_OwnedFull<TypeOf<E>>(movable3[i]);
            }
         }

         if constexpr (CT::Sparse<E>) {
            //TODO pointers are always different
         }
         else if constexpr (Same<E, Text>) {
            REQUIRE(inserted == 4*5*8);
            Text_CheckState_ContainsString(pack,
               "\"49\"\"50\"\"51\"\"52\"\"53\""
               "\"54\"\"55\"\"56\"\"57\"\"58\""
               "\"54\"\"55\"\"56\"\"57\"\"58\""
               "\"54\"\"55\"\"56\"\"57\"\"58\""
               "\"49\"\"50\"\"51\"\"52\"\"53\""
               "\"49\"\"50\"\"51\"\"52\"\"53\""
               "\"49\"\"50\"\"51\"\"52\"\"53\""
               "\"49\"\"50\"\"51\"\"52\"\"53\"\"556\""
            );
         }
         else if constexpr (Same<E, RT>) {
            REQUIRE(inserted == 10*5*8);
            Text_CheckState_ContainsString(pack,
               "RT(copied)RT(copied)RT(copied)RT(copied)RT(copied)"
               "RT(copied)RT(copied)RT(copied)RT(copied)RT(copied)"
               "RT(copied)RT(copied)RT(copied)RT(copied)RT(copied)"
               "RT(copied)RT(copied)RT(copied)RT(copied)RT(copied)"
               "RT(copied)RT(copied)RT(copied)RT(copied)RT(copied)"
               "RT(copied)RT(copied)RT(copied)RT(copied)RT(copied)"
               "RT(copied)RT(copied)RT(copied)RT(copied)RT(copied)"
               "RT(copied)RT(copied)RT(copied)RT(copied)RT(copied)RT(unknown)"
            );
         }
         else if constexpr (Same<E, char>) {
            REQUIRE(inserted == 5*8);
            Text_CheckState_ContainsString(pack,
               "12345"
               "6789:"
               "6789:"
               "6789:"
               "12345"
               "12345"
               "12345"
               "12345,"
            );
         }
         else {
            REQUIRE(inserted == 2*5*8);
            Text_CheckState_ContainsString(pack,
               "4950515253"
               "5455565758"
               "5455565758"
               "5455565758"
               "4950515253"
               "4950515253"
               "4950515253"
               "4950515253556"
            );
         }

         BenchmarkTextStd("Absorb/Insert/Array/Front", 30, 100,
            T temp,              temp.InsertAt(Index::Front, darray1),
            stdstr temp_std,     std::copy(darray1, darray1 + 5, std::front_inserter(temp_std))
         );
      }

      /// MARK: Insert at                                                     
      WHEN("Insert an array to a non-existent index") {
         size_t inserted = 0;
         REQUIRE_THROWS(inserted = pack.InsertAt(1000, immovable));
         REQUIRE(inserted == 0);

         Text_CheckState_OwnedFull(pack);
         Text_CheckState_ContainsOne(pack, *e556);
      }

      /// MARK: <<                                                            
      WHEN("Insert at the back by using << operator)") {
         pack <<           immovable[0]
              << Refer    {immovable[1]}
              << Copy     {immovable[2]}
              << Disown   {immovable[3]}
              << std::move( movable1[0])
              << Move     { movable2[0]}
              << Abandon  { movable3[0]}
              << Clone    {immovable[4]};

         Text_CheckState_OwnedFull(pack);

         if constexpr (CT::Container<E>) {
            for (int i = 0; i < 5; ++i)
               Many_CheckState_OwnedFull<TypeOf<E>>(immovable[i]);
            Many_CheckState_OwnedFull<TypeOf<E>>(movable1[0]);
            Many_CheckState_OwnedFull<TypeOf<E>>(movable2[0]);
            Many_CheckState_OwnedFull<TypeOf<E>>(movable3[0]);
         }

         if constexpr (CT::Sparse<E>) {
            //TODO pointers are always different
         }
         else if constexpr (Same<E, Text>) {
            Text_CheckState_ContainsString(pack,
               "\"556\"\"49\"\"50\"\"51\"\"52\"\"54\"\"54\"\"54\"\"53\""
            );
         }
         else if constexpr (Same<E, RT>) {
            Text_CheckState_ContainsString(pack,
               "RT(unknown)RT(copied)RT(copied)RT(copied)RT(copied)"
                          "RT(copied)RT(copied)RT(copied)RT(copied)"
            );
         }
         else if constexpr (Same<E, char>) {
            Text_CheckState_ContainsString(pack, ",12346665");
         }
         else {
            Text_CheckState_ContainsString(pack, "5564950515254545453");
         }

         BenchmarkTextStd("Absorb/Insert/Element/Back", 30, 100,
            T temp,              temp << immovable[0],
            stdstr temp_std,     temp_std.emplace_back(immovable[0])
         );
      }

      /// MARK: >>                                                            
      WHEN("Insert at the front by using >> operator)") {
         pack >>           immovable[0]
              >> Refer    {immovable[1]}
              >> Copy     {immovable[2]}
              >> Disown   {immovable[3]}
              >> std::move( movable1[0])
              >> Move     { movable2[0]}
              >> Abandon  { movable3[0]}
              >> Clone    {immovable[4]};

         Text_CheckState_OwnedFull(pack);

         if constexpr (CT::Container<E>) {
            for (int i = 0; i < 5; ++i)
               Many_CheckState_OwnedFull<TypeOf<E>>(immovable[i]);
            Many_CheckState_OwnedFull<TypeOf<E>>(movable1[0]);
            Many_CheckState_OwnedFull<TypeOf<E>>(movable2[0]);
            Many_CheckState_OwnedFull<TypeOf<E>>(movable3[0]);
         }

         if constexpr (CT::Sparse<E>) {
            //TODO pointers are always different
         }
         else if constexpr (Same<E, Text>) {
            Text_CheckState_ContainsString(pack,
               "\"53\"\"54\"\"54\"\"54\"\"52\"\"51\"\"50\"\"49\"\"556\""
            );
         }
         else if constexpr (Same<E, RT>) {
            Text_CheckState_ContainsString(pack,
               "RT(copied)RT(copied)RT(copied)RT(copied)"
               "RT(copied)RT(copied)RT(copied)RT(copied)RT(unknown)"
            );
         }
         else if constexpr (Same<E, char>) {
            Text_CheckState_ContainsString(pack, "56664321,");
         }
         else {
            Text_CheckState_ContainsString(pack, "5354545452515049556");
         }

         BenchmarkTextStd("Absorb/Insert/Element/Front", 30, 100,
            T temp,              temp >> immovable[0],
            stdstr temp_std,     temp_std.emplace_front(immovable[0])
         );
      }

      /// MARK: Concat array                                                  
      if constexpr (CT::Text<E> and CT::Container<E>) {
         WHEN("Concatenate to the back") {
            size_t inserted = 0;
            REQUIRE_NOTHROW(inserted += pack.ConcatAt(Index::Back,           immovable[0]));
            REQUIRE_NOTHROW(inserted += pack.ConcatAt(Index::Back, Refer    {immovable[1]}));
            REQUIRE_NOTHROW(inserted += pack.ConcatAt(Index::Back, Copy     {immovable[2]}));
            REQUIRE_NOTHROW(inserted += pack.ConcatAt(Index::Back, Disown   {immovable[3]}));
            REQUIRE_NOTHROW(inserted += pack.ConcatAt(Index::Back, std::move(movable1[0])));
            REQUIRE_NOTHROW(inserted += pack.ConcatAt(Index::Back, Move     {movable2[1]}));
            REQUIRE_NOTHROW(inserted += pack.ConcatAt(Index::Back, Abandon  {movable3[2]}));
            REQUIRE_NOTHROW(inserted += pack.ConcatAt(Index::Back, Clone    {immovable[4]}));
            REQUIRE(inserted == 16);

            Text_CheckState_OwnedFull(pack);

            if constexpr (CT::Container<E>) {
               for (int i = 0; i < 5; ++i) {
                  Many_CheckState_OwnedFull<TypeOf<E>>(immovable[i]);
                  Many_CheckState_OwnedFull<TypeOf<E>>(movable1[i]);
                  Many_CheckState_OwnedFull<TypeOf<E>>(movable2[i]);
                  Many_CheckState_OwnedFull<TypeOf<E>>(movable3[i]);
               }
            }

            Text_CheckState_ContainsString(pack,"\"556\"4950515254555653");

            BenchmarkTextStd("Absorb/Concat/Element/Back", 30, 100,
               T temp,              temp.ConcatAt(Index::Back, immovable),
               stdstr temp_std,     std::copy(immovable, immovable + 5, std::back_inserter(temp_std))
            );
         }

         WHEN("Concatenate to the front") {
            size_t inserted = 0;
            REQUIRE_NOTHROW(inserted += pack.ConcatAt(Index::Front,           immovable[0]));
            REQUIRE_NOTHROW(inserted += pack.ConcatAt(Index::Front, Refer    {immovable[1]}));
            REQUIRE_NOTHROW(inserted += pack.ConcatAt(Index::Front, Copy     {immovable[2]}));
            REQUIRE_NOTHROW(inserted += pack.ConcatAt(Index::Front, Disown   {immovable[3]}));
            REQUIRE_NOTHROW(inserted += pack.ConcatAt(Index::Front, std::move(movable1[0])));
            REQUIRE_NOTHROW(inserted += pack.ConcatAt(Index::Front, Move     {movable2[1]}));
            REQUIRE_NOTHROW(inserted += pack.ConcatAt(Index::Front, Abandon  {movable3[2]}));
            REQUIRE_NOTHROW(inserted += pack.ConcatAt(Index::Front, Clone    {immovable[4]}));
            REQUIRE(inserted == 16);

            Text_CheckState_OwnedFull(pack);

            if constexpr (CT::Container<E>) {
               for (int i = 0; i < 5; ++i) {
                  Many_CheckState_OwnedFull<TypeOf<E>>(immovable[i]);
                  Many_CheckState_OwnedFull<TypeOf<E>>(movable1[i]);
                  Many_CheckState_OwnedFull<TypeOf<E>>(movable2[i]);
                  Many_CheckState_OwnedFull<TypeOf<E>>(movable3[i]);
               }
            }

            Text_CheckState_ContainsString(pack,"5356555452515049\"556\"");

            BenchmarkTextStd("Absorb/Concat/Element/Front", 30, 100,
               T temp,              temp.ConcatAt(Index::Front, darray1),
               stdstr temp_std,     std::copy(darray1, darray1 + 5, std::front_inserter(temp_std))
            );
         }

         /// MARK: Concat at                                                  
         WHEN("Concatenate to a non-existent index") {
            size_t inserted = 0;
            REQUIRE_THROWS(inserted = pack.ConcatAt(1000, immovable[0]));
            REQUIRE(inserted == 0);
            
            Text_CheckState_OwnedFull(pack);
            Text_CheckState_ContainsOne(pack, *e556);
         }
      }

      /// MARK: +=                                                            
      WHEN("Concatenate array at the back by using += operator)") {
         REQUIRE_NOTHROW(pack +=           immovable );
         REQUIRE_NOTHROW(pack += Refer    {immovable});
         REQUIRE_NOTHROW(pack += Copy     {immovable});
         REQUIRE_NOTHROW(pack += Disown   {immovable});
         REQUIRE_NOTHROW(pack += std::move( movable1));
         REQUIRE_NOTHROW(pack += Move     { movable2});
         REQUIRE_NOTHROW(pack += Abandon  { movable3});
         REQUIRE_NOTHROW(pack += Clone    {immovable});

         Text_CheckState_OwnedFull(pack);

         if constexpr (CT::Container<E>) {
            for (int i = 0; i < 5; ++i)
               Many_CheckState_OwnedFull<TypeOf<E>>(immovable[i]);
            Many_CheckState_OwnedFull<TypeOf<E>>(movable1[0]);
            Many_CheckState_OwnedFull<TypeOf<E>>(movable2[0]);
            Many_CheckState_OwnedFull<TypeOf<E>>(movable3[0]);
         }

         if constexpr (CT::Sparse<E>) {
            //TODO pointers are always different
         }
         else if constexpr (Same<E, RT>) {
            Text_CheckState_ContainsString(pack,
               "RT(unknown)RT(copied)RT(copied)RT(copied)RT(copied)RT(copied)"
                          "RT(copied)RT(copied)RT(copied)RT(copied)RT(copied)"
                          "RT(copied)RT(copied)RT(copied)RT(copied)RT(copied)"
                          "RT(copied)RT(copied)RT(copied)RT(copied)RT(copied)"
                          "RT(copied)RT(copied)RT(copied)RT(copied)RT(copied)"
                          "RT(copied)RT(copied)RT(copied)RT(copied)RT(copied)"
                          "RT(copied)RT(copied)RT(copied)RT(copied)RT(copied)"
                          "RT(copied)RT(copied)RT(copied)RT(copied)RT(copied)"
            );
         }
         else if constexpr (Same<E, char>) {
            Text_CheckState_ContainsString(pack,
               ",12345"
                "12345"
                "12345"
                "12345"
                "6789:"
                "6789:"
                "6789:"
                "12345"
            );
         }
         else if constexpr (Same<E, Text>) {
            Text_CheckState_ContainsString(pack,
               "\"556\"4950515253"
                      "4950515253"
                      "4950515253"
                      "4950515253"
                      "5455565758"
                      "5455565758"
                      "5455565758"
                      "4950515253"
            );
         }
         else {
            Text_CheckState_ContainsString(pack,
               "5564950515253"
                  "4950515253"
                  "4950515253"
                  "4950515253"
                  "5455565758"
                  "5455565758"
                  "5455565758"
                  "4950515253"
            );
         }

         BenchmarkTextStd("Absorb/+=/Array/Back", 30, 100,
            T temp,              temp += immovable,
            stdstr temp_std,     temp_std.emplace_back(immovable[0])
         );
      }

      WHEN("Concatenate element the back by using += operator)") {
         REQUIRE_NOTHROW(pack +=           immovable[0] );
         REQUIRE_NOTHROW(pack += Refer    {immovable[1]});
         REQUIRE_NOTHROW(pack += Copy     {immovable[2]});
         REQUIRE_NOTHROW(pack += Disown   {immovable[3]});
         REQUIRE_NOTHROW(pack += std::move( movable1[0]));
         REQUIRE_NOTHROW(pack += Move     { movable2[0]});
         REQUIRE_NOTHROW(pack += Abandon  { movable3[0]});
         REQUIRE_NOTHROW(pack += Clone    {immovable[4]});

         Text_CheckState_OwnedFull(pack);

         if constexpr (CT::Container<E>) {
            for (int i = 0; i < 5; ++i)
               Many_CheckState_OwnedFull<TypeOf<E>>(immovable[i]);
            Many_CheckState_OwnedFull<TypeOf<E>>(movable1[0]);
            Many_CheckState_OwnedFull<TypeOf<E>>(movable2[0]);
            Many_CheckState_OwnedFull<TypeOf<E>>(movable3[0]);
         }

         if constexpr (CT::Sparse<E>) {
            //TODO pointers are always different
         }
         else if constexpr (Same<E, RT>) {
            Text_CheckState_ContainsString(pack,
               "RT(unknown)RT(copied)RT(copied)RT(copied)RT(copied)"
                          "RT(copied)RT(copied)RT(copied)RT(copied)"
            );
         }
         else if constexpr (Same<E, char>) {
            Text_CheckState_ContainsString(pack, ",12346665");
         }
         else if constexpr (Same<E, Text>) {
            Text_CheckState_ContainsString(pack, "\"556\"4950515254545453");
         }
         else {
            Text_CheckState_ContainsString(pack, "5564950515254545453");
         }

         BenchmarkTextStd("Absorb/+=/Element/Back", 30, 100,
            T temp,              temp += immovable[0],
            stdstr temp_std,     temp_std.emplace_back(immovable[0])
         );
      }
   }

   REQUIRE(memoryState.Assert());
   REQUIRE_FALSE(Allocator::CollectGarbage());
}
