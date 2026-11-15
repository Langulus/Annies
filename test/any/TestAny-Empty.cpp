///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "TestAnyCommon.hpp"
#include "../handle/TestHandleCommon.hpp"
#include <Langulus/Many.hpp>


namespace Langulus::Annies  
{
   // Explicit instantiation for using extern templates in other tests  
   template struct TAny<Text>;
   template struct TAny<int>;
   template struct TAny<Any>;
   template struct TAny<RT>;
   template struct TAny<char>;

   template struct TAny<Text*>;
   template struct TAny<int*>;
   template struct TAny<Any*>;
   template struct TAny<RT*>;
   template struct TAny<char*>;

   template struct TAny<Text**>;
   template struct TAny<int**>;
   template struct TAny<Any**>;
   template struct TAny<RT**>;
   template struct TAny<char**>;

#if LANGULUS_FEATURE(MANAGED_MEMORY)
   template struct TAny<pptr8>;
   template struct TAny<pptr16>;
   template struct TAny<pptr32>;
#endif
}


TEST_CASE_TEMPLATE("Test empty Any/TAny", TestType
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
   //TODO pointers to packed pointers?
   //TODO pointers to incompletes?
   //TODO aggregates?
   #endif
) {
   static MemoryState memoryState;
   using T = typename TestType::First;
   using ScopedE = typename TestType::Second;
   using E = TypeOf<ScopedE>;

   constexpr bool Managed   = ScopedE::Managed;
   //constexpr bool Sparse    = CT::Sparse<E>;
   //constexpr bool Reffed    = CT::Referenced<Decay<E>>;
   constexpr bool Ambiguous = not Same<T, E> and CT::DeepDense<E> and LANGULUS(SAFE);
   
   #if LANGULUS(BENCHMARK)
      using stdany = ::std::any;
   #endif

   if constexpr (CT::TypeErased<T>) {
      // All type-erased containers should have all intent              
      // constructors and assigners available, and errors will instead  
      // be thrown as exceptions at runtime                             
      static_assert(Exact<TypeOf<T>, void>);
      static_assert(CT::Untyped<T>);

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
      static_assert(Exact<TypeOf<T>, E>);
      static_assert(CT::Typed<T>);
      static_assert(CT::Comparable<TypeOf<T>, E>);

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

   static_assert(    CT::Deep<T>);
   static_assert(    CT::ContainsOne<T>);
   static_assert(not CT::ContainsMany<T>);
   static_assert(not CT::Handle<T>);
   static_assert(    CT::HasVariableCount<T>);
   static_assert(    CT::HeapAllocated<T>);
   static_assert(    CT::OwnedDeep<T> == (CT::TypeErased<T> or CT::Sparse<TypeOf<T>>));
   static_assert(    CT::Owned<T>);
   static_assert(    CT::OwnedStrong<T>);
   static_assert(    CT::Comparable<T, T>);
   static_assert(    CT::Comparable<T, E>);
   static_assert(not ::std::ranges::range<T>);

   static_assert(    requires (T pack)         { pack.Get(); });
   static_assert(    requires (T pack)         { pack.template As<E>(); });
   //static_assert(    requires (T pack)         { pack.GetDeep(); });
   static_assert(    requires (T pack)         { pack.GetResolved(); });
   static_assert(    requires (T pack)         { pack.GetDense(); });
   static_assert(not requires (T pack)         { pack + pack; });
   static_assert(    CT::TextRange<E> or not requires (T pack, E item){  pack + item; });
   static_assert(not CT::TextRange<E> or     requires (T pack, E item){ {pack + item} -> CT::Text; });
   static_assert(not requires (T pack)         { {pack +=  pack} -> ::std::same_as<T&>; });
   static_assert(not requires (T pack, E item) { {pack +=  item} -> ::std::same_as<T&>; });
   static_assert(not requires (T pack, E item) { {pack <<  item} -> ::std::same_as<T&>; });
   static_assert(not requires (T pack, E item) { {pack >>  item} -> ::std::same_as<T&>; });
   static_assert(not requires (T pack, E item) { {pack <<= item} -> ::std::same_as<T&>; });
   static_assert(not requires (T pack, E item) { {pack >>= item} -> ::std::same_as<T&>; });
   static_assert(not requires (T pack, E item) { pack.InsertAt(Index::Back, item); });
   static_assert(not requires (T pack, E item) { pack.EmplaceAt(Index::Back, item); });
   static_assert(not requires (T pack)         { pack.ConcatAt(Index::Back, pack); });
   static_assert(not requires (T pack)         { pack.Concat(pack); });
   static_assert(not requires (T pack, E item) { pack.MergeAt(Index::Back, item); });
   static_assert(not requires (T pack)         { pack.MergeRangeAt(Index::Back, pack); });
   static_assert(not requires (T pack, E item) { pack.Merge(item); });
   static_assert(not requires (T pack)         { pack.MergeRange(pack); });
   static_assert(not requires (T pack, E item) { pack.Erase(item); });
   static_assert(not requires (T pack)         { pack.EraseAt(Index::Front); });
   static_assert(not requires (T pack)         { pack.Reserve(20); });
   static_assert(not requires (T pack)         { pack.EnableOr(); });
   static_assert(not requires (T pack)         { pack.IsOr(); });
   static_assert(not requires (T pack, E item) { pack.Find(item); });
   static_assert(not requires (T pack)         { pack.ForEach([](const int&) {}); });
   static_assert(not requires (T pack)         { pack.ForEachRev([](const int&) {}); });

   static_assert(T::CountHeapProviders() == 1);
   //static_assert(T::template CountHeapFooterRequests<0>() == 1);
   
   GIVEN("Default-constructed container") {
      const ScopedE element {555};
      T pack;

      /// MARK: Gap test                                                      
      WHEN("Gap test") {
         Common_GapTest<T, ::std::any>();
         //static_assert(sizeof(T) <= sizeof(::std::any)); // G++ implements std::any entirely on the heap, and I refuse to do it like this
      }

      WHEN("Default-constructed") {
         Any_CheckState_Default<E>(pack);
      
         if constexpr (Ambiguous) {
            WHEN("Ambiguous assign value by referral") {
               REQUIRE_THROWS(pack = *element);
               REQUIRE_THROWS(pack = Refer(*element));
            }
         }

         BenchmarkAnyStd("Empty/DefaultConstructor", 30, 40,
            T temp,                 new (&temp)     T{},
            stdany temp_std,        new (&temp_std) stdany{}
         );
      }

      /// MARK: Assign/Refer                                                  
      WHEN("Assigned value by referral") {
         REQUIRE_NOTHROW(pack.Assign(*element));

         Any_CheckState_OwnedFull<E>(pack);
         Any_CheckState_ContainsOne(pack, Refer(element));

         BenchmarkAnyStd("Empty/Assign/Refer", 30, 100,
            T temp,                 temp.Assign(*element),
            stdany temp_std,        temp_std = *element
         );
      }

      if constexpr (CT::ContainsOne<E>) {
         WHEN("Assigned and absorbed by referral") {
            if (CT::Typed<T> and not pack.IsSame(element->GetType())) {
               const auto element_backup = *element;
               REQUIRE_THROWS(pack.AssignAbsorb(*element));
               Any_CheckState_Default<E>(pack);
               Any_Helper_TestSame(element_backup, *element);
               return;
            }

            REQUIRE_NOTHROW(pack.AssignAbsorb(*element));

            Any_Helper_TestSame(pack, *element);         
            REQUIRE(pack.GetUses() == element->GetUses());
            REQUIRE(pack.GetUses() == 2);
            REQUIRE(pack.GetAllocation() == element->GetAllocation());

            BenchmarkAnyStd("Empty/AssignAbsorb/Refer", 30, 100,
               T temp,                    temp.AssignAbsorb(*element),
               stdany src_std{*element};
               stdany temp_std,           temp_std = src_std;
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
         
         if constexpr (CT::Container<E>)
            Any_CheckState_Default<TypeOf<E>>(movable);

         Any_CheckState_OwnedFull<E>(pack);
         Any_CheckState_ContainsOne(pack, Refer(element));

         BenchmarkAnyStd("Empty/Assign/Move", 30, 100,
            auto movable = *element;
            T temp,                       temp.Assign(::std::move(movable)),
            auto movable = *element;
            stdany temp_std,              temp_std = ::std::move(movable)
         );
      }

      if constexpr (CT::ContainsOne<E>) {
         WHEN("Assigned and absorbed by move") {
            auto movable = *element;

            if (CT::Typed<T> and not pack.IsSame(element->GetType())) {
               REQUIRE_THROWS(pack.AssignAbsorb(::std::move(movable)));
               Any_CheckState_Default<E>(pack);
               Any_Helper_TestSame(movable, *element);
               return;
            }

            REQUIRE_NOTHROW(pack.AssignAbsorb(::std::move(movable)));

            //if constexpr (CT::Container<E>)
            Any_CheckState_Default<TypeOf<E>>(movable);
            Any_Helper_TestSame(pack, *element);
            REQUIRE(pack.GetUses() == element->GetUses());
            REQUIRE(pack.GetUses() == 2);
            REQUIRE(pack.GetAllocation() == element->GetAllocation());

            BenchmarkAnyStd("Empty/AssignAbsorb/Move", 30, 100,
               auto movable = *element;
               T temp,                       temp.AssignAbsorb(::std::move(movable)),
               stdany movable = 555;
               stdany temp_std,              temp_std = ::std::move(movable)
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

         Any_CheckState_OwnedFull<E>(pack);
         Any_CheckState_ContainsOne(pack, Copy(element));

         BenchmarkAnyStd("Empty/Assign/Copy", 30, 100,
            T temp,                 temp.Assign(Copy(*element)),
            stdany temp_std,        temp_std = *element
         );
      }

      if constexpr (CT::ContainsOne<E>) {
         WHEN("Assigned and absorbed copied value") {
            if (CT::Typed<T> and not pack.IsSame(element->GetType())) {
               const auto element_backup = *element;
               REQUIRE_THROWS(pack.AssignAbsorb(Copy(*element)));
               Any_CheckState_Default<E>(pack);
               Any_Helper_TestSame(element_backup, *element);
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

            BenchmarkAnyStd("Empty/AssignAbsorb/Copy", 30, 100,
               T temp,                       temp.AssignAbsorb(Copy(*element)),
               stdany src_std = *element;
               stdany temp_std,              temp_std = src_std
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

         Any_CheckState_OwnedFull<E>(pack);
         Any_CheckState_ContainsOne(pack, Clone(element));

         BenchmarkAnyStd("Empty/Assign/Clone", 30, 100,
            T temp,                 temp.Assign(Clone(*element)),
            stdany temp_std,        temp_std = *element
         );
      }

      if constexpr (CT::ContainsOne<E>) {
         WHEN("Assigned and absorbed cloned value") {
            if (CT::Typed<T> and not pack.IsSame(element->GetType())) {
               const auto element_backup = *element;
               REQUIRE_THROWS(pack.AssignAbsorb(Clone(*element)));
               Any_CheckState_Default<E>(pack);
               Any_Helper_TestSame(element_backup, *element);
               return;
            }

            pack.AssignAbsorb(Clone(*element));
            Any_CheckState_OwnedFull<int>(*element);
            Any_CheckState_OwnedFull<int>(pack);

            REQUIRE(pack.GetRaw() != element->GetRaw());
            REQUIRE(pack.IsExact(element->GetType()));
            REQUIRE(pack == *element);
            REQUIRE(pack.IsDeep() == element->IsDeep());
            REQUIRE_FALSE(pack.IsConstant());
            REQUIRE(pack.GetUnconstrainedState() == element->GetUnconstrainedState());
            REQUIRE(pack.GetUses() == 1);
            REQUIRE(pack.GetAllocation() != element->GetAllocation());

            BenchmarkAnyStd("Empty/AssignAbsorb/Clone", 30, 100,
               T temp,                       temp.AssignAbsorb(Clone(*element)),
               stdany src_std = *element;
               stdany temp_std,              temp_std = src_std
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

         Any_CheckState_OwnedFull<E>(pack);
         Any_CheckState_ContainsOne(pack, Disown(element));

         BenchmarkAnyStd("Empty/Assign/Disown", 30, 100,
            T temp,                 temp.Assign(Disown(*element)),
            stdany temp_std,        temp_std = *element
         );
      }

      if constexpr (CT::ContainsOne<E>) {
         WHEN("Assigned and absorbed disowned value") {
            if (CT::Typed<T> and not pack.IsSame(element->GetType())) {
               const auto element_backup = *element;
               REQUIRE_THROWS(pack.AssignAbsorb(Disown(*element)));
               Any_CheckState_Default<E>(pack);
               Any_Helper_TestSame(element_backup, *element);
               return;
            }

            REQUIRE_NOTHROW(pack.AssignAbsorb(Disown(*element)));

            Any_CheckState_OwnedFull<int>(*element);
            Any_CheckState_DisownedFull<int>(pack);
            Any_Helper_TestSame(pack, *element, false);
            REQUIRE(pack.IsConstant());

            BenchmarkAnyStd("Empty/AssignAbsorb/Disown", 30, 100,
               T temp,                       temp.AssignAbsorb(Disown(*element)),
               stdany src_std = *element;
               stdany temp_std,              temp_std = src_std
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

         if constexpr (CT::Container<E>)
            Any_CheckState_Abandoned<E>(movable);
         Any_CheckState_OwnedFull<E>(pack);
         Any_CheckState_ContainsOne(pack, Refer(element));

         BenchmarkAnyStd("Empty/Assign/Abandon", 30, 100,
            auto movable = *element;
            T temp,                       temp.Assign(Abandon(movable)),
            auto movable = *element;
            stdany temp_std,              temp_std = ::std::move(movable)
         );
      }

      if constexpr (CT::ContainsOne<E>) {
         WHEN("Assigned and absorbed abandoned value") {
            auto movable = *element;

            if (CT::Typed<T> and not pack.IsSame(element->GetType())) {
               REQUIRE_THROWS(pack.AssignAbsorb(Abandon(movable)));
               Any_CheckState_Default<E>(pack);
               Any_Helper_TestSame(movable, *element);
               return;
            }

            REQUIRE_NOTHROW(pack.AssignAbsorb(Abandon(movable)));

            Any_CheckState_Abandoned<E>(movable);
            Any_Helper_TestSame(pack, *element);
            REQUIRE(pack.GetUses() == 2);
            REQUIRE(pack.GetAllocation() == element->GetAllocation());

            BenchmarkAnyStd("Empty/AssignAbsorb/Abandon", 30, 100,
               auto movable = *element;
               T temp,                          temp.AssignAbsorb(Abandon(movable)),
               stdany movable = 555;
               stdany temp_std,                 temp_std = ::std::move(movable)
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

         Any_CheckState_Default<E>(pack);
      }

      /// MARK: Emplace                                                       
      WHEN("Emplace (insert)") {
         ScopedE i666 {666};
         const auto i666backup = *i666;
         decltype(auto) instance = pack.Emplace(::std::move(*i666));
         Any_CheckState_OwnedFull<E>(pack);
         if constexpr (CT::Handle<decltype(instance)>)
            REQUIRE(instance.CompareOneEqual(i666backup));
         else
            REQUIRE(instance == i666backup);
         REQUIRE(pack.GetCount() == 1);
         REQUIRE(pack.GetReserved() >= 1);

         if constexpr (not CT::TypeErased<T>) {
            REQUIRE(*pack == i666backup);
            if constexpr (CT::Handle<decltype(instance)>)
               REQUIRE(&*pack == &*instance);
            else
               REQUIRE(&*pack == &instance);
         }

         BenchmarkAny("Empty/Emplace", 30,
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
         Many descriptor {Piecewise, ::std::move(*i666)};
         if constexpr (CT::DescribeConstructible<E> and not CT::Container<T>) {
            decltype(auto) instance = pack.template Emplace<E>(Describe{descriptor});
            Any_CheckState_OwnedFull<E>(pack);
            REQUIRE(instance.CompareOneEqual(i666backup));
            REQUIRE(pack.GetCount() == 1);
            REQUIRE(pack.GetReserved() >= 1);

            BenchmarkAny("Empty/Emplace/Describe", 30,
               T temp,
               temp.Emplace(Describe{descriptor})
            );
         }
         else if constexpr (CT::TypeErased<T>) {
            pack.template SetType<E>();
            REQUIRE_THROWS(pack.Emplace(Describe{descriptor}));
            Any_CheckState_Default<E>(pack, true);
         }
      }

      /// MARK: Clear                                                         
      WHEN("Cleared") {
         REQUIRE_NOTHROW(pack.Clear());

         Any_CheckState_Default<E>(pack);

         BenchmarkAnyStd("Empty/Clear", 30, 100,
            T temp,                 temp.Clear(),
            stdany temp_std,        temp_std.reset()
         );
      }

      /// MARK: Reset                                                         
      WHEN("Reset") {
         REQUIRE_NOTHROW(pack.Reset());

         Any_CheckState_Default<E>(pack);

         BenchmarkAnyStd("Empty/Reset", 30, 100,
            T temp,                 temp.Reset(),
            stdany temp_std,        temp_std.reset()
         );
      }

      WHEN("Referred empty") {
         T refer1 = pack;

         Any_Helper_TestSame(refer1, pack);
         Any_CheckState_Default<E>(refer1);
         Any_CheckState_Default<E>(pack);

         T refer2 = Refer(pack);

         Any_Helper_TestSame(refer2, pack);
         Any_CheckState_Default<E>(refer2);
         Any_CheckState_Default<E>(pack);
      }

      WHEN("Cloned empty") {
         T clone = Clone(pack);

         Any_Helper_TestSame(clone, pack);
         Any_CheckState_Default<E>(clone);
         Any_CheckState_Default<E>(pack);
      }

      WHEN("Disowned empty") {
         T disowned = Disown(pack);

         Any_Helper_TestSame(disowned, pack);
         Any_CheckState_Default<E>(disowned);
         Any_CheckState_Default<E>(pack);
      }

      WHEN("Copied empty") {
         T copy = Copy(pack);

         Any_Helper_TestSame(copy, pack);
         Any_CheckState_Default<E>(copy);
         Any_CheckState_Default<E>(pack);
      }

      WHEN("Moved empty") {
         T movable1 = pack;
         const T moved1 = ::std::move(movable1);

         Any_CheckState_Default<E>(movable1);
         Any_Helper_TestSame(moved1, pack);
         Any_CheckState_Default<E>(moved1);
         Any_CheckState_Default<E>(pack);

         T movable2 = pack;
         const T moved2 = Move(movable2);

         Any_CheckState_Default<E>(movable2);
         Any_Helper_TestSame(moved2, pack);
         Any_CheckState_Default<E>(moved2);
         Any_CheckState_Default<E>(pack);
      }

      WHEN("Abandoned empty") {
         T movable = pack;
         const T moved = Abandon(movable);

         Any_CheckState_Default<E>(movable);
         Any_Helper_TestSame(moved, pack);
         Any_CheckState_Default<E>(moved);
         Any_CheckState_Default<E>(pack);
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
         BenchmarkAny("Empty/operator==", 30,
            (void) 0, dont_optimize |= (another_pack1 == another_pack2)
         );
         BenchmarkAny("Empty/operator!=", 30,
            (void) 0, dont_optimize |= (another_pack1 != another_pack2)
         );
      }

      /// MARK: Contains                                                      
      WHEN("Contains when empty") {
         REQUIRE_FALSE(pack.Contains(*element));

         [[maybe_unused]] volatile bool dont_optimize = false;
         BenchmarkAny("Empty/Contains", 30,
            (void) 0, dont_optimize |= pack.Contains(*element)
         );
      }

      if constexpr (Exact<E, Text>) {
         WHEN("Given text that will be destroyed before the pack") {
            Text owned_text = "666";
            pack = Text(owned_text.operator Token());
         }
      }

      /// MARK: Handles                                                       
      WHEN("GetHandle is called on mutable container") {
         auto h = pack.GetHandle();

         #if not LANGULUS(FORCE_TYPE_ERASURE)
            if constexpr (CT::Untyped<T>)
               static_assert(::std::same_as<decltype(h), HandleMut>);
            else
               static_assert(::std::same_as<decltype(h), THandle<E&>>);
         #else
            static_assert(::std::same_as<decltype(h), HandleMut>);
         #endif

         Handle_CheckState_Default<E>(h);
      }

      WHEN("GetHandle is called on constant container") {
         T const pack_constant;
         auto h = pack_constant.GetHandle();

         #if not LANGULUS(FORCE_TYPE_ERASURE)
            if constexpr (CT::Untyped<T>)
               static_assert(::std::same_as<decltype(h), Handle>);
            else
               static_assert(::std::same_as<decltype(h), THandle<ConstAll<E&>>>);
         #else
            static_assert(::std::same_as<decltype(h), Handle>);
         #endif

         Handle_CheckState_Default<E const>(h);
      }
   }

   REQUIRE(memoryState.Assert());
   REQUIRE_FALSE(Allocator::CollectGarbage());
}
