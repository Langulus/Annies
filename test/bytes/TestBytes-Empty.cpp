///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "TestBytesCommon.hpp"


TEST_CASE_TEMPLATE("Test empty Bytes", TestType
   // Elements are not allocated by the memory manager                  
   , Types<Bytes, ScopedElement<Bytes>>
   , Types<Bytes, ScopedElement<Text>>
   , Types<Bytes, ScopedElement<int32_t>>
   , Types<Bytes, ScopedElement<Many>>
   , Types<Bytes, ScopedElement<RT>>
   , Types<Bytes, ScopedElement<char>>

   , Types<Bytes, ScopedElement<Bytes*>>
   , Types<Bytes, ScopedElement<Text*>>
   , Types<Bytes, ScopedElement<int32_t*>>
   , Types<Bytes, ScopedElement<Many*>>
   , Types<Bytes, ScopedElement<RT*>>
   , Types<Bytes, ScopedElement<char*>>

   , Types<Bytes, ScopedElement<Bytes**>>
   , Types<Bytes, ScopedElement<Text**>>
   , Types<Bytes, ScopedElement<int32_t**>>
   , Types<Bytes, ScopedElement<Many**>>
   , Types<Bytes, ScopedElement<RT**>>
   , Types<Bytes, ScopedElement<char**>>

   #if LANGULUS_FEATURE(MANAGED_MEMORY)
   // Elements are allocated by the memory manager                      
   , Types<Bytes, ScopedElement<Bytes,  true>>
   , Types<Bytes, ScopedElement<Text,   true>>
   , Types<Bytes, ScopedElement<int32_t,true>>
   , Types<Bytes, ScopedElement<Many,   true>>
   , Types<Bytes, ScopedElement<RT,     true>>
   , Types<Bytes, ScopedElement<char,   true>>

   , Types<Bytes, ScopedElement<Bytes*, true>>
   , Types<Bytes, ScopedElement<Text*,  true>>
   , Types<Bytes, ScopedElement<int32_t*,true>>
   , Types<Bytes, ScopedElement<Many*,  true>>
   , Types<Bytes, ScopedElement<RT*,    true>>
   , Types<Bytes, ScopedElement<char*,  true>>

   , Types<Bytes, ScopedElement<Bytes**,true>>
   , Types<Bytes, ScopedElement<Text**, true>>
   , Types<Bytes, ScopedElement<int32_t**,true>>
   , Types<Bytes, ScopedElement<Many**, true>>
   , Types<Bytes, ScopedElement<RT**,   true>>
   , Types<Bytes, ScopedElement<char**, true>>

   // Packed pointers                                                   
   , Types<Bytes, ScopedElementPacked<pptr8>>
   , Types<Bytes, ScopedElementPacked<pptr16>>
   , Types<Bytes, ScopedElementPacked<pptr32>>
   #endif
) {
   static MemoryState memoryState;
   using T        = typename TestType::First;
   using ScopedE  = typename TestType::Second;
   using E        = TypeOf<ScopedE>;

   [[maybe_unused]] constexpr bool Managed = ScopedE::Managed;

   #if LANGULUS(BENCHMARK)
      using stdbyt = ::std::vector<Byte>;
   #endif

   static_assert(    Exact<TypeOf<T>, Byte>);

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
   static_assert(    CT::ComparableEqual<T, Byte>);
   static_assert(    CT::ComparableEqual<T, ::std::array<Byte, 5>>);
   static_assert(    CT::ComparableEqual<T, std::vector<Byte>>);
   static_assert(    CT::ComparableEqual<T, Literal<Byte,4>>);

   static_assert(    CT::Comparable<T, T>);
   static_assert(    CT::Comparable<T, Byte>);
   static_assert(    CT::Comparable<T, ::std::array<Byte, 5>>);
   static_assert(    CT::Comparable<T, std::vector<Byte>>);
   static_assert(    CT::Comparable<T, Literal<Byte,4>>);

   static_assert(::std::ranges::range<T>);

   static_assert(    requires (T pack)         { pack.Get(); });
   static_assert(    requires (T pack)         { pack.template As<Byte>(); });
   static_assert(not requires (T pack)         { pack.GetDeep(); });
   static_assert(not requires (T pack)         { pack.GetResolved(); });
   static_assert(not requires (T pack)         { pack.GetDense(); });
   static_assert(    requires (T pack)         { {pack +   pack} -> ::std::same_as<T>;  });
   static_assert(    requires (T pack)         { {pack +=  pack} -> ::std::same_as<T&>; });
   static_assert(    requires (T pack, E item) { {pack +   item} -> ::std::same_as<T>;  });
   static_assert(    requires (T pack, E item) { {pack +=  item} -> ::std::same_as<T&>; });
   static_assert(    requires (T pack, E item) { {pack <<  item} -> ::std::same_as<T&>; });
   static_assert(    requires (T pack, E item) { {pack >>  item} -> ::std::same_as<T&>; });
   static_assert(not requires (T pack, E item) { {pack <<= item} -> ::std::same_as<T&>; });
   static_assert(not requires (T pack, E item) { {pack >>= item} -> ::std::same_as<T&>; });
   static_assert(    requires (T pack, E item) { pack.InsertAt(Index::Back, item); });
   static_assert(    requires (T pack, E item) { pack.Insert(item); });
   static_assert(not requires (T pack, E item) { pack.EmplaceAt(Index::Back, item); });
   static_assert(not requires (T pack, E item) { pack.Emplace(item); });
   static_assert(    requires (T pack)         { pack.ConcatAt(Index::Back, pack); });
   static_assert(    requires (T pack)         { pack.Concat(pack); });
   static_assert(not requires (T pack, E item) { pack.MergeAt(Index::Back, item); });
   static_assert(not requires (T pack)         { pack.MergeRangeAt(Index::Back, pack); });
   static_assert(not requires (T pack, E item) { pack.Merge(item); });
   static_assert(not requires (T pack)         { pack.MergeRange(pack); });
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
      const ScopedE element {Clone(int32_t{555})};
      T pack;
      prevent_optimization(pack);

      /// MARK: Gap test                                                      
      WHEN("Gap test") {
         Common_GapTest<T, ::std::vector<Byte>>();
         //static_assert(sizeof(T) <= sizeof(::std::vector<Byte>)); // not true on GCC
      }

      WHEN("Default-constructed") {
         Bytes_CheckState_Default(pack);
      
         BenchmarkBytesStd("Empty/DefaultConstructor", 30, 40,
            T temp,                 new (&temp)     T{},
            stdbyt temp_std,        new (&temp_std) stdbyt{}
         );
      }

      /// MARK: Assign/Refer                                                  
      WHEN("Assigned value by referral") {
         REQUIRE_NOTHROW(pack.Assign(*element));

         if constexpr (CT::DeepDense<E>)
            Many_CheckState_OwnedFull<TypeOf<E>>(*element);

         Bytes_CheckState_OwnedFull(pack);
         Bytes_CheckState_ContainsOne(pack, *element);

         BenchmarkBytesStd("Empty/Assign/Refer", 30, 100,
            T temp,                 temp.Assign(*element),
            stdbyt temp_std,        temp_std.emplace_back(*element)
         );
      }

      if constexpr (CT::DeepDense<E>) {
         WHEN("Assigned and misabsorbed by referral") {
            const auto element_backup = *element;
            REQUIRE_THROWS(pack.AssignAbsorb(*element));
            Bytes_CheckState_Default(pack);
            Many_Helper_TestSame(element_backup, *element);
         }
      }

      if constexpr (Same<E, Bytes>) {
         WHEN("Assigned and absorbed by referral") {
            REQUIRE_NOTHROW(pack.AssignAbsorb(*element));

            Bytes_CheckState_OwnedFull(pack);
            Bytes_CheckState_OwnedFull(*element);
            Bytes_Helper_TestSame(pack, *element);

            BenchmarkBytesStd("Empty/AssignAbsorb/Refer", 30, 100,
               T temp,                         temp.AssignAbsorb(*element),
               stdbyt src_std (1, *element);
               stdbyt temp_std,                temp_std = src_std;
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

         Bytes_CheckState_OwnedFull(pack);
         Bytes_CheckState_ContainsOne(pack, *element);

         BenchmarkBytesStd("Empty/Assign/Move", 30, 100,
            auto movable = *element;
            T temp,                       temp.Assign(::std::move(movable)),
            auto movable = *element;
            stdbyt temp_std,              temp_std.emplace_back(::std::move(movable))
         );
      }

      if constexpr (CT::DeepDense<E>) {
         WHEN("Assigned and misabsorbed by move") {
            auto movable = *element;
            REQUIRE_THROWS(pack.AssignAbsorb(::std::move(movable)));
            Bytes_CheckState_Default(pack);
            Many_CheckState_OwnedFull<TypeOf<E>>(movable);
            Many_Helper_TestSame(movable, *element);
         }
      }

      if constexpr (Same<E, Bytes>) {
         WHEN("Assigned and absorbed by move") {
            auto movable = *element;
            REQUIRE_NOTHROW(pack.AssignAbsorb(::std::move(movable)));

            Bytes_CheckState_OwnedFull(pack);
            Bytes_CheckState_Default(movable);
            Bytes_Helper_TestSame(pack, *element);

            BenchmarkBytesStd("Empty/AssignAbsorb/Move", 30, 100,
               auto movable = *element;
               T temp,                       temp.AssignAbsorb(::std::move(movable)),
               stdbyt movable (1, 555);
               stdbyt temp_std,              temp_std.emplace_back(::std::move(movable))
            );
         }
      }

      /// MARK: Assign/Copy                                                   
      WHEN("Assigned value by copy") {
         REQUIRE_NOTHROW(pack.Assign(Copy(*element)));

         if constexpr (CT::DeepDense<E>)
            Many_CheckState_OwnedFull<TypeOf<E>>(*element);
         Bytes_CheckState_OwnedFull(pack);
         Bytes_CheckState_ContainsOne(pack, *element);

         BenchmarkBytesStd("Empty/Assign/Copy", 30, 100,
            T temp,              temp.Assign(Copy(*element)),
            stdbyt temp_std,     temp_std.emplace_back(*element)
         );
      }

      if constexpr (CT::DeepDense<E>) {
         WHEN("Assigned and misabsorbed by copy") {
            const auto element_backup = *element;
            REQUIRE_THROWS(pack.AssignAbsorb(Copy(*element)));
            Bytes_CheckState_Default(pack);
            Many_Helper_TestSame(element_backup, *element);
         }
      }

      if constexpr (Same<E, Bytes>) {
         WHEN("Assigned and absorbed by copy") {
            const uint8_t pattern[] = {
               0x2b, 0x02, 0x00, 0x00
            };
            Bytes_CheckState_ContainsBytes(*element, pattern);

            REQUIRE_NOTHROW(pack.AssignAbsorb(Copy(*element)));

            Bytes_CheckState_OwnedFull(pack);
            Bytes_CheckState_OwnedFull(*element);
            Bytes_CheckState_ContainsBytes(*element, pattern);
            Bytes_CheckState_ContainsBytes(pack, pattern);

            REQUIRE(pack.GetUses() == 1);
            REQUIRE(pack.GetAllocation() != element->GetAllocation());

            BenchmarkBytesStd("Empty/AssignAbsorb/Copy", 30, 100,
               T temp,                          temp.AssignAbsorb(Copy(*element)),
               stdbyt src_std (1, *element);
               stdbyt temp_std,                 temp_std = src_std
            );
         }
      }

      /// MARK: Assign/Clone                                                  
      WHEN("Assigned value by clone") {
         REQUIRE_NOTHROW(pack.Assign(Clone(*element)));

         if constexpr (CT::DeepDense<E>)
            Many_CheckState_OwnedFull<TypeOf<E>>(*element);
         Bytes_CheckState_OwnedFull(pack);
         Bytes_CheckState_ContainsOne(pack, *element);

         BenchmarkBytesStd("Empty/Assign/Clone", 30, 100,
            T temp,                 temp.Assign(Clone(*element)),
            stdbyt temp_std,        temp_std.emplace_back(*element)
         );
      }

      if constexpr (CT::DeepDense<E>) {
         WHEN("Assigned and misabsorbed by clone") {
            const auto element_backup = *element;
            REQUIRE_THROWS(pack.AssignAbsorb(Clone(*element)));
            Bytes_CheckState_Default(pack);
            Many_Helper_TestSame(element_backup, *element);
         }
      }

      if constexpr (Same<E, Bytes>) {
         WHEN("Assigned and absorbed by clone") {
            const uint8_t pattern[] = {
               0x2b, 0x02, 0x00, 0x00
            };
            Bytes_CheckState_ContainsBytes(*element, pattern);

            REQUIRE_NOTHROW(pack.AssignAbsorb(Clone(*element)));

            Bytes_CheckState_OwnedFull(pack);
            Bytes_CheckState_OwnedFull(*element);
            Bytes_CheckState_ContainsBytes(*element, pattern);
            Bytes_CheckState_ContainsBytes(pack, pattern);
            
            REQUIRE(pack.GetUses() == 1);
            REQUIRE(pack.GetAllocation() != element->GetAllocation());

            BenchmarkBytesStd("Empty/AssignAbsorb/Clone", 30, 100,
               T temp,                          temp.AssignAbsorb(Clone(*element)),
               stdbyt src_std (1, *element);
               stdbyt temp_std,                 temp_std = src_std
            );
         }
      }

      /// MARK: Assign/Disown                                                 
      WHEN("Assigned value by disown") {
         REQUIRE_NOTHROW(pack.Assign(Disown(*element)));

         Bytes_CheckState_OwnedFull(pack);
         Bytes_CheckState_ContainsOne(pack, *element);

         BenchmarkBytesStd("Empty/Assign/Disown", 30, 100,
            T temp,                 temp.Assign(Disown(*element)),
            stdbyt temp_std,        temp_std.emplace_back(*element)
         );
      }

      if constexpr (CT::DeepDense<E>) {
         WHEN("Assigned and misabsorbed by disown") {
            const auto element_backup = *element;
            REQUIRE_THROWS(pack.AssignAbsorb(Disown(*element)));
            Bytes_CheckState_Default(pack);
            Many_Helper_TestSame(element_backup, *element);
         }
      }

      if constexpr (Same<E, Bytes>) {
         WHEN("Assigned and absorbed by disown") {
            REQUIRE_NOTHROW(pack.AssignAbsorb(Disown(*element)));

            Bytes_CheckState_DisownedFull(pack);
            Bytes_CheckState_OwnedFull(*element);
            Bytes_Helper_TestSame(pack, *element, false);

            BenchmarkBytesStd("Empty/AssignAbsorb/Disown", 30, 100,
               T temp,                       temp.AssignAbsorb(Disown(*element)),
               stdbyt src_std (1, *element);
               stdbyt temp_std,              temp_std = src_std
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
         
         Bytes_CheckState_OwnedFull(pack);
         Bytes_CheckState_ContainsOne(pack, *element);

         BenchmarkBytesStd("Empty/Assign/Abandon", 30, 100,
            auto movable = *element;
            T temp,                       temp.Assign(Abandon(movable)),
            auto movable = *element;
            stdbyt temp_std,              temp_std.emplace_back(::std::move(movable))
         );
      }

      if constexpr (CT::DeepDense<E>) {
         WHEN("Assigned and misabsorbed by abandon") {
            auto movable = *element;
            REQUIRE_THROWS(pack.AssignAbsorb(Abandon(movable)));
            Bytes_CheckState_Default(pack);
            Bytes_Helper_TestSame(movable, *element);
         }
      }

      if constexpr (Same<E, Bytes>) {
         WHEN("Assigned and absorbed by abandon") {
            auto movable = *element;
            REQUIRE_NOTHROW(pack.AssignAbsorb(Abandon(movable)));

            Bytes_CheckState_OwnedFull(pack);
            Bytes_CheckState_Abandoned(movable);
            Bytes_Helper_TestSame(pack, *element);

            BenchmarkBytesStd("Empty/AssignAbsorb/Abandon", 30, 100,
               auto movable = *element;
               T temp,                       temp.AssignAbsorb(Abandon(movable)),
               stdbyt movable (1, 555);
               stdbyt temp_std,              temp_std = ::std::move(movable)
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

         Bytes_CheckState_Default(pack);
      }

      /// MARK: Clear                                                         
      WHEN("Cleared") {
         REQUIRE_NOTHROW(pack.Clear());

         Bytes_CheckState_Default(pack);

         BenchmarkBytesStd("Empty/Clear", 30, 100,
            T temp,              temp.Clear(),
            stdbyt temp_std,     temp_std.clear()
         );
      }

      /// MARK: Reset                                                         
      WHEN("Reset") {
         REQUIRE_NOTHROW(pack.Reset());

         Bytes_CheckState_Default(pack);

         BenchmarkBytesStd("Empty/Reset", 30, 100,
            T temp,              temp.Reset(),
            stdbyt temp_std,     temp_std.clear()
         );
      }

      /// MARK: Erase                                                         
      WHEN("Erase non-existent value") {
         size_t removed = 0;
         REQUIRE_NOTHROW(removed = pack.Erase(*element));

         Bytes_CheckState_Default(pack);

         REQUIRE(removed == 0);

         BenchmarkBytesStd("Empty/Erase", 30, 100,
            T temp,              temp.Erase(*element),
            stdbyt temp_std,     temp_std.erase(std::remove_if(temp_std.begin(), temp_std.end(), [&element] (auto& value) {
                                    return value == *element;
                                 }), temp_std.end());
         );
      }

      WHEN("Erase non-existent index") {
         size_t removed = 0;
         REQUIRE_NOTHROW(removed = pack.EraseAt(5));

         Bytes_CheckState_Default(pack);

         REQUIRE(removed == 0);

         BenchmarkBytesStd("Empty/EraseAt", 30, 100,
            T temp,              temp.EraseAt(5),
            stdbyt temp_std,     temp_std.erase(temp_std.begin() + 5)
         );
      }

      WHEN("Referred empty") {
         T refer1 = pack;

         Bytes_Helper_TestSame(refer1, pack);
         Bytes_CheckState_Default(refer1);
         Bytes_CheckState_Default(pack);

         T refer2 = Refer(pack);

         Bytes_Helper_TestSame(refer2, pack);
         Bytes_CheckState_Default(refer2);
         Bytes_CheckState_Default(pack);
      }

      WHEN("Cloned empty") {
         T clone = Clone(pack);

         Bytes_Helper_TestSame(clone, pack);
         Bytes_CheckState_Default(clone);
         Bytes_CheckState_Default(pack);
      }

      WHEN("Disowned empty") {
         T disowned = Disown(pack);

         Bytes_Helper_TestSame(disowned, pack);
         Bytes_CheckState_Default(disowned);
         Bytes_CheckState_Default(pack);
      }

      WHEN("Copied empty") {
         T copy = Copy(pack);

         Bytes_Helper_TestSame(copy, pack);
         Bytes_CheckState_Default(copy);
         Bytes_CheckState_Default(pack);
      }

      WHEN("Moved empty") {
         T movable1 = pack;
         const T moved1 = ::std::move(movable1);

         Bytes_CheckState_Default(movable1);
         Bytes_Helper_TestSame(moved1, pack);
         Bytes_CheckState_Default(moved1);
         Bytes_CheckState_Default(pack);

         T movable2 = pack;
         const T moved2 = Move(movable2);

         Bytes_CheckState_Default(movable2);
         Bytes_Helper_TestSame(moved2, pack);
         Bytes_CheckState_Default(moved2);
         Bytes_CheckState_Default(pack);
      }

      WHEN("Abandoned empty") {
         T movable = pack;
         const T moved = Abandon(movable);

         Bytes_CheckState_Default(movable);
         Bytes_Helper_TestSame(moved, pack);
         Bytes_CheckState_Default(moved);
         Bytes_CheckState_Default(pack);
      }

      /// MARK: Compare                                                       
      WHEN("Compared") {
         static_assert(not static_cast<bool>(T{}));
         static_assert(       T{} == T{}       );
         static_assert(  not (T{} != T{})      );

         T another_pack1;
         T another_pack2;

         REQUIRE      (another_pack1 == another_pack2);
         REQUIRE_FALSE(another_pack1 != another_pack2);

         [[maybe_unused]] volatile bool dont_optimize = false;
         BenchmarkBytesStd("Empty/operator==", 30, 100,
            (void) 0,            dont_optimize |= (another_pack1 == another_pack2),
            stdbyt std1;
            stdbyt std2,         dont_optimize |= (std1 == std2)
         );

         BenchmarkBytesStd("Empty/operator!=", 30, 100,
            (void) 0,            dont_optimize |= (another_pack1 != another_pack2),
            stdbyt std1;
            stdbyt std2,         dont_optimize |= (std1 != std2)
         );
      }

      if constexpr (Same<E, Byte>) {
         /// MARK: Contains                                                   
         WHEN("Contains byte when empty") {
            REQUIRE_FALSE(pack.Contains(*element));

            [[maybe_unused]] volatile bool dont_optimize = false;
            BenchmarkBytesStd("Empty/Contains", 30, 100,
               (void) 0,            dont_optimize |= pack.Contains(*element),
               stdbyt std1,         dont_optimize |= std1.contains(*element)
            );
         }
      }

      if constexpr (CT::Container<E>) {
         /// MARK: ContainsRange                                              
         WHEN("Contains byte range when empty") {
            REQUIRE_FALSE(pack.ContainsRange(*element));

            [[maybe_unused]] volatile bool dont_optimize = false;
            BenchmarkBytesStd("Empty/ContainsRange", 30, 100,
               (void) 0,            dont_optimize |= pack.ContainsRange(*element),
               stdbyt std1,         dont_optimize |= std1.containsRange(*element)
            );
         }
      }

      if constexpr (Exact<E, Bytes>) {
         WHEN("Given bytes that will be destroyed before the pack") {
            Bytes owned_text {int32_t{666}};
            REQUIRE_NOTHROW(pack = owned_text);
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
            static_assert(Same<Byte, decltype(it)>);
         }

         for (auto& it : ::std::as_const(pack)) {
            (void) it;
            ++counter;
            static_assert(Same<Byte, decltype(it)>);
         }

         for (auto& it : strategy) {
            (void) it;
            ++counter;
            static_assert(Same<Byte, decltype(it)>);
         }

         for (auto& it : strategyConst) {
            (void) it;
            ++counter;
            static_assert(Same<Byte, decltype(it)>);
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
            static_assert(Same<Byte, decltype(it)>);
         }

         for (auto& it : strategyConst) {
            (void) it;
            ++counter;
            static_assert(Same<Byte, decltype(it)>);
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
            static_assert(Same<Byte, decltype(it.one()), decltype(it.two())>);
         }

         REQUIRE(counter == 0);
      }
      
      /// MARK: Handles                                                       
      WHEN("GetHandle is called on mutable container") {
         auto h = pack.GetHandle();
         #if not LANGULUS(FORCE_TYPE_ERASURE)
            static_assert(::std::same_as<decltype(h), THandle<Byte&>>);
         #else
            static_assert(::std::same_as<decltype(h), HandleMut>);
         #endif
         Handle_CheckState_Default<Byte>(h);
      }

      WHEN("GetHandle is called on constant container") {
         T const pack_constant;
         auto h = pack_constant.GetHandle();
         #if not LANGULUS(FORCE_TYPE_ERASURE)
            static_assert(::std::same_as<decltype(h), THandle<Byte const&>>);
         #else
            static_assert(::std::same_as<decltype(h), Handle>);
         #endif
         Handle_CheckState_Default<Byte const>(h);
      }
   }

   GIVEN("Default-constructed container and a couple of arrays") {
      const ScopedE darray1[5] {
         Clone(int32_t{49}), 
         Clone(int32_t{50}), 
         Clone(int32_t{51}), 
         Clone(int32_t{52}), 
         Clone(int32_t{53})
      };
      const ScopedE darray2[5] {
         Clone(int32_t{54}),
         Clone(int32_t{55}),
         Clone(int32_t{56}),
         Clone(int32_t{57}),
         Clone(int32_t{58})
      };

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
         volatile size_t inserted = 0;
         REQUIRE_NOTHROW(inserted += pack.InsertAt(Index::Back,           immovable));
         REQUIRE_NOTHROW(inserted += pack.InsertAt(Index::Back, Refer    {immovable}));
         REQUIRE_NOTHROW(inserted += pack.InsertAt(Index::Back, Copy     {immovable}));
         REQUIRE_NOTHROW(inserted += pack.InsertAt(Index::Back, Disown   {immovable}));
         REQUIRE_NOTHROW(inserted += pack.InsertAt(Index::Back, std::move(movable1)));
         REQUIRE_NOTHROW(inserted += pack.InsertAt(Index::Back, Move     {movable2}));
         REQUIRE_NOTHROW(inserted += pack.InsertAt(Index::Back, Abandon  {movable3}));
         REQUIRE_NOTHROW(inserted += pack.InsertAt(Index::Back, Clone    {immovable}));

         Bytes_CheckState_OwnedFull(pack);

         /*if constexpr (Same<E, Bytes>) {
            for (int i = 0; i < 5; ++i) {
               Bytes_CheckState_OwnedFull(immovable[i]);
               Bytes_CheckState_OwnedFull(movable1[i]);
               Bytes_CheckState_OwnedFull(movable2[i]);
               Bytes_CheckState_OwnedFull(movable3[i]);
            }
         }
         else*/ if constexpr (CT::Container<E>) {
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
            const uint8_t pattern[] = {
               0b00001010, 10, 2, '4', '9',
               0b00001010, 10, 2, '5', '0',
               0b00001010, 10, 2, '5', '1',
               0b00001010, 10, 2, '5', '2',
               0b00001010, 10, 2, '5', '3',

               0b00001010, 10, 2, '4', '9',
               0b00001010, 10, 2, '5', '0',
               0b00001010, 10, 2, '5', '1',
               0b00001010, 10, 2, '5', '2',
               0b00001010, 10, 2, '5', '3',

               0b00001010, 10, 2, '4', '9',
               0b00001010, 10, 2, '5', '0',
               0b00001010, 10, 2, '5', '1',
               0b00001010, 10, 2, '5', '2',
               0b00001010, 10, 2, '5', '3',

               0b00001010, 10, 2, '4', '9',
               0b00001010, 10, 2, '5', '0',
               0b00001010, 10, 2, '5', '1',
               0b00001010, 10, 2, '5', '2',
               0b00001010, 10, 2, '5', '3',

               0b00001010, 10, 2, '5', '4',
               0b00001010, 10, 2, '5', '5',
               0b00001010, 10, 2, '5', '6',
               0b00001010, 10, 2, '5', '7',
               0b00001010, 10, 2, '5', '8',

               0b00001010, 10, 2, '5', '4',
               0b00001010, 10, 2, '5', '5',
               0b00001010, 10, 2, '5', '6',
               0b00001010, 10, 2, '5', '7',
               0b00001010, 10, 2, '5', '8',

               0b00001010, 10, 2, '5', '4',
               0b00001010, 10, 2, '5', '5',
               0b00001010, 10, 2, '5', '6',
               0b00001010, 10, 2, '5', '7',
               0b00001010, 10, 2, '5', '8',

               0b00001010, 10, 2, '4', '9',
               0b00001010, 10, 2, '5', '0',
               0b00001010, 10, 2, '5', '1',
               0b00001010, 10, 2, '5', '2',
               0b00001010, 10, 2, '5', '3'
            };
            REQUIRE(inserted == sizeof(pattern));

            Bytes_CheckState_ContainsBytes(pack, pattern);
         }
         else if constexpr (Same<E, RT>) {
            const uint8_t pattern[] = {
               0x31, 0x00, 0x00, 0x00, 0b00000010,
               0x32, 0x00, 0x00, 0x00, 0b00000010,
               0x33, 0x00, 0x00, 0x00, 0b00000010,
               0x34, 0x00, 0x00, 0x00, 0b00000010,
               0x35, 0x00, 0x00, 0x00, 0b00000010,

               0x31, 0x00, 0x00, 0x00, 0b00000010,
               0x32, 0x00, 0x00, 0x00, 0b00000010,
               0x33, 0x00, 0x00, 0x00, 0b00000010,
               0x34, 0x00, 0x00, 0x00, 0b00000010,
               0x35, 0x00, 0x00, 0x00, 0b00000010,

               0x31, 0x00, 0x00, 0x00, 0b00000010,
               0x32, 0x00, 0x00, 0x00, 0b00000010,
               0x33, 0x00, 0x00, 0x00, 0b00000010,
               0x34, 0x00, 0x00, 0x00, 0b00000010,
               0x35, 0x00, 0x00, 0x00, 0b00000010,

               0x31, 0x00, 0x00, 0x00, 0b00000010,
               0x32, 0x00, 0x00, 0x00, 0b00000010,
               0x33, 0x00, 0x00, 0x00, 0b00000010,
               0x34, 0x00, 0x00, 0x00, 0b00000010,
               0x35, 0x00, 0x00, 0x00, 0b00000010,

               0x36, 0x00, 0x00, 0x00, 0b00000010,
               0x37, 0x00, 0x00, 0x00, 0b00000010,
               0x38, 0x00, 0x00, 0x00, 0b00000010,
               0x39, 0x00, 0x00, 0x00, 0b00000010,
               0x3a, 0x00, 0x00, 0x00, 0b00000010,

               0x36, 0x00, 0x00, 0x00, 0b00000010,
               0x37, 0x00, 0x00, 0x00, 0b00000010,
               0x38, 0x00, 0x00, 0x00, 0b00000010,
               0x39, 0x00, 0x00, 0x00, 0b00000010,
               0x3a, 0x00, 0x00, 0x00, 0b00000010,

               0x36, 0x00, 0x00, 0x00, 0b00000010,
               0x37, 0x00, 0x00, 0x00, 0b00000010,
               0x38, 0x00, 0x00, 0x00, 0b00000010,
               0x39, 0x00, 0x00, 0x00, 0b00000010,
               0x3a, 0x00, 0x00, 0x00, 0b00000010,

               0x31, 0x00, 0x00, 0x00, 0b00000010,
               0x32, 0x00, 0x00, 0x00, 0b00000010,
               0x33, 0x00, 0x00, 0x00, 0b00000010,
               0x34, 0x00, 0x00, 0x00, 0b00000010,
               0x35, 0x00, 0x00, 0x00, 0b00000010
            };
            REQUIRE(inserted == sizeof(pattern));

            Bytes_CheckState_ContainsBytes(pack, pattern);
         }
         else if constexpr (Same<E, char>) {
            const uint8_t pattern[] = {
               0x31, 0x32, 0x33, 0x34, 0x35,
               0x31, 0x32, 0x33, 0x34, 0x35,
               0x31, 0x32, 0x33, 0x34, 0x35,
               0x31, 0x32, 0x33, 0x34, 0x35,
               0x36, 0x37, 0x38, 0x39, 0x3a,
               0x36, 0x37, 0x38, 0x39, 0x3a,
               0x36, 0x37, 0x38, 0x39, 0x3a,
               0x31, 0x32, 0x33, 0x34, 0x35
            };
            REQUIRE(inserted == sizeof(pattern));

            Bytes_CheckState_ContainsBytes(pack, pattern);
         }
         else if constexpr (Same<E, int32_t>) {
            const uint8_t pattern[] = {
               0x31, 0x00, 0x00, 0x00, //49
               0x32, 0x00, 0x00, 0x00, //50
               0x33, 0x00, 0x00, 0x00, //51
               0x34, 0x00, 0x00, 0x00, //52
               0x35, 0x00, 0x00, 0x00, //53

               0x31, 0x00, 0x00, 0x00, //49
               0x32, 0x00, 0x00, 0x00, //50
               0x33, 0x00, 0x00, 0x00, //51
               0x34, 0x00, 0x00, 0x00, //52
               0x35, 0x00, 0x00, 0x00, //53

               0x31, 0x00, 0x00, 0x00, //49
               0x32, 0x00, 0x00, 0x00, //50
               0x33, 0x00, 0x00, 0x00, //51
               0x34, 0x00, 0x00, 0x00, //52
               0x35, 0x00, 0x00, 0x00, //53

               0x31, 0x00, 0x00, 0x00, //49
               0x32, 0x00, 0x00, 0x00, //50
               0x33, 0x00, 0x00, 0x00, //51
               0x34, 0x00, 0x00, 0x00, //52
               0x35, 0x00, 0x00, 0x00, //53

               0x36, 0x00, 0x00, 0x00, //54
               0x37, 0x00, 0x00, 0x00, //55
               0x38, 0x00, 0x00, 0x00, //56
               0x39, 0x00, 0x00, 0x00, //57
               0x3a, 0x00, 0x00, 0x00, //58

               0x36, 0x00, 0x00, 0x00, //54
               0x37, 0x00, 0x00, 0x00, //55
               0x38, 0x00, 0x00, 0x00, //56
               0x39, 0x00, 0x00, 0x00, //57
               0x3a, 0x00, 0x00, 0x00, //58

               0x36, 0x00, 0x00, 0x00, //54
               0x37, 0x00, 0x00, 0x00, //55
               0x38, 0x00, 0x00, 0x00, //56
               0x39, 0x00, 0x00, 0x00, //57
               0x3a, 0x00, 0x00, 0x00, //58

               0x31, 0x00, 0x00, 0x00, //49
               0x32, 0x00, 0x00, 0x00, //50
               0x33, 0x00, 0x00, 0x00, //51
               0x34, 0x00, 0x00, 0x00, //52
               0x35, 0x00, 0x00, 0x00  //53
            };
      
            REQUIRE(inserted == sizeof(pattern));
            Bytes_CheckState_ContainsBytes(pack, pattern);
         }
         else if constexpr (Same<E, Many>) {
            const uint8_t pattern[] = {
               0b00000010, 4, 0x31, 0x00, 0x00, 0x00, //49
               0b00000010, 4, 0x32, 0x00, 0x00, 0x00, //50
               0b00000010, 4, 0x33, 0x00, 0x00, 0x00, //51
               0b00000010, 4, 0x34, 0x00, 0x00, 0x00, //52
               0b00000010, 4, 0x35, 0x00, 0x00, 0x00, //53

               0b00000010, 4, 0x31, 0x00, 0x00, 0x00, //49
               0b00000010, 4, 0x32, 0x00, 0x00, 0x00, //50
               0b00000010, 4, 0x33, 0x00, 0x00, 0x00, //51
               0b00000010, 4, 0x34, 0x00, 0x00, 0x00, //52
               0b00000010, 4, 0x35, 0x00, 0x00, 0x00, //53

               0b00000010, 4, 0x31, 0x00, 0x00, 0x00, //49
               0b00000010, 4, 0x32, 0x00, 0x00, 0x00, //50
               0b00000010, 4, 0x33, 0x00, 0x00, 0x00, //51
               0b00000010, 4, 0x34, 0x00, 0x00, 0x00, //52
               0b00000010, 4, 0x35, 0x00, 0x00, 0x00, //53

               0b00000010, 4, 0x31, 0x00, 0x00, 0x00, //49
               0b00000010, 4, 0x32, 0x00, 0x00, 0x00, //50
               0b00000010, 4, 0x33, 0x00, 0x00, 0x00, //51
               0b00000010, 4, 0x34, 0x00, 0x00, 0x00, //52
               0b00000010, 4, 0x35, 0x00, 0x00, 0x00, //53

               0b00000010, 4, 0x36, 0x00, 0x00, 0x00, //54
               0b00000010, 4, 0x37, 0x00, 0x00, 0x00, //55
               0b00000010, 4, 0x38, 0x00, 0x00, 0x00, //56
               0b00000010, 4, 0x39, 0x00, 0x00, 0x00, //57
               0b00000010, 4, 0x3a, 0x00, 0x00, 0x00, //58

               0b00000010, 4, 0x36, 0x00, 0x00, 0x00, //54
               0b00000010, 4, 0x37, 0x00, 0x00, 0x00, //55
               0b00000010, 4, 0x38, 0x00, 0x00, 0x00, //56
               0b00000010, 4, 0x39, 0x00, 0x00, 0x00, //57
               0b00000010, 4, 0x3a, 0x00, 0x00, 0x00, //58

               0b00000010, 4, 0x36, 0x00, 0x00, 0x00, //54
               0b00000010, 4, 0x37, 0x00, 0x00, 0x00, //55
               0b00000010, 4, 0x38, 0x00, 0x00, 0x00, //56
               0b00000010, 4, 0x39, 0x00, 0x00, 0x00, //57
               0b00000010, 4, 0x3a, 0x00, 0x00, 0x00, //58

               0b00000010, 4, 0x31, 0x00, 0x00, 0x00, //49
               0b00000010, 4, 0x32, 0x00, 0x00, 0x00, //50
               0b00000010, 4, 0x33, 0x00, 0x00, 0x00, //51
               0b00000010, 4, 0x34, 0x00, 0x00, 0x00, //52
               0b00000010, 4, 0x35, 0x00, 0x00, 0x00  //53
            };
      
            REQUIRE(inserted == sizeof(pattern));
            Bytes_CheckState_ContainsBytes(pack, pattern);
         }
         else {
            const uint8_t pattern[] = {
               0b00001010, 11, 4, 0x31, 0x00, 0x00, 0x00, //49
               0b00001010, 11, 4, 0x32, 0x00, 0x00, 0x00, //50
               0b00001010, 11, 4, 0x33, 0x00, 0x00, 0x00, //51
               0b00001010, 11, 4, 0x34, 0x00, 0x00, 0x00, //52
               0b00001010, 11, 4, 0x35, 0x00, 0x00, 0x00, //53

               0b00001010, 11, 4, 0x31, 0x00, 0x00, 0x00, //49
               0b00001010, 11, 4, 0x32, 0x00, 0x00, 0x00, //50
               0b00001010, 11, 4, 0x33, 0x00, 0x00, 0x00, //51
               0b00001010, 11, 4, 0x34, 0x00, 0x00, 0x00, //52
               0b00001010, 11, 4, 0x35, 0x00, 0x00, 0x00, //53

               0b00001010, 11, 4, 0x31, 0x00, 0x00, 0x00, //49
               0b00001010, 11, 4, 0x32, 0x00, 0x00, 0x00, //50
               0b00001010, 11, 4, 0x33, 0x00, 0x00, 0x00, //51
               0b00001010, 11, 4, 0x34, 0x00, 0x00, 0x00, //52
               0b00001010, 11, 4, 0x35, 0x00, 0x00, 0x00, //53

               0b00001010, 11, 4, 0x31, 0x00, 0x00, 0x00, //49
               0b00001010, 11, 4, 0x32, 0x00, 0x00, 0x00, //50
               0b00001010, 11, 4, 0x33, 0x00, 0x00, 0x00, //51
               0b00001010, 11, 4, 0x34, 0x00, 0x00, 0x00, //52
               0b00001010, 11, 4, 0x35, 0x00, 0x00, 0x00, //53

               0b00001010, 11, 4, 0x36, 0x00, 0x00, 0x00, //54
               0b00001010, 11, 4, 0x37, 0x00, 0x00, 0x00, //55
               0b00001010, 11, 4, 0x38, 0x00, 0x00, 0x00, //56
               0b00001010, 11, 4, 0x39, 0x00, 0x00, 0x00, //57
               0b00001010, 11, 4, 0x3a, 0x00, 0x00, 0x00, //58

               0b00001010, 11, 4, 0x36, 0x00, 0x00, 0x00, //54
               0b00001010, 11, 4, 0x37, 0x00, 0x00, 0x00, //55
               0b00001010, 11, 4, 0x38, 0x00, 0x00, 0x00, //56
               0b00001010, 11, 4, 0x39, 0x00, 0x00, 0x00, //57
               0b00001010, 11, 4, 0x3a, 0x00, 0x00, 0x00, //58

               0b00001010, 11, 4, 0x36, 0x00, 0x00, 0x00, //54
               0b00001010, 11, 4, 0x37, 0x00, 0x00, 0x00, //55
               0b00001010, 11, 4, 0x38, 0x00, 0x00, 0x00, //56
               0b00001010, 11, 4, 0x39, 0x00, 0x00, 0x00, //57
               0b00001010, 11, 4, 0x3a, 0x00, 0x00, 0x00, //58

               0b00001010, 11, 4, 0x31, 0x00, 0x00, 0x00, //49
               0b00001010, 11, 4, 0x32, 0x00, 0x00, 0x00, //50
               0b00001010, 11, 4, 0x33, 0x00, 0x00, 0x00, //51
               0b00001010, 11, 4, 0x34, 0x00, 0x00, 0x00, //52
               0b00001010, 11, 4, 0x35, 0x00, 0x00, 0x00  //53
            };
      
            REQUIRE(inserted == sizeof(pattern));
            Bytes_CheckState_ContainsBytes(pack, pattern);
         }

         BenchmarkBytesStd("Empty/Insert/Array/Back", 30, 100,
            T temp,              temp.InsertAt(Index::Back, immovable),
            stdbyt temp_std,     std::copy(immovable, immovable + 5, std::back_inserter(temp_std))
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

         Bytes_CheckState_OwnedFull(pack);

         /*if constexpr (Same<E, Bytes>) {
            for (int i = 0; i < 5; ++i) {
               Bytes_CheckState_DisownedFull(immovable[i]);
               Bytes_CheckState_DisownedFull(movable1[i]);
               Bytes_CheckState_DisownedFull(movable2[i]);
               Bytes_CheckState_DisownedFull(movable3[i]);
            }
         }
         else*/ if constexpr (CT::Container<E>) {
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
            const uint8_t pattern[] = {
               0b00001010, 10, 2, '4', '9',
               0b00001010, 10, 2, '5', '0',
               0b00001010, 10, 2, '5', '1',
               0b00001010, 10, 2, '5', '2',
               0b00001010, 10, 2, '5', '3',

               0b00001010, 10, 2, '5', '4',
               0b00001010, 10, 2, '5', '5',
               0b00001010, 10, 2, '5', '6',
               0b00001010, 10, 2, '5', '7',
               0b00001010, 10, 2, '5', '8',

               0b00001010, 10, 2, '5', '4',
               0b00001010, 10, 2, '5', '5',
               0b00001010, 10, 2, '5', '6',
               0b00001010, 10, 2, '5', '7',
               0b00001010, 10, 2, '5', '8',

               0b00001010, 10, 2, '5', '4',
               0b00001010, 10, 2, '5', '5',
               0b00001010, 10, 2, '5', '6',
               0b00001010, 10, 2, '5', '7',
               0b00001010, 10, 2, '5', '8',

               0b00001010, 10, 2, '4', '9',
               0b00001010, 10, 2, '5', '0',
               0b00001010, 10, 2, '5', '1',
               0b00001010, 10, 2, '5', '2',
               0b00001010, 10, 2, '5', '3',

               0b00001010, 10, 2, '4', '9',
               0b00001010, 10, 2, '5', '0',
               0b00001010, 10, 2, '5', '1',
               0b00001010, 10, 2, '5', '2',
               0b00001010, 10, 2, '5', '3',

               0b00001010, 10, 2, '4', '9',
               0b00001010, 10, 2, '5', '0',
               0b00001010, 10, 2, '5', '1',
               0b00001010, 10, 2, '5', '2',
               0b00001010, 10, 2, '5', '3',

               0b00001010, 10, 2, '4', '9',
               0b00001010, 10, 2, '5', '0',
               0b00001010, 10, 2, '5', '1',
               0b00001010, 10, 2, '5', '2',
               0b00001010, 10, 2, '5', '3'
            };
            REQUIRE(inserted == sizeof(pattern));

            Bytes_CheckState_ContainsBytes(pack, pattern);
         }
         else if constexpr (Same<E, RT>) {
            const uint8_t pattern[] = {
               0x31, 0x00, 0x00, 0x00, 0b00000010,
               0x32, 0x00, 0x00, 0x00, 0b00000010,
               0x33, 0x00, 0x00, 0x00, 0b00000010,
               0x34, 0x00, 0x00, 0x00, 0b00000010,
               0x35, 0x00, 0x00, 0x00, 0b00000010,

               0x36, 0x00, 0x00, 0x00, 0b00000010,
               0x37, 0x00, 0x00, 0x00, 0b00000010,
               0x38, 0x00, 0x00, 0x00, 0b00000010,
               0x39, 0x00, 0x00, 0x00, 0b00000010,
               0x3a, 0x00, 0x00, 0x00, 0b00000010,

               0x36, 0x00, 0x00, 0x00, 0b00000010,
               0x37, 0x00, 0x00, 0x00, 0b00000010,
               0x38, 0x00, 0x00, 0x00, 0b00000010,
               0x39, 0x00, 0x00, 0x00, 0b00000010,
               0x3a, 0x00, 0x00, 0x00, 0b00000010,

               0x36, 0x00, 0x00, 0x00, 0b00000010,
               0x37, 0x00, 0x00, 0x00, 0b00000010,
               0x38, 0x00, 0x00, 0x00, 0b00000010,
               0x39, 0x00, 0x00, 0x00, 0b00000010,
               0x3a, 0x00, 0x00, 0x00, 0b00000010,

               0x31, 0x00, 0x00, 0x00, 0b00000010,
               0x32, 0x00, 0x00, 0x00, 0b00000010,
               0x33, 0x00, 0x00, 0x00, 0b00000010,
               0x34, 0x00, 0x00, 0x00, 0b00000010,
               0x35, 0x00, 0x00, 0x00, 0b00000010,

               0x31, 0x00, 0x00, 0x00, 0b00000010,
               0x32, 0x00, 0x00, 0x00, 0b00000010,
               0x33, 0x00, 0x00, 0x00, 0b00000010,
               0x34, 0x00, 0x00, 0x00, 0b00000010,
               0x35, 0x00, 0x00, 0x00, 0b00000010,

               0x31, 0x00, 0x00, 0x00, 0b00000010,
               0x32, 0x00, 0x00, 0x00, 0b00000010,
               0x33, 0x00, 0x00, 0x00, 0b00000010,
               0x34, 0x00, 0x00, 0x00, 0b00000010,
               0x35, 0x00, 0x00, 0x00, 0b00000010,

               0x31, 0x00, 0x00, 0x00, 0b00000010,
               0x32, 0x00, 0x00, 0x00, 0b00000010,
               0x33, 0x00, 0x00, 0x00, 0b00000010,
               0x34, 0x00, 0x00, 0x00, 0b00000010,
               0x35, 0x00, 0x00, 0x00, 0b00000010
            };
            REQUIRE(inserted == sizeof(pattern));

            Bytes_CheckState_ContainsBytes(pack, pattern);
         }
         else if constexpr (Same<E, char>) {
            const uint8_t pattern[] = {
               0x31, 0x32, 0x33, 0x34, 0x35,
               0x36, 0x37, 0x38, 0x39, 0x3a,
               0x36, 0x37, 0x38, 0x39, 0x3a,
               0x36, 0x37, 0x38, 0x39, 0x3a,
               0x31, 0x32, 0x33, 0x34, 0x35,
               0x31, 0x32, 0x33, 0x34, 0x35,
               0x31, 0x32, 0x33, 0x34, 0x35,
               0x31, 0x32, 0x33, 0x34, 0x35
            };
            REQUIRE(inserted == sizeof(pattern));

            Bytes_CheckState_ContainsBytes(pack, pattern);
         }
         else if constexpr (Same<E, int32_t>) {
            const uint8_t pattern[] = {
               0x31, 0x00, 0x00, 0x00, //49
               0x32, 0x00, 0x00, 0x00, //50
               0x33, 0x00, 0x00, 0x00, //51
               0x34, 0x00, 0x00, 0x00, //52
               0x35, 0x00, 0x00, 0x00, //53

               0x36, 0x00, 0x00, 0x00, //54
               0x37, 0x00, 0x00, 0x00, //55
               0x38, 0x00, 0x00, 0x00, //56
               0x39, 0x00, 0x00, 0x00, //57
               0x3a, 0x00, 0x00, 0x00, //58

               0x36, 0x00, 0x00, 0x00, //54
               0x37, 0x00, 0x00, 0x00, //55
               0x38, 0x00, 0x00, 0x00, //56
               0x39, 0x00, 0x00, 0x00, //57
               0x3a, 0x00, 0x00, 0x00, //58

               0x36, 0x00, 0x00, 0x00, //54
               0x37, 0x00, 0x00, 0x00, //55
               0x38, 0x00, 0x00, 0x00, //56
               0x39, 0x00, 0x00, 0x00, //57
               0x3a, 0x00, 0x00, 0x00, //58

               0x31, 0x00, 0x00, 0x00, //49
               0x32, 0x00, 0x00, 0x00, //50
               0x33, 0x00, 0x00, 0x00, //51
               0x34, 0x00, 0x00, 0x00, //52
               0x35, 0x00, 0x00, 0x00, //53

               0x31, 0x00, 0x00, 0x00, //49
               0x32, 0x00, 0x00, 0x00, //50
               0x33, 0x00, 0x00, 0x00, //51
               0x34, 0x00, 0x00, 0x00, //52
               0x35, 0x00, 0x00, 0x00, //53

               0x31, 0x00, 0x00, 0x00, //49
               0x32, 0x00, 0x00, 0x00, //50
               0x33, 0x00, 0x00, 0x00, //51
               0x34, 0x00, 0x00, 0x00, //52
               0x35, 0x00, 0x00, 0x00, //53

               0x31, 0x00, 0x00, 0x00, //49
               0x32, 0x00, 0x00, 0x00, //50
               0x33, 0x00, 0x00, 0x00, //51
               0x34, 0x00, 0x00, 0x00, //52
               0x35, 0x00, 0x00, 0x00  //53
            };
      
            REQUIRE(inserted == sizeof(pattern));
            Bytes_CheckState_ContainsBytes(pack, pattern);
         }
         else if constexpr (Same<E, Many>) {
            const uint8_t pattern[] = {
               0b00000010, 4, 0x31, 0x00, 0x00, 0x00, //49
               0b00000010, 4, 0x32, 0x00, 0x00, 0x00, //50
               0b00000010, 4, 0x33, 0x00, 0x00, 0x00, //51
               0b00000010, 4, 0x34, 0x00, 0x00, 0x00, //52
               0b00000010, 4, 0x35, 0x00, 0x00, 0x00, //53

               0b00000010, 4, 0x36, 0x00, 0x00, 0x00, //54
               0b00000010, 4, 0x37, 0x00, 0x00, 0x00, //55
               0b00000010, 4, 0x38, 0x00, 0x00, 0x00, //56
               0b00000010, 4, 0x39, 0x00, 0x00, 0x00, //57
               0b00000010, 4, 0x3a, 0x00, 0x00, 0x00, //58

               0b00000010, 4, 0x36, 0x00, 0x00, 0x00, //54
               0b00000010, 4, 0x37, 0x00, 0x00, 0x00, //55
               0b00000010, 4, 0x38, 0x00, 0x00, 0x00, //56
               0b00000010, 4, 0x39, 0x00, 0x00, 0x00, //57
               0b00000010, 4, 0x3a, 0x00, 0x00, 0x00, //58

               0b00000010, 4, 0x36, 0x00, 0x00, 0x00, //54
               0b00000010, 4, 0x37, 0x00, 0x00, 0x00, //55
               0b00000010, 4, 0x38, 0x00, 0x00, 0x00, //56
               0b00000010, 4, 0x39, 0x00, 0x00, 0x00, //57
               0b00000010, 4, 0x3a, 0x00, 0x00, 0x00, //58

               0b00000010, 4, 0x31, 0x00, 0x00, 0x00, //49
               0b00000010, 4, 0x32, 0x00, 0x00, 0x00, //50
               0b00000010, 4, 0x33, 0x00, 0x00, 0x00, //51
               0b00000010, 4, 0x34, 0x00, 0x00, 0x00, //52
               0b00000010, 4, 0x35, 0x00, 0x00, 0x00, //53

               0b00000010, 4, 0x31, 0x00, 0x00, 0x00, //49
               0b00000010, 4, 0x32, 0x00, 0x00, 0x00, //50
               0b00000010, 4, 0x33, 0x00, 0x00, 0x00, //51
               0b00000010, 4, 0x34, 0x00, 0x00, 0x00, //52
               0b00000010, 4, 0x35, 0x00, 0x00, 0x00, //53

               0b00000010, 4, 0x31, 0x00, 0x00, 0x00, //49
               0b00000010, 4, 0x32, 0x00, 0x00, 0x00, //50
               0b00000010, 4, 0x33, 0x00, 0x00, 0x00, //51
               0b00000010, 4, 0x34, 0x00, 0x00, 0x00, //52
               0b00000010, 4, 0x35, 0x00, 0x00, 0x00, //53

               0b00000010, 4, 0x31, 0x00, 0x00, 0x00, //49
               0b00000010, 4, 0x32, 0x00, 0x00, 0x00, //50
               0b00000010, 4, 0x33, 0x00, 0x00, 0x00, //51
               0b00000010, 4, 0x34, 0x00, 0x00, 0x00, //52
               0b00000010, 4, 0x35, 0x00, 0x00, 0x00  //53
            };
      
            REQUIRE(inserted == sizeof(pattern));
            Bytes_CheckState_ContainsBytes(pack, pattern);
         }
         else {
            const uint8_t pattern[] = {
               0b00001010, 11, 4, 0x31, 0x00, 0x00, 0x00, //49
               0b00001010, 11, 4, 0x32, 0x00, 0x00, 0x00, //50
               0b00001010, 11, 4, 0x33, 0x00, 0x00, 0x00, //51
               0b00001010, 11, 4, 0x34, 0x00, 0x00, 0x00, //52
               0b00001010, 11, 4, 0x35, 0x00, 0x00, 0x00, //53

               0b00001010, 11, 4, 0x36, 0x00, 0x00, 0x00, //54
               0b00001010, 11, 4, 0x37, 0x00, 0x00, 0x00, //55
               0b00001010, 11, 4, 0x38, 0x00, 0x00, 0x00, //56
               0b00001010, 11, 4, 0x39, 0x00, 0x00, 0x00, //57
               0b00001010, 11, 4, 0x3a, 0x00, 0x00, 0x00, //58

               0b00001010, 11, 4, 0x36, 0x00, 0x00, 0x00, //54
               0b00001010, 11, 4, 0x37, 0x00, 0x00, 0x00, //55
               0b00001010, 11, 4, 0x38, 0x00, 0x00, 0x00, //56
               0b00001010, 11, 4, 0x39, 0x00, 0x00, 0x00, //57
               0b00001010, 11, 4, 0x3a, 0x00, 0x00, 0x00, //58

               0b00001010, 11, 4, 0x36, 0x00, 0x00, 0x00, //54
               0b00001010, 11, 4, 0x37, 0x00, 0x00, 0x00, //55
               0b00001010, 11, 4, 0x38, 0x00, 0x00, 0x00, //56
               0b00001010, 11, 4, 0x39, 0x00, 0x00, 0x00, //57
               0b00001010, 11, 4, 0x3a, 0x00, 0x00, 0x00, //58

               0b00001010, 11, 4, 0x31, 0x00, 0x00, 0x00, //49
               0b00001010, 11, 4, 0x32, 0x00, 0x00, 0x00, //50
               0b00001010, 11, 4, 0x33, 0x00, 0x00, 0x00, //51
               0b00001010, 11, 4, 0x34, 0x00, 0x00, 0x00, //52
               0b00001010, 11, 4, 0x35, 0x00, 0x00, 0x00, //53

               0b00001010, 11, 4, 0x31, 0x00, 0x00, 0x00, //49
               0b00001010, 11, 4, 0x32, 0x00, 0x00, 0x00, //50
               0b00001010, 11, 4, 0x33, 0x00, 0x00, 0x00, //51
               0b00001010, 11, 4, 0x34, 0x00, 0x00, 0x00, //52
               0b00001010, 11, 4, 0x35, 0x00, 0x00, 0x00, //53

               0b00001010, 11, 4, 0x31, 0x00, 0x00, 0x00, //49
               0b00001010, 11, 4, 0x32, 0x00, 0x00, 0x00, //50
               0b00001010, 11, 4, 0x33, 0x00, 0x00, 0x00, //51
               0b00001010, 11, 4, 0x34, 0x00, 0x00, 0x00, //52
               0b00001010, 11, 4, 0x35, 0x00, 0x00, 0x00, //53

               0b00001010, 11, 4, 0x31, 0x00, 0x00, 0x00, //49
               0b00001010, 11, 4, 0x32, 0x00, 0x00, 0x00, //50
               0b00001010, 11, 4, 0x33, 0x00, 0x00, 0x00, //51
               0b00001010, 11, 4, 0x34, 0x00, 0x00, 0x00, //52
               0b00001010, 11, 4, 0x35, 0x00, 0x00, 0x00  //53
            };
      
            REQUIRE(inserted == sizeof(pattern));
            Bytes_CheckState_ContainsBytes(pack, pattern);
         }

         BenchmarkBytesStd("Empty/Insert/Array/Front", 30, 100,
            T temp,              temp.InsertAt(Index::Front, darray1),
            stdbyt temp_std,     std::copy(darray1, darray1 + 5, std::front_inserter(temp_std))
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
         Bytes_CheckState_Default(pack);
      }

      /// MARK: <<                                                            
      WHEN("Insert at the back by using << operator") {
         REQUIRE_NOTHROW(pack <<           immovable[0]
                              << Refer    {immovable[1]}
                              << Copy     {immovable[2]}
                              << Disown   {immovable[3]}
                              << std::move( movable1[0])
                              << Move     { movable2[0]}
                              << Abandon  { movable3[0]}
                              << Clone    {immovable[4]});

         Bytes_CheckState_OwnedFull(pack);

         /*if constexpr (Same<E, Bytes>) {
            for (int i = 0; i < 5; ++i)
               Bytes_CheckState_DisownedFull(immovable[i]);
            Bytes_CheckState_DisownedFull(movable1[0]);
            Bytes_CheckState_DisownedFull(movable2[0]);
            Bytes_CheckState_DisownedFull(movable3[0]);
         }
         else*/ if constexpr (CT::Container<E>) {
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
            const uint8_t pattern[] = {
               0b00001010, 10, 2, '4', '9',
               0b00001010, 10, 2, '5', '0',
               0b00001010, 10, 2, '5', '1',
               0b00001010, 10, 2, '5', '2',
               0b00001010, 10, 2, '5', '4',
               0b00001010, 10, 2, '5', '4',
               0b00001010, 10, 2, '5', '4',
               0b00001010, 10, 2, '5', '3'
            };
            Bytes_CheckState_ContainsBytes(pack, pattern);
         }
         else if constexpr (Same<E, RT>) {
            const uint8_t pattern[] = {
               0x31, 0x00, 0x00, 0x00, 0b00000010,
               0x32, 0x00, 0x00, 0x00, 0b00000010,
               0x33, 0x00, 0x00, 0x00, 0b00000010,
               0x34, 0x00, 0x00, 0x00, 0b00000010,
               0x36, 0x00, 0x00, 0x00, 0b00000010,
               0x36, 0x00, 0x00, 0x00, 0b00000010,
               0x36, 0x00, 0x00, 0x00, 0b00000010,
               0x35, 0x00, 0x00, 0x00, 0b00000010
            };
            Bytes_CheckState_ContainsBytes(pack, pattern);
         }
         else if constexpr (Same<E, char>) {
            const uint8_t pattern[] = {
               0x31, 0x32, 0x33, 0x34, 0x36, 0x36, 0x36, 0x35
            };
            Bytes_CheckState_ContainsBytes(pack, pattern);
         }
         else if constexpr (Same<E, int32_t>) {
            const uint8_t pattern[] = {
               0x31, 0x00, 0x00, 0x00, //49
               0x32, 0x00, 0x00, 0x00, //50
               0x33, 0x00, 0x00, 0x00, //51
               0x34, 0x00, 0x00, 0x00, //52
               0x36, 0x00, 0x00, 0x00, //54
               0x36, 0x00, 0x00, 0x00, //54
               0x36, 0x00, 0x00, 0x00, //54
               0x35, 0x00, 0x00, 0x00, //53
            };
            Bytes_CheckState_ContainsBytes(pack, pattern);
         }
         else if constexpr (Same<E, Many>) {
            const uint8_t pattern[] = {
               0b00000010, 4, 0x31, 0x00, 0x00, 0x00, //49
               0b00000010, 4, 0x32, 0x00, 0x00, 0x00, //50
               0b00000010, 4, 0x33, 0x00, 0x00, 0x00, //51
               0b00000010, 4, 0x34, 0x00, 0x00, 0x00, //52
               0b00000010, 4, 0x36, 0x00, 0x00, 0x00, //54
               0b00000010, 4, 0x36, 0x00, 0x00, 0x00, //54
               0b00000010, 4, 0x36, 0x00, 0x00, 0x00, //54
               0b00000010, 4, 0x35, 0x00, 0x00, 0x00, //53
            };
            Bytes_CheckState_ContainsBytes(pack, pattern);
         }
         else {
            const uint8_t pattern[] = {
               0b00001010, 11, 4, 0x31, 0x00, 0x00, 0x00, //49
               0b00001010, 11, 4, 0x32, 0x00, 0x00, 0x00, //50
               0b00001010, 11, 4, 0x33, 0x00, 0x00, 0x00, //51
               0b00001010, 11, 4, 0x34, 0x00, 0x00, 0x00, //52
               0b00001010, 11, 4, 0x36, 0x00, 0x00, 0x00, //54
               0b00001010, 11, 4, 0x36, 0x00, 0x00, 0x00, //54
               0b00001010, 11, 4, 0x36, 0x00, 0x00, 0x00, //54
               0b00001010, 11, 4, 0x35, 0x00, 0x00, 0x00, //53
            };
            Bytes_CheckState_ContainsBytes(pack, pattern);
         }

         BenchmarkBytesStd("Empty/Insert/Element/Back", 30, 100,
            T temp,              temp << immovable[0],
            stdbyt temp_std,     temp_std.emplace_back(immovable[0])
         );
      }

      /// MARK: >>                                                            
      WHEN("Insert at the front by using >> operator") {
         REQUIRE_NOTHROW(pack >>           immovable[0]
                              >> Refer    {immovable[1]}
                              >> Copy     {immovable[2]}
                              >> Disown   {immovable[3]}
                              >> std::move( movable1[0])
                              >> Move     { movable2[0]}
                              >> Abandon  { movable3[0]}
                              >> Clone    {immovable[4]});

         Bytes_CheckState_OwnedFull(pack);

         /*if constexpr (Same<E, Bytes>) {
            for (int i = 0; i < 5; ++i)
               Bytes_CheckState_DisownedFull(immovable[i]);
            Bytes_CheckState_DisownedFull(movable1[0]);
            Bytes_CheckState_DisownedFull(movable2[0]);
            Bytes_CheckState_DisownedFull(movable3[0]);
         }
         else*/ if constexpr (CT::Container<E>) {
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
            const uint8_t pattern[] = {
               0b00001010, 10, 2, '5', '3',
               0b00001010, 10, 2, '5', '4',
               0b00001010, 10, 2, '5', '4',
               0b00001010, 10, 2, '5', '4',
               0b00001010, 10, 2, '5', '2',
               0b00001010, 10, 2, '5', '1',
               0b00001010, 10, 2, '5', '0',
               0b00001010, 10, 2, '4', '9'
            };
            Bytes_CheckState_ContainsBytes(pack, pattern);
         }
         else if constexpr (Same<E, RT>) {
            const uint8_t pattern[] = {
               0x35, 0x00, 0x00, 0x00, 0b00000010,
               0x36, 0x00, 0x00, 0x00, 0b00000010,
               0x36, 0x00, 0x00, 0x00, 0b00000010,
               0x36, 0x00, 0x00, 0x00, 0b00000010,
               0x34, 0x00, 0x00, 0x00, 0b00000010,
               0x33, 0x00, 0x00, 0x00, 0b00000010,
               0x32, 0x00, 0x00, 0x00, 0b00000010,
               0x31, 0x00, 0x00, 0x00, 0b00000010
            };
            Bytes_CheckState_ContainsBytes(pack, pattern);
         }
         else if constexpr (Same<E, char>) {
            const uint8_t pattern[] = {
               0x35, 0x36, 0x36, 0x36, 0x34, 0x33, 0x32, 0x31 
            };
            Bytes_CheckState_ContainsBytes(pack, pattern);
         }
         else if constexpr (Same<E, int32_t>) {
            const uint8_t pattern[] = {
               0x35, 0x00, 0x00, 0x00, //53
               0x36, 0x00, 0x00, 0x00, //54
               0x36, 0x00, 0x00, 0x00, //54
               0x36, 0x00, 0x00, 0x00, //54
               0x34, 0x00, 0x00, 0x00, //52
               0x33, 0x00, 0x00, 0x00, //51
               0x32, 0x00, 0x00, 0x00, //50
               0x31, 0x00, 0x00, 0x00, //49
            };
            Bytes_CheckState_ContainsBytes(pack, pattern);
         }
         else if constexpr (Same<E, Many>) {
            const uint8_t pattern[] = {
               0b00000010, 4, 0x35, 0x00, 0x00, 0x00, //53
               0b00000010, 4, 0x36, 0x00, 0x00, 0x00, //54
               0b00000010, 4, 0x36, 0x00, 0x00, 0x00, //54
               0b00000010, 4, 0x36, 0x00, 0x00, 0x00, //54
               0b00000010, 4, 0x34, 0x00, 0x00, 0x00, //52
               0b00000010, 4, 0x33, 0x00, 0x00, 0x00, //51
               0b00000010, 4, 0x32, 0x00, 0x00, 0x00, //50
               0b00000010, 4, 0x31, 0x00, 0x00, 0x00, //49
            };
            Bytes_CheckState_ContainsBytes(pack, pattern);
         }
         else {
            const uint8_t pattern[] = {
               0b00001010, 11, 4, 0x35, 0x00, 0x00, 0x00, //53
               0b00001010, 11, 4, 0x36, 0x00, 0x00, 0x00, //54
               0b00001010, 11, 4, 0x36, 0x00, 0x00, 0x00, //54
               0b00001010, 11, 4, 0x36, 0x00, 0x00, 0x00, //54
               0b00001010, 11, 4, 0x34, 0x00, 0x00, 0x00, //52
               0b00001010, 11, 4, 0x33, 0x00, 0x00, 0x00, //51
               0b00001010, 11, 4, 0x32, 0x00, 0x00, 0x00, //50
               0b00001010, 11, 4, 0x31, 0x00, 0x00, 0x00, //49
            };
            Bytes_CheckState_ContainsBytes(pack, pattern);
         }

         BenchmarkBytesStd("Empty/Insert/Element/Front", 30, 100,
            T temp,              temp >> immovable[0],
            stdbyt temp_std,     temp_std.emplace_front(immovable[0])
         );
      }

      /// MARK: Concat array                                                  
      if constexpr (Same<E, Bytes>) {
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

            Bytes_CheckState_OwnedFull(pack);

            for (int i = 0; i < 5; ++i) {
               Bytes_CheckState_OwnedFull(immovable[i]);
               Bytes_CheckState_OwnedFull(movable1[i]);
               Bytes_CheckState_OwnedFull(movable2[i]);
               Bytes_CheckState_OwnedFull(movable3[i]);
            }

            const uint8_t pattern[] = {
               0x31, 0x00, 0x00, 0x00, //49
               0x32, 0x00, 0x00, 0x00, //50
               0x33, 0x00, 0x00, 0x00, //51
               0x34, 0x00, 0x00, 0x00, //52
               0x36, 0x00, 0x00, 0x00, //54
               0x37, 0x00, 0x00, 0x00, //55
               0x38, 0x00, 0x00, 0x00, //56
               0x35, 0x00, 0x00, 0x00, //53
            };
            REQUIRE(inserted == sizeof(pattern));
            Bytes_CheckState_ContainsBytes(pack, pattern);

            BenchmarkBytesStd("Empty/Concat/Element/Back", 30, 100,
               T temp,              temp.ConcatAt(Index::Back, immovable),
               stdbyt temp_std,     std::copy(immovable, immovable + 5, std::back_inserter(temp_std))
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

            Bytes_CheckState_OwnedFull(pack);

            for (int i = 0; i < 5; ++i) {
               Bytes_CheckState_OwnedFull(immovable[i]);
               Bytes_CheckState_OwnedFull(movable1[i]);
               Bytes_CheckState_OwnedFull(movable2[i]);
               Bytes_CheckState_OwnedFull(movable3[i]);
            }

            const uint8_t pattern[] = {
               0x35, 0x00, 0x00, 0x00, //53
               0x38, 0x00, 0x00, 0x00, //56
               0x37, 0x00, 0x00, 0x00, //55
               0x36, 0x00, 0x00, 0x00, //54
               0x34, 0x00, 0x00, 0x00, //52
               0x33, 0x00, 0x00, 0x00, //51
               0x32, 0x00, 0x00, 0x00, //50
               0x31, 0x00, 0x00, 0x00, //49
            };
            REQUIRE(inserted == sizeof(pattern));
            Bytes_CheckState_ContainsBytes(pack, pattern);

            BenchmarkBytesStd("Empty/Concat/Element/Front", 30, 100,
               T temp,              temp.ConcatAt(Index::Front, darray1),
               stdbyt temp_std,     std::copy(darray1, darray1 + 5, std::front_inserter(temp_std))
            );
         }

         /// MARK: Concat at                                                  
         WHEN("Concatenate to a non-existent index") {
            size_t inserted = 0;
            REQUIRE_THROWS(inserted = pack.ConcatAt(1000, immovable[0]));
            REQUIRE(inserted == 0);
            
            Bytes_CheckState_Default(pack);
         }
      }

      /// MARK: +=                                                            
      WHEN("Concatenate array at the back by using += operator") {
         REQUIRE_NOTHROW(pack +=           immovable );
         REQUIRE_NOTHROW(pack += Refer    {immovable});
         REQUIRE_NOTHROW(pack += Copy     {immovable});
         REQUIRE_NOTHROW(pack += Disown   {immovable});
         REQUIRE_NOTHROW(pack += std::move( movable1));
         REQUIRE_NOTHROW(pack += Move     { movable2});
         REQUIRE_NOTHROW(pack += Abandon  { movable3});
         REQUIRE_NOTHROW(pack += Clone    {immovable});

         Bytes_CheckState_OwnedFull(pack);

         /*if constexpr (Same<E, Bytes>) {
            for (int i = 0; i < 5; ++i) {
               Bytes_CheckState_DisownedFull(immovable[i]);
               Bytes_CheckState_DisownedFull(movable1[i]);
               Bytes_CheckState_DisownedFull(movable2[i]);
               Bytes_CheckState_DisownedFull(movable3[i]);
            }
         }
         else*/ if constexpr (CT::Container<E>) {
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
         else if constexpr (Same<E, RT>) {
            const uint8_t pattern[] = {
               0x31, 0x00, 0x00, 0x00, 0b00000010,
               0x32, 0x00, 0x00, 0x00, 0b00000010,
               0x33, 0x00, 0x00, 0x00, 0b00000010,
               0x34, 0x00, 0x00, 0x00, 0b00000010,
               0x35, 0x00, 0x00, 0x00, 0b00000010,

               0x31, 0x00, 0x00, 0x00, 0b00000010,
               0x32, 0x00, 0x00, 0x00, 0b00000010,
               0x33, 0x00, 0x00, 0x00, 0b00000010,
               0x34, 0x00, 0x00, 0x00, 0b00000010,
               0x35, 0x00, 0x00, 0x00, 0b00000010,

               0x31, 0x00, 0x00, 0x00, 0b00000010,
               0x32, 0x00, 0x00, 0x00, 0b00000010,
               0x33, 0x00, 0x00, 0x00, 0b00000010,
               0x34, 0x00, 0x00, 0x00, 0b00000010,
               0x35, 0x00, 0x00, 0x00, 0b00000010,

               0x31, 0x00, 0x00, 0x00, 0b00000010,
               0x32, 0x00, 0x00, 0x00, 0b00000010,
               0x33, 0x00, 0x00, 0x00, 0b00000010,
               0x34, 0x00, 0x00, 0x00, 0b00000010,
               0x35, 0x00, 0x00, 0x00, 0b00000010,

               0x36, 0x00, 0x00, 0x00, 0b00000010,
               0x37, 0x00, 0x00, 0x00, 0b00000010,
               0x38, 0x00, 0x00, 0x00, 0b00000010,
               0x39, 0x00, 0x00, 0x00, 0b00000010,
               0x3a, 0x00, 0x00, 0x00, 0b00000010,

               0x36, 0x00, 0x00, 0x00, 0b00000010,
               0x37, 0x00, 0x00, 0x00, 0b00000010,
               0x38, 0x00, 0x00, 0x00, 0b00000010,
               0x39, 0x00, 0x00, 0x00, 0b00000010,
               0x3a, 0x00, 0x00, 0x00, 0b00000010,

               0x36, 0x00, 0x00, 0x00, 0b00000010,
               0x37, 0x00, 0x00, 0x00, 0b00000010,
               0x38, 0x00, 0x00, 0x00, 0b00000010,
               0x39, 0x00, 0x00, 0x00, 0b00000010,
               0x3a, 0x00, 0x00, 0x00, 0b00000010,

               0x31, 0x00, 0x00, 0x00, 0b00000010,
               0x32, 0x00, 0x00, 0x00, 0b00000010,
               0x33, 0x00, 0x00, 0x00, 0b00000010,
               0x34, 0x00, 0x00, 0x00, 0b00000010,
               0x35, 0x00, 0x00, 0x00, 0b00000010
            };
            Bytes_CheckState_ContainsBytes(pack, pattern);
         }
         else if constexpr (Same<E, char>) {
            const uint8_t pattern[] = {
               0x31, 0x32, 0x33, 0x34, 0x35,
               0x31, 0x32, 0x33, 0x34, 0x35,
               0x31, 0x32, 0x33, 0x34, 0x35,
               0x31, 0x32, 0x33, 0x34, 0x35,
               0x36, 0x37, 0x38, 0x39, 0x3a,
               0x36, 0x37, 0x38, 0x39, 0x3a,
               0x36, 0x37, 0x38, 0x39, 0x3a,
               0x31, 0x32, 0x33, 0x34, 0x35
            };
            Bytes_CheckState_ContainsBytes(pack, pattern);
         }
         else if constexpr (Same<E, Text>) {
            const uint8_t pattern[] = {
               0b00001010, 10, 2, '4', '9',
               0b00001010, 10, 2, '5', '0',
               0b00001010, 10, 2, '5', '1',
               0b00001010, 10, 2, '5', '2',
               0b00001010, 10, 2, '5', '3',

               0b00001010, 10, 2, '4', '9',
               0b00001010, 10, 2, '5', '0',
               0b00001010, 10, 2, '5', '1',
               0b00001010, 10, 2, '5', '2',
               0b00001010, 10, 2, '5', '3',

               0b00001010, 10, 2, '4', '9',
               0b00001010, 10, 2, '5', '0',
               0b00001010, 10, 2, '5', '1',
               0b00001010, 10, 2, '5', '2',
               0b00001010, 10, 2, '5', '3',

               0b00001010, 10, 2, '4', '9',
               0b00001010, 10, 2, '5', '0',
               0b00001010, 10, 2, '5', '1',
               0b00001010, 10, 2, '5', '2',
               0b00001010, 10, 2, '5', '3',

               0b00001010, 10, 2, '5', '4',
               0b00001010, 10, 2, '5', '5',
               0b00001010, 10, 2, '5', '6',
               0b00001010, 10, 2, '5', '7',
               0b00001010, 10, 2, '5', '8',

               0b00001010, 10, 2, '5', '4',
               0b00001010, 10, 2, '5', '5',
               0b00001010, 10, 2, '5', '6',
               0b00001010, 10, 2, '5', '7',
               0b00001010, 10, 2, '5', '8',

               0b00001010, 10, 2, '5', '4',
               0b00001010, 10, 2, '5', '5',
               0b00001010, 10, 2, '5', '6',
               0b00001010, 10, 2, '5', '7',
               0b00001010, 10, 2, '5', '8',

               0b00001010, 10, 2, '4', '9',
               0b00001010, 10, 2, '5', '0',
               0b00001010, 10, 2, '5', '1',
               0b00001010, 10, 2, '5', '2',
               0b00001010, 10, 2, '5', '3'
            };
            Bytes_CheckState_ContainsBytes(pack, pattern);
         }
         else if constexpr (Same<E, Many>) {
            const uint8_t pattern[] = {
               0b00000010, 4, 0x31, 0x00, 0x00, 0x00, //49
               0b00000010, 4, 0x32, 0x00, 0x00, 0x00, //50
               0b00000010, 4, 0x33, 0x00, 0x00, 0x00, //51
               0b00000010, 4, 0x34, 0x00, 0x00, 0x00, //52
               0b00000010, 4, 0x35, 0x00, 0x00, 0x00, //53

               0b00000010, 4, 0x31, 0x00, 0x00, 0x00, //49
               0b00000010, 4, 0x32, 0x00, 0x00, 0x00, //50
               0b00000010, 4, 0x33, 0x00, 0x00, 0x00, //51
               0b00000010, 4, 0x34, 0x00, 0x00, 0x00, //52
               0b00000010, 4, 0x35, 0x00, 0x00, 0x00, //53

               0b00000010, 4, 0x31, 0x00, 0x00, 0x00, //49
               0b00000010, 4, 0x32, 0x00, 0x00, 0x00, //50
               0b00000010, 4, 0x33, 0x00, 0x00, 0x00, //51
               0b00000010, 4, 0x34, 0x00, 0x00, 0x00, //52
               0b00000010, 4, 0x35, 0x00, 0x00, 0x00, //53

               0b00000010, 4, 0x31, 0x00, 0x00, 0x00, //49
               0b00000010, 4, 0x32, 0x00, 0x00, 0x00, //50
               0b00000010, 4, 0x33, 0x00, 0x00, 0x00, //51
               0b00000010, 4, 0x34, 0x00, 0x00, 0x00, //52
               0b00000010, 4, 0x35, 0x00, 0x00, 0x00, //53

               0b00000010, 4, 0x36, 0x00, 0x00, 0x00, //54
               0b00000010, 4, 0x37, 0x00, 0x00, 0x00, //55
               0b00000010, 4, 0x38, 0x00, 0x00, 0x00, //56
               0b00000010, 4, 0x39, 0x00, 0x00, 0x00, //57
               0b00000010, 4, 0x3a, 0x00, 0x00, 0x00, //58

               0b00000010, 4, 0x36, 0x00, 0x00, 0x00, //54
               0b00000010, 4, 0x37, 0x00, 0x00, 0x00, //55
               0b00000010, 4, 0x38, 0x00, 0x00, 0x00, //56
               0b00000010, 4, 0x39, 0x00, 0x00, 0x00, //57
               0b00000010, 4, 0x3a, 0x00, 0x00, 0x00, //58

               0b00000010, 4, 0x36, 0x00, 0x00, 0x00, //54
               0b00000010, 4, 0x37, 0x00, 0x00, 0x00, //55
               0b00000010, 4, 0x38, 0x00, 0x00, 0x00, //56
               0b00000010, 4, 0x39, 0x00, 0x00, 0x00, //57
               0b00000010, 4, 0x3a, 0x00, 0x00, 0x00, //58

               0b00000010, 4, 0x31, 0x00, 0x00, 0x00, //49
               0b00000010, 4, 0x32, 0x00, 0x00, 0x00, //50
               0b00000010, 4, 0x33, 0x00, 0x00, 0x00, //51
               0b00000010, 4, 0x34, 0x00, 0x00, 0x00, //52
               0b00000010, 4, 0x35, 0x00, 0x00, 0x00  //53
            };
            Bytes_CheckState_ContainsBytes(pack, pattern);
         }
         else {
            const uint8_t pattern[] = {
               0x31, 0x00, 0x00, 0x00, //49
               0x32, 0x00, 0x00, 0x00, //50
               0x33, 0x00, 0x00, 0x00, //51
               0x34, 0x00, 0x00, 0x00, //52
               0x35, 0x00, 0x00, 0x00, //53

               0x31, 0x00, 0x00, 0x00, //49
               0x32, 0x00, 0x00, 0x00, //50
               0x33, 0x00, 0x00, 0x00, //51
               0x34, 0x00, 0x00, 0x00, //52
               0x35, 0x00, 0x00, 0x00, //53

               0x31, 0x00, 0x00, 0x00, //49
               0x32, 0x00, 0x00, 0x00, //50
               0x33, 0x00, 0x00, 0x00, //51
               0x34, 0x00, 0x00, 0x00, //52
               0x35, 0x00, 0x00, 0x00, //53

               0x31, 0x00, 0x00, 0x00, //49
               0x32, 0x00, 0x00, 0x00, //50
               0x33, 0x00, 0x00, 0x00, //51
               0x34, 0x00, 0x00, 0x00, //52
               0x35, 0x00, 0x00, 0x00, //53

               0x36, 0x00, 0x00, 0x00, //54
               0x37, 0x00, 0x00, 0x00, //55
               0x38, 0x00, 0x00, 0x00, //56
               0x39, 0x00, 0x00, 0x00, //57
               0x3a, 0x00, 0x00, 0x00, //58

               0x36, 0x00, 0x00, 0x00, //54
               0x37, 0x00, 0x00, 0x00, //55
               0x38, 0x00, 0x00, 0x00, //56
               0x39, 0x00, 0x00, 0x00, //57
               0x3a, 0x00, 0x00, 0x00, //58

               0x36, 0x00, 0x00, 0x00, //54
               0x37, 0x00, 0x00, 0x00, //55
               0x38, 0x00, 0x00, 0x00, //56
               0x39, 0x00, 0x00, 0x00, //57
               0x3a, 0x00, 0x00, 0x00, //58

               0x31, 0x00, 0x00, 0x00, //49
               0x32, 0x00, 0x00, 0x00, //50
               0x33, 0x00, 0x00, 0x00, //51
               0x34, 0x00, 0x00, 0x00, //52
               0x35, 0x00, 0x00, 0x00  //53
            };
            Bytes_CheckState_ContainsBytes(pack, pattern);
         }

         BenchmarkBytesStd("Empty/+=/Array/Back", 30, 100,
            T temp,              temp += immovable,
            stdbyt temp_std,     temp_std.emplace_back(immovable[0])
         );
      }

      WHEN("Concatenate element the back by using += operator") {
         REQUIRE_NOTHROW(pack +=           immovable[0] );
         REQUIRE_NOTHROW(pack += Refer    {immovable[1]});
         REQUIRE_NOTHROW(pack += Copy     {immovable[2]});
         REQUIRE_NOTHROW(pack += Disown   {immovable[3]});
         REQUIRE_NOTHROW(pack += std::move( movable1[0]));
         REQUIRE_NOTHROW(pack += Move     { movable2[0]});
         REQUIRE_NOTHROW(pack += Abandon  { movable3[0]});
         REQUIRE_NOTHROW(pack += Clone    {immovable[4]});

         Bytes_CheckState_OwnedFull(pack);

         /*if constexpr (Same<E, Bytes>) {
            for (int i = 0; i < 5; ++i)
               Bytes_CheckState_DisownedFull(immovable[i]);
            Bytes_CheckState_DisownedFull(movable1[0]);
            Bytes_CheckState_DisownedFull(movable2[0]);
            Bytes_CheckState_DisownedFull(movable3[0]);
         }
         else*/ if constexpr (CT::Container<E>) {
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
            const uint8_t pattern[] = {
               0x31, 0x00, 0x00, 0x00, 0b00000010,
               0x32, 0x00, 0x00, 0x00, 0b00000010,
               0x33, 0x00, 0x00, 0x00, 0b00000010,
               0x34, 0x00, 0x00, 0x00, 0b00000010,
               0x36, 0x00, 0x00, 0x00, 0b00000010,
               0x36, 0x00, 0x00, 0x00, 0b00000010,
               0x36, 0x00, 0x00, 0x00, 0b00000010,
               0x35, 0x00, 0x00, 0x00, 0b00000010
            };
            Bytes_CheckState_ContainsBytes(pack, pattern);
         }
         else if constexpr (Same<E, char>) {
            const uint8_t pattern[] = {
               0x31, 0x32, 0x33, 0x34, 0x36, 0x36, 0x36, 0x35
            };
            Bytes_CheckState_ContainsBytes(pack, pattern);
         }
         else if constexpr (Same<E, Text>) {
            const uint8_t pattern[] = {
               0b00001010, 10, 2, '4', '9',
               0b00001010, 10, 2, '5', '0',
               0b00001010, 10, 2, '5', '1',
               0b00001010, 10, 2, '5', '2',
               0b00001010, 10, 2, '5', '4',
               0b00001010, 10, 2, '5', '4',
               0b00001010, 10, 2, '5', '4',
               0b00001010, 10, 2, '5', '3'
            };
            Bytes_CheckState_ContainsBytes(pack, pattern);
         }
         else if constexpr (Same<E, Many>) {
            const uint8_t pattern[] = {
               0b00000010, 4, 0x31, 0x00, 0x00, 0x00, //49
               0b00000010, 4, 0x32, 0x00, 0x00, 0x00, //50
               0b00000010, 4, 0x33, 0x00, 0x00, 0x00, //51
               0b00000010, 4, 0x34, 0x00, 0x00, 0x00, //52
               0b00000010, 4, 0x36, 0x00, 0x00, 0x00, //54
               0b00000010, 4, 0x36, 0x00, 0x00, 0x00, //54
               0b00000010, 4, 0x36, 0x00, 0x00, 0x00, //54
               0b00000010, 4, 0x35, 0x00, 0x00, 0x00, //53
            };
            Bytes_CheckState_ContainsBytes(pack, pattern);
         }
         else {
            const uint8_t pattern[] = {
               0x31, 0x00, 0x00, 0x00, //49
               0x32, 0x00, 0x00, 0x00, //50
               0x33, 0x00, 0x00, 0x00, //51
               0x34, 0x00, 0x00, 0x00, //52
               0x36, 0x00, 0x00, 0x00, //54
               0x36, 0x00, 0x00, 0x00, //54
               0x36, 0x00, 0x00, 0x00, //54
               0x35, 0x00, 0x00, 0x00  //53
            };
            Bytes_CheckState_ContainsBytes(pack, pattern);
         }

         BenchmarkBytesStd("Empty/+=/Element/Back", 30, 100,
            T temp,              temp += immovable[0],
            stdbyt temp_std,     temp_std.emplace_back(immovable[0])
         );
      }
   }

   REQUIRE(memoryState.Assert());
   REQUIRE_FALSE(Allocator::CollectGarbage());
}