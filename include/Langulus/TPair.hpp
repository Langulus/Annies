///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "HandlePair.hpp"
#include <Annies/Components/Typed-Static.hpp"
#include <Annies/Components/Typed-Stack.hpp"
#include <Annies/Components/Heap-Movable.hpp"
#include <Annies/Components/Stack.hpp"
#include <Annies/Components/Count-Static.hpp"
#include <Annies/Components/Reserve-Static.hpp"
#include <Annies/Components/Ownership-Stack.hpp"
#include <Annies/Components/OwnershipDeep-Heap.hpp"
#include <Annies/Components/Hash-Emergent.hpp"
#include <Annies/Components/Emplacement.hpp"
#include <Annies/Components/Assignment.hpp"
#include <Annies/Components/Removal.hpp"
#include <Annies/Components/Conversion.hpp"
#include <Annies/Components/Comparison.hpp"
#include <Annies/Components/Multitype.hpp"
#include <Annies/Components/Multiprovider.hpp"
#include <Annies/Components/Multiown-Deep.hpp"
#include <Annies/States/Encrypted.hpp"
#include <Annies/States/Disowned.hpp"


namespace Langulus::Annies::Inner
{
   /// MARK: Bases                                                            
   /// Stack based pair. Supports references.                                 
   ///   @attention not binary compatible with Pair                           
   template<CT::NotVoid K, CT::NotVoid V> requires CT::NotHandle<K, V>
   using TPairStackBase = Com::Container<
      Com::Multitype    <Com::TypedStatic<DMeta, Deref<K>, 0>,
                         Com::TypedStatic<DMeta, Deref<V>, 1>>,
      Com::Multiprovider<Com::Stack<K, 0>,
                         Com::Stack<V, 1>>,
      Com::CountStatic<1u, 0, 1>,         // Statically sized to 1      
      Com::ReserveStatic<1u, 0, 1>,       // Statically reserved to 1   
      Com::OwnershipDeepEmergent<Com::StrongOwnership, true, 0, 1>,
      Com::HashEmergent<0, Hash, 1>,      // Hash retrieved from items  
      Com::Emplacement<0, 1>,             // Allows emplacement         
      Com::Assignment<false, 0, 1>,       // Allows assignment          
      Com::Removal<0, 1>,                 // Allows clear/reset         
      Com::Conversion<0, 1>,              // Allows conversion          
      Com::Comparison<false, true, 0, 1>  // Allows comparisons         
   >;

   /// Heap based pair. Binary compatible with Pair.                          
   ///   @attention does not support references                               
   template<CT::NotVoid K, CT::NotVoid V>
   requires (CT::NotHandle<K, V> and CT::NotReference<K, V>)
   using TPairHeapBase = Com::Container<
      Com::State::Disowned<>,             // Allows disownment          
      Com::Multitype<Com::TypedStack<DMeta, K, true, 0>,
                     Com::TypedStack<DMeta, V, true, 1>>,
      Com::HeapMovable<0, 0, HeapEntry<0, K*>, HeapEntry<1, V*>>,
      Com::CountStatic<1u, 0, 1>,         // Statically sized to 1      
      Com::ReserveStatic<1u, 0, 1>,       // Statically reserved to 1   
      Com::OwnershipStack<Com::StrongOwnership, 0, 1>,
      Com::MultiownDeep<EnableComponentIf<CT::Sparse<K>, Com::OwnershipDeepHeap<Com::StrongOwnership, true, 0>>,
                        EnableComponentIf<CT::Sparse<V>, Com::OwnershipDeepHeap<Com::StrongOwnership, true, 1>>>,
      Com::HashEmergent<0, Hash, 1>,      // Hash retrieved from items  
      Com::Emplacement<0, 1>,             // Allows emplacement         
      Com::Assignment<false, 0, 1>,       // Allows assignment          
      Com::Removal<0, 1>,                 // Allows clear/reset         
      Com::Conversion<0, 1>,              // Allows conversion          
      Com::Comparison<false, true, 0, 1>, // Allows comparisons         
      Com::State::Encrypted<>             // Toggle encryption          
   >;

