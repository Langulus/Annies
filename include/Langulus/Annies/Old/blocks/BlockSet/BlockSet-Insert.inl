///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../BlockSet.hpp"
#include "../../text/Text.hpp"


namespace Langulus::Annies
{

   /// Wrap the argument into a handle with value's type                      
   ///   @attention if value is a type-erased handle or void*, we assume that 
   ///      the pointer always points to a valid instance of the current      
   ///      value type                                                        
   ///   @param val - the val to wrap, with or without intent                 
   ///   @return the handle object                                            
   template<CT::Set THIS>
   auto BlockSet::CreateValHandle(auto&& val) {
      using S = IntentOf<decltype(val)>;
      using T = TypeOf<S>;

      if constexpr (CT::Typed<THIS>) {
         using V = Conditional<CT::Typed<THIS>, TypeOf<THIS>, TypeOf<T>>;
         return HandleLocal<V> {S::Nest(val)};
      }
      else {
         // Make sure that value is always inserted, and never absorbed 
         auto result = Many::Wrap(S::Nest(val));
         // And make sure that type is set to the contained value type  
         result.mType = mKeys.mType;
         return result;
      }
   }

   /// Insert an element, or an array of elements, with or without intent     
   ///   @param item - the argument and intent to unfold and insert           
   ///   @return the number of inserted elements after unfolding              
   template<CT::Set THIS>
   size_t BlockSet::UnfoldInsert(auto&& item) {
      using E = Conditional<CT::Typed<THIS>, TypeOf<THIS>, void>;
      using S = IntentOf<decltype(item)>;
      using T = TypeOf<S>;

      if constexpr (not CT::TypeErased<E>)
         mKeys.mType = MetaDataOf<E>();

      if constexpr (CT::Array<T>) {
         if constexpr (not CT::TypeErased<E>) {
            if constexpr (CT::MakableFrom<E, Deext<T>>) {
               // Construct from an array of elements, each of which    
               // can be used to initialize an element, nesting any     
               // intents while at it                                   
               Reserve(GetCount() + ExtentOf<T>);
               for (auto& key : DeintCast(item)) {
                  InsertInner<THIS, true>(
                     GetBucket(GetReserved() - 1, DeintCast(key)),
                     S::Nest(key)
                  );
               }
               return ExtentOf<T>;
            }
            else if constexpr (CT::MakableFrom<E, CT::Unfold<Deext<T>>>) {
               // Construct from an array of things, which can't be used
               // to directly construct elements, so nest this insert   
               size_t inserted = 0;
               for (auto& key : DeintCast(item))
                  inserted += UnfoldInsert<THIS>(S::Nest(key));
               return inserted;
            }
            else static_assert(false, "Array elements aren't insertable");
         }
         else if constexpr (CT::StringLiteral<T>) {
            // Implicitly convert string literals to Text containers    
            Reserve(GetCount() + 1);
            Text text {S::Nest(item)};
            InsertInner<THIS, true>(
               GetBucket(GetReserved() - 1, text),
               Abandon(text)
            );
            return 1;
         }
         else {
            // Insert the array                                         
            Mutate<THIS, Decvq<Deext<T>>>();
            Reserve(GetCount() + ExtentOf<T>);
            size_t inserted = 0;
            for (auto& e : DeintCast(item)) {
               inserted += InsertInner<THIS, true>(
                  GetBucket(GetReserved() - 1, e),
                  S::Nest(e)
               );
            }
            return inserted;
         }
      }
      else if constexpr (not CT::TypeErased<E>) {
         if constexpr (CT::Handle<T> and CT::Similar<E, TypeOf<T>>) {
            // Insert a handle                                          
            Reserve(GetCount() + 1);
            InsertInner<THIS, true>(
               GetBucket(GetReserved() - 1, DeintCast(item).Get()),
               S::Nest(item)
            );
            return 1;
         }
         else if constexpr (CT::MakableFrom<E, T>) {
            // Some of the arguments might still be used directly to    
            // make an element, forward these to standard insertion here
            mKeys.mType = MetaDataOf<E>();
            Reserve(GetCount() + 1);
            InsertInner<THIS, true>(
               GetBucket(GetReserved() - 1, DeintCast(item)),
               S::Nest(item)
            );
            return 1;
         }
         else if constexpr (CT::Set<T>) {
            // Construct from any kind of set                           
            if constexpr (CT::Typed<T>) {
               // The contained type is known at compile-time           
               using T2 = TypeOf<T>;

               if constexpr (CT::MakableFrom<E, T2>) {
                  // Elements are mappable                              
                  Reserve(GetCount() + DeintCast(item).GetCount());
                  for (auto& key : DeintCast(item)) {
                     InsertInner<THIS, true>(
                        GetBucket(GetReserved() - 1, key),
                        S::Nest(key)
                     );
                  }
                  return DeintCast(item).GetCount();
               }
               else if constexpr (CT::MakableFrom<E, CT::Unfold<T2>>) {
                  // Set elements need to be unfolded one by one        
                  size_t inserted = 0;
                  for (auto& key : DeintCast(item))
                     inserted += UnfoldInsert<THIS>(S::Nest(key));
                  return inserted;
               }
               else static_assert(false, "Sets aren't mappable to each other");
            }
            else {
               // The rhs set is type-erased                            
               LANGULUS_ASSERT(DeintCast(item).template IsSimilar<E>(), Meta,
                  "Type mismatch");

               Reserve(GetCount() + DeintCast(item).GetCount());
               for (auto& key : DeintCast(item)) {
                  InsertBlockInner<THIS, true>(
                     GetBucketUnknown(GetReserved() - 1, key),
                     S::Nest(key)
                  );
               }
               return DeintCast(item).GetCount();
            }
         }
         else static_assert(false, "Can't insert argument");
      }
      else {
         // This set is type-erased                                     
         if constexpr (CT::Handle<T>) {
            // Insert a handle                                          
            Mutate<THIS, Decvq<TypeOf<T>>>();
            Reserve(GetCount() + 1);
            InsertInner<THIS, true>(
               GetBucket(GetReserved() - 1, DeintCast(item).Get()),
               S::Nest(item)
            );
         }
         else {
            // Some of the arguments might still be used directly to    
            // make an element, forward these to standard insertion here
            Mutate<THIS, Decvq<T>>();
            Reserve(GetCount() + 1);
            InsertInner<THIS, true>(
               GetBucket(GetReserved() - 1, DeintCast(item)),
               S::Nest(item)
            );
         }

         return 1;
      }
   }

