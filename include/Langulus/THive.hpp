///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "TMany.hpp"
#include "TSlots.hpp"
#include "Annies/Iterators/DefaultHive.hpp"


namespace Langulus::Annies
{
   ///                                                                        
   /// A statically-typed non-continuous unordered container of variable size 
   /// that guarantees elements will _never_ move from the memory they were   
   /// first instantiated in.                                                 
   template<CT::NotVoid T>
   struct THive {
   protected:
      TMany<TSlots<T>> data;

   public:
      using CTTI_ReflectAs = THive;
      using CTTI_Typed     = T;

      /// Return an iterator to the first element                             
      template<CT::Container C>
      constexpr auto begin(this C&& self) noexcept {
         return IterateHiveDefault(self).begin();
      }

      /// Return the end sentinel                                             
      template<CT::Container C>
      constexpr auto end(this C&& self) noexcept {
         return IterateHiveDefault(self).end();
      }

      /// Return an iterator to the first element, reversed                   
      template<CT::Container C>
      constexpr auto rbegin(this C&& self) noexcept {
         return IterateHiveDefault<true, Deref<C>>(self).begin();
      }

      /// Return the end sentinel                                             
      template<CT::Container C>
      constexpr auto rend(this C&& self) noexcept {
         return IterateHiveDefault<true, Deref<C>>(self).end();
      }
   };
}

namespace Langulus
{
   using Annies::THive;
}