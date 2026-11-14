///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "TestManyCommon.hpp"


namespace Langulus::Annies  
{
   // Reuses definitions from TestMany-Empty.cpp. Reduces compile time.  
   extern template struct TMany<Text>;
   extern template struct TMany<int>;
   extern template struct TMany<Any>;
   extern template struct TMany<RT>;
   extern template struct TMany<char>;

   extern template struct TMany<Text*>;
   extern template struct TMany<int*>;
   extern template struct TMany<Any*>;
   extern template struct TMany<RT*>;
   extern template struct TMany<char*>;

   extern template struct TMany<Text**>;
   extern template struct TMany<int**>;
   extern template struct TMany<Any**>;
   extern template struct TMany<RT**>;
   extern template struct TMany<char**>;

#if LANGULUS_FEATURE(MANAGED_MEMORY)
   extern template struct TMany<pptr8>;
   extern template struct TMany<pptr16>;
   extern template struct TMany<pptr32>;
#endif
}

TEST_CASE_TEMPLATE("Test empty but preallocated Many/TMany", TestType
   // Elements are not allocated by the memory manager                  
   , Types<Many, ScopedElement<Text>>
   , Types<Many, ScopedElement<int>>
   , Types<Many, ScopedElement<Many>>
   , Types<Many, ScopedElement<RT>>
   , Types<Many, ScopedElement<char>>

   , Types<Many, ScopedElement<Text*>>
   , Types<Many, ScopedElement<int*>>
   , Types<Many, ScopedElement<Many*>>
   , Types<Many, ScopedElement<RT*>>
   , Types<Many, ScopedElement<char*>>

   , Types<Many, ScopedElement<Text**>>
   , Types<Many, ScopedElement<int**>>
   , Types<Many, ScopedElement<Many**>>
   , Types<Many, ScopedElement<RT**>>
   , Types<Many, ScopedElement<char**>>

   , Types<TMany<Text>,   ScopedElement<Text>>
   , Types<TMany<int>,    ScopedElement<int>>
   , Types<TMany<Many>,   ScopedElement<Many>>
   , Types<TMany<RT>,     ScopedElement<RT>>
   , Types<TMany<char>,   ScopedElement<char>>

   , Types<TMany<Text*>,  ScopedElement<Text*>>
   , Types<TMany<int*>,   ScopedElement<int*>>
   , Types<TMany<Many*>,  ScopedElement<Many*>>
   , Types<TMany<RT*>,    ScopedElement<RT*>>
   , Types<TMany<char*>,  ScopedElement<char*>>

   , Types<TMany<Text**>, ScopedElement<Text**>>
   , Types<TMany<int**>,  ScopedElement<int**>>
   , Types<TMany<Many**>, ScopedElement<Many**>>
   , Types<TMany<RT**>,   ScopedElement<RT**>>
   , Types<TMany<char**>, ScopedElement<char**>>

   #if LANGULUS_FEATURE(MANAGED_MEMORY)
   // Elements are allocated by the memory manager                      
   , Types<Many, ScopedElement<Text, true>>
   , Types<Many, ScopedElement<int, true>>
   , Types<Many, ScopedElement<Many, true>>
   , Types<Many, ScopedElement<RT, true>>
   , Types<Many, ScopedElement<char, true>>

   , Types<Many, ScopedElement<Text*, true>>
   , Types<Many, ScopedElement<int*, true>>
   , Types<Many, ScopedElement<Many*, true>>
   , Types<Many, ScopedElement<RT*, true>>
   , Types<Many, ScopedElement<char*, true>>

   , Types<Many, ScopedElement<Text**, true>>
   , Types<Many, ScopedElement<int**, true>>
   , Types<Many, ScopedElement<Many**, true>>
   , Types<Many, ScopedElement<RT**, true>>
   , Types<Many, ScopedElement<char**, true>>

   , Types<TMany<Text>,   ScopedElement<Text, true>>
   , Types<TMany<int>,    ScopedElement<int, true>>
   , Types<TMany<Many>,   ScopedElement<Many, true>>
   , Types<TMany<RT>,     ScopedElement<RT, true>>
   , Types<TMany<char>,   ScopedElement<char, true>>

   , Types<TMany<Text*>,  ScopedElement<Text*, true>>
   , Types<TMany<int*>,   ScopedElement<int*, true>>
   , Types<TMany<Many*>,  ScopedElement<Many*, true>>
   , Types<TMany<RT*>,    ScopedElement<RT*, true>>
   , Types<TMany<char*>,  ScopedElement<char*, true>>

   , Types<TMany<Text**>, ScopedElement<Text**, true>>
   , Types<TMany<int**>,  ScopedElement<int**, true>>
   , Types<TMany<Many**>, ScopedElement<Many**, true>>
   , Types<TMany<RT**>,   ScopedElement<RT**, true>>
   , Types<TMany<char**>, ScopedElement<char**, true>>

   // Packed pointers                                                   
   , Types<Many, ScopedElementPacked<pptr8>>
   , Types<Many, ScopedElementPacked<pptr16>>
   , Types<Many, ScopedElementPacked<pptr32>>

   , Types<TMany<pptr8>,  ScopedElementPacked<pptr8>>
   , Types<TMany<pptr16>, ScopedElementPacked<pptr16>>
   , Types<TMany<pptr32>, ScopedElementPacked<pptr32>>
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
