///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Block.hpp"
#include "../../many/Trait.hpp"
#include "../../many/Construct.hpp"
#include "../../text/Text.hpp"


namespace Langulus::Annies
{
   
#if LANGULUS(DEBUG)
   /// Log a tracking event                                                   
   ///   @param whatHappened - event message                                  
   ///   @param mask - used when tracking tables                              
   template<class TYPE> template<class MASK, class...MSGs> LANGULUS(ALWAYS_INLINED)
   void Block<TYPE>::TrackingReport(MASK mask, MSGs&&...messages) const {
      if (not (mState & DataState::Tracked))
         return;

      constexpr bool MASKED = not CT::Nullptr<MASK>;
      const auto tab = Logger::Section(Logger::Purple,
         NameOf<Block<TYPE>>(), ": ", Forward<MSGs>(messages)...);

      if (mType)
         Logger::Line("Type: ", mType, "; size: ", mType->mSize);
      else
         Logger::Line("Type: ", mType);

      if (mRaw)
         Logger::Line("Raw: ", Logger::Hex(mRaw), "; count: ", mCount, "; reserved: ", mReserved);
      
      if (mEntry)
         Logger::Line("Entry: ", Logger::Hex(mEntry), "; references: ", mEntry->GetUses());
      else if (mRaw)
         Logger::Line("Entry: static/disowned");

      if (IsEmpty())
         return;

      const auto tab2 = Logger::Section("Elements:");
      size_t remaining = mCount;
      for (size_t i = 0; i < mReserved; ++i) {
         if (not remaining)
            break;

         if constexpr (MASKED) {
            if (not mask[i])
               continue;
         }

         auto element = GetElement(i);
         --remaining;

         Text stringified;
         if (mType->template CastsTo<Text, true>()) {
            stringified += "\"";
            stringified += element.template As<Text>();
            stringified += "\"";
         }
         else {
            const DMeta TextMeta = MetaDataOf<Text>();
            auto foundTo = mType->mConvertersTo.find(TextMeta);
            auto foundFr = TextMeta->mConvertersFrom.find(mType);
            if (foundTo != mType->mConvertersTo.end())
               foundTo->second.mFunction(IsSparse() ? *element.mRawSparse : element.mRaw, &stringified);
            else if (foundFr != TextMeta->mConvertersFrom.end())
               foundFr->second.mFunction(IsSparse() ? *element.mRawSparse : element.mRaw, &stringified);
         }

         if (not stringified)
            stringified = "<not stringifiable>";

         if (IsSparse()) {
            // Contains pointers                                        
            if (*((void**)element.mRaw)) {
               // Valid pointer                                         
               if (mType->mReference) {
                  // Valid pointer with deep refs                       
                  Logger::Line(i, "] ", Logger::Hex(*((void**)element.mRaw)), "; ", stringified,
                     " (instance references: ", mType->mReference(*((void**)element.mRaw), 0),
                     "; entry: ", Logger::Hex(GetEntries()[i]),
                     "; entry references: ", GetEntries()[i]->GetUses(), ")"
                  );
               }
               else {
                  // Valid pointer with shallow refs                    
                  Logger::Line(i, "] ", Logger::Hex(*((void**)element.mRaw)),
                     "; ", stringified, " (entry: ", Logger::Hex(GetEntries()[i]),
                     "; entry references: ", GetEntries()[i]->GetUses(), ")"
                  );
               }
            }
            else {
               // Invalid pointer                                       
               Logger::Line(i, "] nullptr");
            }
         }
         else {
            // Contains dense elements                                  
            if (mType->template CastsTo<A::Block, true>()) {
               // Can be represented as a block                         
               auto asBlock = reinterpret_cast<const Block<>*>(element.mRaw);
               if (asBlock->GetAllocation()) {
                  Logger::Line(i, "] ", stringified,
                     " (block references: ", asBlock->GetUses(), ")"
                  );
               }
               else {
                  // Not owned                                          
                  Logger::Line(i, "] ", stringified, " (static/disowned)");
               }
            }
            else {
               // Can't be represented as a block                       
               if (mType->mReference) {
                  // With deep refs                                     
                  Logger::Line(i, "] ", stringified,
                     " (instance references: ", mType->mReference(element.mRaw, 0), ")"
                  );
               }
               else {
                  // Without refs                                       
                  Logger::Line(i, "] ", stringified);
               }
            }
         }
      }
   }
#endif

