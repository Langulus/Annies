///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Insertion.hpp"


namespace Langulus::Annies::Component
{
   ///                                                                        
   /// Adds operators for front (>>) and back (<<) insertion.                 
   /// Adds an cast to contained type if argument mismatches a statically     
   /// typed container, but other than that - the behavior from Com::Insertion
   /// is used.                                                               
   ///   @tparam ID, SHARED operators that share the same insertion behavior. 
   ///   @attention this relies on Com::Insertion being present               
   template<Cid ID, Cid...SHARED>
   struct InsertionOperators {
      using CTTI_Component = Yup;
      using CTTI_ReflectAs = void;
      using Id             = Values<ID, SHARED...>;

      static constexpr int ComponentPrecedence = 3000;

      /// Push back                                                           
      template<CT::ContainsMany C, class A>
      C& operator << (this C& lhs, A&& rhs) {
         /*if constexpr (not CT::TypeErased<C> and not CT::Handle<A>
         and           not Same<Deint<A>, TypeOf<C>>)
            lhs.Insert(TypeOf<C> {LglsFwd(rhs)});
         else*/
            lhs.Insert(LglsFwd(rhs));
         return lhs;
      }

      /// Push front                                                          
      template<CT::ContainsMany C, class A>
      C& operator >> (this C& lhs, A&& rhs) {
         /*if constexpr (not CT::TypeErased<C> and not CT::Handle<A>
         and           not Same<Deint<A>, TypeOf<C>>)
            lhs.InsertAt(Index::Front, TypeOf<C> {LglsFwd(rhs)});
         else*/
            lhs.InsertAt(Index::Front, LglsFwd(rhs));
         return lhs;
      }
   };
}
