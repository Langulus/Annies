///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Block.hpp"


namespace Langulus::A
{

   /// Manual construction via type                                           
   ///   @param meta - the type of the memory block                           
   LANGULUS(INLINED)
   constexpr Block::Block(DMeta meta) noexcept
      : mType {meta} { }

   /// Manual construction via state and type                                 
   ///   @param state - the initial state of the container                    
   ///   @param meta - the type of the memory block                           
   LANGULUS(INLINED)
   constexpr Block::Block(const DataState& state, DMeta meta) noexcept
      : mType {meta}
      , mState {state} { }
   
   /// Manual construction via state and a reflected constant                 
   ///   @param state - the initial state of the container                    
   ///   @param meta - the constant definition                                
   LANGULUS(INLINED)
   Block::Block(const DataState& state, CMeta meta) IF_UNSAFE(noexcept)
      : Block {
         state + DataState::Constrained,
         meta->mValueType, 1, meta->mPtrToValue, nullptr
      } {}
   
   /// Manual construction from mutable data                                  
   /// This constructor has runtime overhead if managed memory is enabled     
   ///   @attention assumes data is not sparse                                
   ///   @param state - the initial state of the container                    
   ///   @param meta - the type of the memory block                           
   ///   @param count - initial element count and reserve                     
   ///   @param raw - pointer to the mutable memory                           
   LANGULUS(INLINED)
   Block::Block(const DataState& state, DMeta meta, Count count, void* raw) IF_UNSAFE(noexcept)
      : Block {state, meta, count, raw, Allocator::Find(meta, raw)} {}

   /// Manual construction from mutable data                                  
   ///   @attention assumes data is not sparse                                
   ///   @param state - the initial state of the container                    
   ///   @param meta - the type of the memory block                           
   ///   @param count - initial element count and reserve                     
   LANGULUS(INLINED)
   Block::Block(const DataState& state, DMeta meta, Count count) IF_UNSAFE(noexcept)
      : Block {state, meta, count, (void*) nullptr, nullptr} {}
   
   /// Manual construction from constant data                                 
   /// This constructor has runtime overhead if managed memory is enabled     
   ///   @attention assumes data is not sparse                                
   ///   @param state - the initial state of the container                    
   ///   @param meta - the type of the memory block                           
   ///   @param count - initial element count and reserve                     
   ///   @param raw - pointer to the constant memory                          
   LANGULUS(INLINED)
   Block::Block(const DataState& state, DMeta meta, Count count, const void* raw)
      IF_UNSAFE(noexcept)
      : Block {state + DataState::Constant, meta, count, const_cast<void*>(raw)}
   {}

   /// Manual construction from mutable data and known entry                  
   ///   @attention assumes 'meta' is not sparse, if 'raw' is provided        
   ///   @attention assumes a valid 'raw' pointer provided, if 'entry' is     
   ///   @attention assumes 'meta' is always valid                            
   ///   @param state - the initial state of the container                    
   ///   @param meta - the type of the memory block                           
   ///   @param count - initial element count and reserve                     
   ///   @param raw - pointer to the mutable memory                           
   ///   @param entry - the memory entry                                      
   LANGULUS(INLINED)
   Block::Block(const DataState& state, DMeta meta, Count count, void* raw, const Allocation* entry)
   IF_UNSAFE(noexcept)
      : mRaw {static_cast<Byte*>(raw)}
      , mCount {count}
      , mReserved {count}
      , mType {meta}
      , mEntry {entry}
      , mState {state} {
      LANGULUS_ASSUME(DevAssumes, not entry or raw,
         "Invalid data pointer");
      LANGULUS_ASSUME(DevAssumes, meta,
         "Invalid data type");
      LANGULUS_ASSUME(DevAssumes, not raw or not meta->mIsSparse or not mEntry,
         "Sparse raw data initialization is not allowed, "
         "unless mEntry is zero");

   }
   
   /// Manual construction from constant data and known entry                 
   ///   @attention assumes data is not sparse                                
   ///   @param state - the initial state of the container                    
   ///   @param meta - the type of the memory block                           
   ///   @param count - initial element count and reserve                     
   ///   @param raw - pointer to the constant memory                          
   ///   @param entry - the memory entry                                      
   LANGULUS(INLINED)
   Block::Block(const DataState& state, DMeta meta, Count count, const void* raw, const Allocation* entry)
      IF_UNSAFE(noexcept)
      : Block {state + DataState::Constant, meta, count, const_cast<void*>(raw), entry}
   {}

} // namespace Langulus::A


namespace Langulus::Annies
{

