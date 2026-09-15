///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../BlockMap.hpp"
#include "../../blocks/Block/Block-Insert.inl"
#include "../../blocks/Block/Block-Construct.inl"


namespace Langulus::Annies
{

   /// Wrap the argument into a handle with key's type                        
   ///   @attention if key is a type-erased handle or void*, we assume that   
   ///      the pointer always points to a valid instance of the current      
   ///      key type                                                          
   ///   @attention make sure this isn't used like:                           
   ///      CreateKeyHandle(GetKeyHandle()) when map is type-erased           
   ///   @param key - the key to wrap, with or without intent                 
   ///   @return the handle object                                            
   template<CT::Map THIS>
   auto BlockMap::CreateKeyHandle(auto&& key) {
      using S = IntentOf<decltype(key)>;
      using T = TypeOf<S>;

      if constexpr (CT::Typed<THIS>) {
         using K = Conditional<CT::Typed<THIS>, typename THIS::Key, TypeOf<T>>;
         return HandleLocal<K> {S::Nest(key)};
      }
      else {
         // Make sure that value is always inserted, and never absorbed 
         auto result = Many::Wrap(S::Nest(key));
         // And make sure that type is set to the contained value type  
         result.mType = mKeys.mType;
         return result;
      }
   }

   /// Wrap the argument into a handle with value's type                      
   ///   @attention if value is a type-erased handle or void*, we assume that 
   ///      the pointer always points to a valid instance of the current      
   ///      value type                                                        
   ///   @attention make sure this isn't used like:                           
   ///      CreateValHandle(GetValHandle()) when map is type-erased           
   ///   @param val - the value to wrap, with or without intent               
   ///   @return the handle object                                            
   template<CT::Map THIS>
   auto BlockMap::CreateValHandle(auto&& val) {
      using S = IntentOf<decltype(val)>;
      using T = TypeOf<S>;

      if constexpr (CT::Typed<THIS>) {
         using V = Conditional<CT::Typed<THIS>, typename THIS::Value, TypeOf<T>>;
         return HandleLocal<V> {S::Nest(val)};
      }
      else {
         // Make sure that value is always inserted, and never absorbed 
         auto result = Many::Wrap(S::Nest(val));
         // And make sure that type is set to the contained value type  
         result.mType = mValues.mType;
         return result;
      }
   }

