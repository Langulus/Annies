///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Handle.hpp"
#include "Annies/Components/Heap-Immovable.hpp"
#include "Annies/Components/Ownership-Stack.hpp"
#include "Annies/Components/OwnershipDeep-Heap.hpp"
#include "Annies/Components/Emplacement.hpp"
#include "Annies/Components/Removal.hpp"
#include "Annies/Components/Typed-Static.hpp"
#include "Annies/Components/Count-Stack.hpp"
#include "Annies/Components/Reserve-Stack.hpp"


namespace Langulus::Annies
{
   ///                                                                        
   /// A statically-typed non-continuous container of variable size that      
   /// guarantees elements will never move from the memory they were first    
   /// instantiated in                                                        
   ///                                                                        
   template<CT::NotVoid T>
   struct THive : Container<
      Com::HeapImmovable<>,            // Immovable heap memory         
      Com::OwnershipStack<>,           // Allocation is referenced      
      Com::DeepOwnershipHeap<>,        // Referenced indirections       
      Com::Emplacement<>,              // Allows emplacement            
      Com::Removal<>,                  // Allows removal                
      Com::TypedStatic<DMeta, T>,      // Statically typed              
      Com::CountStack<>,               // Variable count                
      Com::ReserveHeap<>               // Variable capacity             
   > {
      using PickDenseMut  = T&;
      using PickDense     = T const&;
      
      #if not LANGULUS(FORCE_TYPE_ERASURE)
         using HandleTypeMut = THandle<T&>;
         using HandleType    = THandle<T const&>;
      #else
         using HandleTypeMut = HandleMut;
         using HandleType    = Handle;
      #endif

      using Pick          = Tif<CT::Sparse<T>, PickSparse,    PickDense>;
      using PickMut       = Tif<CT::Sparse<T>, PickSparseMut, PickDenseMut>;
   };
}

namespace Langulus
{
   using Annies::THive;
}