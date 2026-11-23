///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "TestHiveCommon.hpp"
#include "../handle/TestHandleCommon.hpp"


namespace Langulus::Annies  
{
   // Explicit instantiation for using extern templates in other tests  
   template struct THive<Text>;
   template struct THive<int>;
   template struct THive<Any>;
   template struct THive<RT>;
   template struct THive<char>;

   template struct THive<Text*>;
   template struct THive<int*>;
   template struct THive<Any*>;
   template struct THive<RT*>;
   template struct THive<char*>;

   template struct THive<Text**>;
   template struct THive<int**>;
   template struct THive<Any**>;
   template struct THive<RT**>;
   template struct THive<char**>;

#if LANGULUS_FEATURE(MANAGED_MEMORY)
   template struct THive<pptr8>;
   template struct THive<pptr16>;
   template struct THive<pptr32>;
#endif
}


TEST_CASE_TEMPLATE("Test empty hive", TestType
   // Elements are not allocated by the memory manager                  
   , Types<THive<Text>,   ScopedElement<Text>>
   , Types<THive<int>,    ScopedElement<int>>
   , Types<THive<Many>,   ScopedElement<Many>>
   , Types<THive<RT>,     ScopedElement<RT>>
   , Types<THive<char>,   ScopedElement<char>>

   , Types<THive<Text*>,  ScopedElement<Text*>>
   , Types<THive<int*>,   ScopedElement<int*>>
   , Types<THive<Many*>,  ScopedElement<Many*>>
   , Types<THive<RT*>,    ScopedElement<RT*>>
   , Types<THive<char*>,  ScopedElement<char*>>

   , Types<THive<Text**>, ScopedElement<Text**>>
   , Types<THive<int**>,  ScopedElement<int**>>
   , Types<THive<Many**>, ScopedElement<Many**>>
   , Types<THive<RT**>,   ScopedElement<RT**>>
   , Types<THive<char**>, ScopedElement<char**>>

   #if LANGULUS_FEATURE(MANAGED_MEMORY)
   // Elements are allocated by the memory manager                      
   , Types<THive<Text>,   ScopedElement<Text, true>>
   , Types<THive<int>,    ScopedElement<int, true>>
   , Types<THive<Many>,   ScopedElement<Many, true>>
   , Types<THive<RT>,     ScopedElement<RT, true>>
   , Types<THive<char>,   ScopedElement<char, true>>

   , Types<THive<Text*>,  ScopedElement<Text*, true>>
   , Types<THive<int*>,   ScopedElement<int*, true>>
   , Types<THive<Many*>,  ScopedElement<Many*, true>>
   , Types<THive<RT*>,    ScopedElement<RT*, true>>
   , Types<THive<char*>,  ScopedElement<char*, true>>

   , Types<THive<Text**>, ScopedElement<Text**, true>>
   , Types<THive<int**>,  ScopedElement<int**, true>>
   , Types<THive<Many**>, ScopedElement<Many**, true>>
   , Types<THive<RT**>,   ScopedElement<RT**, true>>
   , Types<THive<char**>, ScopedElement<char**, true>>

   // Packed pointers                                                   
   , Types<THive<pptr8>,  ScopedElementPacked<pptr8>>
   , Types<THive<pptr16>, ScopedElementPacked<pptr16>>
   , Types<THive<pptr32>, ScopedElementPacked<pptr32>>
   #endif
) {
   static MemoryState memoryState;
   using T = typename TestType::First;
   using ScopedE = typename TestType::Second;
   using E = TypeOf<ScopedE>;

   constexpr bool Managed   = ScopedE::Managed;
   constexpr bool Sparse    = CT::Sparse<E>;
   //constexpr bool Reffed    = CT::Referenced<Decay<E>>;
   constexpr bool Ambiguous = false;//not Same<T, E> and CT::DeepDense<E> and LANGULUS(SAFE);

   #if LANGULUS(BENCHMARK)
      using stdvec = ::std::vector<E>;
   #endif

   // Statically-typed containers behave the same as their inner     
   // type                                                           
   static_assert(    Exact<TypeOf<T>, E>);
   static_assert(not CT::TypeErased<T>);
   static_assert(    CT::Comparable<TypeOf<T>, E>);

   static_assert(    CT::CopyConstructible<T>    == CT::CopyConstructible<E>);
   static_assert(not CT::ReferConstructible<T>   /*== CT::ReferConstructible<E>*/);
   static_assert(    CT::AbandonConstructible<T> == CT::AbandonConstructible<E>);
   static_assert(    CT::MoveConstructible<T>    == CT::MoveConstructible<E>);
   static_assert(    CT::CloneConstructible<T>   == CT::CloneConstructible<E>);
   static_assert(not CT::DisownConstructible<T>  /*== CT::DisownConstructible<E>*/);

   static_assert(    CT::CopyAssignable<T>       == CT::CopyAssignable<E>);
   static_assert(not CT::ReferAssignable<T>      /*== CT::ReferAssignable<E>*/);
   static_assert(    CT::AbandonAssignable<T>    == CT::AbandonAssignable<E>);
   static_assert(    CT::MoveAssignable<T>       == CT::MoveAssignable<E>);
   static_assert(    CT::CloneAssignable<T>      == CT::CloneAssignable<E>);
   static_assert(not CT::DisownAssignable<T>     /*== CT::DisownAssignable<E>*/); 
   
   {
      static_assert(    CT::Deep<T>);
      static_assert(not CT::ContainsOne<T>);
      static_assert(    CT::ContainsMany<T>);
      static_assert(not CT::Handle<T>);
      static_assert(    CT::HasVariableCount<T>);
      static_assert(    CT::HeapAllocated<T>);
      static_assert(    CT::OwnedDeep<T> == CT::Sparse<TypeOf<T>>);
      static_assert(    CT::Owned<T>);
      static_assert(    CT::OwnedStrong<T>);
      static_assert(    CT::Comparable<T, T>);
      static_assert(    CT::Comparable<T, E>);

      static_assert(::std::ranges::range<T>);

      static_assert(not CT::Contiguous<T>);

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
      static_assert(not requires (T pack)         { pack.template As<E>(); });
      //static_assert(not requires (T pack)         { pack.GetDeep(); });
      static_assert(not requires (T pack)         { pack.GetResolved(); });
      static_assert(not requires (T pack)         { pack.GetDense(); });
      static_assert(not requires (T pack)         { pack + pack; });
      static_assert(    CT::TextRange<E> or not requires (T pack, E item){  pack + item; });
      static_assert(    CT::TextRange<E> or not requires (T pack, E item){  item + pack; });
      static_assert(not CT::TextRange<E> or     requires (T pack, E item){ {pack + item} -> CT::Text; });
      static_assert(not CT::TextRange<E> or     requires (T pack, E item){ {item + pack} -> CT::Text; });
      static_assert(not requires (T pack)         { pack +=  pack; });
      static_assert(not requires (T pack, E item) { pack +=  item; });
      static_assert(not requires (T pack, E item) { pack <<  item; });
      static_assert(not requires (T pack, E item) { pack >>  item; });
      static_assert(not requires (T pack, E item) { pack <<= item; });
      static_assert(not requires (T pack, E item) { pack >>= item; });
      static_assert(not requires (T pack, E item) { pack.InsertAt(Index::Back, item); });
      static_assert(not requires (T pack, E item) { pack.EmplaceAt(Index::Back, item); });
      static_assert(not requires (T pack)         { pack.ConcatAt(Index::Back, pack); });
      static_assert(not requires (T pack)         { pack.Concat(pack); });
      static_assert(not requires (T pack, E item) { pack.MergeAt(Index::Back, item); });
      static_assert(not requires (T pack)         { pack.MergeRangeAt(Index::Back, pack); });
      static_assert(not requires (T pack, E item) { pack.Merge(item); });
      static_assert(not requires (T pack)         { pack.MergeRange(pack); });
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
         Common_GapTest<T, ::std::vector<E>>();
         //static_assert(sizeof(T) <= sizeof(::std::vector<E>)); // bigger, because it precomputes and stores a hash on the stack
      }

      WHEN("Default-constructed") {
         Hive_CheckState_Default<E>(pack);
      
         if constexpr (Ambiguous) {
            WHEN("Ambiguous assign value by referral") {
               REQUIRE_THROWS(pack = *element);
               REQUIRE_THROWS(pack = Refer(*element));
            }
         }

         /*BenchmarkManyStd("Empty/DefaultConstructor", 30, 40,
            T temp,                 new (&temp)     T{},
            stdvec temp_std,        new (&temp_std) stdvec{}
         );*/
      }

      /// MARK: Assign/Refer                                                  
      WHEN("Assigned value by referral") {
         REQUIRE_NOTHROW(pack.Assign(*element));

         Hive_CheckState_OwnedFull<E>(pack);
         Hive_CheckState_ContainsOne(pack, Refer(element));

         /*BenchmarkManyStd("Empty/Assign/Refer", 30, 100,
            T temp,                 temp.Assign(*element),
            stdvec temp_std,        temp_std.emplace_back(*element)
         );*/
      }

      /*if constexpr (CT::DeepDense<E>) {
         WHEN("Assigned and absorbed by referral") {
            if (CT::Typed<T> and not pack.IsSame(element->GetType())) {
               const auto element_backup = *element;
               REQUIRE_THROWS(pack.AssignAbsorb(*element));
               Hive_CheckState_Default<E>(pack);
               Hive_Helper_TestSame(element_backup, *element);
               return;
            }

            REQUIRE_NOTHROW(pack.AssignAbsorb(*element));

            Hive_Helper_TestSame(pack, *element);
            REQUIRE(pack.GetUses() == element->GetUses());
            REQUIRE(pack.GetUses() == 2);
            REQUIRE(pack.GetAllocation() == element->GetAllocation());

            *//*BenchmarkManyStd("Empty/AssignAbsorb/Refer", 30, 100,
               T temp,                         temp.AssignAbsorb(*element),
               stdvec src_std (1, *element);
               stdvec temp_std,                temp_std = src_std;
            );*//*
         }
      }*/

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
         
         if constexpr (CT::DeepDense<E>)
            Hive_CheckState_Default<TypeOf<E>>(movable);

         Hive_CheckState_OwnedFull<E>(pack);
         Hive_CheckState_ContainsOne(pack, Refer(element));

         /*BenchmarkManyStd("Empty/Assign/Move", 30, 100,
            auto movable = *element;
            T temp,                       temp.Assign(::std::move(movable)),
            auto movable = *element;
            stdvec temp_std,              temp_std.emplace_back(::std::move(movable))
         );*/
      }

      /*if constexpr (CT::DeepDense<E>) {
         WHEN("Assigned and absorbed by move") {
            auto movable = *element;

            if (CT::Typed<T> and not pack.IsSame(element->GetType())) {
               REQUIRE_THROWS(pack.AssignAbsorb(::std::move(movable)));
               Hive_CheckState_Default<E>(pack);
               Hive_Helper_TestSame(movable, *element);
               return;
            }

            REQUIRE_NOTHROW(pack.AssignAbsorb(::std::move(movable)));

            Hive_CheckState_Default<TypeOf<E>>(movable);
            Hive_Helper_TestSame(pack, *element);
            REQUIRE(pack.GetUses() == element->GetUses());
            REQUIRE(pack.GetUses() == 2);
            REQUIRE(pack.GetAllocation() == element->GetAllocation());

            *//*BenchmarkManyStd("Empty/AssignAbsorb/Move", 30, 100,
               auto movable = *element;
               T temp,                       temp.AssignAbsorb(::std::move(movable)),
               stdvec movable (1, 555);
               stdvec temp_std,              temp_std.emplace_back(::std::move(movable))
            );*//*
         }
      }*/

      /// MARK: Assign/Copy                                                   
      if constexpr (Ambiguous) {
         WHEN("Ambiguous assign copied value") {
            REQUIRE_THROWS(pack = Copy(*element));
         }
      }
      
      WHEN("Assigned value by copy") {
         REQUIRE_NOTHROW(pack.Assign(Copy(*element)));

         Hive_CheckState_OwnedFull<E>(pack);
         Hive_CheckState_ContainsOne(pack, Copy(element));

         /*BenchmarkManyStd("Empty/Assign/Copy", 30, 100,
            T temp,              temp.Assign(Copy(*element)),
            stdvec temp_std,     temp_std.emplace_back(*element)
         );*/
      }

      /*if constexpr (CT::DeepDense<E>) {
         WHEN("Assigned and absorbed copied value") {
            if (CT::Typed<T> and not pack.IsSame(element->GetType())) {
               const auto element_backup = *element;
               REQUIRE_THROWS(pack.AssignAbsorb(Copy(*element)));
               Hive_CheckState_Default<E>(pack);
               Hive_Helper_TestSame(element_backup, *element);
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

            *//*BenchmarkManyStd("Empty/AssignAbsorb/Copy", 30, 100,
               T temp,                          temp.AssignAbsorb(Copy(*element)),
               stdvec src_std (1, *element);
               stdvec temp_std,                 temp_std = src_std
            );*//*
         }
      }*/

      /// MARK: Assign/Clone                                                  
      if constexpr (Ambiguous) {
         WHEN("Ambiguous assign cloned value") {
            REQUIRE_THROWS(pack = Clone(*element));
         }
      }
      
      WHEN("Assigned value by clone") {
         REQUIRE_NOTHROW(pack.Assign(Clone(*element)));

         Hive_CheckState_OwnedFull<E>(pack);
         Hive_CheckState_ContainsOne(pack, Clone(element));

         /*BenchmarkManyStd("Empty/Assign/Clone", 30, 100,
            T temp,                 temp.Assign(Clone(*element)),
            stdvec temp_std,        temp_std.emplace_back(*element)
         );*/
      }

      /*if constexpr (CT::DeepDense<E>) {
         WHEN("Assigned and absorbed cloned value") {
            if (CT::Typed<T> and not pack.IsSame(element->GetType())) {
               const auto element_backup = *element;
               REQUIRE_THROWS(pack.AssignAbsorb(Clone(*element)));
               Hive_CheckState_Default<E>(pack);
               Hive_Helper_TestSame(element_backup, *element);
               return;
            }

            REQUIRE_NOTHROW(pack.AssignAbsorb(Clone(*element)));
            Hive_CheckState_OwnedFull<int>(*element);
            Hive_CheckState_OwnedFull<int>(pack);

            REQUIRE(pack.GetRaw() != element->GetRaw());
            REQUIRE(pack.IsExact(element->GetType()));
            REQUIRE(pack == *element);
            REQUIRE(pack.IsDeep() == element->IsDeep());
            REQUIRE_FALSE(pack.IsConstant());
            REQUIRE(pack.GetUnconstrainedState() == element->GetUnconstrainedState());
            REQUIRE(pack.GetUses() == 1);
            REQUIRE(pack.GetAllocation());

            *//*BenchmarkManyStd("Empty/AssignAbsorb/Clone", 30, 100,
               T temp,                          temp.AssignAbsorb(Clone(*element)),
               stdvec src_std (1, *element);
               stdvec temp_std,                 temp_std = src_std
            );*//*
         }
      }*/

      /// MARK: Assign/Disown                                                 
      if constexpr (Ambiguous) {
         WHEN("Ambiguous assign disowned value") {
            REQUIRE_THROWS(pack = Disown(*element));
         }
      }
      
      WHEN("Assigned value by disown") {
         REQUIRE_NOTHROW(pack.Assign(Disown(*element)));

         Hive_CheckState_OwnedFull<E>(pack);
         Hive_CheckState_ContainsOne(pack, Disown(element));

         /*BenchmarkManyStd("Empty/Assign/Disown", 30, 100,
            T temp,                 temp.Assign(Disown(*element)),
            stdvec temp_std,        temp_std.emplace_back(*element)
         );*/
      }

      /*if constexpr (CT::DeepDense<E>) {
         WHEN("Assigned and absorbed disowned value") {
            if (CT::Typed<T> and not pack.IsSame(element->GetType())) {
               const auto element_backup = *element;
               REQUIRE_THROWS(pack.AssignAbsorb(Disown(*element)));
               Hive_CheckState_Default<E>(pack);
               Hive_Helper_TestSame(element_backup, *element);
               return;
            }

            REQUIRE_NOTHROW(pack.AssignAbsorb(Disown(*element)));

            Hive_CheckState_OwnedFull<int>(*element);
            Hive_CheckState_DisownedFull<int>(pack);
            Hive_Helper_TestSame(pack, *element, false);
            REQUIRE(pack.IsConstant());

            *//*BenchmarkManyStd("Empty/AssignAbsorb/Disown", 30, 100,
               T temp,                       temp.AssignAbsorb(Disown(*element)),
               stdvec src_std (1, *element);
               stdvec temp_std,              temp_std = src_std
            );*//*
         }
      }*/

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

         if constexpr (CT::DeepDense<E>)
            Hive_CheckState_Abandoned<E>(movable);
         Hive_CheckState_OwnedFull<E>(pack);
         Hive_CheckState_ContainsOne(pack, Refer(element));

         /*BenchmarkManyStd("Empty/Assign/Abandon", 30, 100,
            auto movable = *element;
            T temp,                       temp.Assign(Abandon(movable)),
            auto movable = *element;
            stdvec temp_std,              temp_std.emplace_back(::std::move(movable))
         );*/
      }

      /*if constexpr (CT::DeepDense<E>) {
         WHEN("Assigned and absorbed abandoned value") {
            auto movable = *element;

            if (CT::Typed<T> and not pack.IsSame(element->GetType())) {
               REQUIRE_THROWS(pack.AssignAbsorb(Abandon(movable)));
               Hive_CheckState_Default<E>(pack);
               Hive_Helper_TestSame(movable, *element);
               return;
            }

            REQUIRE_NOTHROW(pack.AssignAbsorb(Abandon(movable)));

            Hive_CheckState_Abandoned<E>(movable);
            Hive_Helper_TestSame(pack, *element);
            REQUIRE(pack.GetUses() == 2);
            REQUIRE(pack.GetAllocation() == element->GetAllocation());

            *//*BenchmarkManyStd("Empty/AssignAbsorb/Abandon", 30, 100,
               auto movable = *element;
               T temp,                       temp.AssignAbsorb(Abandon(movable)),
               stdvec movable (1, 555);
               stdvec temp_std,              temp_std = ::std::move(movable)
            );*//*
         }
      }*/

      /// MARK: Assign empty                                                  
      /*WHEN("Ambigous assigned empty self") {
         LglsDisableWarningPush
         LglsDisableWarning_SelfAssign
         REQUIRE_NOTHROW(pack = pack);
         LglsDisableWarningPop
      }*/
      
      WHEN("Assigned empty self") {
         REQUIRE_NOTHROW(pack.AssignAbsorb(pack));

         Hive_CheckState_Default<E>(pack);
      }

      /// MARK: Emplace                                                       
      WHEN("Emplace (insert)") {
         ScopedE i666 {666};
         const auto i666backup = *i666;
         decltype(auto) instance = pack.Emplace(::std::move(*i666));
         Hive_CheckState_OwnedFull<E>(pack);
         if constexpr (CT::Handle<decltype(instance)>)
            REQUIRE(instance.CompareOneEqual(i666backup));
         else
            REQUIRE(instance == i666backup);
         REQUIRE(pack.GetCount() == 1);
         REQUIRE(pack.GetReserved() >= 1);

         if constexpr (CT::Typed<T>) {
            REQUIRE(pack[0] == i666backup);
            if constexpr (CT::Handle<decltype(instance)>)
               REQUIRE(&pack[0] == &*instance);
            else
               REQUIRE(&pack[0] == &instance);
         }

         BenchmarkHive("Empty/Emplace", 30,
            auto movable = *element; T temp,
            temp.Emplace(::std::move(movable))
         );

         if constexpr (not Managed) {
            // On unmanaged tests i666 will be destroyed at the end of this scope,
            // and the container will be left with a dangling pointer.
            // Make sure this isn't happening. When inserting raw unmanaged pointers, 
            // safety is solely in the hands of the user.
            pack.Reset();
         }
      }

      /// MARK: Describe                                                      
      WHEN("Emplace (insert, describe)") {
         ScopedE i666{666};
         const auto i666backup = *i666;
         Neat descriptor {::std::move(*i666)};
         if constexpr (CT::DescribeConstructible<E> and not CT::Container<T>) {
            decltype(auto) instance = pack.template Emplace<E>(Describe{descriptor});
            Hive_CheckState_OwnedFull<E>(pack);
            REQUIRE(instance.CompareOneEqual(i666backup));
            REQUIRE(pack.GetCount() == 1);
            REQUIRE(pack.GetReserved() >= 1);

            BenchmarkHive("Empty/Emplace/Describe", 30,
               T temp,
               temp.Emplace(Describe{descriptor})
            );
         }
         else if constexpr (CT::TypeErased<T>) {
            pack.template SetType<E>();
            REQUIRE_THROWS(pack.Emplace(Describe{descriptor}));
            Hive_CheckState_Default<E>(pack, true);
         }
      }

      /// MARK: Clear                                                         
      WHEN("Cleared") {
         REQUIRE_NOTHROW(pack.Clear());

         Hive_CheckState_Default<E>(pack);

         /*BenchmarkManyStd("Empty/Clear", 30, 100,
            T temp,              temp.Clear(),
            stdvec temp_std,     temp_std.clear()
         );*/
      }

      /// MARK: Reset                                                         
      WHEN("Reset") {
         REQUIRE_NOTHROW(pack.Reset());

         Hive_CheckState_Default<E>(pack);

         /*BenchmarkManyStd("Empty/Reset", 30, 100,
            T temp,              temp.Reset(),
            stdvec temp_std,     temp_std.clear()
         );*/
      }

      /// MARK: Erase                                                         
      WHEN("Erase non-existent value") {
         size_t removed = 0;
         REQUIRE_NOTHROW(removed = pack.Erase(*element));

         Hive_CheckState_Default<E>(pack);

         REQUIRE(removed == 0);

         /*BenchmarkManyStd("Empty/Erase", 30, 100,
            T temp,              temp.Erase(*element),
            stdvec temp_std,     temp_std.erase(std::remove_if(temp_std.begin(), temp_std.end(), [&element] (auto& value) {
                                    return value == *element;
                                 }), temp_std.end());
         );*/
      }

      /*WHEN("Referred empty") {
         T refer1 = pack;

         Hive_Helper_TestSame(refer1, pack);
         Hive_CheckState_Default<E>(refer1);
         Hive_CheckState_Default<E>(pack);

         T refer2 = Refer(pack);

         Hive_Helper_TestSame(refer2, pack);
         Hive_CheckState_Default<E>(refer2);
         Hive_CheckState_Default<E>(pack);
      }*/

      WHEN("Cloned empty") {
         T clone = Clone(pack);

         Hive_Helper_TestSame(clone, pack);
         Hive_CheckState_Default<E>(clone);
         Hive_CheckState_Default<E>(pack);
      }

      /*WHEN("Disowned empty") {
         T disowned = Disown(pack);

         Hive_Helper_TestSame(disowned, pack);
         Hive_CheckState_Default<E>(disowned);
         Hive_CheckState_Default<E>(pack);
      }*/

      WHEN("Copied empty") {
         T copy = Copy(pack);

         Hive_Helper_TestSame(copy, pack);
         Hive_CheckState_Default<E>(copy);
         Hive_CheckState_Default<E>(pack);
      }

      WHEN("Moved empty") {
         const T moved1 = ::std::move(pack);

         Hive_Helper_TestSame(moved1, pack);
         Hive_CheckState_Default<E>(moved1);
         Hive_CheckState_Default<E>(pack);
      }

      WHEN("Moved empty (intent)") {
         const T moved2 = Move(pack);

         Hive_Helper_TestSame(moved2, pack);
         Hive_CheckState_Default<E>(moved2);
         Hive_CheckState_Default<E>(pack);
      }

      WHEN("Abandoned empty") {
         const T moved = Abandon(pack);

         Hive_Helper_TestSame(moved, pack);
         Hive_CheckState_Default<E>(moved);
         Hive_CheckState_Default<E>(pack);
      }

      /// MARK: Compare                                                       
      WHEN("Compared empty") {
         T another_pack1;
         T another_pack2;

         REQUIRE      (another_pack1 == another_pack2);
         REQUIRE_FALSE(another_pack1 != another_pack2);

         [[maybe_unused]] volatile bool dont_optimize = false;
         if constexpr (CT::TypeErased<T>) {
            // No type-erased equivalent for ::std::vector              
            BenchmarkHive("Empty/operator==", 30,
               (void) 0, dont_optimize |= (another_pack1 == another_pack2)
            );
            BenchmarkHive("Empty/operator!=", 30,
               (void) 0, dont_optimize |= (another_pack1 != another_pack2)
            );
         }
         else {
            /*BenchmarkManyStd("Empty/operator==", 30, 100,
               (void) 0,     dont_optimize |= (another_pack1 == another_pack2),
               stdvec std1;
               stdvec std2,  dont_optimize |= (std1 == std2)
            );

            BenchmarkManyStd("Empty/operator!=", 30, 100,
               (void) 0,     dont_optimize |= (another_pack1 != another_pack2),
               stdvec std1;
               stdvec std2,  dont_optimize |= (std1 != std2)
            );*/
         }
      }

      /// MARK: Contains                                                      
      WHEN("Contains when empty") {
         REQUIRE_FALSE(pack.Contains(*element));

         [[maybe_unused]] volatile bool dont_optimize = false;
         BenchmarkHive("Empty/Contains", 30,
            (void) 0, dont_optimize |= pack.Contains(*element)
         );
      }

      if constexpr (Exact<E, Text>) {
         WHEN("Given text that will be destroyed before the pack") {
            Text owned_text = "666";
            REQUIRE_NOTHROW(pack = Text(owned_text.operator Token()));
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

         size_t counter = 0;
         for (auto& it : pack) {
            (void) it;
            ++counter;

            if constexpr (CT::TypeErased<T> or Sparse)
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

            if constexpr (CT::TypeErased<T> or Sparse)
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

         size_t counter = 0;
         for (decltype(auto) it : strategy) {
            (void) it;
            ++counter;

            if constexpr (CT::TypeErased<T> or Sparse)
               static_assert(CT::Handle<decltype(it)>);
            else
               static_assert(Same<E, decltype(it)>);
         }

         for (decltype(auto) it : strategyConst) {
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
         
         size_t counter = 0;
         for (auto& it : strategy) {
            (void) it;
            ++counter;

            if constexpr (CT::TypeErased<T> or Sparse)
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
            static_assert(::std::same_as<decltype(h), HandleMut>);
         else
            static_assert(::std::same_as<decltype(h), THandle<E&>>);

         Handle_CheckState_Default<E>(h);
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
      /*const ScopedE darray1[5] {1, 2, 3, 4,  5};
      const ScopedE darray2[5] {6, 7, 8, 9, 10};*/

      /*const E immovable[5] {
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

      T pack;*/

      /// MARK: <<                                                            
      /*WHEN("Insert at the back by using << operator)") {
         REQUIRE_NOTHROW(pack <<           immovable[0]
                              << Refer    {immovable[1]}
                              << Copy     {immovable[2]}
                              << Disown   {immovable[3]}
                              << std::move(movable1[0])
                              << Move     {movable2[0]}
                              << Abandon  {movable3[0]}
                              << Clone    {immovable[4]});

         Hive_CheckState_OwnedFull<E>(pack);

         if constexpr (CT::DeepDense<E>) {
            Many_CheckState_Default<int>  (movable1[0]);
            Many_CheckState_Default<int>  (movable2[0]);
            Many_CheckState_Abandoned<int>(movable3[0]);
         }

         REQUIRE(pack.GetCount() == 8);
         REQUIRE(pack.GetReserved() >= 8);

         for (int i = 0; i < 4; ++i) {
            REQUIRE(*pack.template GetAt<E>(i) == *darray1[i]);
         }

         for (int i = 4; i < 7; ++i)
            REQUIRE(*pack.template GetAt<E>(i) == *darray2[0]);

         // Last one is cloned and pointers won't match                 
         if constexpr (Sparse) {
            REQUIRE(*pack.template GetAt<E>(7) != *darray1[4]);
            REQUIRE(DenseCast(pack.template GetAt<E>(7)) == DenseCast(*darray1[4]));
         }
         else REQUIRE(*pack.template GetAt<E>(7) == *darray1[4]);

         if constexpr (Reffed) {
            REQUIRE(DenseCast(*darray1[4]).GetReferences() == 1);
            REQUIRE(DenseCast(pack.template GetAt<E>(7)).GetReferences() == 1);
         }

         *//*BenchmarkManyStd("Empty/Insert/Element/Back", 30, 100,
            T temp,              temp << immovable[0],
            stdvec temp_std,     temp_std.emplace_back(immovable[0])
         );*//*
      }*/

      /// MARK: >>                                                            
      /*WHEN("Insert at the front by using >> operator)") {
         REQUIRE_NOTHROW(pack >>           immovable[0]
                              >> Refer    {immovable[1]}
                              >> Copy     {immovable[2]}
                              >> Disown   {immovable[3]}
                              >> std::move(movable1[0])
                              >> Move     {movable2[0]}
                              >> Abandon  {movable3[0]}
                              >> Clone    {immovable[4]});

         Hive_CheckState_OwnedFull<E>(pack);

         if constexpr (CT::DeepDense<E>) {
            Many_CheckState_Default<int>  (movable1[0]);
            Many_CheckState_Default<int>  (movable2[0]);
            Many_CheckState_Abandoned<int>(movable3[0]);
         }

         REQUIRE(pack.GetCount() == 8);
         REQUIRE(pack.GetReserved() >= 8);

         // first one is cloned and pointers won't match                
         if constexpr (Sparse) {
            REQUIRE(*pack.template GetAt<E>(0) != *darray1[4]);
            REQUIRE(DenseCast(pack.template GetAt<E>(0)) == DenseCast(*darray1[4]));
         }
         else REQUIRE(*pack.template GetAt<E>(0) == *darray1[4]);

         if constexpr (Reffed) {
            REQUIRE(DenseCast(*darray1[4]).GetReferences() == 1);
            REQUIRE(DenseCast(pack.template GetAt<E>(0)).GetReferences() == 1);
         }

         for (int i = 1; i < 4; ++i)
            REQUIRE(*pack.template GetAt<E>(i) == *darray2[0]);

         for (int i = 4; i < 8; ++i)
            REQUIRE(*pack.template GetAt<E>(i) == *darray1[4 - (i - 3)%5]);

         *//*BenchmarkManyStd("Empty/Insert/Element/Front", 30, 100,
            T temp,              temp >> immovable[0],
            stdvec temp_std,     temp_std.emplace_front(immovable[0])
         );*//*
      }*/
   }

   REQUIRE(memoryState.Assert());
   REQUIRE_FALSE(Allocator::CollectGarbage());
}