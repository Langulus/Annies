///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Annies/Components/Typed-Static.hpp"
#include "Annies/Components/Heap-Movable.hpp"
#include "Annies/Components/Ownership-Stack.hpp"
#include "Annies/Components/Count-Static.hpp"
#include "Annies/Components/Reserve-Emergent.hpp"
#include "Annies/Components/Emplacement.hpp"
#include "Annies/Components/Assignment.hpp"
#include "Annies/Components/Comparison.hpp"
#include "Annies/Components/Removal.hpp"


namespace Langulus::Annies::Inner
{
   template<class T> requires (CT::NotHandle<T> and CT::NotReference<T>)
   using TRefBase = Com::Container<
      Com::TypedStatic<DMeta, T>,         // Statically typed          
      Com::HeapMovable<0, 0, HeapEntry<0, T*>>,
      Com::CountStatic<1u>,               // Statically sized          
      Com::ReserveEmergent<>,             // Reserve derived from alloc
      Com::OwnershipStack<>,              // Allocation is referenced  
      EnableComponentIf<CT::Sparse<T>, Com::OwnershipDeepHeap<>>,
      Com::Emplacement<>,                 // Can be emplaced           
      Com::Assignment<>,                  // Can be reassigned         
      Com::Comparison<>,                  // Can be compared           
      Com::Removal<>                      // Can be cleared/reset      
   >;
}

namespace Langulus::Annies
{
   ///                                                                        
   ///   A statically typed shared pointer                                    
   ///                                                                        
   /// Works fine with packed pointers as well. Has deep ownership, but no    
   /// states are applied. You can use TAny instead if you want states.       
   /// This container is similar in functionality to ::std::shared_ptr.       
   template<class T>
   struct TRef : Inner::TRefBase<T> {
      using CTTI_Deep     = Yup;
      using Base          = Inner::TRefBase<T>;
      using Pick          = ConstAll<T>;
      using PickMut       = T;
      
      #if not LANGULUS(FORCE_TYPE_ERASURE)
         using HandleType    = THandle<ConstAll<T> const&>;
         using HandleMutType = THandle<T&>;
      #else
         using HandleType    = Handle;
         using HandleMutType = HandleMut;
      #endif

      constexpr TRef() noexcept {
         this->ConstructDefault();
      }

      constexpr TRef(nullptr_t) noexcept
         : TRef {} {}

      constexpr TRef(TRef const& other)
         : Base {Absorb, Refer {other}} {}

      constexpr TRef(TRef&& other) noexcept
         : Base {Absorb, Move {other}} {}

      constexpr ~TRef() noexcept {
         this->Destroy();
      }

      /// Initialize with a pointer. Respects intents.                        
      template<class A>
      constexpr TRef(A&& pointer) noexcept {
         if constexpr (CT::ContainsOne<A>)
            this->Absorb(FWDIntent(pointer));
         else if constexpr (CT::Sparse<A>) {
            if (DeintCast(pointer)) {
               this->SetHeapInner(DeintCast(pointer));
               if constexpr (not CT::Disowned<A>)
                  this->FindAllocationInner();
            }
            else this->ConstructDefault();
         }
         else static_assert(false, "A must be a pointer (intent is optional)");
      }
      
      /// Assignment                                                          
      constexpr TRef& operator = (TRef const& other) {
         return this->AssignAbsorb(Refer(other));
      }
      constexpr TRef& operator = (TRef&& other) noexcept {
         return this->AssignAbsorb(Move(other));
      }

      /// Assign a pointer. Respects intents.                                 
      template<class A>
      constexpr TRef& operator = (A&& pointer) noexcept {
         if constexpr (CT::ContainsOne<A>)
            return this->AssignAbsorb(FWDIntent(pointer));
         else {
            static_assert(CT::Sparse<A>,
               "A must be a pointer (intent is optional)");

            if (DeintCast(pointer) == this->GetHeapInner())
               return *this;
         
            if (DeintCast(pointer)) {
               this->Destroy();
               this->SetHeapInner(DeintCast(pointer));
               if constexpr (not CT::Disowned<A>)
                  this->FindAllocationInner();
            }
         }
         return *this;
      }

      /// Three-way comparison                                                
      constexpr auto operator <=> (const TRef& rhs) const noexcept {
         return Base::GetHeapInner() <=> rhs.GetHeapInner();
      }

      friend constexpr auto operator <=> (const TRef& lhs, const T* rhs) noexcept {
         if (rhs == nullptr) {
            return lhs.IsEmpty() ? ::std::strong_ordering::equal
                                 : ::std::strong_ordering::greater;
         }
         return lhs.GetRaw() <=> rhs;
      }

      friend constexpr auto operator <=> (const T* lhs, const TRef& rhs) noexcept {
         if (lhs == nullptr) {
            return rhs.IsEmpty() ? ::std::strong_ordering::equal
                                 : ::std::strong_ordering::greater;
         }
         return lhs <=> rhs.GetRaw();
      }

      friend constexpr auto operator <=> (const TRef& lhs, nullptr_t) noexcept {
         return lhs.IsEmpty() ? ::std::strong_ordering::equal
                              : ::std::strong_ordering::greater;
      }

      friend constexpr auto operator <=> (nullptr_t, const TRef& rhs) noexcept {
         return rhs.IsEmpty() ? ::std::strong_ordering::equal
                              : ::std::strong_ordering::greater;
      }

      /// Equality comparison                                                 
      constexpr bool operator == (const TRef& rhs) const noexcept {
         return Base::GetHeapInner() == rhs.GetHeapInner();
      }

      friend constexpr bool operator == (const TRef& lhs, const T* rhs) noexcept {
         if (rhs == nullptr)
            return lhs.IsEmpty();
         return lhs.GetRaw() == rhs;
      }

      friend constexpr bool operator == (const T* lhs, const TRef& rhs) noexcept {
         if (lhs == nullptr)
            return rhs.IsEmpty();
         return lhs == rhs.GetRaw();
      }

      friend constexpr bool operator == (const TRef& lhs, nullptr_t) noexcept {
         return lhs.IsEmpty();
      }

      friend constexpr bool operator == (nullptr_t, const TRef& rhs) noexcept {
         return rhs.IsEmpty();
      }
   };

   template<class T>
   using Ref = TRef<T>;
}
