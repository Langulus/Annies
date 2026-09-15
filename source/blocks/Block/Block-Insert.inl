///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Block.hpp"
#include "../../text/Text.hpp"
#include "Block-Indexing.inl"
#include <Langulus/Core/Memory.hpp>


namespace Langulus::Annies
{

   /// Allocate 'count' elements and fill the container with zeroes           
   /// If T is not CT::Nullifiable, this function does default construction,  
   /// which would be slower, than batch zeroing                              
   ///   @param count - number of elements to zero-construct                  
   template<class TYPE> LANGULUS(INLINED)
   void Block<TYPE>::Null(const Count count) {
      if constexpr (not TypeErased) {
         if constexpr (CT::Nullifiable<TYPE>) {
            if (count < mReserved)
               AllocateLess(count);
            else
               AllocateMore<false, true>(count);

            ZeroMemory(GetRaw(), count);
         }
         else New(count);
      }
      else TODO();
   }

   /// Extend the container by default construction, and return the new part  
   ///   @attention if extending memory without jurisdiction, the container   
   ///      will take authority and diverge                                   
   ///   @param count - the number of elements to extend by                   
   ///   @return a container that represents only the extended part           
   template<class TYPE> template<CT::Block THIS> LANGULUS(INLINED)
   THIS Block<TYPE>::Extend(const Count count) {
      const auto previousCount = mCount;
      AllocateMore<true>(mCount + count);
      const auto newRegion = CropInner(previousCount, count);
      return reinterpret_cast<const THIS&>(newRegion);
   }

   /// Create N new elements, using default construction                      
   /// Elements will be added to the back of the container                    
   ///   @param count - number of elements to construct                       
   ///   @return the number of new elements                                   
   template<class TYPE> LANGULUS(INLINED)
   Count Block<TYPE>::New(const Count count)
   requires (TypeErased or CT::Defaultable<TYPE>) {
      BranchOut();
      AllocateMore(mCount + count);
      CropInner(mCount, count).CreateDefault();
      mCount += count;
      return count;
   }
   
   /// Create N new elements, using the provided arguments for construction   
   /// Elements will be added to the back of the container                    
   ///   @param count - number of elements to construct                       
   ///   @param arguments... - constructor arguments, all forwarded together  
   ///      for each instance of T                                            
   ///   @return the number of new elements                                   
   template<class TYPE> template<class...A> LANGULUS(INLINED)
   Count Block<TYPE>::New(const Count count, A&&...arguments)
   requires (TypeErased or ::std::constructible_from<TYPE, A...>) {
      LANGULUS_ASSUME(UserAssumes, count, "Zero count not allowed");
      BranchOut();
      AllocateMore(mCount + count);
      CropInner(mCount, count).Create(Forward<A>(arguments)...);
      mCount += count;
      return count;
   }

   /// Inner intent function for a contiguous range-insertion                 
   ///   @tparam FORCE - insert even if types mismatch, by making this block  
   ///      deep with provided type - use void to disable                     
   ///   @tparam MOVE_ASIDE - true to allocate more elements, and move any    
   ///      elements at index to the right, in order to fit the insertion     
   ///   @param index - the offset at which to start inserting                
   ///   @param data - block to insert, with intent or not                    
   template<class TYPE>
   template<class FORCE, bool MOVE_ASIDE, class T1> requires CT::Block<Deint<T1>>
   void Block<TYPE>::InsertBlockInner(CT::Index auto index, T1&& data) {
      // If both sides are void, then we have a type-erased insertion   
      using S = IntentOf<decltype(data)>;
      using T = Conditional<TypeErased, TypeOf<Deint<T1>>, TYPE>;

      if constexpr (CT::CanBeDeepened<FORCE, Block> and MOVE_ASIDE) {
         // Type may mutate                                             
         bool depened;
         if constexpr (CT::TypeErased<T>)
            depened = Mutate<FORCE>(DeintCast(data).GetType()->mDecvq);
         else
            depened = Mutate<Decvq<T>, FORCE>();

         // If reached, then type mutated to a deep type                
         if (depened) {
            FORCE temp;
            temp.template InsertBlockInner<void, true>(IndexBack, S::Nest(data));
            Insert<void>(index, Abandon(temp));
            return;
         }
      }
      else {
         // Type can't mutate, but we still have to check if compatible 
         LANGULUS_ASSERT(IsSimilar(DeintCast(data)), Meta,
            "Inserting incompatible type `", DeintCast(data).GetType(),
            "` to container of type `", GetType(), '`'
         );
      }

      // If reached, then we have binary compatible type, so allocate   
      const auto count = DeintCast(data).GetCount();
      const auto idx   = SimplifyIndex<false>(index);
      BranchOut();

      if constexpr (MOVE_ASIDE) {
         AllocateMore(mCount + count);

         if (idx < mCount) {
            // Move memory if required                                  
            LANGULUS_ASSERT(GetUses() == 1, Move,
               "Moving elements that are used from multiple places");

            // We're moving to the right, so make sure we do it in      
            // reverse to avoid any potential overlap                   
            const auto moved = mCount - idx;
            CropInner(idx + count, moved).template CreateWithIntent<true>(
               Abandon(CropInner(idx, moved)));
         }
      }

      // Construct data in place                                        
      CropInner(idx, count).CreateWithIntent(S::Nest(data));
      mCount += count;
   }

   /// Inner intent insertion function                                        
   ///   @tparam FORCE - insert even if types mismatch, by making this block  
   ///      deep with provided type - use void to disable                     
   ///   @tparam MOVE_ASIDE - true to allocate more elements, and move any    
   ///      elements at index to the right, in order to fit the insertion     
   ///   @param index - the offset at which to insert                         
   ///   @param item - item to insert, with or without intent                 
   template<class TYPE> template<class FORCE, bool MOVE_ASIDE>
   void Block<TYPE>::InsertInner(CT::Index auto index, auto&& item) {
      using S = IntentOf<decltype(item)>;
      using T = Conditional<TypeErased, TypeOf<S>, TYPE>;

      if constexpr (CT::Similar<S, Describe>) {
         // We're using descriptor constructors                         
         // For this to work, contained type must be known              
         auto type = GetType();
         LANGULUS_ASSERT(type, Meta,
            "Unknown type, can't insert via descriptor");
         LANGULUS_ASSERT(type->mDescriptorConstructor, Meta,
            "Type is not descriptor-constructible");

         const auto idx = SimplifyIndex<false>(index);
         BranchOut();

         if constexpr (MOVE_ASIDE) {
            AllocateMore(mCount + 1);

            if (idx < mCount) {
               // Move memory if required                               
               LANGULUS_ASSERT(GetUses() == 1, Move,
                  "Moving elements that are used from multiple places");

               // We're moving to the right, so make sure we do it in   
               // reverse to avoid any potential overlap                
               const auto moved = mCount - idx;
               CropInner(idx + 1, moved).template CreateWithIntent<true>(
                  Abandon(CropInner(idx, moved)));
            }
         }

         CropInner(idx, 1).CreateDescribe(*item);
      }
      else {
         if constexpr (CT::CanBeDeepened<FORCE, Block> and MOVE_ASIDE) {
            // Type may mutate                                          
            if (Mutate<T, FORCE>()) {
               // If reached, then type mutated to a deep type          
               FORCE temp {S::Nest(item)};
               Insert<void, true>(index, Abandon(temp));
               return;
            }
         }
         else {
            // We still have to mutate if untyped - this also acts      
            // as a runtime type-check                                  
            Mutate<T, void>();
         }

         const auto idx = SimplifyIndex<false>(index);
         BranchOut();

         // If reached, we have compatible type, so allocate            
         if constexpr (MOVE_ASIDE) {
            AllocateMore(mCount + 1);

            if (idx < mCount) {
               // Move memory if required                               
               LANGULUS_ASSERT(GetUses() == 1, Move,
                  "Moving elements that are used from multiple places");

               // We're moving to the right, so make sure we do it in   
               // reverse to avoid any potential overlap                
               const auto moved = mCount - idx;
               CropInner(idx + 1, moved).template CreateWithIntent<true>(
                  Abandon(CropInner(idx, moved)));
            }
         }

         GetHandle<T>(idx).CreateWithIntent(S::Nest(item), mType);
      }

      ++mCount;
   }