   /// Insert elements, with or without intents                               
   ///   @param t1 - the first item to insert                                 
   ///   @param tn... - the rest of items to insert (optional)                
   ///   @return number of inserted elements                                  
   template<CT::Set THIS, class T1, class...TN> LANGULUS(INLINED)
   size_t BlockSet::Insert(T1&& t1, TN&&...tn) {
      size_t inserted = 0;
        inserted += UnfoldInsert<THIS>(Forward<T1>(t1));
      ((inserted += UnfoldInsert<THIS>(Forward<TN>(tn))), ...);
      return inserted;
   }
   
   /// Insert all elements of a set, with or without intents                  
   ///   @param item - the set to insert                                      
   ///   @return number of inserted elements                                  
   template<CT::Set THIS, class T> requires CT::Set<Deint<T>> LANGULUS(INLINED)
   size_t BlockSet::InsertBlock(T&& item) {
      using S  = IntentOf<decltype(item)>;
      using ST = TypeOf<S>;
      const auto count = DeintCast(item).GetCount();
      if (not count)
         return 0;

      Reserve(GetCount() + count);

      if (IsEmpty()) {
         // This set was empty, so no chance of collision, and since    
         // we're inserting a set, we can save a lot of CPU by not      
         // checking for matches                                        
         if constexpr (CT::Typed<ST> or CT::Typed<THIS>) {
            // Merging with a statically typed sets                     
            using B = Conditional<CT::Typed<ST>, ST, THIS>;
            for (auto& it : reinterpret_cast<const B&>(DeintCast(item))) {
               InsertInner<THIS, false>(
                  GetBucket(GetReserved() - 1, it),
                  S::Nest(it)
               );
            }
         }
         else {
            // Merging type-erased sets                                 
            for (auto& it : DeintCast(item)) {
               InsertBlockInner<THIS, false>(
                  GetBucketUnknown(GetReserved() - 1, it),
                  S::Nest(it)
               );
            }
         }
      }
      else {
         // This set already contains some stuff, so be careful when    
         // inserting elements from the other set - they might repeat   
         if constexpr (CT::Typed<ST> or CT::Typed<THIS>) {
            // Merging with a statically typed sets                     
            using B = Conditional<CT::Typed<ST>, ST, THIS>;
            for (auto& it : reinterpret_cast<const B&>(DeintCast(item))) {
               InsertInner<THIS, true>(
                  GetBucket(GetReserved() - 1, it),
                  S::Nest(it)
               );
            }
         }
         else {
            // Merging type-erased sets                                 
            for (auto& it : DeintCast(item)) {
               InsertBlockInner<THIS, true>(
                  GetBucketUnknown(GetReserved() - 1, it),
                  S::Nest(it)
               );
            }
         }
      }

      return count;
   }
   
