///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Block.hpp"
//#include <Langulus/Core/Memory.hpp>


namespace Langulus::Annies
{

   /// Get a size based on reflected allocation page and count                
   ///   @param count - the number of elements to request                     
   ///   @return both the provided byte size and reserved count               
   template<class TYPE> LANGULUS(INLINED)
   auto Block<TYPE>::RequestSize(const size_t count) const IF_UNSAFE(noexcept)
   -> AllocationRequest {
      if constexpr (TypeErased) {
         LglsAssumeDev(IsTyped(),
            "Requesting allocation size for an untyped container");
         return mType->RequestSize(count);
      }
      else if constexpr (CT::Fundamental<TYPE> or CT::Exact<TYPE, Byte>) {
         AllocationRequest result;
         result.mByteSize = ::std::max(Roof2(count * sizeof(TYPE)), Alignment);
         result.mElementCount = result.mByteSize / sizeof(TYPE);
         return result;
      }
      else return GetType()->RequestSize(count);
   }

   /// Reserve a number of elements without initializing them                 
   /// If reserved data is smaller than currently initialized count, the      
   /// excess elements will be destroyed                                      
   ///   @tparam SETSIZE - whether or not to set size, too                    
   ///   @attention using SETSIZE will NOT construct any elements, use only   
   ///      if you know what you're doing                                     
   ///   @param count - number of elements to reserve                         
   template<class TYPE> template<bool SETSIZE> LANGULUS(INLINED)
   void Block<TYPE>::Reserve(const size_t count) {
      if (count < mCount)
         AllocateLess(count);
      else 
         AllocateMore(count);

      if constexpr (SETSIZE)
         mCount = count;
   }
   
   /// Allocate a number of elements, relying on the type of the container    
   ///   @attention assumes a valid and non-abstract type, if dense           
   ///   @tparam CREATE - true to call constructors and set count             
   ///   @tparam SETSIZE - true to set count, despite not constructing        
   ///   @param elements - number of elements to allocate                     
   template<class TYPE> template<bool CREATE, bool SETSIZE>
   void Block<TYPE>::AllocateMore(const size_t elements) {
      LglsAssumeDev(elements > mCount, "Bad element count");

      if constexpr (not TypeErased) {
         // Allocate/reallocate                                         
         const auto request = RequestSize(elements);
         if (mEntry) {
            if (mReserved >= elements) {
               // Required memory is already available                  
               if constexpr (CREATE) {
                  // But is not yet initialized, so initialize it       
                  if (mCount < elements) {
                     const auto count = elements - mCount;
                     CropInner(mCount, count).CreateDefault();
                  }
               }

               if constexpr (CREATE or SETSIZE)
                  mCount = elements;
               return;
            }

            // Reallocate                                               
            LglsAssumeDev(mEntry->GetUses() == 1,
               "Can't reuse memory of a block used from multiple places, "
               "BranchOut should've been called prior to AllocateMore"
            );

            Block previousBlock {*this};
            mEntry = Allocator::Reallocate(
               request.mByteSize * (Sparse ? 2 : 1),
               const_cast<Allocation*>(mEntry)
            );
            LANGULUS_ASSERT(mEntry, Allocate, "Out of memory");
            mReserved = request.mElementCount;

            if (mEntry != previousBlock.mEntry) {
               if (not previousBlock.mCount) {
                  // Memory moved, but nothing was initialized, so      
                  // just update mRaw pointer                           
                  mRaw = const_cast<Byte*>(mEntry->GetBlockStart());
               }
               else {
                  // Memory moved, and we should move all elements in it
                  // We're moving to new memory, so no reverse required 
                  if constexpr (CT::AbandonMakable<TYPE>
                  or CT::MoveMakable<TYPE>
                  or CT::ReferMakable<TYPE>
                  or CT::CopyMakable<TYPE>) {
                     mRaw = const_cast<Byte*>(mEntry->GetBlockStart());
                     CreateWithIntent(Abandon(previousBlock));
                     previousBlock.Free();
                  }
                  else LANGULUS_THROW(Construct,
                     "Memory moved, but T is not move-constructible");
               }
            }
            else {
               // Memory didn't move, but reserved count changed        
               if constexpr (Sparse) {
                  // Move entry data to its new place                   
                  MoveMemory(
                     GetEntries(),
                     previousBlock.GetEntries(),
                     mCount
                  );
               }
            }

            if constexpr (CREATE) {
               // Default-construct the rest                            
               const auto count = elements - mCount;
               CropInner(mCount, count).CreateDefault();
            }
         }
         else {
            // Allocate a fresh set of elements                         
            mType = MetaDataOf<TYPE>();
            AllocateFresh(request);

            if constexpr (CREATE) {
               // Default-construct everything                          
               CropInner(mCount, elements).CreateDefault();
            }
         }
      }
      else {
         if (not mType or (mType->mIsAbstract and not mType->mIsSparse)) {
            if (not mType)
               Logger::Error("Can't instantiate unknown type");
            else
               Logger::Error("Unable to instantiate ", elements,
                  " elements of abstract type ", mType);

            LANGULUS_THROW(Allocate, "Allocating untyped/abstract/sparse data");
         }

         if (mReserved >= elements) {
            // Required memory is already available                     
            if constexpr (CREATE) {
               // But is not yet initialized, so initialize it          
               if (mCount < elements) {
                  const auto count = elements - mCount;
                  CropInner(mCount, count).CreateDefault();
               }
            }
         }
         else AllocateInner<CREATE>(elements);
      }

      if constexpr (CREATE or SETSIZE)
         mCount = elements;
   }

