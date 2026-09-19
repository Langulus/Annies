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
      size_t mAllocatedBytes;
      // The number of references to this memory                        
      size_t mReferences;
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
         size_t mStep;
      #endif

   public:
      Allocation() = delete;
      Allocation(const Allocation&) = delete;
      Allocation(Allocation&&) = delete;
      ~Allocation() = delete;

      constexpr Allocation(size_t, Pool*) noexcept;

      static constexpr size_t GetSize() noexcept;
      static constexpr size_t GetNewAllocationSize(size_t) noexcept;
      static constexpr size_t GetMinAllocation() noexcept;

      auto GetUses() const noexcept -> size_t;
      auto GetBlockStart() const noexcept -> Byte*;
      auto GetBlockEnd() const noexcept -> Byte const*;
      auto GetTotalSize() const noexcept -> size_t;
      auto GetAllocatedSize() const noexcept -> size_t;
      bool Contains(const void*) const noexcept;
      bool CollisionFree(const Allocation&) const noexcept;

      template<class T>
      T* As() const noexcept;

      constexpr void Keep() noexcept;
      constexpr void Keep(size_t) noexcept;
      constexpr void Free() noexcept;
      constexpr void Free(size_t) noexcept;
   };

} // namespace Langulus::Annies

#include "Allocation.inl"