   /// Insert a pair, or an array of pairs                                    
   ///   @param item - the argument to unfold and insert, can have intent     
   ///   @return the number of inserted elements after unfolding              
   template<CT::Map THIS>
   Count BlockMap::UnfoldInsert(auto&& item) {
      using E = Conditional<CT::Typed<THIS>, typename THIS::Pair, Annies::Pair>;
      using S = IntentOf<decltype(item)>;
      using T = TypeOf<S>;

      if constexpr (CT::Typed<THIS>) {
         mKeys.mType = MetaDataOf<typename THIS::Key>();
         mValues.mType = MetaDataOf<typename THIS::Value>();
      }

      Count inserted = 0;
      if constexpr (CT::Array<T>) {
         if constexpr (CT::Typed<THIS>) {
            if constexpr (CT::MakableFrom<E, Deext<T>>) {
               // Construct from an array of elements, each of which    
               // can be used to initialize a pair, nesting any intent  
               // while at it                                           
               Reserve<THIS>(GetCount() + ExtentOf<T>);
               const auto mask = GetReserved() - 1;
               for (auto& pair : DeintCast(item))
                  inserted += InsertPairInner<THIS, true>(mask, S::Nest(pair));
            }
            else if constexpr (CT::MakableFrom<E, CT::Unfold<Deext<T>>>) {
               // Construct from an array of things, which can't be used
               // to directly construct elements, so nest this insert   
               for (auto& pair : DeintCast(item))
                  inserted += UnfoldInsert<THIS>(S::Nest(pair));
            }
            else static_assert(false, "Array elements aren't insertable as pairs");
         }
         else {
            // Insert the array                                         
            const auto& firstPair = DeintCast(item)[0];
            Mutate<THIS>(
               firstPair.GetKeyBlock().GetType(),
               firstPair.GetValueBlock().GetType()
            );
            Reserve<THIS>(GetCount() + ExtentOf<T>);
            const auto mask = GetReserved() - 1;
            for (auto& pair : DeintCast(item)) {
               Mutate<THIS>(
                  pair.GetKeyBlock().GetType(),
                  pair.GetValueBlock().GetType()
               );
               inserted += InsertPairInner<THIS, true>(mask, S::Nest(pair));
            }
         }
      }
      else if constexpr (CT::Typed<THIS>) {
         if constexpr (CT::MakableFrom<E, T>) {
            // Some of the arguments might still be used directly to    
            // make a pair, forward these to standard insertion here    
            Reserve<THIS>(GetCount() + 1);
            const auto mask = GetReserved() - 1;
            inserted += InsertPairInner<THIS, true>(mask, S::Nest(item));
         }
         else if constexpr (CT::Map<T>) {
            // Construct from any kind of map                           
            if constexpr (CT::Typed<T>) {
               // The contained type is known at compile-time           
               using T2 = TypeOf<T>;

               if constexpr (CT::MakableFrom<E, T2>) {
                  // Elements are mappable                              
                  Reserve<THIS>(GetCount() + DeintCast(item).GetCount());
                  const auto mask = GetReserved() - 1;
                  for (auto& pair : DeintCast(item))
                     inserted += InsertPairInner<THIS, true>(mask, S::Nest(pair));
               }
               else if constexpr (CT::MakableFrom<E, CT::Unfold<T2>>) {
                  // Map pairs need to be unfolded one by one           
                  for (auto& pair : DeintCast(item))
                     inserted += UnfoldInsert<THIS>(S::Nest(pair));
               }
               else static_assert(false, "Maps aren't mappable to each other");
            }
            else {
               // The rhs map is type-erased                            
               Mutate<THIS>(
                  DeintCast(item).GetKeyType(),
                  DeintCast(item).GetValueType()
               );
               Reserve<THIS>(GetCount() + DeintCast(item).GetCount());
               const auto mask = GetReserved() - 1;
               for (auto& pair : DeintCast(item))
                  inserted += InsertPairInner<THIS, true>(mask, S::Nest(pair));
            }
         }
         else static_assert(false, "Can't insert argument");
      }
      else if constexpr (CT::Pair<T>) {
         // This map is type-erased                                     
         // Some of the arguments might still be used directly to       
         // make pairs, forward these to standard insertion here        
         Mutate<THIS>(
            DeintCast(item).GetKeyBlock().GetType(),
            DeintCast(item).GetValueBlock().GetType()
         );
         Reserve<THIS>(GetCount() + 1);
         const auto mask = GetReserved() - 1;
         inserted += InsertPairInner<THIS, true>(mask, S::Nest(item));
      }
      else static_assert(false, "T isn't a pair/map, or an array of pairs/maps");

      return inserted;
   }

   /// Manually insert pair, with or without intent                           
   ///   @param key - the key to insert                                       
   ///   @param val - the value to insert                                     
   ///   @return 1 if pair was inserted, zero otherwise                       
   template<CT::Map THIS> LANGULUS(INLINED)
   Count BlockMap::Insert(auto&& key, auto&& val) {
      using SK = IntentOf<decltype(key)>;
      using SV = IntentOf<decltype(val)>;

      Mutate<THIS, TypeOf<SK>, TypeOf<SV>>();
      Reserve<THIS>(GetCount() + 1);
      InsertInner<THIS, true>(
         GetBucket<THIS>(GetReserved() - 1, DeintCast(key)), 
         SK::Nest(key), SV::Nest(val)
      );
      return 1;
   }
   
