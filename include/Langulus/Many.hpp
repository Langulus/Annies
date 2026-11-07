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
#include "Annies/Components/Ownership-Stack.hpp"
#include "Annies/Components/Count-Stack.hpp"
#include "Annies/Components/Reserve-Stack.hpp"
#include "Annies/Components/OwnershipDeep-Heap.hpp"
#include "Annies/Components/Hash-Stack.hpp"
#include "Annies/Components/Deepening.hpp"
#include "Annies/Components/Emplacement.hpp"
#include "Annies/Components/IndexedLinear.hpp"
#include "Annies/Components/Insertion.hpp"
#include "Annies/Components/InsertionOperators.hpp"
#include "Annies/Components/InsertionOperatorsConcat.hpp"
#include "Annies/Components/Merging.hpp"
#include "Annies/Components/MergingOperators.hpp"
#include "Annies/Components/Assignment.hpp"
#include "Annies/Components/Removal.hpp"
#include "Annies/Components/Conversion.hpp"
#include "Annies/Components/Comparison.hpp"
#include "Annies/Components/Iteration-ForEach.hpp"
#include "Annies/Components/Iteration-Range.hpp"
#include "Annies/Components/State-Stack.hpp"
#include "Annies/Components/Descriptor.hpp"
#include "Annies/States/Disowned.hpp"
#include "Annies/States/Future.hpp"
#include "Annies/States/Past.hpp"
#include "Annies/States/Compressed.hpp"
#include "Annies/States/Encrypted.hpp"
#include "Annies/States/Or.hpp"
#include "Annies/States/Tracked.hpp"


namespace Langulus::Annies::Inner
{
   using ManyBase = Com::Container<
      Com::State::Disowned<>,          // Allows disownment             
      Com::State::Future<>,            // Toggle future linking         
      Com::State::Past<>,              // Toggle past linking           
      Com::State::Or<>,                // Toggle disjunction            
      Com::State::Compressed<>,        // Toggle compression            
      Com::State::Encrypted<>,         // Toggle encryption             
      Com::TypedStack<DMeta>,          // Type-erased                   
      Com::HeapMovable<>,              // Pointer to heap memory        
      Com::CountStack<>,               // Dynamically sized             
      Com::ReserveStack<>,             // Reserve kept as member        
      Com::IndexedLinear<>,            // Indexed directly              
      Com::OwnershipStack<>,           // Allocation is referenced      
      Com::OwnershipDeepHeap<>,        // Sparse elements are referenced
      Com::HashStack<>,                // Hash can be cached            
      Com::Deepening<Many>,            // Makes container deep          
      Com::Insertion<>,                // Allows insertion              
      Com::InsertionOperators<>,       // << and >> insertion           
      Com::InsertionOperatorsConcat<>,
      Com::Merging<>,                  // Allows merging                
      Com::MergingOperators<>,         // <<= and >>= merging           
      Com::Emplacement<>,              // Allows emplacement            
      Com::Assignment<>,               // Allows assignment             
      Com::Removal<>,                  // Allows clear/reset            
      Com::Conversion<>,               // Allows conversions            
      Com::Comparison<>,               // Allows comparisons            
      Com::IterationForEach<>,         // ForEach iteration             
      Com::IterationRange<>,           // Ranged iteration              
      Com::Descriptor<>                // Add descriptor interface      
   >;
}

namespace Langulus::Annies
{
   ///                                                                        
   ///   A universal type-erased contiguous container of variable size        
   ///                                                                        
   ///   This is the most universal and feature-complete container, that      
   /// supports all kinds of data states: branching, compression, encryption, 
   /// linking, and so on. If you want to contain a single element, consider  
   /// using Any instead, for a bit shorter and faster representation.        
   struct Many : Inner::ManyBase {
      using CTTI_ReflectAs = Many;
      using Base           = Inner::ManyBase;
      
      constexpr Many() noexcept {
         this->ConstructDefault();
      }
      constexpr Many(Many const& other) {
         this->Absorb(Refer(other));
      }
      constexpr Many(Many&& other) noexcept  {
         this->Absorb(Move(other));
      }
      constexpr ~Many() noexcept {
         this->Destroy();
      }

      /// Create an empty container from a dynamic type and state             
      ///   @param type type of the container                                 
      ///   @param states optional states of the container                    
      ///   @return the new container instance                                
      static Many Typed(DMeta type, int states = 0) {
         Many result;
         result.SetType(type);
         result.SetState(states);
         return result;
      }
   