   /// Shrink the block, depending on currently reserved	elements             
   /// Initialized elements on the back will be destroyed                     
   ///   @attention assumes 'elements' is smaller than the current reserve    
   ///   @param elements - number of elements to allocate                     
   template<class TYPE> LANGULUS(INLINED)
   void Block<TYPE>::AllocateLess(const size_t elements) {
      LglsAssumeDev(elements < mReserved, "Bad element count");

      if (mCount > elements) {
         // Destroy back entries on smaller allocation                  
         // Allowed even when container is static and out of            
         // jurisdiction, as in that case this acts as a simple count   
         // decrease, and no destructors shall be called                
         Trim(elements);
         return;
      }

      #if LANGULUS_FEATURE(MANAGED_MEMORY)
         // Shrink the memory block                                     
         // Guaranteed that entry doesn't move                          
         const auto request = RequestSize(elements);
         if (request.mElementCount == mReserved)
            return;
         
         LglsAssumeDev(mEntry->GetUses() == 1,
            "Can't reuse memory of a block used from multiple places, "
            "BranchOut should've been called prior to AllocateMore"
         );

         if constexpr (not TypeErased) {
            if constexpr (Sparse) {
               // Move entry data to its new place                      
               MoveMemory(
                  GetEntries() - mReserved + request.mElementCount,
                  GetEntries(), mCount
               );
            }

            mEntry = Allocator::Reallocate(
               request.mByteSize * (Sparse ? 2 : 1),
               const_cast<Allocation*>(mEntry)
            );
         }
         else {
            LglsAssumeDev(mType, "Invalid type");

            if (mType->mIsSparse) {
               // Move entry data to its new place                      
               MoveMemory(
                  GetEntries() - mReserved + request.mElementCount,
                  GetEntries(), mCount
               );
            }

            mEntry = Allocator::Reallocate(
               request.mByteSize * (mType->mIsSparse ? 2 : 1),
               const_cast<Allocation*>(mEntry)
            );
         }

         mReserved = request.mElementCount;
      #endif
   }

   /// Shallow-copy all elements inside this memory block into another block, 
   /// that is owned by us. Preserve hierarchy, density and state, but remove 
   /// size constraints and constness. If we already own this block's memory, 
   /// then only a Keep() is done                                             
   template<class TYPE> LANGULUS(INLINED)
   void Block<TYPE>::TakeAuthority() {
      if (not mRaw)
         return;

      if (mEntry) {
         // We already have authority, but to enforce it, we must ref   
         const_cast<Allocation*>(mEntry)->Keep();
         return;
      }

      // Shallow-copy all elements (equivalent to a Copy intent)        
      Block shallowCopy {*this};
      shallowCopy.AllocateFresh(RequestSize(mCount));
      shallowCopy.CreateWithIntent(Refer(*this));
      shallowCopy.mState -= DataState::Constant;
      CopyMemory(this, &shallowCopy);
   }