   /// Manually insert type-rased pair, with or without intent                
   ///   @param key - the key to insert                                       
   ///   @param val - the value to insert                                     
   ///   @return 1 if pair was inserted or value was overwritten              
   template<CT::Map THIS, class T1, class T2>
   requires CT::Block<Deint<T1>, Deint<T2>> LANGULUS(INLINED)
   Count BlockMap::InsertBlock(T1&& key, T2&& val) {
      using SK = IntentOf<decltype(key)>;
      using SV = IntentOf<decltype(val)>;
      using KB = TypeOf<SK>;
      using VB = TypeOf<SV>;

      // Type checks and mutations                                      
      if constexpr (CT::Typed<KB, VB>)
         Mutate<THIS, TypeOf<KB>, TypeOf<VB>>();
      else {
         Mutate<THIS>(
            DeintCast(key).GetType(),
            DeintCast(val).GetType()
         );
      }

      const auto count = ::std::min(
         DeintCast(key).GetCount(),
         DeintCast(val).GetCount()
      );

      Reserve<THIS>(GetCount() + count);

      for (Offset i = 0; i < count; ++i) {
         if constexpr (not CT::Typed<KB> or not CT::Typed<VB>) {
            // Type-erased insertion                                    
            auto keyBlock = DeintCast(key).GetElement(i);
            InsertBlockInner<THIS, true>(
               GetBucketUnknown(GetReserved() - 1, keyBlock),
               SK::Nest(keyBlock),
               SV::Nest(DeintCast(val).GetElement(i))
            );
         }
         else {
            // Static type insertion                                    
            auto& keyRef = DeintCast(key)[i];
            InsertInner<THIS, true>(
               GetBucket<THIS>(GetReserved() - 1, keyRef),
               SK::Nest(keyRef),
               SV::Nest(DeintCast(val)[i])
            );
         }
      }

      return count;
   }

   /// Unfold-insert pairs, with or without intent                            
   ///   @param t1 - the first pair to insert                                 
   ///   @param tn... - the rest of the pairs to insert (optional)            
   ///   @return the number of inserted pairs                                 
   template<CT::Map THIS, class T1, class...TN>
   Count BlockMap::InsertPair(T1&& t1, TN&&...tn) {
      Count inserted = 0;
        inserted += UnfoldInsert<THIS>(Forward<T1>(t1));
      ((inserted += UnfoldInsert<THIS>(Forward<TN>(tn))), ...);
      return inserted;
   }

   /// Request a new size of keys and info via the value container            
   /// The memory layout is:                                                  
   ///   [keys for each bucket, including entries, if sparse]                 
   ///         [padding for alignment]                                        
   ///               [info for each bucket]                                   
   ///                     [one sentinel byte for terminating loops]          
   ///   @attention assumes key type has been set                             
   ///   @param request - number of keys to allocate                          
   ///   @param infoStart - [out] the offset at which info bytes start        
   ///   @return the requested byte size                                      
   template<CT::Map THIS> LANGULUS(INLINED)
   Size BlockMap::RequestKeyAndInfoSize(
      const Count request, Offset& infoStart
   ) const IF_UNSAFE(noexcept) {
      Offset keymemory;
      if constexpr (CT::Typed<THIS>) {
         using K = typename THIS::Key;
         keymemory = request * sizeof(K);
         if constexpr (CT::Sparse<K>)
            keymemory *= 2;
      }
      else {
         LANGULUS_ASSUME(DevAssumes, mKeys.mType, "Key type was not set");
         keymemory = request * mKeys.mType->mSize;
         if (mKeys.mType->mIsSparse)
            keymemory *= 2;
      }

      infoStart = keymemory + Alignment - (keymemory % Alignment);
      return infoStart + request + 1;
   }

   /// Request a new size of value container                                  
   ///   @param count - number of values to allocate                          
   ///   @return the requested byte size                                      
   LANGULUS(INLINED)
   Size BlockMap::RequestValuesSize(const Count count) const IF_UNSAFE(noexcept) {
      LANGULUS_ASSUME(DevAssumes, mValues.mType, "Value type was not set");
      auto valueByteSize = count * mValues.mType->mSize;
      if (mValues.mType->mIsSparse)
         valueByteSize *= 2;
      return valueByteSize;
   }