   template<CT::NotVoid K, CT::NotVoid V> requires CT::NotHandle<K, V>
   using TPairBase = Tif<CT::NotReference<K, V>, TPairHeapBase<Deref<K>, Deref<V>>, TPairStackBase<K, V>>;

   template<class K, class V>
   concept PairOnStack = not CT::NotReference<K, V>;

   template<class K, class V>
   concept PairOnHeap = not PairOnStack<K, V>;
}

namespace Langulus::Annies
{
   /// MARK: TPair                                                            
   ///                                                                        
   /// A statically-typed pair. Supports holding references.                  
   ///   @attention when containing references, this pair is stack-based and  
   ///      binary incompatible with Pair                                     
   template<CT::NotVoid K, CT::NotVoid V>
   struct TPair : Inner::TPairBase<K, V> {
      using CTTI_ReflectAs = TPair;
      using CTTI_Deep      = Yup;
      using CTTI_Pair      = Yup;

      using Base           = Inner::TPairBase<K, V>;
      using DeepType       = Any;

      #if not LANGULUS(FORCE_TYPE_ERASURE)
         using HandleType    = Tif<CT::NotReference<K, V>,
            THandlePair<THandle        <ConstAll<K&>>, THandle        <ConstAll<V&>>>,
            THandlePair<THandleEmergent<ConstAll<K&>>, THandleEmergent<ConstAll<V&>>>
         >;
         using HandleMutType = Tif<CT::NotReference<K, V>,
            THandlePair<THandle        <K&>,  THandle        <V&>>,
            THandlePair<THandleEmergent<K&>,  THandleEmergent<V&>>
         >;
      #else
         using HandleType    = THandlePair<Handle, Handle>;
         using HandleMutType = THandlePair<HandleMut, HandleMut>;
      #endif

      using Pick           = HandleType;
      using PickMut        = HandleMutType;

      constexpr TPair() noexcept requires Inner::PairOnHeap<K, V> {
         this->ConstructDefault();
      }
      constexpr TPair(TPair const& other) requires Inner::PairOnHeap<K, V> {
         this->Absorb(Refer(other));
      }
      constexpr TPair(TPair&& other) noexcept requires Inner::PairOnHeap<K, V> {
         this->Absorb(Move(other));
      }
      constexpr ~TPair() noexcept {
         this->Destroy();
      }
      
      constexpr TPair(CT::Pair auto&& p) {
         this->Absorb(LglsFwd(p));
      }
      
      constexpr TPair(Inner::Absorb, CT::Pair auto&& p) {
         this->Absorb(LglsFwd(p));
      }

      /// Stack-based constructors                                            
      template<CT::NotHandle K_ALT, CT::NotHandle V_ALT>
      constexpr TPair(K_ALT&& a1, V_ALT&& a2) requires (Inner::PairOnStack<K, V> and NotTag<K_ALT, V_ALT>)
         : Base {Stackwise, LglsFwd(a1), LglsFwd(a2)} {
         if constexpr (CT::Sparse<K> or CT::Sparse<V>)
            this->Com::OwnershipDeepEmergent<Com::StrongOwnership, true, 0, 1>::Keep();
      }
      
      template<CT::NotHandle K_ALT, CT::NotHandle V_ALT>
      constexpr TPair(Inner::Piecewise, K_ALT&& a1, V_ALT&& a2) requires Inner::PairOnStack<K, V>
         : Base {Stackwise, LglsFwd(a1), LglsFwd(a2)} {
         if constexpr (CT::Sparse<K> or CT::Sparse<V>)
            this->Com::OwnershipDeepEmergent<Com::StrongOwnership, true, 0, 1>::Keep();
      }

