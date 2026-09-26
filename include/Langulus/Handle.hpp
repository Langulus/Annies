///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
//#include "Langulus/Core.hpp"
//#include "Langulus/Typenav.hpp"
//#include "source/Component.hpp"
#include "Annies/Container.hpp"
#include "Annies/Components/Typed-Stack.hpp"
#include "Annies/Components/Heap-Reference.hpp"
#include "Annies/Components/Count-Static.hpp"
#include "Annies/Components/OwnershipDeep-Reference.hpp"
#include "Annies/Components/Hash-Emergent.hpp"
#include "Annies/Components/Assignment.hpp"
#include "Annies/Components/Emplacement.hpp"
#include "Annies/Components/Comparison.hpp"
#include "Annies/Components/Iteration-Operators.hpp"


namespace Langulus::Annies::Inner
{
   using TypeErasedHandleMut = Com::Container<
      Com::TypedStack<DMeta, void, true>,
      Com::HeapReference<>,
      Com::CountStatic<1u>,
      Com::OwnershipDeepReference<Com::WeakOwnership>,
      Com::HashEmergent<>,
      Com::Assignment<>,
      Com::Emplacement<>,
      Com::Comparison<>,
      Com::IterationOperators<>
   >;

   using TypeErasedHandle = Com::Container<
      Com::TypedStack<DMeta, void, true>,
      Com::HeapReference<>,
      Com::CountStatic<1u>,
      Com::OwnershipDeepReference<Com::WeakOwnership>,
      Com::HashEmergent<>,
      Com::Comparison<>,
      Com::IterationOperators<>
   >;

   using TypeErasedHandleMutDisowned = Com::Container<
      Com::TypedStack<DMeta, void, true>,
      Com::HeapReference<>,
      Com::CountStatic<1u>,
      Com::HashEmergent<>,
      Com::Assignment<>,
      Com::Emplacement<>,
      Com::Comparison<>,
      Com::IterationOperators<>
   >;

   using TypeErasedHandleDisowned = Com::Container<
      Com::TypedStack<DMeta, void, true>,
      Com::HeapReference<>,
      Com::CountStatic<1u>,
      Com::HashEmergent<>,
      Com::Comparison<>,
      Com::IterationOperators<>
   >;
}

#if not LANGULUS(FORCE_TYPE_ERASURE)
#include "Annies/Components/Typed-Static.hpp"
#include "Annies/Components/Stack.hpp"
#include "Annies/Components/Reserve-Emergent.hpp"
#include "Annies/Components/OwnershipDeep-Heap.hpp"
#include <Langulus/CT/Deep.hpp>


namespace Langulus::Annies::Inner
{
   /// Statically typed handle to a dense element held inside a container     
   template<CT::Reference T> requires (CT::Dense<T> and CT::NotSheddable<T> and CT::NotHandle<T>)
   using THandleEmbeddedDense = Com::Container<
      Com::TypedStatic<DMeta, Deref<T>>,
      Com::HeapReference<HeapEntry<0, Deref<T>*>>,
      Com::CountStatic<1u>,
      Com::ReserveEmergent<>,
      Com::OwnershipStack<Com::WeakOwnership>,
      Com::HashEmergent<>,
      Com::Assignment<>,
      Com::Emplacement<>,
      Com::Comparison<>,
      Com::IterationOperators<>
   >;
   
   /// Statically typed handle to a sparse element held inside a container    
   template<CT::Reference T> requires (CT::Sparse<T> and CT::NotSheddable<T> and CT::NotHandle<T>)
   using THandleEmbeddedSparse = Com::Container<
      Com::TypedStatic<DMeta, Deref<T>>,
      Com::HeapReference<HeapEntry<0, Deref<T>*>>,
      Com::CountStatic<1u>,
      Com::OwnershipDeepReference<Com::WeakOwnership>,
      Com::HashEmergent<>,
      Com::Assignment<>,
      Com::Emplacement<>,
      Com::Comparison<>,
      Com::IterationOperators<>
   >;
   
   /// Statically typed handle to a dense element held inside a container     
   template<CT::Reference T> requires (CT::Dense<T> and CT::NotSheddable<T> and CT::NotHandle<T>)
   using THandleEmbeddedDenseEmergent = Com::Container<
      Com::TypedStatic<DMeta, Deref<T>>,
      Com::HeapReference<HeapEntry<0, Deref<T>*>>,
      Com::CountStatic<1u>,
      Com::ReserveEmergent<>,
      Com::OwnershipEmergent<Com::WeakOwnership>,
      Com::HashEmergent<>,
      Com::Assignment<>,
      Com::Emplacement<>,
      Com::Comparison<>,
      Com::IterationOperators<>
   >;

