///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Component.hpp"
//#include <Langulus/CT/Index.hpp>
//#include <Langulus/Allocator.hpp>


namespace Langulus::Annies::Component
{
   ///                                                                        
   /// Designates stacks/heaps to separate dimensions. Useful for forming     
   /// pairs, maps, etc.                                                      
   ///   @tparam ID1, ID2, IDN... the providers we're designating             
   ///   @attention all dimensions share the same count/reserve components    
   template<Cid ID0, Cid ID1, Cid...IDN>
   struct Multidimensional {
      using CTTI_Component = Yup;
      using CTTI_ReflectAs = void;
      using Id             = Values<ID0, ID1, IDN...>;
      using Dimensions     = Id;

      static constexpr int ComponentPrecedence = -1000;
   };
}