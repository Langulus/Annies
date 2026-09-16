///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../BlockSet.hpp"
#include <Langulus/Annies/Memory.hpp>


namespace Langulus::Annies
{
   
   /// Transfer the members of one set onto another                           
   ///   @tparam TO - the type of set we're transferring to                   
   ///   @param other - the set and intent to transfer from                   
   template<CT::Set TO, template<class> class S, CT::Set FROM>
   requires CT::Intent<S<FROM>> LANGULUS(INLINED)
   void BlockSet::BlockTransfer(S<FROM>&& other) {
      using SS = S<FROM>;

      if constexpr (not CT::TypedSet<TO>) {
         // TO is not statically typed, so we can safely                
         // overwrite type and state                                    
         mKeys.mType = other->GetType();
         mKeys.mState = other->mKeys.mState;
      }
      else {
         // TO is typed, so we never touch mType, and we make sure that 
         // we don't affect Typed state                                 
         mKeys.mType = MetaDataOf<TypeOf<TO>>();
         mKeys.mState = other->mKeys.mState + DataState::Typed;
      }

      if constexpr (SS::Shallow) {
         // We're transferring via a shallow intent                     
         if constexpr (SS::Keep) {
            // Move/Refer/Copy other                                    
            if constexpr (SS::Move) {
               // Move                                                  
               mKeys.mEntry = other->mKeys.mEntry;
               mKeys.mCount = other->mKeys.mCount;
               mKeys.mRaw = other->mKeys.mRaw;
               mKeys.mReserved = other->mKeys.mReserved;
               mInfo = other->mInfo;

               if constexpr (not FROM::Ownership) {
                  // Since we are not aware if that block is referenced 
                  // or not we reference it just in case, and we also   
                  // do not reset 'other' to avoid leaks. When using    
                  // raw Blocks, it's your responsibility to take care  
                  // of ownership.                                      
                  Keep<TO, true>();
               }
               else {
                  other->mKeys.ResetMemory();
                  other->mKeys.ResetState();
               }
            }
            else if constexpr (CT::Referred<SS>) {
               // Refer                                                 
               mKeys.mEntry = other->mKeys.mEntry;
               mKeys.mCount = other->mKeys.mCount;
               mKeys.mRaw = other->mKeys.mRaw;
               mKeys.mReserved = other->mKeys.mReserved;
               mInfo = other->mInfo;

               Keep<TO, true>();
            }
            else {
               // Copy                                                  
               // We're shallow-copying, so we're 100% sure, that       
               // each element will end up in the same place            
               mKeys.mState -= DataState::Constant;
               if (other->IsEmpty())
                  return;

               // Always prefer statically typed set interface (if any) 
               using B = Conditional<CT::Typed<FROM>, FROM, TO>;
               using K = Conditional<CT::Typed<B>, TypeOf<B>, void>;
               auto asFrom = const_cast<B*>(reinterpret_cast<const B*>(&*other));

               if constexpr (CT::Untyped<B>) {
                  // Runtime checks are required before allocating      
                  LANGULUS_ASSERT(asFrom->mKeys.mType->mReferConstructor, Construct,
                     "Can't refer-construct keys"
                     " - no refer-constructor was reflected for type ", asFrom->mKeys.mType);
               }
               else {
                  static_assert(CT::ReferMakable<K>,
                     "Type is not refer-constructible");
               }

               AllocateFresh<B>(other->GetReserved());
               CopyMemory(mInfo, other->mInfo, GetReserved() + 1);

               if constexpr (CT::Typed<B>) {
                  // At least one of the sets is typed                  
                  if constexpr (CT::POD<K>) {
                     // Data is POD, we can directly copy all keys      
                     CopyMemory(
                        mKeys.mRaw, asFrom->mKeys.mRaw,
                        GetReserved() * sizeof(K)
                     );
                  }
                  else {
                     // Data isn't pod, refer valid keys one by one     
                     auto info = GetInfo();
                     const auto infoEnd = GetInfoEnd();
                     auto dstKey = GetHandle<B>(0);
                     auto srcKey = asFrom->template GetHandle<B>(0);
                     while (info != infoEnd) {
                        if (*info)
                           dstKey.CreateWithIntent(Refer(srcKey));

                        ++info;
                        ++dstKey;
                        ++srcKey;
                     }
                  }
               }
               else {
                  // Both sets are type-erased                          
                  if (asFrom->mKeys.mType->mIsPOD) {
                     // Keys are POD, we can directly copy them all     
                     CopyMemory(
                        mKeys.mRaw, asFrom->mKeys.mRaw,
                        GetReserved() * asFrom->mKeys.mType->mSize
                     );
                  }
                  else {
                     // Keys aren't POD, clone valid keys one by one    
                     auto info = GetInfo();
                     const auto infoEnd = GetInfoEnd();
                     auto dstKey = GetHandle<B>(0);
                     auto srcKey = asFrom->template GetHandle<B>(0);
                     while (info != infoEnd) {
                        if (*info)
                           dstKey.CreateWithIntent(Refer(srcKey));

                        ++info;
                        ++dstKey;
                        ++srcKey;
                     }
                  }
               }

               // This validates elements, do it last in case           
               // something throws along the way                        
               mKeys.mCount = other->GetCount();
            }
         }
         else if constexpr (SS::Move) {
            // Abandon                                                  
            mKeys.mEntry = other->mKeys.mEntry;
            mKeys.mCount = other->mKeys.mCount;
            mKeys.mRaw = other->mKeys.mRaw;
            mKeys.mReserved = other->mKeys.mReserved;
            mInfo = other->mInfo;

            other->mKeys.mEntry = nullptr;
         }
         else {
            // Disown                                                   
            mKeys.mCount = other->mKeys.mCount;
            mKeys.mRaw = other->mKeys.mRaw;
            mKeys.mReserved = other->mKeys.mReserved;
            mInfo = other->mInfo;
         }
      }
      else {
         // We're cloning, so we guarantee, that data is no longer      
         // static and constant via state                               
         mKeys.mState -= DataState::Constant;
         if (other->IsEmpty())
            return;

         // Always prefer statically typed set interface (if any)       
         using B = Conditional<CT::Typed<FROM>, FROM, TO>;
         using K = Conditional<CT::Typed<B>, TypeOf<B>, void>;
         auto asFrom = const_cast<B*>(reinterpret_cast<const B*>(&*other));

         if constexpr (CT::Untyped<B>) {
            // Runtime checks are required before allocating            
            LANGULUS_ASSERT(asFrom->mKeys.mType->mCloneConstructor, Construct,
               "Can't clone-construct keys"
               " - no clone-constructor was reflected for type ", asFrom->mKeys.mType);
         }
         else {
            static_assert(CT::CloneMakable<K>,
               "Key type is not clone-constructible");
         }

         AllocateFresh<B>(other->GetReserved());

         if constexpr (CT::Typed<B>) {
            if constexpr (CT::Dense<TypeOf<B>>) {
               // We're cloning dense elements, so we're 100% sure, that
               // each element will end up in the same place            
               CopyMemory(mInfo, other->mInfo, GetReserved() + 1);

               if constexpr (CT::POD<TypeOf<B>>) {
                  // Data is POD, we can directly copy the entire table 
                  CopyMemory(
                     mKeys.mRaw, asFrom->mKeys.mRaw,
                     GetReserved() * sizeof(TypeOf<B>)
                  );
               }
               else {
                  // Data isn't pod, clone valid elements one by one    
                  auto info = GetInfo();
                  const auto infoEnd = GetInfoEnd();
                  auto dstKey = GetHandle<B>(0);
                  auto srcKey = asFrom->template GetHandle<B>(0);
                  while (info != infoEnd) {
                     if (*info)
                        dstKey.CreateWithIntent(SS::Nest(srcKey));

                     ++info;
                     ++dstKey;
                     ++srcKey;
                  }
               }

               // This validates elements, do it last in case           
               // something throws along the way                        
               mKeys.mCount = other->GetCount();
            }
            else {
               // We're cloning pointers, which will inevitably end up  
               // pointing elsewhere, which means that all elements must
               // be rehashed and reinserted                            
               TMany<Deptr<TypeOf<B>>> coalesced;
               coalesced.Reserve(asFrom->GetCount());

               // Coalesce all densified elements, to avoid multiple    
               // allocations                                           
               for (auto& item : *asFrom) {
                  coalesced.template InsertInner<void, false>(
                     IndexBack, SS::Nest(*item));
               }

               const_cast<Allocation*>(coalesced.mEntry)
                  ->Keep(asFrom->GetCount());

               // Zero info bytes and insert pointers                   
               ZeroMemory(mInfo, mKeys.mReserved);
               mInfo[mKeys.mReserved] = 1;

               auto ptr = coalesced.GetRaw();
               const auto ptrEnd = coalesced.GetRawEnd();
               while (ptr != ptrEnd) {
                  InsertInner<B, false>(
                     GetBucket(GetReserved() - 1, ptr),
                     Abandon(HandleLocal<TypeOf<B>> {ptr, coalesced.mEntry})
                  );

                  if constexpr (CT::Referencable<Deptr<K>>)
                     ptr->Reference(1);

                  ++ptr;
               }
            }
         }
         else {
            if (not asFrom->mKeys.mType->mIsSparse) {
               // We're cloning dense elements, so we're 100% sure, that
               // each element will end up in the same place            
               CopyMemory(mInfo, other->mInfo, GetReserved() + 1);

               if (asFrom->mKeys.mType->mIsPOD) {
                  // Data is POD, we can directly copy the entire table 
                  CopyMemory(
                     mKeys.mRaw, asFrom->mKeys.mRaw,
                     GetReserved() * asFrom->mKeys.mType->mSize
                  );
               }
               else {
                  // Data isn't pod, clone valid elements one by one    
                  auto info = GetInfo();
                  const auto infoEnd = GetInfoEnd();
                  auto dstKey = GetHandle<B>(0);
                  auto srcKey = asFrom->template GetHandle<B>(0);
                  while (info != infoEnd) {
                     if (*info)
                        dstKey.CreateWithIntent(SS::Nest(srcKey));

                     ++info;
                     ++dstKey;
                     ++srcKey;
                  }
               }

               // This validates elements, do it last in case           
               // something throws along the way                        
               mKeys.mCount = other->GetCount();
            }
            else {
               // We're cloning pointers, which will inevitably end up  
               // pointing elsewhere, which means that all elements must
               // be rehashed and reinserted                            
               auto coalesced = Many::FromMeta(asFrom->mKeys.mType->mDeptr);
               coalesced.Reserve(asFrom->GetCount());

               // Coalesce all densified elements, to avoid multiple    
               // allocations                                           
               for (auto& item : *asFrom) {
                  coalesced.template InsertBlock<void, false>(
                     IndexBack, SS::Nest(*item));
               }

               const_cast<Allocation*>(coalesced.mEntry)
                  ->Keep(asFrom->GetCount());

               // Zero info bytes and insert pointers                   
               ZeroMemory(mInfo, mKeys.mReserved);
               mInfo[mKeys.mReserved] = 1;

               auto ptr = coalesced.mRaw;
               const auto ptrEnd = coalesced.mRaw + coalesced.GetBytesize();
               const Size stride = coalesced.GetStride();
               while (ptr != ptrEnd) {
                  InsertInner<B, false>(
                     GetBucket(GetReserved() - 1, ptr),
                     Abandon(HandleLocal<void*> {ptr, coalesced.mEntry})
                  );

                  if (coalesced.GetType()->mReference)
                     coalesced.GetType()->mReference(ptr, 1);

                  ptr += stride.mSize;
               }
            }
         }
      }
   }
   
   /// Branch the set, by doing a shallow copy                                
   template<CT::Set THIS>
   void BlockSet::BranchOut() {
      if (GetUses() > 1) {
         // Set is used from multiple locations, and we must branch out 
         // before changing it - only this copy will be affected        
         if constexpr (CT::Typed<THIS> and CT::ReferMakable<TypeOf<THIS>>) {
            const BlockSet backup = *this;
            const_cast<Allocation*>(mKeys.mEntry)->Free();
            new (this) THIS {Copy(reinterpret_cast<const THIS&>(backup))};
         }
         else LANGULUS_THROW(Construct,
            "Set needs to branch out, but type doesn't support Intent::Copy");
      }
   }

} // namespace Langulus::Annies