   /// Statically typed handle to a sparse element held inside a container    
   /// (with emergent deep ownership)                                         
   template<CT::Reference T> requires (CT::Sparse<T> and CT::NotSheddable<T> and CT::NotHandle<T>)
   using THandleEmbeddedSparseEmergent = Com::Container<
      Com::TypedStatic<DMeta, Deref<T>>,
      Com::HeapReference<HeapEntry<0, Deref<T>*>>,
      Com::CountStatic<1u>,
      Com::OwnershipDeepEmergent<Com::WeakOwnership>,
      Com::HashEmergent<>,
      Com::Assignment<>,
      Com::Emplacement<>,
      Com::Comparison<>,
      Com::IterationOperators<>
   >;
   
   /// Statically typed handle to a disowned element held inside container    
   template<CT::Reference T> requires (CT::NotSheddable<T> and CT::NotHandle<T>)
   using THandleDisownedEmbedded = Com::Container<
      Com::TypedStatic<DMeta, Deref<T>>,
      Com::HeapReference<HeapEntry<0, Deref<T>*>>,
      Com::CountStatic<1u>,
      Com::HashEmergent<>,
      Com::Assignment<>,
      Com::Emplacement<>,
      Com::Comparison<>,
      Com::IterationOperators<>
   >;
   
   /// Statically typed handle to a local dense value                         
   /// (isomorphic to TOwn)                                                   
   //TODO inherit TOwn from this?
   template</*CT::NotReference*/class T> requires (CT::Dense<T> and CT::NotSheddable<T> and CT::NotHandle<T>)
   using THandleLocalDense = Com::Container<
      Com::TypedStatic<DMeta, Deref<T>>,
      Com::Stack<T>,
      Com::CountStatic<1u>,
      Com::HashEmergent<>,
      Com::Assignment<>,
      Com::Emplacement<>,
      Com::Comparison<>
   >;
   
   /// Statically typed handle to a local sparse value.                       
   ///   @attention this handle is local and has strong ownership!            
   template<CT::NotReference T> requires (CT::Sparse<T> and CT::NotSheddable<T> and CT::NotHandle<T>)
   using THandleLocalSparse = Com::Container<
      Com::TypedStatic<DMeta, T>,
      Com::HeapMovable<0, 0, HeapEntry<0, T*>>,
      Com::CountStatic<1u>,
      Com::ReserveEmergent<>,
      Com::OwnershipStack<>,
      Com::OwnershipDeepHeap<>,
      Com::HashEmergent<>,
      Com::Emplacement<>,
      Com::Assignment<>,
      Com::Comparison<>
   >;
}
#endif

namespace Langulus::Annies
{
   /// MARK: HandleMut                                                        
   ///                                                                        
   /// A type-erased mutable handle with ownership.                           
   /// It refers to a picked element inside a type-erased container.          
   ///   @attention handles are never (de)referenced upon construction and    
   ///      destruction - only on reassignment                                
   struct HandleMut : Inner::TypeErasedHandleMut {
      using CTTI_Deep      = Yup;
      using CTTI_Handle    = Yup;
      using CTTI_ReflectAs = void;
      using DeepType       = HandleDisowned; //TODO why disowned??

      template<CT::Handle, CT::Handle> friend struct THandlePair;

      constexpr HandleMut() noexcept {
         this->ConstructDefault();
      }