   /// Overwrite the current data state                                       
   ///   @attention you can not add/remove constraints like that              
   ///   @param state - the state to overwrite with                           
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   constexpr void Block<TYPE>::SetState(DataState state) noexcept {
      mState = state - DataState::Constrained;
   }

   /// Add a state                                                            
   ///   @attention you can not add constraints like that                     
   ///   @param state - the state to add to the current                       
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   constexpr void Block<TYPE>::AddState(DataState state) noexcept {
      mState += state - DataState::Constrained;
      #if LANGULUS(DEBUG)
         if (state & DataState::Tracked)
            TrackingReport(nullptr, "Started tracking");
      #endif
   }

   /// Remove a state                                                         
   ///   @attention you can not remove constraints like that                  
   ///   @param state - the state to remove from the current                  
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   constexpr void Block<TYPE>::RemoveState(DataState state) noexcept {
      mState -= state - DataState::Constrained;
   }

   /// Explicit bool cast operator, for use in if statements                  
   ///   @return true if block contains at least one valid element            
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   constexpr Block<TYPE>::operator bool() const noexcept {
      return not IsEmpty();
   }

   /// Check if a pointer is anywhere inside the block's reserved memory      
   ///   @attention doesn't check deep or sparse data regions                 
   ///   @param ptr - the pointer to check                                    
   ///   @return true if inside the immediate reserved memory block range     
   template<class TYPE> LANGULUS(INLINED)
   bool Block<TYPE>::Owns(const void* ptr) const noexcept {
      return ptr >= mRaw and ptr < mRaw + GetReservedSize();
   }

   /// Check if we have jurisdiction over the contained memory                
   ///   @return the allocation pointer, if we have jurisdiction              
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   constexpr const Allocation* Block<TYPE>::GetAllocation() const noexcept {
      return mEntry;
   }

   /// Get the number of references for the allocated memory block            
   ///   @return the references for the memory block, or 0 if memory is       
   ///           outside authority (or unallocated)                           
   template<class TYPE> LANGULUS(INLINED)
   constexpr size_t Block<TYPE>::GetUses() const noexcept {
      return mEntry ? mEntry->GetUses() : 0;
   }
   
   /// Get the contained type                                                 
   ///   @return the meta data                                                
   template<class TYPE> LANGULUS(INLINED)
   constexpr DMeta Block<TYPE>::GetType() const noexcept {
      if constexpr (TypeErased)
         return mType;
      else
         return (mType = MetaDataOf<TYPE>());
   }

   /// Get the number of initialized elements                                 
   ///   @return the number of initialized elements                           
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   constexpr size_t Block<TYPE>::GetCount() const noexcept {
      return mCount;
   }

   /// Get the number of reserved (maybe uninitialized) elements              
   ///   @return the number of reserved (maybe uninitialized) elements        
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   constexpr size_t Block<TYPE>::GetReserved() const noexcept {
      return mReserved;
   }
   
   /// Get the number of reserved bytes                                       
   ///   @attention this doesn't include bytes reserved for entries in sparse 
   ///              containers, when managed memory is enabled                
   ///   @return the number of reserved bytes                                 
   template<class TYPE> LANGULUS(INLINED)
   constexpr size_t Block<TYPE>::GetReservedSize() const noexcept {
      if constexpr (TypeErased)
         return mType ? mReserved * mType->mSize : 0;
      else
         return mReserved * sizeof(TYPE);
   }
   
   /// Get the number of sub-blocks (this one included)                       
   ///   @return the number of contained blocks, including this one           
   template<class TYPE>
   size_t Block<TYPE>::GetCountDeep() const noexcept {
      if (IsEmpty() or not IsDeep())
         return 1;

      using B = decltype(GetDeep());
      size_t counter = 1;
      IterateInner<false, false>(mCount, [&counter](B block) noexcept {
         counter += block.GetCountDeep();
      });
      return counter;
   }

   /// Get the sum of initialized non-deep elements in all sub-blocks         
   ///   @return the number of contained non-deep elements                    
   template<class TYPE>
   size_t Block<TYPE>::GetCountElementsDeep() const noexcept {
      if (IsEmpty() or not IsTyped())
         return 0;

      if (not IsDeep())
         return mCount;

      using B = decltype(GetDeep());
      size_t counter = 0;
      IterateInner<false, false>(mCount, [&counter](B block) noexcept {
         counter += block.GetCountElementsDeep();
      });
      return counter;
   }
   