      static Many Typed(CT::Container auto const& typed, int states = 0) {
         Many result;
         result.SetType(typed.GetType());
         result.SetState(states);
         return result;
      }
   
      /// Create an empty container by copying another container's state      
      ///   @attention only the unconstrained state is copied                 
      ///   @param states states of the container                             
      ///   @param additional additional states of the container              
      ///   @return the new container instance                                
      static Many CopyStates(CT::Container auto const& states, int additional = 0) {
         Many result;
         result.SetState(states.GetState() | additional);
         return result;
      }

      #if LANGULUS_FEATURE(MANAGED_REFLECTION)
         /// Make a container with a missing past state and optional filter   
         /// at runtime, using reflected tokens                               
         ///   @param tokens... reflected tokens                              
         ///   @return a container marked as missing past with chosen filters 
         template<CT::Text...T>
         static Many Past(T&&...tokens) {
            Many result;
            result.EnablePast();
            if constexpr (sizeof...(tokens)) {
               auto push = [&result](RTTI::DefinitionData const* meta) {
                  LglsAssert(meta, "Missing meta for past filter");
                  result << DMeta {meta};
               };
               (push(RTTI::Registry::GetMetaDataByToken(tokens)), ...);
            }
            return result;
         }

         /// Make a container with a missing future state and optional filter 
         /// at runtime, using reflected tokens                               
         ///   @param tokens... reflected tokens                              
         ///   @return container marked as missing future with chosen filters 
         template<CT::Text...T>
         static Many Future(T&&...tokens) {
            Many result;
            result.EnableFuture();
            if constexpr (sizeof...(tokens)) {
               auto push = [&result](RTTI::DefinitionData const* meta) {
                  LglsAssert(meta, "Missing meta for future filter");
                  result << DMeta {meta};
               };
               (push(RTTI::Registry::GetMetaDataByToken(tokens)), ...);
            }
            return result;
         }
      #endif
   
      /// Construction that either absorbs the provided containers, or        
      /// emplaces all A in the container                                     
      template<Disambiguate A1, class...AN>
      constexpr Many(A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0 and not CT::BoundedArray<A1>) {
            if constexpr (CT::DeepDense<Deint<A1>>) {
               LglsAssumeUser(Same<Deint<A1>, Many>,
                  "Ambiguous use of construction "
                  "- you should use tag-dispatch with first argument either Absorb "
                  "(if you want to overwrite the container itself) or Piecewise "
                  "(if you want to overwrite the first item) in order to clearly "
                  "state your intent. Absorb will be used by default!"
               );
               this->Absorb(LglsFwd(a1));
            }
            else this->EmplaceConstruct(LglsFwd(a1));
         }
         else {
            this->ConstructDefault();
            this->Insert(LglsFwd(a1), LglsFwd(an)...);
         }
      }
      
      /// Construction that absorbs an abstract memory block                  
      constexpr Many(Inner::Absorb, RTTI::Block const& b)
         : Base {Stackwise, StateType{}, b.type, b.data, b.count, 0u, nullptr, Hash {0}} {}