      constexpr HandleMut(HandleMut const& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr HandleMut(HandleMut&& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr HandleMut(CT::Container auto&& other) noexcept {
         this->Absorb(Disown(other));
      }

      template<Cid SID>
      constexpr HandleMut(Inner::Slice<SID>, CT::Container auto&& other) noexcept {
         this->template SliceFrom<SID>(Disown(other));
      }

      constexpr HandleMut(Inner::Stackwise, auto&&...arguments) noexcept
         : Inner::TypeErasedHandleMut {Stackwise, LglsFwd(arguments)...} {}

      HandleMut& operator = (HandleMut const&) = default;
      HandleMut& operator = (HandleMut&&) = default;

      /// Force the handle to become mutable, so that we have methods like    
      /// emplacement in constructors.                                        
      auto ForceMutable() noexcept -> HandleMut& {
         return *this;
      }

      /// Pick a specific dimension if handle is complex (like THandlePair).  
      /// In this case it returns itself for dimension #0.                    
      template<Cid SID>
      constexpr decltype(auto) PickDimension(this auto&& self) noexcept {
         static_assert(SID == 0, "No such dimension");
         return LglsFwd(self);
      }
   };
   

   /// MARK: HandleDisownedMut                                                
   ///                                                                        
   /// A type-erased mutable handle without ownership.                        
   /// It refers to a picked element inside a type-erased container.          
   struct HandleDisownedMut : Inner::TypeErasedHandleMutDisowned {
      using CTTI_Deep      = Yup;
      using CTTI_Handle    = Yup;
      using CTTI_ReflectAs = void;
      using DeepType       = HandleDisowned; //TODO why disowned??

      template<CT::Handle, CT::Handle> friend struct THandlePair;

      constexpr HandleDisownedMut() noexcept {
         this->ConstructDefault();
      }

      constexpr HandleDisownedMut(HandleDisownedMut const& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr HandleDisownedMut(HandleDisownedMut&& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr HandleDisownedMut(CT::Container auto&& other) noexcept {
         this->Absorb(Disown(other));
      }

      template<Cid SID>
      constexpr HandleDisownedMut(Inner::Slice<SID>, CT::Container auto&& other) noexcept {
         this->template SliceFrom<SID>(Disown(other));
      }

      constexpr HandleDisownedMut(Inner::Stackwise, auto&&...arguments) noexcept
         : Inner::TypeErasedHandleMutDisowned {Stackwise, LglsFwd(arguments)...} {}

      HandleDisownedMut& operator = (HandleDisownedMut const&) = default;
      HandleDisownedMut& operator = (HandleDisownedMut&&) = default;

      /// Force the handle to become mutable, so that we have methods like    
      /// emplacement in constructors.                                        
      auto ForceMutable() noexcept -> HandleDisownedMut& {
         return *this;
      }

      /// Pick a specific dimension if handle is complex (like THandlePair).  
      /// In this case it returns itself for dimension #0.                    
      template<Cid SID>
      constexpr decltype(auto) PickDimension(this auto&& self) noexcept {
         static_assert(SID == 0, "No such dimension");
         return LglsFwd(self);
      }
   };
   

   /// MARK: Handle                                                           
   ///                                                                        
   /// A type-erased immutable handle with ownership.                         
   /// It refers to a picked element inside a type-erased container.          
   ///   @attention handles are never (de)referenced upon construction and    
   ///      destruction - only on reassignment. Since this handle is not      
   ///      mutable, this isn't possible either, however the handle still     
   ///      carries ownership information, so that it can be used on demand   
   ///      instead of sought from the memory manager every time.             
   struct Handle : Inner::TypeErasedHandle {
      using CTTI_Deep      = Yup;
      using CTTI_Handle    = Yup;
      using CTTI_ReflectAs = void;
      using DeepType       = HandleDisowned; //TODO why disowned???

      template<CT::Handle, CT::Handle> friend struct THandlePair;

      constexpr Handle() noexcept {
         this->ConstructDefault();
      }

      constexpr Handle(Handle const& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr Handle(Handle&& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr Handle(CT::Container auto&& other) noexcept {
         this->Absorb(Disown(other));
      }

      template<Cid SID>
      constexpr Handle(Inner::Slice<SID>, CT::Container auto&& other) noexcept {
         this->template SliceFrom<SID>(Disown(other));
      }

      constexpr Handle(Inner::Stackwise, auto&&...arguments) noexcept
         : Inner::TypeErasedHandle {Stackwise, LglsFwd(arguments)...} {}

      Handle& operator = (Handle const&) = default;
      Handle& operator = (Handle&&) = default;

      /// Force the handle to become mutable, so that we have methods like    
      /// emplacement in constructors.                                        
      auto ForceMutable() noexcept -> HandleMut& {
         return *reinterpret_cast<HandleMut*>(this);
      }

      /// Pick a specific dimension if handle is complex (like THandlePair).  
      /// In this case it returns itself for dimension #0.                    
      template<Cid SID>
      constexpr decltype(auto) PickDimension(this auto&& self) noexcept {
         static_assert(SID == 0, "No such dimension");
         return LglsFwd(self);
      }

      /// Immutable handles are always constant                               
      constexpr bool IsConstant() const noexcept {
         return true;
      }
   };
   

   /// MARK: HandleDisowned                                                   
   ///                                                                        
   /// A type-erased immutable handle without ownership.                      
   /// It refers to a picked element inside a type-erased container.          
   struct HandleDisowned : Inner::TypeErasedHandleDisowned {
      using CTTI_Deep      = Yup;
      using CTTI_Handle    = Yup;
      using CTTI_ReflectAs = void;
      using DeepType       = HandleDisowned;

      template<CT::Handle, CT::Handle> friend struct THandlePair;

      constexpr HandleDisowned() noexcept {
         this->ConstructDefault();
      }

      /// Refer constructor                                                   
      constexpr HandleDisowned(HandleDisowned const& other) noexcept {
         this->Absorb(Disown(other));
      }

      /// Move constructor                                                    
      constexpr HandleDisowned(HandleDisowned&& other) noexcept {
         this->Absorb(Disown(other));
      }

      /// Construction that absorbs the provided container                    
      constexpr HandleDisowned(CT::Container auto&& other) noexcept {
         this->Absorb(Disown(other));
      }

      template<Cid SID>
      constexpr HandleDisowned(Inner::Slice<SID>, CT::Container auto&& other) noexcept {
         this->template SliceFrom<SID>(Disown(other));
      }

      constexpr HandleDisowned(Inner::Stackwise, auto&&...arguments) noexcept
         : Inner::TypeErasedHandleDisowned {Stackwise, LglsFwd(arguments)...} {}

      /// Assignment is disabled                                              
      HandleDisowned& operator = (HandleDisowned const&) = default;
      HandleDisowned& operator = (HandleDisowned&&) = default;

      /// Force the handle to become mutable, so that we have methods like    
      /// emplacement in constructors.                                        
      auto ForceMutable() noexcept -> HandleDisownedMut& {
         return *reinterpret_cast<HandleDisownedMut*>(this);
      }

      /// Pick a specific dimension if handle is complex (like THandlePair).  
      /// In this case it returns itself for dimension #0.                    
      template<Cid SID>
      constexpr decltype(auto) PickDimension(this auto&& self) noexcept {
         static_assert(SID == 0, "No such dimension");
         return LglsFwd(self);
      }
      
      /// Immutable handles are always constant                               
      constexpr bool IsConstant() const noexcept {
         return true;
      }
   };

#if not LANGULUS(FORCE_TYPE_ERASURE)
   /// MARK: THandle                                                          
   ///                                                                        
   /// When T is a reference, then element is embedded inside container       
   ///   @attention memory is never (de)referenced upon construction and      
   ///      destruction - only on reassignment                                
   ///   @tparam T the contained type                                         
   template<CT::Reference T> requires (CT::Dense<T> and CT::NotSheddable<T>)
   struct THandle<T> : Inner::THandleEmbeddedDense<T> {
      using CTTI_Deep      = Yup;
      using CTTI_Handle    = Yup;
      using CTTI_ReflectAs = void;
      using Denser         = THandle;
      using DeepType       = HandleDisowned; //TODO why disowned??

      template<CT::Handle, CT::Handle> friend struct THandlePair;

      constexpr THandle() noexcept {
         this->ConstructDefault();
      }

      constexpr THandle(THandle const& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr THandle(THandle&& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr THandle(CT::Container auto&& other) noexcept {
         this->Absorb(Disown(other));
      }

      template<Cid SID>
      constexpr THandle(Inner::Slice<SID>, CT::Container auto&& other) noexcept {
         this->template SliceFrom<SID>(Disown(other));
      }

      constexpr THandle(Inner::Stackwise, auto&&...arguments) noexcept
         : Inner::THandleEmbeddedDense<T> {Stackwise, LglsFwd(arguments)...} {}

      /// Assignment is disabled                                              
      THandle& operator = (THandle const&) = default;
      THandle& operator = (THandle&&) = default;

      /// Force the handle to become mutable, so that we have methods like    
      /// emplacement in constructors.                                        
      auto ForceMutable() noexcept -> THandle<DecvqAll<T>>& {
         return *reinterpret_cast<THandle<DecvqAll<T>>*>(this);
      }

      /// Pick a specific dimension if handle is complex (like THandlePair).  
      /// In this case it returns itself for dimension #0.                    
      template<Cid SID>
      constexpr decltype(auto) PickDimension(this auto&& self) noexcept {
         static_assert(SID == 0, "No such dimension");
         return LglsFwd(self);
      }
   };
   
   template<CT::Reference T> requires (CT::Sparse<T> and CT::NotSheddable<T>)
   struct THandle<T> : Inner::THandleEmbeddedSparse<T> {
      using CTTI_Deep      = Yup;
      using CTTI_Handle    = Yup;
      using CTTI_ReflectAs = void;
      using Denser         = THandle<Deptr<T>&>;
      using DeepType       = HandleDisowned; //TODO why disowned??

      template<CT::Handle, CT::Handle> friend struct THandlePair;

      constexpr THandle() noexcept {
         this->ConstructDefault();
      }

      constexpr THandle(THandle const& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr THandle(THandle&& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr THandle(CT::Container auto&& other) noexcept {
         this->Absorb(Disown(other));
      }

      template<Cid SID>
      constexpr THandle(Inner::Slice<SID>, CT::Container auto&& other) noexcept {
         this->template SliceFrom<SID>(Disown(other));
      }

      constexpr THandle(Inner::Stackwise, auto&&...arguments) noexcept
         : Inner::THandleEmbeddedSparse<T> {Stackwise, LglsFwd(arguments)...} {}

      /// Assignment is disabled                                              
      THandle& operator = (THandle const&) = default;
      THandle& operator = (THandle&&) = default;

      /// Force the handle to become mutable, so that we have methods like    
      /// emplacement in constructors.                                        
      auto ForceMutable() noexcept -> THandle<DecvqAll<T>>& {
         return *reinterpret_cast<THandle<DecvqAll<T>>*>(this);
      }

      /// Pick a specific dimension if handle is complex (like THandlePair).  
      /// In this case it returns itself for dimension #0.                    
      template<Cid SID>
      constexpr decltype(auto) PickDimension(this auto&& self) noexcept {
         static_assert(SID == 0, "No such dimension");
         return LglsFwd(self);
      }
   };
   
   
   /// MARK: THandleEmergent                                                  
   ///                                                                        
   /// When T is a reference, then element is embedded inside container       
   ///   @attention memory is never (de)referenced upon construction and      
   ///      destruction - only on reassignment                                
   ///   @tparam T the contained type                                         
   template<CT::Reference T> requires (CT::Dense<T> and CT::NotSheddable<T>)
   struct THandleEmergent<T> : Inner::THandleEmbeddedDenseEmergent<T> {
      using CTTI_Deep      = Yup;
      using CTTI_Handle    = Yup;
      using CTTI_ReflectAs = void;
      using Denser         = THandleEmergent;
      using DeepType       = HandleDisowned; //TODO why disowned??

      template<CT::Handle, CT::Handle> friend struct THandlePair;

      constexpr THandleEmergent() noexcept {
         this->ConstructDefault();
      }

      constexpr THandleEmergent(THandleEmergent const& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr THandleEmergent(THandleEmergent&& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr THandleEmergent(CT::Container auto&& other) noexcept {
         this->Absorb(Disown(other));
      }

      template<Cid SID>
      constexpr THandleEmergent(Inner::Slice<SID>, CT::Container auto&& other) noexcept {
         this->template SliceFrom<SID>(Disown(other));
      }

      constexpr THandleEmergent(Inner::Stackwise, auto&&...arguments) noexcept
         : Inner::THandleEmbeddedDenseEmergent<T> {Stackwise, LglsFwd(arguments)...} {}

      /// Assignment is disabled                                              
      THandleEmergent& operator = (THandleEmergent const&) = default;
      THandleEmergent& operator = (THandleEmergent&&) = default;

      /// Force the handle to become mutable, so that we have methods like    
      /// emplacement in constructors.                                        
      auto ForceMutable() noexcept -> THandleEmergent<DecvqAll<T>>& {
         return *reinterpret_cast<THandleEmergent<DecvqAll<T>>*>(this);
      }

      /// Pick a specific dimension if handle is complex (like THandlePair).  
      /// In this case it returns itself for dimension #0.                    
      template<Cid SID>
      constexpr decltype(auto) PickDimension(this auto&& self) noexcept {
         static_assert(SID == 0, "No such dimension");
         return LglsFwd(self);
      }
   };


   template<CT::Reference T> requires (CT::Sparse<T> and CT::NotSheddable<T>)
   struct THandleEmergent<T> : Inner::THandleEmbeddedSparseEmergent<T> {
      using CTTI_Deep      = Yup;
      using CTTI_Handle    = Yup;
      using CTTI_ReflectAs = void;
      using Denser         = THandleEmergent<Deptr<T>&>;
      using DeepType       = HandleDisowned; //TODO why disowned??

      template<CT::Handle, CT::Handle> friend struct THandlePair;

      constexpr THandleEmergent() noexcept {
         this->ConstructDefault();
      }

      constexpr THandleEmergent(THandleEmergent const& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr THandleEmergent(THandleEmergent&& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr THandleEmergent(CT::Container auto&& other) noexcept {
         this->Absorb(Disown(other));
      }

      template<Cid SID>
      constexpr THandleEmergent(Inner::Slice<SID>, CT::Container auto&& other) noexcept {
         this->template SliceFrom<SID>(Disown(other));
      }

      constexpr THandleEmergent(Inner::Stackwise, auto&&...arguments) noexcept
         : Inner::THandleEmbeddedSparseEmergent<T> {Stackwise, LglsFwd(arguments)...} {}

      /// Assignment is disabled                                              
      THandleEmergent& operator = (THandleEmergent const&) = default;
      THandleEmergent& operator = (THandleEmergent&&) = default;

      /// Force the handle to become mutable, so that we have methods like    
      /// emplacement in constructors.                                        
      auto ForceMutable() noexcept -> THandleEmergent<DecvqAll<T>>& {
         return *reinterpret_cast<THandleEmergent<DecvqAll<T>>*>(this);
      }

      /// Pick a specific dimension if handle is complex (like THandlePair).  
      /// In this case it returns itself for dimension #0.                    
      template<Cid SID>
      constexpr decltype(auto) PickDimension(this auto&& self) noexcept {
         static_assert(SID == 0, "No such dimension");
         return LglsFwd(self);
      }
   };
   

   /// MARK: THandleDisowned                                                  
   ///                                                                        
   /// When T is a reference, then element is embedded inside container.      
   /// This handle never propagates or modifies ownership.                    
   ///   @tparam T the contained type                                         
   template<CT::Reference T> requires CT::NotSheddable<T>
   struct THandleDisowned<T> : Inner::THandleDisownedEmbedded<T> {
      using CTTI_Deep      = Yup;
      using CTTI_Handle    = Yup;
      using CTTI_ReflectAs = void;
      using Denser         = THandle<Deptr<T>&>;
      using DeepType       = HandleDisowned;

      template<CT::Handle, CT::Handle> friend struct THandlePair;

      constexpr THandleDisowned() noexcept {
         this->ConstructDefault();
      }

      constexpr THandleDisowned(THandleDisowned const& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr THandleDisowned(THandleDisowned&& other) noexcept {
         this->Absorb(Disown(other));
      }

      constexpr THandleDisowned(CT::Container auto&& other) noexcept {
         this->Absorb(Disown(other));
      }

      template<Cid SID>
      constexpr THandleDisowned(Inner::Slice<SID>, CT::Container auto&& other) noexcept {
         this->template SliceFrom<SID>(Disown(other));
      }

      constexpr THandleDisowned(Inner::Stackwise, auto&&...arguments) noexcept
         : Inner::THandleDisownedEmbedded<T> {Stackwise, LglsFwd(arguments)...} {}

      /// Assignment is disabled                                              
      THandleDisowned& operator = (THandleDisowned const&) = default;
      THandleDisowned& operator = (THandleDisowned&&) = default;

      /// Force the handle to become mutable, so that we have methods like    
      /// emplacement in constructors.                                        
      auto ForceMutable() noexcept -> THandleDisowned<DecvqAll<T>>& {
         return *this;
      }

      /// Pick a specific dimension if handle is complex (like THandlePair).  
      /// In this case it returns itself for dimension #0.                    
      template<Cid SID>
      constexpr decltype(auto) PickDimension(this auto&& self) noexcept {
         static_assert(SID == 0, "No such dimension");
         return LglsFwd(self);
      }
   };
   

   /// MARK: THandle local                                                    
   ///                                                                        
   /// When T is not a reference, then it is not embedded.                    
   /// Such dense handles are similar to TOwn<T> - data is on the stack.      
   ///   @tparam T the contained type                                         
   template<CT::NotReference T> requires (CT::Dense<T> and CT::NotSheddable<T>)
   struct THandle<T> : Inner::THandleLocalDense<T> {
      using CTTI_Deep      = Yup;
      using CTTI_Handle    = Yup;
      using CTTI_ReflectAs = void;
      using Denser         = THandle<T&>; // avoids nested local handles (and thus copies) by adding a reference
      using DeepType       = HandleDisowned; //TODO why disowned??
      using Base           = typename Inner::THandleLocalDense<T>::Base;

      template<CT::Handle, CT::Handle> friend struct THandlePair;

      constexpr THandle() noexcept = default;

      /// Absorb constructors                                                 
      constexpr THandle(THandle const& other) {
         this->Absorb(Refer(other));
      }

      constexpr THandle(THandle&& other) noexcept {
         this->Absorb(Move(other));
      }

      /// Local dense handles have a very specific kind of absorption:        
      /// Instead of directly absorbing the container, we use the stack,      
      /// and transfer the first element with the desired intent.             
      constexpr THandle(Inner::Absorb, CT::Container auto&& other) {
         if (not DeintCast(other).IsEmpty())
            this->EmplaceConstruct(IntentOf(other) {DeintCast(other).GetHandle()});
         else
            this->ConstructDefault();
      }

      /// Piecewise constructors                                              
      /// (for local dense handles, piecewise == stackwise)                   
      constexpr THandle(Inner::Stackwise, auto&& a)
      requires requires { T{LglsFwd(a)}; }
         : Base {Stackwise, LglsFwd(a)} {}

      constexpr THandle(Inner::Stackwise, CT::Intent auto&& a)
      requires (not requires { T{LglsFwd(a)}; })
         : Base {Stackwise, DeintCast(a)} {}

      constexpr THandle(Inner::Piecewise, auto&& a)
      requires requires { T{LglsFwd(a)}; }
         : Base {Stackwise, LglsFwd(a)} {}

      constexpr THandle(Inner::Piecewise, CT::Intent auto&& a)
      requires (not requires { T{LglsFwd(a)}; })
         : Base {Stackwise, DeintCast(a)} {}

      template<Disambiguate ALT_T> requires (not CT::DeepDense<ALT_T>)
      constexpr THandle(ALT_T&& a) requires requires { T{LglsFwd(a)}; }
         : Base {Stackwise, LglsFwd(a)} {}

      template<Disambiguate ALT_T> requires (CT::Intent<ALT_T> and not CT::DeepDense<ALT_T>)
      constexpr THandle(ALT_T&& a) requires (not requires { T{LglsFwd(a)}; })
         : Base {Stackwise, DeintCast(a)} {}

      constexpr ~THandle() noexcept {
         this->Destroy();
      }

      /// Force the handle to become mutable, so that we have methods like    
      /// emplacement in constructors.                                        
      auto ForceMutable() noexcept -> THandle<DecvqAll<T>>& {
         return *this;
      }

      /// Pick a specific dimension if handle is complex (like THandlePair).  
      /// In this case it returns itself for dimension #0.                    
      template<Cid SID>
      constexpr decltype(auto) PickDimension(this auto&& self) noexcept {
         static_assert(SID == 0, "No such dimension");
         return LglsFwd(self);
      }
   };
   

   ///                                                                        
   /// When T is not a reference, then it is not embedded.                    
   /// Such sparse handles are similar to TRef<Deptr<T>>.                     
   ///   @attention such handles are local and have strong ownership! This    
   ///      means that they need to be cleared of their allocation upon move  
   ///      or abandon!                                                       
   ///   @tparam T the contained sparse type                                  
   template<CT::NotReference T> requires (CT::Sparse<T> and CT::NotSheddable<T>)
   struct THandle<T> : Inner::THandleLocalSparse<T> {
      using CTTI_Deep      = Yup;
      using CTTI_Handle    = Yup;
      using CTTI_ReflectAs = void;
      using Denser         = THandle<Deptr<T>&>; // avoids nested local handles (and thus copies) by adding a reference
      using DeepType       = HandleDisowned; //TODO why disowned??
      using Base           = typename Inner::THandleLocalSparse<T>::Base;

      template<CT::Handle, CT::Handle> friend struct THandlePair;

      constexpr THandle() noexcept {
         this->ConstructDefault();
      }

      /// Absorb constructors                                                 
      constexpr THandle(THandle const& other) {
         this->Absorb(Refer(other));
      }

      constexpr THandle(THandle&& other) noexcept {
         this->Absorb(Move(other));
      }

      /// Local sparse handles have a very specific kind of absorption:       
      /// Instead of directly absorbing the container, we allocate locally,   
      /// and then transfer the first element with the desired intent.        
      constexpr THandle(Inner::Absorb, CT::Container auto&& other) {
         if (not DeintCast(other).IsEmpty())
            this->EmplaceConstruct(IntentOf(other) {DeintCast(other).GetHandle()});
         else
            this->ConstructDefault();
      }

      /// Piecewise constructors                                              
      constexpr THandle(Inner::Piecewise, auto&& pointer) {
         if (DeintCast(pointer))
            this->EmplaceConstruct(LglsFwd(pointer));
         else
            this->ConstructDefault();
      }

      constexpr THandle(Disambiguate auto&& pointer) {
         if (DeintCast(pointer))
            this->EmplaceConstruct(LglsFwd(pointer));
         else
            this->ConstructDefault();
      }
      
      constexpr ~THandle() noexcept {
         this->Destroy();
      }

      /// Force the handle to become mutable, so that we have methods like    
      /// emplacement in constructors.                                        
      auto ForceMutable() noexcept -> THandle<DecvqAll<T>>& {
         return *this;
      }

      /// Pick a specific dimension if handle is complex (like THandlePair).  
      /// In this case it returns itself for dimension #0.                    
      template<Cid SID>
      constexpr decltype(auto) PickDimension(this auto&& self) noexcept {
         static_assert(SID == 0, "No such dimension");
         return LglsFwd(self);
      }
   };
#endif
}


/// Some components need to be aware of HandleDisowned                        
#include "Annies/Components/Heap-Reference.hpp"
#include "Annies/Components/Stack.hpp"
#include "Annies/Components/Multiprovider.hpp"


namespace Langulus::Annies::Component
{
   /// A safe way to get the first sparse entry after being resolved to       
   /// the most concrete type. Available only if container has DeepType.      
   ///   @return the most concrete representation of the first item           
   template<CT::HeapEntry ENTRY0, CT::HeapEntry...ENTRYN>
   template<Cid SID, CT::Contiguous C>
   auto HeapReference<ENTRY0, ENTRYN...>::GetResolved(this C&& self) -> HandleDisowned {
      if (self.template IsEmpty<SID>())
         return {};

      if constexpr (CT::TypeErased<C>) {
         const auto T = self.template GetType<SID>();
         HandleDisowned h {Slice<SID>, self};
         if (not T.IsSparse())
            return h;
      
         const auto resolver = T.GetResolver();
         if (resolver)
            return resolver(h.GetDense().GetRaw());
         else
            return h.GetDense();
      }
      else {
         using T = TypeOf<C, SID>;
         if constexpr (CT::Dense<T>)
            return {Slice<SID>, self};
         else {
            auto& dense_item = DenseCast(self.HeapReference<ENTRY0, ENTRYN...>::template Get<T, SID>());
            if constexpr (CT::Resolvable<Decay<T>>)
               return dense_item.GetResolved();
            else
               return {Stackwise, MetaDataOf<Decay<T>>(), &dense_item};
         }
      }
   }

   /// Get first element, removing 'count' indirections                       
   ///   @attention throws if type is incomplete and origin was reached       
   ///   @tparam SID can be used to access specific dimension                 
   ///   @param count how many levels of indirection to remove?               
   ///   @return the dense first element for chosen dimension                 
   template<CT::HeapEntry ENTRY0, CT::HeapEntry...ENTRYN>
   template<Cid SID, CT::Contiguous C>
   auto HeapReference<ENTRY0, ENTRYN...>::GetDense(this C&& self, size_t count) -> HandleDisowned {
      if (self.template IsEmpty<SID>())
         return {};

      HandleDisowned h {Slice<SID>, self};
      if (not self.template IsSparse<SID>() or count <= 0)
         return h;

      // Check if origin type is complete before attempting anything    
      if constexpr (CT::TypeErased<C>) {
         const auto T = self.template GetType<SID>();
         if (count >= T.GetIndirections()) {
            LglsAssert((bool) T.GetOrigin(),
               "Trying to interface incomplete data `", T,
               "` as dense"
            );
         }
      }
      else {
         using T = TypeOf<C, SID>;
         if (count >= IndirectsOf<T>) {
            LglsAssert(CT::Complete<Decay<T>>,
               "Trying to interface incomplete data `", MetaDataOf<T>(),
               "` as dense"
            );
         }
      }

      auto     T = self.template GetType<SID>();
      auto nextT = T.GetDeptr();
      void* heap = self.HeapReference<ENTRY0, ENTRYN...>::template GetRawVoid<SID>();
      while (count and T.IsSparse()) {            
         if (nextT.IsSparse()) {
            // Pointer T -> Pointer nextT                               
            T.GetDereffer()(heap, &heap);
            T = nextT;
            nextT = T.GetDeptr();
            --count;
         }
         else break;
      }
      return {Stackwise, nextT, UnpackPointer(T, nextT, heap)};
   }

   /// A safe way to get the first sparse entry after being resolved to       
   /// the most concrete type. Available only if container has DeepType.      
   ///   @return the most concrete representation of the first item           
   template<CT::NotVoid T, Cid ID>
   template<Cid SID, CT::Container C>
   auto Stack<T, ID>::GetResolved(this C&& self) -> HandleDisowned {
      if (self.IsEmpty())
         return {};
      
      if (not self.IsSparse())
         return {Slice<SID>, self};

      if constexpr (CT::Resolvable<T>)
         return DenseCast(self.Stack<T, ID>::Get()).GetResolved();
      else
         return {Stackwise, self.template GetType<SID>().GetOrigin(), &DenseCast(self.Stack<T, ID>::Get())};
   }

   /// Get the first contained element, removing 'count' indirections.        
   /// Available only if container has DeepType defined.                      
   ///   @attention throws if type is incomplete and origin was reached       
   ///   @tparam AS specify the type we wrap the result in.                   
   ///      Using 'void' will choose C::DeepType.                             
   ///   @param self deduced this                                             
   ///   @param count how many levels of indirection to remove?               
   ///   @return the dense first element                                      
   template<CT::NotVoid T, Cid ID>
   template<Cid SID, CT::Container C>
   auto Stack<T, ID>::GetDense(this C&& self, size_t count) -> HandleDisowned {
      if (self.IsEmpty())
         return {};

      if (not self.IsSparse() or count <= 0)
         return {Slice<SID>, self};

      // Check if origin type is complete before attempting anything    
      if (count >= IndirectsOf<T>) {
         LglsAssert(CT::Complete<Decay<T>>,
            "Trying to interface incomplete data `", self.GetType(),
            "` as dense"
         );
      }

      void* src = DecvqAllCast(&self.Stack<T, ID>::GetStackInner());
      auto type = self.GetType();
      while (count and type.IsSparse()) {
         auto nextType = type.GetDeptr();
         
         if (nextType.IsSparse()) {
            // Pointer T -> Pointer nextT                               
            type.GetDereffer()(src, &src);
         }
         else {
            // Pointer T -> Dense nextT                                 
            return {Stackwise, nextType, UnpackPointer(type, nextType, src)};
         }

         type = nextType;
         --count;
      }
      
      LglsError("Should never be reached");
      return {};
   }


   /// A safe way to get the first sparse entry after being resolved to       
   /// the most concrete type. Available only if container has DeepType.      
   ///   @return the most concrete representation of the first item           
   template<CT::Component...TN> requires (CountEnabled<TN...> >= 2)
   template<Cid SID>
   auto Multiprovider<TN...>::GetResolved(this auto&& self) -> HandleDisowned {
      using C = typename Subcomponents::template At<SID>;
      return self.C::template GetResolved<SID>();
   }

   /// Get first element, removing 'count' indirections                       
   ///   @attention throws if type is incomplete and origin was reached       
   ///   @tparam SID can be used to access specific dimension                 
   ///   @tparam AS specify the type we wrap the result in.                   
   ///      Using 'void' will default to C::DeepType.                         
   ///   @param count how many levels of indirection to remove?               
   ///   @return the dense first element for chosen dimension                 
   template<CT::Component...TN> requires (CountEnabled<TN...> >= 2)
   template<Cid SID>
   auto Multiprovider<TN...>::GetDense(this auto&& self, size_t count) -> HandleDisowned {
      using C = typename Subcomponents::template At<SID>;
      return self.C::template GetDense<SID>(count);
   }
}