   /// Check if memory is allocated                                           
   ///   @return true if the block contains any reserved memory               
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   constexpr bool Block<TYPE>::IsAllocated() const noexcept {
      return mRaw != nullptr;
   }

   /// Check if block is marked as past                                       
   ///   @return true if this container is marked as past                     
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   constexpr bool Block<TYPE>::IsPast() const noexcept {
      return mState.IsPast();
   }

   /// Check if block is marked as future                                     
   ///   @return true if this container is marked as future                   
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   constexpr bool Block<TYPE>::IsFuture() const noexcept {
      return mState.IsFuture();
   }

   /// Check if block is not phased (is neither past, nor future)             
   ///   @return true if this container is not phased                         
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   constexpr bool Block<TYPE>::IsNow() const noexcept {
      return mState.IsNow();
   }

   /// Check if block is marked as missing                                    
   ///   @return true if this container is marked as missing                  
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   constexpr bool Block<TYPE>::IsMissing() const noexcept {
      return mState.IsMissing();
   }

   /// Check if block has a data type                                         
   ///   @return true if data contained in this pack is specified             
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   constexpr bool Block<TYPE>::IsTyped() const noexcept {
      if constexpr (TypeErased)
         return static_cast<bool>(mType);
      else
         return true;
   }

   /// Check if block has a data type                                         
   ///   @return true if data contained in this pack is unspecified           
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   constexpr bool Block<TYPE>::IsUntyped() const noexcept {
      if constexpr (TypeErased)
         return not mType;
      else
         return false;
   }

   /// Check if block has a data type, and is type-constrained                
   ///   @return true if type-constrained                                     
   template<class TYPE> LANGULUS(INLINED)
   constexpr bool Block<TYPE>::IsTypeConstrained() const noexcept {
      if constexpr (TypeErased)
         return mType and mState.IsTyped();
      else
         return true;
   }

   /// Check if block is encrypted                                            
   ///   @return true if the contents of this pack are encrypted              
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   constexpr bool Block<TYPE>::IsEncrypted() const noexcept {
      return mState.IsEncrypted();
   }

   /// Check if block is compressed                                           
   ///   @return true if the contents of this pack are compressed             
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   constexpr bool Block<TYPE>::IsCompressed() const noexcept {
      return mState.IsCompressed();
   }

   /// Check if block is constant                                             
   ///   @return true if the contents are constant                            
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   constexpr bool Block<TYPE>::IsConstant() const noexcept {
      return mState.IsConstant();
   }

   /// Check if block is mutable                                              
   ///   @return true if the contents are mutable                             
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   constexpr bool Block<TYPE>::IsMutable() const noexcept {
      return not IsConstant();
   }

   /// Check if block is static (size-constrained)                            
   ///   @attention static containers don't contain entries when sparse       
   ///   @attention static containers can't be resized                        
   ///   @return true if the contents are static (size-constrained)           
   template<class TYPE> LANGULUS(INLINED)
   constexpr bool Block<TYPE>::IsStatic() const noexcept {
      return mRaw and not mEntry;
   }
   
   /// Check if block is inhibitory (or) container                            
   ///   @return true if this is an inhibitory container                      
   template<class TYPE> LANGULUS(INLINED)
   constexpr bool Block<TYPE>::IsOr() const noexcept {
      return mState.IsOr() and mCount > 1;
   }

   /// Check if block contains no initialized elements                        
   ///   @return true if this is an empty container                           
   template<class TYPE> LANGULUS(INLINED)
   constexpr bool Block<TYPE>::IsEmpty() const noexcept {
      return mCount == 0;
   }

   /// Check if block contains either created elements, or relevant state     
   ///   @return true if block either contains state, or has inserted stuff   
   template<class TYPE> LANGULUS(INLINED)
   constexpr bool Block<TYPE>::IsValid() const noexcept {
      return mCount or GetUnconstrainedState();
   }

   /// Check if block contains no elements and no relevant state              
   ///   @return true if this is an empty stateless container                 
   template<class TYPE> LANGULUS(INLINED)
   constexpr bool Block<TYPE>::IsInvalid() const noexcept {
      return not IsValid();
   }
   