   /// Blocks are always constructible from other blocks                      
   /// This is only a binary-compatible intermediate container without        
   /// ownership - all it does is copy block properties.                      
   ///   @param other - block to copy                                         
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   constexpr Block<TYPE>::Block(const A::Block& other) noexcept
      : A::Block {other} {}

   /// Construct from a list of elements with or without intents, an array,   
   /// as well as any other kinds of blocks                                   
   ///   @param t1 - first element                                            
   ///   @param tn - tail of elements (optional)                              
   template<class TYPE> template<class T1, class...TN>
   requires CT::DeepMakable<TYPE, T1, TN...>
   void Block<TYPE>::BlockCreate(T1&& t1, TN&&...tn) {
      using S = IntentOf<decltype(t1)>;
      using T = TypeOf<S>;

      if constexpr (TypeErased) {
         // Construct a type-erased block                               
         if constexpr (sizeof...(TN) == 0) {
            if constexpr (CT::Deep<T>)
               BlockTransfer(Forward<T1>(t1));
            else
               Insert<Many, true>(IndexBack, Forward<T1>(t1));
         }
         else Insert<Many, true>(IndexBack, Forward<T1>(t1), Forward<TN>(tn)...);
      }
      else {
         // Construct a typed block                                     
         mType = MetaDataOf<TYPE>();

         if constexpr (sizeof...(TN) == 0) {
            if constexpr (CT::Block<T>) {
               if constexpr (CT::Typed<T>) {
                  // Not type-erased block, do compile-time type checks 
                  using STT = TypeOf<T>;

                  if constexpr (CT::Similar<TYPE, STT>) {
                     // Type is binary compatible, just transfer block  
                     BlockTransfer(Forward<T1>(t1));
                  }
                  else if constexpr (CT::Sparse<TYPE, STT>) {
                     if constexpr (CT::DerivedFrom<TYPE, STT>) {
                        // The statically typed block contains items    
                        // that are base of this container's type. Each 
                        // element should be dynamically cast to this   
                        // type                                         
                        for (auto pointer : DeintCast(t1)) {
                           auto dcast = dynamic_cast<TYPE>(&(*pointer));
                           if (dcast)
                              Insert<void, true>(IndexBack, dcast);
                        }
                     }
                     else if constexpr (CT::DerivedFrom<STT, TYPE>) {
                        // The statically typed block contains items    
                        // that are derived from this container's type. 
                        // Each element should be statically sliced to  
                        // this type                                    
                        for (auto pointer : DeintCast(t1))
                           Insert<void, true>(IndexBack, static_cast<TYPE>(&(*pointer)));
                     }
                     else Insert(IndexBack, Forward<T1>(t1));
                  }
                  else Insert(IndexBack, Forward<T1>(t1));
               }
               else if constexpr (CT::Deep<T>) {
                  // Type-erased block, do run-time type checks         
                  if (IsSimilar(DeintCast(t1).GetType())) {
                     // If types are similar, it is safe to absorb the  
                     // block, essentially converting a type- erased    
                     // Many back to its TMany equivalent               
                     BlockTransfer(Forward<T1>(t1));
                  }
                  else if constexpr (CT::Deep<TYPE>) {
                     // This block accepts any kind of deep element     
                     Insert(IndexBack, Forward<T1>(t1));
                  }
                  else LANGULUS_OOPS(Meta, "Unable to absorb block");
               }
               else static_assert(false, "Can't construct this TMany from this kind of Block");
            }
            else Insert(IndexBack, Forward<T1>(t1));
         }
         else Insert(IndexBack, Forward<T1>(t1), Forward<TN>(tn)...);
      }
   }

   /// Blocks are always assignable from other blocks                         
   /// This is only a binary-compatible intermediate container without        
   /// ownership - all it does is copy block properties.                      
   ///   @param other - block to copy                                         
   ///   @return a reference to this block                                    
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   constexpr Block<TYPE>& Block<TYPE>::operator = (const A::Block& rhs) noexcept {
      A::Block::operator = (rhs);
      return *this;
   }