   /// Rehashes the table, by optionally reusing parts of the map             
   ///   @attention assumes count and oldCount are power-of-two               
   ///   @attention assumes count > oldCount                                  
   ///   @attention will deallocate oldKeys and oldVals if provided           
   ///   @param oldInfo - pointer to old info (used only if reusing keys)     
   ///   @param oldCount - the old number of pairs                            
   ///   @param oldKeys - source of keys (use nullptr to reuse the current)   
   ///   @param oldVals - source of values (use nullptr to reuse the current) 
   ///   @return true if map requires another resize and rehash (very rare)   
   template<CT::Map THIS, class KEY_SOURCE, class VAL_SOURCE>
   bool BlockMap::Rehash(const InfoType* oldInfo, const Count oldCount, KEY_SOURCE& oldKeys, VAL_SOURCE& oldVals) {
      LANGULUS_ASSUME(DevAssumes, mKeys.mReserved > oldCount,
         "New count is not larger than oldCount");
      LANGULUS_ASSUME(DevAssumes, IsPowerOfTwo(mKeys.mReserved),
         "New count is not a power-of-two");
      LANGULUS_ASSUME(DevAssumes, IsPowerOfTwo(oldCount),
         "Old count is not a power-of-two");

      constexpr bool ReusingKeys = CT::Nullptr<KEY_SOURCE>;
      constexpr bool ReusingVals = CT::Nullptr<VAL_SOURCE>;
      static_assert(ReusingKeys or ReusingVals,
         "No need for a rehash call when nothing is reused "
         "- just reinsert instead"
      );

      if (IsEmpty()) {
         ZeroMemory(mInfo, GetReserved());

         // We should discard the old keys or values before returning   
         if constexpr (not ReusingKeys) {
            LANGULUS_ASSUME(DevAssumes, oldKeys.mKeys.mEntry->GetUses() == 1,
               "Deallocating old keys data that is still in use");
            Allocator::Deallocate(const_cast<Allocation*>(oldKeys.mKeys.mEntry));
         }

         if constexpr (not ReusingVals) {
            LANGULUS_ASSUME(DevAssumes, oldVals.mValues.mEntry->GetUses() == 1,
               "Deallocating old values data that is still in use");
            Allocator::Deallocate(const_cast<Allocation*>(oldVals.mValues.mEntry));
         }
         return false;
      }
      else {
         // Make sure new info data is zeroed                           
         // Moving memory to account for overlap                        
         // IT IS CRITICAL THAT THIS IS DONE BEFORE ENTRIES!            
         MoveMemory(mInfo, oldInfo, oldCount);
         ZeroMemory(mInfo + oldCount, GetReserved() - oldCount);
      }

      if constexpr (ReusingKeys) {
         // Keys were reused, but their entries shift forward           
         if (IsKeySparse<THIS>()) {
            MoveMemory(
               mKeys.mRawSparse + GetReserved(),
               mKeys.mRawSparse + oldCount,
               oldCount
            );
         };
      }

      if constexpr (ReusingVals) {
         // Vals were reused, but their entries shift forward           
         if (IsValueSparse<THIS>()) {
            MoveMemory(
               mValues.mRawSparse + GetReserved(),
               mValues.mRawSparse + oldCount,
               oldCount
            );
         };
      }
      
      (void) oldInfo;
      auto info = mInfo;
      const auto infoend = info + oldCount;
      const auto hashmask = GetReserved() - 1;
      constexpr auto MarkedForReinsertion = AllowedMisses;

      // In order to minimize swaps, we do the rehash in three passes   
      // First pass immediately moves any pair into an empty slot, if   
      // such exists after a rehash (rehashing onto itself also counts).
      // This provides the table with some breathing room. The rest of  
      // the pairs are marked for next pass by their offset being       
      // exactly AllowedMisses                                          
      while (info != infoend) {
         if (not *info) {
            ++info;
            continue;
         }

         // Where does the pair want to move after a rehash?            
         const Offset current = info - mInfo;
         auto key = [&] {
            if constexpr (ReusingKeys) return GetKeyHandle<THIS>(current);
            else return oldKeys.template GetKeyHandle<THIS>(current);
         }();
         auto val = [&] {
            if constexpr (ReusingVals) return GetValHandle<THIS>(current);
            else return oldVals.template GetValHandle<THIS>(current);
         }();

         Offset moveTo = 0;
         if constexpr (CT::TypedMap<THIS>)
            moveTo = GetBucket<THIS>(hashmask, key.Get());
         else
            moveTo = GetBucketUnknown(hashmask, key);

         if (moveTo == current) {
            // It rehashes onto itself                                  
            // Reset the offset and don't forget to copy from sources   
            if constexpr (not ReusingKeys) {
               GetKeyHandle<THIS>(current).CreateWithIntent(Abandon(key));
               key.FreeInner();
            }
            if constexpr (not ReusingVals) {
               GetValHandle<THIS>(current).CreateWithIntent(Abandon(val));
               val.FreeInner();
            }

            *info = 1;
         }
         else if (not mInfo[moveTo]) {
            // Immediately move it if destination is empty              
            mInfo[moveTo] = 1;
            GetKeyHandle<THIS>(moveTo).CreateWithIntent(Abandon(key));
            GetValHandle<THIS>(moveTo).CreateWithIntent(Abandon(val));
            key.FreeInner();
            val.FreeInner();

            *info = 0;
         }
         else {
            // Other mark for stage 2. Just make sure data that is from 
            // outside is moved in                                      
            if constexpr (not ReusingKeys) {
               GetKeyHandle<THIS>(current).CreateWithIntent(Abandon(key));
               key.FreeInner();
            }
            if constexpr (not ReusingVals) {
               GetValHandle<THIS>(current).CreateWithIntent(Abandon(val));
               val.FreeInner();
            }

            *info = MarkedForReinsertion;
         }

         ++info;
      }

      // We can discard the old keys or values at this point, because   
      // the second stage works only with local stuff                   
      if constexpr (not ReusingKeys) {
         LANGULUS_ASSUME(DevAssumes, oldKeys.mKeys.mEntry->GetUses() == 1,
            "Deallocating old keys data that is still in use");
         Allocator::Deallocate(const_cast<Allocation*>(oldKeys.mKeys.mEntry));
      }

      if constexpr (not ReusingVals) {
         LANGULUS_ASSUME(DevAssumes, oldVals.mValues.mEntry->GetUses() == 1,
            "Deallocating old values data that is still in use");
         Allocator::Deallocate(const_cast<Allocation*>(oldVals.mValues.mEntry));
      }

      // The second pass is interested only in pairs that have offset   
      // of exactly AllowedMisses. A duplicate is inserted by as many   
      // swaps as needed, after which the old slot is freed.            
      info = mInfo;
      while (info != infoend) {
         if (*info != MarkedForReinsertion) {
            ++info;
            continue;
         }

         // Where does the pair want to move after a rehash?            
         const Offset current = info - mInfo;
         auto key = GetKeyHandle<THIS>(current);
         auto val = GetValHandle<THIS>(current);
         Offset moveTo = 0;
         if constexpr (CT::TypedMap<THIS>)
            moveTo = GetBucket<THIS>(hashmask, key.Get());
         else
            moveTo = GetBucketUnknown(hashmask, key);

         // Insert, swap if we have to                                  
         {
            // This is like a simplified InsertInner, keep it in sync   
            // Get the starting index based on the key hash             
            auto psl = mInfo + moveTo;
            const auto pslEnd = GetInfoEnd();
            InfoType attempts = 1;
            while (*psl) {
               const auto index = psl - GetInfo();
               if (attempts > *psl) {
                  // Pair we're inserting is closer to bucket, so swap  
                  GetKeyHandle<THIS>(index).Swap(key);
                  GetValHandle<THIS>(index).Swap(val);
                  ::std::swap(attempts, *psl);
               }

               if (attempts >= AllowedMisses) {
                  // Oh boy, we need a resize while resizing!           
                  // Stop whatever we're doing, just dump the last thing
                  // anywhere that is empty. The offsets will be wrong  
                  // but it doesn't matter mid-rehashing. Just inform   
                  // we need another one, and it should tidy things up. 
                  // This can repeat indefinitely until RAM ends.       
                  //Logger::Special("Sequential resize triggered, ", GetCount(), "/", GetReserved(), " full");
                  Offset last = 0;
                  while (mInfo[last] and last < GetReserved())
                     ++last;

                  LANGULUS_ASSUME(DevAssumes, last < GetReserved(),
                     "Shouldn't ever happen, but better safe than sorry");

                  GetKeyHandle<THIS>(last).CreateWithIntent(Abandon(key));
                  GetValHandle<THIS>(last).CreateWithIntent(Abandon(val));
                  key.FreeInner();
                  val.FreeInner();
                  mInfo[last] = 1;
                  return true;
               }

               ++attempts;

               // Wrap around and start from the beginning if we have to
               if (psl < pslEnd - 1) ++psl;
               else psl = mInfo;
            }

            const Offset index = psl - GetInfo();
            LANGULUS_ASSUME(DevAssumes, current != index,
               "Shouldn't ever happen, but better safe than sorry");

            GetKeyHandle<THIS>(index).CreateWithIntent(Abandon(key));
            GetValHandle<THIS>(index).CreateWithIntent(Abandon(val));
            key.FreeInner();
            val.FreeInner();
            *psl = attempts;
         }

         *info = 0;
         ++info;
      }

      // Third pass shifts element left wherever possible to fill haps  
      ShiftPairs<THIS>();
      return false;
   }
   
