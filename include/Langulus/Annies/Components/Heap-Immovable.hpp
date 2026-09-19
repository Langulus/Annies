///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Component.hpp"


namespace Langulus::Annies::Component
{
   ///                                                                        
   /// Interfaces a heap allocation                                           
   /// Adds a pointer member to the raw byte memory                           
   /// The pointer is not allowed to move on reallocation, and instead        
   /// multiple allocations are chained together                              
   ///   @tparam INITIAL_SIZE the initial size (in elements). Used in hashed  
   ///      containers in order to control hash table size. If 0, the heap    
   ///      will use reflected type properties only.                          
   ///   @tparam GROWTH_FACTOR growth factor on reallocation. Ued in hashed   
   ///      containers in order to control hash table growth on reallocation. 
   ///      If 0, the heap will grow according to reflected type properties.  
   ///   @tparam POINTER_TYPE heap pointer type (you can use packed pointers) 
   template<unsigned INITIAL_SIZE, unsigned GROWTH_FACTOR, CT::HeapEntry ENTRY0, CT::HeapEntry...ENTRYN>
   struct HeapImmovable {
      using CTTI_Component = Yup;
      using CTTI_ReflectAs = void;
      using Id             = Values<ENTRY0::Id, ENTRYN::Id...>;
      using HeapProvider   = Id;
      using StackRequest   = typename ENTRY0::T;

      static constexpr bool Shared              = sizeof...(ENTRYN) > 0;
      static constexpr int  ComponentPrecedence = -2000;
      static constexpr bool HeapCanBeNull       = true;
      static constexpr bool Reallocatable       = true;
      static constexpr unsigned InitialSize     = INITIAL_SIZE;
      static constexpr unsigned GrowthFactor    = GROWTH_FACTOR;
      //template<Cid SID>
      //static constexpr bool Relevant = Id::template Contains<SID>;

   protected:
      using Byte = uint8_t;

      // A heap of heaps - the inner ones are immovable                 
      Byte** mHeap;
      // Number of allocated heaps - each new heap is twice as big      
      uint8_t mHeapCount;

      // The start of the reusable chain, in the first heap that has    
      // a free cell                                                    
      Byte* mReusable;

   public:
   #if LANGULUS(TESTING)
      auto GetReusable()      const noexcept { return mReusable;  }
      auto GetFrames()        const noexcept { return mHeapCount; }
      auto GetFrame(int idx)  const noexcept { return mHeap[idx]; }
   #endif
   };
}
