///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Allocation.hpp"


namespace Langulus::Annies
{
   
   /// MSVC will likely never support std::aligned_alloc, so we use           
   /// a custom portable routine that's almost the same                       
   /// https://stackoverflow.com/questions/62962839                           
   ///                                                                        
   /// Each allocation has the following prefixed bytes:                      
   /// [padding][T::GetSize()][client bytes...]                               
   ///                                                                        
   ///   @param size - the number of client bytes to allocate                 
   ///   @return a newly allocated memory that is correctly aligned           
   template<AllocationPrimitive T>
   T* AlignedAllocate(Offset size) noexcept {
      const auto finalSize = T::GetNewAllocationSize(size) + Alignment;
      const auto base = ::std::malloc(finalSize);
      if (not base)
         return nullptr;

      // Align pointer to the alignment LANGULUS was built with         
      auto ptr = reinterpret_cast<T*>(
         (reinterpret_cast<Offset>(base) + Alignment)
         & ~(Alignment - Offset {1})
      );

      // Place the entry there                                          
      new (ptr) T {size, base};
      return ptr;
   }


   ///                                                                        
   ///   A mockup of a memory manager                                         
   ///                                                                        
   struct Allocator {
      /// No state when MANAGED_MEMORY feature is disabled                    
      struct State {
         consteval bool Assert() const noexcept { return true; }
      };

      LANGULUS(INLINED)
      static Allocation* Allocate(DMeta, Offset size) IF_UNSAFE(noexcept) {
         LglsAssumeDev(size, "Zero allocation is not allowed");
         return AlignedAllocate<Allocation>(size);
      }

      LANGULUS(INLINED)
      static Allocation* Reallocate(Offset size, Allocation* previous) IF_UNSAFE(noexcept) {
         LglsAssumeDev(previous,
            "Reallocating nullptr");
         LglsAssumeDev(size != previous->GetAllocatedSize(),
            "Reallocation suboptimal - size is same as previous");
         LglsAssumeDev(size,
            "Zero reallocation is not allowed");
         LglsAssumeDev(previous->mReferences,
            "Deallocating an unused allocation");

         (void) previous;
         return Allocator::Allocate(nullptr, size);
      }

      LANGULUS(INLINED)
      static void Deallocate(Allocation* entry) IF_UNSAFE(noexcept) {
         LglsAssumeDev(entry,
            "Deallocating nullptr");
         LglsAssumeDev(entry->GetAllocatedSize(),
            "Deallocating an empty allocation");
         LglsAssumeDev(entry->mReferences,
            "Deallocating an unused allocation");
         LglsAssumeDev(entry->mReferences == 1,
            "Deallocating an allocation used from multiple places");

         ::std::free(entry->mPool);
      }

      static constexpr const Allocation* Find(DMeta, const void*) noexcept {
         return nullptr;
      }

      static constexpr bool CheckAuthority(DMeta, const void*) noexcept {
         return false;
      }

      static consteval bool CollectGarbage() noexcept {
         return false;
      }

      #if LANGULUS_FEATURE(MEMORY_STATISTICS)
         static consteval void DumpPools() noexcept {}
      #endif
   };

} // namespace Langulus::Annies