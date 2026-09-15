///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Block.hpp"
#include "../../Index.inl"


namespace Langulus::Annies
{
   
   /// Get the internal byte array with a given offset                        
   /// This is lowest level access and checks nothing                         
   ///   @attention assumes block is allocated                                
   ///   @param byteOffset - number of bytes to add                           
   ///   @return pointer to the selected raw data offset                      
   template<class TYPE> LANGULUS(ALWAYS_INLINED) IF_UNSAFE(constexpr)
   Byte* Block<TYPE>::At(const Offset byteOffset) IF_UNSAFE(noexcept) {
      LANGULUS_ASSUME(DevAssumes, mRaw, "Invalid memory");
      return mRaw + byteOffset;
   }

   template<class TYPE> LANGULUS(ALWAYS_INLINED) IF_UNSAFE(constexpr)
   const Byte* Block<TYPE>::At(Offset byte_offset) const IF_UNSAFE(noexcept) {
      return const_cast<Block*>(this)->At(byte_offset);
   }

   /// Access element at a specific index                                     
   ///   @param idx - the index                                               
   ///   @return the element (or block, if this is type-erased)               
   template<class TYPE> LANGULUS(INLINED)
   decltype(auto) Block<TYPE>::operator[] (CT::Index auto idx) {
      const auto index = SimplifyIndex(idx);
      LANGULUS_ASSERT(index < mCount, Access, "Index out of range");
      if constexpr (TypeErased)
         return GetElement(index);
      else if constexpr (CT::Sparse<TYPE>)
         return LocalRef {GetRaw() + index, mEntry ? GetEntries() + index : nullptr};
      else
         return GetRaw()[index];
   }

   template<class TYPE> LANGULUS(INLINED)
   decltype(auto) Block<TYPE>::operator[] (CT::Index auto idx) const {
      const auto index = SimplifyIndex(idx);
      LANGULUS_ASSERT(index < mCount, Access, "Index out of range");
      if constexpr (TypeErased)  return GetElement(index);
      else                       return GetRaw()[index];
   }
   
   /// Get an element pointer or reference with a given index                 
   /// This is a lower-level routine that does only sparseness checking       
   /// No conversion or copying occurs, only pointer arithmetic               
   ///   @attention assumes the container is typed                            
   ///   @tparam T - the type of data we're accessing                         
   ///   @param idx - simple index for accessing                              
   ///   @param baseOffset - byte offset from the element to apply            
   ///   @return either pointer or reference to the element (depends on T)    
   template<class TYPE> template<CT::Data T> LANGULUS(INLINED) IF_UNSAFE(constexpr)
   decltype(auto) Block<TYPE>::Get(Offset idx) IF_UNSAFE(noexcept) {
      if constexpr (TypeErased) {
         LANGULUS_ASSUME(DevAssumes, mType, "Block is not typed");
         Byte* pointer;
         if (mType->mIsSparse)
            pointer = GetRaw<Byte*>()[idx];
         else
            pointer = At(mType->mSize * idx);

         if constexpr (CT::Dense<T>)
            return *reinterpret_cast<Deref<T>*>(pointer);
         else
            return  reinterpret_cast<Deptr<Deref<T>>*>(pointer);
      }
      else {
         if constexpr (Sparse) {
            if constexpr (CT::Dense<T>)
               return static_cast<Deref<T>&>(*GetRaw()[idx]);
            else
               return static_cast<Deref<T> >(*&GetRaw()[idx]);
         }
         else {
            if constexpr (CT::Dense<T>)
               return static_cast<Deref<T>&>( GetRaw()[idx]);
            else
               return static_cast<Deref<T> >(&GetRaw()[idx]);
         }
      }
   }

