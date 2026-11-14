///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "TestTextCommon.hpp"


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
   , Types<Many, Text,   ScopedElement<Text>>
   , Types<Many, int,    ScopedElement<int>>
   , Types<Many, Many,   ScopedElement<Many>>
   , Types<Many, RT,     ScopedElement<RT>>
   , Types<Many, char,   ScopedElement<char>>

   , Types<Many, Text*,  ScopedElement<Text*>>
   , Types<Many, int*,   ScopedElement<int*>>
   , Types<Many, Many*,  ScopedElement<Many*>>
   , Types<Many, RT*,    ScopedElement<RT*>>
   , Types<Many, char*,  ScopedElement<char*>>

   , Types<Many, Text**, ScopedElement<Text**>>
   , Types<Many, int**,  ScopedElement<int**>>
   , Types<Many, Many**, ScopedElement<Many**>>
   , Types<Many, RT**,   ScopedElement<RT**>>
   , Types<Many, char**, ScopedElement<char**>>

   , Types<TMany<Text>,   Text,   ScopedElement<Text>>
   , Types<TMany<int>,    int,    ScopedElement<int>>
   , Types<TMany<Many>,   Many,   ScopedElement<Many>>
   , Types<TMany<RT>,     RT,     ScopedElement<RT>>
   , Types<TMany<char>,   char,   ScopedElement<char>>

   , Types<TMany<Text*>,  Text*,  ScopedElement<Text*>>
   , Types<TMany<int*>,   int*,   ScopedElement<int*>>
   , Types<TMany<Many*>,  Many*,  ScopedElement<Many*>>
   , Types<TMany<RT*>,    RT*,    ScopedElement<RT*>>
   , Types<TMany<char*>,  char*,  ScopedElement<char*>>

   , Types<TMany<Text**>, Text**, ScopedElement<Text**>>
   , Types<TMany<int**>,  int**,  ScopedElement<int**>>
   , Types<TMany<Many**>, Many**, ScopedElement<Many**>>
   , Types<TMany<RT**>,   RT**,   ScopedElement<RT**>>
   , Types<TMany<char**>, char**, ScopedElement<char**>>

   #if LANGULUS_FEATURE(MANAGED_MEMORY)
   // Elements are allocated by the memory manager                      
   , Types<Many, Text,   ScopedElement<Text, true>>
   , Types<Many, int,    ScopedElement<int, true>>
   , Types<Many, Many,   ScopedElement<Many, true>>
   , Types<Many, RT,     ScopedElement<RT, true>>
   , Types<Many, char,   ScopedElement<char, true>>

   , Types<Many, Text*,  ScopedElement<Text*, true>>
   , Types<Many, int*,   ScopedElement<int*, true>>
   , Types<Many, Many*,  ScopedElement<Many*, true>>
   , Types<Many, RT*,    ScopedElement<RT*, true>>
   , Types<Many, char*,  ScopedElement<char*, true>>

   , Types<Many, Text**, ScopedElement<Text**, true>>
   , Types<Many, int**,  ScopedElement<int**, true>>
   , Types<Many, Many**, ScopedElement<Many**, true>>
   , Types<Many, RT**,   ScopedElement<RT**, true>>
   , Types<Many, char**, ScopedElement<char**, true>>

   , Types<TMany<Text>,   Text,   ScopedElement<Text, true>>
   , Types<TMany<int>,    int,    ScopedElement<int, true>>
   , Types<TMany<Many>,   Many,   ScopedElement<Many, true>>
   , Types<TMany<RT>,     RT,     ScopedElement<RT, true>>
   , Types<TMany<char>,   char,   ScopedElement<char, true>>

   , Types<TMany<Text*>,  Text*,  ScopedElement<Text*, true>>
   , Types<TMany<int*>,   int*,   ScopedElement<int*, true>>
   , Types<TMany<Many*>,  Many*,  ScopedElement<Many*, true>>
   , Types<TMany<RT*>,    RT*,    ScopedElement<RT*, true>>
   , Types<TMany<char*>,  char*,  ScopedElement<char*, true>>

   , Types<TMany<Text**>, Text**, ScopedElement<Text**, true>>
   , Types<TMany<int**>,  int**,  ScopedElement<int**, true>>
   , Types<TMany<Many**>, Many**, ScopedElement<Many**, true>>
   , Types<TMany<RT**>,   RT**,   ScopedElement<RT**, true>>
   , Types<TMany<char**>, char**, ScopedElement<char**, true>>

   // Packed pointers                                                   
   , Types<Many, pptr8,  ScopedElementPacked<pptr8>>
   , Types<Many, pptr16, ScopedElementPacked<pptr16>>
   , Types<Many, pptr32, ScopedElementPacked<pptr32>>

   , Types<TMany<pptr8>,  pptr8,  ScopedElementPacked<pptr8>>
   , Types<TMany<pptr16>, pptr16, ScopedElementPacked<pptr16>>
   , Types<TMany<pptr32>, pptr32, ScopedElementPacked<pptr32>>
   #endif
) {
   static MemoryState memoryState;
   using T = typename TestType::First;
   using E = typename TestType::Second;
   using ScopedE = typename TestType::template At<2>;
   constexpr bool Managed = ScopedE::Managed;

   //TODO base off TestMany_Empty.cpp

   REQUIRE(memoryState.Assert());
   REQUIRE_FALSE(Allocator::CollectGarbage());
}
