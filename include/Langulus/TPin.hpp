///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "TOwn.hpp"
#include "TRef.hpp"
/*#include "Annies/Container.hpp"
#include "Annies/Components/Typed-Static.hpp"
#include "Annies/Components/Stack.hpp"
#include "Annies/Components/Count-Static.hpp"
#include "Annies/Components/Deepening.hpp"
#include "Annies/Components/Emplacement.hpp"
#include "Annies/Components/Assignment.hpp"
#include "Annies/Components/Comparison.hpp"*/
#include "Annies/States/Pinned.hpp"
#include <Langulus/CT/Pinnable.hpp>


namespace Langulus::Annies::Inner
{
   template<CT::NotVoid T>
   using TPinBase = Tif<CT::Dense<T>, TOwn<T>, TRef<T>>;
}

namespace Langulus::Annies
{
   ///                                                                        
   /// A statically typed stack-based container of size 1.                    
   /// Mainly serves to transfer values and/or pointers on move.              
   /// Allows for pinning, which disables overwrite on assignment.            
   /// Especially useful for members that are part of a hierarchy and can     
   /// be overridden by things from upper in the hierarchy.                   
   /// You can optionally add tags to it.                                     
   template<CT::NotVoid T, class...TAGS>
   struct TPin : Inner::TPinBase<T> {
      using CTTI_ReflectAs = TPin;
      using CTTI_POD       = Maybe<CT::POD<T> and CT::Dense<T>>;
      using CTTI_Tagged    = Types<TAGS...>;
      using CTTI_Pinnable  = Yup;
      using Base           = Inner::TPinBase<T>;

   private:
      bool pinned = false;

   public:
      constexpr bool IsPinned() const noexcept { return pinned; }

      TPin& Pin() noexcept {
         pinned = true;
         return *this;
      }

      TPin& Unpin() noexcept {
         pinned = false;
         return *this;
      }

      using Inner::TPinBase<T>::TPinBase;
      using Base::operator ==;
      using Base::operator <=>;

      constexpr TPin& operator = (auto&& other) {
         if (pinned)
            return *this;
         Base::operator = (LglsFwd(other));
         return *this;
      }
   };
}