   /// Allocate a number of elements, relying on the type of the container    
   ///   @attention assumes a valid and non-abstract type, if dense           
   ///   @tparam CREATE - true to call constructors and set count             
   ///   @param elements - number of elements to allocate                     
   template<class TYPE> template<bool CREATE>
   void Block<TYPE>::AllocateInner(size_t elements) {
      LANGULUS_ASSERT(IsTyped(), Allocate,
         "Invalid type");
      LANGULUS_ASSERT(not GetType()->mIsAbstract or IsSparse(), Allocate,
         "Abstract dense type");

      // Retrieve the required byte size                                
      const auto request = RequestSize(elements);
      
      // Allocate/reallocate                                            
      if (mEntry) {
         // Reallocate                                                  
         LglsAssumeDev(mEntry->GetUses() == 1,
            "Can't reuse memory of a block used from multiple places, "
            "BranchOut should've been called prior to AllocateMore"
         );

         Block previousBlock {*this};
         mEntry = Allocator::Reallocate(
            request.mByteSize * (mType->mIsSparse ? 2 : 1),
            const_cast<Allocation*>(mEntry)
         );
         LANGULUS_ASSERT(mEntry, Allocate, "Out of memory");
         mReserved = request.mElementCount;

         if (mEntry != previousBlock.mEntry) {
            // Memory moved, and we should call abandon-construction    
            // We're moving to a new allocation, so no reverse needed   
            mRaw = const_cast<Byte*>(mEntry->GetBlockStart());
            CreateWithIntent(Abandon(previousBlock));
         }
         else {
            // Memory didn't move, but reserved count changed           
            if (mType->mIsSparse) {
               // Move entry data to its new place                      
               MoveMemory(
                  GetEntries(),
                  previousBlock.GetEntries(),
                  mCount
               );
            }
         }
      }
      else AllocateFresh(request);

      if constexpr (CREATE) {
         // Default-construct the rest                                  
         const auto count = elements - mCount;
         CropInner(mCount, count).CreateDefault();
         mCount = elements;
      }
   }

   /// Allocate a fresh allocation (inner function)                           
   ///   @attention changes entry, memory and reserve count                   
   ///   @param request - request to fulfill                                  
   template<class TYPE> LANGULUS(INLINED)
   void Block<TYPE>::AllocateFresh(const AllocationRequest& request) {
      // Sparse containers have additional memory allocated             
      // for each pointer's entry                                       
      mEntry = Allocator::Allocate(GetType(),
         request.mByteSize * (IsSparse() ? 2 : 1));

      LANGULUS_ASSERT(mEntry, Allocate, "Out of memory");
      mRaw = const_cast<Byte*>(mEntry->GetBlockStart());
      mReserved = request.mElementCount;
   }

   /// Reference memory block once                                            
   ///   @param DEEP - reference inner pointers/referenced instances, too?    
   template<class TYPE> template<bool DEEP> LANGULUS(INLINED)
   void Block<TYPE>::Keep() const noexcept {
      if (not mEntry)
         return;

      const_cast<Allocation*>(mEntry)->Keep(1);

      if constexpr (DEEP)
         KeepInner();
   }
   