   template<class TYPE> template<CT::Data T> LANGULUS(INLINED) IF_UNSAFE(constexpr)
   decltype(auto) Block<TYPE>::Get(Offset idx) const IF_UNSAFE(noexcept) {
      if constexpr (TypeErased) {
         LANGULUS_ASSUME(DevAssumes, mType, "Block is not typed");
         const Byte* pointer;
         if (mType->mIsSparse)
            pointer = GetRaw<Byte*>()[idx];
         else
            pointer = At(mType->mSize * idx);

         if constexpr (CT::Dense<T>)
            return *reinterpret_cast<const Deref<T>*>(pointer);
         else
            return  reinterpret_cast<const Deptr<Deref<T>>*>(pointer);
      }
      else {
         if constexpr (Sparse) {
            if constexpr (CT::Dense<T>)
               return static_cast<const Deref<T>&>(*GetRaw()[idx]);
            else
               return static_cast<const Deptr<Deref<T>>*>( GetRaw()[idx]);
         }
         else {
            if constexpr (CT::Dense<T>)
               return static_cast<const Deref<T>&>( GetRaw()[idx]);
            else
               return static_cast<const Deptr<Deref<T>>*>(&GetRaw()[idx]);
         }
      }
   }

   /// A safe (only in safe-mode!) way to get Nth deep entry                  
   /// Will utilize any staticly typed deep containers, if available          
   template<class TYPE> LANGULUS(INLINED) IF_UNSAFE(constexpr)
   decltype(auto) Block<TYPE>::GetDeep(Offset idx) IF_UNSAFE(noexcept) {
      if constexpr (TypeErased) {
         LANGULUS_ASSUME(DevAssumes, IsDeep(), "Block is not deep");
         return Get<Block<>>(idx);
      }
      else {
         static_assert(CT::Deep<Decay<TYPE>>, "Block is not deep");
         return DenseCast(GetRaw() + idx);
      }
   }

   template<class TYPE> LANGULUS(INLINED) IF_UNSAFE(constexpr)
   decltype(auto) Block<TYPE>::GetDeep(Offset idx) const IF_UNSAFE(noexcept) {
      if constexpr (TypeErased) {
         LANGULUS_ASSUME(DevAssumes, IsDeep(), "Block is not deep");
         return Get<Block<>>(idx);
      }
      else {
         static_assert(CT::Deep<Decay<TYPE>>, "Block is not deep");
         return DenseCast(GetRaw() + idx);
      }
   }