   /// Shift elements left where possible                                     
   /// Repeat this couple of times until no more moves are possible           
   template<CT::Map THIS>
   void BlockMap::ShiftPairs() {
      int moves_performed;
      do {
         moves_performed = 0;
         auto oldInfo = mInfo;
         const auto newInfoEnd = GetInfoEnd();
         while (oldInfo != newInfoEnd) {
            if (*oldInfo > 1) {
               // Entry can be moved by *oldInfo - 1 cells to the left  
               const Offset oldIndex = oldInfo - GetInfo();

               // Will loop around if it goes beyond mKeys.mReserved    
               Offset to = mKeys.mReserved + oldIndex - *oldInfo + 1;
               if (to >= mKeys.mReserved)
                  to -= mKeys.mReserved;

               InfoType attempt = 1;
               while (mInfo[to] and attempt < *oldInfo) {
                  // Might loop around                                  
                  ++to;
                  if (to >= mKeys.mReserved)
                     to -= mKeys.mReserved;
                  ++attempt;
               }

               if (not mInfo[to] and attempt < *oldInfo) {
                  // Empty spot found, so move pair there               
                  auto key = GetKeyHandle<THIS>(oldIndex);
                  auto val = GetValHandle<THIS>(oldIndex);
                  GetKeyHandle<THIS>(to).CreateWithIntent(Abandon(key));
                  GetValHandle<THIS>(to).CreateWithIntent(Abandon(val));
                  key.FreeInner();
                  val.FreeInner();

                  mInfo[to] = attempt;
                  *oldInfo = 0;
                  ++moves_performed;
                  //Logger::Verbose(Logger::Red, oldIndex, " shifted to ", to);
               }
            }

            ++oldInfo;
         }

         //Logger::Verbose(moves_performed, " moves performed");

      } while (moves_performed);
   }
   
