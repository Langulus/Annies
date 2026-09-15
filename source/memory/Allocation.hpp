///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include <Langulus/MetaOf.hpp>


namespace Langulus::Annies
{

   using RTTI::AllocationRequest;
   using RTTI::DMeta;
   using RTTI::CMeta;
   using RTTI::TMeta;

   using Pool = void;
   
   template<class T>
   concept AllocationPrimitive = requires(T a) { 
      {T::GetNewAllocationSize(0)} -> CT::Unsigned;
   };


   ///                                                                        
   ///   Memory allocation                                                    
   ///                                                                        
   /// This is a single allocation record                                     
   ///                                                                        
   struct Allocation final {
   #if LANGULUS_FEATURE(MANAGED_MEMORY)
      friend class Pool;
   #endif
   friend struct Allocator;
   protected:
      // Allocated bytes for this chunk                                 
      Offset mAllocatedBytes;
      // The number of references to this memory                        
      Count mReferences;
      union {
         // This pointer has two uses, depending on mReferences         
         // If mReferences > 0, it refers to the pool that owns the     
         //    allocation, or	handle for std::free() if MANAGED_MEMORY  
         //    feature is not enabled                                   
         // If mReferences == 0, it refers to the next free entry to be 
         //    reused                                                   
         Pool* mPool;
         Allocation* mNextFreeEntry;
      };

      // Acts like a timestamp of when the allocation happened          
      #if LANGULUS_FEATURE(MEMORY_STATISTICS)
         Count mStep;
      #endif

   public:
      Allocation() = delete;
      Allocation(const Allocation&) = delete;
      Allocation(Allocation&&) = delete;
      ~Allocation() = delete;

      constexpr Allocation(Offset, Pool*) noexcept;

      static constexpr Offset GetSize() noexcept;
      static constexpr Offset GetNewAllocationSize(Offset) noexcept;
      static constexpr Offset GetMinAllocation() noexcept;

      auto GetUses() const noexcept -> Count;
      auto GetBlockStart() const noexcept -> Byte*;
      auto GetBlockEnd() const noexcept -> Byte const*;
      auto GetTotalSize() const noexcept -> Offset;
      auto GetAllocatedSize() const noexcept -> Offset;
      bool Contains(const void*) const noexcept;
      bool CollisionFree(const Allocation&) const noexcept;

      template<class T>
      T* As() const noexcept;

      constexpr void Keep() noexcept;
      constexpr void Keep(Count) noexcept;
      constexpr void Free() noexcept;
      constexpr void Free(Count) noexcept;
   };

} // namespace Langulus::Annies

#include "Allocation.inl"