   /// Get an element at an index, trying to interpret it as T                
   /// No conversion or copying shall occur in this routine, only pointer     
   /// arithmetic based on RTTI                                               
   ///   @tparam T - the type to interpret to                                 
   ///   @param index - the index                                             
   ///   @return either pointer or reference to the element (depends on T)    
   template<class TYPE> template<CT::Data T>
   decltype(auto) Block<TYPE>::As(CT::Index auto index) {
      if constexpr (TypeErased) {
         // Type-erased As                                              
         // First quick type stage for fast access - this will ignore   
         // sparsity if possible                                        
         LANGULUS_ASSUME(DevAssumes, mType, "Block is not typed");
         if (mType->Is<T>())
            return Get<T>(SimplifyIndex(index));
      
         // Optimize if we're interpreting as a container               
         if constexpr (CT::Deep<T>) {
            LANGULUS_ASSERT(IsDeep(), Access, "Type mismatch");
            const auto idx = SimplifyIndex(index);
            auto& result = GetDeep(idx);

            if constexpr (CT::Typed<T>) {
               // Additional check, if T is a typed block               
               LANGULUS_ASSERT(result.template IsSimilar<TypeOf<T>>(),
                  Access, "Deep type mismatch");
            }

            if constexpr (CT::Sparse<T>)
               return reinterpret_cast<T >(&result);
            else
               return reinterpret_cast<T&>( result);
         }

         // Fallback stage for compatible bases and mappings            
         const auto idx = SimplifyIndex(index);
         LANGULUS_ASSERT(idx < mCount, Access, "Index out of range");

         RTTI::Base base;
         if (not mType->template GetBase<T>(0, base)) {
            // There's still a chance if this container is resolvable   
            // This is the third and final stage                        
            auto resolved = GetElementResolved(idx);
            if (resolved.mType->template IsExact<T>()) {
               // Element resolved to a compatible type, so get it      
               return resolved.template Get<T>();
            }
            else if (resolved.mType->template GetBase<T>(0, base)) {
               // Get base memory of the resolved element and access    
               return resolved.GetBaseMemory(base)
                  .template Get<T>(idx % base.mCount);
            }

            // All stages of interpretation failed                      
            // Don't log this, because it will spam the crap out of us  
            // That throw is used by ForEach to handle irrelevant types 
            LANGULUS_THROW(Access, "Type mismatch");
         }

         // Get base memory of the required element and access          
         return 
            GetElementDense(idx / base.mCount)
               .GetBaseMemory(base)
                  .template Get<T>(idx % base.mCount);
      }
      else {
         // Statically optimized As                                     
         if constexpr (CT::Same<TYPE, T>) {
            // Notice that this can ignore sparsity                     
            return Get<T>(SimplifyIndex(index));
         }
         else if constexpr (CT::Deep<T>) {
            // Optimize if we're interpreting as a container            
            static_assert(CT::Deep<Decay<TYPE>>, "Type mismatch");
            const auto idx = SimplifyIndex(index);
            auto& result = GetRaw()[idx];

            if constexpr (CT::Typed<T>) {
               // Additional check, if T is a typed block               
               if (not result.template IsSimilar<TypeOf<T>>())
                  LANGULUS_THROW(Access, "Deep type mismatch");
            }

            if constexpr (CT::Sparse<T>) {
               if constexpr (Sparse)
                  return reinterpret_cast<T >( result);
               else
                  return reinterpret_cast<T >(&result);
            }
            else {
               if constexpr (Sparse)
                  return reinterpret_cast<T&>(*result);
               else
                  return reinterpret_cast<T&>( result);
            }
         }
         else if constexpr (CT::Sparse<T>
         and requires (Decay<TYPE>* e) { dynamic_cast<T>(e); }) {
            // Do a dynamic_cast whenever possible                      
            const auto idx = SimplifyIndex(index);
            Decvq<T> ptr;
            if constexpr (Sparse)
               ptr = dynamic_cast<T>( GetRaw()[idx]);
            else
               ptr = dynamic_cast<T>(&GetRaw()[idx]);
            LANGULUS_ASSERT(ptr, Access, "Failed dynamic_cast");
            return ptr;
         }
         else {
            // Do a quick static_cast whenever possible                 
            const auto idx = SimplifyIndex(index);

            if constexpr (CT::Sparse<T>) {
               if constexpr (Sparse)
                  return static_cast<T >( GetRaw()[idx]);
               else
                  return static_cast<T >(&GetRaw()[idx]);
            }
            else {
               if constexpr (Sparse)
                  return static_cast<T&>(*GetRaw()[idx]);
               else
                  return static_cast<T&>( GetRaw()[idx]);
            }
         }
      }
   }

   template<class TYPE> template<CT::Data T> LANGULUS(ALWAYS_INLINED)
   decltype(auto) Block<TYPE>::As(CT::Index auto index) const {
      return const_cast<Block&>(*this).As<T>(index);
   }
   
   /// Select an initialized region from the memory block                     
   ///   @param start - starting element index                                
   ///   @param count - number of elements to remain after 'start'            
   ///   @return the block representing the region                            
   template<class TYPE> template<CT::Block THIS>
   LANGULUS(INLINED) IF_UNSAFE(constexpr)
   THIS Block<TYPE>::Select(Offset start, Count count) IF_UNSAFE(noexcept) {
      LANGULUS_ASSUME(DevAssumes, start + count <= mCount, "Out of limits");

      if (count == 0) {
         THIS result {Disown(reinterpret_cast<const THIS&>(*this))};
         result.ResetMemory();
         return Abandon(result);
      }

      THIS result {reinterpret_cast<const THIS&>(*this)};
      result.mCount = count;
      result.mReserved -= start;
      result.mRaw  += start * GetStride();
      return Abandon(result);
   }

