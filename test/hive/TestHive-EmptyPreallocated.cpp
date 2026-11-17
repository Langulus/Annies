///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "TestHiveCommon.hpp"


namespace Langulus::Annies  
{
   // Reuses definitions from TestHive-Empty.cpp. Reduces compile time.  
   extern template struct THive<Text>;
   extern template struct THive<int>;
   extern template struct THive<Any>;
   extern template struct THive<RT>;
   extern template struct THive<char>;

   extern template struct THive<Text*>;
   extern template struct THive<int*>;
   extern template struct THive<Any*>;
   extern template struct THive<RT*>;
   extern template struct THive<char*>;

   extern template struct THive<Text**>;
   extern template struct THive<int**>;
   extern template struct THive<Any**>;
   extern template struct THive<RT**>;
   extern template struct THive<char**>;

#if LANGULUS_FEATURE(MANAGED_MEMORY)
   extern template struct THive<pptr8>;
   extern template struct THive<pptr16>;
   extern template struct THive<pptr32>;
#endif
}

TEST_CASE_TEMPLATE("Test empty but preallocated hive", TestType
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

   constexpr bool Managed = ScopedE::Managed;

   //TODO base off TestMany_Empty.cpp

   REQUIRE(memoryState.Assert());
   REQUIRE_FALSE(Allocator::CollectGarbage());
}