   /// Insert an element, or an array of elements                             
   ///   @tparam FORCE - insert even if types mismatch, by making this block  
   ///      deep with provided type - use void to disable                     
   ///   @tparam MOVE_ASIDE - true to allocate more elements, and move any    
   ///      elements at index to the right, in order to fit the insertion     
   ///   @param index - the index at which to insert                          
   ///   @param item - item to unfold and insert, with or without intent      
   ///   @return the number of inserted elements after unfolding              
   template<class TYPE> template<class FORCE, bool MOVE_ASIDE>
   Count Block<TYPE>::UnfoldInsert(CT::Index auto index, auto&& item) {
      using S = IntentOf<decltype(item)>;
      using T = TypeOf<S>;
      
      if constexpr (CT::Array<T>) {
         if constexpr (CT::StringLiteral<T>) {
            // Implicitly convert string literals to Text containers    
            InsertInner<FORCE, MOVE_ASIDE>(index, Text {S::Nest(item)});
            return 1;
         }
         else {
            // Insert the array                                         
            InsertBlockInner<FORCE, MOVE_ASIDE>(index,
               S::Nest(MakeBlock<Block>(S::Nest(item))));
            return ExtentOf<T>;
         }
      }
      else {
         // Some of the arguments might still be used directly to       
         // make an element, forward these to standard insertion here   
         InsertInner<FORCE, MOVE_ASIDE>(index, S::Nest(item));
         return 1;
      }
   }

   /// Insert an element, or an array of elements, if not found               
   ///   @tparam FORCE - insert even if types mismatch, by making this block  
   ///      deep with provided type - use void to disable                     
   ///   @tparam MOVE_ASIDE - true to allocate more elements, and move any    
   ///      elements at index to the right, in order to fit the insertion     
   ///   @param index - the index at which to insert                          
   ///   @param item - item to unfold and insert, with or without intent      
   ///   @return the number of inserted elements after unfolding              
   template<class TYPE> template<class FORCE, bool MOVE_ASIDE>
   Count Block<TYPE>::UnfoldMerge(CT::Index auto index, auto&& item) {
      using S = IntentOf<decltype(item)>;
      using T = TypeOf<S>;
      
      if constexpr (CT::Array<T>) {
         if constexpr (CT::StringLiteral<T>) {
            // Implicitly convert string literals to Text containers    
            if (CT::Void<FORCE> and not IsSimilar<Text>())
               return 0;

            Text text {S::Nest(item)};
            if (not IsSimilar<Text>() or not Find<false>(text)) {
               InsertInner<FORCE, MOVE_ASIDE>(index, Abandon(text));
               return 1;
            }
         }
         else {
            // Insert the array                                         
            using DT = Deext<T>;
            if (CT::Void<FORCE> and not IsSimilar<DT>())
               return 0;

            const auto data = Block<DT>::From(item);
            if (not IsSimilar<DT>()
            or  not FindBlock<false>(data, IndexFront)) {
               InsertBlockInner<FORCE, MOVE_ASIDE>(index, S::Nest(data));
               return ExtentOf<T>;
            }
         }
      }
      else {
         // Some of the arguments might still be used directly to       
         // make an element, forward these to standard insertion here   
         if (CT::Void<FORCE> and not IsSimilar<T>())
            return 0;

         if (not IsSimilar<T>() or not Find<false>(DeintCast(item))) {
            InsertInner<FORCE, MOVE_ASIDE>(index, S::Nest(item));
            return 1;
         }
      }

      return 0;
   }

   /// Insert elements at a given index, with or without intent               
   ///   @tparam FORCE - insert even if types mismatch, by making this block  
   ///      deep with provided type - use void to disable                     
   ///   @tparam MOVE_ASIDE - true to allocate more elements, and move any    
   ///      elements at index to the right, in order to fit the insertion     
   ///   @param idx - the index at which to insert                            
   ///   @param t1 - the first item to insert                                 
   ///   @param tn... - the rest of items to insert (optional)                
   ///   @return number of inserted elements                                  
   template<class TYPE> 
   template<class FORCE, bool MOVE_ASIDE, class T1, class...TN> LANGULUS(INLINED)
   Count Block<TYPE>::Insert(CT::Index auto idx, T1&& t1, TN&&...tn)
   requires (TypeErased or CT::UnfoldMakableFrom<TYPE, T1, TN...>) {
      Count inserted = 0;
        inserted += UnfoldInsert<FORCE, MOVE_ASIDE>(idx, Forward<T1>(t1));
      ((inserted += UnfoldInsert<FORCE, MOVE_ASIDE>(idx, Forward<TN>(tn))), ...);
      return inserted;
   }
   
   /// Insert all elements of a block at an index, with or without intent     
   ///   @tparam FORCE - insert even if types mismatch, by making this block  
   ///      deep with provided type - use void to disable                     
   ///   @tparam MOVE_ASIDE - true to allocate more elements, and move any    
   ///      elements at index to the right, in order to fit the insertion     
   ///   @param index - index to insert thems at                              
   ///   @param other - the block to insert                                   
   ///   @return the number of inserted elements                              
   template<class TYPE> template<class FORCE, bool MOVE_ASIDE, class T>
   requires CT::Block<Deint<T>> LANGULUS(INLINED)
   Count Block<TYPE>::InsertBlock(CT::Index auto index, T&& other) {
      using S = IntentOf<decltype(other)>;
      auto& rhs = DeintCast(other);
      const auto count = rhs.GetCount();
      if (not count)
         return 0;

      InsertBlockInner<FORCE, MOVE_ASIDE>(index, Forward<T>(other));

      if constexpr (ResetsOnMove<S>() and TypeOf<S>::Ownership) {
         // All elements were moved, only empty husks remain            
         // so destroy them, and discard ownership of 'other'           
         rhs.Free();
      }

      return count;
   }
   
   /// Merge a single element, using a semantic or not                        
   /// Element will be pushed only if not found in block                      
   ///   @tparam FORCE - insert even if types mismatch, by making this block  
   ///      deep with provided type - use void to disable                     
   ///   @tparam MOVE_ASIDE - true to allocate more elements, and move any    
   ///      elements at index to the right, in order to fit the insertion     
   ///   @param index - the index at which to insert                          
   ///   @param t1 - the first item to insert                                 
   ///   @param tn... - the rest of items to insert (optional)                
   ///   @return the number of inserted elements                              
   template<class TYPE> 
   template<class FORCE, bool MOVE_ASIDE, class T1, class...TN> LANGULUS(INLINED)
   Count Block<TYPE>::Merge(CT::Index auto index, T1&& t1, TN&&...tn)
   requires (TypeErased or CT::UnfoldMakableFrom<TYPE, T1, TN...>) {
      Count inserted = 0;
        inserted += UnfoldMerge<FORCE, MOVE_ASIDE>(index, Forward<T1>(t1));
      ((inserted += UnfoldMerge<FORCE, MOVE_ASIDE>(index, Forward<TN>(tn))), ...);
      return inserted;
   }

   /// Search for a sequence of elements, and if not found - insert it        
   ///   @tparam FORCE - insert even if types mismatch, by making this block  
   ///      deep with provided type - use void to disable                     
   ///   @tparam MOVE_ASIDE - true to allocate more elements, and move any    
   ///      elements at index to the right, in order to fit the insertion     
   ///   @param index - index to insert at                                    
   ///   @param other - the block to search for, and eventually insert        
   ///   @return the number of inserted elements                              
   template<class TYPE> template<class FORCE, bool MOVE_ASIDE, class T>
   requires CT::Block<Deint<T>> LANGULUS(INLINED)
   Count Block<TYPE>::MergeBlock(CT::Index auto index, T&& other) {
      using S = IntentOf<decltype(other)>;
      Count inserted = 0;
      if (not FindBlock(DeintCast(other), IndexFront))
         inserted += InsertBlock<FORCE, MOVE_ASIDE>(index, S::Nest(other));
      return inserted;
   }
   