   /// Select an initialized region from the memory block (const)             
   ///   @param start - starting element index                                
   ///   @param count - number of elements                                    
   ///   @return the block representing the region                            
   template<class TYPE> template<CT::Block THIS>
   LANGULUS(ALWAYS_INLINED) IF_UNSAFE(constexpr)
   THIS Block<TYPE>::Select(Offset start, Count count) const IF_UNSAFE(noexcept) {
      auto result = const_cast<Block*>(this)->template Select<THIS>(start, count);
      result.MakeConst();
      return result;
   }

   /// Get an element in container, and wrap it in a mutable dense block      
   ///   @attention the result will be empty if a sparse nullptr              
   ///   @tparam COUNT - number of indirections to remove                     
   ///   @param index - index of the element inside the block                 
   ///   @return the dense mutable memory block for the element               
   template<class TYPE> template<Count COUNT> LANGULUS(ALWAYS_INLINED)
   Block<> Block<TYPE>::GetElementDense(Offset index) {
      return GetElement(index).template GetDense<COUNT>();
   }

   template<class TYPE> template<Count COUNT> LANGULUS(ALWAYS_INLINED)
   Block<> Block<TYPE>::GetElementDense(Offset index) const {
      auto result = GetElement(index).template GetDense<COUNT>();
      result.MakeConst();
      return result;
   }
   
   /// Get the dense and most concrete block of an element inside the block   
   ///   @attention the element might be empty if resolved a sparse nullptr   
   ///   @param index - index of the element inside the block                 
   ///   @return the dense resolved memory block for the element              
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   Block<> Block<TYPE>::GetElementResolved(Offset index) {
      return GetElement(index).GetResolved();
   }

   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   Block<> Block<TYPE>::GetElementResolved(Offset index) const {
      auto result = GetElement(index).GetResolved();
      result.MakeConst();
      return result;
   }

   /// Public function, to get a specific element block                       
   /// The resulting container will be a static view                          
   ///   @param index - the element's index                                   
   ///   @return the element's block                                          
   template<class TYPE> LANGULUS(INLINED)
   Block<> Block<TYPE>::GetElement(Offset index) IF_UNSAFE(noexcept) {
      LANGULUS_ASSUME(DevAssumes, index < mReserved, "Index out of range");
      Block result = GetElementInner(index);
      result.mState -= DataState::Or;
      return result;
   }

   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   Block<> Block<TYPE>::GetElement(Offset index) const IF_UNSAFE(noexcept) {
      auto result = const_cast<Block*>(this)->GetElement(index);
      result.MakeConst();
      return result;
   }
   
   /// Get a specific element block (inner, unsafe)                           
   ///   @attention will not make the resulting block static or const         
   ///   @param index - the element's index                                   
   ///   @return the element's block                                          
   template<class TYPE> LANGULUS(INLINED)
   Block<> Block<TYPE>::GetElementInner(Offset index) IF_UNSAFE(noexcept) {
      LANGULUS_ASSUME(DevAssumes, mRaw, "Invalid memory");
      Block result {*this};
      result.mCount = 1;
      result.mRaw += index * mType->mSize;
      return result;
   }

   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   Block<> Block<TYPE>::GetElementInner(Offset index) const IF_UNSAFE(noexcept) {
      return const_cast<Block*>(this)->GetElementInner(index);
   }

