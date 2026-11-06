///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include <Langulus/Recipe.hpp>
#include <Langulus/Annies/Functional/Compare.hpp>


namespace Langulus::Annies::Component
{
   template void  Inner::RecipeBase::ConstructDefault<Recipe>(this Recipe&& self) noexcept;
   template void  Inner::RecipeBase::Absorb<Recipe, Recipe>(this Recipe& self, Recipe&& from);
   template void  Inner::RecipeBase::Destroy<Recipe>(this Recipe& self);
   template void  Inner::RecipeBase::Free<true, Recipe>(this Recipe& self);
   template void  Inner::RecipeBase::Free<false, Recipe>(this Recipe& self);
   template void  Inner::RecipeBase::Keep<Recipe>(this Recipe& self);
   template Recipe& Inner::RecipeBase::AssignAbsorb<Recipe, Recipe>(this Recipe& self, Recipe&& rhs);
   template bool  Inner::RecipeBase::IsMissingDeep<Recipe>(this Recipe const& self) noexcept;
}

namespace Langulus::Annies
{
   bool Recipe::operator == (Recipe const& other) const noexcept {
      return this->CompareEqual(other);
   }
}