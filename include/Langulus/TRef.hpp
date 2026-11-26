///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Annies/Components/State-Stack.hpp"
#include "Annies/Components/Typed-Static.hpp"
#include "Annies/Components/Stack.hpp"
//#include "Annies/Components/Ownership-Stack.hpp"
#include "Annies/Components/OwnershipDeep-Emergent.hpp"
#include "Annies/Components/Count-Static.hpp"
#include "Annies/Components/Reserve-Emergent.hpp"
#include "Annies/Components/Deepening.hpp"
#include "Annies/Components/Emplacement.hpp"
#include "Annies/Components/Assignment.hpp"
#include "Annies/Components/Comparison.hpp"
#include "Annies/Components/Removal.hpp"


namespace Langulus::Annies::Inner
{
   template<class T> requires (not ::std::is_reference_v<T>)
   using TRefBase = Com::Container<
      Com::StateStack<>,                  // Force no states            
      Com::TypedStatic<DMeta, T>,         // Statically typed           
      Com::Stack<T>,
      Com::CountStatic<1u>,               // Statically sized           
      Com::ReserveEmergent<>,             // Reserve derived from alloc 
      Com::OwnershipDeepEmergent<>,       // Individual references      //TODO temporary
      Com::Deepening<Many>,               // Makes container deep       
      Com::Emplacement<>,                 // Can be emplaced            
      Com::Assignment<>,                  // Can be reassigned          
      Com::Comparison<false, false>,      // Can be compared            
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
   /// You can optionally add tags to it.                                     
   template<class T, class...TAGS>
   struct TRef : Inner::TRefBase<T> {
      using CTTI_ReflectAs = TRef;
      using CTTI_Tagged    = Types<TAGS...>;
      using Base           = Inner::TRefBase<T>;
      using Pick           = ConstAll<T>;
      using PickMut        = T;
      
      //#if not LANGULUS(FORCE_TYPE_ERASURE)
         using HandleType    = THandleEmergent<ConstAll<T> const&>;
         using HandleMutType = THandleEmergent<T&>;
      /*#else
         using HandleType    = HandleEmergent;
         using HandleMutType = HandleMut;
      #endif*/

      constexpr TRef() noexcept {
         this->ConstructDefault();
      }

      constexpr TRef(nullptr_t) noexcept : TRef {} {}

      constexpr TRef(TRef const& other) {
         this->Absorb(Refer(other));
      }
      constexpr TRef(TRef&& other) noexcept {
         this->Absorb(Move(other));
      }
      constexpr ~TRef() noexcept {
         this->Destroy();
      }

      /// Initialize with a pointer. Respects intents.                        
      template<Disambiguate A>
      constexpr TRef(A&& pointer) noexcept {
         if constexpr (CT::ContainsOne<A>)
            this->Absorb(FWDIntent(pointer));
         else {
            static_assert(CT::Sparse<A>,
               "A must be a pointer (intent is optional)");
            static_assert(IndirectsOf<A> == IndirectsOf<T>,
               "Number of indirections must match");

            if (DeintCast(pointer))
               this->EmplaceConstruct(LglsFwd(pointer));
            else
               this->ConstructDefault();
         }
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
            return this->AssignAbsorb(LglsFwd(pointer));
         else if constexpr (::std::is_null_pointer_v<Deint<A>>) {
            this->Clear();
            return *this;
         }
         else {
            static_assert(CT::Sparse<A>,
               "A must be a pointer (intent is optional)");
            static_assert(IndirectsOf<A> == IndirectsOf<T>,
               "Number of indirections must match");
            
            if (DeintCast(pointer))
               this->Assign(LglsFwd(pointer));
            else 
               this->Clear();
            return *this;
         }
      }

      /// Three-way comparison                                                
      constexpr auto operator <=> (const TRef& rhs) const noexcept {
         return *Base::GetRaw() <=> *rhs.GetRaw();
      }

      friend constexpr auto operator <=> (const TRef& lhs, const T rhs) noexcept {
         return *lhs.GetRaw() <=> rhs;
      }

      friend constexpr auto operator <=> (const T lhs, const TRef& rhs) noexcept {
         return lhs <=> *rhs.GetRaw();
      }

      friend constexpr auto operator <=> (const TRef& lhs, nullptr_t) noexcept {
         return *lhs.GetRaw() <=> nullptr;
      }

      friend constexpr auto operator <=> (nullptr_t, const TRef& rhs) noexcept {
         return nullptr <=> *rhs.GetRaw();
      }

      /// Equality comparison                                                 
      constexpr bool operator == (const TRef& rhs) const noexcept {
         return *Base::GetRaw() == *rhs.GetRaw();
      }

      friend constexpr bool operator == (const TRef& lhs, const T rhs) noexcept {
         return *lhs.GetRaw() == rhs;
      }

      friend constexpr bool operator == (const T lhs, const TRef& rhs) noexcept {
         return lhs == *rhs.GetRaw();
      }

      friend constexpr bool operator == (const TRef& lhs, nullptr_t) noexcept {
         return *lhs.GetRaw() == nullptr;
      }

      friend constexpr bool operator == (nullptr_t, const TRef& rhs) noexcept {
         return *rhs.GetRaw() == nullptr;
      }

      explicit constexpr operator bool () const noexcept {
         return *this->GetRaw() != nullptr;
      }
      
      constexpr bool IsEmpty() const noexcept {
         return *this->GetRaw() == nullptr;
      }
   };

   /// MARK: CTAD                                                             
   template<CT::Sparse T>
   TRef(T&&) -> TRef<Decvq<Deref<Deint<T>>>>;
}