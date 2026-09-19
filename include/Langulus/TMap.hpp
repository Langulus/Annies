///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Langulus/IntentOf.hpp"
#include "Map.hpp"


namespace Langulus::Annies::Inner
{
   /// MARK: TMapBase                                                         
   template<CT::NotVoid K, CT::NotVoid V, StateValue SORT>
   requires (CT::NotHandle<K, V> and CT::NotReference<K, V>)
   using TMapBase = Com::Container<
      Com::State::Disowned<>,             // Allows disownment          
      Com::Multitype<Com::TypedStack<DMeta, K, true, 0>,
                     Com::TypedStack<DMeta, V, true, 1>>,
      Com::HeapMovable<8, 2, HeapEntry<0, K*>, HeapEntry<1, V*>>,
      Com::CountStack<size_t, 0, 1>,      // Dynamically sized          
      Com::ReserveStack<size_t, 0, 1>,    // Reserve kept as member     
      Com::IndexedHashHeap<0, Hash, 1>,   // Indexed by hash table      
      Com::OwnershipStack<Com::StrongOwnership, 0, 1>,
      Com::MultiownDeep<EnableComponentIf<CT::Sparse<K>, Com::OwnershipDeepHeap<Com::StrongOwnership, true, 0>>,
                        EnableComponentIf<CT::Sparse<V>, Com::OwnershipDeepHeap<Com::StrongOwnership, true, 1>>>,
      Com::HashHeap<0, Hash, 1>,          // Hash can be cached         
      Com::Merging<false, 0, 1>,          // Only merging for keys      
      Com::Removal<0, 1>,                 // Allows clear/reset of K/V  
      Com::Conversion<0, 1>,              // Allows conversions of K/V  
      Com::Comparison<false, true, 0, 1>, // Allows comparisons of K/V  
      Com::IterationForEach<0, 1>,        // ForEach iteration of K/V   
      Com::IterationRange<0, 1>,          // Ranged iteration of K/V    
      Com::State::Sorted<SORT>,           // Toggle ordered map         
      Com::State::Compressed<>,           // Toggle compression         
      Com::State::Encrypted<>             // Toggle encryption          
   >;
}

namespace Langulus::Annies
{
   /// MARK: TMap                                                             
   ///                                                                        
   ///   A statically-typed non-contiguous map of variable size that is       
   /// binary-compatible with the type-erased alternative `Map`.              
   /// Emplacement is disabled for maps, because keys aren't allowed to       
   /// change in-place. This also means that they are only const-iteratable.  
   /// Values, on the other hand, are mutable.                                
   template<CT::NotVoid K, CT::NotVoid V, StateValue SORT>
   struct TMap : Inner::TMapBase<K, V, SORT> {
      using CTTI_ReflectAs = Map;
      using CTTI_Map       = Yup;
      using CTTI_Deep      = Yup;

      using Base           = Inner::TMapBase<K, V, SORT>;
      using DeepType       = Any;

      #if not LANGULUS(FORCE_TYPE_ERASURE)
         using HandleType     = THandlePair<THandle<ConstAll<K&>>, THandle<ConstAll<V&>>>;
         using HandleMutType  = THandlePair<THandle<ConstAll<K&>>, THandle<V&>>;
      #else
         using HandleType     = THandlePair<Handle, Handle>;
         using HandleMutType  = THandlePair<Handle, HandleMut>;
      #endif
      
      using Pick           = HandleType;
      using PickMut        = HandleMutType;

      static constexpr bool ReferenceElements = true;

      constexpr TMap() noexcept {
         this->ConstructDefault();
      }
      constexpr TMap(TMap const& other) {
         this->Absorb(Refer(other));
      }
      constexpr TMap(TMap&& other) noexcept {
         this->Absorb(Move(other));
      }
      constexpr ~TMap() noexcept {
         this->Destroy();
      }
      
      /// Construction that either absorbs the provided containers, or        
      /// emplaces all A in the container                                     
      template<class A1, class...AN>
      constexpr TMap(A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0 and CT::Map<A1>)
            this->Absorb(LglsFwd(a1));
         else {
            static_assert(CT::Pair<A1, AN...>, "Arguments must be pairs");
            this->ConstructDefault();
            this->Merge(NestIntentOf(a1, DeintCast(a1).GetHandle()));
           (this->Merge(NestIntentOf(an, DeintCast(an).GetHandle())), ...);
         }
      }
      
      /// Construction that absorbs the provided container                    
      template<class A1, class...AN>
      constexpr TMap(Inner::Absorb, A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0)
            this->Absorb(LglsFwd(a1));
         else {
            this->ConstructDefault();
            this->MergeRange(LglsFwd(a1));
           (this->MergeRange(LglsFwd(an)), ...);
         }
      }
      
      /// Construction that emplaces all arguments inside                     
      template<CT::Pair A1, CT::Pair...AN>
      constexpr TMap(Inner::Piecewise, A1&& a1, AN&&...an) {
         this->ConstructDefault();
         this->Merge(NestIntentOf(a1, DeintCast(a1).GetHandle()));
        (this->Merge(NestIntentOf(an, DeintCast(an).GetHandle())), ...);
      }

      /// Assignment                                                          
      constexpr TMap& operator = (TMap const& other) {
         return this->AssignAbsorb(Refer(other));
      }
      constexpr TMap& operator = (TMap&& other) noexcept {
         return this->AssignAbsorb(Move(other));
      }

      template<class A>
      constexpr TMap& operator = (A&& argument) {
         if constexpr (CT::Map<A>)
            return this->AssignAbsorb(LglsFwd(argument));
         else {
            static_assert(CT::Pair<A>, "Argument must be pair or map");
            return this->Assign(LglsFwd(argument));
         }
      }
      
      /// Create a temporary swapper with compatible elements and initialize  
      /// it with a compatible value, by shallow copying it.                  
      template<CT::Pair P>
      constexpr auto CreateSwapper(P&& pair) assumptious {
         using PK = Decvq<Deref<TypeOf<Deint<P>, 0>>>;
         using PV = Decvq<Deref<TypeOf<Deint<P>, 1>>>;
         static_assert(Same<PK, K>, "Key type mismatch");
         static_assert(Same<PV, V>, "Val type mismatch");
         return TPair<PK, PV> {Copy {LglsFwd(pair)}}; //TODO Annies::Piecewise, ?
      }

      /// Clear the map and assign a single pair                              
      auto Assign(CT::Pair auto&& pair) -> TMap& {
         this->Clear();
         this->Merge(NestIntentOf(pair, DeintCast(pair).GetHandle()));
         return *this;
      }

      /// Clear the map and assign a key and a value                          
      auto Assign(auto&& key, auto&& val) -> TMap& {
         this->Clear();
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

   /// MARK: CTAD                                                             
   template<CT::NotVoid K, CT::NotVoid V>
   using TMapSorted = TMap<K, V, StateValue::Enabled>;

   template<CT::NotVoid K, CT::NotVoid V>
   using TMapUnsorted = TMap<K, V, StateValue::Disabled>;
}