///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Container.hpp"
#include <Langulus/CT/Resolvable.hpp>
#include <Langulus/CT/MinAlloc.hpp>
#include <Langulus/MetaOf.hpp>
#include <Langulus/Utils/Pot.hpp>
#include <Langulus/Allocator.hpp>


namespace Langulus::Annies::Component
{
   /// Refers back to this particular component instance through the deduced  
   /// 'this'. Just for convenience. It is #undef-ed at the end of this file. 
   #define ThisCom self.HeapReference<ENTRY0, ENTRYN...>

   ///                                                                        
   /// Adds a variable to a container that only references a remote heap.     
   /// No allocation interface is provided.                                   
   /// Increases the container's bytesize.                                    
   ///   @tparam ENTRY0 first heap provider                                   
   ///   @tparam ENTRYN optional extensions that include more data into       
   ///      the heap allocation. Each ID must correspond to a matching type   
   ///      component ID. Each entry also allows for pointer customization,   
   ///      including support for packed pointers. Also helps with debugging. 
   ///   @attention only the first ENTRY0::T type is used as a heap reference 
   ///      variable on the stack.                                            
   template<CT::HeapEntry ENTRY0, CT::HeapEntry...ENTRYN>
   struct HeapReference {
      using CTTI_Component = Yup;
      using CTTI_ReflectAs = void;
      using Id             = Values<ENTRY0::Id, ENTRYN::Id...>;
      using HeapProvider   = Id;
      using StackRequest   = typename ENTRY0::T;

      static constexpr bool Shared = sizeof...(ENTRYN) > 0;
      static constexpr int  ComponentPrecedence = -2000;
      static constexpr bool HeapCanBeNull = true;
      static constexpr bool Reallocatable = false;
      /*template<Cid SID>
      static constexpr bool Relevant = Id::template Contains<SID>;*/

   protected:
      LglsComIterationOperators(friend);
      LglsComRemoval(friend);
      LglsComIndexedCommon(friend);
      LglsComIndexedCommonHashed(friend);
      LglsComIndexedLinear(friend);
      LglsComHeapMovable(friend);
      LglsComEmplacement(friend);
      LglsComComparison(friend);
      LglsComConversion(friend);
      LglsComCountStatic(friend);
      LglsComOwnershipEmergent(friend);
      LglsComOwnershipDeepEmergent(friend);
      LglsComHashEmergent(friend);

      template<CT::Container C>
      using Deep = typename Deref<C>::DeepType;

      template<CT::Container C>
      using Count = typename Deref<C>::CountType;

      template<CT::Container C>
      static constexpr auto CountMax = ::std::numeric_limits<Count<C>>::max();

   public:
      /// Get a direct access to the heap memory                              
      ///   @attention using raw pointer while self.IsEmpty() may lead to     
      ///      undefined behavior                                             
      template<Cid SID = Id::First, CT::Container C>// requires Relevant<SID>
      constexpr auto GetRaw(this C&& self) noexcept {
         using Tcvq = LglsMutIf(C, StackRequest);
         if constexpr (SID == Id::First)
            return static_cast<Tcvq>(ThisCom::GetHeapInner());
         else {
            // Each subsequent dimension is located at:                 
            // prev_heap + prev_reserved * sizeof(prev_type)            
            //           + prev_footer                                  
            //           + alignment for next_type                      
            const auto heap     = ThisCom::template GetRawAs<uint8_t, SID - 1>();
            const auto reserved = self.template GetReserved<SID - 1>();
            const auto size     = self.template GetStride<SID - 1>();
            const auto indirect = self.template GetIndirections<SID - 1>();
            const auto footer   = Deref<C>::template DefineHeapFooter<SID - 1>(reserved, indirect);
            const auto align    = self.template GetAlignment<SID>();
            return reinterpret_cast<Tcvq>(
               Align(heap + reserved * size + footer, align)
            );
         }
      }
      
