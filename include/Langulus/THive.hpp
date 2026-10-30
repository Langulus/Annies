///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "TMany.hpp"
#include "TSlots.hpp"
#include "Annies/Components/IndexedComposite.hpp"

//#include "Annies/Iterators/DefaultHive.hpp"


namespace Langulus::Annies::Inner
{
   /// MARK: HiveBase                                                         
   template<CT::NotVoid T>
   using HiveBase = Com::Container<
      Com::StateStack<>,                     // No states               
      Com::Stack<TMany<TSlots<T>>>,
      Com::IndexedComposite<1>,              // Composite indexing      
      Com::HashEmergent<>,                   // Emergent hash           
      Com::Assignment<>,                     // Assignment              
      Com::InsertionOperators<>,             // Insertion operators     
      Com::MergingOperators<>,               // Merge operators         
      Com::Comparison<>,                     // Allows comparisons      
      Com::IterationForEach<>,               // ForEach iteration       
      Com::IterationRange<>                  // Ranged iteration        
   >;
}

namespace Langulus::Annies
{
   ///                                                                        
   /// A statically-typed non-continuous unordered container of variable size 
   /// that guarantees elements will _never_ move from the memory they were   
   /// first instantiated in.                                                 
   template<CT::NotVoid T>
   struct THive : Inner::HiveBase<T> {
      using CTTI_ReflectAs = THive;

      //using DeepType       = Many;
      using HandleType     = DecideHandle<const TSlots<T>>;
      using HandleMutType  = DecideHandle<      TSlots<T>>;
      using Pick           = DecidePick  <const TSlots<T>>;
      using PickMut        = DecidePick  <      TSlots<T>>;

      static constexpr bool ContainsMany = true;

      ///                                                                     
      /// MARK:Construction                                                   
      constexpr THive() noexcept {
         this->ConstructDefault();
      }
      constexpr THive(THive const& other) {
         this->Absorb(Refer(other));
      }
      constexpr THive(THive&& other) noexcept  {
         this->Absorb(Move(other));
      }
      constexpr ~THive() noexcept {
         this->Destroy();
      }

      ///                                                                     
      /// MARK:Assignment                                                     
      constexpr THive& operator = (THive const& other) {
         return this->AssignAbsorb(Refer(other));
      }
      constexpr THive& operator = (THive&& other) noexcept {
         return this->AssignAbsorb(Move(other));
      }
      
      template<class A>
      constexpr THive& operator = (A&& argument) {
         if constexpr (Same<Deint<A>, THive>)
            return this->AssignAbsorb(LglsFwd(argument));
         else
            return this->Assign(LglsFwd(argument));
      }

      using Com::Comparison<>::operator <=>;
      using Com::Comparison<>::operator ==;
   };
}

namespace Langulus
{
   using Annies::THive;
}