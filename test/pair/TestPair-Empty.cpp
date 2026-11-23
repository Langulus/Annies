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
   // Explicit instantiation for using extern templates in other tests  
   template struct TPair<Text,   Text>;
   template struct TPair<int,    int>;
   template struct TPair<Any,    Any>;
   template struct TPair<RT,     RT>;
   template struct TPair<char,   char>;

   template struct TPair<Text*,  Text*>;
   template struct TPair<int*,   int*>;
   template struct TPair<Any*,   Any*>;
   template struct TPair<RT*,    RT*>;
   template struct TPair<char*,  char*>;

   template struct TPair<Text**, Text**>;
   template struct TPair<int**,  int**>;
   template struct TPair<Any**,  Any**>;
   template struct TPair<RT**,   RT**>;
   template struct TPair<char**, char**>;

#if LANGULUS_FEATURE(MANAGED_MEMORY)
   template struct TPair<pptr8,  pptr8>;
   template struct TPair<pptr16, pptr16>;
   template struct TPair<pptr32, pptr32>;
#endif
}


TEST_CASE_TEMPLATE("Test empty Pair/TPair", TestType
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
      static_assert(    CT::ContainsOne<T>);
      static_assert(not CT::Handle<T>);
      static_assert(not CT::ContainsMany<T>);
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
      static_assert(not ::std::ranges::range<T>);

      static_assert(    requires (T pack)         { pack.Get(); });
      static_assert(    requires (T pack)         { pack.template As<E1, 0>(); });
      static_assert(    requires (T pack)         { pack.template As<E2, 1>(); });
      //static_assert(not requires (T pack)         { pack.GetDeep(); });
      static_assert(    requires (T pack)         { pack.template GetResolved<0>(); });
      static_assert(    requires (T pack)         { pack.template GetResolved<1>(); });
      static_assert(    requires (T pack)         { pack.template GetDense<0>(); });
      static_assert(    requires (T pack)         { pack.template GetDense<1>(); });
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
      static_assert(not requires (T pack, E1 item){ pack.Merge(item); });
      static_assert(not requires (T pack)         { pack.MergeRange(pack); });
      static_assert(not requires (T pack, E1 item){ pack.Erase(item); });
      static_assert(not requires (T pack)         { pack.EraseAt(Index::Front); });
      static_assert(not requires (T pack)         { pack.Reserve(20); });
      static_assert(not requires (T pack)         { pack.EnableOr(); });
      static_assert(not requires (T pack)         { pack.IsOr(); });
      static_assert(not requires (T pack, E1 item){ pack.Find(item); });
      static_assert(not requires (T pack)         { pack.ForEach([](const int&) {}); });
      static_assert(not requires (T pack)         { pack.ForEachRev([](const int&) {}); });
   }

   constexpr bool Ambiguous = LANGULUS(SAFE) and ((not Same<T, E1> and CT::Pair<E1>)
                                               or (not Same<T, E2> and CT::Pair<E2>));

   #if LANGULUS(BENCHMARK)
      using stdpair = ::std::pair<E1, E2>;
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
         Common_GapTest<T, ::std::pair<E1, E2>>();
         //static_assert(sizeof(T) <= sizeof(::std::pair<E1, E2>)); //TODO not true on 32bit builds unfortunately
      }

      WHEN("Default-constructed") {
         Pair_CheckState_Default<E1, E2>(pack);
      
         if constexpr (Ambiguous) {
            WHEN("Ambiguous assign value by referral") {
               REQUIRE_THROWS(pack = {*element1, *element2});
               REQUIRE_THROWS(pack = Refer({*element1, *element2}));
            }
         }

         BenchmarkPairStd("Empty/DefaultConstructor", 30, 40,
            T temp,              new (&temp)     T{},
            stdpair temp_std,    new (&temp_std) stdpair{}
         );
      }

      WHEN("Assigned value by referral") {
         pack.Assign(*element1, *element2);

         Pair_CheckState_OwnedFull<E1, E2>(pack);
         Pair_CheckState_ContainsOne(pack, Refer(element1), Refer(element2));

         BenchmarkPairStd("Empty/Assign/Refer", 30, 100,
            T temp,              temp.Assign(*element1, *element2),
            stdpair temp_std,    temp_std.emplace(*element1, *element2)
         );
      }

      if constexpr (CT::Pair<E1>) { //TODO not tested yet
         WHEN("Assigned and absorbed by referral") {
            if (CT::Typed<T> and not pack.IsSame(element1->GetType())) {
               const auto element_backup = *element1;
               REQUIRE_THROWS(pack.AssignAbsorb(*element1));
               Pair_CheckState_Default<E1, E2>(pack);
               Pair_Helper_TestSame(element_backup, *element1);
               return;
            }

            pack.AssignAbsorb(*element1);

            Pair_Helper_TestSame(pack, *element1);
            REQUIRE(pack.GetUses() == element1->GetUses());
            REQUIRE(pack.GetUses() == 2);
            REQUIRE(pack.GetAllocation() == element1->GetAllocation());

            BenchmarkPairStd("Empty/AssignAbsorb/Refer", 30, 100,
               T temp,                             temp.AssignAbsorb(*element),
               stdpair temp_std;
               stdpair src_std (1, *element1),     temp_std = src_std;
            );
         }
      }

      if constexpr (Ambiguous) {
         WHEN("Ambiguous assign value by move") {
            auto movable = *element1;
            REQUIRE_THROWS(pack = ::std::move(movable));
            REQUIRE_THROWS(pack = Move(movable));
         }
      }
      
      WHEN("Assigned value by move") {
         auto movable1 = *element1;
         auto movable2 = *element2;
         pack.Assign(::std::move(movable1), ::std::move(movable2));
         
         if constexpr (CT::DeepDense<E1>)
            Any_CheckState_Default<TypeOf<E1>>(movable1);
         if constexpr (CT::DeepDense<E2>)
            Any_CheckState_Default<TypeOf<E2>>(movable2);

         Pair_CheckState_OwnedFull<E1, E2>(pack);
         Pair_CheckState_ContainsOne(pack, Refer(element1), Refer(element2));

         BenchmarkPairStd("Empty/Assign/Move", 30, 100,
            auto movable1 = *element1;
            auto movable2 = *element2;
            T temp,                       temp.Assign(::std::move(movable1), ::std::move(movable2)),
            auto movable1 = *element1;
            auto movable2 = *element2;
            stdpair temp_std,             temp_std.emplace(::std::move(movable1), ::std::move(movable2))
         );
      }

      if constexpr (CT::Pair<E1>) {
         WHEN("Assigned and absorbed by move") {
            auto movable = *element1;

            if (CT::Typed<T> and not pack.IsSame(element1->GetType())) {
               REQUIRE_THROWS(pack.AssignAbsorb(::std::move(movable)));
               Pair_CheckState_Default<E1, E2>(pack);
               Pair_Helper_TestSame(movable, *element1);
               return;
            }

            pack.AssignAbsorb(::std::move(movable));

            Pair_CheckState_Default<int, int>(movable);
            Pair_Helper_TestSame(pack, *element1);
            REQUIRE(pack.GetUses() == element1->GetUses());
            REQUIRE(pack.GetUses() == 2);
            REQUIRE(pack.GetAllocation() == element1->GetAllocation());

            BenchmarkPairStd("Empty/AssignAbsorb/Move", 30, 100,
               auto movable = *element;
               T temp,                     temp.AssignAbsorb(::std::move(movable)),
               stdpair movable (1, 555);
               stdpair temp_std,           temp_std.emplace(::std::move(movable))
            );
         }
      }

      if constexpr (Ambiguous) {
         WHEN("Ambiguous assign copied value") {
            REQUIRE_THROWS(pack = Copy(*element1));
         }
      }
      
      WHEN("Assigned value by copy") {
         pack.Assign(Copy(*element1), Copy(*element2));

         Pair_CheckState_OwnedFull<E1, E2>(pack);
         Pair_CheckState_ContainsOne(pack, Copy(element1), Copy(element2));

         BenchmarkPairStd("Empty/Assign/Copy", 30, 100,
            T temp,              temp.Assign(Copy(*element1), Copy(*element2)),
            stdpair temp_std,    temp_std.emplace(*element1, *element2)
         );
      }

      if constexpr (CT::Pair<E1>) {
         WHEN("Assigned and absorbed copied value") {
            if (CT::Typed<T> and not pack.IsSame(element1->GetType())) {
               const auto element_backup = *element1;
               REQUIRE_THROWS(pack.AssignAbsorb(Copy(*element1)));
               Pair_CheckState_Default<E1, E2>(pack);
               Pair_Helper_TestSame(element_backup, *element1);
               return;
            }

            pack.AssignAbsorb(Copy(*element1));

            REQUIRE(pack.GetRaw() != element1->GetRaw());
            REQUIRE(pack.IsExact(element1->GetType()));
            REQUIRE(pack == *element1);
            REQUIRE(pack.IsDeep() == element1->IsDeep());
            REQUIRE_FALSE(pack.IsConstant());
            REQUIRE(pack.GetUnconstrainedState() == element1->GetUnconstrainedState());

            REQUIRE(pack.GetUses() == 1);
            REQUIRE(pack.GetAllocation());

            BenchmarkPairStd("Empty/AssignAbsorb/Copy", 30, 100,
               T temp,                             temp.AssignAbsorb(Copy(*element)),
               stdpair temp_std;
               stdpair src_std (1, *element1),     temp_std = src_std
            );
         }
      }

      if constexpr (Ambiguous) {
         WHEN("Ambiguous assign cloned value") {
            REQUIRE_THROWS(pack = Clone(*element1));
         }
      }
      
      WHEN("Assigned value by clone") {
         pack.Assign(Clone(*element1), Clone(*element2));

         Pair_CheckState_OwnedFull<E1, E2>(pack);
         Pair_CheckState_ContainsOne(pack, Clone(element1), Clone(element2));

         BenchmarkPairStd("Empty/Assign/Clone", 30, 100,
            T temp,              temp.Assign(Clone(*element1), Clone(*element2)),
            stdpair temp_std,    temp_std.emplace(*element1, *element2)
         );
      }

      if constexpr (CT::Pair<E1>) {
         WHEN("Assigned and absorbed cloned value") {
            if (CT::Typed<T> and not pack.IsSame(element1->GetType())) {
               const auto element_backup = *element1;
               REQUIRE_THROWS(pack.AssignAbsorb(Clone(*element1)));
               Pair_CheckState_Default<E1>(pack);
               Pair_Helper_TestSame(element_backup, *element1);
               return;
            }

            pack.AssignAbsorb(Clone(*element1));

            REQUIRE(pack.GetRaw() != element1->GetRaw());
            REQUIRE(pack.IsExact(element1->GetType()));
            REQUIRE(pack == *element1);
            REQUIRE(pack.IsDeep() == element1->IsDeep());
            REQUIRE_FALSE(pack.IsConstant());
            REQUIRE(pack.GetUnconstrainedState() == element1->GetUnconstrainedState());
            REQUIRE(pack.GetUses() == 1);
            REQUIRE(pack.GetAllocation());

            BenchmarkPairStd("Empty/AssignAbsorb/Clone", 30, 100,
               T temp,                        temp.AssignAbsorb(Clone(*element1)),
               stdpair temp_std;
               stdpair src_std (*element1),   temp_std = src_std
            );
         }
      }

      if constexpr (Ambiguous) {
         WHEN("Ambiguous assign disowned value") {
            REQUIRE_THROWS(pack = Disown(*element1));
         }
      }
      
      WHEN("Assigned value by disown") {
         pack.Assign(Disown(*element1), Disown(*element2));

         Pair_CheckState_OwnedFull<E1, E2>(pack);
         Pair_CheckState_ContainsOne(pack, Disown(element1), Disown(element2));

         BenchmarkPairStd("Empty/Assign/Disown", 30, 100,
            T temp,              temp.Assign(Disown(*element1), Disown(*element2)),
            stdpair temp_std,    temp_std.emplace(*element1, *element2)
         );
      }

      if constexpr (CT::Pair<E1>) {
         WHEN("Assigned and absorbed disowned value") {
            if (CT::Typed<T> and not pack.IsSame(element1->GetType())) {
               const auto element_backup = *element1;
               REQUIRE_THROWS(pack.AssignAbsorb(Disown(*element1)));
               Set_CheckState_Default<E1, E2>(pack);
               Set_Helper_TestSame(element_backup, *element1);
               return;
            }

            pack.AssignAbsorb(Disown(*element1));

            REQUIRE(pack.GetRaw() == element1->GetRaw());
            REQUIRE(pack.IsExact(element1->GetType()));
            REQUIRE(pack == *element1);
            REQUIRE(pack.IsDeep() == element1->IsDeep());
            REQUIRE(pack.IsConstant() != element1->IsConstant());
            REQUIRE(pack.GetUnconstrainedState() == element1->GetUnconstrainedState());
            REQUIRE(pack.GetUses() == 0);
            REQUIRE_FALSE(pack.GetAllocation());

            BenchmarkPairStd("Empty/AssignAbsorb/Disown", 30, 100,
               T temp,                             temp.AssignAbsorb(Disown(*element1)),
               stdpair temp_std;
               stdpair src_std (1, *element1),     temp_std = src_std
            );
         }
      }

      if constexpr (Ambiguous) {
         WHEN("Ambiguous assigned abandoned value") {
            auto movable = *element1;
            REQUIRE_THROWS(pack = Abandon(movable));
         }
      }
      
      WHEN("Assigned value by abandon") {
         auto movable1 = *element1;
         auto movable2 = *element2;
         pack.Assign(Abandon(movable1), Abandon(movable2));

         if constexpr (CT::DeepDense<E1>)
            Any_CheckState_Abandoned<TypeOf<E1>>(movable1);
         if constexpr (CT::DeepDense<E2>)
            Any_CheckState_Abandoned<TypeOf<E2>>(movable2);

         Pair_CheckState_OwnedFull<E1, E2>(pack);
         Pair_CheckState_ContainsOne(pack, Refer(element1), Refer(element2));

         BenchmarkPairStd("Empty/Assign/Abandon", 30, 100,
            auto movable = *element;
            T temp,                    temp.Assign(Abandon(movable1), Abandon(movable2)),
            auto movable = *element;
            stdpair temp_std,          temp_std.emplace(::std::move(movable1), ::std::move(movable2))
         );
      }

      if constexpr (CT::Pair<E1>) {
         WHEN("Assigned and absorbed abandoned value") {
            auto movable = *element1;

            if (CT::Typed<T> and not pack.IsSame(element1->GetType())) {
               REQUIRE_THROWS(pack.AssignAbsorb(Abandon(movable)));
               Pair_CheckState_Default<E1, E2>(pack);
               Pair_Helper_TestSame(movable, *element1);
               return;
            }

            pack.AssignAbsorb(Abandon(movable));

            Pair_CheckState_Abandoned<E1, E2>(movable);
            Pair_Helper_TestSame(pack, *element1);
            REQUIRE(pack.GetUses() == 2);
            REQUIRE(pack.GetAllocation() == element1->GetAllocation());

            BenchmarkPairStd("Empty/AssignAbsorb/Abandon", 30, 100,
               auto movable = *element;
               T temp,                    temp.AssignAbsorb(Abandon(movable)),
               stdpair movable (1, 555);
               stdpair temp_std,          temp_std = ::std::move(movable)
            );
         }
      }

      WHEN("Ambigous assigned empty self") {
         LglsDisableWarningPush
         LglsDisableWarning_SelfAssign
         REQUIRE_NOTHROW(pack = pack);
         LglsDisableWarningPop
      }
      
      WHEN("Assigned empty self") {
         pack.AssignAbsorb(pack);

         Pair_CheckState_Default<E1, E2>(pack);
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

         Pair_CheckState_Default<E1, E2>(pack);

         BenchmarkPairStd("Empty/Clear", 30, 100,
            T temp,              temp.Clear(),
            stdpair temp_std,    temp_std.clear()
         );
      }

      WHEN("Reset") {
         pack.Reset();

         Pair_CheckState_Default<E1, E2>(pack);

         BenchmarkPairStd("Empty/Reset", 30, 100,
            T temp,              temp.Reset(),
            stdpair temp_std,    temp_std.clear()
         );
      }

      WHEN("Referred empty") {
         T refer1 = pack;

         Pair_Helper_TestSame(refer1, pack);
         Pair_CheckState_Default<E1, E2>(refer1);
         Pair_CheckState_Default<E1, E2>(pack);

         T refer2 = Refer(pack);

         Pair_Helper_TestSame(refer2, pack);
         Pair_CheckState_Default<E1, E2>(refer2);
         Pair_CheckState_Default<E1, E2>(pack);
      }

      WHEN("Cloned empty") {
         T clone = Clone(pack);

         Pair_Helper_TestSame(clone, pack);
         Pair_CheckState_Default<E1, E2>(clone);
         Pair_CheckState_Default<E1, E2>(pack);
      }

      WHEN("Disowned empty") {
         T disowned = Disown(pack);

         Pair_Helper_TestSame(disowned, pack);
         Pair_CheckState_Default<E1, E2>(disowned);
         Pair_CheckState_Default<E1, E2>(pack);
      }

      WHEN("Copied empty") {
         T copy = Copy(pack);

         Pair_Helper_TestSame(copy, pack);
         Pair_CheckState_Default<E1, E2>(copy);
         Pair_CheckState_Default<E1, E2>(pack);
      }

      WHEN("Moved empty") {
         T movable1 = pack;
         const T moved1 = ::std::move(movable1);

         Pair_CheckState_Default<E1, E2>(movable1);
         Pair_Helper_TestSame(moved1, pack);
         Pair_CheckState_Default<E1, E2>(moved1);
         Pair_CheckState_Default<E1, E2>(pack);

         T movable2 = pack;
         const T moved2 = Move(movable2);

         Pair_CheckState_Default<E1, E2>(movable2);
         Pair_Helper_TestSame(moved2, pack);
         Pair_CheckState_Default<E1, E2>(moved2);
         Pair_CheckState_Default<E1, E2>(pack);
      }

      WHEN("Abandoned empty") {
         T movable = pack;
         const T moved = Abandon(movable);

         Pair_CheckState_Default<E1, E2>(movable);
         Pair_Helper_TestSame(moved, pack);
         Pair_CheckState_Default<E1, E2>(moved);
         Pair_CheckState_Default<E1, E2>(pack);
      }

      WHEN("Compared empty") {
         T another_pack1;
         T another_pack2;

         REQUIRE      (another_pack1 == another_pack2);
         REQUIRE_FALSE(another_pack1 != another_pack2);
         static_assert(     T{} == T{} );
         static_assert(not (T{} != T{}));

         [[maybe_unused]] volatile bool dont_optimize = false;
         BenchmarkPair("Empty/operator==", 30,
            (void) 0, dont_optimize |= (another_pack1 == another_pack2)
         );
         BenchmarkPair("Empty/operator!=", 30,
            (void) 0, dont_optimize |= (another_pack1 != another_pack2)
         );
      }

      WHEN("Contains when empty") {
         REQUIRE_FALSE(pack.Contains(*element1));

         [[maybe_unused]] volatile bool dont_optimize = false;
         BenchmarkPair("Empty/Contains", 30,
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
            static_assert(::std::same_as<decltype(h), THandlePair<HandleMut, HandleMut>>);
         else
            static_assert(::std::same_as<decltype(h), THandlePair<THandle<E1&>, THandle<E2&>>>);

         HandlePair_CheckState_Default<E1, E2>(h);
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