   /// Transfer the members of one block onto another with the smallest       
   /// number of instructions possible, with or without intents.              
   ///   @attention will not set mType if TO is type-constrained              
   ///   @attention will not set mRaw, mReserved, mEntry, if 'from' is empty  
   ///   @param from - the block and intent to transfer from                  
   template<class TYPE> template<class FROM> requires CT::Block<Deint<FROM>>
   LANGULUS(INLINED) void Block<TYPE>::BlockTransfer(FROM&& block) {
      using S = IntentOf<decltype(block)>;
      using T = TypeOf<S>;
      using B = Conditional<T::TypeErased, Block, T>;
      auto& from = DeintCast(block);

      if constexpr (TypeErased) {
         // We can safely overwrite type and state                      
         mType  = from.GetType();
         mState = from.mState;
      }
      else {
         // Block is typed so we never touch mType, and we make sure    
         // that we don't affect Typed state                            
         mState = from.mState + DataState::Typed;
      }

      if constexpr (S::Shallow) {
         // Move/Copy/Refer/Abandon/Disown other                        
         if constexpr (S::Keep) {
            // Move/Copy/Refer other                                    
            if constexpr (S::Move) {
               // Move                                                  
               mEntry    = from.mEntry;
               mRaw      = from.mRaw;
               mReserved = from.mReserved;
               mCount    = from.mCount;

               if constexpr (not T::Ownership) {
                  // Since we are not aware if that block is referenced 
                  // or not we reference it just in case, and we also   
                  // do not reset 'other' to avoid leaks. When using    
                  // raw Blocks, it's your responsibility to take care  
                  // of ownership.                                      
                  Keep<true>();
               }
               else {
                  from.ResetMemory();
                  from.ResetState();
               }
            }
            else {
               // Copy/Refer other                                      
               if constexpr (CT::Referred<S>) {
                  // Refer                                              
                  mRaw      = from.mRaw;
                  mReserved = from.mReserved;
                  mEntry    = from.mEntry;
                  mCount    = from.mCount;
                  Keep<true>();
               }
               else {
                  // Do a shallow copy                                  
                  // We're cloning first layer, so we guarantee, that   
                  // data is no longer static and constant (unless      
                  // mType is constant)                                 
                  mState -= DataState::Constant;
                  if (0 == from.mCount)
                     return;

                  // Pick a preferably typed block to optimize          
                  if constexpr (B::TypeErased) {
                     // A runtime check is required before allocating   
                     LANGULUS_ASSERT(mType->mReferConstructor, Construct,
                        "Can't refer-construct elements"
                        " - no refer-constructor was reflected for type ", mType);
                  }
                  else {
                     static_assert(CT::ReferMakable<TypeOf<B>>,
                        "Contained type is not refer-constructible");
                  }

                  auto& thisb = reinterpret_cast<B&>(*this);
                  thisb.AllocateFresh(thisb.RequestSize(from.mCount));
                  thisb.CreateWithIntent(Refer(block));
                  // This validates elements, do it last in case        
                  // something throws along the way                     
                  mCount = from.mCount;
               }
            }
         }
         else if constexpr (S::Move) {
            // Abandon                                                  
            mRaw      = from.mRaw;
            mReserved = from.mReserved;
            mEntry    = from.mEntry;
            mCount    = from.mCount;
            from.mEntry = nullptr;
         }
         else {
            // Disown                                                   
            mRaw      = from.mRaw;
            mReserved = from.mReserved;
            mCount    = from.mCount;
         }
      }
      else {
         // We're cloning, so we guarantee, that data is no longer      
         // static and constant                                         
         mState -= DataState::Constant;
         if (0 == from.mCount)
            return;

         // Pick a preferably typed block to optimize the construction  
         mType = mType->mDecvq;
         if constexpr (B::TypeErased) {
            // A runtime check is required before allocating            
            LANGULUS_ASSERT(mType->mCloneConstructor, Construct,
               "Can't clone-construct elements"
               " - no clone-constructor was reflected for type ", mType);
         }
         else {
            static_assert(CT::CloneMakable<TypeOf<B>>,
               "Contained type is not clone-constructible");
         }

         auto& thisb = reinterpret_cast<B&>(*this);
         thisb.AllocateFresh(thisb.RequestSize(from.mCount));
         thisb.CreateWithIntent(Forward<FROM>(block));

         // This validates elements, do it last in case something throw 
         mCount = from.mCount;
      }
   }

