///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Handle.hpp"
#include "Annies/Components/Multitype.hpp"
#include "Annies/Components/Multiprovider.hpp"
#include "Annies/Components/Multiown.hpp"
#include "Annies/Components/Multiown-Deep.hpp"
#include "Annies/Components/Reserve-Static.hpp"


namespace Langulus::Annies
{
   /// MARK: H/H                                                              
   ///                                                                        
   /// Type-erased immutable handle                                           
   ///                                                                        
   template<>
   struct THandlePair<Handle, Handle> : Com::Container<
      Com::Multitype<Com::TypedStack<DMeta, void, false, 0>,
                     Com::TypedStack<DMeta, void, false, 1>>,
      Com::Multiprovider<Com::HeapReference<HeapEntry<0>>,
                         Com::HeapReference<HeapEntry<1>>>,
      Com::CountStatic<1u, 0, 1>,
      Com::ReserveStatic<1u, 0, 1>,
      Com::MultiownDeep<Com::OwnershipDeepReference<Com::WeakOwnership, true, 0>,
                        Com::OwnershipDeepReference<Com::WeakOwnership, true, 1>>,
      Com::HashEmergent<0, Hash, 1>,
      Com::Comparison<false, true, 0, 1>,
      Com::IterationOperators<0, 1>
   > {
      using CTTI_Deep      = Yup;
      using CTTI_Handle    = Yup;
      using CTTI_Pair      = Yup;
      using CTTI_ReflectAs = void;

      using DeepType  = HandleDisowned; // TODO why disowned?
      using KeyHandle = Handle;
      using ValHandle = Handle;

      /// Handles can't be piecewise-initialized                              
      //THandlePair(Inner::Piecewise, auto&&) = delete;

      constexpr THandlePair() noexcept {
         this->ConstructDefault();
      }

      constexpr THandlePair(THandlePair const& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr THandlePair(THandlePair&& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr THandlePair(CT::Map auto&& other) noexcept {
         this->Absorb(Disown(other));
      }
      
      constexpr THandlePair(CT::Pair auto&& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr THandlePair(Handle&& key, Handle&& val) noexcept {
         this->Com::HeapReference<HeapEntry<0>>::SetHeapInner(key.GetHeapInner());
         this->Com::HeapReference<HeapEntry<1>>::SetHeapInner(val.GetHeapInner());
         this->Com::OwnershipDeepReference<Com::WeakOwnership, true, 0>::SetEntriesInner(key.GetEntriesInner());
         this->Com::OwnershipDeepReference<Com::WeakOwnership, true, 1>::SetEntriesInner(val.GetEntriesInner());
         this->DeduceType(key, val);
         //this->Com::TypedStack<DMeta, void, false, 0>::SetTypeInner(key.GetTypeInner());
         //this->Com::TypedStack<DMeta, void, false, 1>::SetTypeInner(val.GetTypeInner());
      }

      /// Assignment is disabled                                              
      THandlePair& operator = (THandlePair const&) = default;
      THandlePair& operator = (THandlePair&&) = default;

      Handle GetKey()       const noexcept { return {*this}; }
      Handle GetKeyHandle() const noexcept { return {*this}; }
      
      Handle GetVal()       const noexcept { return {Slice<1>, *this}; }
      Handle GetValHandle() const noexcept { return {Slice<1>, *this}; }

      /// Force the handle to become mutable, so that we have methods like    
      /// emplacement in constructors.                                        
      auto& ForceMutable() noexcept {
         return *reinterpret_cast<THandlePair<HandleMut, HandleMut>*>(this);
      }

      /// Pick a specific dimension                                           
      template<Cid SID>
      constexpr decltype(auto) PickDimension(this auto&& self) noexcept {
         if constexpr      (SID == 0)  return self.GetKeyHandle();
         else if constexpr (SID == 1)  return self.GetValHandle();
         else static_assert(false, "No such dimension");
      }
   };
   

   /// MARK: HMut/HMut                                                        
   ///                                                                        
   /// Type-erased mutable handle                                             
   ///                                                                        
   template<>
   struct THandlePair<HandleMut, HandleMut> : Com::Container<
      Com::Multitype<Com::TypedStack<DMeta, void, false, 0>,
                     Com::TypedStack<DMeta, void, false, 1>>,
      Com::Multiprovider<Com::HeapReference<HeapEntry<0>>,
                         Com::HeapReference<HeapEntry<1>>>,
      Com::CountStatic<1u, 0, 1>,
      Com::ReserveStatic<1u, 0, 1>,
      Com::MultiownDeep<Com::OwnershipDeepReference<Com::WeakOwnership, true, 0>,
                        Com::OwnershipDeepReference<Com::WeakOwnership, true, 1>>,
      Com::HashEmergent<0, Hash, 1>,
      Com::Assignment<false, 0, 1>,
      Com::Emplacement<0, 1>,
      Com::Comparison<false, true, 0, 1>,
      Com::IterationOperators<0, 1>
   > {
      using CTTI_Deep      = Yup;
      using CTTI_Handle    = Yup;
      using CTTI_Pair      = Yup;
      using CTTI_ReflectAs = void;

      using DeepType  = HandleDisowned; // TODO why disowned?
      using KeyHandle = HandleMut;
      using ValHandle = HandleMut;

      /// Handles can't be piecewise-initialized                              
      //THandlePair(Inner::Piecewise, auto&&) = delete;

      constexpr THandlePair() noexcept {
         this->ConstructDefault();
      }

      constexpr THandlePair(THandlePair const& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr THandlePair(THandlePair&& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr THandlePair(CT::Map auto&& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr THandlePair(CT::Pair auto&& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr THandlePair(HandleMut&& key, HandleMut&& val) noexcept {
         this->Com::HeapReference<HeapEntry<0>>::SetHeapInner(key.GetHeapInner());
         this->Com::HeapReference<HeapEntry<1>>::SetHeapInner(val.GetHeapInner());
         this->Com::OwnershipDeepReference<Com::WeakOwnership, true, 0>::SetEntriesInner(key.GetEntriesInner());
         this->Com::OwnershipDeepReference<Com::WeakOwnership, true, 1>::SetEntriesInner(val.GetEntriesInner());
         this->DeduceType(key, val);
         //this->Com::TypedStack<DMeta, void, false, 0>::SetTypeInner(key.GetTypeInner());
         //this->Com::TypedStack<DMeta, void, false, 1>::SetTypeInner(val.GetTypeInner());
      }

      /// Assignment is disabled                                              
      THandlePair& operator = (THandlePair const&) = default;
      THandlePair& operator = (THandlePair&&) = default;

      HandleMut GetKey()       const noexcept { return {*this}; }
      HandleMut GetKeyHandle() const noexcept { return {*this}; }
      
      HandleMut GetVal()       const noexcept { return {Slice<1>, *this}; }
      HandleMut GetValHandle() const noexcept { return {Slice<1>, *this}; }

      /// Already as mutable as it gets                                       
      auto& ForceMutable() noexcept { return *this; }

      /// Pick a specific dimension                                           
      template<Cid SID>
      constexpr decltype(auto) PickDimension(this auto&& self) noexcept {
         if constexpr      (SID == 0)  return self.GetKeyHandle();
         else if constexpr (SID == 1)  return self.GetValHandle();
         else static_assert(false, "No such dimension");
      }
   };
   

   /// MARK: H/HMut                                                           
   ///                                                                        
   /// Type-erased immutable key paired with mutable value.                   
   /// Often used for mutable access in maps, where keys can't be modified.   
   template<>
   struct THandlePair<Handle, HandleMut> : Com::Container<
      Com::Multitype<Com::TypedStack<DMeta, void, false, 0>,
                     Com::TypedStack<DMeta, void, false, 1>>,
      Com::Multiprovider<Com::HeapReference<HeapEntry<0>>,
                         Com::HeapReference<HeapEntry<1>>>,
      Com::CountStatic<1u, 0, 1>,
      Com::ReserveStatic<1u, 0, 1>,
      Com::MultiownDeep<Com::OwnershipDeepReference<Com::WeakOwnership, true, 0>,
                        Com::OwnershipDeepReference<Com::WeakOwnership, true, 1>>,
      Com::HashEmergent<0, Hash, 1>,
      Com::Assignment<false, 1>,
      Com::Emplacement<1>,
      Com::Comparison<false, true, 0, 1>,
      Com::IterationOperators<0, 1>
   > {
      using CTTI_Deep      = Yup;
      using CTTI_Handle    = Yup;
      using CTTI_Pair      = Yup;
      using CTTI_ReflectAs = void;

      using DeepType  = HandleDisowned; // TODO why disowned?
      using KeyHandle = Handle;
      using ValHandle = HandleMut;

      /// Handles can't be piecewise-initialized                              
      //THandlePair(Inner::Piecewise, auto&&) = delete;

      constexpr THandlePair() noexcept {
         this->ConstructDefault();
      }

      constexpr THandlePair(THandlePair const& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr THandlePair(THandlePair&& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr THandlePair(CT::Map auto&& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr THandlePair(CT::Pair auto&& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr THandlePair(Handle&& key, HandleMut&& val) noexcept {
         this->Com::HeapReference<HeapEntry<0>>::SetHeapInner(key.GetHeapInner());
         this->Com::HeapReference<HeapEntry<1>>::SetHeapInner(val.GetHeapInner());
         this->Com::OwnershipDeepReference<Com::WeakOwnership, true, 0>::SetEntriesInner(key.GetEntriesInner());
         this->Com::OwnershipDeepReference<Com::WeakOwnership, true, 1>::SetEntriesInner(val.GetEntriesInner());
         this->DeduceType(key, val);
         //this->Com::TypedStack<DMeta, void, false, 0>::SetTypeInner(key.GetTypeInner());
         //this->Com::TypedStack<DMeta, void, false, 1>::SetTypeInner(val.GetTypeInner());
      }

      /// Assignment is disabled                                              
      THandlePair& operator = (THandlePair const&) = default;
      THandlePair& operator = (THandlePair&&) = default;

      Handle    GetKey()       const noexcept { return {*this}; }
      Handle    GetKeyHandle() const noexcept { return {*this}; }
      
      HandleMut GetVal()       const noexcept { return {Slice<1>, *this}; }
      HandleMut GetValHandle() const noexcept { return {Slice<1>, *this}; }

      /// Force the handle to become mutable, so that we have methods like    
      /// emplacement in constructors.                                        
      auto& ForceMutable() noexcept {
         return *reinterpret_cast<THandlePair<HandleMut, HandleMut>*>(this);
      }
 
      /// Pick a specific dimension                                           
      template<Cid SID>
      constexpr decltype(auto) PickDimension(this auto&& self) noexcept {
         if constexpr      (SID == 0)  return self.GetKeyHandle();
         else if constexpr (SID == 1)  return self.GetValHandle();
         else static_assert(false, "No such dimension");
      }
   };

#if not LANGULUS(FORCE_TYPE_ERASURE)
   /// MARK: HE/HE                                                            
   ///                                                                        
   /// Statically typed emergent handles                                      
   ///                                                                        
   template<CT::Reference K, CT::Reference V> requires CT::NotSheddable<K, V>
   struct THandlePair<THandleEmergent<K>, THandleEmergent<V>> : Com::Container<
      Com::Multitype<Com::TypedStatic<DMeta, Deref<K>, 0>,
                     Com::TypedStatic<DMeta, Deref<V>, 1>>,
      Com::Multiprovider<Com::HeapReference<HeapEntry<0, Deref<K>*>>,
                         Com::HeapReference<HeapEntry<1, Deref<V>*>>>,
      Com::CountStatic<1u, 0, 1>,
      Com::ReserveStatic<1u, 0, 1>,
      Com::Multiown    <EnableComponentIf<CT::Dense<K>,  Com::OwnershipEmergent<Com::WeakOwnership, 0>>,
                        EnableComponentIf<CT::Dense<V>,  Com::OwnershipEmergent<Com::WeakOwnership, 1>>>,
      Com::MultiownDeep<EnableComponentIf<CT::Sparse<K>, Com::OwnershipDeepEmergent<Com::WeakOwnership, true, 0>>,
                        EnableComponentIf<CT::Sparse<V>, Com::OwnershipDeepEmergent<Com::WeakOwnership, true, 1>>>,
      Com::HashEmergent<0, Hash, 1>,
      Com::Comparison<false, true, 0, 1>,
      Com::IterationOperators<0, 1>
   > {
      using CTTI_Deep      = Yup;
      using CTTI_Handle    = Yup;
      using CTTI_Pair      = Yup;
      using CTTI_ReflectAs = void;

      using Denser    = THandlePair<typename THandleEmergent<K>::Denser, typename THandleEmergent<V>::Denser>;
      using DeepType  = HandleDisowned; // TODO why disowned?
      using KeyHandle = THandleEmergent<K>;
      using ValHandle = THandleEmergent<V>;

      /// Handles can't be piecewise-initialized                              
      //THandlePair(Inner::Piecewise, auto&&) = delete;

      constexpr THandlePair() noexcept {
         this->ConstructDefault();
      }

      constexpr THandlePair(THandlePair const& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr THandlePair(THandlePair&& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr THandlePair(CT::Map auto&& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr THandlePair(CT::Pair auto&& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr THandlePair(CT::Handle auto&& key, CT::Handle auto&& val) noexcept {
         this->Com::HeapReference<HeapEntry<0, Deref<K>*>>::SetHeapInner(DeintCast(key).GetHeapInner());
         this->Com::HeapReference<HeapEntry<1, Deref<V>*>>::SetHeapInner(DeintCast(val).GetHeapInner());
      }

      /// Assignment is disabled                                              
      THandlePair& operator = (THandlePair const&) = default;
      THandlePair& operator = (THandlePair&&) = default;

      decltype(auto) GetKey() const noexcept {
         if constexpr (CT::Constant<K>)
            return *this->Com::HeapReference<HeapEntry<0, Deref<K>*>>::Get();
         else
            return GetKeyHandle();
      }

      KeyHandle GetKeyHandle() const noexcept { return {*this}; }

      decltype(auto) GetVal() const noexcept {
         if constexpr (CT::Constant<V>)
            return *this->Com::HeapReference<HeapEntry<1, Deref<V>*>>::Get();
         else
            return GetValHandle();
      }

      ValHandle GetValHandle() const noexcept { return {Slice<1>, *this}; }

      /// Force the handle to become mutable, so that we have methods like    
      /// emplacement in constructors.                                        
      auto& ForceMutable() noexcept {
         return *reinterpret_cast<THandlePair<THandleEmergent<DecvqAll<K>&>,
                                              THandleEmergent<DecvqAll<V>&>>*>(this);
      }

      /// Pick a specific dimension                                           
      template<Cid SID>
      constexpr decltype(auto) PickDimension(this auto&& self) noexcept {
         if constexpr      (SID == 0) return self.GetKeyHandle();
         else if constexpr (SID == 1) return self.GetValHandle();
         else static_assert(false, "No such dimension");
      }
   };


   /// MARK: HT/HT                                                            
   ///                                                                        
   /// Statically typed embedded handles                                      
   ///                                                                        
   template<CT::Reference K, CT::Reference V> requires CT::NotSheddable<K, V>
   struct THandlePair<THandle<K>, THandle<V>> : Com::Container<
      Com::Multitype<Com::TypedStatic<DMeta, Deref<K>, 0>,
                     Com::TypedStatic<DMeta, Deref<V>, 1>>,
      Com::Multiprovider<Com::HeapReference<HeapEntry<0, Deref<K>*>>,
                         Com::HeapReference<HeapEntry<1, Deref<V>*>>>,
      Com::CountStatic<1u, 0, 1>,
      Com::ReserveStatic<1u, 0, 1>,
      Com::Multiown    <EnableComponentIf<CT::Dense<K>,  Com::OwnershipStack<Com::WeakOwnership, 0>>,
                        EnableComponentIf<CT::Dense<V>,  Com::OwnershipStack<Com::WeakOwnership, 1>>>,
      Com::MultiownDeep<EnableComponentIf<CT::Sparse<K>, Com::OwnershipDeepReference<Com::WeakOwnership, true, 0>>,
                        EnableComponentIf<CT::Sparse<V>, Com::OwnershipDeepReference<Com::WeakOwnership, true, 1>>>,
      Com::HashEmergent<0, Hash, 1>,
      Com::Assignment<false, 0, 1>,
      Com::Emplacement<0, 1>,
      Com::Comparison<false, true, 0, 1>,
      Com::IterationOperators<0, 1>
   > {
      using CTTI_Deep      = Yup;
      using CTTI_Handle    = Yup;
      using CTTI_Pair      = Yup;
      using CTTI_ReflectAs = void;

      using Denser    = THandlePair<typename THandle<K>::Denser, typename THandle<V>::Denser>;
      using DeepType  = HandleDisowned; // TODO why disowned?
      using KeyHandle = THandle<K>;
      using ValHandle = THandle<V>;

      static constexpr bool ReferenceElements = true;

      /// Handles can't be piecewise-initialized                              
      //THandlePair(Inner::Piecewise, auto&&) = delete;

      constexpr THandlePair() noexcept {
         this->ConstructDefault();
      }

      constexpr THandlePair(THandlePair const& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr THandlePair(THandlePair&& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr THandlePair(CT::Map auto&& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr THandlePair(CT::Pair auto&& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr THandlePair(THandle<K>&& key, THandle<V>&& val) noexcept {
         this->Com::HeapReference<HeapEntry<0, Deref<K>*>>::SetHeapInner(key.GetHeapInner());
         this->Com::HeapReference<HeapEntry<1, Deref<V>*>>::SetHeapInner(val.GetHeapInner());

         if constexpr (CT::Sparse<K>)
            this->Com::OwnershipDeepReference<Com::WeakOwnership, true, 0>::SetEntriesInner(key.GetEntries());

         if constexpr (CT::Sparse<V>)
            this->Com::OwnershipDeepReference<Com::WeakOwnership, true, 1>::SetEntriesInner(val.GetEntries());
      }

      /// Assignment is disabled                                              
      THandlePair& operator = (THandlePair const&) = default;
      THandlePair& operator = (THandlePair&&) = default;

      decltype(auto) GetKey() const noexcept {
         if constexpr (CT::Constant<K>)
            return *this->Com::HeapReference<HeapEntry<0, Deref<K>*>>::Get();
         else
            return GetKeyHandle();
      }

      KeyHandle GetKeyHandle() const noexcept { return {*this}; }

      decltype(auto) GetVal() const noexcept {
         if constexpr (CT::Constant<V>)
            return *this->Com::HeapReference<HeapEntry<1, Deref<V>*>>::Get();
         else
            return GetValHandle();
      }

      ValHandle GetValHandle() const noexcept { return {Slice<1>, *this}; }

      /// Force the handle to become mutable, so that we have methods like    
      /// emplacement in constructors.                                        
      auto& ForceMutable() noexcept {
         return *reinterpret_cast<THandlePair<THandle<DecvqAll<K>&>,
                                              THandle<DecvqAll<V>&>>*>(this);
      }

      /// Pick a specific dimension                                           
      template<Cid SID>
      constexpr decltype(auto) PickDimension(this auto&& self) noexcept {
         if constexpr      (SID == 0)  return self.GetKeyHandle();
         else if constexpr (SID == 1)  return self.GetValHandle();
         else static_assert(false, "No such dimension");
      }
   };


   /// MARK: HTL/HTL                                                          
   ///                                                                        
   /// Statically typed local handles                                         
   ///                                                                        
   template<CT::NotReference K, CT::NotReference V> requires (CT::NotSheddable<K, V> and CT::NotHandle<K, V>)
   struct THandlePair<THandle<K>, THandle<V>> : Com::Container<
      Com::Multitype<Com::TypedStatic<DMeta, Deref<K>, 0>,
                     Com::TypedStatic<DMeta, Deref<V>, 1>>,
      Com::Multiprovider<EnableComponentIf<CT::Dense<K>,  Com::Stack<K, 0>>,
                         EnableComponentIf<CT::Dense<V>,  Com::Stack<V, 1>>,
                         EnableComponentIf<CT::Sparse<K>, Com::HeapMovable<0, 0, HeapEntry<0, K*>>>,
                         EnableComponentIf<CT::Sparse<V>, Com::HeapMovable<0, 0, HeapEntry<1, V*>>>>,
      Com::CountStatic<1u, 0, 1>,
      Com::ReserveStatic<1u, 0, 1>,
      Com::Multiown    <EnableComponentIf<CT::Sparse<K>, Com::OwnershipStack<Com::StrongOwnership, 0>>,
                        EnableComponentIf<CT::Sparse<V>, Com::OwnershipStack<Com::StrongOwnership, 1>>>,
      Com::MultiownDeep<EnableComponentIf<CT::Sparse<K>, Com::OwnershipDeepHeap<Com::StrongOwnership, true, 0>>,
                        EnableComponentIf<CT::Sparse<V>, Com::OwnershipDeepHeap<Com::StrongOwnership, true, 1>>>,
      Com::HashEmergent<0, Hash, 1>,
      Com::Assignment<false, 0, 1>,
      Com::Emplacement<0, 1>,
      Com::Comparison<false, true, 0, 1>
   > {
      using CTTI_Deep      = Yup;
      using CTTI_Handle    = Yup;
      using CTTI_Pair      = Yup;
      using CTTI_ReflectAs = void;

      using Denser    = THandlePair<typename THandle<K>::Denser, typename THandle<V>::Denser>;
      using DeepType  = HandleDisowned; // TODO why disowned?
      using KeyHandle = Tif<CT::Sparse<K>, THandle<K&>, THandleEmergent<K&>>;
      using ValHandle = Tif<CT::Sparse<V>, THandle<V&>, THandleEmergent<V&>>;

      static constexpr bool ReferenceElements = true;

      constexpr THandlePair() noexcept {
         this->ConstructDefault();
      }

      constexpr THandlePair(THandlePair const& other) {
         this->Absorb(Refer(other));
      }

      constexpr THandlePair(THandlePair&& other) noexcept {
         this->Absorb(Move(other));
      }

      /// Local dense handles have a very specific kind of absorption:        
      /// Instead of directly absorbing the container, we use the stack,      
      /// and transfer each element with the desired intent.                  
      constexpr THandlePair(Inner::Absorb, CT::Pair auto&& other) noexcept {
         if (not DeintCast(other).IsEmpty()) {
            Com::Emplacement<0, 1>::template EmplaceConstruct<0>(LglsFwd(other));
            Com::Emplacement<0, 1>::template EmplaceConstruct<1>(LglsFwd(other));
         }
         else this->ConstructDefault();
      }

      constexpr THandlePair(NotTag auto&& a1, auto&& a2) noexcept
         : THandlePair {Piecewise, LglsFwd(a1), LglsFwd(a2)} {}

      constexpr THandlePair(Inner::Piecewise, auto&& a1, auto&& a2) noexcept {
         Com::Emplacement<0, 1>::template EmplaceConstruct<0>(LglsFwd(a1));
         Com::Emplacement<0, 1>::template EmplaceConstruct<1>(LglsFwd(a2));
      }

      constexpr ~THandlePair() noexcept {
         this->Destroy();
      }

      /// Assignment is disabled                                              
      THandlePair& operator = (THandlePair const&) = default;
      THandlePair& operator = (THandlePair&&) = default;

      auto GetKey()       const noexcept -> KeyHandle { return {*this}; }
      auto GetKeyHandle() const noexcept -> KeyHandle { return {*this}; }
      
      auto GetVal()       const noexcept -> ValHandle { return {Slice<1>, *this}; }
      auto GetValHandle() const noexcept -> ValHandle { return {Slice<1>, *this}; }

      /// Force the handle to become mutable, so that we have methods like    
      /// emplacement in constructors.                                        
      auto& ForceMutable() noexcept {
         return *reinterpret_cast<THandlePair<THandle<DecvqAll<K>>,
                                              THandle<DecvqAll<V>>>*>(this);
      }

      /// Pick a specific dimension                                           
      template<Cid SID>
      constexpr decltype(auto) PickDimension(this auto&& self) noexcept {
         if constexpr      (SID == 0)  return self.GetKeyHandle();
         else if constexpr (SID == 1)  return self.GetValHandle();
         else static_assert(false, "No such dimension");
      }
   };

   template<CT::Handle K, CT::Handle V>
   THandlePair(K&&, V&&) -> THandlePair<Decay<K>, Decay<V>>;
#endif
}
