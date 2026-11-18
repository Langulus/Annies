///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Many.hpp"
#include "Annies/Components/IndexedCoverage-Stack.hpp"
#include "Annies/Components/Heap-Immovable.hpp"


namespace Langulus::Annies::Inner
{
   template<CT::NotVoid T> requires (CT::NotHandle<T> and CT::NotReference<T>)
   using THiveBase = Com::Container<
      Com::State::Compressed<>,        // Toggle compression            
      Com::State::Encrypted<>,         // Toggle encrypted              
      Com::TypedStack<DMeta, T>,       // Type-constrained              
      Com::HeapImmovable<32, 2, HeapEntry<0, T*>>,
      Com::CountStack<>,               // Dynamically sized             
      Com::ReserveStack<>,             // Reserve kept as member        
      Com::IndexedCoverageStack<>,     // Indexed by coverage mask      
      Com::OwnershipStack<>,           // Allocation is referenced      
      EnableComponentIf<CT::Sparse<T> and not ::std::is_function_v<Deptr<T>>, Com::OwnershipDeepHeap<>>,
      Com::HashStack<>,                // Hash can be cached            
      Com::Deepening<Many>,            // Makes container deep          
      /*Com::Insertion<>,                // Allows insertion              
      Com::InsertionOperators<>,       // << and >> insertion           
      Com::Merging<>,                  // Allows merging                
      Com::MergingOperators<>,         // <<= and >>= merging           
      Com::Emplacement<>,*/              // Allows emplacement            
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
   /// MARK: THive                                                            
   ///                                                                        
   ///   A statically-typed non-contiguous container of constant size, capable
   /// of reusing empty cells. Guarantees that elements _never_ move from     
   /// the memory location they were initially constructed in.                
   ///   @attention a hive can't be refer-constructed - only moved, copied or 
   ///      cloned.                                                           
   template<CT::NotVoid T> 
   struct THive : Inner::THiveBase<T> {
      using CTTI_ReflectAs = THive;
      using CTTI_Hive      = Yup;
      using CTTI_POD       = No; //TODO could be, when we implement SVO
      using Base           = Inner::THiveBase<T>;

      constexpr THive() noexcept {
         this->ConstructDefault();
      }
      constexpr THive(THive const& other) = delete; /*{ //Hives can't be referenced, only moved/copied/cloned
         this->Absorb(Refer(other));
      }*/
      constexpr THive(THive&& other) noexcept {
         this->Absorb(Move(other));
      }
      constexpr ~THive() noexcept {
         this->Destroy();
      }

      /// @attention sensitive to whether you forward && due to IntentOfT     
      template<class A1, class...AN>
      static consteval bool ConstructibleWith() {
         if constexpr      (sizeof...(AN) == 0 and CT::Copied<A1> and Same<THive, Deint<A1>>)
            return CT::ReferConstructible<T>;
         else if constexpr (sizeof...(AN) == 0 and CT::Cloned<A1> and Same<THive, Deint<A1>>)
            return CT::CloneConstructible<T>;
         else if constexpr (sizeof...(AN) == 0 and (CT::Moved<IntentOfT<A1>> or CT::Abandoned<Deref<A1>>) and Same<THive, Deint<A1>>)
            return CT::AbandonConstructible<T>;
         else
            return not Same<THive, Deint<A1>>;
      }
      
      /// Construction that either absorbs the provided containers, or        
      /// emplaces all A in the container                                     
      template<Disambiguate A1, class...AN> requires (ConstructibleWith<A1&&, AN&&...>())
      constexpr THive(A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0 and Same<Deint<A1>, THive> and not CT::BoundedArray<A1>)
            this->Absorb(LglsFwd(a1));
         else {
            this->ConstructDefault();
            this->Emplace(LglsFwd(a1));
           (this->Emplace(LglsFwd(an)), ...);
         }
      }
      
      /// Construction that absorbs the provided container                    
      template<class A1, class...AN> requires (ConstructibleWith<A1&&, AN&&...>())
      constexpr THive(Inner::Absorb, A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0 and Same<Deint<A1>, THive> and not CT::BoundedArray<A1>)
            this->Absorb(LglsFwd(a1));
         else {
            this->ConstructDefault();
            this->Concat(LglsFwd(a1), LglsFwd(an)...);
         }
      }
      
      /// Construction that emplaces all arguments inside                     
      template<class A1, class...AN>
      constexpr THive(Inner::Piecewise, A1&& a1, AN&&...an) {
         this->ConstructDefault();
         this->Emplace(LglsFwd(a1));
        (this->Emplace(LglsFwd(an)), ...);
      }

      /// Assignment                                                          
      constexpr THive& operator = (THive const& other) = delete; /*{
         return this->AssignAbsorb(Refer(other));
      }*/
      constexpr THive& operator = (THive&& other) noexcept {
         return this->AssignAbsorb(Move(other));
      }

      template<class A> requires (ConstructibleWith<A&&>())
      constexpr THive& operator = (A&& argument) {
         if constexpr (Same<Deint<A>, THive> and not CT::BoundedArray<A>)
            return this->AssignAbsorb(LglsFwd(argument));
         else
            return this->Assign(LglsFwd(argument));
      }
      
      using Com::Comparison<>::operator <=>;
      using Com::Comparison<>::operator ==;
      
      /// Emplaces an element of T at a free spot in the hive.                
      /// There's a strong guarantee that no elements will move.              
      ///   @param arguments... constructor arguments for T                   
      ///   @return a handle to the newly emplaced element                    
      auto Emplace(auto&&...arguments) -> THandle<T&> {
         if (this->GetCount() == this->GetReserved())
            this->AllocateMore(this->GetReserved() + 1);

         return this->TableEmplace(LglsFwd(arguments)...);
      }
   };

   /// MARK: CTAD                                                             
   template<CT::NotVoid T>
   THive(T&&) -> THive<Decvq<Deref<Deint<T>>>>;

   template<CT::NotVoid T>
   THive(Inner::Absorb, T&&) -> THive<TypeOf<T>>;

   template<CT::NotVoid T>
   THive(Inner::Piecewise, T&&) -> THive<Decvq<Deref<Deint<T>>>>;
}