   /// Reference referencable elements inside the block                       
   ///   @param MASK - used only when KeepInner is called from maps/sets      
   template<class TYPE> template<class MASK> LANGULUS(INLINED)
   void Block<TYPE>::KeepInner(MASK mask) const noexcept {
      constexpr bool MASKED = not CT::Nullptr<MASK>;
      [[maybe_unused]] size_t remaining;
      if constexpr (MASKED)
         remaining = GetCount();
      const auto count = MASKED ? mReserved : mCount;

      if constexpr (not TypeErased) {
         if constexpr (Sparse and CT::Referencable<Deptr<TYPE>>) {
            // Statically typed and sparse                              
            const auto entryBeg = GetEntries();
            auto entry = entryBeg;
            const auto entryEnd = entry + count;

            while (entry != entryEnd) {
               if constexpr (MASKED) {
                  if (not remaining)
                     break;

                  if (not mask[entry - entryBeg]) {
                     ++entry;
                     continue;
                  }

                  --remaining;
               }

               if (*entry) {
                  const_cast<Allocation*>(*entry)->Keep();
                  DecvqCast(GetRaw()[entry - GetEntries()])->Reference(1);
               }

               ++entry;
            }
         }
         else if constexpr (CT::Referencable<TYPE>) {
            // Statically typed and dense                               
            const auto rawBeg = GetRaw();
            auto raw = rawBeg;
            const auto rawEnd = raw + count;

            while (raw != rawEnd) {
               if constexpr (MASKED) {
                  if (not remaining)
                     break;

                  if (not mask[raw - rawBeg]) {
                     ++raw;
                     continue;
                  }

                  --remaining;
               }

               DecvqCast(raw++)->Reference(1);
            }
         }
      }
      else if (mType->mIsSparse and mType->mReference) {
         // Type-erased and sparse                                      
         const auto reference = mType->mReference;
         const auto entryBeg = GetEntries();
         auto entry = entryBeg;
         const auto entryEnd = entry + count;

         while (entry != entryEnd) {
            if constexpr (MASKED) {
               if (not remaining)
                  break;

               if (not mask[entry - entryBeg]) {
                  ++entry;
                  continue;
               }

               --remaining;
            }

            if (*entry) {
               const_cast<Allocation*>(*entry)->Keep();
               reference(mRawSparse[entry - GetEntries()], 1);
            }

            ++entry;
         }
      }
      else if (mType->mReference) {
         // Type-erased and dense                                       
         const auto reference = mType->mReference;
         const auto rawBeg = mRaw;
         auto raw = rawBeg;
         const auto rawEnd = mRaw + mType->mSize * count;

         while (raw != rawEnd) {
            if constexpr (MASKED) {
               if (not remaining)
                  break;

               if (not mask[raw - rawBeg]) {
                  raw += mType->mSize;
                  continue;
               }

               --remaining;
            }

            reference(raw, 1);
            raw += mType->mSize;
         }
      }
   }

   /// Dereference memory block once and destroy all elements if data was     
   /// fully dereferenced                                                     
   ///   @attention this never modifies any state, except mEntry              
   template<class TYPE> LANGULUS(INLINED)
   void Block<TYPE>::Free() {
      if (not mEntry)
         return;

      LglsAssumeDev(mEntry->GetUses() >= 1,
         "Bad memory dereferencing");

      if (mEntry->GetUses() == 1) {
         // Free memory                                                 
         LglsAssumeDev(not IsStatic(),
            "Last reference, but container was marked static"
            " - make sure initialization of this container was correct, "
            "did you forget to add a reference?",
            " Container contains ", mCount, " elements of ", mType
         );

         if (mCount)
            FreeInner();

         Allocator::Deallocate(const_cast<Allocation*>(mEntry));
      }
      else {
         // Dereference memory                                          
         if (mCount)
            FreeInner<false>();

         const_cast<Allocation*>(mEntry)->Free();
      }

      mEntry = nullptr;
   }
   