   /// Construct an item of this container's type at the specified position   
   /// by forwarding A... as constructor arguments                            
   /// If this container is type-erased and exact constructor signatures      
   /// aren't reflected, the following constructors will be attempted:        
   ///   1. If A is a single argument of exactly the same type, the reflected 
   ///      move constructor will be used, if available                       
   ///   2. If A is empty, the reflected default constructor is used          
   ///   3. If A is not empty, not exactly same as the contained type, or     
   ///      is more than a single argument, then all arguments will be        
   ///      wrapped in a Many, and then forwarded to the descriptor-          
   ///      constructor, if such is reflected                                 
   ///   If none of these constructors are available, or block is not typed,  
   ///   this function throws Except::Allocate                                
   ///   @tparam MOVE_ASIDE - true to allocate more elements, and move any    
   ///      elements at index to the right, in order to fit the insertion     
   ///   @param idx - the index to emplace at                                 
   ///   @param arguments... - the arguments to forward to constructor        
   ///   @return 1 if the element was emplaced successfully                   
   template<class TYPE> template<bool MOVE_ASIDE, class...A> LANGULUS(INLINED)
   decltype(auto) Block<TYPE>::Emplace(CT::Index auto idx, A&&...arguments)
   requires (TypeErased or ::std::constructible_from<TYPE, A...>) {
      const auto offset = SimplifyIndex<false>(idx);

      if constexpr (MOVE_ASIDE) {
         AllocateMore(mCount + 1);

         if (offset < mCount) {
            // Move memory if required                                  
            LANGULUS_ASSERT(GetUses() == 1, Move,
               "Emplacing elements to memory block, used from multiple places, "
               "requires memory to move");

            // We're moving to the right, so make sure we do it in      
            // reverse to avoid any overlap                             
            const auto tail = mCount - offset;
            CropInner(offset + 1, tail).template CreateWithIntent<true>(
               Abandon(CropInner(offset, tail)));
         }
      }

      auto selection = CropInner(offset, 1);
      selection.Create(Forward<A>(arguments)...);
      ++mCount;
      if constexpr (TypeErased)
         return selection;
      else if constexpr (Sparse)
         return selection[0];
      else
         return *selection.GetRaw();
   }
   
   /// Wrap all contained elements inside a sub-block, making this one deep   
   ///   @tparam T - the type of deep container to use                        
   ///   @tparam TRANSFER_OR - whether to send the current orness deeper      
   ///   @return a reference to this container                                
   template<class TYPE> template<CT::Deep T, bool TRANSFER_OR> LANGULUS(INLINED)
   T& Block<TYPE>::Deepen() {
      static_assert(CT::CanBeDeepened<T, Block>,
         "This Block can't be deepened with T");
      LANGULUS_ASSERT(not IsTypeConstrained() or IsSimilar<T>(), Mutate,
         "Can't deepen with incompatible type");

      // Back up the state so that we can restore it if not moved over  
      [[maybe_unused]] const DataState state = mState.mState & DataState::Or;
      if constexpr (not TRANSFER_OR)
         mState -= state;

      // Allocate a new T and move this inside it                       
      Block<T> wrapper;
      wrapper.AllocateFresh(wrapper.RequestSize(1));
      new (wrapper.mRaw) Block {*this};
      wrapper.mCount = 1;
      wrapper.mState -= DataState::Typed;
      *this = wrapper;
      
      // Restore the state if not moved over                            
      if constexpr (not TRANSFER_OR)
         mState += state;

      return Get<T>();
   }

   /// Intent-insert that uses the best approach to push anything inside      
   /// container in order to keep hierarchy and states, but also reuse memory 
   /// Static data will always be preserved as static.                        
   ///   @tparam ALLOW_CONCAT - whether or not concatenation is allowed       
   ///   @tparam FORCE - insert even if types mismatch, by making this block  
   ///      deep with provided type - use void to disable                     
   ///   @param index - the index at which to insert (if needed)              
   ///   @param value - the value to smart-push                               
   ///   @param state - a state to apply after pushing is done                
   ///   @return the number of pushed items (zero if unsuccessful)            
   template<class TYPE> template<bool ALLOW_CONCAT, class FORCE>
   Count Block<TYPE>::SmartPush(
      CT::Index auto index, auto&& value, DataState state
   ) {
      using S = IntentOf<decltype(value)>;
      using T = TypeOf<S>;

      if constexpr (CT::Deep<T>) {
         // We're inserting a deep item, so we can do various smart     
         // things before inserting, like absorbing and concatenating   
         if (not DeintCast(value).IsValid())
            return 0;

         const bool stateCompliant = CanFitState(DeintCast(value));
         if (IsEmpty() and stateCompliant) {
            // We can directly absorb. This will preserve staticness.   
            Free();
            BlockTransfer(S::Nest(value));
            return 1;
         }

         if constexpr (ALLOW_CONCAT) {
            // Let's try concatenating                                  
            // It will deepen in order to preserve staticness, if FORCE 
            // is not void                                              
            try {
               if (SmartConcat<FORCE>(index, stateCompliant, S::Nest(value), state))
                  return true;
            }
            catch (...) {}
         }
      }

      // If reached, then none of the above succeeded - just push.      
      // It will deepen in order to preserve staticness, if FORCE is    
      // not void                                                       
      return SmartPushInner<FORCE>(index, S::Nest(value), state);
   }

   
   /// Smart concatenation inner call, used by smart push                     
   /// Attempts to either concatenate elements, or deepen and push block      
   /// Static data will always be preserved as static, by deepening the block 
   ///   @tparam FORCE - insert even if types mismatch, by making this block  
   ///      deep with provided type - use void to disable                     
   ///   @param index - the place to insert at                                
   ///   @param sc - is this block state-compliant for insertion              
   ///   @param value - the value to concatenate                              
   ///   @param state - the state to apply after concatenation                
   ///   @return the number of inserted elements                              
   template<class TYPE> template<class FORCE, class T1> requires CT::Deep<Deint<T1>>
   LANGULUS(INLINED) Count Block<TYPE>::SmartConcat(
      CT::Index auto index, bool sc, T1&& value, DataState state
   ) {
      if constexpr (TypeErased) {
         // At runtime...                                               
         const bool typeCompliant = IsUntyped()
            or (not CT::Void<FORCE> and DeintCast(value).IsDeep())
            or IsSimilar(DeintCast(value));

         if (not IsConstant() and typeCompliant and sc
         and not (GetCount() > 1 and not mState.IsOr() and state.IsOr())) {
            if (IsUntyped()) {
               // Block insert never mutates, so make sure type         
               // is valid before insertion                             
               SetType<false>(DeintCast(value).GetType());
            }
            else if constexpr (not CT::Void<FORCE>) {
               if ((not IsDeep() and DeintCast(value).IsDeep())
               or IsStatic() or DeintCast(value).IsStatic())
                  Deepen<FORCE, false>();
            }

            const auto cat = InsertBlock<void>(index, Forward<T1>(value));
            mState += state;
            return cat;
         }
      }
      else {
         // As statically optimized as possible                         
         const bool typeCompliant = IsSimilar(DeintCast(value)) or
            (not CT::Void<FORCE> and DeintCast(value).IsDeep());

         if (not IsConstant() and typeCompliant and sc
         and not (GetCount() > 1 and not mState.IsOr() and state.IsOr())) {
            if constexpr (CT::CanBeDeepened<FORCE, Block>) {
               if ((not IsDeep() and DeintCast(value).IsDeep())
               or IsStatic() or DeintCast(value).IsStatic())
                  Deepen<FORCE, false>();
            }

            const auto cat = InsertBlock<void>(index, Forward<T1>(value));
            mState += state;
            return cat;
         }
      }

      return 0;
   }
   
   /// Inner smart-push function                                              
   ///   @tparam FORCE - insert even if types mismatch, by making this block  
   ///      deep with provided type - use void to disable                     
   ///   @param index - the place to insert at                                
   ///   @param value - the value to concatenate                              
   ///   @param state - the state to apply after concatenation                
   ///   @return the number of inserted elements                              
   template<class TYPE> template<class FORCE> LANGULUS(INLINED)
   Count Block<TYPE>::SmartPushInner(
      CT::Index auto index, auto&& value, DataState state
   ) {
      using S = IntentOf<decltype(value)>;
      using T = TypeOf<S>;

      if constexpr (TypeErased) {
         if (IsUntyped() and IsInvalid()) {
            // Mutate-insert inside untyped container                   
            SetState(mState + state);
            return Insert<void>(index, S::Nest(value));
         }
         else if (IsSimilar<T>()) {
            // Insert to a same-typed container                         
            SetState(mState + state);
            return Insert<void>(index, S::Nest(value));
         }
         else if (IsEmpty() and IsTyped() and not IsTypeConstrained()) {
            // If incompatibly typed but empty and not constrained, we  
            // can still reset the container and reuse it               
            Reset();
            SetState(mState + state);
            return Insert<void>(index, S::Nest(value));
         }
         else if (IsDeep()) {
            // Already deep, push value wrapped in a container          
            //TODO hmm, what if this is deep but of specific Block<type>, that doesn't correspond to Many? should be checked inside Deepen!
            if (mCount > 1 and not mState.IsOr() and state.IsOr()) {
               // If container is not or-compliant after insertion, we  
               // need to add another layer                             
               Deepen<Many>();
            }

            SetState(mState + state);
            return Insert<void>(index, Many {S::Nest(value)});
         }

         if constexpr (not CT::Void<FORCE>) {
            // If this is reached, all else failed, but we are allowed  
            // to deepen, so do it                                      
            Deepen<FORCE, false>();
            SetState(mState + state);
            return Insert<void>(index, FORCE {S::Nest(value)});
         }
         else return 0;
      }
      else {
         if constexpr (CT::Similar<TYPE, T>) {
            // Insert to a same-typed container                         
            SetState(mState + state);
            return Insert<void>(index, S::Nest(value));
         }
         else if constexpr (CT::Deep<Decay<TYPE>>) {
            // Already deep, push value wrapped in a container          
            //TODO hmm, what if this is deep but of specific Block<type>, that doesn't correspond to Many? should be checked inside Deepen!
            if (mCount > 1 and not mState.IsOr() and state.IsOr()) {
               // If container is not or-compliant after insertion, we  
               // need to add another layer                             
               Deepen<Many>();
            }

            SetState(mState + state);
            return Insert<void>(index, Many {S::Nest(value)});
         }
         else return 0;
      }
   }
   