   /// Get a deep memory sub-block                                            
   ///   @param index - the index to get, indices are mapped as the following:
   ///      0 always refers to this block                                     
   ///      [1; mCount] always refer to subblocks in this block               
   ///      [mCount + 1; mCount + N] refer to subblocks in the first subblock 
   ///                               N being the size of that subblock        
   ///      ... and so on ...                                                 
   ///   @return a pointer to the block or nullptr if index is invalid        
   template<class TYPE>
   Block<>* Block<TYPE>::GetBlockDeep(Count index) noexcept {
      // Zero index always returns this                                 
      if (index == 0)
         return this;
      if (not IsDeep())
         return nullptr;

      --index;

      // [1; mCount] always refer to subblocks in this block            
      if (index < mCount)
         return &GetDeep(index);

      index -= mCount;

      // [mCount + 1; mCount + N] refer to subblocks in local blocks    
      auto data = &GetDeep();
      const auto dataEnd = data + mCount;
      while (data != dataEnd) {
         const auto subpack = data->GetBlockDeep(index + 1);
         if (subpack)
            return subpack;

         index -= data->GetCountDeep() - 1; //TODO excess loops here, should be retrieved from GetElementDeep above as an optimization
         ++data;
      }

      return nullptr;
   }

   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   const Block<>* Block<TYPE>::GetBlockDeep(Count index) const noexcept {
      return const_cast<Block*>(this)->GetBlockDeep(index);
   }

   /// Get a deep element block                                               
   ///   @param index - the index to get                                      
   ///   @return the element block                                            
   template<class TYPE>
   Block<> Block<TYPE>::GetElementDeep(Count index) noexcept {
      if (not IsDeep())
         return index < mCount ? GetElement(index) : Block<> {};

      auto data = &GetDeep();
      const auto dataEnd = data + mCount;
      while (data != dataEnd) {
         const auto subpack = data->GetElementDeep(index);
         if (subpack)
            return subpack;

         index -= data->GetCountElementsDeep(); //TODO excess loops here, should be retrieved from GetElementDeep above as an optimization
         ++data;
      }

      return {};
   }

   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   Block<> Block<TYPE>::GetElementDeep(Count index) const noexcept {
      auto result = const_cast<Block*>(this)->GetElementDeep(index);
      result.MakeConst();
      return result;
   }
   
   /// Get the resolved first mutable element of this block                   
   ///   @attention assumes this block is valid and has at least one element  
   ///   @return the mutable resolved first element                           
   template<class TYPE> LANGULUS(INLINED)
   Block<> Block<TYPE>::GetResolved() {
      LANGULUS_ASSUME(DevAssumes, IsTyped(),  "Block is not typed");
      LANGULUS_ASSUME(DevAssumes, mCount > 0, "Block is empty");

      if (mType->mResolver)
         return mType->mResolver(GetDense<CountMax>().mRaw);
      else
         return GetDense<CountMax>();
   }

   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   Block<> Block<TYPE>::GetResolved() const {
      auto result = const_cast<Block*>(this)->GetResolved();
      result.MakeConst();
      return result;
   }

   /// Dereference first contained pointer                                    
   ///   @attention throws if type is incomplete and origin was reached       
   ///   @attention assumes this block is valid and has exactly one element   
   ///   @tparam COUNT - how many levels of indirection to remove?            
   ///   @return the mutable denser first element                             
   template<class TYPE> template<Count COUNT> LANGULUS(INLINED)
   Block<> Block<TYPE>::GetDense() {
      static_assert(COUNT > 0, "COUNT must be greater than 0");
      LANGULUS_ASSUME(DevAssumes, IsTyped(),  "Block is not typed");
      LANGULUS_ASSUME(DevAssumes, mCount > 0, "Block is empty");

      Block copy {*this};
      copy.mCount = 1;

      if constexpr (not TypeErased and Dense)
         return copy;
      else if constexpr (not TypeErased and COUNT == 1) {
         // Statically dereference once                                 
         static_assert(CT::Complete<Deptr<TYPE>>,
            "Trying to interface incomplete data as dense");

         if (mEntry)
            copy.mEntry = *GetEntries();

         copy.mRaw = *mRawSparse;
         copy.mType = copy.mType->mDeptr;
      }
      else if (copy.mType->mIsSparse) {
         // Dereference as much as needed at runtime                    
         Count counter = COUNT;
         if (mEntry)
            copy.mEntry = *GetEntries();

         while (counter and copy.mType->mIsSparse) {
            LANGULUS_ASSERT(copy.mType->mDeptr, Access,
               "Trying to interface incomplete data `", copy.mType, "` as dense");

            copy.mRaw = *mRawSparse;
            copy.mType = copy.mType->mDeptr;
            if (mEntry and counter != COUNT)
               copy.mEntry = Allocator::Find(copy.mType, copy.mRaw);
            --counter;
         }
      }

      return copy;
   }