   /// Call destructors of all initialized items                              
   ///   @attention never modifies any block state                            
   ///   @attention assumes block is not empty                                
   ///   @attention assumes block is not static                               
   ///   @tparam DESTROY - used only when GetUses() == 1                      
   ///   @param mask - internally used for destroying tables (tag dispatch)   
   template<class TYPE> template<bool DESTROY, class MASK>
   void Block<TYPE>::FreeInner(MASK mask) {
      LglsAssumeDev(not DESTROY or GetUses() == 1,
         "Attempting to destroy elements used from multiple locations");
      LglsAssumeDev(not IsEmpty(),
         "Attempting to destroy elements in an empty container");
      LglsAssumeDev(not IsStatic(),
         "Destroying elements in a static container is not allowed");

      constexpr bool MASKED = not CT::Nullptr<MASK>;
      if constexpr (not TypeErased) {
         using DT = Decay<TYPE>;

         if constexpr (CT::Sparse<TYPE>) {
            // Destroy every sparse element                             
            FreeInnerSparse(mask);
         }
         else if constexpr (not CT::Complete<TYPE> and not CT::Function<TYPE>) {
            // CT::Destroyable<TYPE> will fail silently if TYPE isn't   
            // defined yet, causing nasty leaks. So make it             
            // not-so-silent...                                         
            static_assert(false, "Attempting to destroy an incomplete type");
         }
         else if constexpr (CT::Dense<TYPE> and CT::Destroyable<DT>
         and (DESTROY or CT::Referencable<DT>)) {
            // Destroy every dense element                              
            const auto count = not MASKED ? mCount : mReserved;
            auto data = GetRaw();
            const auto begMarker = data;
            const auto endMarker = data + count;
            [[maybe_unused]] size_t remaining;
            if constexpr (MASKED)
               remaining = GetCount();

            while (data != endMarker) {
               if constexpr (MASKED) {
                  if (not remaining)
                     break;

                  if (not mask[data - begMarker]) {
                     ++data;
                     continue;
                  }

                  --remaining;
               }

               if constexpr (DESTROY) {
                  if constexpr (CT::Referencable<DT>)
                     data->Reference(-1);
                  data->~DT();
               }
               else if constexpr (CT::Referencable<DT>) {
                  if (not data->Reference(-1))
                     data->~DT();
               }

               ++data;
            }
         }
      }
      else {
         if (mType->mIsSparse) {
            // Destroy every sparse element                             
            FreeInnerSparse(mask);
         }
         else if (not mType->mIsSparse and mType->mDestructor
         and (DESTROY or mType->mReference)) {
            // Destroy every dense element                              
            // Notice that fully dereferenced elements WILL be destroyed
            // regardless if DESTROY has been requested or not          
            // This prevents leaks.                                     
            const auto count = not MASKED ? mCount : mReserved;
            auto data = mRaw;
            [[maybe_unused]] int index;
            [[maybe_unused]] size_t remaining;
            if constexpr (MASKED) {
               index = 0;
               remaining = GetCount();
            }
            const auto endMarker = data + mType->mSize * count;

            if (mType->mReference) {
               while (data != endMarker) {
                  if constexpr (MASKED) {
                     if (not remaining)
                        break;

                     if (not mask[index]) {
                        data += mType->mSize;
                        ++index;
                        continue;
                     }

                     --remaining;
                  }

                  if constexpr (DESTROY) {
                     mType->mReference(data, -1);
                     mType->mDestructor(data);
                  }
                  else if (not mType->mReference(data, -1))
                     mType->mDestructor(data);

                  data += mType->mSize;

                  if constexpr (MASKED)
                     ++index;
               }
            }
            else if constexpr (DESTROY) {
               while (data != endMarker) {
                  if constexpr (MASKED) {
                     if (not remaining)
                        break;

                     if (not mask[index]) {
                        data += mType->mSize;
                        ++index;
                        continue;
                     }

                     --remaining;
                  }

                  mType->mDestructor(data);
                  data += mType->mSize;

                  if constexpr (MASKED)
                     ++index;
               }
            }
         }
      }

      // Always nullify upon destruction only if we're paranoid         
      //TODO IF_LANGULUS_PARANOID(ZeroMemory(mRaw, GetBytesize<THIS>()));
   }

