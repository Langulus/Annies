///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Config.hpp"


namespace Langulus
{
   namespace A
   {

      ///                                                                     
      ///   An abstract iterator base                                         
      ///                                                                     
      ///   Anything derived from it is non-insertable lightweight helper     
      /// structure, use for iterating containers - has no ownership, nor     
      /// safety features, mainly used silently in ranged-for loops           
      ///                                                                     
      struct Iterator {
         using CTTI_ReflectAs    = void;
         using CTTI_Allocatable  = No;
         using CTTI_Abstract     = Yup;
      };

      ///                                                                     
      ///   A weightless 'end' iterator helper type                           
      ///                                                                     
      ///   Used to return from container's end() methods. It only compares   
      /// equal to other iterators, if they've reached their end marker       
      ///                                                                     
      struct IteratorEnd : Iterator {};

   } // namespace Langulus::A

   namespace CT
   {

      /// Check if a type is an iterator                                      
      template<class...T>
      concept Iterator = (DerivedFrom<T, A::Iterator> and ...);

   } // namespace Langulus::CT

} // namespace Langulus
