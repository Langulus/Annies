///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include <Langulus/Neat.hpp>
#include <Langulus/Annies/Functional/Compare.hpp>
#include <Langulus/Annies/Functional/Assign.hpp>
#include <Langulus/Annies/Functional/Deepen.hpp>
#include <Langulus/Annies/Functional/Remove.hpp>


namespace Langulus::Annies::Component
{
   template LANGULUS_EXPORT() void  Inner::NeatBase::ConstructDefault<Neat>(this Neat&& self) noexcept;
   //template void  Inner::NeatBase::Absorb<Many, Many>(this Many& self, Many&& from);
   template LANGULUS_EXPORT() void  Inner::NeatBase::Destroy<Neat>(this Neat& self);
   template LANGULUS_EXPORT() void  Inner::NeatBase::Free<true, Neat>(this Neat& self);
   template LANGULUS_EXPORT() void  Inner::NeatBase::Free<false, Neat>(this Neat& self);
   template LANGULUS_EXPORT() void  Inner::NeatBase::Keep<Neat>(this Neat& self);
   //template Many& Inner::NeatBase::AssignAbsorb<Many, Many>(this Many& self, Many&& rhs);
   template LANGULUS_EXPORT() bool  Inner::NeatBase::IsMissingDeep<Neat>(this Neat const& self) noexcept;
}

namespace Langulus::Annies::Component
{
   template LANGULUS_EXPORT() void  Inner::TManyBase<Recipe>::ConstructDefault<TMany<Recipe>>(this TMany<Recipe>&& self) noexcept;
   template LANGULUS_EXPORT() void  Inner::TManyBase<Recipe>::Absorb<TMany<Recipe>, TMany<Recipe>>(this TMany<Recipe>& self, TMany<Recipe>&& from);
   template LANGULUS_EXPORT() void  Inner::TManyBase<Recipe>::Destroy<TMany<Recipe>>(this TMany<Recipe>& self);
   template LANGULUS_EXPORT() void  Inner::TManyBase<Recipe>::Free<true, TMany<Recipe>>(this TMany<Recipe>& self);
   template LANGULUS_EXPORT() void  Inner::TManyBase<Recipe>::Free<false, TMany<Recipe>>(this TMany<Recipe>& self);
   template LANGULUS_EXPORT() void  Inner::TManyBase<Recipe>::Keep<TMany<Recipe>>(this TMany<Recipe>& self);
   template LANGULUS_EXPORT() TMany<Recipe>& Inner::TManyBase<Recipe>::AssignAbsorb<TMany<Recipe>, TMany<Recipe>>(this TMany<Recipe>& self, TMany<Recipe>&& rhs);
   template LANGULUS_EXPORT() bool  Inner::TManyBase<Recipe>::IsMissingDeep<TMany<Recipe>>(this TMany<Recipe> const& self) noexcept;

   template LANGULUS_EXPORT() void  Assignment<false, 0>::PrepareForReconstruction<TMany<Recipe>>(this TMany<Recipe>&);
   template LANGULUS_EXPORT() bool  Assignment<false, 0>::PrepareForReassignment<TMany<Recipe>>(this TMany<Recipe>&);

   template LANGULUS_EXPORT() bool   Comparison<false, true, 0>::CompareEqual<0, TMany<Recipe>, TMany<Recipe>>(this TMany<Recipe> const&, TMany<Recipe> const&);
   template LANGULUS_EXPORT() bool   Comparison<false, true, 0>::CompareHashes<TMany<Recipe>, TMany<Recipe>>(this TMany<Recipe> const&, TMany<Recipe> const&);
   template LANGULUS_EXPORT() size_t Comparison<false, true, 0>::Matches<TMany<Recipe>, TMany<Recipe>>(this TMany<Recipe> const&, TMany<Recipe> const&);

   template LANGULUS_EXPORT() size_t Deepening<Many>::GetCountDeep<TMany<Recipe>>(this TMany<Recipe> const&, size_t);
   template LANGULUS_EXPORT() size_t Deepening<Many>::GetReservedDeep<TMany<Recipe>>(this TMany<Recipe> const&, size_t);
   //template Many&  Deepening<Many>::Deepen<TMany<Recipe>>(this TMany<Recipe>&);

   template LANGULUS_EXPORT() void Removal<0>::Trim<TMany<Recipe>>(this TMany<Recipe>&, size_t) noexcept;
   template LANGULUS_EXPORT() void Removal<0>::Clear<TMany<Recipe>>(this TMany<Recipe>&);
   template LANGULUS_EXPORT() void Removal<0>::Reset<TMany<Recipe>>(this TMany<Recipe>&);
}

namespace Langulus::Annies::Component
{
   template LANGULUS_EXPORT() void  Inner::TManyBase<Tag>::ConstructDefault<TMany<Tag>>(this TMany<Tag>&& self) noexcept;
   template LANGULUS_EXPORT() void  Inner::TManyBase<Tag>::Absorb<TMany<Tag>, TMany<Tag>>(this TMany<Tag>& self, TMany<Tag>&& from);
   template LANGULUS_EXPORT() void  Inner::TManyBase<Tag>::Destroy<TMany<Tag>>(this TMany<Tag>& self);
   template LANGULUS_EXPORT() void  Inner::TManyBase<Tag>::Free<true, TMany<Tag>>(this TMany<Tag>& self);
   template LANGULUS_EXPORT() void  Inner::TManyBase<Tag>::Free<false, TMany<Tag>>(this TMany<Tag>& self);
   template LANGULUS_EXPORT() void  Inner::TManyBase<Tag>::Keep<TMany<Tag>>(this TMany<Tag>& self);
   template LANGULUS_EXPORT() TMany<Tag>& Inner::TManyBase<Tag>::AssignAbsorb<TMany<Tag>, TMany<Tag>>(this TMany<Tag>& self, TMany<Tag>&& rhs);
   template LANGULUS_EXPORT() bool  Inner::TManyBase<Tag>::IsMissingDeep<TMany<Tag>>(this TMany<Tag> const& self) noexcept;