   /// Assign one block onto another with the smallest number of instructions 
   /// possible, using an intent or not.                                      
   ///   @param rhs - the block/value and semantic to assign                  
   ///   @return a reference to this block                                    
   template<class TYPE> template<CT::Block THIS, class T1>
   THIS& Block<TYPE>::BlockAssign(T1&& rhs) requires CT::DeepAssignable<TYPE, T1> {
      using S = IntentOf<decltype(rhs)>;
      using T = TypeOf<S>;

      if constexpr (CT::Block<T>) {
         if (static_cast<const A::Block*>(this)
          == static_cast<const A::Block*>(&DeintCast(rhs)))
            return reinterpret_cast<THIS&>(*this);
      }

      if constexpr (TypeErased) {
         if constexpr (CT::Deep<T>) {
            // Potentially absorb a container                           
            Free();
            new (this) THIS {S::Nest(rhs)};
         }
         else if (IsSimilar<CT::Unfold<T>>()) {
            // Unfold-insert by reusing memory                          
            Clear();
            UnfoldInsert<void, true>(IndexBack, S::Nest(rhs));
         }
         else {
            // Allocate anew and unfold-insert                          
            Free();
            new (this) THIS {S::Nest(rhs)};
         }
      }
      else {
         if constexpr (CT::Block<T>) {
            // Potentially absorb a container                           
            Free();
            new (this) THIS {S::Nest(rhs)};
         }
         else {
            // Unfold-insert                                            
            Clear();
            UnfoldInsert<void, true>(IndexBack, S::Nest(rhs));
         }
      }

      return reinterpret_cast<THIS&>(*this);
   }

   /// Branch the block, by doing a shallow copy                              
   template<class TYPE>
   void Block<TYPE>::BranchOut() {
      if (GetUses() <= 1)
         return;
      
      // Block is used from multiple locations, and we must branch out  
      // before changing it - only this copy will be affected           
      if constexpr (not TypeErased) {
         if constexpr (CT::ReferMakable<TYPE>) {
            const auto backup = *this;
            const_cast<Allocation*>(mEntry)->Free();
            new (this) TMany<TYPE> {Copy(reinterpret_cast<const TMany<TYPE>&>(backup))};
         }
         else LANGULUS_THROW(Construct,
            "Block needs to branch out, but type doesn't support Intent::Copy"
         );
      }
      else {
         if (mType->mCopyConstructor) {
            const auto backup = *this;
            const_cast<Allocation*>(mEntry)->Free();
            new (this) Many {Copy(reinterpret_cast<const Many&>(backup))};
         }
         else LANGULUS_THROW(Construct,
            "Block needs to branch out, but type doesn't support Intent::Copy"
         );
      }
   }

   
   /// Construct a block, that best represents a contiguous piece of memory   
   /// If BLOCK is a container with ownership, then data will be copied if    
   /// not in jurisdiction, which involves a slow authority check. If you     
   /// want to avoid checking and copying at all - use the Disown intent.     
   /// By default, data will be wrapped in an automatically detected Block    
   /// without ownership.                                                     
   ///   @attention this function doesn't call any BLOCK constructor! For     
   ///      some containers, like Text for example, it will not execute       
   ///      null-terminator detection! It is designed as a low-level tool for 
   ///      interfacing memory directly, so use it only if you know what      
   ///      you're doing!!                                                    
   ///   @tparam BLOCK - the type of block to use, use void to auto-detect    
   ///      When auto-detected, the block will never own the data, unless     
   ///      an intent that isn't Disown is given.                             
   ///   @param what - start of data to interface, with or without intent     
   ///   @param count - number of items, in case 'what' is sparse             
   ///   @attention if 'what' is a bounded array, then count is multiplied by 
   ///      the array's extent, for the final number of elements              
   ///   @return the memory wrapped inside a block                            
   template<class BLOCK>
   auto MakeBlock(auto&& what, Count count) {
      static_assert(CT::Void<BLOCK> or CT::Block<BLOCK>,
         "BLOCK can be either void, or a Block type");

      using A  = Deref<decltype(what)>;
      using S  = IntentOf<A>;
      using ST = TypeOf<S>;

      // Auto-detect block type                                         
      using B = Conditional<CT::Void<BLOCK>
         , Conditional<CT::NoIntent<A> or CT::Disowned<A>
            , Conditional<CT::Array<Deint<A>>
               , Block<Decvq<CT::Unfold<A>>>
               , Block<Decvq<Deptr<CT::Unfold<A>>>>
            >
            , Conditional<CT::Array<Deint<A>>
               , TMany<Decvq<CT::Unfold<A>>>
               , TMany<Decvq<Deptr<CT::Unfold<A>>>>
            >
         >
         , BLOCK
      >;

      B result;
      if constexpr (not B::TypeErased) {
         using T = TypeOf<B>;

         // We're creating a dense block...                             
         if constexpr (CT::Array<ST>) {
            // ... from a bounded array                                 
            using DST = Deext<ST>;
            const auto count2 = count * ExtentOf<ST> * sizeof(DST);
            LANGULUS_ASSERT(0 == (count2 % sizeof(T)),
               Meta, "Provided array type is not a multiple of sizeof(T)");
            count = count2 / sizeof(T);

            if constexpr (CT::Similar<T, DST> or CT::POD<T, DST>) {
               new (&result) Block<T> {
                  DataState::Constrained,
                  result.GetType(), count,
                  DeintCast(what), nullptr
               };
            }
            else {
               static_assert(false,
                  "Can't wrap a bounded array inside incompatible block:"
                  " types are not binary compatible");
            }
         }
         else if constexpr (CT::Sparse<ST>) {
            // ... from a pointer                                       
            using DST = Deptr<ST>;
            const auto count2 = count * sizeof(DST);
            LANGULUS_ASSERT(0 == (count2 % sizeof(T)),
               Meta, "Provided pointer type is not a multiple of sizeof(T)");
            count = count2 / sizeof(T);

            if constexpr (CT::Similar<T, DST> or CT::POD<T, DST>) {
               new (&result) Block<T> {
                  DataState::Constrained,
                  result.GetType(), count,
                  DeintCast(what), nullptr
               };
            }
            else {
               static_assert(false,
                  "Can't wrap a unbounded array inside incompatible block:"
                  " types are not binary compatible");
            }
         }
         else {
            // ... from a value                                         
            static_assert(0 == (sizeof(ST) % sizeof(T)),
               "Provided type is not a multiple of sizeof(T)");
            count = sizeof(ST) / sizeof(T);

            if constexpr (CT::Similar<T, ST> or CT::POD<T, ST>) {
               new (&result) Block<T> {
                  DataState::Constrained,
                  result.GetType(), count,
                  &DeintCast(what), nullptr
               };
            }
            else {
               static_assert(false,
                  "Can't wrap a dense element inside incompatible block:"
                  " types are not binary compatible");
            }
         }
      }
      else {
         if constexpr (CT::Array<ST>) {
            // ... from a bounded array                                 
            count *= ExtentOf<ST>;
            new (&result) Block<Deext<ST>> {
               DataState::Constrained,
               MetaDataOf<Deext<ST>>(), count,
               DeintCast(what), nullptr
            };
         }
         else if constexpr (CT::Sparse<ST>) {
            // ... from an unbounded array/pointer                      
            new (&result) Block<Deptr<ST>> {
               DataState::Constrained,
               MetaDataOf<Deptr<ST>>(), count,
               DeintCast(what), nullptr
            };
         }
         else {
            // ... from a value                                         
            if constexpr (CT::Resolvable<ST>) {
               // Resolve a runtime-resolvable value                    
               new (&result) Block<> {DeintCast(what).GetBlock()};
            }
            else if constexpr (CT::Deep<ST>) {
               // Static cast to Block if CT::Deep                      
               new (&result) Block<> {DeintCast(what)};
            }
            else {
               // Any other value gets wrapped inside a temporary Block 
               new (&result) Block<ST> {
                  DataState::Constrained,
                  MetaDataOf<ST>(), 1,
                  &DeintCast(what), nullptr
               };
            }
         }
      }

      if constexpr (not S::Move and S::Keep and B::Ownership)
         result.TakeAuthority();
      return result;
   }
   