   /// Concatenate this, and another block into a new block                   
   ///   @param rhs - block concatenate with (right side), can have intent    
   ///   @return the concatenated container                                   
   template<class TYPE> template<CT::Block THIS, class T1>
   requires CT::Block<Deint<T1>> LANGULUS(INLINED)
   THIS Block<TYPE>::ConcatBlock(T1&& rhs) const {
      using S = IntentOf<decltype(rhs)>;
      auto& lhs = reinterpret_cast<const THIS&>(*this);

      if (IsEmpty())
         return {S::Nest(rhs)};
      else if (DeintCast(rhs).IsEmpty())
         return lhs;

      if (GetUses() == 1 and not mType->mDestructor and not mType->mIsSparse) {
         // Silently append rhs to this block's memory, to save on a    
         // reallocation. This block will remain the same, but it will  
         // diverge, if changed in the future. This is only allowed     
         // if the container contains dense indestructible items        
         const auto countBackup = mCount;
         const_cast<Block*>(this)->template
            InsertBlock<void>(IndexBack, S::Nest(rhs));
         Block result = *this;
         const_cast<Block*>(this)->mCount = countBackup;
         return reinterpret_cast<THIS&>(result);
      }

      // Allocate a new concatenated container, and push inside         
      THIS result;
      if constexpr (TypeErased)
         result.template SetType<false>(DeintCast(rhs).GetType());
      result.AllocateFresh(result.RequestSize(mCount + DeintCast(rhs).GetCount()));
      result.template InsertBlock<void, false>(IndexBack, Refer(lhs));
      result.template InsertBlock<void, false>(IndexBack, S::Nest(rhs));
      return Abandon(result);
   }

   /// Call default constructors in a region and initialize memory            
   ///   @attention never modifies any block state                            
   ///   @attention assumes block elements are not initialized, despite having
   ///      mCount set                                                        
   template<class TYPE>
   void Block<TYPE>::CreateDefault() {
      LANGULUS_ASSUME(DevAssumes, mCount and mCount <= mReserved,
         "Count outside limits", '(', mCount, " > ", mReserved);
      LANGULUS_ASSUME(DevAssumes, GetUses() == 1,
         "Data is referenced from multiple locations");

      if constexpr (not TypeErased) {
         if constexpr (Sparse) {
            // Zero pointers and entries                                
            ZeroMemory(GetRaw(), mCount);
            ZeroMemory(GetEntries(), mCount);
         }
         else if constexpr (CT::Nullifiable<TYPE>) {
            // Zero the dense memory (optimization)                     
            ZeroMemory(GetRaw(), mCount);
         }
         else if constexpr (CT::Defaultable<TYPE>) {
            // Construct requested elements one by one                  
            auto to = GetRaw();
            const auto toEnd = to + mCount;
            while (to != toEnd)
               new (to++) TYPE {};
         }
         else static_assert(false,
            "Trying to default-construct elements that are "
            "incapable of default-construction");
      }
      else {
         if (mType->mIsSparse) {
            // Zero pointers and entries                                
            ZeroMemory(mRawSparse, mCount);
            ZeroMemory(GetEntries(), mCount);
         }
         else if (mType->mIsNullifiable) {
            // Zero the dense memory (optimization)                     
            ZeroMemory(mRaw, mCount * mType->mSize);
         }
         else {
            LANGULUS_ASSERT(
               mType->mDefaultConstructor, Construct,
               "Can't default-construct elements"
               " - no default constructor reflected"
            );

            // Construct requested elements one by one                  
            auto to = mRaw;
            const auto stride = mType->mSize;
            const auto toEnd = to + mCount * stride;
            while (to != toEnd) {
               mType->mDefaultConstructor(to);
               to += stride;
            }
         }
      }
   }
   
   /// Call descriptor constructors in a region, initializing memory          
   ///   @attention never modifies any block state                            
   ///   @attention assumes block elements are not initialized, despite having
   ///      mCount set                                                        
   ///   @param desc - the descriptor to pass on to constructors              
   ///      each new element will use the same descriptor                     
   template<class TYPE> template<class...A>
   void Block<TYPE>::CreateDescribe(A&&...arguments) {
      static_assert(sizeof...(A) > 0, "Bad number of arguments");
      LANGULUS_ASSUME(DevAssumes, mCount and mCount <= mReserved,
         "Count outside limits", '(', mCount, " > ", mReserved);
      LANGULUS_ASSUME(DevAssumes, GetUses() == 1,
         "Data is referenced from multiple locations");

      Many descriptor;
      if constexpr (sizeof...(A) == 1) {
         using A1 = FirstOf<A...>;
         if constexpr (CT::Similar<A1, Describe>)
            (descriptor = ... = *arguments);
         else
            descriptor = Many {Forward<A>(arguments)...};
      }
      else descriptor = Many {Forward<A>(arguments)...};

      if constexpr (not TypeErased) {
         using DT = Decay<TYPE>;
         static_assert(CT::DescriptorMakable<TYPE>,
            "T is not descriptor-constructible");

         if constexpr (Sparse) {
            // Bulk-allocate the required count, construct each         
            // instance and push the pointers                           
            auto lhsPtr = GetRaw();
            auto lhsEnt = GetEntries();
            const auto lhsEnd = lhsPtr + mCount;
            const auto allocation = Allocator::Allocate(
               MetaDataOf<DT>(), sizeof(DT) * mCount);
            allocation->Keep(mCount - 1);

            auto rhs = allocation->template As<DT*>();
            while (lhsPtr != lhsEnd) {
               new (rhs) DT (Describe(descriptor));
               *(lhsPtr++) = rhs;
               *(lhsEnt++) = allocation;
               ++rhs;
            }
         }
         else {
            // Construct all dense elements in place                    
            auto lhs = GetRaw();
            const auto lhsEnd = lhs + mCount;
            while (lhs != lhsEnd)
               new (lhs++) DT (Describe(descriptor));
         }
      }
      else {
         LANGULUS_ASSUME(DevAssumes, IsTyped(),
            "Block is expected to be typed");
         LANGULUS_ASSERT(
            mType->mDescriptorConstructor, Construct,
            "Can't descriptor-construct ", '`', mType,
            "` - no descriptor-constructor reflected");

         if (mType->mDeptr) {
            if (not mType->mDeptr->mIsSparse) {
               // Bulk-allocate the required count, construct each      
               // instance and set the pointers                         
               auto lhsPtr = mRawSparse;
               auto lhsEnt = GetEntries();
               const auto lhsEnd = lhsPtr + mCount;
               const auto allocation = Allocator::Allocate(
                  mType->mOrigin,
                  mType->mOrigin->mSize * mCount
               );
               allocation->Keep(mCount - 1);

               auto rhs = allocation->GetBlockStart();
               while (lhsPtr != lhsEnd) {
                  mType->mOrigin->mDescriptorConstructor(rhs, descriptor);
                  *(lhsPtr++) = rhs;
                  const_cast<const Allocation*&>(*(lhsEnt++)) = allocation;
                  rhs += mType->mOrigin->mSize;
               }
            }
            else {
               // We need to allocate another indirection layer         
               TODO();
            }
         }
         else {
            // Construct all dense elements in place                    
            auto lhs = mRaw;
            const auto lhsEnd = lhs + mCount * mType->mSize;
            while (lhs != lhsEnd) {
               mType->mDescriptorConstructor(lhs, descriptor);
               lhs += mType->mSize;
            }
         }
      }
   }
   
