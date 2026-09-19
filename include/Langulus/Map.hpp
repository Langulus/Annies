///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "HandlePair.hpp"
#include "Annies/Components/Typed-Stack.hpp"
#include "Annies/Components/Multitype.hpp"
#include "Annies/Components/Heap-Movable.hpp"
#include "Annies/Components/Count-Stack.hpp"
#include "Annies/Components/Reserve-Stack.hpp"
#include "Annies/Components/Ownership-Stack.hpp"
#include "Annies/Components/OwnershipDeep-Heap.hpp"
#include "Annies/Components/Hash-Heap.hpp"
#include "Annies/Components/IndexedHash-Heap.hpp"
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
#include "Annies/States/Sorted.hpp"
#include "Annies/States/Compressed.hpp"
#include "Annies/States/Encrypted.hpp"
#include "Annies/States/Tracked.hpp"
#include <Langulus/IntentOf.hpp>


namespace Langulus::Annies::Inner
{
   template<StateValue SORT>
   using MapBase = Com::Container<
      Com::State::Disowned<>,             // Allows disownment          
      Com::Multitype<Com::TypedStack<DMeta, void, false, 0>,
                     Com::TypedStack<DMeta, void, false, 1>>,
      Com::HeapMovable<8, 2, HeapEntry<0>, HeapEntry<1>>,
      Com::CountStack<size_t, 0, 1>,      // Dynamically sized          
      Com::ReserveStack<size_t, 0, 1>,    // Reserve kept as member     
      Com::IndexedHashHeap<0, Hash, 1>,   // Indexed by hash table      
      Com::OwnershipStack<Com::StrongOwnership, 0, 1>,
      Com::OwnershipDeepHeap<Com::StrongOwnership, true, 0, 1>,
      Com::HashHeap<0, Hash, 1>,          // Hash can be cached         
      Com::Merging<false, 0, 1>,          // Only merging for keys      
      //Com::Assignment<false, 1>,        // Assignment of values       
      Com::Removal<0, 1>,                 // Allows clear/reset of K/V  
      Com::Conversion<0, 1>,              // Allows conversions of K/V  
      Com::Comparison<false, true, 0, 1>, // Allows comparisons of K/V  
      Com::IterationForEach<0, 1>,        // ForEach iteration of K/V   
      Com::IterationRange<0, 1>,          // Ranged iteration of K/V    
      Com::State::Sorted<SORT>,           // Toggle ordered map         
      Com::State::Compressed<>,           // Toggle compression         
      Com::State::Encrypted<>             // Toggle encryption          
   >;

   /// MARK: Map                                                              
   ///                                                                        
   /// A universal type-erased non-contiguous map of variable size.           
   /// Emplacement is disabled for maps, because keys aren't allowed to       
   /// change in-place. This also means that they are only const-iteratable.  
   /// Values, on the other hand, are mutable.                                
   template<StateValue SORTED>
   struct Map : MapBase<SORTED> {
      using CTTI_ReflectAs = Map;
      using CTTI_Map       = Yup;
      using CTTI_Deep      = Yup;
      using Base           = MapBase<SORTED>;
      using DeepType       = Many;
      using HandleType     = THandlePair<Handle, Handle>;
      using HandleMutType  = THandlePair<Handle, HandleMut>;
      using Pick           = HandleType;
      using PickMut        = HandleMutType;

      static constexpr bool ReferenceElements = true;

      constexpr Map() noexcept {
         this->ConstructDefault();
      }
      constexpr Map(Map const& other) {
         this->Absorb(Refer(other));
      }
      constexpr Map(Map&& other) noexcept  {
         this->Absorb(Move(other));
      }
      constexpr ~Map() noexcept {
         this->Destroy();
      }

      /// Construction that either absorbs the provided containers, or        
      /// emplaces all A in the container                                     
      template<class A1, class...AN>
      constexpr Map(A1&& a1, AN&&...an) {
         if constexpr (CT::Map<A1> and sizeof...(AN) == 0) {
            /*LglsAssumeUser((Same<Deint<A1>, Map>),
               "Ambiguous use of construction "
               "- you should use tag-dispatch with first argument either Absorb "
               "(if you want to overwrite the container itself) or Piecewise "
               "(if you want to overwrite the first item) in order to clearly "
               "state your intent. Absorb will be used by default!"
            );*/ //TODO irrelevant for maps? never ambiguous because they require pairs, and if not pairs, then at least two arguments?
            this->Absorb(LglsFwd(a1));
         }
         else {
            static_assert(CT::Pair<A1, AN...>, "Arguments must be pairs");
            this->ConstructDefault();
            this->Merge(NestIntentOf(a1, DeintCast(a1).GetHandle()));
           (this->Merge(NestIntentOf(an, DeintCast(an).GetHandle())), ...);
         }
      }
      