   /// Insert the provided elements, making sure to insert and never absorb   
   ///   @tparam BLOCK - the block type to wrap elements in, use void to      
   ///      auto-deduce                                                       
   ///   @param items... - items to insert                                    
   ///   @returns the new container containing the data                       
   template<class BLOCK, CT::Data...TN>
   auto WrapBlock(TN&&...items) {
      if constexpr (sizeof...(TN) == 0) {
         if constexpr (CT::TypeErased<BLOCK>)
            return Block<> {};
         else
            return BLOCK {};
      }
      else if constexpr (CT::TypeErased<BLOCK>) {
         // Auto-detect type, statically optimize as much as possible   
         using First = FirstOf<Decvq<CT::Unfold<TN>>...>;
         if constexpr (CT::Similar<First, CT::Unfold<TN>...>) {
            // All provided types are the same                          
            TMany<First> result;
            (result.template Insert<void>(IndexBack, Forward<TN>(items)), ...);
            return result;
         }
         else {
            // Different kinds of data, wrap them in Manies             
            TMany<Many> result;
            (result.template Insert<void>(IndexBack, Forward<TN>(items)), ...);
            return result;
         }
      }
      else {
         static_assert(CT::Block<BLOCK>, "BLOCK must be a Block type");
         BLOCK result;
         (result.template Insert<void>(IndexBack, Forward<TN>(items)), ...);
         return result;
      }
   }

} // namespace Langulus::Annies