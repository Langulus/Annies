///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include <Langulus/Many.hpp>
#include <Langulus/Annies/Functional/Compare.hpp>


namespace Langulus::Annies
{
   bool Many::operator == (Many const& other) const noexcept {
      return this->CompareEqual(other);
   }
}