   /// Insert all elements of a block, with or without intents                
   ///   @param item - the set to insert                                      
   ///   @return number of inserted elements                                  
   template<CT::Set THIS, class T> requires CT::Block<Deint<T>> LANGULUS(INLINED)
   size_t BlockSet::InsertBlock(T&& item) {
      using S  = IntentOf<decltype(item)>;
      using ST = TypeOf<S>;
      const auto count = DeintCast(item).GetCount();
      if (not count)
         return 0;

      Reserve(GetCount() + count);

      if constexpr (CT::Typed<ST> or CT::Typed<THIS>) {
         // Merging with a statically typed set and/or block            
         using B = Conditional<CT::Typed<ST>, ST, typename THIS::BlockType>;
         for (auto& it : reinterpret_cast<const B&>(DeintCast(item))) {
            InsertInner<THIS, true>(
               GetBucket(GetReserved() - 1, it),
               S::Nest(it)
            );
         }
      }
      else {
         // Merging type-erased block with a type-erased set            
         for (auto& it : DeintCast(item)) {
            InsertBlockInner<THIS, true>(
               GetBucketUnknown(GetReserved() - 1, it),
               S::Nest(it)
            );
         }
      }

      return count;
   }
   
   /// Request a new size of keys and info                                    
   /// The memory layout is:                                                  
   ///   [keys for each bucket, including entries, if sparse]                 
   ///         [padding for alignment]                                        
   ///               [info for each bucket]                                   
   ///                     [one sentinel byte for terminating loops]          
   ///   @attention assumes key type has been set                             
   ///   @param request - number of keys to allocate                          
   ///   @param infoStart - [out] the offset at which info bytes start        
   ///   @return the requested byte size                                      
   template<CT::Set THIS> LANGULUS(INLINED)
   size_t BlockSet::RequestKeyAndInfoSize(
      const size_t request, size_t& infoStart
   ) const IF_UNSAFE(noexcept) {
      size_t keymemory;
      if constexpr (CT::Typed<THIS>) {
         using T = TypeOf<THIS>;
         keymemory = request * sizeof(T);
         if constexpr (CT::Sparse<T>)
            keymemory *= 2;
      }
      else {
         LglsAssumeDev(mKeys.mType, "Key type was not set");
         keymemory = request * mKeys.mType->mSize;
         if (mKeys.mType->mIsSparse)
            keymemory *= 2;
      }

      infoStart = keymemory + Alignment - (keymemory % Alignment);
      return infoStart + request + 1;
   }