   /// Construct region.mCount items of THIS container's type in the specified
   /// region by forwarding A... as constructor arguments                     
   /// If this container is type-erased, exact constructor signatures aren't  
   /// reflected, and the following stock constructors will be attempted:     
   ///   1. If A is empty, the reflected default constructor is used, if such 
   ///      has been reflected.                                               
   ///   2. If A is a single argument of exactly the same type, then the      
   ///      appropriate reflected intent constructor will be used.            
   ///   3. If A is not empty, not exactly same as the contained type, or     
   ///      is more than a single argument, then all arguments will be        
   ///      wrapped in a Neat, and then forwarded to the descriptor-          
   ///      constructor, if such is reflected for the type.                   
   ///   If none of these constructors are available, this function throws    
   ///   Except::Construct                                                    
   ///   @attention this is assumed to have no initialized elements, despite  
   ///      having its mCount set                                             
   ///   @attention be mindful when initializing multiple elements with move  
   ///      or abandon intents - the arguments will be lost after the first   
   ///      element is initialized.                                           
   ///   @param arguments... - the arguments to forward to constructor        
   template<class TYPE> template<class...A>
   void Block<TYPE>::Create(A&&...arguments) {
      LANGULUS_ASSUME(DevAssumes, mCount and mCount <= mReserved,
         "Count outside limits", '(', mCount, " > ", mReserved);
      LANGULUS_ASSUME(DevAssumes, GetUses() == 1,
         "Data is referenced from multiple locations");

      if constexpr (sizeof...(A) == 0) {
         // Attempt default construction                                
         CreateDefault();
      }
      else if constexpr (not TypeErased) {
         // Construct by directly checking if arguments satisfy a       
         // constructor signature, knowing what the contained type is   
         // at compile time                                             
         if constexpr (::std::constructible_from<TYPE, A...>) {
            auto lhs = GetRaw();
            const auto lhsEnd = lhs + mCount;
            while (lhs != lhsEnd)
               new (lhs++) TYPE (Forward<A>(arguments)...);

            if constexpr (sizeof...(A) == 1 and Sparse and not CT::Disowned<A...>) {
               // We just copied a pointer multiple times, make sure    
               // we reference the memory behind it, if we own it       
               auto ent = GetEntries();
               auto allocation = Allocator::Find(MetaDataOf<Deptr<TYPE>>(), *(--lhs));
               if (allocation) {
                  const auto entEnd = ent + mCount;
                  while (ent != entEnd)
                     *(ent++) = allocation;
                  const_cast<Allocation*>(allocation)->Keep(mCount);
                  if constexpr (CT::Referencable<Deptr<TYPE>>)
                     (*GetRaw())->Reference(static_cast<int>(mCount));
               }
               else memset(ent, 0, mCount * sizeof(void*));
            }
         }
         else CreateDescribe(Forward<A>(arguments)...);
      }
      else {
         // Constructing type-erased items                              
         // We expect that type has been previously set                 
         LANGULUS_ASSUME(DevAssumes, IsTyped(),
            "Block was expected to be typed");

         if constexpr (sizeof...(A) == 1) {
            using F = Decvq<Deref<FirstOf<A...>>>;

            if constexpr (CT::Similar<F, Describe>) {
               // We have a descriptor for argument, forward it to the  
               // reflected descriptor constructor, if any              
               // Initialize only the first element, and then refer it  
               // for the rest of the elements.                         
               CreateDescribe(Forward<A>(arguments)...);
            }
            else if (IsSimilar<Deint<F>>()) {
               // One argument, that is the same as the contained type, 
               // so just invoke the refer-construction for each newly  
               // allocated item                                        
               if constexpr (CT::Sparse<Deint<F>>) {
                  // Initialize sparse elemets                          
                  auto lhs = GetRaw<void*>();
                  const auto lhsEnd = lhs + mCount;
                  while (lhs != lhsEnd) {
                     ((*lhs) = ... = DeintCast(arguments));
                     ++lhs;
                  }

                  if constexpr (not CT::Disowned<F>) {
                     // We just copied a pointer multiple times, make   
                     // sure we reference the memory behind it, if we   
                     // own it                                          
                     auto ent = GetEntries();
                     auto allocation = Allocator::Find(mType->mDeptr, *(--lhs));
                     if (allocation) {
                        const auto entEnd = ent + mCount;
                        while (ent != entEnd)
                           *(ent++) = allocation;
                        const_cast<Allocation*>(allocation)->Keep(mCount);
                        if (mType->mReference)
                           mType->mReference(*GetRaw<void*>(), static_cast<int>(mCount));
                     }
                     else memset(ent, 0, mCount * sizeof(void*));
                  }
               }
               else if constexpr (CT::Moved<F> or CT::Abandoned<F>) {
                  // Initialize dense elements                          
                  RTTI::FMoveConstruct mover;

                  if constexpr (CT::Moved<F>)
                     mover = mType->mMoveConstructor;
                  else
                     mover = mType->mAbandonConstructor;

                  if (mover) {
                     auto lhs = mRaw;
                     const auto lhsEnd = lhs + GetBytesize();
                     while (lhs != lhsEnd) {
                        (mover(&DeintCast(arguments), lhs), ...);
                        lhs += mType->mSize;
                     }
                  }
                  else CreateDescribe(Forward<A>(arguments)...);
               }
               else {
                  // Initialize dense elements                          
                  RTTI::FCopyConstruct copier;

                  if constexpr (CT::Disowned<F>)
                     copier = mType->mDisownConstructor;
                  else if constexpr (CT::Referred<F>)
                     copier = mType->mReferConstructor;
                  else if constexpr (CT::Copied<F>)
                     copier = mType->mCopyConstructor;
                  else if constexpr (CT::Cloned<F>)
                     copier = mType->mCloneConstructor;
                  else {
                     copier = mType->mReferConstructor;
                     if (not copier)
                        copier = mType->mCopyConstructor;
                     if (not copier)
                        copier = mType->mCloneConstructor;
                  }

                  if (copier) {
                     auto lhs = mRaw;
                     const auto lhsEnd = lhs + GetBytesize();
                     while (lhs != lhsEnd) {
                        (copier(&DeintCast(arguments), lhs), ...);
                        lhs += mType->mSize;
                     }
                  }
                  else CreateDescribe(Forward<A>(arguments)...);
               }
            }
            else CreateDescribe(Forward<A>(arguments)...);
         }
         else CreateDescribe(Forward<A>(arguments)...);
      }
   }