   /// Check if block contains dense data                                     
   ///   @returns true if this container refers to dense memory               
   template<class TYPE> LANGULUS(INLINED)
   constexpr bool Block<TYPE>::IsDense() const noexcept {
      if constexpr (TypeErased)
         return mType ? not mType->mIsSparse : true;
      else
         return CT::Dense<TYPE>;
   }

   /// Check if block contains pointers                                       
   ///   @return true if the block contains pointers                          
   template<class TYPE> LANGULUS(INLINED)
   constexpr bool Block<TYPE>::IsSparse() const noexcept {
      if constexpr (TypeErased)
         return mType ? mType->mIsSparse : false;
      else
         return CT::Sparse<TYPE>;
   }
   
   /// Check if block contains POD items - if so, it's safe to directly copy  
   /// raw memory from container. Note, that this doesn't only consider the   
   /// standard c++ type traits, like trivially_constructible.                
   /// Want a non-trivial type to be handled as POD in these containers?      
   /// - You can explicitly reflect any type with `LANGULUS(POD) true`        
   ///   @return true if contained data is plain old data                     
   template<class TYPE> LANGULUS(INLINED)
   constexpr bool Block<TYPE>::IsPOD() const noexcept {
      if constexpr (TypeErased)
         return mType and mType->mIsPOD;
      else
         return CT::POD<Decay<TYPE>>;
   }

   /// Check if block contains resolvable items, that is, items that have a   
   /// reflected GetBlock() function, that can be used to represent           
   /// themselves as their most concretely typed block                        
   ///   @return true if contained data can be resolved on element basis      
   template<class TYPE> LANGULUS(INLINED)
   constexpr bool Block<TYPE>::IsResolvable() const noexcept {
      if constexpr (TypeErased)
         return mType and mType->mIsSparse and mType->mResolver;
      else
         return CT::Resolvable<TYPE>;
   }

   /// Check if the memory block contains memory blocks considered deep       
   ///   @return true if the memory block contains deep memory blocks         
   template<class TYPE> LANGULUS(INLINED)
   constexpr bool Block<TYPE>::IsDeep() const noexcept {
      if constexpr (TypeErased)
         return mType and mType->mIsDeep and mType->template CastsTo<A::Block, true>();
      else
         return CT::Deep<Decay<TYPE>>;
   }

   /// Check if the memory block contains memory blocks                       
   ///   @return true if the memory block contains memory blocks              
   template<class TYPE> LANGULUS(INLINED)
   constexpr bool Block<TYPE>::IsBlock() const noexcept {
      if constexpr (TypeErased)
         return mType and mType->template CastsTo<A::Block, true>();
      else
         return CT::Block<Decay<TYPE>>;
   }

   /// Check state compatibility                                              
   ///   @param other - the block to check                                    
   ///   @return true if state is compatible                                  
   template<class TYPE> LANGULUS(INLINED)
   constexpr bool Block<TYPE>::CanFitState(const CT::Block auto& other) const noexcept {
      return IsInvalid() or (
             IsMissing() == other.IsMissing()
         and (not IsTypeConstrained() or other.IsExact(mType))
         and (mCount <= 1 or other.mCount <= 1 or IsOr() == other.IsOr())
         and (IsNow() or other.IsNow() or IsFuture() == other.IsFuture())
      );
   }

   /// Get the size of the contained data, in bytes                           
   ///   @return the byte size                                                
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   constexpr size_t Block<TYPE>::GetBytesize() const noexcept {
      return mCount * GetStride();
   }

   /// Get the token of the contained type                                    
   ///   @return the token                                                    
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   constexpr Token Block<TYPE>::GetToken() const noexcept {
      return GetType().GetToken();
   }
   
   /// Get the size of a single element (in bytes)                            
   ///   @attention this returns zero if block is untyped                     
   ///   @return the size of a single element in bytes                        
   template<class TYPE> LANGULUS(INLINED)
   constexpr size_t Block<TYPE>::GetStride() const noexcept {
      if constexpr (TypeErased)
         return mType ? mType->mSize : 0_B;
      else
         return sizeof(TYPE);
   }
   
   /// Get the data state of the container                                    
   ///   @return the current block state                                      
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   constexpr DataState Block<TYPE>::GetState() const noexcept {
      return mState;
   }