   template<class TYPE> template<Count COUNT> LANGULUS(ALWAYS_INLINED)
   Block<> Block<TYPE>::GetDense() const {
      auto result = const_cast<Block*>(this)->template GetDense<COUNT>();
      result.MakeConst();
      return result;
   }

   /// Dereference first contained pointer once                               
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   Block<> Block<TYPE>::operator * () {
      return GetDense<1>();
   }

   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   Block<> Block<TYPE>::operator * () const {
      return GetDense<1>();
   }
   
   /// Swap two elements inside this container                                
   ///   @param from_ - first index                                           
   ///   @param to_ - second index                                            
   template<class TYPE> LANGULUS(INLINED)
   void Block<TYPE>::SwapIndices(CT::Index auto from_, CT::Index auto to_) {
      const auto from = SimplifyIndex(from_);
      const auto to   = SimplifyIndex(to_);
      if (from >= mCount or to >= mCount or from == to)
         return;

      if constexpr (TypeErased) {
         auto fblock = GetElementInner(from);
         auto tblock = GetElementInner(to);
         fblock.Swap(Abandon(tblock));
      }
      else {
         auto data = GetRaw();
         TYPE temp = ::std::move(data[to]);
         data[to]  = ::std::move(data[from]);
         IntentAssign(data[from], Abandon(temp));
      }
   }
   
   /// Swap contents of this block, with the contents of another, using       
   /// a temporary block                                                      
   ///   @param rhs - the block to swap with                                  
   template<class TYPE> template<class T1> requires CT::Block<Deint<T1>>
   void Block<TYPE>::Swap(T1&& rhs) {
      using S = IntentOf<decltype(rhs)>;
      using ST = Conditional<TypeErased, TypeOf<S>, Block>;
      LANGULUS_ASSUME(DevAssumes, mCount and DeintCast(rhs).mCount == mCount,
         "Invalid count");

      // Type-erased pointers (void*) are always acceptable             
      //TODO add this check to IsSimilar(Block auto) directly?
      LANGULUS_ASSUME(DevAssumes, (
          DeintCast(rhs).IsSimilar(*this)
      or (DeintCast(rhs).template IsSimilar<void*>() and IsSparse())
      ), "Type mismatch on swap", ": ", DeintCast(rhs).GetType(), " != ", GetType());

      using B = Block<TypeOf<ST>>;
      B temporary {mState, mType};
      temporary.AllocateFresh(temporary.RequestSize(mCount));
      temporary.mCount = mCount;

      // Move this to temporary                                         
      temporary.CreateWithIntent(Move(*this));
      // Assign all elements from rhs to this                           
      reinterpret_cast<B*>(this)->AssignWithIntent(S::Nest(rhs));
      // Assign all elements from temporary to rhs                      
      reinterpret_cast<B*>(&DeintCast(rhs))->AssignWithIntent(Abandon(temporary));
      // Cleanup temporary                                              
      temporary.FreeInner();
      Allocator::Deallocate(const_cast<Allocation*>(temporary.mEntry));
   }

