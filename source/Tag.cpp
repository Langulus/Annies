///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include <Langulus/TTag.hpp>
#include <Langulus/SerializeText.hpp>
#include <Langulus/Annies/Functional/Compare.hpp>
#include <Langulus/Annies/Functional/Remove.hpp>


namespace Langulus::Annies::Component
{
   template void  Inner::TTagBase<void>::ConstructDefault<Tag>(this Tag&& self) noexcept;
   template void  Inner::TTagBase<void>::Absorb<Tag, Tag>(this Tag& self, Tag&& from);
   template void  Inner::TTagBase<void>::Destroy<Tag>(this Tag& self);
   template void  Inner::TTagBase<void>::Free<true, Tag>(this Tag& self);
   template void  Inner::TTagBase<void>::Free<false, Tag>(this Tag& self);
   template void  Inner::TTagBase<void>::Keep<Tag>(this Tag& self);
   template Tag&  Inner::TTagBase<void>::AssignAbsorb<Tag, Tag>(this Tag& self, Tag&& rhs);
   template bool  Inner::TTagBase<void>::IsMissingDeep<Tag>(this Tag const& self) noexcept;
}

namespace Langulus::Annies
{
   template struct LANGULUS_API(ANNIES) TTag<void>;
}