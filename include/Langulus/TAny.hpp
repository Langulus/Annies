///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Any.hpp"


namespace Langulus::Annies::Inner
{
   template<CT::NotVoid T> requires (CT::NotHandle<T> and CT::NotReference<T>)
   using TAnyBase = Com::Container<
      Com::State::Disowned<>,          // Allows disownment             
      Com::TypedStack<DMeta, T>,       // Type-constrained              
      Com::HeapMovable<0, 0, HeapEntry<0, T*>>,
      Com::CountStatic<1u>,            // Statically sized to 1         
      Com::ReserveEmergent<>,          // Reserve derived from alloc    
      Com::OwnershipStack<>,           // Allocation is referenced      
      EnableComponentIf<CT::Sparse<T>, Com::OwnershipDeepHeap<>>,
      Com::HashEmergent<>,             // Hash is retrieved from item   
      Com::Emplacement<>,              // Allows emplacement            
      Com::Assignment<>,               // Allows assignment             
      Com::Removal<>,                  // Allows clear/reset            
      Com::Conversion<>,               // Allows conversion             
      Com::Comparison<>,               // Allows comparisons            
      Com::State::Future<>,            // Allows future linking         
      Com::State::Past<>               // Allows past linking           
   >;
}

namespace Langulus::Annies
{
   /// MARK: TAny                                                             
   ///                                                                        
   /// A statically-typed container of size 1 that is binary-compatible with  
   /// the type-erased alternative `Any`.                                     
   template<CT::NotVoid T> 
   struct TAny : Inner::TAnyBase<T> {
      using CTTI_ReflectAs = Any;
      using CTTI_Deep      = Yup;
      using Base           = Inner::TAnyBase<T>;
      using DeepType       = Any;

      constexpr TAny() noexcept {
         this->ConstructDefault();
      }
      constexpr TAny(TAny const& other) {
         this->Absorb(Refer(other));
      }
      constexpr TAny(TAny&& other) noexcept {
         this->Absorb(Move(other));
      }
      constexpr ~TAny() noexcept {
         this->Destroy();
      }

      /// Construction that either absorbs the provided container, or         
      /// emplaces T in the container, using A... as constructor arguments    
      template<class...A>
      constexpr TAny(A&&...arguments) {
         if constexpr (sizeof...(A) == 1 and CT::ContainsOne<A...>) {
            LglsAssumeUser(
               ((Same<Deint<A>, TAny> or Same<TypeOf<Deint<A>>, T>) and ...),
               "Ambiguous use of construction "
               "- you should use tag-dispatch with first argument either Absorb "
               "(if you want to overwrite the container itself) or Piecewise "
               "(if you want to overwrite the first item) in order to clearly "
               "state your intent. Absorb will be used by default!"
            );
            this->Absorb(LglsFwd(arguments)...);
         }
         else this->EmplaceConstruct(LglsFwd(arguments)...);
      }
      
      /// Construction that absorbs the provided container                    
      template<class A>
      constexpr TAny(Inner::Absorb, A&& argument) {
         this->Absorb(LglsFwd(argument));
      }
      
      /// Emplaces T inside, using A... as constructor arguments              
      template<class...A>
      constexpr TAny(Inner::Piecewise, A&&...arguments) {
         this->EmplaceConstruct(LglsFwd(arguments)...);
      }

      /// Assignment                                                          
      constexpr TAny& operator = (TAny const& other) {
         return this->AssignAbsorb(Refer(other));
      }
      constexpr TAny& operator = (TAny&& other) noexcept {
         return this->AssignAbsorb(Move(other));
      }

      template<class A>
      constexpr TAny& operator = (A&& argument) {
         if constexpr (CT::ContainsOne<A>) {
            LglsAssumeUser(
               (Same<Deint<A>, TAny> or Same<TypeOf<Deint<A>>, T>),
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
      
      using Com::Comparison<>::operator <=>;
      using Com::Comparison<>::operator ==;
   };

   /// MARK: CTAD                                                             
   template<CT::NotVoid T>
   TAny(T&&) -> TAny<Decvq<Deref<Deint<T>>>>;

   template<CT::NotVoid T>
   TAny(Inner::Absorb, T&&) -> TAny<TypeOf<T>>;

   template<CT::NotVoid T>
   TAny(Inner::Piecewise, T&&) -> TAny<Decvq<Deref<Deint<T>>>>;
}

namespace Langulus
{
   using Annies::TAny;
}