   /// Inner insertion function                                               
   ///   @attention assumes that keys and values are constructible with the   
   ///      provided arguments                                                
   ///   @tparam CHECK_FOR_MATCH - false if you guarantee key doesn't exist   
   ///   @param start - the starting index                                    
   ///   @param key - key to insert, with or without intent                   
   ///   @param val - value to insert, with or without intent                 
   ///   @return the offset at which pair was inserted                        
   template<CT::Map THIS, bool CHECK_FOR_MATCH>
   Offset BlockMap::InsertInner(Offset start, auto&& key, auto&& val) {
      BranchOut<THIS>();
      using SK = IntentOf<decltype(key)>;
      using SV = IntentOf<decltype(val)>;
      auto keyswapper = CreateKeyHandle<THIS>(SK::Nest(key));
      auto valswapper = CreateValHandle<THIS>(SV::Nest(val));

      // Get the starting index based on the key hash                   
      auto psl = GetInfo() + start;
      auto pslEnd = GetInfoEnd();
      InfoType attempts = 1;
      Offset insertedAt = mKeys.mReserved;
      while (*psl) {
         const auto index = psl - GetInfo();

         if constexpr (CHECK_FOR_MATCH) {
            if (keyswapper.Compare(GetKeyRef<THIS>(index))) {
               // Neat, the key already exists - just set value and go  
               GetValHandle<THIS>(index).AssignWithIntent(Abandon(valswapper));
               return index;
            }
         }

         if (attempts > *psl) {
            // The pair we're inserting is closer to bucket, so swap    
            GetKeyHandle<THIS>(index).Swap(keyswapper);
            GetValHandle<THIS>(index).Swap(valswapper);

            ::std::swap(attempts, *psl);
            if (insertedAt == mKeys.mReserved)
               insertedAt = index;
         }

         if (attempts >= AllowedMisses) {
            // Oops, this is bad - we've reached the limit of the       
            // robin-hood algorithm. The map is too saturated, and we   
            // need to widen its table. We should also do it while      
            // conscious of the loop we're in currently, so that we     
            // don't break anything.                                    
            //Logger::Special("Attempt will go out of bounds (", AllowedMisses,
            //   ") - map is ", GetCount(), "/", GetReserved(), " full");

            // Make map twice as big. This will invalidate any iterator 
            // Can repeat indefinitely                                  
            while (AllocateData<THIS, true>(GetReserved() * 2));

            // Rehash the key that currently resides in keyswapper      
            if constexpr (CT::TypedMap<THIS>)
               start = GetBucket<THIS>(GetReserved() - 1, keyswapper.Get());
            else
               start = GetBucketUnknown(GetReserved() - 1, keyswapper);

            // Refresh all local variables before continuing the loop   
            psl = GetInfo() + start;
            pslEnd = GetInfoEnd();
            attempts = 1;
            // Continue as if nothing had happened                      
            continue;
         }

         ++attempts;

         // Wrap around and start from the beginning if we have to      
         if (psl < pslEnd - 1)
            ++psl;
         else 
            psl = GetInfo();
      }

      // If reached, empty slot reached, so put the pair there          
      // Might not seem like it, but we gave a guarantee, that this is  
      // eventually reached, unless key exists and returns early, or    
      // attempts go beyond the info byte capacity                      
      const auto index = psl - GetInfo();
      GetKeyHandle<THIS>(index).CreateWithIntent(Abandon(keyswapper));
      GetValHandle<THIS>(index).CreateWithIntent(Abandon(valswapper));

      if (insertedAt == mKeys.mReserved)
         insertedAt = index;

      *psl = attempts;
      ++mKeys.mCount;
      return insertedAt; //TODO insertedAt will mostly likely be invalid if (attempts >= AllowedMisses) branch happened!!!!! can't figure out a way to compensate for that :(
   }
   
