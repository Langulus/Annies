///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Handle.hpp"
#include "Annies/Components/Typed-Static.hpp"
#include "Annies/Components/Count-Stack.hpp"
#include "Annies/Components/Reserve-Emergent.hpp"
#include "Annies/Components/Heap-Immovable.hpp"
#include "Annies/Components/Ownership-Stack.hpp"
#include "Annies/Components/OwnershipDeep-Heap.hpp"
#include "Annies/Components/Emplacement.hpp"
#include "Annies/Components/Removal.hpp"


namespace Langulus::Annies
{
   ///                                                                        
   /// A statically-typed non-continuous container of variable size that      
   /// guarantees elements will never move from the memory they were first    
   /// instantiated in                                                        
   ///                                                                        
   template<CT::NotVoid T>
   struct THive : Com::Container<
      Com::TypedStatic<DMeta, T>,      // Statically typed              
      Com::CountStack<>,               // Variable count                
      Com::ReserveEmergent<>,          // Variable capacity             
      Com::HeapImmovable<>,            // Immovable heap memory         
      Com::OwnershipStack<>,           // Allocation is referenced      
      Com::OwnershipDeepHeap<>,        // Referenced indirections       
      Com::Emplacement<>,              // Allows emplacement            
      Com::Removal<>                   // Allows removal                
   > {
      using CTTI_ReflectAs = THive;
   };
}

namespace Langulus
{
   using Annies::THive;
}