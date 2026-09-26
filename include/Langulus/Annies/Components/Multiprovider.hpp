///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Component.hpp"
#include <Langulus/IntentOf.hpp>


namespace Langulus::Annies
{
   struct HandleDisowned;
}

LglsDisableWarningPush
LglsDisableWarning_UnusedLocalTypedef

namespace Langulus::Annies::Component
{
   template<class...> struct Multiprovider;

   template<CT::Component...TN> requires (CountEnabled<TN...> == 0)
   struct Multiprovider<TN...> {
      using CTTI_Component = Yup;
      static constexpr bool SkipThisComponent = true;
   };

   ///                                                                        
   /// Combines multiple heap/stack components into a unified interface to    
   /// combat C++ base method ambiguities, and to add a bit more convenience. 
   ///   @tparam TN... all the heap/stack components to unify                 
   template<CT::Component...TN> requires (CountEnabled<TN...> >= 2)
   struct LANGULUS_EBCO Multiprovider<TN...> : TN... {
      using CTTI_Component = Yup;
      using CTTI_ReflectAs = void;
      using Subcomponents  = decltype(Discard(Types<TN...>{}, []<class C> static { return requires { C::SkipThisComponent; }; }));
      using Id             = decltype(Extract(Subcomponents{}, []<class C> static { return typename C::Id{}; }));

      static_assert(ForEachIndexedAnd(Subcomponents{}, []<class C, size_t I> {
         return C::Id::Count == 1 and C::Id::First == I; }),
         "Each enabled subcomponent needs to be dedicated to their single dimension, "
         "and all subcomponents need to be sequential"
      );

      static constexpr int ComponentPrecedence = -2000;
      static_assert(ForEachAnd(Subcomponents{}, []<class C> { return C::ComponentPrecedence == -2000; }),
         "All precedences should match");

      static constexpr bool HeapCanBeNull = ForEachOr(Subcomponents{}, []<class C> { return C::HeapCanBeNull; });
      static constexpr bool Reallocatable = ForEachOr(Subcomponents{}, []<class C> { return C::Reallocatable; });

      #define if_inherits(...) requires requires { self.C::__VA_ARGS__; }

      /// Get a direct access to the heap memory                              
      ///   @attention using raw pointer while self.IsEmpty() may lead to     
      ///      undefined behavior                                             
      template<Cid SID = 0>
      constexpr auto GetRaw(this auto&& self) noexcept {
         using C = typename Subcomponents::template At<SID>;
         return self.C::GetRaw();
      }
      
      /// Get a direct access to the heap memory as a different type          
      ///   @attention using raw pointer while self.IsEmpty() may lead to     
      ///      undefined behavior                                             
      template<class T, Cid SID = 0>
      constexpr auto GetRawAs(this auto&& self) noexcept {
         using C = typename Subcomponents::template At<SID>;
         return self.C::template GetRawAs<T>();
      }

      /// Get a direct access to the initialized heap memory's end.           
      ///   @attention this makes sense only when provider is contiguous.     
      template<Cid SID = 0>
      constexpr auto GetRawEnd(this auto&& self) noexcept {
         using C = typename Subcomponents::template At<SID>;
         return self.C::GetRawEnd();
      }
    
      /// Get a direct access to the entire heap reserve's end.               
      template<Cid SID = 0>
      constexpr auto GetRawReserveEnd(this auto&& self) noexcept {
         using C = typename Subcomponents::template At<SID>;
         return self.C::GetRawReserveEnd();
      }
      
      /// Get pointer to the first element for the given dimension.           
      /// This is a lower-level routine that does only sparseness checking.   
      /// No conversion or copying occurs, only pointer arithmetic.           
      ///   @attention no type-safety                                         
      ///   @attention assumes the container is typed                         
      ///   @attention assumes the container has valid memory                 
      ///   @tparam AS the type of data we're accessing - use void to use the 
      ///      type of the container, if statically typed                     
      ///   @tparam SID can be used to access specific dimension              
      ///   @return pointer to the first element of the desired dimension     
      template<class AS = void, Cid SID = 0>
      auto* Get(this auto&& self) assumptious {
         using C = typename Subcomponents::template At<SID>;
         return self.C::template Get<AS>();
      }