   /// Inner insertion function based on reflected move-assignment            
   ///   @tparam CHECK_FOR_MATCH - false if you guarantee key doesn't exist   
   ///   @param start - the starting index                                    
   ///   @param key - key to move in                                          
   ///   @param val - value to move in                                        
   ///   @return the offset at which pair was inserted                        
   template<CT::Map THIS, bool CHECK_FOR_MATCH, template<class> class S1, template<class> class S2, CT::Block T>
   requires CT::Intent<S1<T>, S2<T>>
   Offset BlockMap::InsertBlockInner(const Offset start, S1<T>&& key, S2<T>&& val) {
      BranchOut<THIS>();

      // Get the starting index based on the key hash                   
      auto psl = GetInfo() + start;
      const auto pslEnd = GetInfoEnd();
      InfoType attempts = 1;
      Offset insertedAt = mKeys.mReserved;
      while (*psl) {
         const auto index = psl - GetInfo();
         if constexpr (CHECK_FOR_MATCH) {
            const auto candidate = GetKeyHandle<THIS>(index);
            if (candidate == *key) {
               // Neat, the key already exists - just set value and go  
               GetValHandle<THIS>(index).AssignWithIntent(val.Forward());

               if constexpr (S2<T>::Move) {
                  val->FreeInner();
                  val->mCount = 0;
               }

               return index;
            }
         }

         if (attempts > *psl) {
            // The pair we're inserting is closer to bucket, so swap    
            GetKeyHandle<THIS>(index).Swap(key.Forward());
            GetValHandle<THIS>(index).Swap(val.Forward());

            ::std::swap(attempts, *psl);
            if (insertedAt == mKeys.mReserved)
               insertedAt = index;
         }

         LANGULUS_ASSUME(DevAssumes, attempts < AllowedMisses,
            "Attempt will go out of bounds");

         ++attempts;

         // Wrap around and start from the beginning if needed          
         if (psl < pslEnd - 1)
            ++psl;
         else
            psl = GetInfo();
      }

      // If reached, empty slot reached, so put the pair there	         
      // Might not seem like it, but we gave a guarantee, that this is  
      // eventually reached, unless key exists and returns early        
      // We're moving only a single element, so no chance of overlap    
      const auto index = psl - GetInfo();
      GetKeyHandle<THIS>(index).CreateWithIntent(key.Forward());
      GetValHandle<THIS>(index).CreateWithIntent(val.Forward());

      if (insertedAt == mKeys.mReserved)
         insertedAt = index;

      if constexpr (S1<T>::Move) {
         key->FreeInner();
         key->mCount = 0;
      }

      if constexpr (S2<T>::Move) {
         val->FreeInner();
         val->mCount = 0;
      }

      *psl = attempts;
      ++mKeys.mCount;
      return insertedAt;
   }
   