      /// Construction that absorbs the provided containers                   
      template<class A1, class...AN>
      constexpr Map(Inner::Absorb, A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0)
            this->Absorb(LglsFwd(a1));
         else {
            this->ConstructDefault();
            this->MergeRange(LglsFwd(a1));
           (this->MergeRange(LglsFwd(an)), ...);
         }
      }
      
      /// Construction that merges all provided pairs                         
      template<CT::Pair A1, CT::Pair...AN>
      constexpr Map(Inner::Piecewise, A1&& a1, AN&&...an) {
         this->ConstructDefault();
         this->Merge(NestIntentOf(a1, DeintCast(a1).GetHandle()));
        (this->Merge(NestIntentOf(an, DeintCast(an).GetHandle())), ...);
      }
      
      /// Assignment                                                          
      constexpr Map& operator = (Map const& other) {
         return this->AssignAbsorb(Refer(other));
      }
      constexpr Map& operator = (Map&& other) noexcept {
         return this->AssignAbsorb(Move(other));
      }
      
      template<class A>
      constexpr Map& operator = (A&& argument) {
         if constexpr (CT::Map<A>) {
            /*LglsAssumeUser((Same<Deint<A>, Map>),
               "Ambiguous use of assignment "
               "- you should use either AssignAbsorb (if you want to overwrite "
               "the container itself) or Assign (if you want to overwrite the "
               "first item) in order to clearly state your intent. "
               "AssignAbsorb will be used by default!"
            );*/ //TODO irrelevant for maps?
            return this->AssignAbsorb(LglsFwd(argument));
         }
         else {
            static_assert(CT::Pair<A>, "Argument must be pair or map");
            return this->Assign(LglsFwd(argument));
         }
      }

      /// Create a temporary swapper with compatible elements and initialize  
      /// it with a compatible value, by shallow copying it.                  
      template<CT::Pair P>
      constexpr auto CreateSwapper(P&& pair) assumptious {
         using K = Decvq<Deref<TypeOf<Deint<P>, 0>>>;
         using V = Decvq<Deref<TypeOf<Deint<P>, 1>>>;
         LglsAssumeDev(this->template IsSame<K, 0>(), "Type mismatch");
         LglsAssumeDev(this->template IsSame<V, 1>(), "Type mismatch");
         return TPair<K, V> {Copy {LglsFwd(pair)}}; //TODO Annies::Piecewise, ?
         //return TPair<K, V> {LglsFwd(pair)}; //TODO Annies::Piecewise, ?
      }

      /// Clear the map and assign a single pair                              
      auto Assign(CT::Pair auto&& pair) -> Map& {
         this->Reset();
         this->Merge(NestIntentOf(pair, DeintCast(pair).GetHandle()));
         return *this;
      }

      /// Clear the map and assign a key and a value                          
      auto Assign(auto&& key, auto&& val) -> Map& {
         this->Reset();
         TPair temp {LglsFwd(key), LglsFwd(val)};
         this->Merge(Abandon {temp.GetHandle()});
         return *this;
      }

      /// Equality comparison with maps                                       
      constexpr bool operator == (CT::Map auto const& rhs) const assumptious {
         return Com::Comparison<false, true, 0, 1>::operator == (rhs);
      }

      /// Equality comparison with pairs                                      
      constexpr bool operator == (CT::Pair auto const& rhs) const assumptious {
         using C = Com::Comparison<false, true, 0, 1>;
         return C::template CompareOneEqual<0>(rhs.GetKey())
            and C::template CompareOneEqual<1>(rhs.GetVal());
      }

      constexpr bool IsKeyConstant() const noexcept {
         return true;
      }
   };
}

namespace Langulus::Annies
{
   using Map         = Inner::Map<StateValue::Variable>;
   using MapSorted   = Inner::Map<StateValue::Enabled>;
   using MapUnsorted = Inner::Map<StateValue::Disabled>;
}