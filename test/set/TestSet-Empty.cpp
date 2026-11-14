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
   // Explicit instantiation for using extern templates in other tests  
   template struct TSet<Text>;
   template struct TSet<int>;
   template struct TSet<Any>;
   template struct TSet<RT>;
   template struct TSet<char>;

   template struct TSet<Text*>;
   template struct TSet<int*>;
   template struct TSet<Any*>;
   template struct TSet<RT*>;
   template struct TSet<char*>;

   template struct TSet<Text**>;
   template struct TSet<int**>;
   template struct TSet<Any**>;
   template struct TSet<RT**>;
   template struct TSet<char**>;

#if LANGULUS_FEATURE(MANAGED_MEMORY)
   template struct TSet<pptr8>;
   template struct TSet<pptr16>;
   template struct TSet<pptr32>;
#endif
}


TEST_CASE_TEMPLATE("Test empty Set/TSet", TestType
   // Elements are not allocated by the memory manager                  
   , Types<Set, ScopedElement<Text>>
   , Types<Set, ScopedElement<int>>
   , Types<Set, ScopedElement<Any>>
   , Types<Set, ScopedElement<RT>>
   , Types<Set, ScopedElement<char>>

   , Types<Set, ScopedElement<Text*>>
   , Types<Set, ScopedElement<int*>>
   , Types<Set, ScopedElement<Any*>>
   , Types<Set, ScopedElement<RT*>>
   , Types<Set, ScopedElement<char*>>

   , Types<Set, ScopedElement<Text**>>
   , Types<Set, ScopedElement<int**>>
   , Types<Set, ScopedElement<Any**>>
   , Types<Set, ScopedElement<RT**>>
   , Types<Set, ScopedElement<char**>>

   , Types<TSet<Text>,   ScopedElement<Text>>
   , Types<TSet<int>,    ScopedElement<int>>
   , Types<TSet<Any>,    ScopedElement<Any>>
   , Types<TSet<RT>,     ScopedElement<RT>>
   , Types<TSet<char>,   ScopedElement<char>>

   , Types<TSet<Text*>,  ScopedElement<Text*>>
   , Types<TSet<int*>,   ScopedElement<int*>>
   , Types<TSet<Any*>,   ScopedElement<Any*>>
   , Types<TSet<RT*>,    ScopedElement<RT*>>
   , Types<TSet<char*>,  ScopedElement<char*>>

   , Types<TSet<Text**>, ScopedElement<Text**>>
   , Types<TSet<int**>,  ScopedElement<int**>>
   , Types<TSet<Any**>,  ScopedElement<Any**>>
   , Types<TSet<RT**>,   ScopedElement<RT**>>
   , Types<TSet<char**>, ScopedElement<char**>>

   #if LANGULUS_FEATURE(MANAGED_MEMORY)
   // Elements are allocated by the memory manager                      
   , Types<Set, ScopedElement<Text, true>>
   , Types<Set, ScopedElement<int, true>>
   , Types<Set, ScopedElement<Any, true>>
   , Types<Set, ScopedElement<RT, true>>
   , Types<Set, ScopedElement<char, true>>

   , Types<Set, ScopedElement<Text*, true>>
   , Types<Set, ScopedElement<int*, true>>
   , Types<Set, ScopedElement<Any*, true>>
   , Types<Set, ScopedElement<RT*, true>>
   , Types<Set, ScopedElement<char*, true>>

   , Types<Set, ScopedElement<Text**, true>>
   , Types<Set, ScopedElement<int**, true>>
   , Types<Set, ScopedElement<Any**, true>>
   , Types<Set, ScopedElement<RT**, true>>
   , Types<Set, ScopedElement<char**, true>>

   , Types<TSet<Text>,   ScopedElement<Text, true>>
   , Types<TSet<int>,    ScopedElement<int, true>>
   , Types<TSet<Any>,    ScopedElement<Any, true>>
   , Types<TSet<RT>,     ScopedElement<RT, true>>
   , Types<TSet<char>,   ScopedElement<char, true>>

   , Types<TSet<Text*>,  ScopedElement<Text*, true>>
   , Types<TSet<int*>,   ScopedElement<int*, true>>
   , Types<TSet<Any*>,   ScopedElement<Any*, true>>
   , Types<TSet<RT*>,    ScopedElement<RT*, true>>
   , Types<TSet<char*>,  ScopedElement<char*, true>>

   , Types<TSet<Text**>, ScopedElement<Text**, true>>
   , Types<TSet<int**>,  ScopedElement<int**, true>>
   , Types<TSet<Any**>,  ScopedElement<Any**, true>>
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

   //constexpr bool Managed   = ScopedE::Managed;
   constexpr bool Sparse    = CT::Sparse<E>;
   constexpr bool Reffed    = CT::Referenced<Decay<E>>;
   constexpr bool Ambiguous = not Same<T, E> and CT::Set<E> and LANGULUS(SAFE);

   #if LANGULUS(BENCHMARK)
      using stdset = ::std::unordered_set<E>;
   #endif

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
      static_assert(    Exact<TypeOf<T>, E>);
      static_assert(not CT::TypeErased<T>);
      static_assert(    CT::Comparable<TypeOf<T>, E>);

      static_assert(CT::CopyConstructible<T>    == CT::CopyConstructible<E>);
      static_assert(CT::ReferConstructible<T>   == CT::ReferConstructible<E>);
      static_assert(CT::AbandonConstructible<T> == CT::AbandonConstructible<E>);
      static_assert(CT::MoveConstructible<T>    == CT::MoveConstructible<E>);
      static_assert(CT::CloneConstructible<T>   == CT::CloneConstructible<E>);
      static_assert(CT::DisownConstructible<T>  == CT::DisownConstructible<E>);

      static_assert(CT::CopyAssignable<T>       == CT::CopyAssignable<E>);
      static_assert(CT::ReferAssignable<T>      == CT::ReferAssignable<E>);
      static_assert(CT::AbandonAssignable<T>    == CT::AbandonAssignable<E>);
      static_assert(CT::MoveAssignable<T>       == CT::MoveAssignable<E>);
      static_assert(CT::CloneAssignable<T>      == CT::CloneAssignable<E>);
      static_assert(CT::DisownAssignable<T>     == CT::DisownAssignable<E>);      
   }
   
   {
      static_assert(    CT::Deep<T>);
      static_assert(not CT::ContainsOne<T>);
      static_assert(    CT::ContainsMany<T>);
      static_assert(not CT::Handle<T>);
      static_assert(    CT::HasVariableCount<T>);
      static_assert(    CT::HeapAllocated<T>);
      static_assert(    CT::OwnedDeep<T> == (CT::TypeErased<T> or CT::Sparse<TypeOf<T>>));
      static_assert(    CT::Owned<T>);
      static_assert(    CT::OwnedStrong<T>);
      static_assert(    CT::Comparable<T, T>);
      static_assert(    CT::Comparable<T, E>);

      static_assert(::std::input_or_output_iterator<decltype(LglsFake(T).begin())>);
      static_assert(::std::input_or_output_iterator<decltype(LglsFake(T).end())>);

      static_assert(::std::ranges::range<T>);

      static_assert(    requires (T pack)         { pack.Get(); });
      static_assert(not requires (T pack)         { pack.template As<E>(); });
      //static_assert(not requires (T pack)         { pack.GetDeep(); });
      static_assert(not requires (T pack)         { pack.GetResolved(); });
      static_assert(not requires (T pack)         { pack.GetDense(); });
      static_assert(not requires (T pack)         { pack +   pack; });
      static_assert(    CT::TextRange<E> or not requires (T pack, E item) { pack + item; });
      static_assert(    CT::TextRange<E> or not requires (T pack, E item) { item + pack; });
      static_assert(not CT::TextRange<E> or     requires (T pack, E item) { {item + pack} -> CT::Text; });
      static_assert(not CT::TextRange<E> or     requires (T pack, E item) { {pack + item} -> CT::Text; });
      static_assert(not requires (T pack)         { pack +=  pack; });
      static_assert(not requires (T pack, E item) { pack +=  item; });
      static_assert(not requires (T pack, E item) { pack <<  item; });
      static_assert(not requires (T pack, E item) { pack >>  item; });
      static_assert(    requires (T pack, E item) { {pack <<= item} -> ::std::same_as<T&>; });
      static_assert(    requires (T pack, E item) { {pack >>= item} -> ::std::same_as<T&>; });
      static_assert(not requires (T pack, E item) { pack.InsertAt(Index::Back, item); });
      static_assert(not requires (T pack, E item) { pack.Insert(item); });
      static_assert(not requires (T pack, E item) { pack.EmplaceAt(Index::Back, item); });
      static_assert(not requires (T pack, E item) { pack.Emplace(item); });
      static_assert(not requires (T pack)         { pack.ConcatAt(Index::Back, pack); });
      static_assert(not requires (T pack)         { pack.Concat(pack); });
      static_assert(not requires (T pack, E item) { pack.MergeAt(Index::Back, item); });
      static_assert(not requires (T pack)         { pack.MergeRangeAt(Index::Back, pack); });
      static_assert(    requires (T pack, E item) { pack.Merge(item); });
      static_assert(    requires (T pack)         { pack.MergeRange(pack); });
      static_assert(    requires (T pack, E item) { pack.Erase(item); });
      static_assert(not requires (T pack)         { pack.EraseAt(Index::Front); });
      static_assert(    requires (T pack)         { pack.Reserve(20); });
      static_assert(not requires (T pack)         { pack.EnableOr(); });
      static_assert(not requires (T pack)         { pack.IsOr(); });
      static_assert(    requires (T pack, E item) { pack.Find(item); });
      static_assert(    requires (T pack)         { pack.ForEach([](const int&) {}); });
      static_assert(    requires (T pack)         { pack.ForEachRev([](const int&) {}); });
   }

   static_assert(T::CountHeapProviders() == 1);
   //static_assert(T::template CountHeapFooterRequests<0>() == 1);

   GIVEN("Default-constructed container") {
      const ScopedE element {555};
      T pack;

      /// MARK: Gap test                                                      
      WHEN("Gap test") {
         Common_GapTest<T, ::std::unordered_set<E>>();
         static_assert(sizeof(T) <= sizeof(::std::unordered_set<E>));
      }

      WHEN("Default-constructed") {
         Set_CheckState_Default<E>(pack);
      
         if constexpr (Ambiguous) {
            WHEN("Ambiguous assign value by referral") {
               REQUIRE_THROWS(pack = *element);
               REQUIRE_THROWS(pack = Refer(*element));
            }
         }

         BenchmarkSetStd("Empty/DefaultConstructor", 30, 40,
            T temp,                 new (&temp)     T{},
            stdset temp_std,        new (&temp_std) stdset{}
         );
      }

      /// MARK: Assign/Refer                                                  
      WHEN("Assigned value by referral") {
         REQUIRE_NOTHROW(pack.Assign(*element));

         Set_CheckState_OwnedFull<E>(pack);
         Set_CheckState_ContainsOne(pack, Refer(element));

         BenchmarkSetStd("Empty/Assign/Refer", 30, 100,
            T temp,              temp.Assign(*element),
            stdset temp_std,     temp_std.emplace(*element)
         );
      }

      if constexpr (CT::Set<E>) {
         WHEN("Assigned and absorbed by referral") {
            if (CT::Typed<T> and not pack.IsSame(element->GetType())) {
               const auto element_backup = *element;
               REQUIRE_THROWS(pack.AssignAbsorb(*element));
               Set_CheckState_Default<E>(pack);
               Set_Helper_TestSame(element_backup, *element);
               return;
            }

            REQUIRE_NOTHROW(pack.AssignAbsorb(*element));

            Set_Helper_TestSame(pack, *element);
            REQUIRE(pack.GetUses() == element->GetUses());
            REQUIRE(pack.GetUses() == 2);
            REQUIRE(pack.GetAllocation() == element->GetAllocation());

            BenchmarkSetStd("Empty/AssignAbsorb/Refer", 30, 100,
               T temp,                          temp.AssignAbsorb(*element),
               stdset src_std (1, *element);
               stdset temp_std,                 temp_std = src_std;
            );
         }
      }

      /// MARK: Assign/Move                                                   
      if constexpr (Ambiguous) {
         WHEN("Ambiguous assign value by move") {
            auto movable = *element;
            REQUIRE_THROWS(pack = ::std::move(movable));
            REQUIRE_THROWS(pack = Move(movable));
         }
      }
      
      WHEN("Assigned value by move") {
         auto movable = *element;
         REQUIRE_NOTHROW(pack.Assign(::std::move(movable)));
         
         if constexpr (CT::Set<E>)
            Set_CheckState_Default<TypeOf<E>>(movable);

         Set_CheckState_OwnedFull<E>(pack);
         Set_CheckState_ContainsOne(pack, Refer(element));

         BenchmarkSetStd("Empty/Assign/Move", 30, 100,
            auto movable = *element;
            T temp,                       temp.Assign(::std::move(movable)),
            auto movable = *element;
            stdset temp_std,              temp_std.emplace(::std::move(movable))
         );
      }

      if constexpr (CT::Set<E>) {
         WHEN("Assigned and absorbed by move") {
            auto movable = *element;

            if (CT::Typed<T> and not pack.IsSame(element->GetType())) {
               REQUIRE_THROWS(pack.AssignAbsorb(::std::move(movable)));
               Set_CheckState_Default<E>(pack);
               Set_Helper_TestSame(movable, *element);
               return;
            }

            REQUIRE_NOTHROW(pack.AssignAbsorb(::std::move(movable)));

            Set_CheckState_Default<TypeOf<E>>(movable);
            Set_Helper_TestSame(pack, *element);
            REQUIRE(pack.GetUses() == element->GetUses());
            REQUIRE(pack.GetUses() == 2);
            REQUIRE(pack.GetAllocation() == element->GetAllocation());

            BenchmarkSetStd("Empty/AssignAbsorb/Move", 30, 100,
               auto movable = *element;
               T temp,                          temp.AssignAbsorb(::std::move(movable)),
               stdset movable (1, 555);
               stdset temp_std,                 temp_std.emplace(::std::move(movable))
            );
         }
      }

      /// MARK: Assign/Copy                                                   
      if constexpr (Ambiguous) {
         WHEN("Ambiguous assign copied value") {
            REQUIRE_THROWS(pack = Copy(*element));
         }
      }
      
      WHEN("Assigned value by copy") {
         REQUIRE_NOTHROW(pack.Assign(Copy(*element)));

         Set_CheckState_OwnedFull<E>(pack);
         Set_CheckState_ContainsOne(pack, Copy(element));

         BenchmarkSetStd("Empty/Assign/Copy", 30, 100,
            T temp,                 temp.Assign(Copy(*element)),
            stdset temp_std,        temp_std.emplace(*element)
         );
      }

      if constexpr (CT::Set<E>) {
         WHEN("Assigned and absorbed copied value") {
            if (CT::Typed<T> and not pack.IsSame(element->GetType())) {
               const auto element_backup = *element;
               REQUIRE_THROWS(pack.AssignAbsorb(Copy(*element)));
               Set_CheckState_Default<E>(pack);
               Set_Helper_TestSame(element_backup, *element);
               return;
            }

            REQUIRE_NOTHROW(pack.AssignAbsorb(Copy(*element)));

            REQUIRE(pack.GetRaw() != element->GetRaw());
            REQUIRE(pack.IsExact(element->GetType()));
            REQUIRE(pack == *element);
            REQUIRE(pack.IsDeep() == element->IsDeep());
            REQUIRE_FALSE(pack.IsConstant());
            REQUIRE(pack.GetUnconstrainedState() == element->GetUnconstrainedState());

            REQUIRE(pack.GetUses() == 1);
            REQUIRE(pack.GetAllocation());

            BenchmarkSetStd("Empty/AssignAbsorb/Copy", 30, 100,
               T temp,                          temp.AssignAbsorb(Copy(*element)),
               stdset src_std (1, *element);
               stdset temp_std,                 temp_std = src_std
            );
         }
      }

      /// MARK: Assign/Clone                                                  
      if constexpr (Ambiguous) {
         WHEN("Ambiguous assign cloned value") {
            REQUIRE_THROWS(pack = Clone(*element));
         }
      }
      
      WHEN("Assigned value by clone") {
         REQUIRE_NOTHROW(pack.Assign(Clone(*element)));

         Set_CheckState_OwnedFull<E>(pack);
         Set_CheckState_ContainsOne(pack, Clone(element));

         BenchmarkSetStd("Empty/Assign/Clone", 30, 100,
            T temp,              temp.Assign(Clone(*element)),
            stdset temp_std,     temp_std.emplace(*element)
         );
      }

      if constexpr (CT::Set<E>) {
         WHEN("Assigned and absorbed cloned value") {
            if (CT::Typed<T> and not pack.IsSame(element->GetType())) {
               const auto element_backup = *element;
               REQUIRE_THROWS(pack.AssignAbsorb(Clone(*element)));
               Set_CheckState_Default<E>(pack);
               Set_Helper_TestSame(element_backup, *element);
               return;
            }

            REQUIRE_NOTHROW(pack.AssignAbsorb(Clone(*element)));
            Any_CheckState_OwnedFull<int>(*element);
            Any_CheckState_OwnedFull<int>(pack);

            REQUIRE(pack.GetRaw() != element->GetRaw());
            REQUIRE(pack.IsExact(element->GetType()));
            REQUIRE(pack == *element);
            REQUIRE(pack.IsDeep() == element->IsDeep());
            REQUIRE_FALSE(pack.IsConstant());
            REQUIRE(pack.GetUnconstrainedState() == element->GetUnconstrainedState());
            REQUIRE(pack.GetUses() == 1);
            REQUIRE(pack.GetAllocation());

            BenchmarkSetStd("Empty/AssignAbsorb/Clone", 30, 100,
               T temp,                       temp.AssignAbsorb(Clone(*element)),
               stdset src_std ({*element});
               stdset temp_std,              temp_std = src_std
            );
         }
      }

      /// MARK: Assign/Disown                                                 
      if constexpr (Ambiguous) {
         WHEN("Ambiguous assign disowned value") {
            REQUIRE_THROWS(pack = Disown(*element));
         }
      }
      
      WHEN("Assigned value by disown") {
         REQUIRE_NOTHROW(pack.Assign(Disown(*element)));

         Set_CheckState_OwnedFull<E>(pack);
         Set_CheckState_ContainsOne(pack, Disown(element));

         BenchmarkSetStd("Empty/Assign/Disown", 30, 100,
            T temp,                 temp.Assign(Disown(*element)),
            stdset temp_std,        temp_std.emplace(*element)
         );
      }

      if constexpr (CT::Set<E>) {
         WHEN("Assigned and absorbed disowned value") {
            if (CT::Typed<T> and not pack.IsSame(element->GetType())) {
               const auto element_backup = *element;
               REQUIRE_THROWS(pack.AssignAbsorb(Disown(*element)));
               Set_CheckState_Default<E>(pack);
               Set_Helper_TestSame(element_backup, *element);
               return;
            }

            REQUIRE_NOTHROW(pack.AssignAbsorb(Disown(*element)));

            Set_CheckState_OwnedFull<int>(*element);
            Set_CheckState_DisownedFull<int>(pack);
            Set_Helper_TestSame(pack, *element, false);
            REQUIRE(pack.IsConstant());

            BenchmarkSetStd("Empty/AssignAbsorb/Disown", 30, 100,
               T temp,                          temp.AssignAbsorb(Disown(*element)),
               stdset src_std (1, *element);
               stdset temp_std,                 temp_std = src_std
            );
         }
      }

      /// MARK: Assign/Abandon                                                
      if constexpr (Ambiguous) {
         WHEN("Ambiguous assigned abandoned value") {
            auto movable = *element;
            REQUIRE_THROWS(pack = Abandon(movable));
         }
      }
      
      WHEN("Assigned value by abandon") {
         auto movable = *element;
         REQUIRE_NOTHROW(pack.Assign(Abandon(movable)));

         if constexpr (CT::Set<E>)
            Set_CheckState_Abandoned<E>(movable);
         Set_CheckState_OwnedFull<E>(pack);
         Set_CheckState_ContainsOne(pack, Refer(element));

         BenchmarkSetStd("Empty/Assign/Abandon", 30, 100,
            auto movable = *element;
            T temp,                          temp.Assign(Abandon(movable)),
            auto movable = *element;
            stdset temp_std,                 temp_std.emplace(::std::move(movable))
         );
      }

      if constexpr (CT::Set<E>) {
         WHEN("Assigned and absorbed abandoned value") {
            auto movable = *element;

            if (CT::Typed<T> and not pack.IsSame(element->GetType())) {
               REQUIRE_THROWS(pack.AssignAbsorb(Abandon(movable)));
               Set_CheckState_Default<E>(pack);
               Set_Helper_TestSame(movable, *element);
               return;
            }

            REQUIRE_NOTHROW(pack.AssignAbsorb(Abandon(movable)));

            Set_CheckState_Abandoned<E>(movable);
            Set_Helper_TestSame(pack, *element);
            REQUIRE(pack.GetUses() == 2);
            REQUIRE(pack.GetAllocation() == element->GetAllocation());

            BenchmarkSetStd("Empty/AssignAbsorb/Abandon", 30, 100,
               auto movable = *element;
               T temp,                       temp.AssignAbsorb(Abandon(movable)),
               stdset movable (1, 555);
               stdset temp_std,              temp_std = ::std::move(movable)
            );
         }
      }

      /// MARK: Assign empty                                                  
      WHEN("Ambigous assigned empty self") {
         LglsDisableWarningPush
         LglsDisableWarning_SelfAssign
         REQUIRE_NOTHROW(pack = pack);
         LglsDisableWarningPop
      }
      
      WHEN("Assigned empty self") {
         REQUIRE_NOTHROW(pack.AssignAbsorb(pack));

         Set_CheckState_Default<E>(pack);
      }

      /// MARK: Clear                                                         
      WHEN("Cleared") {
         REQUIRE_NOTHROW(pack.Clear());

         Set_CheckState_Default<E>(pack);

         BenchmarkSetStd("Empty/Clear", 30, 100,
            T temp,              temp.Clear(),
            stdset temp_std,     temp_std.clear()
         );
      }

      /// MARK: Reset                                                         
      WHEN("Reset") {
         REQUIRE_NOTHROW(pack.Reset());

         Set_CheckState_Default<E>(pack);

         BenchmarkSetStd("Empty/Reset", 30, 100,
            T temp,              temp.Reset(),
            stdset temp_std,     temp_std.clear()
         );
      }
      
      /// MARK: Erase                                                         
      WHEN("Erase non-existent value") {
         size_t removed = 0;
         REQUIRE_NOTHROW(removed = pack.Erase(*element));

         Set_CheckState_Default<E>(pack);

         REQUIRE(removed == 0);

         BenchmarkSetStd("Empty/Erase", 30, 100,
            T temp,              temp.Erase(*element),
            stdvec temp_std,     temp_std.erase(std::remove_if(temp_std.begin(), temp_std.end(), [&element] (auto& value) {
                                    return value == *element;
                                 }), temp_std.end());
         );
      }

      WHEN("Referred empty") {
         T refer1 = pack;

         Set_Helper_TestSame(refer1, pack);
         Set_CheckState_Default<E>(refer1);
         Set_CheckState_Default<E>(pack);

         T refer2 = Refer(pack);

         Set_Helper_TestSame(refer2, pack);
         Set_CheckState_Default<E>(refer2);
         Set_CheckState_Default<E>(pack);
      }

      WHEN("Cloned empty") {
         T clone = Clone(pack);

         Set_Helper_TestSame(clone, pack);
         Set_CheckState_Default<E>(clone);
         Set_CheckState_Default<E>(pack);
      }

      WHEN("Disowned empty") {
         T disowned = Disown(pack);

         Set_Helper_TestSame(disowned, pack);
         Set_CheckState_Default<E>(disowned);
         Set_CheckState_Default<E>(pack);
      }

      WHEN("Copied empty") {
         T copy = Copy(pack);

         Set_Helper_TestSame(copy, pack);
         Set_CheckState_Default<E>(copy);
         Set_CheckState_Default<E>(pack);
      }

      WHEN("Moved empty") {
         T movable1 = pack;
         const T moved1 = ::std::move(movable1);

         Set_CheckState_Default<E>(movable1);
         Set_Helper_TestSame(moved1, pack);
         Set_CheckState_Default<E>(moved1);
         Set_CheckState_Default<E>(pack);

         T movable2 = pack;
         const T moved2 = Move(movable2);

         Set_CheckState_Default<E>(movable2);
         Set_Helper_TestSame(moved2, pack);
         Set_CheckState_Default<E>(moved2);
         Set_CheckState_Default<E>(pack);
      }

      WHEN("Abandoned empty") {
         T movable = pack;
         const T moved = Abandon(movable);

         Set_CheckState_Default<E>(movable);
         Set_Helper_TestSame(moved, pack);
         Set_CheckState_Default<E>(moved);
         Set_CheckState_Default<E>(pack);
      }

      /// MARK: Compare                                                       
      WHEN("Compared empty") {
         T another_pack1;
         T another_pack2;

         REQUIRE      (another_pack1 == another_pack2);
         REQUIRE_FALSE(another_pack1 != another_pack2);
         static_assert(     T{} == T{} );
         static_assert(not (T{} != T{}));

         // Unfortunately, ::std::any aren't comparable when empty      
         [[maybe_unused]] volatile bool dont_optimize = false;
         BenchmarkSet("Empty/operator==", 30,
            (void) 0, dont_optimize |= (another_pack1 == another_pack2)
         );
         BenchmarkSet("Empty/operator!=", 30,
            (void) 0, dont_optimize |= (another_pack1 != another_pack2)
         );
      }

      /// MARK: Contains                                                      
      WHEN("Contains when empty") {
         REQUIRE_FALSE(pack.Contains(*element));

         [[maybe_unused]] volatile bool dont_optimize = false;
         BenchmarkSet("Empty/Contains", 30,
            (void) 0, dont_optimize |= pack.Contains(*element)
         );
      }

      if constexpr (Exact<E, Text>) {
         WHEN("Given text that will be destroyed before the pack") {
            Text owned_text = "666";
            pack = Text(owned_text.operator Token());
         }
      }
      
      /// MARK: Range                                                         
      WHEN("Range-iterated (default)") {
         IterateDefault strategy(pack);
         IterateDefault strategyConst(::std::as_const(pack));
         using Iterator = decltype(strategy.begin());
         using IteratorConst = decltype(strategyConst.begin());

         static_assert(::std::same_as<Iterator, decltype(strategy.end())>);
         static_assert(::std::same_as<IteratorConst, decltype(strategyConst.end())>);
         static_assert(::std::input_or_output_iterator<Iterator>);
         static_assert(::std::input_or_output_iterator<IteratorConst>);

         // These are not possible to satisfy if type-erased            
         static_assert(CT::TypeErased<T> or Sparse or ::std::random_access_iterator<Iterator>);
         static_assert(CT::TypeErased<T>           or ::std::random_access_iterator<IteratorConst>);
         //static_assert(CT::TypeErased<T> or Sparse or ::std::contiguous_iterator<Iterator>);
         //static_assert(CT::TypeErased<T>           or ::std::contiguous_iterator<IteratorConst>);

         size_t counter = 0;
         for (auto& it : pack) {
            (void) it;
            ++counter;

            if constexpr (CT::TypeErased<T>/* or Sparse*/)
               static_assert(CT::Handle<decltype(it)>);
            else
               static_assert(Same<E, decltype(it)>);
         }

         for (auto& it : ::std::as_const(pack)) {
            (void) it;
            ++counter;

            if constexpr (CT::TypeErased<T>)
               static_assert(CT::Handle<decltype(it)>);
            else
               static_assert(Same<E, decltype(it)>);
         }

         for (auto& it : strategy) {
            (void) it;
            ++counter;

            if constexpr (CT::TypeErased<T>/* or Sparse*/)
               static_assert(CT::Handle<decltype(it)>);
            else
               static_assert(Same<E, decltype(it)>);
         }

         for (auto& it : strategyConst) {
            (void) it;
            ++counter;

            if constexpr (CT::TypeErased<T>)
               static_assert(CT::Handle<decltype(it)>);
            else
               static_assert(Same<E, decltype(it)>);
         }

         REQUIRE(counter == 0);
      }

      WHEN("Range-iterated (reverse)") {
         IterateInReverse strategy(pack);
         IterateInReverse strategyConst(::std::as_const(pack));
         using Iterator = decltype(strategy.begin());
         using IteratorConst = decltype(strategyConst.begin());

         static_assert(::std::same_as<Iterator, decltype(strategy.end())>);
         static_assert(::std::same_as<IteratorConst, decltype(strategyConst.end())>);
         static_assert(::std::input_or_output_iterator<Iterator>);
         static_assert(::std::input_or_output_iterator<IteratorConst>);

         // These are not possible to satisfy if type-erased            
         static_assert(CT::TypeErased<T> or Sparse or ::std::random_access_iterator<Iterator>);
         static_assert(CT::TypeErased<T>           or ::std::random_access_iterator<IteratorConst>);
         //static_assert(CT::TypeErased<T> or Sparse or ::std::contiguous_iterator<Iterator>);
         //static_assert(CT::TypeErased<T>           or ::std::contiguous_iterator<IteratorConst>);

         size_t counter = 0;
         for (auto& it : strategy) {
            (void) it;
            ++counter;

            if constexpr (CT::TypeErased<T>/* or Sparse*/)
               static_assert(CT::Handle<decltype(it)>);
            else
               static_assert(Same<E, decltype(it)>);
         }

         for (auto& it : strategyConst) {
            (void) it;
            ++counter;

            if constexpr (CT::TypeErased<T>)
               static_assert(CT::Handle<decltype(it)>);
            else
               static_assert(Same<E, decltype(it)>);
         }

         REQUIRE(counter == 0);
      }

      WHEN("Range-iterated (noderef)") {
         IterateNoDeref strategy(pack);
         using Iterator = decltype(strategy.begin());

         static_assert(::std::same_as<Iterator, decltype(strategy.end())>);
         static_assert(::std::input_or_output_iterator<Iterator>);

         // These are not possible to satisfy if C is type-erased       
         static_assert(CT::TypeErased<T> or Sparse or ::std::random_access_iterator<typename Iterator::value_type>);
         //static_assert(CT::TypeErased<T> or Sparse or ::std::contiguous_iterator<typename Iterator::value_type>);

         size_t counter = 0;
         for (auto& it : strategy) {
            (void) it;
            ++counter;

            static_assert(Same<typename IterateDefault<false, T>::Iterator, decltype(it)>);
         }

         REQUIRE(counter == 0);
      }

      WHEN("Range-iterated (handles)") {
         IterateHandles strategy(pack);
         using Iterator = decltype(strategy.begin());

         static_assert(::std::same_as<Iterator, decltype(strategy.end())>);
         static_assert(::std::input_or_output_iterator<Iterator>);

         // These are not possible to satisfy if C is type-erased       
         //static_assert(CT::TypeErased<T> or CT::Sparse<E> or ::std::random_access_iterator<typename Iterator::value_type>);
         //static_assert(CT::TypeErased<T> or CT::Sparse<E> or ::std::contiguous_iterator<typename Iterator::value_type>);

         size_t counter = 0;
         for (auto& it : strategy) {
            (void) it;
            ++counter;

            static_assert(CT::Handle<decltype(it)>);
         }

         REQUIRE(counter == 0);
      }

      WHEN("Range-iterated (together)") {
         T pack2;
         IterateTogether strategy(pack, pack2);
         using Iterator = decltype(strategy.begin());

         static_assert(::std::same_as<Iterator, decltype(strategy.end())>);
         static_assert(::std::input_or_output_iterator<Iterator>);

         // These are not possible to satisfy if C is type-erased       
         //static_assert(CT::TypeErased<T> or CT::Sparse<E> or ::std::random_access_iterator<Iterator>);
         //static_assert(CT::TypeErased<T> or CT::Sparse<E> or ::std::contiguous_iterator<Iterator>);

         size_t counter = 0;
         for (auto& it : strategy) {
            (void) it;
            ++counter;

            if constexpr (CT::TypeErased<T>/* or Sparse*/)
               static_assert(CT::Handle<decltype(it.one()), decltype(it.two())>);
            else
               static_assert(Same<E, decltype(it.one()), decltype(it.two())>);
         }

         REQUIRE(counter == 0);
      }
      
      /// MARK: Handles                                                       
      WHEN("GetHandle is called on mutable container") {
         auto h = pack.GetHandle();

         if constexpr (CT::Untyped<T>)
            static_assert(::std::same_as<decltype(h), Handle>);
         else
            static_assert(::std::same_as<decltype(h), THandle<ConstAll<E&>>>);

         Handle_CheckState_Default<E const>(h);
      }

      WHEN("GetHandle is called on constant container") {
         T const pack_constant;
         auto h = pack_constant.GetHandle();

         if constexpr (CT::Untyped<T>)
            static_assert(::std::same_as<decltype(h), Handle>);
         else
            static_assert(::std::same_as<decltype(h), THandle<ConstAll<E&>>>);

         Handle_CheckState_Default<E const>(h);
      }
   }

   GIVEN("Default-constructed container and a couple of arrays") {
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

      T pack;

      /// MARK: Merge array                                                   
      WHEN("Merge an array") {
         size_t inserted = 0;
         REQUIRE_NOTHROW(inserted += pack.Merge(          immovable));
         REQUIRE(inserted == 5);
         DumpSet(pack);
         Set_CheckState_ContainsArray(pack, immovable);

         if constexpr (Sparse and Reffed) {
            for (uint i = 0; i < 5; ++i) {
               for (auto& arr : darray1) {
                  if (*pack.template GetAt<E>(i) == *arr) {
                     // Shallowly referenced in the set              
                     REQUIRE(DenseCast(*arr).GetReferences() == 2);
                  }
               }
            }
         }

         REQUIRE_NOTHROW(inserted += pack.Merge(Refer    {immovable}));
         REQUIRE_NOTHROW(inserted += pack.Merge(Copy     {immovable}));
         REQUIRE_NOTHROW(inserted += pack.Merge(Disown   {immovable}));
         REQUIRE(inserted == 5);
         DumpSet(pack);
         Set_CheckState_ContainsArray(pack, immovable);

         //auto table = pack.GetHashTable();
         REQUIRE_NOTHROW(inserted += pack.Merge(std::move(movable1)));
         REQUIRE(inserted == 10);
         DumpSet(pack);
         Set_CheckState_ContainsArray(pack, immovable, movable2);

         if constexpr (Sparse and Reffed) {
            for (uint i = 0; i < 10; ++i) {
               for (auto& arr : darray1) {
                  if (*pack.template GetAt<E>(i) == *arr) {
                     // Shallowly referenced in the set              
                     REQUIRE(DenseCast(*arr).GetReferences() == 2);
                  }
               }
            }
         }

         REQUIRE_NOTHROW(inserted += pack.Merge(Move     {movable2}));
         REQUIRE_NOTHROW(inserted += pack.Merge(Abandon  {movable3}));
         Set_CheckState_ContainsArray(pack, immovable, movable2);

         // Cloning will generate new pointers when E is sparse, and    
         // thus all elements will be inserted.                         
         REQUIRE_NOTHROW(inserted += pack.Merge(Clone    {immovable}));
         DumpSet(pack);

         if constexpr (Sparse) {
            Set_CheckState_ContainsN(pack, 15);
            REQUIRE(inserted == 15);
         }
         else {
            Set_CheckState_ContainsArray(pack, immovable, movable2);
            REQUIRE(inserted == 10);
         }

         Set_CheckState_OwnedFull<E>(pack);

         if constexpr (Sparse and Reffed) {
            for (uint i = 0; i < 15; ++i) {
               for (auto& arr : darray1) {
                  if (*pack.template GetAt<E>(i) == *arr) {
                     // Shallowly referenced in the set              
                     REQUIRE(DenseCast(*arr).GetReferences() == 2);
                  }
               }
            }
         }

         if constexpr (CT::Set<E>) {
            for (int i = 0; i < 5; ++i) {
               Set_CheckState_Default<int>  (movable1[i]);
            }
            for (int i = 0; i < 5; ++i) {
               Set_CheckState_OwnedFull<int>(movable2[i]);
               Set_CheckState_OwnedFull<int>(movable3[i]);
            }
         }

         if constexpr (Sparse) {
            // Hashing order is different each time, so we can't check  
            // it.                                                      
            /*int deep_matches1_total = 0;
            int deep_matches2_total = 0;*/

            for (uint i = 0; i < 15; ++i) {
               int shallow_matches1 = 0;
               int shallow_matches2 = 0;
               int deep_matches1 = 0;
               int deep_matches2 = 0;

               for (auto& arr : darray1) {
                  deep_matches1 += (DenseCast(pack.template GetAt<E>(i)) == DenseCast(*arr));
                  shallow_matches1 += (*pack.template GetAt<E>(i) == *arr);

                  if constexpr (Reffed) {
                     if (*pack.template GetAt<E>(i) == *arr) {
                        // Shallowly referenced in the set              
                        REQUIRE(DenseCast(*arr).GetReferences() == 2);
                     }
                     else if (DenseCast(pack.template GetAt<E>(i)) == DenseCast(*arr)) {
                        // Cloned                                       
                        REQUIRE(DenseCast(*arr).GetReferences() == 2);
                        REQUIRE(DenseCast(pack.template GetAt<E>(i)).GetReferences() == 1);
                     }
                  }
               }

               for (auto& arr : darray2) {
                  deep_matches2 += (DenseCast(pack.template GetAt<E>(i)) == DenseCast(*arr));
                  shallow_matches2 += (*pack.template GetAt<E>(i) == *arr);

                  if constexpr (Reffed) {
                     if (*pack.template GetAt<E>(i) == *arr) {
                        // Shallowly referenced in the set              
                        REQUIRE(DenseCast(*arr).GetReferences() == 2);
                     }
                  }
               }
               
               bool cond1 = shallow_matches1 == 1 xor shallow_matches2 == 1 xor (not shallow_matches1 and not shallow_matches2);
               REQUIRE(cond1);

               if constexpr (not Akin<E, char>) {
                  bool cond2 = deep_matches1 == 1 xor deep_matches2 == 1;
                  REQUIRE(cond2);
               }

               /*deep_matches1_total += deep_matches1;
               deep_matches2_total += deep_matches2;*/
            }

            /*REQUIRE(deep_matches1_total == 5*2);
            REQUIRE(deep_matches2_total == 5);*/
         }
         else {
            const auto hashed_order = [] -> std::array<int, 10> {
               if constexpr (Same<E, Text>)
                  return {5,7,4,6,1,2,3,9,10,8};
               else if constexpr (Same<E, char>)
                  return {1,2,9,5,3,6,4,7,10,8};
               else
                  return {2,1,5,3,6,4,9,8,7,10};
            }();

            for (uint i = 0; i < 10; ++i) {
               if (hashed_order[i] <= 5) {
                  const int idx = hashed_order[i] - 1;
                  REQUIRE(*pack.template GetAt<E>(i) == *darray1[idx]);
                  if constexpr (Reffed)
                     REQUIRE(DenseCast(*darray1[idx]).GetReferences() == 1);
               }
               else {
                  const int idx = hashed_order[i] - 6;
                  REQUIRE(*pack.template GetAt<E>(i) == *darray2[idx]);
                  if constexpr (Reffed)
                     REQUIRE(DenseCast(*darray2[idx]).GetReferences() == 1);
               }
            }
         }

         BenchmarkSetStd("Empty/Merge/Array", 30, 100,
            T temp,              temp.Merge(immovable),
            stdset temp_std,     std::copy(immovable, immovable + 5, std::back_inserter(temp_std))
         );
      }

      /// MARK: <<=                                                           
      WHEN("Merge by using <<= operator)") {
         pack <<=           immovable[0];
         pack <<= Refer    {immovable[1]};
         pack <<= Copy     {immovable[2]};
         pack <<= Disown   {immovable[3]};
         pack <<= std::move( movable1[0]);
         pack <<= Move     { movable2[0]};
         pack <<= Abandon  { movable3[0]};
         pack <<= Clone    {immovable[4]};

         Set_CheckState_OwnedFull<E>(pack);

         if constexpr (CT::Set<E>) {
            Set_CheckState_Default<int>  (movable1[0]);
            Set_CheckState_Default<int>  (movable2[0]);
            Set_CheckState_Abandoned<int>(movable3[0]);
         }

         Set_CheckState_ContainsN(pack, 6);
         DumpSet(pack);
         
         if constexpr (Sparse) {
            // Hashing order is different each time, so we can't check  
            // it.                                                      
            /*int deep_matches1_total = 0;
            int deep_matches2_total = 0;*/

            for (uint i = 0; i < 6; ++i) {
               int shallow_matches1 = 0;
               int shallow_matches2 = 0;
               int deep_matches1 = 0;
               int deep_matches2 = 0;

               for (auto& arr : darray1) {
                  deep_matches1 += (DenseCast(pack.template GetAt<E>(i)) == DenseCast(*arr));
                  shallow_matches1 += (*pack.template GetAt<E>(i) == *arr);
               }

               for (auto& arr : darray2) {
                  deep_matches2 += (DenseCast(pack.template GetAt<E>(i)) == DenseCast(*arr));
                  shallow_matches2 += (*pack.template GetAt<E>(i) == *arr);
               }
               
               bool cond1 = shallow_matches1 == 1 xor shallow_matches2 == 1 xor (not shallow_matches1 and not shallow_matches2);
               REQUIRE(cond1);

               if constexpr (not Akin<E, char>) {
                  bool cond2 = deep_matches1 == 1 xor deep_matches2 == 1;
                  REQUIRE(cond2);
               }

               /*deep_matches1_total += deep_matches1;
               deep_matches2_total += deep_matches2;*/
            }

            /*REQUIRE(deep_matches1_total == 5);
            REQUIRE(deep_matches2_total == 1);*/
         }
         else {
            const auto hashed_order = [] -> std::array<int, 6> {
               if constexpr (Same<E, Text>)
                  return {5,4,6,1,2,3};
               else if constexpr (Same<E, char>)
                  return {1,2,5,3,6,4};
               else
                  return {2,1,5,3,6,4};
            }();

            for (int i = 0; i < 6; ++i) {
               if (hashed_order[i] <= 5) {
                  const int idx = hashed_order[i] - 1;
                  REQUIRE(*pack.template GetAt<E>(i) == *darray1[idx]);
                  if constexpr (Reffed)
                     REQUIRE(DenseCast(*darray1[idx]).GetReferences() == 1);
               }
               else {
                  const int idx = hashed_order[i] - 6;
                  REQUIRE(*pack.template GetAt<E>(i) == *darray2[idx]);
                  if constexpr (Reffed)
                     REQUIRE(DenseCast(*darray2[idx]).GetReferences() == 1);
               }
            }
         }

         BenchmarkSetStd("Empty/Merge/Element", 30, 100,
            T temp,              temp << immovable[0],
            stdset temp_std,     temp_std.emplace_back(immovable[0])
         );
      }

      /// MARK: >>=                                                           
      WHEN("Merge by using >>= operator)") {
         pack >>=           immovable[0];
         pack >>= Refer    {immovable[1]};
         pack >>= Copy     {immovable[2]};
         pack >>= Disown   {immovable[3]};
         pack >>= std::move( movable1[0]);
         pack >>= Move     { movable2[0]};
         pack >>= Abandon  { movable3[0]};
         pack >>= Clone    {immovable[4]};

         Set_CheckState_OwnedFull<E>(pack);

         if constexpr (CT::Set<E>) {
            Set_CheckState_Default<int>  (movable1[0]);
            Set_CheckState_Default<int>  (movable2[0]);
            Set_CheckState_Abandoned<int>(movable3[0]);
         }

         Set_CheckState_ContainsN(pack, 6);
         DumpSet(pack);
         
         if constexpr (Sparse) {
            // Hashing order is different each time, so we can't check  
            // it.                                                      
            /*int deep_matches1_total = 0;
            int deep_matches2_total = 0;*/

            for (uint i = 0; i < 6; ++i) {
               int shallow_matches1 = 0;
               int shallow_matches2 = 0;
               int deep_matches1 = 0;
               int deep_matches2 = 0;

               for (auto& arr : darray1) {
                  deep_matches1 += (DenseCast(pack.template GetAt<E>(i)) == DenseCast(*arr));
                  shallow_matches1 += (*pack.template GetAt<E>(i) == *arr);
               }

               for (auto& arr : darray2) {
                  deep_matches2 += (DenseCast(pack.template GetAt<E>(i)) == DenseCast(*arr));
                  shallow_matches2 += (*pack.template GetAt<E>(i) == *arr);
               }
               
               bool cond1 = shallow_matches1 == 1 xor shallow_matches2 == 1 xor (not shallow_matches1 and not shallow_matches2);
               REQUIRE(cond1);

               if constexpr (not Akin<E, char>) {
                  bool cond2 = deep_matches1 == 1 xor deep_matches2 == 1;
                  REQUIRE(cond2);
               }

               /*deep_matches1_total += deep_matches1;
               deep_matches2_total += deep_matches2;*/
            }

            /*REQUIRE(deep_matches1_total == 5);
            REQUIRE(deep_matches2_total == 1);*/
         }
         else {
            const auto hashed_order = [] -> std::array<int, 6> {
               if constexpr (Same<E, Text>)
                  return {5,4,6,1,2,3};
               else if constexpr (Same<E, char>)
                  return {1,2,5,3,6,4};
               else
                  return {2,1,5,3,6,4};
            }();
                  
            for (int i = 0; i < 6; ++i) {
               if (hashed_order[i] <= 5) {
                  const int idx = hashed_order[i] - 1;
                  REQUIRE(*pack.template GetAt<E>(i) == *darray1[idx]);
                  if constexpr (Reffed)
                     REQUIRE(DenseCast(*darray1[idx]).GetReferences() == 1);
               }
               else {
                  const int idx = hashed_order[i] - 6;
                  REQUIRE(*pack.template GetAt<E>(i) == *darray2[idx]);
                  if constexpr (Reffed)
                     REQUIRE(DenseCast(*darray2[idx]).GetReferences() == 1);
               }
            }
         }

         BenchmarkSetStd("Empty/Merge/Element", 30, 100,
            T temp,              temp >> immovable[0],
            stdset temp_std,     temp_std.emplace_front(immovable[0])
         );
      }
   }

   REQUIRE(memoryState.Assert());
   REQUIRE_FALSE(Allocator::CollectGarbage());
}