   /// Gather items from source container, and fill this one                  
   ///   @tparam REVERSE - iterate in reverse?                                
   ///   @param source - container to gather from, type acts as filter        
   ///   @return the number of gathered elements                              
   template<class TYPE> template<bool REVERSE> LANGULUS(INLINED)
   Count Block<TYPE>::GatherFrom(const CT::Block auto& source) {
      return source.template GatherInner<REVERSE>(*this);
   }

   /// Gather items of specific state from source container, and fill this one
   ///   @tparam REVERSE - iterate in reverse?                                
   ///   @param source - container to gather from, type acts as filter        
   ///   @param state - state filter                                          
   ///   @return the number of gathered elements                              
   template<class TYPE> template<bool REVERSE> LANGULUS(INLINED)
   Count Block<TYPE>::GatherFrom(const CT::Block auto& source, DataState state) {
      return source.template GatherPolarInner<REVERSE>(GetType(), *this, state);
   }

   /// Get the index of the biggest/smallest element                          
   ///   @tparam INDEX - either IndexBiggest or IndexSmallest                 
   ///   @return the index of the biggest element T inside this block         
   template<class TYPE> template<Index INDEX> LANGULUS(INLINED)
   Index Block<TYPE>::GetIndex() const IF_UNSAFE(noexcept) {
      if constexpr (not TypeErased and CT::Sortable<TYPE, TYPE>) {
         if (IsEmpty())
            return IndexNone;

         auto data = GetRaw();
         const auto dataEnd = data + mCount;
         auto selection = data++;
         while (data != dataEnd) {
            if constexpr (INDEX == IndexBiggest) {
               if (*data > *selection)
                  selection = data;
            }
            else if constexpr (INDEX == IndexSmallest) {
               if (*data < *selection)
                  selection = data;
            }
            else static_assert(false, "Unsupported index");

            ++data;
         }

         return selection - GetRaw();
      }
      else return IndexNone;
   }

   /// Get the index of element that repeats the most times                   
   ///   @param count - [out] count the number of repeats for the mode        
   ///   @return the index of the first found mode                            
   template<class TYPE>
   Index Block<TYPE>::GetIndexMode(Count& count) const IF_UNSAFE(noexcept) {
      if constexpr (not TypeErased and CT::Comparable<TYPE, TYPE>) {
         if (IsEmpty()) {
            count = 0;
            return IndexNone;
         }

         auto data = GetRaw();
         const auto dataEnd = data + mCount;
         decltype(data) best = nullptr;
         Count best_count = 0;
         while (data != dataEnd) {
            Count counter = 0;
            auto tail = data;
            while (tail != dataEnd) {
               if (*data == *tail)
                  ++counter;

               if (counter + (dataEnd - tail) <= best_count)
                  break;

               ++tail;
            }

            if (counter > best_count or not best) {
               best_count = counter;
               best = data;
            }

            ++data;
         }

         count = best_count;
         return best - data;
      }
      else return IndexNone;
   }
   
   /// Return a handle to an element                                          
   ///   @attention when this block is type-erased, T1 is assumed to be of    
   ///      the same sparseness                                               
   ///   @param index - the element index                                     
   ///   @return the handle                                                   
   template<class TYPE> template<class T1> LANGULUS(INLINED)
   auto Block<TYPE>::GetHandle(const Offset index) IF_UNSAFE(noexcept) {
      using T = Decvq<Conditional<CT::Handle<T1>, TypeOf<T1>, T1>>;

      if constexpr (not TypeErased) {
         // Either sparse or not type-erased                            
         if constexpr (Sparse) {
            static_assert(CT::Sparse<T>, "Sparseness mismatch");
            return Handle<T> {GetRaw<T>() + index, GetEntries() + index};
         }
         else {
            static_assert(CT::Dense<T1>, "Sparseness mismatch");
            return Handle<T> {GetRaw<T>() + index, mEntry};
         }
      }
      else {
         // Type erased and dense                                       
         LANGULUS_ASSUME(DevAssumes, IsSparse() == CT::Sparse<T>,
            "Sparseness mismatch");

         if constexpr (CT::Sparse<T>)
            return Handle<T> {GetRaw<T>() + index, GetEntries() + index};
         else if constexpr (not CT::TypeErased<T>)
            return Handle<T> {GetRaw<T>() + index, mEntry};
         else
            return Handle<T> {mRaw + index * GetStride(), mEntry};
      }
   }