   /// Call intent constructors in a region and initialize memory, by using   
   /// a Block as a source                                                    
   ///   @attention never modifies any block state                            
   ///   @attention assumes none of the elements are constructed              
   ///   @attention assumes blocks types are similar                          
   ///   @tparam REVERSE - calls constructors in reverse, to let you          
   ///                     account for potential memory overlap               
   ///   @param source - the source of the elements to initialize with        
   template<class TYPE>
   template<bool REVERSE, class T1> requires CT::Block<Deint<T1>>
   void Block<TYPE>::CreateWithIntent(T1&& source) {
      using S = IntentOf<decltype(source)>;
      const auto count = DeintCast(source).mCount;
      LANGULUS_ASSUME(DevAssumes, count and count <= mReserved,
         "Count outside limits", '(', count, " > ", mReserved);

      // Type-erased pointers (void*) are acceptable                    
      LANGULUS_ASSUME(DevAssumes, 
            (DeintCast(source).IsSimilar(*this)
         or (DeintCast(source).template IsSimilar<void*>() and IsSparse())
         or (DeintCast(source).IsSparse() and IsSimilar<void*>())),
         "Type mismatch on creation", ": ", DeintCast(source).GetType(), " != ", GetType());

      using B = Conditional<TypeErased, TypeOf<S>, Block>;
      using T = TypeOf<B>;
      auto& mthis = BlockCast<B>(*this);
      auto& other = BlockCast<B>(DeintCast(source));

      if constexpr (not CT::TypeErased<T>) {
         // Leverage the fact, that containers are statically typed     
         if constexpr (CT::Sparse<T>) {
            using DT = Deptr<T>;

            if constexpr (S::Shallow) {
               // Shallow pointer transfer                              
               ShallowBatchPointerConstruction(Forward<T1>(source));
            }
            else if constexpr (CT::Unallocatable<Decay<T>> or not CT::CloneMakable<T>) {
               // We early-return with an enforced shallow pointer      
               // transfer, because its requesting to clone             
               // unallocatable/unclonable/abstract data                
               ShallowBatchPointerConstruction(Refer(Forward<T1>(source)));
            }
            else if constexpr (CT::Sparse<DT> or not CT::Resolvable<T>) {
               // If contained type is not resolvable, or its deptr     
               // version is still a pointer, we can coalesce all       
               // clones into a single allocation (optimization)        
               Block<DT> clonedCoalescedSrc;
               clonedCoalescedSrc.AllocateFresh(
                  clonedCoalescedSrc.RequestSize(count));
               clonedCoalescedSrc.mCount = count;

               // Clone each inner element                              
               auto handle = mthis.GetHandle();
               auto dst = clonedCoalescedSrc.GetRaw();
               auto src = other.GetRaw();
               const auto srcEnd = src + count;
               while (src != srcEnd) {
                  IntentNew(dst, Clone(**src));
                  handle.Assign(dst, clonedCoalescedSrc.mEntry);

                  ++dst;
                  ++src;
                  ++handle;
               }

               const_cast<Allocation*>(clonedCoalescedSrc.mEntry)
                  ->Keep(count - 1);
            }
            else {
               // Type can be resolved to objects of varying size, so   
               // we are forced to make a separate allocation for each  
               // element                                               
               TODO();
            }
         }
         else if constexpr (CT::POD<T>) {
            // Both RHS and LHS are dense and POD                       
            // This is a constructor, so we're allowed to cast away     
            // any qualifiers on the left side                          
            auto lhs = mthis.GetRaw();
            auto rhs = other.GetRaw();
            if constexpr (REVERSE)
               MoveMemory(lhs, rhs, count);
            else
               CopyMemory(lhs, rhs, count);
         }
         else {
            // Both RHS and LHS are dense and non POD                   
            // Call constructor for each element (optionally in reverse)
            // This is a constructor, so we're allowed to cast away     
            // any qualifiers on the left side                          
            auto lhs = mthis.GetRaw();
            auto rhs = other.GetRaw();
            if constexpr (REVERSE) {
               lhs += count - 1;
               rhs += count - 1;
            }
            const auto lhsEnd = REVERSE ? lhs - count : lhs + count;
            while (lhs != lhsEnd) {
               if constexpr (CT::Abandoned<S> and not CT::AbandonMakable<T>) {
                  if constexpr (CT::MoveMakable<T>) {
                     // We can fallback to move-construction, but report
                     // a performance warning                           
                     IF_SAFE(Logger::Warning(
                        "Move used, instead of abandon - implement an "
                        "abandon-constructor for type ", NameOf<T>(),
                        " to fix this warning"
                     ));
                     IntentNew(lhs, Move(*rhs));
                  }
                  else static_assert(false, "T is not movable, nor abandon-constructible");
               }
               else IntentNew(lhs, S::Nest(*rhs));

               if constexpr (REVERSE) {
                  --lhs;
                  --rhs;
               }
               else {
                  ++lhs;
                  ++rhs;
               }
            }
         }
      }
      else {
         // Containers are type-erased                                  
         // First make sure that reflected constructors are available   
         // There's no point in iterating anything otherwise            
         if constexpr (S::Move) {
            if constexpr (S::Keep) {
               LANGULUS_ASSERT(
                  mType->mIsSparse or mType->mMoveConstructor, Construct,
                  "Can't move-construct elements "
                  "- no move-constructor was reflected"
               );
            }
            else {
               LANGULUS_ASSERT(
                  mType->mIsSparse or mType->mAbandonConstructor, Construct,
                  "Can't abandon-construct elements "
                  "- no abandon-constructor was reflected"
               );
            }
         }
         else if constexpr (S::Shallow) {
            if constexpr (S::Keep) {
               if constexpr (CT::Referred<S>) {
                  LANGULUS_ASSERT(
                     mType->mIsSparse or mType->mReferConstructor, Construct,
                     "Can't refer-construct elements"
                     " - no refer-constructor was reflected");
               }
               else {
                  LANGULUS_ASSERT(
                     mType->mIsSparse or mType->mCopyConstructor, Construct,
                     "Can't copy-construct elements"
                     " - no copy-constructor was reflected");
               }
            }
            else {
               LANGULUS_ASSERT(
                  mType->mIsSparse or mType->mDisownConstructor, Construct,
                  "Can't disown-construct elements"
                  " - no disown-constructor was reflected");
            }
         }
         else {
            LANGULUS_ASSERT(mType->mCloneConstructor, Construct,
               "Can't clone-construct elements"
               " - no clone-constructor was reflected");
         }

         if (mType->mIsSparse) {
            // Both LHS and RHS are sparse                              
            if constexpr (S::Shallow) {
               // Shallow pointer transfer                              
               ShallowBatchPointerConstruction(Forward<T1>(source));
            }
            else if (not mType->mDeptr->mIsSparse
            and (mType->mIsUnallocatable or not mType->mCloneConstructor)) {
               // We early-return with an enforced shallow pointer      
               // transfer, because its requesting to clone             
               // unallocatable/unclonable/abstract data                
               ShallowBatchPointerConstruction(Refer(Forward<T1>(source)));
            }
            else if (mType->mDeptr->mIsSparse or not mType->mResolver) {
               // If contained type is not resolvable (or is just       
               // another level of indirection), we can coalesce all    
               // clones into a single allocation                       
               Block<> clonedCoalescedSrc {mType->mDeptr};
               clonedCoalescedSrc.AllocateFresh(
                  clonedCoalescedSrc.RequestSize(count));
               clonedCoalescedSrc.mCount = count;

               // Clone each inner element by nesting this call         
               auto lhs = mthis.template GetHandle<void*>();
               auto dst = clonedCoalescedSrc.GetElementInner();
               auto src = other.GetElementInner();
               const auto lhsEnd = lhs + count;
               while (lhs.mValue != lhsEnd.mValue) {
                  dst.CreateWithIntent(Clone(src.template GetDense<1>()));
                  lhs.Assign(dst.mRaw, clonedCoalescedSrc.mEntry);
                  ++dst;
                  ++src;
                  ++lhs;
               }

               const_cast<Allocation*>(clonedCoalescedSrc.mEntry)
                  ->Keep(count - 1);
            }
            else {
               // Type is resolved to dense elements of varying size,   
               // so we are forced to make a separate allocation for    
               // each of them                                          
               TODO();
            }
         }
         else if (mType->mIsPOD) {
            // Both are POD - Copy/Refer/Disown/Move/Abandon/Clone      
            // by memcpy all at once (batch optimization)               
            const auto bytesize = mType->mSize * count;
            if constexpr (REVERSE)
               MoveMemory(mRaw, other.mRaw, bytesize);
            else
               CopyMemory(mRaw, other.mRaw, bytesize);
         }
         else {
            // Both RHS and LHS are dense and non-POD                   
            // We invoke reflected constructors for each element        
            const auto stride = mType->mSize;
            auto lhs = mRaw + (REVERSE ? (count - 1) * stride : 0);
            auto rhs = other.mRaw + (REVERSE ? (count - 1) * stride : 0);
            const auto rhsEnd = REVERSE ? rhs - count * stride : rhs + count * stride;

            while (rhs != rhsEnd) {
               if constexpr (S::Move) {
                  if constexpr (S::Keep)
                     mType->mMoveConstructor(rhs, lhs);
                  else
                     mType->mAbandonConstructor(rhs, lhs);
               }
               else if constexpr (S::Shallow) {
                  if constexpr (S::Keep) {
                     if constexpr (CT::Referred<S>)
                        mType->mReferConstructor(rhs, lhs);
                     else
                        mType->mCopyConstructor(rhs, lhs);
                  }
                  else mType->mDisownConstructor(rhs, lhs);
               }
               else mType->mCloneConstructor(rhs, lhs);

               if constexpr (REVERSE) {
                  lhs -= stride;
                  rhs -= stride;
               }
               else {
                  lhs += stride;
                  rhs += stride;
               }
            }
         }
      }
   }
   
