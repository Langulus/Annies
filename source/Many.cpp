///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include <Langulus/Many.hpp>
#include <Langulus/Annies/Functional/Compare.hpp>
#include <Langulus/Annies/Functional/Assign.hpp>
#include <Langulus/Annies/Functional/Deepen.hpp>
#include <Langulus/Annies/Functional/Remove.hpp>


namespace Langulus::Annies
{
   template void  Inner::ManyBase::ConstructDefault<Many>(this Many&& self) noexcept;
   template void  Inner::ManyBase::Absorb<Many, Many>(this Many& self, Many&& from);
   template void  Inner::ManyBase::Destroy<Many>(this Many& self);
   template void  Inner::ManyBase::Free<true, Many>(this Many& self);
   template void  Inner::ManyBase::Free<false, Many>(this Many& self);
   template void  Inner::ManyBase::Keep<Many>(this Many& self);
   template Many& Inner::ManyBase::AssignAbsorb<Many, Many>(this Many& self, Many&& rhs);
   template bool  Inner::ManyBase::IsMissingDeep<Many>(this Many const& self) noexcept;

   template void Component::Assignment<false, 0>::PrepareForReconstruction<Many>(this Many&);
   template bool Component::Assignment<false, 0>::PrepareForReassignment<Many>(this Many&);

   template bool   Component::Comparison<false, true, 0>::CompareEqual<0, Many, Many>(this Many const&, Many const&);
   template bool   Component::Comparison<false, true, 0>::CompareHashes<Many, Many>(this Many const&, Many const&);
   template size_t Component::Comparison<false, true, 0>::Matches<Many, Many>(this Many const&, Many const&);
   
   template size_t Component::Deepening<Many>::GetCountDeep<Many>(this Many const&, size_t);
   template size_t Component::Deepening<Many>::GetReservedDeep<Many>(this Many const&, size_t);
   template Many&  Component::Deepening<Many>::Deepen<Many>(this Many&);

   template void Component::Removal<0>::Trim<Many>(this Many&, size_t) noexcept;
   template void Component::Removal<0>::Clear<Many>(this Many&);
   template void Component::Removal<0>::Reset<Many>(this Many&);

   bool Many::operator == (Many const& other) const noexcept {
      return this->CompareEqual(other);
   }
}