///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include <Langulus/Text.hpp>
#include <Langulus/Annies/Functional/Compare.hpp>

namespace Langulus::Annies::Component
{
   template struct LANGULUS_API(ANNIES) Container<
         Com::State::Disowned<>,          // Allows disownment          
         Com::State::Compressed<>,        // Toggle compression         
         Com::State::Encrypted<>,         // Toggle encryption          
         Com::TypedStatic<DMeta, char>,   // Type-constrained           
         Com::HeapMovable<0, 0, HeapEntry<0, char*>>,
         Com::CountStack<>,               // Variable count             
         Com::ReserveEmergent<>,          // Capacity derived from alloc
         Com::IndexedLinear<>,            // Indexed directly           
         Com::OwnershipStack<>,           // Allocation is referenced   
         Com::HashStack<>,                // Variable hash (cached)     
         Com::Insertion<true>,            // Serialize + insert         
         Com::InsertionOperators<>,       // << and >> insertion        
         Com::InsertionOperatorsConcat<>, // + and += concat            
         Com::Merging<true>,              // Serialize + merge          
         Com::MergingOperators<>,         // <<= and >>= merging        
         Com::Removal<>,                  // Allows removal             
         Com::Assignment<true>,           // Allows assignment          
         Com::Comparison<true>,           // Allows for comparison      
         Com::Conversion<>,               // Allows conversion          
         Com::IterationForEach<>,         // ForEach iteration          
         Com::IterationRange<>            // Range iteration            
      >;
}

namespace Langulus::Annies
{
   bool Text::operator == (Text const& other) const noexcept {
      return this->CompareEqual(other);
   }
}