      /// Get a direct access to the heap memory as a different type          
      ///   @attention using raw pointer while self.IsEmpty() may lead to     
      ///      undefined behavior                                             
      template<class T, Cid SID = Id::First, CT::Container C>// requires Relevant<SID>
      constexpr auto GetRawAs(this C&& self) noexcept {
         using Tcvq = LglsMutIf(C, T*);
         return static_cast<Tcvq>(ThisCom::template GetRawVoid<SID>());
      }

      /// Get a direct access to the initialized heap memory's end.           
      ///   @attention this makes sense only when heap is contiguous.         
      template<Cid SID = Id::First, CT::Contiguous C>// requires Relevant<SID>
      constexpr auto GetRawEnd(this C&& self) noexcept {
         if constexpr (CT::TypeErased<C>)
            return ThisCom::template GetRawAs<uint8_t, SID>() + self.template GetBytesize<SID>();
         else
            return ThisCom::template GetRaw<SID>() + self.template GetCount<SID>();
      }
    
      /// Get a direct access to the entire heap reserve's end.               
      template<Cid SID = Id::First, CT::Container C>// requires Relevant<SID>
      constexpr auto GetRawReserveEnd(this C&& self) noexcept {
         const auto reserved = self.template GetReserved<SID>();
         if constexpr (CT::TypeErased<C>)
            return ThisCom::template GetRawAs<uint8_t, SID>() + reserved * self.template GetStride<SID>();
         else
            return ThisCom::template GetRaw<SID>() + reserved;
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
      template<class AS = void, Cid SID = Id::First, CT::Container C>// requires Relevant<SID>
      auto* Get(this C&& self) {
         static_assert(not CT::Handle<AS>,    "AS can't be a handle");
         static_assert(not CT::Reference<AS>, "Strip references first");

         IF_NOT_LANGULUS_FORCE_TYPE_ERASURE(if constexpr (CT::TypeErased<C>) {)
            using TH   = Tif<CT::Void<AS>, void, AS>;
            using THP  = LglsMutIf(C, TH*);
            void* heap = DecvqAllCast(ThisCom::template GetRaw<SID>());
      
            const auto T = self.template GetType<SID>();
            LglsAssumeDev((bool) T, "Block is not typed");

            if constexpr (CT::Void<AS>) {
               // Unknown type, just return the heap pointer            
               return heap;
            }
            else {
               // Casting to a desired runtime type                     
               bool custom_pointer_encountered;
               const auto indirections = T.GetIndirections(custom_pointer_encountered);

               if (indirections == IndirectsOf<TH>) {
                  // No difference in indirections                      
                  return static_cast<THP>(heap);
               }
               else if (indirections > IndirectsOf<TH>) {
                  if (indirections == IndirectsOf<THP> and not custom_pointer_encountered) {
                     // If we're going to add the same pointer later,   
                     // then avoid dereferencing altogether.            
                     // Unfortunately this can't support packed pointers
                     /*LglsAssert(not custom_pointer_encountered, 
                        "Custom pointer detected, use Cast instead of As/Get",
                        ", when trying to convert: ", T, " to ", MetaDataOf<THP>());*/
                     LglsAssumeDev(T.IsSame(MetaDataOf<THP>()), "Type mismatch",
                        ": ", T, " not same as ", MetaDataOf<THP>());
                     return *static_cast<THP*>(heap);
                  }

                  // We need to dereference. Supports packed pointers   
                  auto diff = indirections - IndirectsOf<TH>;
                  auto denser = ThisCom::template GetDense<SID>(diff);
                  return static_cast<THP>(denser.GetRaw());
               }
               else {
                  // We are allowed to add one additional indirection   
                  LglsAssumeDev(indirections + 1 == IndirectsOf<TH>,
                     "Too many indirections");
                  return static_cast<THP>(heap);
               }
            }
         #if not LANGULUS(FORCE_TYPE_ERASURE)
         } else {
            using TC   = LglsMutIf(C, TypeOf<C, SID>);
            using TCP  = LglsMutIf(C, TC*);
            using TH   = Tif<CT::Void<AS>, TC, AS>;
            using THP  = LglsMutIf(C, TH*);
            auto* heap = DecvqAllCast(ThisCom::template GetRaw<SID>());
      
            // Casting to a desired static type                         
            if constexpr (IndirectsOf<TC> == IndirectsOf<TH>) {
               // No difference in indirections                         
               return const_cast<THP>(static_cast<DecvqAll<THP>>(heap));
            }
            else if constexpr (IndirectsOf<TC> > IndirectsOf<TH>) {
               // We need to dereference. Can be done without a         
               // reinterpret_cast, and thus be constexpr-friendly.     
               // Supports packed pointers as well.                     
               return static_cast<THP>(DenseCast<IndirectsOf<TC> - IndirectsOf<TH>>(heap));
            }
            else {
               // We are allowed to add one additional indirection      
               static_assert(IndirectsOf<TCP> == IndirectsOf<TH>,
                  "Too many indirections");
               static_assert(CT::Sparse<TH>,
                  "Casting to a dense shouldn't happen here");
               return static_cast<LglsMutIf(C, TH)>(heap);
            }
         }
         #endif
      }

      template<CT::NotVoid AS>
      auto* GetKey(this auto&& self) requires Shared {
         return ThisCom::template Get<AS, 0>();
      }

      template<CT::NotVoid AS>
      auto* GetVal(this auto&& self) requires Shared {
         return ThisCom::template Get<AS, 1>();
      }

      /// Get first element as a handle, or any desired wrapping type.        
      /// Conversion or copying may occur depending on type.                  
      ///   @attention will throw if incompatible type is provided            
      ///   @tparam AS the type we're wrapping in                             
      ///   @tparam SID can be used to access specific dimension. It is       
      ///      irrelevant if AS is a handle.                                  
      ///   @return the element, as a reference if possible                   
      template<CT::NotVoid AS, Cid SID = Id::First, CT::Contiguous C>// requires Relevant<SID>
      decltype(auto) As(this C&& self) {
         static_assert(not CT::Reference<AS>, "Strip references first");

         if constexpr (CT::Handle<AS>)
            return self.template GetHandle<AS>();
         else {
            // Access directly or wrapped in a container                
            if constexpr (CT::Pair<AS>) {
               // User desires a pair, so we give them a pair           
               static_assert(Shared, "Indexing must be shared to access as a pair");
               using AS1 = TypeOf<AS, 0>;
               using AS2 = TypeOf<AS, 1>;
               return AS {
                  ThisCom::template As<Decvq<Deref<AS1>>, SID + 0>(),
                  ThisCom::template As<Decvq<Deref<AS2>>, SID + 1>()
               };
            }
            else IF_NOT_LANGULUS_FORCE_TYPE_ERASURE(if constexpr (CT::TypeErased<C>)) {
               auto type = self.template GetType<SID>();
               auto requested = MetaDataOf<AS>();
               LglsAssert(type.Is(requested), 
                  "Type mismatch", ": ", type, " not akin to ", requested);

               // Access directly                                       
               if constexpr (CT::Dense<AS> or CT::CustomPointer<AS>)
                  return *ThisCom::template Get<AS, SID>();
               else
                  return ThisCom::template Get<Deptr<AS>, SID>();
            }
            #if not LANGULUS(FORCE_TYPE_ERASURE)
            else {
               using T = TypeOf<C, SID>;

               if constexpr (Akin<T, AS>) {
                  // Access directly                                    
                  if constexpr (CT::Dense<AS> or CT::CustomPointer<AS>)
                     return *ThisCom::template Get<AS, SID>();
                  else
                     return ThisCom::template Get<Deptr<AS>, SID>();
               }
               else {
                  // Wrap in a container                                
                  static_assert(CT::DeepDense<AS>, "Type mismatch");
                  using H = DecideHandle<C>;
                  if constexpr (CT::Pair<H> and not CT::Pair<AS>) {
                     //TODO magic numbers here, use H::PickDimension?
                     if constexpr (SID == 0)
                        return Decvq<AS> {Absorb, ThisCom::template As<typename H::KeyHandle, 0>()};
                     else if constexpr (SID == 1)
                        return Decvq<AS> {Absorb, ThisCom::template As<typename H::ValHandle, 1>()};
                     else
                        static_assert(false, "Unsupported SID");
                  }
                  else return Decvq<AS> {Absorb, ThisCom::template As<H, SID>()};
               }
            }
            #endif
         }
      }

      template<CT::NotVoid AS>
      decltype(auto) KeyAs(this auto&& self) requires Shared {
         return ThisCom::template As<AS, 0>();
      }
      template<CT::NotVoid AS>
      decltype(auto) ValAs(this auto&& self) requires Shared {
         return ThisCom::template As<AS, 1>();
      }

      /// A safe way to get the first sparse entry after being resolved to    
      /// the most concrete type. Available only if container has DeepType.   
      ///   @return the most concrete representation of the first item        
      ///   @note defined in Handle.hpp because it requires HandleDisowned    
      template<Cid SID = Id::First, CT::Contiguous C> //requires Relevant<SID>
      auto GetResolved(this C&& self) -> HandleDisowned;

      /// Get first element, removing 'count' indirections                    
      ///   @attention throws if type is incomplete and origin was reached    
      ///   @tparam SID can be used to access specific dimension              
      ///   @param count how many levels of indirection to remove?            
      ///   @return the dense first element for chosen dimension              
      ///   @note defined in Handle.hpp because it requires HandleDisowned    
      template<Cid SID = Id::First, CT::Contiguous C>// requires Relevant<SID>
      auto GetDense(this C&& self, size_t count = -1) -> HandleDisowned;

   protected:
      /// Get the heap pointer (inner)                                        
      template<Cid SID = Id::First>// requires Relevant<SID>
      constexpr auto& GetHeapInner(this auto&& self) noexcept {
         return self.template AccessStack<HeapReference>();
      }

      /// Get a direct access to the heap memory                              
      ///   @attention using raw pointer while self.IsEmpty() may lead to     
      ///      undefined behavior                                             
      template<Cid SID = Id::First, CT::Container C>// requires Relevant<SID>
      constexpr void* GetRawVoid(this C&& self) noexcept {
         return const_cast<void*>(static_cast<const void*>(ThisCom::template GetRaw<SID>()));
      }

      /// Set the heap pointer, any data pointer will do                      
      template<Cid SID = Id::First, CT::Sparse P>// requires Relevant<SID>
      constexpr void SetHeapInner(this auto& self, P heap) assumptious {
         /*if constexpr (requires { ThisCom::GetHeapInner() = heap; })
            ThisCom::GetHeapInner() = heap;
         else if constexpr (CT::CustomPointer<P>)
            ThisCom::GetHeapInner() = static_cast<StackRequest>(heap.Unpack());
         else {
            static_assert(Same<StackRequest, DecvqAll<StackRequest>>);*/
            ThisCom::GetHeapInner() = static_cast<StackRequest>(
               const_cast<void*>(static_cast<void const*>(heap))
            );
         //}
      }

      /// Reset the heap pointer to null                                      
      template<Cid SID = Id::First>// requires Relevant<SID>
      constexpr void SetHeapInner(this auto& self, nullptr_t) noexcept {
         ThisCom::GetHeapInner() = nullptr;
      }

      /// Default-initialization of this component                            
      void ConstructDefault(this auto& self) noexcept {
         ThisCom::SetHeapInner(nullptr);
      }
      
      /// Transfer from any kind of container.                                
      /// This is only a reference to a heap allocation and is not allowed    
      /// to allocate any new memory, so all this does is copy the heap       
      /// pointer, ignoring any intents.                                      
      ///   @param self deduced this                                          
      ///   @param intent the intent and container to transfer from           
      template<class C, CT::Intent I> requires CT::Container<I>
      void ConstructFrom(this C& self, I&& intent, size_t = 0) noexcept {
         ThisCom::SetHeapInner(intent->template GetRaw<Id::First>());
      }

      /// Transfer from any kind of container, respecting intents.            
      /// Do it for a particular dimension.                                   
      ///   @param intent The intent and container to transfer from.          
      template<Cid D, class C, CT::Intent I> requires CT::Container<I>
      void SliceFrom(this C& self, I&& intent) {
         static_assert(CT::Disowned<I>);
         ThisCom::SetHeapInner(intent->template GetRaw<D>());
      }

      /// Get a size based on reflected allocation page and count.            
      /// This will allocate memory for relevant headers, footers, and types  
      /// across all dimensions used in this heap component.                  
      ///   @param reserve the number of elements to request                  
      template<Cid SID = Id::First, CT::Container C>// requires Relevant<SID>
      auto RequestHeap(this C const& self, size_t reserve) assumptious -> Request {
         if constexpr (CT::ContainsOne<C>) {
            LglsAssumeDev(reserve == 1,
               "Container allows only one allocated element");
         }
         else if constexpr (C::InitialSize and C::GrowthFactor) {
            // We override allocation size with predefined parameters,  
            // if such are defined                                      
            size_t total = C::InitialSize;
            size_t growth = C::InitialSize;
            while (reserve > total) {
               growth *= C::GrowthFactor;
               total += growth;
            }
            reserve = total;
            //TODO when pagefile size is reached, start growing linearly by pagefile-sized intervals. this way we minimize cache misses in huge hash tables
         }

         Request result;
         result.mHeaderBytes = C::template DefineHeapHeader<Id::First>();
         result.mHeaderBytes = Align(result.mHeaderBytes, self.template GetAlignment<Id::First>());
         size_t total = result.mHeaderBytes;

         if constexpr (C::template CountHeapFooterRequests<Id::First>()) {
            // When there are footer requests (heap requests that       
            // depend on count & indirections), we aren't allowed to    
            // change the requested reserve to avoid heap corruptions.  
            if constexpr (CT::TypeErased<C>) {
               // Check for reflected minimal allocation at runtime     
               const auto T = self.template GetType<Id::First>();
               LglsAssumeDev((bool) T, "Requesting allocation size for an untyped container");
               total += reserve * T.GetSize();
               total += C::template DefineHeapFooter<Id::First>(reserve, T.GetIndirections());
            }
            else {
               // Check for reflected minimal allocation at compile-time
               using T = TypeOf<C, Id::First>;
               total += reserve * sizeof(T);
               total += C::template DefineHeapFooter<Id::First>(reserve, IndirectsOf<T>);
            }
         }
         else {
            // When there are no footer requests, we are allowed to     
            // reserve more bytes than requested. Makes reallocations   
            // less frequent and is thus faster.                        
            if constexpr (CT::TypeErased<C>) {
               // Check for reflected minimal allocation at runtime     
               const auto T = self.template GetType<Id::First>();
               LglsAssumeDev((bool) T, "Requesting allocation size for an untyped container");
               const auto size = T.GetSize();
               size_t for_T = ::std::max(reserve * size, static_cast<size_t>(T.GetMinAllocation()));
               reserve = for_T / size;
               total += for_T;
            }
            else {
               // Check for reflected minimal allocation at compile-time
               using T = TypeOf<C, Id::First>;
               size_t for_T = ::std::max(reserve * sizeof(T), MinAllocOf<T>());
               reserve = for_T / sizeof(T);
               total += for_T;
            }
         }

         // Add space for any additional dimensions, with alignment     
         Values<ENTRYN::Id...>::ForEach([&]<Cid i>{
            if constexpr (CT::TypeErased<C>) {
               const auto T = self.template GetType<i>();
               LglsAssumeDev((bool) T, "Requesting allocation size for an untyped container");
               total = Align(total, T.GetAlignment());
               total += reserve * T.GetSize();
               total += C::template DefineHeapFooter<i>(reserve, T.GetIndirections());
            }
            else {
               using T = TypeOf<C, i>;
               total = Align(total, alignof(T));
               total += reserve * sizeof(T);
               total += C::template DefineHeapFooter<i>(reserve, IndirectsOf<T>);
            }
         });

         total += C::template DefineHeapFooterGlobal<Id::First>(reserve);
         result.mTotalBytes = Roof2(total);
         result.mReserved = reserve;
         return result;
      }
   };

   #undef ThisCom
}
