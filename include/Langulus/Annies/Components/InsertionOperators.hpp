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
   /// May convert the argument (if AS is specified in Com::Insertion).       
   /// May deepen the container in order to insert, if able to.               
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
         lhs.Insert(LglsFwd(rhs));
         return lhs;
      }

      /// Push front                                                          
      template<CT::ContainsMany C, class A>
      C& operator >> (this C& lhs, A&& rhs) {
         lhs.InsertAt(Index::Front, LglsFwd(rhs));
         return lhs;
      }
   };
}
