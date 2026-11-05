
///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include <Langulus/Neat.hpp>

namespace Langulus::Annies
{
   template struct LANGULUS_API(ANNIES) TMany<Recipe>;
   template struct LANGULUS_API(ANNIES) TMany<Many>;
   template struct LANGULUS_API(ANNIES) TMany<Tag>;

   template struct LANGULUS_API(ANNIES) TMap<DMeta, Inner::RecipeList, StateValue::Disabled>;
   template struct LANGULUS_API(ANNIES) TMap<DMeta, Inner::MessyList, StateValue::Disabled>;
   template struct LANGULUS_API(ANNIES) TMap<TMeta, Inner::TagList, StateValue::Disabled>;
}

namespace Langulus::Annies::Component
{
   template struct LANGULUS_API(ANNIES) Container<
      Com::StateStack<>,                     // No states               
      Com::Multiprovider<
         // Tags are ordered first by their tag type, then by their     
         // order of appearance. Duplicate tag types are allowed.       
         // Tag contents may or may not also be normalized.             
         Com::Stack<Inner::TagMap,    0>,
         // Subconstructs are sorted first by the recipe target, then   
         // by their order of appearance. Their contents may or may not 
         // also be normalized.                                         
         Com::Stack<Inner::RecipeMap, 1>,
         // Any other container that doesn't fit in the above is sorted 
         // first by its type, then by the order of appearance.         
         // Contents may or may not be normalized.                      
         Com::Stack<Inner::MessyMap,  2>
      >,
      Com::IndexedComposite<0, 0, 1, 2>,     // Composite indexing      
      Com::HashEmergent<Hash, 0, 1, 2>,      // Emergent hash           
      Com::Deepening<Many>,                  // Makes container deep    
      Com::Assignment<false, 0, 1, 2>,       // Assignment              
      Com::InsertionOperators<>,             // Insertion operators     
      Com::MergingOperators<>,               // Merge operators         
      /*Com::Comparison<false, true, 0, 1, 2>,*/ // Allows comparisons      
      Com::IterationForEach<0, 1, 2>,        // ForEach iteration       
      Com::IterationRange<0, 1, 2>,          // Ranged iteration        
      Com::Descriptor<0, 1, 2>               // Descriptor interface    
   >;
}