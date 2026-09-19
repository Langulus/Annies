///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Annies/Old/many/Construct.hpp"

//TODO constructs not implemented yet
/*#include "Handle.hpp"
#include "Annies/Components/Heap-Movable.hpp"
#include "Annies/Components/Ownership-Stack.hpp"
#include "Annies/Components/OwnershipDeep-Heap.hpp"
#include "Annies/Components/IndexedLinear.hpp"
#include "Annies/Components/Insertion.hpp"
#include "Annies/Components/InsertionOperators.hpp"
#include "Annies/Components/Emplacement.hpp"
#include "Annies/Components/Removal.hpp"
#include "Annies/Components/Assignment.hpp"
#include "Annies/Components/Typed-Stack.hpp"
#include "Annies/Components/Count-Stack.hpp"
#include "Annies/Components/Reserve-Stack.hpp"
#include "Annies/Components/Hash-Stack.hpp"
#include "Annies/Components/Descriptor.hpp"
#include "Annies/Components/Iteration-ForEach.hpp"
#include "Annies/Components/Iteration-Range.hpp"
#include "Annies/Components/Comparison.hpp"
#include "Annies/Components/Conversion.hpp"
#include "Annies/Components/State-Stack.hpp"
#include "Annies/Components/Charge-Stack.hpp"
#include "Annies/States/Typed.hpp"
#include "Annies/States/Future.hpp"
#include "Annies/States/Past.hpp"
#include "Annies/States/Compressed.hpp"
#include "Annies/States/Encrypted.hpp"
#include "Annies/States/Or.hpp"
#include "Annies/States/Tracked.hpp"
#include <Langulus/RTTI/MetaData.hpp>


namespace Langulus::Annies
{
   ///                                                                        
   ///   Construct                                                            
   ///                                                                        
   ///   Used to contain constructor arguments for any type. It is just a     
   /// type-erased Many, but also carries a charge and a type. It is often    
   /// used in Verbs::Create to provide instructions on how to instantiate a  
   /// data type.                                                             
   struct Construct : Container<
      // Some additional data                                           
      Com::TypedStack<DMeta, void, 1>, // What are we constructing?     
      Com::Charge,                     // How many, when?               
      // The rest is just a Many for the descriptor                     
      Com::HeapMovable<>,              // Pointer to heap memory        
      Com::OwnershipStack<>,           // Allocation is referenced      
      Com::DeepOwnershipHeap<>,        // Referenced indirections       
      Com::Contiguous,                 // Heap memory is continuous     
      Com::IndexedLinear<>,            // Indexed directly              
      Com::Insertion<>,                // Allows insertion              
      Com::InsertionOperators<>,       // << and >> insertion           
      Com::Emplacement<>,              // Allows emplacement            
      Com::Removal<>,                  // Allows removal                
      Com::Assignment<>,               // Allows assignment             
      Com::TypedStack<DMeta>,          // Variable type                 
      Com::CountStack<>,               // Variable count                
      Com::ReserveStack<>,             // Variable capacity             
      Com::HashStack<>,                // Variable hash (cached)        
      Com::Descriptor,                 // Descriptor interface          
      Com::IterationForEach<>,         // ForEach iteration             
      Com::IterationRange<>,           // Ranged iteration              
      Com::Comparison,                 // Allows for comparison         
      Com::Conversion,                 // Allows conversion             
      Com::StateStack<                 // Variable state                
         DefineState::Typed<>,         // Can be type-constrained       
         DefineState::Future<>,        // Adds a 'missing future' state 
         DefineState::Past<>,          // Adds a 'missing past' state   
         DefineState::Compressed<>,    // Adds 'compressed' state       
         DefineState::Encrypted<>,     // Adds 'encrypted' state        
         DefineState::Or<>,            // Adds 'or' state               
         DefineState::Tracked<>        // Adds 'tracked' state          
      >
   > {
      using Charge = Com::Charge;

      constexpr Construct() noexcept = default;
      Construct(const Construct&) noexcept;
      Construct(Construct&&) noexcept;

      Construct(DMeta);
      Construct(DMeta, auto&&, const Charge& = {});

      #if LANGULUS_FEATURE(MANAGED_REFLECTION)
         Construct(const Token&);
         Construct(const Token&, auto&&, const Charge& = {});
      #endif

      template<CT::NotVoid, CT::NotVoid A1, CT::NotVoid...AN>
      static Construct From(A1&&, AN&&...);
      template<CT::NotVoid>
      static Construct From();

      #if LANGULUS_FEATURE(MANAGED_REFLECTION)
         template<CT::NotVoid A1, CT::NotVoid...AN>
         static Construct FromToken(const Token&, A1&&, AN&&...);
         static Construct FromToken(const Token&);
      #endif

      Hash GetHash() const;
      auto GetProducer() const noexcept -> DMeta;
      void Clear();
      void Reset();
      void ResetCharge() noexcept;
   };
}
*/

namespace Langulus
{
   using Annies::Construct;
}