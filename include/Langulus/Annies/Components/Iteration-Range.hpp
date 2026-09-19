///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Component.hpp"
#include "../Iterators/Reverse.hpp"
#include "../Iterators/NoDeref.hpp"
#include "../Iterators/Default.hpp"
#include "../Iterators/Handles.hpp"
#include "../Iterators/Together.hpp"


namespace Langulus::Annies::Component
{
   ///                                                                        
   /// Implements ranged iteration interface for containers                   
   ///   @tparam ID the provider we're iterating                              
   ///   @tparam SHARED additional providers to simultaneously iterate        
   template<Cid ID, Cid...SHARED>
   struct IterationRange {
      using CTTI_Component = Yup;
      using CTTI_ReflectAs = void;
      using Id = Values<ID, SHARED...>;

      static constexpr int ComponentPrecedence = 3000;

   private:
      template<CT::Container C>
      using Count = typename Deref<C>::CountType;
      template<CT::Container C>
      using Iterator = typename IterateDefault<false, Deref<C>>::Iterator;
      template<CT::Container C>
      using IteratorRev = typename IterateDefault<true, Deref<C>>::Iterator;

   public:
      /// Return an iterator to the first element                             
      template<CT::Container C>
      constexpr auto begin(this C&& self) noexcept {
         return IterateDefault(self).begin();
      }

      /// Return the end sentinel                                             
      template<CT::Container C>
      constexpr auto end(this C&& self) noexcept {
         return IterateDefault(self).end();
      }

      /// Return an iterator to the first element, reversed                   
      template<CT::Container C>
      constexpr auto rbegin(this C&& self) noexcept {
         return IterateDefault<true, Deref<C>>(self).begin();
      }

      /// Return the end sentinel                                             
      template<CT::Container C>
      constexpr auto rend(this C&& self) noexcept {
         return IterateDefault<true, Deref<C>>(self).end();
      }

      /// Return the last item                                                
      /*template<CT::Container C>
      constexpr auto last(this C&& self) noexcept -> Iterator<C> {
         const auto offset = self.IsEmpty() ? 0 : self.GetCount() - 1;

         if constexpr (CT::TypeErased<C> or (CT::Mutable<C> and Deref<C>::Sparse))
            return {self.GetHandle() + offset, self};
         else
            return {self.GetRaw() + offset, self};
      }*/
   };
}