   template<class TYPE> template<class T1> LANGULUS(ALWAYS_INLINED)
   auto Block<TYPE>::GetHandle(const Offset index) const IF_UNSAFE(noexcept) {
      return const_cast<Block*>(this)->template GetHandle<T1>(index).MakeConst();
   }

   /// Select region from the memory block - unsafe and may return memory     
   /// that has not been initialized yet (for internal use only)              
   ///   @attention assumes block is typed and allocated                      
   ///   @param start - starting element index                                
   ///   @param count - number of elements                                    
   ///   @return the block representing the region                            
   template<class TYPE> LANGULUS(INLINED)
   Block<TYPE> Block<TYPE>::CropInner(const Offset start, const Count count)
   const IF_UNSAFE(noexcept) {
      LANGULUS_ASSUME(DevAssumes, mRaw,
         "Block is not allocated");
      LANGULUS_ASSUME(DevAssumes, IsTyped(),
         "Block is not typed");

      Block result {*this};
      result.mCount = count;
      result.mRaw += start * mType->mSize;
      return result;
   }
   
   /// Constrain an index to the limits of the current block                  
   ///   @param idx - the index to constrain                                  
   ///   @return the constrained index or a special one of constrain fails    
   template<class TYPE> LANGULUS(INLINED)
   Index Block<TYPE>::Constrain(const Index idx) const IF_UNSAFE(noexcept) {
      const auto result = idx.Constrained(mCount);
      if (result == IndexBiggest)
         return GetIndex<IndexBiggest>();
      else if (result == IndexSmallest)
         return GetIndex<IndexSmallest>();
      else if (result == IndexMode) {
         [[maybe_unused]] Count unused;
         return GetIndexMode(unused);
      }

      return result;
   }

   /// Convert an index to an offset                                          
   /// Complex indices will be fully constrained                              
   /// Unsigned/signed integers are directly forwarded without any overhead   
   ///   @attention assumes T is correct for type-erased containers           
   ///   @tparam SAFE - whether to throw if index is beyond initialized count 
   ///   @param index - the index to simplify                                 
   ///   @return the simplified index, as a simple offset                     
   template<class TYPE> template<bool SAFE, CT::Index INDEX> LANGULUS(INLINED)
   Offset Block<TYPE>::SimplifyIndex(const INDEX index) const
   noexcept(not LANGULUS_SAFE() and CT::BuiltinInteger<INDEX>) {
      if constexpr (CT::Same<INDEX, Index>) {
         // This is the most safe path, throws on errors                
         if constexpr (SAFE)
            return Constrain(index).GetOffset();
         else
            return Constrain(index).GetOffsetUnsafe();
      }
      else {
         // Unsafe, works only on assumptions                           
         // Using an integer index explicitly makes a statement, that   
         // you know what you're doing                                  
         LANGULUS_ASSUME(UserAssumes, 
            not SAFE or index < static_cast<INDEX>(mCount),
            "Integer index out of range"
         );

         if constexpr (CT::Signed<INDEX>) {
            LANGULUS_ASSUME(UserAssumes, index >= 0, 
               "Integer index is below zero, "
               "use Index for reverse indices instead"
            );
         }

         return index;
      }
   }
   
   /// Access last element                                                    
   ///   @return a mutable reference to the last element                      
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   decltype(auto) Block<TYPE>::Last() {
      return (*this)[mCount - 1];
   }

   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   decltype(auto) Block<TYPE>::Last() const {
      return (*this)[mCount - 1];
   }
}