   /// Call a single intent constructor by using a Handle as a source         
   ///   @attention never modifies any block state                            
   ///   @attention assumes none of the elements are constructed              
   ///   @attention assumes blocks types are similar                          
   ///   @param source - the handle to initialize with                        
   template<class TYPE> template<class T1> requires CT::Handle<Deint<T1>>
   void Block<TYPE>::CreateWithIntent(T1&& source) {
      using S = IntentOf<decltype(source)>;
      using T = TypeOf<TypeOf<S>>;

      //static_assert(CT::Sparse<T>,
      //   "Handle isn't sparse");
      LANGULUS_ASSUME(DevAssumes, 1 <= mReserved,
         "Count outside limits (1 > ", mReserved);
      //LANGULUS_ASSUME(DevAssumes, IsSparse(),
      //   "Container is not sparse");
      LANGULUS_ASSUME(DevAssumes, GetUses() == 1,
         "Data is referenced from multiple locations");
      //LANGULUS_ASSUME(DevAssumes, GetCount() == 0, // doesn't work for maps
      //   "Element is already initialized");

      // Type-erased pointers (void*) are acceptable, because they're   
      // required for some internal stuff, although not recommended     
      LANGULUS_ASSUME(DevAssumes, (
         (IsSparse() and CT::Similar<T, void*>) or IsSimilar<T>()),
         "Type mismatch on creation from handle", ": ", GetType(),
         " instead of ", MetaDataOf<T>());

      using LOSSLESS = Conditional<TypeErased, T, TYPE>;
      GetHandle<LOSSLESS>().CreateWithIntent(S::Nest(source), GetType());
   }
   
   /// Batch-optimized intentful pointer constructions                        
   ///   @attention overwrites pointers without dereferencing their memory    
   ///   @attention doesn't modify any container state                        
   ///   @attention assumes blocks are similar                                
   ///   @param source - the source of pointers                               
   template<class TYPE> template<class T> requires CT::Block<Deint<T>>
   void Block<TYPE>::ShallowBatchPointerConstruction(T&& source) {
      using S  = IntentOf<decltype(source)>;
      using ST = TypeOf<S>;

      LANGULUS_ASSUME(DevAssumes, IsSimilar(DeintCast(source)),
         "Type mismatch");

      static_assert(TypeErased or Sparse,
         "This isn't sparse");
      static_assert(ST::TypeErased or ST::Sparse,
         "Source isn't sparse");
      static_assert(S::Shallow,
         "This function works only for shallow intents");

      using P = Conditional<TypeErased,
         Conditional<ST::TypeErased, void*, DecvqAll<TypeOf<ST>>>, void*>;

      const auto count = DeintCast(source).mCount;
      const auto pointersDst = GetRaw<P>();
      const auto pointersSrc = DeintCast(source).template GetRaw<P>();
      const auto entriesDst  = GetEntries();
      const auto entriesSrc  = DeintCast(source).mEntry
         ? DeintCast(source).GetEntries()
         : nullptr;

      LANGULUS_ASSUME(DevAssumes, pointersDst != pointersSrc,
         "Memory source and destination are the same");
      LANGULUS_ASSUME(DevAssumes, entriesDst != entriesSrc,
         "Entries source and destination are the same");

      if constexpr (S::Move) {
         // Move/Abandon                                                
         // Memory may overlap!!!                                       
         MoveMemory(pointersDst, pointersSrc, count);

         if (entriesSrc) {
            // Transfer entries, if available                           
            MoveMemory(entriesDst, entriesSrc, count);
            ZeroMemoryOverlapped(entriesSrc, entriesDst, count);
         }
         else {
            // Otherwise make sure all entries are zero                 
            ZeroMemory(entriesDst, count);
         }

         // Reset source pointers, too, if not abandoned                
         if constexpr (S::Keep)
            ZeroMemoryOverlapped(entriesSrc, entriesDst, count);
      }
      else {
         // Copy/Refer/Disown                                           
         // Memory may NOT overlap!!!                                   
         LANGULUS_ASSUME(DevAssumes, pointersDst >= pointersSrc + count
                                  or pointersDst <  pointersSrc - count,
                        "Can't copy/refer/disown memory onto itself");
         CopyMemory(pointersDst, pointersSrc, count);

         if constexpr (S::Keep) {
            // Copy/Refer                                               
            // Reference each entry, if not disowned                    
            if (entriesSrc) {
               CopyMemory(entriesDst, entriesSrc, count);
               auto entry = entriesDst;
               const auto entryEnd = entry + count;

               if constexpr (not ST::TypeErased) {
                  while (entry != entryEnd) {
                     if (*entry) {
                        const_cast<Allocation*>(*entry)->Keep();
                        if constexpr (CT::Referencable<Deptr<P>>)
                           pointersDst[entry - entriesDst]->Reference(1);
                     }
                     ++entry;
                  }
               }
               else if (DeintCast(source).mType->mReference) {
                  auto reference = DeintCast(source).mType->mReference;
                  while (entry != entryEnd) {
                     if (*entry) {
                        const_cast<Allocation*>(*entry)->Keep();
                        reference(pointersDst[entry - entriesDst], 1);
                     }
                     ++entry;
                  }
               }
               else {
                  while (entry != entryEnd) {
                     if (*entry)
                        const_cast<Allocation*>(*entry)->Keep();
                     ++entry;
                  }
               }
            }
            else {
               // No entries available, make sure all entries are zero  
               ZeroMemory(entriesDst, count);
            }
         }
         else {
            // Disown: make sure all entries are zero                   
            ZeroMemory(entriesDst, count);
         }
      }
   }
   
