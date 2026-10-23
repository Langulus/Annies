///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Table.hpp"


namespace Langulus::Annies::Inner
{
   template<CT::NotVoid T>
   requires (CT::NotHandle<T> and CT::NotReference<T>)
   using TTableBase = Com::Container<
      Com::State::Disowned<>,          // Allows disownment             
      Com::State::Compressed<>,        // Toggle compression            
      Com::State::Encrypted<>,         // Toggle encryption             
      Com::TypedStack<DMeta, T>,       // Type-constrained              
      Com::HeapMovable<8, 2, HeapEntry<0, T*>>,
      Com::CountStack<>,               // Dynamically sized             
      Com::ReserveStack<>,             // Reserve kept as member        
      Com::IndexedHashStack<>,         // Indexed by hash table         
      Com::OwnershipStack<>,           // Allocation is referenced      
      EnableComponentIf<CT::Sparse<T>, Com::OwnershipDeepHeap<>>,
      Com::HashStack<>,                // Hash can be cached            
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
   ///                                                                        
   /// A statically-typed non-contiguous table of variable size that is       
   /// binary-compatible with the type-erased alternative `Table`.            
   /// Think of it as a TSet<T> that allows duplicates.                       
   template<CT::NotVoid T>
   struct TTable : Inner::TTableBase<T> {
      using CTTI_ReflectAs = Table;
      using CTTI_Table     = Yup;
      using CTTI_Deep      = Yup;
      using CTTI_POD       = No; //TODO could be, when we implement SVO

      using Base           = Inner::TTableBase<T>;
      using DeepType       = Many;//Any;

      constexpr TTable() noexcept {
         this->ConstructDefault();
      }
      constexpr TTable(TTable const& other) {
         this->Absorb(Refer(other));
      }
      constexpr TTable(TTable&& other) noexcept {
         this->Absorb(Move(other));
      }
      constexpr ~TTable() noexcept {
         this->Destroy();
      }
      
      /// Construction that either absorbs the provided containers, or        
      /// emplaces all A in the container                                     
      template<Disambiguate A1, class...AN>
      constexpr TTable(A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0) {
            if constexpr (CT::Table<A1>) {
               LglsAssumeUser(CT::NotTable<T>,
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
            }
         }
         else {
            this->ConstructDefault();
            this->Merge(LglsFwd(a1));
           (this->Merge(LglsFwd(an)), ...);
         }
      }
      
      /// Construction that absorbs the provided container                    
      template<class A1, class...AN>
      constexpr TTable(Inner::Absorb, A1&& a1, AN&&...an) {
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
      constexpr TTable(Inner::Piecewise, A1&& a1, AN&&...an) {
         this->ConstructDefault();
         this->Merge(LglsFwd(a1));
        (this->Merge(LglsFwd(an)), ...);
      }

      /// Slice a container that is representable by a table                  
      template<Cid SID>
      constexpr TTable(Inner::Slice<SID>, CT::NotContiguous auto&& other) noexcept {
         this->template SliceFrom<SID>(Disown(other));
      }

      /// Assignment                                                          
      constexpr TTable& operator = (TTable const& other) {
         return this->AssignAbsorb(Refer(other));
      }
      constexpr TTable& operator = (TTable&& other) noexcept {
         return this->AssignAbsorb(Move(other));
      }

      template<class A>
      constexpr TTable& operator = (A&& argument) {
         if constexpr (CT::Table<A>) {
            LglsAssumeUser(CT::NotTable<T>,
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
            static_assert(Same<T, TypeOf<A>>, "Type mismatch");
            return TAny {Annies::Absorb, Copy {LglsFwd(argument)}};
         }
         else {
            static_assert(Same<T, Deint<A>>, "Type mismatch");
            return TAny {Annies::Piecewise, LglsFwd(argument)};
         }
      }
      
      using Com::Comparison<>::operator <=>;
      using Com::Comparison<>::operator ==;
      using Com::IterationRange<>::begin;
      using Com::IterationRange<>::end;
      using Com::IterationRange<>::rbegin;
      using Com::IterationRange<>::rend;
   };
}

namespace Langulus
{
   using Annies::TTable;

   template<CT::Map MAP> requires (not CT::TypeErased<MAP>)
   auto InValuesOf(MAP& some_map) {
      return TTable<TypeOf<MAP, 1>> {Annies::Slice<1>, some_map};
   }
}