   /// Rehashes and reinserts each pair in the same block                     
   ///   @attention assumes count and oldCount are power-of-two               
   ///   @attention assumes count > oldCount                                  
   ///   @param oldCount - the old number of pairs                            
   template<CT::Set THIS>
   void BlockSet::Rehash(const size_t oldCount) {
      LglsAssumeDev(mKeys.mReserved > oldCount,
         "New count is not larger than oldCount");
      LglsAssumeDev(IsPowerOfTwo(mKeys.mReserved),
         "New count is not a power-of-two");
      LglsAssumeDev(IsPowerOfTwo(oldCount),
         "Old count is not a power-of-two");

      [[maybe_unused]] auto& me = reinterpret_cast<const THIS&>(*this);
      auto oldKey = GetHandle<THIS>(0);
      auto oldInfo = GetInfo();
      const auto oldInfoEnd = oldInfo + oldCount;
      const auto hashmask = mKeys.mReserved - 1;

      // First run: move elements closer to their new buckets           
      while (oldInfo != oldInfoEnd) {
         if (*oldInfo) {
            // Rehash and check if hashes match                         
            const size_t oldIndex = oldInfo - GetInfo();
            size_t oldBucket = (oldCount + oldIndex) - *oldInfo + 1;
            size_t newBucket = 0;
            if constexpr (CT::TypedSet<THIS>)
               newBucket += GetBucket(hashmask, oldKey.Get());
            else
               newBucket += GetBucketUnknown(hashmask, oldKey);

            if (oldBucket < oldCount or oldBucket - oldCount != newBucket) {
               // Move it only if it won't end up in same bucket        
               if constexpr (CT::Typed<THIS>) {
                  using K = TypeOf<THIS>;
                  HandleLocal<K> keyswap {Abandon(oldKey)};

                  // Destroy the key, info and value                    
                  oldKey.FreeInner();
                  *oldInfo = 0;
                  --mKeys.mCount;

                  // Reinsert at the new bucket                         
                  InsertInner<THIS, false>(newBucket, Abandon(keyswap));
               }
               else {
                  Block<> keyswap {DataState {}, GetType(), 1};
                  keyswap.AllocateFresh(keyswap.RequestSize(1));
                  keyswap.CreateWithIntent(Abandon(oldKey));

                  // Destroy the pair and info at old index             
                  oldKey.FreeInner();
                  *oldInfo = 0;
                  --mKeys.mCount;

                  InsertBlockInner<THIS, false>(newBucket, Abandon(keyswap));
                  keyswap.Free();
               }
            }
         }

         ++oldKey;
         ++oldInfo;
      }

      // First run might cause gaps                                     
      // Second run: shift elements left, where possible                
      ShiftPairs<THIS>();
   }
   