   /// Insert any pair into a preinitialized map                              
   ///   @tparam CHECK_FOR_MATCH - false if you guarantee key doesn't exist   
   ///   @param hashmask - precalculated hashmask                             
   ///   @param pair - the pair to insert, with or without intent             
   ///   @return the number of newly inserted pairs                           
   template<CT::Map THIS, bool CHECK_FOR_MATCH, template<class> class S, CT::Pair T>
   requires CT::Intent<S<T>>
   Count BlockMap::InsertPairInner(const Count hashmask, S<T>&& pair) {
      const auto initialCount = GetCount();
      if constexpr (CT::Typed<T>) {
         // Insert a statically typed pair                              
         InsertInner<THIS, CHECK_FOR_MATCH>(
            GetBucket<THIS>(hashmask, pair->GetKey()),
            pair.Nest(pair->GetKeyHandle()),
            pair.Nest(pair->GetValueHandle())
         );
      }
      else {
         // Insert a type-erased pair                                   
         InsertBlockInner<THIS, CHECK_FOR_MATCH>(
            GetBucketUnknown(hashmask, pair->GetKey()),
            pair.Nest(pair->GetKey()),
            pair.Nest(pair->GetValue())
         );
      }
      return GetCount() - initialCount;
   }

} // namespace Langulus::Annies
