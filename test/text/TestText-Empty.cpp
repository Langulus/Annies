///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "TestTextCommon.hpp"
#include "../handle/TestHandleCommon.hpp"

#include <Langulus/Annies/Functional/Compare.hpp>

//TODO char* std::string_view std::string std::array<char> Literal

TEST_CASE_TEMPLATE("Test empty Text", TestType
   // Elements are not allocated by the memory manager                  
   , Types<Text, ScopedElement<Text>>
   , Types<Text, ScopedElement<int>>
   , Types<Text, ScopedElement<Many>>
   , Types<Text, ScopedElement<RT>>

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

   static_assert(    Exact<TypeOf<T>, char>);

   #if not LANGULUS(FORCE_TYPE_ERASURE)
      static_assert(not CT::TypeErased<T>);
      static_assert(::std::ranges::contiguous_range<T>);
   #else
      static_assert(    CT::TypeErased<T>);
   #endif

   static_assert(CT::CopyConstructible<T>    );
   static_assert(CT::ReferConstructible<T>   );
   static_assert(CT::AbandonConstructible<T> );
   static_assert(CT::MoveConstructible<T>    );
   static_assert(CT::CloneConstructible<T>   );
   static_assert(CT::DisownConstructible<T>  );

   static_assert(CT::CopyAssignable<T>       );
   static_assert(CT::ReferAssignable<T>      );
   static_assert(CT::AbandonAssignable<T>    );
   static_assert(CT::MoveAssignable<T>       );
   static_assert(CT::CloneAssignable<T>      );
   static_assert(CT::DisownAssignable<T>     );

   static_assert(not CT::Deep<T>             );
   static_assert(not CT::ContainsOne<T>      );
   static_assert(    CT::ContainsMany<T>     );
   static_assert(not CT::Handle<T>           );
   static_assert(    CT::HasVariableCount<T> );
   static_assert(    CT::HeapAllocated<T>    );
   static_assert(not CT::OwnedDeep<T>        );
   static_assert(    CT::Owned<T>            );
   static_assert(    CT::OwnedStrong<T>      );
   static_assert(    CT::Contiguous<T>       );

   static_assert(    CT::ComparableEqual<T, T>);
   static_assert(    CT::ComparableEqual<T, char>);
   static_assert(    CT::ComparableEqual<T, wchar_t>);
   static_assert(    CT::ComparableEqual<T, char8_t>);
   static_assert(    CT::ComparableEqual<T, char16_t>);
   static_assert(    CT::ComparableEqual<T, char32_t>);
   static_assert(    CT::ComparableEqual<T, ::std::array<char, 5>>);
   static_assert(    CT::ComparableEqual<T, ::std::array<wchar_t, 5>>);
   static_assert(    CT::ComparableEqual<T, ::std::array<char8_t, 5>>);
   static_assert(    CT::ComparableEqual<T, ::std::array<char16_t, 5>>);
   static_assert(    CT::ComparableEqual<T, ::std::array<char32_t, 5>>);
   static_assert(    CT::ComparableEqual<T, char*>);
   static_assert(    CT::ComparableEqual<T, wchar_t*>);
   static_assert(    CT::ComparableEqual<T, char8_t*>);
   static_assert(    CT::ComparableEqual<T, char16_t*>);
   static_assert(    CT::ComparableEqual<T, char32_t*>);
   static_assert(    CT::ComparableEqual<T, std::string>);
   static_assert(    CT::ComparableEqual<T, std::wstring>);
   static_assert(    CT::ComparableEqual<T, std::string_view>);
   static_assert(    CT::ComparableEqual<T, std::wstring_view>);
   static_assert(    CT::ComparableEqual<T, Literal<char,4>>);
   static_assert(    CT::ComparableEqual<T, Literal<wchar_t,4>>);
   static_assert(    CT::ComparableEqual<T, Literal<char8_t,4>>);
   static_assert(    CT::ComparableEqual<T, Literal<char16_t,4>>);
   static_assert(    CT::ComparableEqual<T, Literal<char32_t,4>>);

   static_assert(    CT::Comparable<T, T>);
   static_assert(    CT::Comparable<T, char>);
   static_assert(    CT::Comparable<T, wchar_t>);
   static_assert(    CT::Comparable<T, char8_t>);
   static_assert(    CT::Comparable<T, char16_t>);
   static_assert(    CT::Comparable<T, char32_t>);
   static_assert(    CT::Comparable<T, ::std::array<char, 5>>);
   static_assert(    CT::Comparable<T, ::std::array<wchar_t, 5>>);
   static_assert(    CT::Comparable<T, ::std::array<char8_t, 5>>);
   static_assert(    CT::Comparable<T, ::std::array<char16_t, 5>>);
   static_assert(    CT::Comparable<T, ::std::array<char32_t, 5>>);
   static_assert(    CT::Comparable<T, char*>);
   static_assert(    CT::Comparable<T, wchar_t*>);
   static_assert(    CT::Comparable<T, char8_t*>);
   static_assert(    CT::Comparable<T, char16_t*>);
   static_assert(    CT::Comparable<T, char32_t*>);
   static_assert(    CT::Comparable<T, std::string>);
   static_assert(    CT::Comparable<T, std::wstring>);
   static_assert(    CT::Comparable<T, std::string_view>);
   static_assert(    CT::Comparable<T, std::wstring_view>);
   static_assert(    CT::Comparable<T, Literal<char,4>>);
   static_assert(    CT::Comparable<T, Literal<wchar_t,4>>);
   static_assert(    CT::Comparable<T, Literal<char8_t,4>>);
   static_assert(    CT::Comparable<T, Literal<char16_t,4>>);
   static_assert(    CT::Comparable<T, Literal<char32_t,4>>);

   static_assert(::std::ranges::range<T>);

   static_assert(    requires (T pack)         { pack.Get(); });
   static_assert(    requires (T pack)         { pack.template As<char>(); });
   static_assert(not requires (T pack)         { pack.GetDeep(); });
   static_assert(not requires (T pack)         { pack.GetResolved(); });
   static_assert(not requires (T pack)         { pack.GetDense(); });
   static_assert(    requires (T pack)         { {pack +   pack} -> ::std::same_as<T>;  });
   static_assert(    requires (T pack)         { {pack +=  pack} -> ::std::same_as<T&>; });
   static_assert(    requires (T pack, E item) { {pack +   item} -> ::std::same_as<T>;  });
   static_assert(    requires (T pack, E item) { {pack +=  item} -> ::std::same_as<T&>; });
   static_assert(    requires (T pack, E item) { {pack <<  item} -> ::std::same_as<T&>; });
   static_assert(    requires (T pack, E item) { {pack >>  item} -> ::std::same_as<T&>; });
   static_assert(    requires (T pack, E item) { {pack <<= item} -> ::std::same_as<T&>; });
   static_assert(    requires (T pack, E item) { {pack >>= item} -> ::std::same_as<T&>; });
   static_assert(    requires (T pack, E item) { pack.InsertAt(Index::Back, item); });
   static_assert(    requires (T pack, E item) { pack.Insert(item); });
   static_assert(not requires (T pack, E item) { pack.EmplaceAt(Index::Back, item); });
   static_assert(not requires (T pack, E item) { pack.Emplace(item); });
   static_assert(    requires (T pack)         { pack.ConcatAt(Index::Back, pack); });
   static_assert(    requires (T pack)         { pack.Concat(pack); });
   static_assert(    requires (T pack, E item) { pack.MergeAt(Index::Back, item); });
   static_assert(    requires (T pack)         { pack.MergeRangeAt(Index::Back, pack); });
   static_assert(    requires (T pack, E item) { pack.Merge(item); });
   static_assert(    requires (T pack)         { pack.MergeRange(pack); });
   static_assert(    requires (T pack, E item) { pack.Erase(item); });
   static_assert(    requires (T pack)         { pack.EraseAt(Index::Front); });
   static_assert(    requires (T pack)         { pack.Reserve(20); });
   static_assert(not requires (T pack)         { pack.EnableOr(); });
   static_assert(not requires (T pack)         { pack.IsOr(); });
   static_assert(    requires (T pack, E item) { pack.Find(item); });
   static_assert(    requires (T pack)         { pack.ForEach([](const int&) {}); });
   static_assert(    requires (T pack)         { pack.ForEachRev([](const int&) {}); });

   static_assert(T::CountHeapProviders() == 1);
   
   GIVEN("Default-constructed container") {
      const ScopedE element {555};
      T pack;
      prevent_optimization(pack);

      /// MARK: Gap test                                                      
      WHEN("Gap test") {
         Common_GapTest<T, ::std::string>();
         static_assert(sizeof(T) <= sizeof(::std::string));
      }

      WHEN("Default-constructed") {
         Text_CheckState_Default(pack);
      
         BenchmarkTextStd("Empty/DefaultConstructor", 30, 40,
            T temp,                 new (&temp)     T{},
            stdstr temp_std,        new (&temp_std) stdstr{}
         );
      }

      /// MARK: Assign/Refer                                                  
      WHEN("Assigned value by referral") {
         REQUIRE_NOTHROW(pack.Assign(*element));

         if constexpr (CT::DeepDense<E>)
            Many_CheckState_OwnedFull<TypeOf<E>>(*element);

         Text_CheckState_OwnedFull(pack);
         Text_CheckState_ContainsOne(pack, *element);

         BenchmarkTextStd("Empty/Assign/Refer", 30, 100,
            T temp,                 temp.Assign(*element),
            stdstr temp_std,        temp_std.emplace_back(*element)
         );
      }

      if constexpr (CT::DeepDense<E>) {
         WHEN("Assigned and misabsorbed by referral") {
            const auto element_backup = *element;
            REQUIRE_THROWS(pack.AssignAbsorb(*element));
            Text_CheckState_Default(pack);
            Many_Helper_TestSame(element_backup, *element);
         }
      }

      if constexpr (CT::Container<E> and CT::Text<E>) {
         WHEN("Assigned and absorbed by referral") {
            REQUIRE_NOTHROW(pack.AssignAbsorb(*element));

            Text_CheckState_OwnedFull(pack);
            Text_CheckState_OwnedFull(*element);
            Text_Helper_TestSame(pack, *element);
            REQUIRE(pack.GetUses() == element->GetUses());
            REQUIRE(pack.GetUses() == 2);
            REQUIRE(pack.GetAllocation() == element->GetAllocation());

            BenchmarkTextStd("Empty/AssignAbsorb/Refer", 30, 100,
               T temp,                         temp.AssignAbsorb(*element),
               stdstr src_std (1, *element);
               stdstr temp_std,                temp_std = src_std;
            );
         }
      }

      /// MARK: Assign/Move                                                   
      WHEN("Assigned value by move") {
         auto movable = *element;
         if constexpr (Same<E, RT>)
            movable.copied_in = false;

         REQUIRE_NOTHROW(pack.Assign(::std::move(movable)));

         if constexpr (CT::DeepDense<E>) {
            Many_CheckState_OwnedFull<TypeOf<E>>(movable);
            Many_Helper_TestSame(movable, *element);
         }

         Text_CheckState_OwnedFull(pack);
         Text_CheckState_ContainsOne(pack, *element);

         BenchmarkTextStd("Empty/Assign/Move", 30, 100,
            auto movable = *element;
            T temp,                       temp.Assign(::std::move(movable)),
            auto movable = *element;
            stdstr temp_std,              temp_std.emplace_back(::std::move(movable))
         );
      }

      if constexpr (CT::DeepDense<E>) {
         WHEN("Assigned and misabsorbed by move") {
            auto movable = *element;
            REQUIRE_THROWS(pack.AssignAbsorb(::std::move(movable)));
            Text_CheckState_Default(pack);
            Many_CheckState_OwnedFull<TypeOf<E>>(movable);
            Many_Helper_TestSame(movable, *element);
         }
      }

      if constexpr (CT::Container<E> and CT::Text<E>) {
         WHEN("Assigned and absorbed by move") {
            auto movable = *element;
            REQUIRE_NOTHROW(pack.AssignAbsorb(::std::move(movable)));

            Text_CheckState_OwnedFull(pack);
            Text_CheckState_Default(movable);
            Text_Helper_TestSame(pack, *element);
            REQUIRE(pack.GetUses() == element->GetUses());
            REQUIRE(pack.GetUses() == 2);
            REQUIRE(pack.GetAllocation() == element->GetAllocation());

            BenchmarkTextStd("Empty/AssignAbsorb/Move", 30, 100,
               auto movable = *element;
               T temp,                       temp.AssignAbsorb(::std::move(movable)),
               stdstr movable (1, 555);
               stdstr temp_std,              temp_std.emplace_back(::std::move(movable))
            );
         }
      }

      /// MARK: Assign/Copy                                                   
      WHEN("Assigned value by copy") {
         REQUIRE_NOTHROW(pack.Assign(Copy(*element)));

         if constexpr (CT::DeepDense<E>)
            Many_CheckState_OwnedFull<TypeOf<E>>(*element);
         Text_CheckState_OwnedFull(pack);
         Text_CheckState_ContainsOne(pack, *element);

         BenchmarkTextStd("Empty/Assign/Copy", 30, 100,
            T temp,              temp.Assign(Copy(*element)),
            stdstr temp_std,     temp_std.emplace_back(*element)
         );
      }

      if constexpr (CT::DeepDense<E>) {
         WHEN("Assigned and misabsorbed by copy") {
            const auto element_backup = *element;
            REQUIRE_THROWS(pack.AssignAbsorb(Copy(*element)));
            Text_CheckState_Default(pack);
            Many_Helper_TestSame(element_backup, *element);
         }
      }

      if constexpr (CT::Container<E> and CT::Text<E>) {
         WHEN("Assigned and absorbed by copy") {
            REQUIRE_NOTHROW(pack.AssignAbsorb(Copy(*element)));

            Text_CheckState_OwnedFull(pack);
            Text_CheckState_OwnedFull(*element);
            Text_CheckState_ContainsString(pack, "555");
            Text_CheckState_ContainsString(*element, "555");
            REQUIRE(pack.GetUses() == 1);
            REQUIRE(pack.GetAllocation() != element->GetAllocation());

            BenchmarkTextStd("Empty/AssignAbsorb/Copy", 30, 100,
               T temp,                          temp.AssignAbsorb(Copy(*element)),
               stdstr src_std (1, *element);
               stdstr temp_std,                 temp_std = src_std
            );
         }
      }

      /// MARK: Assign/Clone                                                  
      WHEN("Assigned value by clone") {
         REQUIRE_NOTHROW(pack.Assign(Clone(*element)));

         if constexpr (CT::DeepDense<E>)
            Many_CheckState_OwnedFull<TypeOf<E>>(*element);
         Text_CheckState_OwnedFull(pack);
         Text_CheckState_ContainsOne(pack, *element);

         BenchmarkTextStd("Empty/Assign/Clone", 30, 100,
            T temp,                 temp.Assign(Clone(*element)),
            stdstr temp_std,        temp_std.emplace_back(*element)
         );
      }

      if constexpr (CT::DeepDense<E>) {
         WHEN("Assigned and misabsorbed by clone") {
            const auto element_backup = *element;
            REQUIRE_THROWS(pack.AssignAbsorb(Clone(*element)));
            Text_CheckState_Default(pack);
            Many_Helper_TestSame(element_backup, *element);
         }
      }

      if constexpr (CT::Container<E> and CT::Text<E>) {
         WHEN("Assigned and absorbed by clone") {
            REQUIRE_NOTHROW(pack.AssignAbsorb(Clone(*element)));

            Text_CheckState_OwnedFull(pack);
            Text_CheckState_OwnedFull(*element);
            Text_CheckState_ContainsString(pack, "555");
            Text_CheckState_ContainsString(*element, "555");
            REQUIRE(pack.GetUses() == 1);
            REQUIRE(pack.GetAllocation() != element->GetAllocation());

            BenchmarkTextStd("Empty/AssignAbsorb/Clone", 30, 100,
               T temp,                          temp.AssignAbsorb(Clone(*element)),
               stdstr src_std (1, *element);
               stdstr temp_std,                 temp_std = src_std
            );
         }
      }

      /// MARK: Assign/Disown                                                 
      WHEN("Assigned value by disown") {
         REQUIRE_NOTHROW(pack.Assign(Disown(*element)));

         Text_CheckState_OwnedFull(pack);
         Text_CheckState_ContainsOne(pack, *element);

         BenchmarkTextStd("Empty/Assign/Disown", 30, 100,
            T temp,                 temp.Assign(Disown(*element)),
            stdstr temp_std,        temp_std.emplace_back(*element)
         );
      }

      if constexpr (CT::DeepDense<E>) {
         WHEN("Assigned and misabsorbed by disown") {
            const auto element_backup = *element;
            REQUIRE_THROWS(pack.AssignAbsorb(Disown(*element)));
            Text_CheckState_Default(pack);
            Many_Helper_TestSame(element_backup, *element);
         }
      }

      if constexpr (CT::Container<E> and CT::Text<E>) {
         WHEN("Assigned and absorbed by disown") {
            REQUIRE_NOTHROW(pack.AssignAbsorb(Disown(*element)));

            Text_CheckState_DisownedFull(pack);
            Text_CheckState_OwnedFull(*element);
            Text_Helper_TestSame(pack, *element, false);
            REQUIRE(pack.GetUses() == 1);
            REQUIRE(pack.GetAllocation() == element->GetAllocation());

            BenchmarkTextStd("Empty/AssignAbsorb/Disown", 30, 100,
               T temp,                       temp.AssignAbsorb(Disown(*element)),
               stdstr src_std (1, *element);
               stdstr temp_std,              temp_std = src_std
            );
         }
      }

      /// MARK: Assign/Abandon                                                
      WHEN("Assigned value by abandon") {
         auto movable = *element;
         if constexpr (Same<E, RT>)
            movable.copied_in = false;

         REQUIRE_NOTHROW(pack.Assign(Abandon(movable)));

         if constexpr (CT::DeepDense<E>) {
            Many_CheckState_OwnedFull<TypeOf<E>>(movable);
            Many_Helper_TestSame(movable, *element);
         }
         
         Text_CheckState_OwnedFull(pack);
         Text_CheckState_ContainsOne(pack, *element);

         BenchmarkTextStd("Empty/Assign/Abandon", 30, 100,
            auto movable = *element;
            T temp,                       temp.Assign(Abandon(movable)),
            auto movable = *element;
            stdstr temp_std,              temp_std.emplace_back(::std::move(movable))
         );
      }

      if constexpr (CT::DeepDense<E>) {
         WHEN("Assigned and misabsorbed by abandon") {
            auto movable = *element;
            REQUIRE_THROWS(pack.AssignAbsorb(Abandon(movable)));
            Text_CheckState_Default(pack);
            Text_Helper_TestSame(movable, *element);
         }
      }

      if constexpr (CT::Container<E> and CT::Text<E>) {
         WHEN("Assigned and absorbed by abandon") {
            auto movable = *element;
            REQUIRE_NOTHROW(pack.AssignAbsorb(Abandon(movable)));

            Text_CheckState_OwnedFull(pack);
            Text_CheckState_Abandoned(movable);
            Text_Helper_TestSame(pack, *element);
            REQUIRE(pack.GetUses() == 2);
            REQUIRE(pack.GetAllocation() == element->GetAllocation());

            BenchmarkTextStd("Empty/AssignAbsorb/Abandon", 30, 100,
               auto movable = *element;
               T temp,                       temp.AssignAbsorb(Abandon(movable)),
               stdstr movable (1, 555);
               stdstr temp_std,              temp_std = ::std::move(movable)
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

         Text_CheckState_Default(pack);
      }

      /// MARK: Clear                                                         
      WHEN("Cleared") {
         REQUIRE_NOTHROW(pack.Clear());

         Text_CheckState_Default(pack);

         BenchmarkTextStd("Empty/Clear", 30, 100,
            T temp,              temp.Clear(),
            stdstr temp_std,     temp_std.clear()
         );
      }

      /// MARK: Reset                                                         
      WHEN("Reset") {
         REQUIRE_NOTHROW(pack.Reset());

         Text_CheckState_Default(pack);

         BenchmarkTextStd("Empty/Reset", 30, 100,
            T temp,              temp.Reset(),
            stdstr temp_std,     temp_std.clear()
         );
      }

      /// MARK: Erase                                                         
      WHEN("Erase non-existent value") {
         size_t removed = 0;
         REQUIRE_NOTHROW(removed = pack.Erase(*element));

         Text_CheckState_Default(pack);

         REQUIRE(removed == 0);

         BenchmarkTextStd("Empty/Erase", 30, 100,
            T temp,              temp.Erase(*element),
            stdstr temp_std,     temp_std.erase(std::remove_if(temp_std.begin(), temp_std.end(), [&element] (auto& value) {
                                    return value == *element;
                                 }), temp_std.end());
         );
      }

      WHEN("Erase non-existent index") {
         size_t removed = 0;
         REQUIRE_NOTHROW(removed = pack.EraseAt(5));

         Text_CheckState_Default(pack);

         REQUIRE(removed == 0);

         BenchmarkTextStd("Empty/EraseAt", 30, 100,
            T temp,              temp.EraseAt(5),
            stdstr temp_std,     temp_std.erase(temp_std.begin() + 5)
         );
      }

      WHEN("Referred empty") {
         T refer1 = pack;

         Text_Helper_TestSame(refer1, pack);
         Text_CheckState_Default(refer1);
         Text_CheckState_Default(pack);

         T refer2 = Refer(pack);

         Text_Helper_TestSame(refer2, pack);
         Text_CheckState_Default(refer2);
         Text_CheckState_Default(pack);
      }

      WHEN("Cloned empty") {
         T clone = Clone(pack);

         Text_Helper_TestSame(clone, pack);
         Text_CheckState_Default(clone);
         Text_CheckState_Default(pack);
      }

      WHEN("Disowned empty") {
         T disowned = Disown(pack);

         Text_Helper_TestSame(disowned, pack);
         Text_CheckState_Default(disowned);
         Text_CheckState_Default(pack);
      }

      WHEN("Copied empty") {
         T copy = Copy(pack);

         Text_Helper_TestSame(copy, pack);
         Text_CheckState_Default(copy);
         Text_CheckState_Default(pack);
      }

      WHEN("Moved empty") {
         T movable1 = pack;
         const T moved1 = ::std::move(movable1);

         Text_CheckState_Default(movable1);
         Text_Helper_TestSame(moved1, pack);
         Text_CheckState_Default(moved1);
         Text_CheckState_Default(pack);

         T movable2 = pack;
         const T moved2 = Move(movable2);

         Text_CheckState_Default(movable2);
         Text_Helper_TestSame(moved2, pack);
         Text_CheckState_Default(moved2);
         Text_CheckState_Default(pack);
      }

      WHEN("Abandoned empty") {
         T movable = pack;
         const T moved = Abandon(movable);

         Text_CheckState_Default(movable);
         Text_Helper_TestSame(moved, pack);
         Text_CheckState_Default(moved);
         Text_CheckState_Default(pack);
      }

      /// MARK: Compare                                                       
      WHEN("Compared") {
         static_assert(not static_cast<bool>(T{}));
         static_assert(       T{} == T{}       );
         static_assert(  not (T{} != T{})      );
         static_assert(       T{} == nullptr   );
         static_assert(   nullptr == T{}       );
         static_assert(       T{} == ""        );
         static_assert(        "" == T{}       );
         static_assert(T{nullptr} == T{nullptr});
         static_assert(     T{""} == T{""}     );
         static_assert(   nullptr == T{nullptr});
         static_assert(     T{""} == ""        );
         static_assert(        "" == T{""}     );

         T another_pack1;
         T another_pack2;

         REQUIRE      (another_pack1 == another_pack2);
         REQUIRE_FALSE(another_pack1 != another_pack2);

         //TODO compare against literals and stuff
         [[maybe_unused]] volatile bool dont_optimize = false;
         BenchmarkTextStd("Empty/operator==", 30, 100,
            (void) 0,            dont_optimize |= (another_pack1 == another_pack2),
            stdstr std1;
            stdstr std2,         dont_optimize |= (std1 == std2)
         );

         BenchmarkTextStd("Empty/operator!=", 30, 100,
            (void) 0,            dont_optimize |= (another_pack1 != another_pack2),
            stdstr std1;
            stdstr std2,         dont_optimize |= (std1 != std2)
         );
      }

      if constexpr (CT::Character<E>) {
         /// MARK: Contains                                                   
         WHEN("Contains character when empty") {
            REQUIRE_FALSE(pack.Contains(*element));

            //TODO compare against literals and stuff
            [[maybe_unused]] volatile bool dont_optimize = false;
            BenchmarkTextStd("Empty/Contains", 30, 100,
               (void) 0,            dont_optimize |= pack.Contains(*element),
               stdstr std1,         dont_optimize |= std1.contains(*element)
            );
         }
      }

      if constexpr (CT::Container<E>) {
         /// MARK: ContainsRange                                              
         WHEN("Contains substring when empty") {
            REQUIRE_FALSE(pack.ContainsRange(*element));

            //TODO compare against literals and stuff
            [[maybe_unused]] volatile bool dont_optimize = false;
            BenchmarkTextStd("Empty/ContainsRange", 30, 100,
               (void) 0,            dont_optimize |= pack.ContainsRange(*element),
               stdstr std1,         dont_optimize |= std1.containsRange(*element)
            );
         }
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

         for (auto& it : ::std::as_const(pack)) {
            (void) it;
            ++counter;
            static_assert(Same<char, decltype(it)>);
         }

         for (auto& it : strategy) {
            (void) it;
            ++counter;
            static_assert(Same<char, decltype(it)>);
         }

         for (auto& it : strategyConst) {
            (void) it;
            ++counter;
            static_assert(Same<char, decltype(it)>);
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

         static_assert(::std::random_access_iterator<Iterator>);
         static_assert(::std::random_access_iterator<IteratorConst>);
         static_assert(::std::contiguous_iterator<Iterator>);
         static_assert(::std::contiguous_iterator<IteratorConst>);

         size_t counter = 0;
         for (auto& it : strategy) {
            (void) it;
            ++counter;
            static_assert(Same<char, decltype(it)>);
         }

         for (auto& it : strategyConst) {
            (void) it;
            ++counter;
            static_assert(Same<char, decltype(it)>);
         }

         REQUIRE(counter == 0);
      }

      WHEN("Range-iterated (noderef)") {
         IterateNoDeref strategy(pack);
         using Iterator = decltype(strategy.begin());

         static_assert(::std::same_as<Iterator, decltype(strategy.end())>);
         static_assert(::std::input_or_output_iterator<Iterator>);

         static_assert(::std::random_access_iterator<typename Iterator::value_type>);
         static_assert(::std::contiguous_iterator<typename Iterator::value_type>);

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
            static_assert(Same<char, decltype(it.one()), decltype(it.two())>);
         }

         REQUIRE(counter == 0);
      }
      
      /// MARK: Handles                                                       
      WHEN("GetHandle is called on mutable container") {
         auto h = pack.GetHandle();
         static_assert(::std::same_as<decltype(h), THandle<char&>>);
         Handle_CheckState_Default<char>(h);
      }

      WHEN("GetHandle is called on constant container") {
         T const pack_constant;
         auto h = pack_constant.GetHandle();
         static_assert(::std::same_as<decltype(h), THandle<char const&>>);
         Handle_CheckState_Default<char const>(h);
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

      /// MARK: Insert array                                                  
      WHEN("Insert an array to the back") {
         size_t inserted = 0;
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
               "\"49\"\"50\"\"51\"\"52\"\"53\""
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
               "RT(copied)RT(copied)RT(copied)RT(copied)RT(copied)"
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
               "12345"
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
               "4950515253"
               "4950515253"
               "4950515253"
               "4950515253"
               "5455565758"
               "5455565758"
               "5455565758"
               "4950515253"
            );
         }

         BenchmarkTextStd("Empty/Insert/Array/Back", 30, 100,
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
               "\"49\"\"50\"\"51\"\"52\"\"53\""
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
               "RT(copied)RT(copied)RT(copied)RT(copied)RT(copied)"
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
               "12345"
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
               "4950515253"
            );
         }

         BenchmarkTextStd("Empty/Insert/Array/Front", 30, 100,
            T temp,              temp.InsertAt(Index::Front, darray1),
            stdstr temp_std,     std::copy(darray1, darray1 + 5, std::front_inserter(temp_std))
         );
      }

      /// MARK: Insert at                                                     
      WHEN("Insert an array to a non-existent index") {
         size_t inserted = 0;
         REQUIRE_THROWS(inserted = pack.InsertAt(5, immovable));
         REQUIRE(inserted == 0);

         // Residual type from the failed insertion remains.            
         // Shouldn't be a problem, generally speaking, because an      
         // empty container can mutate later, as long as it wasn't      
         // allocated.                                                  
         Text_CheckState_Default(pack);
      }

      /// MARK: <<                                                            //TODO test only with characters. it's insertion operator, not concat operator!
      /*WHEN("Insert at the back by using << operator)") {
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
               "\"49\"\"50\"\"51\"\"52\"\"54\"\"54\"\"54\"\"53\""
            );
         }
         else if constexpr (Same<E, RT>) {
            Text_CheckState_ContainsString(pack,
               "RT(copied)RT(copied)RT(copied)RT(copied)"
               "RT(copied)RT(copied)RT(copied)RT(copied)"
            );
         }
         else if constexpr (Same<E, char>) {
            Text_CheckState_ContainsString(pack, "12346665");
         }
         else {
            Text_CheckState_ContainsString(pack, "4950515254545453");
         }

         BenchmarkTextStd("Empty/Insert/Element/Back", 30, 100,
            T temp,              temp << immovable[0],
            stdstr temp_std,     temp_std.emplace_back(immovable[0])
         );
      }*/

      /// MARK: >>                                                            //TODO test only with characters. it's insertion operator, not concat operator!
      /*WHEN("Insert at the front by using >> operator)") {
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
               "\"53\"\"54\"\"54\"\"54\"\"52\"\"51\"\"50\"\"49\""
            );
         }
         else if constexpr (Same<E, RT>) {
            Text_CheckState_ContainsString(pack,
               "RT(copied)RT(copied)RT(copied)RT(copied)"
               "RT(copied)RT(copied)RT(copied)RT(copied)"
            );
         }
         else if constexpr (Same<E, char>) {
            Text_CheckState_ContainsString(pack, "56664321");
         }
         else {
            Text_CheckState_ContainsString(pack, "5354545452515049");
         }

         BenchmarkTextStd("Empty/Insert/Element/Front", 30, 100,
            T temp,              temp >> immovable[0],
            stdstr temp_std,     temp_std.emplace_front(immovable[0])
         );
      }*/

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

            Text_CheckState_ContainsString(pack,"4950515254555653");

            BenchmarkTextStd("Empty/Concat/Element/Back", 30, 100,
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

            Text_CheckState_ContainsString(pack,"5356555452515049");

            BenchmarkTextStd("Empty/Concat/Element/Front", 30, 100,
               T temp,              temp.ConcatAt(Index::Front, darray1),
               stdstr temp_std,     std::copy(darray1, darray1 + 5, std::front_inserter(temp_std))
            );
         }

         /// MARK: Concat at                                                  
         WHEN("Concatenate to a non-existent index") {
            size_t inserted = 0;
            REQUIRE_THROWS(inserted = pack.ConcatAt(1000, immovable[0]));
            REQUIRE(inserted == 0);
            
            Text_CheckState_Default(pack);
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
               "RT(copied)RT(copied)RT(copied)RT(copied)RT(copied)"
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
               "12345"
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
            Text_CheckState_ContainsString(pack,
               "4950515253"
               "4950515253"
               "4950515253"
               "4950515253"
               "5455565758"
               "5455565758"
               "5455565758"
               "4950515253"
            );
         }

         BenchmarkTextStd("Empty/+=/Array/Back", 30, 100,
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
               "RT(copied)RT(copied)RT(copied)RT(copied)"
               "RT(copied)RT(copied)RT(copied)RT(copied)"
            );
         }
         else if constexpr (Same<E, char>) {
            Text_CheckState_ContainsString(pack, "12346665");
         }
         else {
            Text_CheckState_ContainsString(pack, "4950515254545453");
         }

         BenchmarkTextStd("Empty/+=/Element/Back", 30, 100,
            T temp,              temp += immovable[0],
            stdstr temp_std,     temp_std.emplace_back(immovable[0])
         );
      }
   }

   REQUIRE(memoryState.Assert());
   REQUIRE_FALSE(Allocator::CollectGarbage());
}