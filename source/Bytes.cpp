///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include <Langulus/Bytes.hpp>
#include <Langulus/Annies/Functional/Compare.hpp>

namespace Langulus::Annies
{
   bool Bytes::operator == (Bytes const& other) const noexcept {
      return this->CompareEqual(other);
   }
}