   /// Shift elements left, where possible                                    
   template<CT::Set THIS>
   void BlockSet::ShiftPairs() {
      int moves_performed;
      do {
         moves_performed = 0;
         auto oldInfo = mInfo;
         const auto newInfoEnd = GetInfoEnd();
         while (oldInfo != newInfoEnd) {
            if (*oldInfo > 1) {
               // Entry can be moved by *oldInfo - 1 cells to the left  
               const size_t oldIndex = oldInfo - GetInfo();

               // Might loop around                                     
               size_t to = mKeys.mReserved + oldIndex - *oldInfo + 1;
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
                  // Empty spot found, so move element there            
                  auto key = GetHandle<THIS>(oldIndex);
                  GetHandle<THIS>(to).CreateWithIntent(Abandon(key));
                  key.FreeInner();

                  mInfo[to] = attempt;
                  *oldInfo = 0;
                  ++moves_performed;
               }
            }

            ++oldInfo;
         }
      } while (moves_performed);
   }

   /// Inner insertion function                                               
   ///   @tparam CHECK_FOR_MATCH - false if you guarantee key doesn't exist   
   ///   @param start - the starting index                                    
   ///   @param key - key and intent to insert                                
   ///   @return the offset at which pair was inserted                        
   template<CT::Set THIS, bool CHECK_FOR_MATCH>
   size_t BlockSet::InsertInner(const size_t start, auto&& key) {
      BranchOut<THIS>();
      using S = IntentOf<decltype(key)>;
      auto keyswapper = CreateValHandle<THIS>(S::Nest(key));

      // Get the starting index based on the key hash                   
      auto psl = GetInfo() + start;
      const auto pslEnd = GetInfoEnd();
      InfoType attempts {1};
      size_t insertedAt = mKeys.mReserved;
      while (*psl) {
         const auto index = psl - GetInfo();

         if constexpr (CHECK_FOR_MATCH) {
            if (keyswapper == GetRef<THIS>(index)) {
               // Neat, the value already exists - just return          
               return index;
            }
         }

         if (attempts > *psl) {
            // The value we're inserting is closer to bucket, so swap   
            GetHandle<THIS>(index).Swap(keyswapper);
            ::std::swap(attempts, *psl);
            if (insertedAt == mKeys.mReserved)
               insertedAt = index;
         }

         ++attempts;

         // Wrap around and start from the beginning if we have to      
         if (psl < pslEnd - 1)
            ++psl;
         else 
            psl = GetInfo();
      }

      // If reached, empty slot reached, so put the value there         
      // Might not seem like it, but we gave a guarantee, that this is  
      // eventually reached, unless key exists and returns early        
      const auto index = psl - GetInfo();
      GetHandle<THIS>(index).CreateWithIntent(Abandon(keyswapper));

      if (insertedAt == mKeys.mReserved)
         insertedAt = index;

      *psl = attempts;
      ++mKeys.mCount;
      return insertedAt;
   }
   
   /// Inner insertion function from type-erased block                        
   ///   @tparam CHECK_FOR_MATCH - false if you guarantee key doesn't exist   
   ///   @param start - the starting index                                    
   ///   @param key - key and intent to insert                                
   ///   @return the offset at which pair was inserted                        
   template<CT::Set THIS, bool CHECK_FOR_MATCH, template<class> class S, CT::Block B>
   requires CT::Intent<S<B>>
   size_t BlockSet::InsertBlockInner(const size_t start, S<B>&& key) {
      BranchOut<THIS>();

      // Get the starting index based on the key hash                   
      auto psl = GetInfo() + start;
      const auto pslEnd = GetInfoEnd();
      InfoType attempts {1};
      size_t insertedAt = mKeys.mReserved;
      while (*psl) {
         const auto index = psl - GetInfo();
         if constexpr (CHECK_FOR_MATCH) {
            if (GetHandle<THIS>(index) == *key) {
               // Neat, the key already exists - just return            
               return index;
            }
         }

         if (attempts > *psl) {
            // The pair we're inserting is closer to bucket, so swap    
            GetHandle<THIS>(index).Swap(key.Forward());
            ::std::swap(attempts, *psl);
            if (insertedAt == mKeys.mReserved)
               insertedAt = index;
         }

         ++attempts;

         // Wrap around and start from the beginning if needed          
         if (psl < pslEnd - 1)
            ++psl;
         else
            psl = GetInfo();
      }

      // If reached, empty slot reached, so put the pair there	         
      // Might not seem like it, but we gave a guarantee, that this is  
      // eventually reached, unless element exists and returns early    
      // We're moving only a single element, so no chance of overlap    
      const auto index = psl - GetInfo();
      GetHandle<THIS>(index).CreateWithIntent(key.Forward());
      if (insertedAt == mKeys.mReserved)
         insertedAt = index;

      if constexpr (S<B>::Move) {
         key->FreeInner();
         key->mCount = 0;
      }

      *psl = attempts;
      ++mKeys.mCount;
      return insertedAt;
   }

} // namespace Langulus::Annies