      template<CT::NotHandle K_ALT>
      constexpr TPair(Inner::Piecewise, K_ALT&& a1) requires Inner::PairOnStack<K, V>
         : Base {Stackwise, LglsFwd(a1), {}} {
         if constexpr (CT::Sparse<K> or CT::Sparse<V>)
            this->Com::OwnershipDeepEmergent<Com::StrongOwnership, true, 0, 1>::Keep();
      }

      /// Construct from handles                                              
      template<NotTag K_ALT, NotTag V_ALT>
      constexpr TPair(K_ALT&& a1, V_ALT&& a2)
      requires (Inner::PairOnHeap<K, V> or CT::Handle<K_ALT> or CT::Handle<V_ALT>) {
         this->ResetState();
         this->DeduceType(a1, a2);
         
         if constexpr (Inner::PairOnHeap<K, V>) {
            this->AllocateFresh(1);
            this->template EmplaceConstruct<0, Com::AllocationStrategy::DontAllocate>(FWDIntent(a1));
            this->template EmplaceConstruct<1, Com::AllocationStrategy::DontAllocate>(FWDIntent(a2));
         }
         else {
            this->template EmplaceWithIntent<0>(FWDIntent(a1));
            this->template EmplaceWithIntent<1>(FWDIntent(a2));
         }
      }

      constexpr TPair(Inner::Piecewise, auto&& a1, auto&& a2) requires Inner::PairOnHeap<K, V>
         : TPair {LglsFwd(a1), LglsFwd(a2)} {}
     
      /// Assignment                                                          
      constexpr TPair& operator = (TPair const& other) {
         return this->AssignAbsorb(Refer(other));
      }
      constexpr TPair& operator = (TPair&& other) noexcept {
         return this->AssignAbsorb(Move(other));
      }
      constexpr TPair& operator = (CT::Pair auto&& pair) {
         return this->AssignAbsorb(LglsFwd(pair));
      }

      /// Clear the pair and assign a key and a value                         
      constexpr TPair& Assign(auto&& a1, auto&& a2) {
         if constexpr (Inner::PairOnHeap<K, V>) {
            this->Reset();
            this->DeduceType(a1, a2);
            this->AllocateFresh(1);
            this->template EmplaceConstruct<0, Com::AllocationStrategy::DontAllocate>(LglsFwd(a1));
            this->template EmplaceConstruct<1, Com::AllocationStrategy::DontAllocate>(LglsFwd(a2));
         }
         else {
            this->DeduceType(a1, a2);
            this->template AssignWithIntent<0>(FWDIntent(a1));
            this->template AssignWithIntent<1>(FWDIntent(a2));
         }
         return *this;
      }

      using Com::Comparison<false, true, 0, 1>::operator <=>;
      using Com::Comparison<false, true, 0, 1>::operator ==;

      auto GetKeyHandle() const noexcept -> typename HandleType::KeyHandle {
         return {*this};
      }

      auto GetKeyHandle() noexcept -> typename HandleMutType::KeyHandle {
         return {*this};
      }

      auto GetValHandle() const noexcept -> typename HandleType::ValHandle {
         return {Slice<1>, *this};
      }

      auto GetValHandle() noexcept -> typename HandleMutType::ValHandle {
         return {Slice<1>, *this};
      }
   };

   /// MARK: CTAD                                                             
   template<CT::Pair P>
   TPair(P&&) -> TPair<TypeOf<Deint<P>, 0>, TypeOf<Deint<P>, 1>>;

   template<CT::NotHandle K, CT::NotHandle V>
   TPair(K&&, V&&) -> TPair<Decvq<Deref<Deint<K>>>, Decvq<Deref<Deint<V>>>>;

   template<CT::Handle K, CT::Handle V>
   TPair(K&&, V&&) -> TPair<TypeOf<Deint<K>>, TypeOf<Deint<V>>>;
}

namespace Langulus
{
   using Annies::TPair;
}