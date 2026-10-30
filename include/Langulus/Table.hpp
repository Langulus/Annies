///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Handle.hpp"
#include "Annies/Components/Typed-Stack.hpp"
#include "Annies/Components/Heap-Movable.hpp"
#include "Annies/Components/Count-Stack.hpp"
#include "Annies/Components/Reserve-Stack.hpp"
#include "Annies/Components/Ownership-Stack.hpp"
#include "Annies/Components/OwnershipDeep-Heap.hpp"
#include "Annies/Components/Hash-Stack.hpp"
#include "Annies/Components/IndexedHash-Stack.hpp"
#include "Annies/Components/Deepening.hpp"
#include "Annies/Components/Merging.hpp"
#include "Annies/Components/MergingOperators.hpp"
#include "Annies/Components/Assignment.hpp"
#include "Annies/Components/Removal.hpp"
#include "Annies/Components/Conversion.hpp"
#include "Annies/Components/Comparison.hpp"
#include "Annies/Components/Iteration-ForEach.hpp"
#include "Annies/Components/Iteration-Range.hpp"
#include "Annies/Components/State-Stack.hpp"
#include "Annies/States/Typed.hpp"
#include "Annies/States/Compressed.hpp"
#include "Annies/States/Encrypted.hpp"
#include "Annies/States/Tracked.hpp"
#include "Annies/States/Disowned.hpp"


namespace Langulus::Annies::Inner
{
   /// MARK: TableBase                                                        
   using TableBase = Com::Container<
      Com::State::Disowned<>,          // Allows disownment             
      Com::State::Compressed<>,        // Toggle compression            
      Com::State::Encrypted<>,         // Toggle encryption             
      Com::TypedStack<DMeta>,          // Type-erased                   
      Com::HeapMovable<8, 2>,          // Pointer to heap memory        
      Com::CountStack<>,               // Dynamically sized             
      Com::ReserveStack<>,             // Reserve kept as member        
      Com::IndexedHashStack<>,         // Indexed by hash table         
      Com::OwnershipStack<>,           // Allocation is referenced      
      Com::OwnershipDeepHeap<>,        // Sparse elements are referenced
      Com::HashStack<>,                // Hash can be cached            
      Com::Deepening<Many>,            // Makes container deep          
      Com::Merging<>,                  // Allows merging                //TODO emplacement and insertion? tables allow duplicates, right?
      Com::MergingOperators<>,         // <<= and >>= merging           
      Com::Assignment<>,               // Allows assignment             
      Com::Removal<>,                  // Allows clear/reset            
      Com::Conversion<>,               // Allows conversions            
      Com::Comparison<>,               // Allows comparisons            
      Com::IterationForEach<>,         // ForEach iteration             
      Com::IterationRange<>            // Ranged iteration              
   >;
}

namespace Langulus::Annies
{
   /// MARK: Set                                                              
   ///                                                                        
   ///   A universal type-erased non-contiguous table of variable size.       
   /// Think of it as a hash set that allows duplicate items.                 
   struct Table : Inner::TableBase {
      using CTTI_ReflectAs = Table;
      using CTTI_Table     = Yup;
      using Base           = Inner::TableBase;

      constexpr Table() noexcept {
         this->ConstructDefault();
      }
      constexpr Table(Table const& other) {
         this->Absorb(Refer(other));
      }
      constexpr Table(Table&& other) noexcept  {
         this->Absorb(Move(other));
      }
      constexpr ~Table() noexcept {
         this->Destroy();
      }

      /// Construction that either absorbs the provided containers, or        
      /// emplaces all A in the container                                     
      template<Disambiguate A1, class...AN>
      constexpr Table(A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0 and CT::Table<A1>) {
            LglsAssumeUser(Same<Deint<A1>, Table>,
               "Ambiguous use of construction "
               "- you should use tag-dispatch with first argument either Absorb "
               "(if you want to overwrite the container itself) or Piecewise "
               "(if you want to overwrite the first item) in order to clearly "
               "state your intent. Absorb will be used by default!"
            );
            this->Absorb(LglsFwd(a1));
         }
         else {
            this->ConstructDefault();
            this->Merge(LglsFwd(a1));
           (this->Merge(LglsFwd(an)), ...);
         }
      }
      
      /// Construction that absorbs the provided containers                   
      template<class A1, class...AN>
      constexpr Table(Inner::Absorb, A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0)
            this->Absorb(LglsFwd(a1));
         else {
            this->ConstructDefault();
            this->MergeRange(LglsFwd(a1));
           (this->MergeRange(LglsFwd(an)), ...);
         }
      }
      
      /// Construction that emplaces all arguments inside                     
      template<class A1, class...AN>
      constexpr Table(Inner::Piecewise, A1&& a1, AN&&...an) {
         this->ConstructDefault();
         this->Merge(LglsFwd(a1));
        (this->Merge(LglsFwd(an)), ...);
      }

      /// Slice a container that is representable by a table                  
      template<Cid SID>
      constexpr Table(Inner::Slice<SID>, CT::NotContiguous auto&& other) noexcept {
         this->template SliceFrom<SID>(Disown(other));
      }

      /// Assignment                                                          
      constexpr Table& operator = (Table const& other) {
         return this->AssignAbsorb(Refer(other));
      }
      constexpr Table& operator = (Table&& other) noexcept {
         return this->AssignAbsorb(Move(other));
      }
      
      template<class A>
      constexpr Table& operator = (A&& argument) {
         if constexpr (CT::Table<A>) {
            LglsAssumeUser(Same<Deint<A>, Table>,
               "Ambiguous use of assignment "
               "- you should use either AssignAbsorb (if you want to overwrite "
               "the container itself) or Assign (if you want to overwrite the "
               "first item) in order to clearly state your intent. "
               "AssignAbsorb will be used by default!"
            );
            return this->AssignAbsorb(LglsFwd(argument));
         }
         else return this->Assign(LglsFwd(argument));
      }

      /// Create a temporary swapper with compatible elements and initialize  
      /// it with a compatible value, by shallow copying it.                  
      template<class A>
      constexpr auto CreateSwapper(A&& argument) assumptious {
         if constexpr (CT::Handle<A>) {
            LglsAssumeDev(this->template IsSame<TypeOf<A>>(), "Type mismatch");
            return TAny {Annies::Absorb, Copy {LglsFwd(argument)}};
         }
         else {
            LglsAssumeDev(this->template IsSame<Deint<A>>(), "Type mismatch");
            return TAny {Annies::Piecewise, LglsFwd(argument)};
         }
      }
   };
}

namespace Langulus
{
   using Annies::Table;
}