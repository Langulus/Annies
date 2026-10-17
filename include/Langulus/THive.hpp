///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "TSlots.hpp"


namespace Langulus::Annies
{
   ///                                                                        
   /// A statically-typed non-continuous unordered container of variable size 
   /// that guarantees elements will _never_ move from the memory they were   
   /// first instantiated in.                                                 
   template<CT::NotVoid T>
   struct THive : TSlots<TSlots<T>> {
      using CTTI_ReflectAs = THive;
      using TSlots<TSlots<T>>::TSlots;
   };
}

namespace Langulus
{
   using Annies::THive;
}