      /// Get first element as a handle, or any desired wrapping type.        
      /// Conversion or copying may occur, depending on type.                 
      ///   @attention will throw if incompatible type is provided            
      ///   @tparam AS the type we're wrapping in                             
      ///   @tparam SID can be used to access specific dimension              
      ///   @return the element, as a reference if possible                   
      template<CT::NotVoid AS, Cid SID = 0>
      decltype(auto) As(this auto&& self) {
         using C = typename Subcomponents::template At<SID>;
         return self.C::template As<AS>();
      }

      /// A safe way to get the first sparse entry after being resolved to    
      /// the most concrete type. Available only if container has DeepType.   
      ///   @return the most concrete representation of the first item        
      ///   @note defined in Handle.hpp because it requires HandleDisowned    
      template<Cid SID = 0>
      auto GetResolved(this auto&& self) -> HandleDisowned;

      /// Get first element, removing 'count' indirections                    
      ///   @attention throws if type is incomplete and origin was reached    
      ///   @tparam SID can be used to access specific dimension              
      ///   @tparam AS specify the type we wrap the result in.                
      ///      Using 'void' will default to C::DeepType.                      
      ///   @param count how many levels of indirection to remove?            
      ///   @return the dense first element for chosen dimension              
      ///   @note defined in Handle.hpp because it requires HandleDisowned    
      template<Cid SID = 0>
      auto GetDense(this auto&& self, size_t count = -1) -> HandleDisowned;

   protected:
      LglsComIterationOperators(friend);
      LglsComReserveEmergent(friend);
      LglsComInsertion(friend);
      LglsComMerging(friend);
      LglsComEmplacement(friend);
      LglsComConversion(friend);
      LglsComOwnershipEmergent(friend);
      
      /// Get the heap pointer (inner)                                        
      template<Cid SID = 0>
      constexpr auto& GetHeapInner(this auto&& self) noexcept {
         using C = typename Subcomponents::template At<SID>;
         return self.C::GetHeapInner();
      }

      /// Get a direct access to the heap memory                              
      ///   @attention using raw pointer while self.IsEmpty() may lead to     
      ///      undefined behavior                                             
      template<Cid SID = 0>
      constexpr void* GetRawVoid(this auto&& self) noexcept {
         using C = typename Subcomponents::template At<SID>;
         return self.C::GetRawVoid();
      }

      /// Set the heap pointer, any data pointer will do                      
      template<Cid SID = 0, class C = typename Subcomponents::template At<SID>>
      constexpr void SetHeapInner(this auto& self, CT::Sparse auto heap) assumptious 
      if_inherits(SetHeapInner(nullptr)) {
         self.C::SetHeapInner(heap);
      }

      /// Reset the heap pointer to null                                      
      template<Cid SID = 0, class C = typename Subcomponents::template At<SID>>
      constexpr void SetHeapInner(this auto& self, nullptr_t) noexcept 
      if_inherits(SetHeapInner(nullptr)) {
         self.C::SetHeapInner(nullptr);
      }
      
      /// Get a size based on reflected allocation page and count.            
      /// This will allocate memory for relevant headers, footers, and types  
      /// across all dimensions used in this heap component.                  
      ///   @param reserve the number of elements to request                  
      template<Cid SID = 0, class C = typename Subcomponents::template At<SID>>
      constexpr auto RequestHeap(this auto const& self, size_t reserve) assumptious
      -> Request if_inherits(RequestHeap(reserve)) {
         return self.C::RequestHeap(reserve);
      }

