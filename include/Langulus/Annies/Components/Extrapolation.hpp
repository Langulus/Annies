///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Component.hpp"


namespace Langulus::Annies::Component
{
   struct Extrapolation {
      using CTTI_Component = Yup;
      using CTTI_ReflectAs = void;

      static constexpr int ComponentPrecedence = 3000;
   };
}