      /// Construction that absorbs the provided containers                   
      template<class A1, class...AN> requires (not Same<A1, RTTI::Block>)
      constexpr Many(Inner::Absorb, A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0)
            this->Absorb(LglsFwd(a1));
         else {
            this->ConstructDefault();
            this->Concat(LglsFwd(a1), LglsFwd(an)...);
         }
      }
      
      /// Construction that emplaces all arguments inside                     
      template<class A1, class...AN>
      constexpr Many(Inner::Piecewise, A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0 and not CT::BoundedArray<A1>)
            this->EmplaceConstruct(LglsFwd(a1));
         else {
            this->ConstructDefault();
            this->Insert(LglsFwd(a1), LglsFwd(an)...);
         }
      }

      /// Construction that manually initializes the container                
      constexpr Many(Inner::Stackwise, auto&&...arguments)
         : Base {Stackwise, LglsFwd(arguments)...} {}

      /// Assignment                                                          
      constexpr Many& operator = (Many const& other) {
         return this->AssignAbsorb(Refer(other));
      }
      constexpr Many& operator = (Many&& other) noexcept {
         return this->AssignAbsorb(Move(other));
      }
      
      template<class A>
      constexpr Many& operator = (A&& argument) {
         if constexpr (CT::DeepDense<Deint<A>>) {
            LglsAssumeUser(Same<Deint<A>, Many>,
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
      //using Com::Comparison<>::operator ==;

      LANGULUS_API(ANNIES)
      bool operator == (Many const& other) const noexcept;

      template<CT::Container C> requires (not Same<C, Many>)
      constexpr bool operator == (C const& other) const noexcept {
         return this->CompareEqual(other);
      }
   };
}

#if defined(LANGULUS_EXPORT_ALL) or defined(LANGULUS_EXPORT_ANNIES)
namespace Langulus::Annies::Component
{
   extern template void  Inner::ManyBase::ConstructDefault<Many>(this Many&& self) noexcept;
   extern template void  Inner::ManyBase::Absorb<Many, Many>(this Many& self, Many&& from);
   extern template void  Inner::ManyBase::Destroy<Many>(this Many& self);
   extern template void  Inner::ManyBase::Free<true, Many>(this Many& self);
   extern template void  Inner::ManyBase::Free<false, Many>(this Many& self);
   extern template void  Inner::ManyBase::Keep<Many>(this Many& self);
   extern template Many& Inner::ManyBase::AssignAbsorb<Many, Many>(this Many& self, Many&& rhs);
   extern template bool  Inner::ManyBase::IsMissingDeep<Many>(this Many const& self) noexcept;

   extern template void  Assignment<false, 0>::PrepareForReconstruction<Many>(this Many&);
   extern template bool  Assignment<false, 0>::PrepareForReassignment<Many>(this Many&);

   extern template bool   Comparison<false, true, 0>::CompareEqual<0, Many, Many>(this Many const&, Many const&);
   extern template bool   Comparison<false, true, 0>::CompareHashes<Many, Many>(this Many const&, Many const&);
   extern template size_t Comparison<false, true, 0>::Matches<Many, Many>(this Many const&, Many const&);

   extern template size_t Deepening<Many>::GetCountDeep<Many>(this Many const&, size_t);
   extern template size_t Deepening<Many>::GetReservedDeep<Many>(this Many const&, size_t);
   extern template Many&  Deepening<Many>::Deepen<Many>(this Many&);

   extern template void Removal<0>::Trim<Many>(this Many&, size_t) noexcept;
   extern template void Removal<0>::Clear<Many>(this Many&);
   extern template void Removal<0>::Reset<Many>(this Many&);
}
#else
namespace Langulus::Annies::Component
{
   template LANGULUS_IMPORT() void  Inner::ManyBase::ConstructDefault<Many>(this Many&& self) noexcept;
   template LANGULUS_IMPORT() void  Inner::ManyBase::Absorb<Many, Many>(this Many& self, Many&& from);
   template LANGULUS_IMPORT() void  Inner::ManyBase::Destroy<Many>(this Many& self);
   template LANGULUS_IMPORT() void  Inner::ManyBase::Free<true, Many>(this Many& self);
   template LANGULUS_IMPORT() void  Inner::ManyBase::Free<false, Many>(this Many& self);
   template LANGULUS_IMPORT() void  Inner::ManyBase::Keep<Many>(this Many& self);
   template LANGULUS_IMPORT() Many& Inner::ManyBase::AssignAbsorb<Many, Many>(this Many& self, Many&& rhs);
   template LANGULUS_IMPORT() bool  Inner::ManyBase::IsMissingDeep<Many>(this Many const& self) noexcept;

   template LANGULUS_IMPORT() void  Assignment<false, 0>::PrepareForReconstruction<Many>(this Many&);
   template LANGULUS_IMPORT() bool  Assignment<false, 0>::PrepareForReassignment<Many>(this Many&);

   template LANGULUS_IMPORT() bool   Comparison<false, true, 0>::CompareEqual<0, Many, Many>(this Many const&, Many const&);
   template LANGULUS_IMPORT() bool   Comparison<false, true, 0>::CompareHashes<Many, Many>(this Many const&, Many const&);
   template LANGULUS_IMPORT() size_t Comparison<false, true, 0>::Matches<Many, Many>(this Many const&, Many const&);

   template LANGULUS_IMPORT() size_t Deepening<Many>::GetCountDeep<Many>(this Many const&, size_t);
   template LANGULUS_IMPORT() size_t Deepening<Many>::GetReservedDeep<Many>(this Many const&, size_t);
   template LANGULUS_IMPORT() Many&  Deepening<Many>::Deepen<Many>(this Many&);

   template LANGULUS_IMPORT() void Removal<0>::Trim<Many>(this Many&, size_t) noexcept;
   template LANGULUS_IMPORT() void Removal<0>::Clear<Many>(this Many&);
   template LANGULUS_IMPORT() void Removal<0>::Reset<Many>(this Many&);
}
#endif