      /// Default-initialize the heap pointer                                 
      template<class SELF>
      constexpr void ConstructDefault([[maybe_unused]] this SELF& self) noexcept {
         ForEach(Subcomponents{}, [&]<class C> noexcept {
            if_available_gcc(C::template ConstructDefault<SELF>)();
         });
      }
      
      /// Transfer from any kind of container, respecting intents             
      ///   @param intent The intent and container to transfer from.          
      ///   @param reserve Optional reserve override, which is taken into     
      ///      account only when we're cloning or copying, as only then       
      ///      a new allocation occurs.                                       
      template<class SELF, CT::Intent I> requires CT::Container<I>
      void ConstructFrom([[maybe_unused]] this SELF& self, I&& intent, size_t reserve = 0) {
         ForEach(Subcomponents{}, [&]<class C> {
            if_available_gcc(C::template ConstructFrom<SELF, I>)(LglsFwd(intent), reserve);
         });
      }
      
      /// Allocate a fresh allocation                                         
      ///   @attention changes allocation, heap pointer and reserve count only
      ///   @param request request to fulfill                                 
      template<Cid SID = 0, class C = typename Subcomponents::template At<SID>>
      void AllocateFresh(this auto& self, size_t elements)
      if_inherits(AllocateFresh(elements)) {
         self.C::AllocateFresh(elements);
      }

      /// Allocate a number of elements, relying on the type of the container 
      ///   @attention assumes container is typed                             
      ///   @param elements number of elements to allocate                    
      template<Cid SID = 0>
      void AllocateMore(this auto& self, size_t elements) {
         using C = typename Subcomponents::template At<SID>;
         self.C::AllocateMore(elements);
      }

      /// Shrink the block, depending on currently reserved	elements.         
      /// Initialized elements on the back will be destroyed.                 
      /// When MANAGED_MEMORY is enabled we have a strong guarantee that      
      /// allocations never move when shrinking.                              
      ///   @param elements number of elements to reserve                     
      template<Cid SID = 0>
      void AllocateLess(this auto& self, size_t elements) {
         using C = typename Subcomponents::template At<SID>;
         self.C::AllocateLess(elements);
      }

      /// Remap footer requests onto the new reserve                          
      ///   @param newReserved the newly reserved number of elements          
      ///   @attention works on one dimension at a time!                      
      template<Cid SID = 0>
      void RemapAllHeapRequests(this auto& self, size_t newReserved) {
         using C = typename Subcomponents::template At<SID>;
         self.C::RemapAllHeapRequests(newReserved);
      }

      /// Transfer footer requests onto the new reserve and memory            
      ///   @param oldSelf container with the old heap                        
      ///   @param newReserved the newly reserved number of elements          
      ///   @attention works on one dimension at a time!                      
      template<Cid SID = 0, class SELF>
      void TransferAllHeapRequests(this SELF& self, SELF const& oldSelf, size_t newReserved) {
         using C = typename Subcomponents::template At<SID>;
         self.C::TransferAllHeapRequests(oldSelf, newReserved);
      }

      /// Invoked to remedy the situation when element constructors throw     
      ///   @param n the number of elements that were actually initialized    
      template<Cid SID = 0>
      void PartialSuccess(this auto& self, size_t n) {
         using C = typename Subcomponents::template At<SID>;
         self.C::PartialSuccess(n);
      }

      /// Branch out the current container by doing a shallow copy.           
      /// Happens when you try to modify a container with strong ownership    
      /// from somewhere else (when GetUses() > 1). Allocates a fresh         
      /// allocation in the case we haven't allocated anything yet.           
      /// Essentially implements the Copy-On-Write principle.                 
      ///   @param elements usually branching is accompanied by a resize,     
      ///      so specify it here                                             
      template<Cid SID = 0>
      void BranchOut(this auto& self, size_t elements) {
         using C = typename Subcomponents::template At<SID>;
         self.C::BranchOut(elements);
      }

      #undef if_inherits
   };
}

LglsDisableWarningPop
