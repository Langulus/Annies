///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#include <Langulus/Testing.hpp>
#include <Langulus/Testing/CommonTypes.hpp>
#include <Langulus/Anyness/Any.hpp>

using namespace Langulus;


///                                                                           
/// CT::Container                                                             
///                                                                           
TEST_CASE_TEMPLATE("Testing CT::Container types", TestType
   //, IncompleteType                  // shouldn't compile
   //, IncompleteType const            // shouldn't compile
   //, IncompleteType const&           // shouldn't compile
   //, SheddableType<IncompleteType>   // shouldn't compile
   //, SheddableType<IncompleteType>&  // shouldn't compile
   , CustomContainer
   , CustomContainer const
   , CustomContainer&
   , CustomContainer[5]
   , CustomContainer[5][5]
   , SheddableType<CustomContainer>
   , Refer<CustomContainer>
   , Move<CustomContainer>
   , Copy<CustomContainer>
   , Abandon<CustomContainer>
   , Disown<CustomContainer>
   , Clone<CustomContainer>
   , SheddableType<CustomContainer[5]>
   , Refer<CustomContainer[5]>
   , Move<CustomContainer[5]>
   , Copy<CustomContainer[5]>
   , Abandon<CustomContainer[5]>
   , Disown<CustomContainer[5]>
   , Clone<CustomContainer[5]>
   , Clone<CustomContainer[5][5]>
) {
   static_assert(CT::Container<TestType>);
}

TEST_CASE_TEMPLATE("Testing non CT::Container types", TestType
   //, IncompleteType                  // shouldn't compile
   //, IncompleteType const            // shouldn't compile
   //, IncompleteType const&           // shouldn't compile
   //, SheddableType<IncompleteType>   // shouldn't compile
   //, SheddableType<IncompleteType>&  // shouldn't compile
   , IncompleteType*
   , void, void*
   , int, int const, int const&, int&
   , Types<void*>
   , CustomContainer*
   , SheddableType<CustomContainer*>
   , CustomContainer*[5]
   , SheddableType<CustomContainer*[5]>
   , SheddableType<CustomContainer*[5][5]>
) {
   static_assert(not CT::Container<TestType>);
}

//static_assert(    CT::Container<>); // shouldn't compile at all
static_assert(    CT::Container<CustomContainer, CustomContainer, CustomContainer>);
static_assert(not CT::Container<CustomContainer, CustomContainer, int>);