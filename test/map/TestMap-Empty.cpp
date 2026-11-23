///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "TestMapCommon.hpp"
#include "../handle/TestHandlePairCommon.hpp"

namespace Langulus::Annies  
{
   // Explicit instantiation for using extern templates in other tests  
   template struct TMap<Text,   Text>;
   template struct TMap<int,    int>;
   template struct TMap<Any,    Any>;
   template struct TMap<RT,     RT>;
   template struct TMap<char,   char>;

   template struct TMap<Text*,  Text*>;
   template struct TMap<int*,   int*>;
   template struct TMap<Any*,   Any*>;
   template struct TMap<RT*,    RT*>;
   template struct TMap<char*,  char*>;

   template struct TMap<Text**, Text**>;
   template struct TMap<int**,  int**>;
   template struct TMap<Any**,  Any**>;
   template struct TMap<RT**,   RT**>;
   template struct TMap<char**, char**>;

#if LANGULUS_FEATURE(MANAGED_MEMORY)
   template struct TMap<pptr8,  pptr8>;
   template struct TMap<pptr16, pptr16>;
   template struct TMap<pptr32, pptr32>;
#endif
}


TEST_CASE_TEMPLATE("Test empty Map/TMap", TestType
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
   using T  = typename TestType::First;
   using ScopedE1 = typename TestType::template At<1>;
   using ScopedE2 = typename TestType::template At<2>;
   using E1 = TypeOf<ScopedE1>;
   using E2 = TypeOf<ScopedE2>;

   [[maybe_unused]] constexpr bool Managed = ScopedE1::Managed;
   static_assert(ScopedE1::Managed == ScopedE2::Managed);

   if constexpr (CT::Untyped<T>) {
      // All type-erased containers should have all intent              
      // constructors and assigners available, and errors will instead  
      // be thrown as exceptions at runtime                             
      static_assert(Exact<TypeOf<T>, void>);
      static_assert(CT::TypeErased<T>);

      static_assert(CT::CopyConstructible<T>);
      static_assert(CT::ReferConstructible<T>);
      static_assert(CT::AbandonConstructible<T>);
      static_assert(CT::MoveConstructible<T>);
      static_assert(CT::CloneConstructible<T>);
      static_assert(CT::DisownConstructible<T>);

      static_assert(CT::CopyAssignable<T>);
      static_assert(CT::ReferAssignable<T>);
      static_assert(CT::AbandonAssignable<T>);
      static_assert(CT::MoveAssignable<T>);
      static_assert(CT::CloneAssignable<T>);
      static_assert(CT::DisownAssignable<T>);
   }
   else {
      // Statically-typed containers behave the same as their inner     
      // type                                                           
      static_assert(    Exact<TypeOf<T, 0>, E1>);
      static_assert(    Exact<TypeOf<T, 1>, E2>);
      static_assert(    Exact<typename T::Key, E1>);
      static_assert(    Exact<typename T::Val, E2>);
      static_assert(not CT::TypeErased<T>);
      static_assert(    CT::Comparable<typename T::Key, E1>);
      static_assert(    CT::Comparable<typename T::Val, E2>);

      static_assert(CT::CopyConstructible<T>    == CT::CopyConstructible<E1, E2>);
      static_assert(CT::ReferConstructible<T>   == CT::ReferConstructible<E1, E2>);
      static_assert(CT::AbandonConstructible<T> == CT::AbandonConstructible<E1, E2>);
      static_assert(CT::MoveConstructible<T>    == CT::MoveConstructible<E1, E2>);
      static_assert(CT::CloneConstructible<T>   == CT::CloneConstructible<E1, E2>);
      static_assert(CT::DisownConstructible<T>  == CT::DisownConstructible<E1, E2>);

      static_assert(CT::CopyAssignable<T>       == CT::CopyAssignable<E1, E2>);
      static_assert(CT::ReferAssignable<T>      == CT::ReferAssignable<E1, E2>);
      static_assert(CT::AbandonAssignable<T>    == CT::AbandonAssignable<E1, E2>);
      static_assert(CT::MoveAssignable<T>       == CT::MoveAssignable<E1, E2>);
      static_assert(CT::CloneAssignable<T>      == CT::CloneAssignable<E1, E2>);
      static_assert(CT::DisownAssignable<T>     == CT::DisownAssignable<E1, E2>);      
   }
   
   {
      static_assert(    CT::Deep<T>);
      static_assert(not CT::ContainsOne<T>);
      static_assert(not CT::Handle<T>);
      static_assert(    CT::ContainsMany<T>);
      static_assert(    CT::HasVariableCount<T>);
      static_assert(    CT::HeapAllocated<T>);
      static_assert(    CT::OwnedDeep<T> == (CT::TypeErased<T> or CT::Sparse<E1> or CT::Sparse<E2>));
      static_assert(    CT::Owned<T>);
      static_assert(    CT::OwnedStrong<T>);
      static_assert(    CT::Comparable<T, T>);
      static_assert(not CT::Comparable<T, E1>);
      static_assert(not CT::Comparable<T, E2>);
      static_assert(    CT::Comparable<T, Pair>);
      static_assert(    CT::Comparable<T, TPair<E1, E2>>);

      static_assert(::std::input_or_output_iterator<decltype(LglsFake(T).begin())>);
      static_assert(::std::input_or_output_iterator<decltype(LglsFake(T).end())>);

      static_assert(::std::ranges::range<T>);

      T test;
      for (auto& it : IterateInReverse(test)) {
         (void) it;
      }
      for (auto& it : test) {
         (void) it;
      }
      for (auto& it : IterateDefault(test)) {
         (void) it;
      }
      for (auto& it : IterateNoDeref(test)) {
         (void) it;
      }

      static_assert(    requires (T pack)         { pack.Get(); });
      static_assert(not requires (T pack)         { pack.template As<E1>(); });
      static_assert(not requires (T pack)         { pack.template As<E2>(); });
      //static_assert(not requires (T pack)         { pack.GetDeep(); });
      static_assert(not requires (T pack)         { pack.GetResolved(); });
      static_assert(not requires (T pack)         { pack.GetDense(); });
      static_assert(not requires (T pack)         { pack + pack; });
      static_assert(    CT::TextRange<E1> or not requires (T pack, E1 item){  pack + item; });
      static_assert(    CT::TextRange<E2> or not requires (T pack, E2 item){  pack + item; });
      static_assert(not CT::TextRange<E1> or     requires (T pack, E1 item){ {pack + item} -> CT::Text; });
      static_assert(not CT::TextRange<E2> or     requires (T pack, E2 item){ {pack + item} -> CT::Text; });
      static_assert(not requires (T pack)         { pack +=  pack; });
      static_assert(not requires (T pack, E1 item){ pack +=  item; });
      static_assert(not requires (T pack, E2 item){ pack +=  item; });
      static_assert(not requires (T pack, E1 item){ pack <<  item; });
      static_assert(not requires (T pack, E2 item){ pack <<  item; });
      static_assert(not requires (T pack, E1 item){ pack >>  item; });
      static_assert(not requires (T pack, E2 item){ pack >>  item; });
      static_assert(not requires (T pack, E1 item){ pack <<= item; });
      static_assert(not requires (T pack, E2 item){ pack <<= item; });
      static_assert(not requires (T pack, E1 item){ pack >>= item; });
      static_assert(not requires (T pack, E2 item){ pack >>= item; });
      static_assert(not requires (T pack, E1 item){ pack.InsertAt(Index::Back, item); });
      static_assert(not requires (T pack, E2 item){ pack.InsertAt(Index::Back, item); });
      static_assert(not requires (T pack, E1 item){ pack.EmplaceAt(Index::Back, item); });
      static_assert(not requires (T pack, E2 item){ pack.EmplaceAt(Index::Back, item); });
      static_assert(not requires (T pack)         { pack.ConcatAt(Index::Back, pack); });
      static_assert(not requires (T pack)         { pack.Concat(pack); });
      static_assert(not requires (T pack, E1 item){ pack.MergeAt(Index::Back, item); });
      static_assert(not requires (T pack, E2 item){ pack.MergeAt(Index::Back, item); });
      static_assert(not requires (T pack)         { pack.MergeRangeAt(Index::Back, pack); });
      static_assert(    requires (T pack, E1 item){ pack.Merge(item); });
      static_assert(    requires (T pack)         { pack.MergeRange(pack); });
      static_assert(    requires (T pack, E1 item){ pack.Erase(item); });
      static_assert(not requires (T pack)         { pack.EraseAt(Index::Front); });
      static_assert(    requires (T pack)         { pack.Reserve(20); });
      static_assert(not requires (T pack)         { pack.EnableOr(); });
      static_assert(not requires (T pack)         { pack.IsOr(); });
      static_assert(    requires (T pack, E1 item){ pack.Find(item); });
      static_assert(    requires (T pack)         { pack.ForEach([](const int&) {}); });
      static_assert(    requires (T pack)         { pack.ForEachRev([](const int&) {}); });
   }

   constexpr bool Ambiguous = LANGULUS(SAFE) and ((not Same<T, E1> and CT::Map<E1>)
                                               or (not Same<T, E2> and CT::Map<E2>));

   #if LANGULUS(BENCHMARK)
      using stdmap = ::std::unordered_map<E1, E2>;
   #endif
   
   GIVEN("Empty-constructed container, assigned (refer), and then destroyed") {
      const ScopedE1 element1{555};
      const ScopedE2 element2{111};
      T pack;
      pack.Assign(*element1, *element2);
   }

   GIVEN("Empty-constructed container, assigned (refer using intent), and then destroyed") {
      const ScopedE1 element1{555};
      const ScopedE2 element2{111};
      T pack;
      pack.Assign(Refer(*element1), Refer(*element2));
   }

   GIVEN("Empty-constructed container, assigned (copied), and then destroyed") {
      const ScopedE1 element1{555};
      const ScopedE2 element2{111};
      T pack;
      pack.Assign(Copy(*element1), Copy(*element2));
   }

   GIVEN("Empty-constructed container, assigned (cloned), and then destroyed") {
      const ScopedE1 element1{555};
      const ScopedE2 element2{111};
      T pack;
      pack.Assign(Clone(*element1), Clone(*element2));
   }

   GIVEN("Empty-constructed container, assigned (move), and then destroyed") {
      const ScopedE1 element1{555};
      const ScopedE2 element2{111};
      T pack;
      pack.Assign(::std::move(*element1), ::std::move(*element2));
   }

   GIVEN("Empty-constructed container, assigned (move using intent), and then destroyed") {
      ScopedE1 element1{555};
      ScopedE2 element2{111};
      T pack;
      pack.Assign(Move(*element1), Move(*element2));
   }

   GIVEN("Empty-constructed container, assigned (abandon), and then destroyed") {
      ScopedE1 element1{555};
      ScopedE2 element2{111};
      T pack;
      pack.Assign(Abandon(*element1), Abandon(*element2));
   }

   GIVEN("Empty-constructed container, assigned (disown), and then destroyed") {
      const ScopedE1 element1{555};
      const ScopedE2 element2{111};
      T pack;
      pack.Assign(Disown(*element1), Disown(*element2));
   }

   GIVEN("Default-constructed container") {
      const ScopedE1 element1 {555};
      const ScopedE2 element2 {111};
      T pack;

      WHEN("Gap test") {
         Common_GapTest<T, ::std::unordered_map<E1, E2>>();
         //static_assert(sizeof(T) <= sizeof(::std::unordered_map<E1, E2>)); //TODO not true on 32bit builds unfortunately
      }

      WHEN("Default-constructed") {
         Map_CheckState_Default<E1, E2>(pack);
      
         if constexpr (Ambiguous) {
            WHEN("Ambiguous assign value by referral") {
               REQUIRE_THROWS(pack = {*element1, *element2});
               REQUIRE_THROWS(pack = Refer({*element1, *element2}));
            }
         }

         BenchmarkMapStd("Empty/DefaultConstructor", 30, 40,
            T temp,              new (&temp)     T{},
            stdmap temp_std,     new (&temp_std) stdmap{}
         );
      }

      WHEN("Assigned value by referral") {
         pack.Assign(*element1, *element2);

         Map_CheckState_OwnedFull<E1, E2>(pack);
         Map_CheckState_ContainsOne(pack, Refer(element1), Refer(element2));

         BenchmarkMapStd("Empty/Assign/Refer", 30, 100,
            T temp,              temp.Assign(*element1, *element2),
            stdmap temp_std,     temp_std.emplace(*element1, *element2)
         );
      }

      /*if constexpr (CT::Map<E1>) {
         WHEN("Assigned and absorbed by referral") {
            if (CT::Typed<T> and not pack.IsSame(element->GetType())) {
               const auto element_backup = *element;
               REQUIRE_THROWS(pack.AssignAbsorb(*element));
               Map_CheckState_Default<E1, E2>(pack);
               Map_Helper_TestSame(element_backup, *element);
               return;
            }

            pack.AssignAbsorb(*element);

            Map_Helper_TestSame(pack, *element);
            REQUIRE(pack.GetUses() == element->GetUses());
            REQUIRE(pack.GetUses() == 2);
            REQUIRE(pack.GetAllocation() == element->GetAllocation());

            [[maybe_unused]] stdmap src_std {1, *element};
            BenchmarkMapStd("Empty/AssignAbsorb(Refer(" + NameOf<E>() + "))", 30, 100,
               T temp,              temp.AssignAbsorb(*element),
               stdmap temp_std,     temp_std = src_std;
            );
         }
      }

      if constexpr (Ambiguous) {
         WHEN("Ambiguous assign value by move") {
            auto movable = *element;
            REQUIRE_THROWS(pack = ::std::move(movable));
            REQUIRE_THROWS(pack = Move(movable));
         }
      }*/
      
      WHEN("Assigned value by move") {
         auto movable1 = *element1;
         auto movable2 = *element2;
         pack.Assign(::std::move(movable1), ::std::move(movable2));
         
         if constexpr (CT::DeepDense<E1>)
            Any_CheckState_Default<TypeOf<E1>>(movable1);
         if constexpr (CT::DeepDense<E2>)
            Any_CheckState_Default<TypeOf<E2>>(movable2);

         Map_CheckState_OwnedFull<E1, E2>(pack);
         Map_CheckState_ContainsOne(pack, Refer(element1), Refer(element2));

         BenchmarkMapStd("Empty/Assign/Move", 30, 100,
            auto movable1 = *element1;
            auto movable2 = *element2;
            T temp,                       temp.Assign(::std::move(movable1), ::std::move(movable2)),
            auto movable1 = *element1;
            auto movable2 = *element2;
            stdmap temp_std,              temp_std.emplace(::std::move(movable1), ::std::move(movable2))
         );
      }

      /*if constexpr (CT::Set<E>) {
         WHEN("Assigned and absorbed by move") {
            auto movable = *element;

            if (CT::Typed<T> and not pack.IsSame(element->GetType())) {
               REQUIRE_THROWS(pack.AssignAbsorb(::std::move(movable)));
               Set_CheckState_Default<E>(pack);
               Set_Helper_TestSame(movable, *element);
               return;
            }

            pack.AssignAbsorb(::std::move(movable));

            Set_CheckState_Default<TypeOf<E>>(movable);
            Set_Helper_TestSame(pack, *element);
            REQUIRE(pack.GetUses() == element->GetUses());
            REQUIRE(pack.GetUses() == 2);
            REQUIRE(pack.GetAllocation() == element->GetAllocation());

            BenchmarkMapStd("Empty/AssignAbsorb(Move(" + NameOf<E>() + "))", 30, 100,
               auto movable = *element;
               T temp,                    temp.AssignAbsorb(::std::move(movable)),
               stdmap movable (1, 555);
               stdmap temp_std,           temp_std.emplace(::std::move(movable))
            );
         }
      }

      if constexpr (Ambiguous) {
         WHEN("Ambiguous assign copied value") {
            REQUIRE_THROWS(pack = Copy(*element));
         }
      }*/
      
      WHEN("Assigned value by copy") {
         pack.Assign(Copy(*element1), Copy(*element2));

         Map_CheckState_OwnedFull<E1, E2>(pack);
         Map_CheckState_ContainsOne(pack, Copy(element1), Copy(element2));

         BenchmarkMapStd("Empty/Assign/Copy", 30, 100,
            T temp,              temp.Assign(Copy(*element1), Copy(*element2)),
            stdmap temp_std,     temp_std.emplace(*element1, *element2)
         );
      }

      /*if constexpr (CT::Set<E>) {
         WHEN("Assigned and absorbed copied value") {
            if (CT::Typed<T> and not pack.IsSame(element->GetType())) {
               const auto element_backup = *element;
               REQUIRE_THROWS(pack.AssignAbsorb(Copy(*element)));
               Set_CheckState_Default<E>(pack);
               Set_Helper_TestSame(element_backup, *element);
               return;
            }

            pack.AssignAbsorb(Copy(*element));

            REQUIRE(pack.GetRaw() != element->GetRaw());
            REQUIRE(pack.IsExact(element->GetType()));
            REQUIRE(pack == *element);
            REQUIRE(pack.IsDeep() == element->IsDeep());
            REQUIRE_FALSE(pack.IsConstant());
            REQUIRE(pack.GetUnconstrainedState() == element->GetUnconstrainedState());

            REQUIRE(pack.GetUses() == 1);
            REQUIRE(pack.GetAllocation());

            [[maybe_unused]] ::std::vector<E> src_std (1, *element);
            BenchmarkMapStd("Empty/AssignAbsorb(Copy(" + NameOf<E>() + "))", 30, 100,
               T temp,              temp.AssignAbsorb(Copy(*element)),
               stdmap temp_std,     temp_std = src_std
            );
         }
      }

      if constexpr (Ambiguous) {
         WHEN("Ambiguous assign cloned value") {
            REQUIRE_THROWS(pack = Clone(*element));
         }
      }*/
      
      WHEN("Assigned value by clone") {
         pack.Assign(Clone(*element1), Clone(*element2));

         Map_CheckState_OwnedFull<E1, E2>(pack);
         Map_CheckState_ContainsOne(pack, Clone(element1), Clone(element2));

         BenchmarkMapStd("Empty/Assign/Clone", 30, 100,
            T temp,              temp.Assign(Clone(*element1), Clone(*element2)),
            stdmap temp_std,     temp_std.emplace(*element1, *element2)
         );
      }

      /*if constexpr (CT::Set<E>) {
         WHEN("Assigned and absorbed cloned value") {
            if (CT::Typed<T> and not pack.IsSame(element->GetType())) {
               const auto element_backup = *element;
               REQUIRE_THROWS(pack.AssignAbsorb(Clone(*element)));
               Set_CheckState_Default<E>(pack);
               Set_Helper_TestSame(element_backup, *element);
               return;
            }

            pack.AssignAbsorb(Clone(*element));

            REQUIRE(pack.GetRaw() != element->GetRaw());
            REQUIRE(pack.IsExact(element->GetType()));
            REQUIRE(pack == *element);
            REQUIRE(pack.IsDeep() == element->IsDeep());
            REQUIRE_FALSE(pack.IsConstant());
            REQUIRE(pack.GetUnconstrainedState() == element->GetUnconstrainedState());
            REQUIRE(pack.GetUses() == 1);
            REQUIRE(pack.GetAllocation());

            [[maybe_unused]] stdmap src_std ({*element});
            BenchmarkMapStd("Empty/AssignAbsorb(Clone(" + NameOf<E>() + "))", 30, 100,
               T temp,              temp.AssignAbsorb(Clone(*element)),
               stdmap temp_std,     temp_std = src_std
            );
         }
      }

      if constexpr (Ambiguous) {
         WHEN("Ambiguous assign disowned value") {
            REQUIRE_THROWS(pack = Disown(*element));
         }
      }*/
      
      WHEN("Assigned value by disown") {
         pack.Assign(Disown(*element1), Disown(*element2));

         Map_CheckState_OwnedFull<E1, E2>(pack);
         Map_CheckState_ContainsOne(pack, Disown(element1), Disown(element2));

         BenchmarkMapStd("Empty/Assign/Disown", 30, 100,
            T temp,              temp.Assign(Disown(*element1), Disown(*element2)),
            stdmap temp_std,     temp_std.emplace(*element1, *element2)
         );
      }

      /*if constexpr (CT::Set<E>) {
         WHEN("Assigned and absorbed disowned value") {
            if (CT::Typed<T> and not pack.IsSame(element->GetType())) {
               const auto element_backup = *element;
               REQUIRE_THROWS(pack.AssignAbsorb(Disown(*element)));
               Set_CheckState_Default<E>(pack);
               Set_Helper_TestSame(element_backup, *element);
               return;
            }

            pack.AssignAbsorb(Disown(*element));

            REQUIRE(pack.GetRaw() == element->GetRaw());
            REQUIRE(pack.IsExact(element->GetType()));
            REQUIRE(pack == *element);
            REQUIRE(pack.IsDeep() == element->IsDeep());
            REQUIRE(pack.IsConstant() != element->IsConstant());
            REQUIRE(pack.GetUnconstrainedState() == element->GetUnconstrainedState());
            REQUIRE(pack.GetUses() == 0);
            REQUIRE_FALSE(pack.GetAllocation());

            [[maybe_unused]] stdmap src_std (1, *element);
            BenchmarkMapStd("Empty/AssignAbsorb(Disown(" + NameOf<E>() + "))", 30, 100,
               T temp,              temp.AssignAbsorb(Disown(*element)),
               stdmap temp_std,     temp_std = src_std
            );
         }
      }

      if constexpr (Ambiguous) {
         WHEN("Ambiguous assigned abandoned value") {
            auto movable = *element;
            REQUIRE_THROWS(pack = Abandon(movable));
         }
      }*/
      
      WHEN("Assigned value by abandon") {
         auto movable1 = *element1;
         auto movable2 = *element2;
         pack.Assign(Abandon(movable1), Abandon(movable2));

         if constexpr (CT::DeepDense<E1>)
            Any_CheckState_Abandoned<TypeOf<E1>>(movable1);
         if constexpr (CT::DeepDense<E2>)
            Any_CheckState_Abandoned<TypeOf<E2>>(movable2);

         Map_CheckState_OwnedFull<E1, E2>(pack);
         Map_CheckState_ContainsOne(pack, Refer(element1), Refer(element2));

         BenchmarkMapStd("Empty/Assign/Abandon", 30, 100,
            auto movable = *element;
            T temp,                    temp.Assign(Abandon(movable1), Abandon(movable2)),
            auto movable = *element;
            stdmap temp_std,           temp_std.emplace(::std::move(movable1), ::std::move(movable2))
         );
      }

      /*if constexpr (CT::Set<E>) {
         WHEN("Assigned and absorbed abandoned value") {
            auto movable = *element;

            if (CT::Typed<T> and not pack.IsSame(element->GetType())) {
               REQUIRE_THROWS(pack.AssignAbsorb(Abandon(movable)));
               Set_CheckState_Default<E>(pack);
               Set_Helper_TestSame(movable, *element);
               return;
            }

            pack.AssignAbsorb(Abandon(movable));

            Set_CheckState_Abandoned<E>(movable);
            Set_Helper_TestSame(pack, *element);
            REQUIRE(pack.GetUses() == 2);
            REQUIRE(pack.GetAllocation() == element->GetAllocation());

            BenchmarkMapStd("Empty/AssignAbsorb(Abandon(" + NameOf<E>() + "))", 30, 100,
               auto movable = *element;
               T temp,                    temp.AssignAbsorb(Abandon(movable)),
               stdmap movable (1, 555);
               stdmap temp_std,            temp_std = ::std::move(movable)
            );
         }
      }*/

      WHEN("Ambigous assigned empty self") {
         LglsDisableWarningPush
         LglsDisableWarning_SelfAssign
         REQUIRE_NOTHROW(pack = pack);
         LglsDisableWarningPop
      }
      
      WHEN("Assigned empty self") {
         pack.AssignAbsorb(pack);

         Map_CheckState_Default<E1, E2>(pack);
      }

      /*WHEN("Emplace (insert)") {
         ScopedE i666 {666};
         const auto i666backup = *i666;
         decltype(auto) instance = pack.Emplace(::std::move(*i666));
         Set_CheckState_OwnedFull<E>(pack);
         if constexpr (CT::Handle<decltype(instance)>)
            REQUIRE(instance.CompareOneEqual(i666backup));
         else
            REQUIRE(instance == i666backup);
         REQUIRE(pack.GetCount() == 1);
         REQUIRE(pack.GetReserved() >= 1);

         if constexpr (CT::Typed<T>) {
            REQUIRE(*pack == i666backup);
            if constexpr (CT::Handle<decltype(instance)>)
               REQUIRE(&*pack == &*instance);
            else
               REQUIRE(&*pack == &instance);
         }

         BenchmarkSet("Empty/Emplace(" + NameOf<E>() + ")", 30,
            auto movable = *element; T temp,
            temp.Emplace(::std::move(movable))
         );
      }*/

      /*WHEN("Emplace (insert, describe)") {
         ScopedE i666{666};
         const auto i666backup = *i666;
         Neat descriptor {::std::move(*i666)};
         if constexpr (CT::DescribeConstructible<E> and not CT::Container<T>) {
            decltype(auto) instance = pack.template Emplace<E>(Describe{descriptor});
            Set_CheckState_OwnedFull<E>(pack);
            REQUIRE(instance.CompareOneEqual(i666backup));
            REQUIRE(pack.GetCount() == 1);
            REQUIRE(pack.GetReserved() >= 1);

            BenchmarkSet("Empty/Emplace(Describe(" + NameOf<E>() + "))", 30,
               T temp,
               temp.Emplace(Describe{descriptor})
            );
         }
         else if constexpr (CT::TypeErased<T>) {
            pack.template SetType<E>();
            REQUIRE_THROWS(pack.Emplace(Describe{descriptor}));
            Set_CheckState_Default<E>(pack, true);
         }
      }*/

      WHEN("Cleared") {
         pack.Clear();

         Map_CheckState_Default<E1, E2>(pack);

         BenchmarkMapStd("Empty/Clear", 30, 100,
            T temp,              temp.Clear(),
            stdmap temp_std,     temp_std.clear()
         );
      }

      WHEN("Reset") {
         pack.Reset();

         Map_CheckState_Default<E1, E2>(pack);

         BenchmarkMapStd("Empty/Reset", 30, 100,
            T temp,              temp.Reset(),
            stdmap temp_std,     temp_std.clear()
         );
      }

      WHEN("Referred empty") {
         T refer1 = pack;

         Map_Helper_TestSame(refer1, pack);
         Map_CheckState_Default<E1, E2>(refer1);
         Map_CheckState_Default<E1, E2>(pack);

         T refer2 = Refer(pack);

         Map_Helper_TestSame(refer2, pack);
         Map_CheckState_Default<E1, E2>(refer2);
         Map_CheckState_Default<E1, E2>(pack);
      }

      WHEN("Cloned empty") {
         T clone = Clone(pack);

         Map_Helper_TestSame(clone, pack);
         Map_CheckState_Default<E1, E2>(clone);
         Map_CheckState_Default<E1, E2>(pack);
      }

      WHEN("Disowned empty") {
         T disowned = Disown(pack);

         Map_Helper_TestSame(disowned, pack);
         Map_CheckState_Default<E1, E2>(disowned);
         Map_CheckState_Default<E1, E2>(pack);
      }

      WHEN("Copied empty") {
         T copy = Copy(pack);

         Map_Helper_TestSame(copy, pack);
         Map_CheckState_Default<E1, E2>(copy);
         Map_CheckState_Default<E1, E2>(pack);
      }

      WHEN("Moved empty") {
         T movable1 = pack;
         const T moved1 = ::std::move(movable1);

         Map_CheckState_Default<E1, E2>(movable1);
         Map_Helper_TestSame(moved1, pack);
         Map_CheckState_Default<E1, E2>(moved1);
         Map_CheckState_Default<E1, E2>(pack);

         T movable2 = pack;
         const T moved2 = Move(movable2);

         Map_CheckState_Default<E1, E2>(movable2);
         Map_Helper_TestSame(moved2, pack);
         Map_CheckState_Default<E1, E2>(moved2);
         Map_CheckState_Default<E1, E2>(pack);
      }

      WHEN("Abandoned empty") {
         T movable = pack;
         const T moved = Abandon(movable);

         Map_CheckState_Default<E1, E2>(movable);
         Map_Helper_TestSame(moved, pack);
         Map_CheckState_Default<E1, E2>(moved);
         Map_CheckState_Default<E1, E2>(pack);
      }

      WHEN("Compared empty") {
         T another_pack1;
         T another_pack2;

         REQUIRE      (another_pack1 == another_pack2);
         REQUIRE_FALSE(another_pack1 != another_pack2);
         static_assert(     T{} == T{} );
         static_assert(not (T{} != T{}));

         [[maybe_unused]] volatile bool dont_optimize = false;
         BenchmarkMap("Empty/operator==", 30,
            (void) 0, dont_optimize |= (another_pack1 == another_pack2)
         );
         BenchmarkMap("Empty/operator!=", 30,
            (void) 0, dont_optimize |= (another_pack1 != another_pack2)
         );
      }

      WHEN("Contains when empty") {
         REQUIRE_FALSE(pack.Contains(*element1));

         [[maybe_unused]] volatile bool dont_optimize = false;
         BenchmarkMap("Empty/Contains", 30,
            (void) 0, dont_optimize |= pack.Contains(*element1)
         );
      }

      if constexpr (Exact<E1, Text>) {
         WHEN("Given text key that will be destroyed before the pack") {
            Text owned_text = "666";
            pack = TPair {Text(owned_text.operator Token()), *element2};
         }
      }

      if constexpr (Exact<E2, Text>) {
         WHEN("Given text value that will be destroyed before the pack") {
            Text owned_text = "666";
            pack = TPair {*element1, Text(owned_text.operator Token())};
         }
      }

      WHEN("GetHandle is called on mutable container") {
         auto h = pack.GetHandle();

         if constexpr (CT::Untyped<T>)
            static_assert(::std::same_as<decltype(h), THandlePair<Handle, HandleMut>>);
         else
            static_assert(::std::same_as<decltype(h), THandlePair<THandle<ConstAll<E1&>>, THandle<E2&>>>);

         HandlePair_CheckState_Default<E1 const, E2>(h);
      }

      WHEN("GetHandle is called on constant container") {
         T const pack_constant;
         auto h = pack_constant.GetHandle();

         if constexpr (CT::Untyped<T>)
            static_assert(::std::same_as<decltype(h), THandlePair<Handle, Handle>>);
         else
            static_assert(::std::same_as<decltype(h), THandlePair<THandle<ConstAll<E1&>>, THandle<ConstAll<E2&>>>>);

         HandlePair_CheckState_Default<E1 const, E2 const>(h);
      }
   }

   REQUIRE(memoryState.Assert());
   REQUIRE_FALSE(Allocator::CollectGarbage());
}