   /// Call intent assignment in a region                                     
   ///   @attention never modifies any block state                            
   ///   @attention assumes blocks don't overlap (sparse elements may still   
   ///      overlap, but this is handled in the assignment operators)         
   ///   @attention assumes blocks are binary compatible                      
   ///   @param source - the elements to assign                               
   template<class TYPE> template<class T1>
   void Block<TYPE>::AssignWithIntent(T1&& source) requires CT::Block<Deint<T1>> {
      const auto count = source->mCount;
      LANGULUS_ASSUME(DevAssumes, count and count <= mReserved,
         "Count outside limits", '(', count, " > ", mReserved);
      LANGULUS_ASSUME(DevAssumes, GetUses() == 1,
         "Data is referenced from multiple locations");

      // Type-erased pointers (void*) are acceptable                    
      LANGULUS_ASSUME(DevAssumes, (source->IsSimilar(*this)
         or (source->template IsSimilar<void*>() and IsSparse())
         or (source->IsSparse() and IsSimilar<void*>())),
         "Type mismatch on assignment", ": ", source->GetType(), " != ", GetType());

      using OTHER = Deint<T1>;
      using S = IntentOf<decltype(source)>;
      using B = Conditional<TypeErased, OTHER, Block>;
      using T = TypeOf<B>;
      const auto mthis = reinterpret_cast<B*>(this);
      const auto other = reinterpret_cast<B*>(const_cast<OTHER*>(&(*source)));

      if constexpr (not B::TypeErased) {
         if constexpr (B::Sparse) {
            // We're reassigning pointers                               
            using DT = Deptr<T>;

            if constexpr (S::Shallow) {
               // Shallow pointer transfer                              
               FreeInner();
               ShallowBatchPointerConstruction(source.Forward());
            }
            else if constexpr (CT::Unallocatable<Decay<T>> or not CT::CloneAssignable<T>) {
               // We early-return with an enforced shallow pointer      
               // transfer, because its requesting to clone             
               // unallocatable/unclonable/abstract data, such as metas 
               FreeInner();
               ShallowBatchPointerConstruction(Refer(*source));
            }
            else if constexpr (CT::Sparse<DT> or not CT::Resolvable<T>) {
               // If contained type is not resolvable, or its deptr     
               // version is still a pointer, we can coalesce all       
               // clones into a single allocation (optimization)        
               Block<DT> clonedCoalescedSrc;
               clonedCoalescedSrc.AllocateFresh(
                  clonedCoalescedSrc.RequestSize(count));
               clonedCoalescedSrc.mCount = count;

               // Clone each inner element                              
               auto handle = GetHandle<T>(0);
               auto dst = clonedCoalescedSrc.GetRaw();
               auto src = source->GetRaw();
               const auto srcEnd = src + count;
               while (src != srcEnd) {
                  IntentNew(dst, Clone(**src));
                  handle.Assign(dst, clonedCoalescedSrc.mEntry);

                  ++dst;
                  ++src;
                  ++handle;
               }

               const_cast<Allocation*>(clonedCoalescedSrc.mEntry)
                  ->Keep(count - 1);
            }
            else {
               // Type can be resolved to objects of varying size, so   
               // we are forced to make a separate allocation for each  
               // element                                               
               TODO();
            }
         }
         else if constexpr (CT::POD<T>) {
            // Both RHS and LHS are dense and POD                       
            // So we batch-overwrite them at once                       
            auto lhs = mthis->GetRaw();
            auto rhs = other->GetRaw();
            CopyMemory(lhs, rhs, count);
         }
         else {
            // Both RHS and LHS are dense and non POD                   
            // Assign to each element                                   
            auto lhs = mthis->GetRaw();
            auto rhs = other->GetRaw();
            const auto lhsEnd = lhs + count;
            while (lhs != lhsEnd) {
               if constexpr (CT::Abandoned<S> and not CT::AbandonAssignable<T>) {
                  if constexpr (CT::MoveAssignable<T>) {
                     // We can fallback to move-assignment, but report  
                     // a performance warning                           
                     IF_SAFE(Logger::Warning(
                        "Move used, instead of abandon - implement an "
                        "abandon-assignment for type ", NameOf<T>(),
                        " to fix this warning"
                     ));
                     IntentAssign(*lhs, Move(*rhs));
                  }
                  else static_assert(false, "T is not movable, nor abandon-assignable");
               }
               else IntentAssign(*lhs, S::Nest(*rhs));

               ++lhs;
               ++rhs;
            }
         }
      }
      else {
         // Containers are type-erased                                  
         // First make sure that reflected assigners are available      
         // There's no point in iterating anything otherwise            
         if constexpr (S::Move) {
            if constexpr (S::Keep) {
               LANGULUS_ASSERT(
                  mType->mIsSparse or mType->mMoveAssigner, Construct,
                  "Can't move-assign elements "
                  "- no move-assigner was reflected"
               );
            }
            else {
               LANGULUS_ASSERT(
                  mType->mIsSparse or mType->mAbandonAssigner, Construct,
                  "Can't abandon-assign elements "
                  "- no abandon-assigner was reflected"
               );
            }
         }
         else if constexpr (S::Shallow) {
            if constexpr (S::Keep) {
               if constexpr (CT::Referred<S>) {
                  LANGULUS_ASSERT(
                     mType->mIsSparse or mType->mReferAssigner, Construct,
                     "Can't refer-assign elements"
                     " - no refer-assigner was reflected");
               }
               else {
                  LANGULUS_ASSERT(
                     mType->mIsSparse or mType->mCopyAssigner, Construct,
                     "Can't copy-assign elements"
                     " - no copy-assigner was reflected");
               }
            }
            else {
               LANGULUS_ASSERT(
                  mType->mIsSparse or mType->mDisownAssigner, Construct,
                  "Can't disown-assign elements"
                  " - no disown-assigner was reflected");
            }
         }
         else {
            LANGULUS_ASSERT(mType->mCloneAssigner, Construct,
               "Can't clone-assign elements"
               " - no clone-assigner was reflected");
         }

         if (mType->mIsSparse) {
            // Since we're overwriting pointers, we have to dereference 
            // the old ones, but conditionally reference the new ones   
            auto lhs = mthis->template GetHandle<void*>();
            auto rhs = other->template GetHandle<void*>();
            const auto lhsEnd = lhs + count;

            while (lhs.mValue != lhsEnd.mValue) {
               lhs.AssignWithIntent(S::Nest(rhs), mType);

               ++lhs;
               ++rhs;
            }
         }
         else if (mType->mIsPOD) {
            // Both RHS and LHS are dense and POD                       
            // So we batch-overwrite them at once                       
            CopyMemory(mRaw, source->mRaw, GetBytesize());
         }
         else {
            // Both RHS and LHS are dense and non-POD                   
            // We invoke reflected assignments for each element         
            const auto stride = mType->mSize;
            auto lhs = mRaw;
            auto rhs = source->mRaw;
            const auto rhsEnd = rhs + count * stride;

            while (rhs != rhsEnd) {
               if constexpr (S::Move) {
                  if constexpr (S::Keep)
                     mType->mMoveAssigner(rhs, lhs);
                  else
                     mType->mAbandonAssigner(rhs, lhs);
               }
               else if constexpr (S::Shallow) {
                  if constexpr (S::Keep) {
                     if constexpr (CT::Referred<S>)
                        mType->mReferAssigner(rhs, lhs);
                     else
                        mType->mCopyAssigner(rhs, lhs);
                  }
                  else mType->mDisownAssigner(rhs, lhs);
               }
               else mType->mCloneAssigner(rhs, lhs);

               lhs += stride;
               rhs += stride;
            }
         }
      }
   }

   /// Never allocate new elements, instead assign all currently initialized  
   /// elements a single value                                                
   ///   @param what - the value to assign                                    
   ///   @attention be careful when filling using a move/abandon intent -     
   ///      'what' can be reset after the first assignment if not trivial     
   template<class TYPE> template<class A> LANGULUS(INLINED)
   void Block<TYPE>::Fill(A&& what)
   requires (TypeErased or CT::AssignableFrom<TYPE, A>) {
      if (IsEmpty())
         return;

      using S  = IntentOf<decltype(what)>;
      using ST = TypeOf<S>;

      if constexpr (not TypeErased) {
         // Assign by directly checking if argument satisfies an        
         // assignment signature, knowing what the contained type is    
         // at compile time                                             
         if constexpr (CT::AssignableFrom<TYPE, decltype(what)>) {
            auto lhs = GetRaw();
            const auto lhsEnd = lhs + mCount;
            while (lhs != lhsEnd)
               *(lhs++) = S::Nest(what);

            if constexpr (Sparse) {
               // We just copied a pointer multiple times, make sure    
               // we dereference the old entries, and reference the new 
               // memory multiple times, if we own it                   
               auto ent = GetEntries();
               const auto entEnd = ent + mCount;
               auto allocation = Allocator::Find(MetaDataOf<Deptr<TYPE>>(), *(--lhs));

               if (allocation) {
                  while (ent != entEnd) {
                     if (*ent)
                        const_cast<Allocation*>(*ent)->Free();
                     *(ent++) = allocation;
                  }
                  const_cast<Allocation*>(allocation)->Keep(mCount);
               }
               else {
                  // New pointer is out of jurisdiction, just reset     
                  // the current entries                                
                  while (ent != entEnd) {
                     if (*ent) {
                        const_cast<Allocation*>(*ent)->Free();
                        *ent = nullptr;
                     }
                  }
               }
            }
         }
         else static_assert(false, "Can't fill using that value "
            "- contained type is not assignable by it");
      }
      else {
         // Assigning type-erased items                                 
         // We expect, that type has been previously set                
         LANGULUS_ASSUME(DevAssumes, IsTyped(),
            "Block was expected to be typed");
         LANGULUS_ASSERT(IsSimilar<ST>(), Mutate,
            "Type mismatch");

         // Wrap argument into a block, and assign it to each element   
         auto rhs = MakeBlock(DeintCast(what));
         auto lhs = GetElement();
         const auto size = GetBytesize();
         const auto lhsEnd = mRaw + size;
         while (lhs.mRaw < lhsEnd) {
            lhs.AssignWithIntent(S::Nest(rhs));
            lhs.mRaw += size;
         }
      }
   }

   /// Overwrite a selected pointer                                           
   ///   @param newPointer - the new pointer to set                           
   ///   @return a reference to the selected pointer                          
   template<CT::Sparse T>
   auto LocalRef<T>::operator = (T newPointer) noexcept -> LocalRef& {
      // Leverage handle code to overwrite pointer and reference        
      Handle<T> interface {mPointer, mEntry};
      interface.AssignWithIntent(Refer(newPointer));
      return *this;
   }

   /// Construct a new item behind a selected pointer                         
   ///   @param arguments.. - constructor arguments                           
   ///   @return a reference to the selected pointer                          
   template<CT::Sparse T> template<class...A>
   auto LocalRef<T>::New(A&&...arguments) -> LocalRef& {
      static_assert(::std::constructible_from<Deptr<T>, A...>,
         "Type behind pointer isn't constructible with these arguments");
      LANGULUS_ASSERT(mEntry, Construct,
         "Can't create a new pointer inside a sparse block which is out of jurisdiction!");

      // Leverage handle code to free the element, if valid             
      Handle<T> interface {mPointer, mEntry};
      interface.template FreeInner<false, true>();

      // Analogous to what happens in Ref::New                          
      using DT = Decvq<Deptr<T>>;
      *mEntry = Allocator::Allocate(MetaDataOf<DT>(), sizeof(DT));
      LANGULUS_ASSERT(*mEntry, Allocate, "Out of memory");
      *mPointer = reinterpret_cast<DT*>((*mEntry)->GetBlockStart());
      new (const_cast<DT*>(*mPointer)) DecvqAll<DT> {
         Forward<A>(arguments)...
      };
      return *this;
   }

} // namespace Langulus::Annies