   /// Get the relevant state when relaying one block	to another              
   /// Relevant states exclude size and type constraints, as well as tracking 
   /// in order to avoid changes in behavior due to debugging.                
   ///   @return the current unconstrained block state                        
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   constexpr DataState Block<TYPE>::GetUnconstrainedState() const noexcept {
      #if LANGULUS(DEBUG)
         return mState - DataState::Constrained - DataState::Tracked;
      #else
         return mState - DataState::Constrained;
      #endif
   }
   
   /// Deep (slower) check if there's anything missing inside nested blocks   
   ///   @return true if any deep or flat memory block contains missing data  
   template<class TYPE> LANGULUS(INLINED)
   constexpr bool Block<TYPE>::IsMissingDeep() const {
      if (IsMissing())
         return true;

      bool result = false;
      ForEachDeep<false, false>(
         [&result](const Block& b) noexcept {
            result = b.IsMissing();
            return not result;
         }
      );
      return result;
   }
   
   /// Check if a memory block can be concatenated to this one                
   ///   @param other - the block to concatenate                              
   ///   @return true if able to concatenate to this one                      
   template<class TYPE> LANGULUS(INLINED)
   constexpr bool Block<TYPE>::IsConcatable(const CT::Block auto& other) const noexcept {
      return not IsStatic() and not IsConstant()
         and CanFitState(other) and IsSimilar(other);
   }

   /// Check if a type can be inserted to this block                          
   ///   @param other - check if a given type is insertable to this block     
   ///   @return true if able to insert an instance of the type to this block 
   template<class TYPE> LANGULUS(INLINED)
   constexpr bool Block<TYPE>::IsInsertable(DMeta other) const noexcept {
      return other and not IsStatic() and not IsConstant()
         and ((IsSparse() == other->mIsSparse))
         and ((IsDeep() and other->mIsDeep) or CastsToMeta(other));
   }
   
   /// Check if a static type can be inserted                                 
   ///   @tparam T - the type to check                                        
   ///   @return true if able to insert an instance of the type to this block 
   template<class TYPE> template<CT::NotVoid T> LANGULUS(INLINED)
   constexpr bool Block<TYPE>::IsInsertable() const noexcept {
      if constexpr (TypeErased)
         return IsInsertable(MetaDataOf<T>());
      else if constexpr (CT::Similar<TYPE, T>
      or (CT::Deep<Decay<TYPE>, Decay<T>> and CT::Dense<TYPE> == CT::Dense<T>))
         return not IsStatic() and not IsConstant();
      else
         return false;
   }

   /// Get the raw data inside the container                                  
   ///   @attention as unsafe as it gets, but as fast as it gets              
   ///   @return a pointer to the first allocated element                     
   template<class TYPE> template<class T> LANGULUS(ALWAYS_INLINED)
   auto Block<TYPE>::GetRaw() IF_UNSAFE(noexcept) -> T* {
      LglsAssumeDev(CT::Dense<T> or IsSparse(),
         "Representing dense data as sparse");
      return reinterpret_cast<T*>(mRaw);
   }

   template<class TYPE> template<class T> LANGULUS(ALWAYS_INLINED)
   auto Block<TYPE>::GetRaw() const IF_UNSAFE(noexcept) -> T const* {
      return const_cast<Block<TYPE>*>(this)->template GetRaw<T>();
   }

   /// Get the end raw data pointer inside the container (const)              
   ///   @attention as unsafe as it gets, but as fast as it gets              
   ///   @attention the resulting pointer never points to a valid element     
   ///   @return a pointer to the last+1 element (never initialized)          
   template<class TYPE> template<class T> LANGULUS(ALWAYS_INLINED)
   auto Block<TYPE>::GetRawEnd() const IF_UNSAFE(noexcept) -> const T* {
      LglsAssumeDev(CT::Dense<T> or IsSparse(),
         "Representing dense data as sparse");

      if constexpr (TypeErased and CT::Dense<T>)
         return reinterpret_cast<const T*>(mRaw + GetBytesize());
      else
         return reinterpret_cast<const T*>(mRaw) + mCount;
   }

   /// Make memory block constant                                             
   /// Disables the ability to access members as mutable, and disallows       
   /// memory movement and reallocation                                       
   ///   @param enable - whether to enable or disable the constant state      
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   constexpr void Block<TYPE>::MakeConst(bool enable) noexcept {
      if (enable)
         mState += DataState::Constant;
      else
         mState -= DataState::Constant;
   }