   /// Call destructors of all initialized sparse items                       
   ///   @attention never modifies any block state                            
   ///   @attention assumes block is not empty                                
   ///   @attention assumes block is not static                               
   ///   @param mask - internally used for destroying tables (tag dispatch)   
   template<class TYPE> template<class MASK>
   void Block<TYPE>::FreeInnerSparse(MASK mask) {
      LglsAssumeDev(IsSparse(), "Container must be sparse");

      // Destroy all indirection layers, if their references reach      
      // 1, and destroy the dense element, if it has destructor         
      // This is done in the following way:                             
      //    1. First dereference all handles that point to the          
      //       same memory together as one                              
      //    2. Destroy those groups, that are fully dereferenced        
      constexpr bool MASKED = not CT::Nullptr<MASK>;
      const auto count = not MASKED ? mCount : mReserved;
      auto handle = GetHandle<void*>(0);
      const auto begMarker = handle.mValue;
      const auto endMarker = handle.mValue + count;
      [[maybe_unused]] size_t remaining;
      if constexpr (MASKED)
         remaining = GetCount();

      // Execute a call for each handle that matches current entry      
      const auto for_each_match = [&](auto&& call) {
         auto handle2 = handle + 1;
         [[maybe_unused]] size_t remaining2;
         if constexpr (MASKED)
            remaining2 = remaining;

         while (handle2.mValue != endMarker) {
            if constexpr (MASKED) {
               if (not remaining2)
                  break;

               if (not mask[handle2.mValue - begMarker]) {
                  ++handle2;
                  continue;
               }

               --remaining2;
            }

            if (handle.GetEntry() == handle2.GetEntry())
               call(handle2);

            ++handle2;
         }
      };

      //                                                                
      while (handle.mValue != endMarker) {
         if constexpr (MASKED) {
            if (not remaining)
               break;

            if (not mask[handle.mValue - begMarker]) {
               ++handle;
               continue;
            }

            --remaining;
         }

         if (not handle.GetEntry()) {
            ++handle;
            continue;
         }

         if (1 != mEntry->GetUses()) {
            if constexpr (not TypeErased) {
               if constexpr (CT::Referencable<Deptr<TYPE>>) {
                  // Statically typed and sparse                        
                  const_cast<Allocation*>(handle.GetEntry())->Free();
                  DecvqCast(static_cast<TYPE>(handle.Get()))->Reference(-1);
               }
            }
            else if (mType->mReference) {
               // Type-erased and sparse                                
               const auto reference = mType->mReference;
               const_cast<Allocation*>(handle.GetEntry())->Free();
               reference(handle.Get(), -1);
            }
         }
         else {
            // size_t all handles that match the current entry           
            size_t matches = 0;
            for_each_match([&matches](const Handle<void*>&) {
               ++matches;
            });
            LglsAssumeDev(handle.GetEntry()->GetUses() >= matches + 1,
               "Matches shouldn't exceed the expected count");

            if (matches) {
               const_cast<Allocation*>(handle.GetEntry())->Free(matches);

               if (1 == handle.GetEntry()->GetUses()) {
                  // Destroy all matching handles, but deallocate only  
                  // once after that                                    
                  for_each_match([&](Handle<void*>& h) {
                     h.template FreeInner<true, false>(mType);
                  });
               }
               else {
                  // We're not allowed to destroy anything, just deref  
                  // and reset all matching handles                     
                  for_each_match([&](Handle<void*>& h) {
                     // We still have to make sure that per-instance    
                     // references are also affected                    
                     [[maybe_unused]] size_t remaining_refs = 0;
                     if constexpr (TypeErased) {
                        if (mType->mReference)
                           remaining_refs = mType->mReference(h.Get(), -1);
                     }
                     else {
                        if constexpr (CT::Referencable<Deptr<TYPE>>)
                           remaining_refs = DecvqCast(static_cast<TYPE>(h.Get()))->Reference(-1);
                     }

                     h.GetEntry() = nullptr;
                  });
               }
            }
         
            // Deref the last element                                   
            handle.FreeInner(mType);
         }

         ++handle;
      }
   }

   /// Reset the memory inside the block                                      
   template<class TYPE> LANGULUS(INLINED)
   constexpr void Block<TYPE>::ResetMemory() noexcept {
      mRaw   = nullptr;
      mEntry = nullptr;
      mCount = mReserved = 0;
   }
}