   template LANGULUS_EXPORT() void  Assignment<false, 0>::PrepareForReconstruction<TMany<Tag>>(this TMany<Tag>&);
   template LANGULUS_EXPORT() bool  Assignment<false, 0>::PrepareForReassignment<TMany<Tag>>(this TMany<Tag>&);

   template LANGULUS_EXPORT() bool   Comparison<false, true, 0>::CompareEqual<0, TMany<Tag>, TMany<Tag>>(this TMany<Tag> const&, TMany<Tag> const&);
   template LANGULUS_EXPORT() bool   Comparison<false, true, 0>::CompareHashes<TMany<Tag>, TMany<Tag>>(this TMany<Tag> const&, TMany<Tag> const&);
   template LANGULUS_EXPORT() size_t Comparison<false, true, 0>::Matches<TMany<Tag>, TMany<Tag>>(this TMany<Tag> const&, TMany<Tag> const&);

   template LANGULUS_EXPORT() size_t Deepening<Many>::GetCountDeep<TMany<Tag>>(this TMany<Tag> const&, size_t);
   template LANGULUS_EXPORT() size_t Deepening<Many>::GetReservedDeep<TMany<Tag>>(this TMany<Tag> const&, size_t);
   //template Many&  Deepening<Many>::Deepen<TMany<Recipe>>(this TMany<Recipe>&);

   template LANGULUS_EXPORT() void Removal<0>::Trim<TMany<Tag>>(this TMany<Tag>&, size_t) noexcept;
   template LANGULUS_EXPORT() void Removal<0>::Clear<TMany<Tag>>(this TMany<Tag>&);
   template LANGULUS_EXPORT() void Removal<0>::Reset<TMany<Tag>>(this TMany<Tag>&);
}

namespace Langulus::Annies::Component
{
   template LANGULUS_EXPORT() void  Inner::TManyBase<Many>::ConstructDefault<TMany<Many>>(this TMany<Many>&& self) noexcept;
   template LANGULUS_EXPORT() void  Inner::TManyBase<Many>::Absorb<TMany<Many>, TMany<Many>>(this TMany<Many>& self, TMany<Many>&& from);
   template LANGULUS_EXPORT() void  Inner::TManyBase<Many>::Destroy<TMany<Many>>(this TMany<Many>& self);
   template LANGULUS_EXPORT() void  Inner::TManyBase<Many>::Free<true, TMany<Many>>(this TMany<Many>& self);
   template LANGULUS_EXPORT() void  Inner::TManyBase<Many>::Free<false, TMany<Many>>(this TMany<Many>& self);
   template LANGULUS_EXPORT() void  Inner::TManyBase<Many>::Keep<TMany<Many>>(this TMany<Many>& self);
   template LANGULUS_EXPORT() TMany<Many>& Inner::TManyBase<Many>::AssignAbsorb<TMany<Many>, TMany<Many>>(this TMany<Many>& self, TMany<Many>&& rhs);
   template LANGULUS_EXPORT() bool  Inner::TManyBase<Many>::IsMissingDeep<TMany<Many>>(this TMany<Many> const& self) noexcept;

   template LANGULUS_EXPORT() void  Assignment<false, 0>::PrepareForReconstruction<TMany<Many>>(this TMany<Many>&);
   template LANGULUS_EXPORT() bool  Assignment<false, 0>::PrepareForReassignment<TMany<Many>>(this TMany<Many>&);

   template LANGULUS_EXPORT() bool   Comparison<false, true, 0>::CompareEqual<0, TMany<Many>, TMany<Many>>(this TMany<Many> const&, TMany<Many> const&);
   template LANGULUS_EXPORT() bool   Comparison<false, true, 0>::CompareHashes<TMany<Many>, TMany<Many>>(this TMany<Many> const&, TMany<Many> const&);
   template LANGULUS_EXPORT() size_t Comparison<false, true, 0>::Matches<TMany<Many>, TMany<Many>>(this TMany<Many> const&, TMany<Many> const&);

   template LANGULUS_EXPORT() size_t Deepening<Many>::GetCountDeep<TMany<Many>>(this TMany<Many> const&, size_t);
   template LANGULUS_EXPORT() size_t Deepening<Many>::GetReservedDeep<TMany<Many>>(this TMany<Many> const&, size_t);
   template LANGULUS_EXPORT() Many&  Deepening<Many>::Deepen<TMany<Many>>(this TMany<Many>&);

   template LANGULUS_EXPORT() void Removal<0>::Trim<TMany<Many>>(this TMany<Many>&, size_t) noexcept;
   template LANGULUS_EXPORT() void Removal<0>::Clear<TMany<Many>>(this TMany<Many>&);
   template LANGULUS_EXPORT() void Removal<0>::Reset<TMany<Many>>(this TMany<Many>&);
}