   /// Make memory block type-constrained                                     
   /// Doesn't allow insertion of data types that differ from the contained   
   /// one. Disallows any type mutations. Used extensively by TMany and other 
   /// statically typed Block equivalents.                                    
   ///   @param enable - whether to enable or disable the typed state         
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   constexpr void Block<TYPE>::MakeTypeConstrained(bool enable) noexcept {
      if (enable)
         mState += DataState::Typed;
      else
         mState -= DataState::Typed;
   }

   /// Make memory block exclusive (aka an `OR` block)                        
   /// OR blocks are handled differently by Langulus::Flow, when executing    
   /// verbs in them, or when used as arguments for verbs.                    
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   constexpr void Block<TYPE>::MakeOr() noexcept {
      mState += DataState::Or;
   }

   /// Make memory block inclusive (aka an `AND` block)                       
   /// AND blocks are handled conventionally by Langulus::Flow, as they are   
   /// the default type of blocks                                             
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   constexpr void Block<TYPE>::MakeAnd() noexcept {
      mState -= DataState::Or;
   }

   /// Set memory block phase to past                                         
   /// This is used to mark missing past symbols for Langulus::Flow           
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   constexpr void Block<TYPE>::MakePast() noexcept {
      mState -= DataState::Future;
      mState += DataState::Missing;
   }

   /// Set memory block phase to future                                       
   /// This is used to mark missing future symbols for Langulus::Flow         
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   constexpr void Block<TYPE>::MakeFuture() noexcept {
      mState += DataState::MissingFuture;
   }
   
   /// Set memory block phase to neutral                                      
   /// This restores the default state of any kind of missing block, so that  
   /// it is processed conventionally by Langulus::Flow                       
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   constexpr void Block<TYPE>::MakeNow() noexcept {
      mState -= DataState::MissingFuture;
   }
   
   /// Mark the block as missing. It will no longer participate in hashing    
   /// and comparisons                                                        
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   constexpr void Block<TYPE>::MakeMissing() noexcept {
      mState += DataState::Missing;
   }

   /// Get entry array when block is sparse (const)                           
   ///   @attention entries exist only for sparse containers                  
   ///   @return the array of entries                                         
   template<class TYPE> LANGULUS(INLINED)
   auto Block<TYPE>::GetEntries() IF_UNSAFE(noexcept) -> Allocation const** {
      LglsAssumeDev(IsSparse(),
         "Entries do not exist for dense container");
      LglsAssumeDev(mEntry,
         "Entries do not exist for sparse containers which are out of jurisdiction");
      LglsAssumeDev(mRawSparse,
         "No memory available");
      LglsAssumeDev(mReserved,
         "Invalid reserved count - don't use it from maps!");
      return const_cast<const Allocation**>(
         reinterpret_cast<Allocation**>(mRawSparse + mReserved));
   }

   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   auto Block<TYPE>::GetEntries() const IF_UNSAFE(noexcept) -> Allocation const* const* {
      return const_cast<Block*>(this)->GetEntries();
   }
   
   /// Flat check if block contains verbs                                     
   ///   @return true if the block contains immediate verbs                   
   template<class TYPE>
   bool Block<TYPE>::IsExecutable() const noexcept {
      if (IsEmpty())
         return false;

      // Do an early return if possible                                 
      if constexpr (TypeErased) {
         if (mType->mIsExecutable)
            return true;
      }
      else if constexpr (CT::DerivedFrom<TYPE, Flow::Verb>)
         return true;

      // Depending on immediate contents...                             
      bool exe = false;
      ForEach(
         [&exe](const Trait& trait) noexcept {
            // Scan deeper into traits, because they're not deep,       
            // unless they're being executed                            
            exe = trait.IsExecutable();
            return not exe;
         },
         [&exe](const Construct& cst) noexcept {
            // Scan deeper into constructs, because they're not deep,   
            // unless they're being executed                            
            exe = cst.IsExecutable();
            return not exe;
         }
      );

      return exe;
   }

   /// Deep (nested and slower) check if block contains verbs                 
   ///   @return true if the block contains anything executable deeply        
   template<class TYPE>
   bool Block<TYPE>::IsExecutableDeep() const noexcept {
      // Do an early return if possible                                 
      if (IsExecutable())
         return true;

      // Depending on deep contents...                                  
      bool exe = false;
      ForEachDeep<false, true>(
         [&exe](const Block<>& group) noexcept {
            exe = group.IsExecutable();
            return not exe;
         }
      );

      return exe;
   }

} // namespace Langulus::Annies