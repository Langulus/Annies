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


namespace Langulus::Annies
{
   
   /// Compare to any other kind of deep container                            
   ///   @param rhs - element to compare against                              
   ///   @return true if containers match                                     
   template<class TYPE>
   bool Block<TYPE>::operator == (const CT::Block auto& rhs) const {
      return Compare<true>(rhs) or CompareSingleValue(rhs);
   }
   
   /// Compare to any other kind of deep container, or single custom element  
   ///   @param rhs - element to compare against                              
   ///   @return true if containers match                                     
   template<class TYPE> template<CT::NotBlock T1>
   bool Block<TYPE>::operator == (const T1& rhs) const
   requires (TypeErased or CT::Comparable<TYPE, T1>) {
      return CompareSingleValue(rhs);
   }
   
   /// Compare two block's contents for equality                              
   ///   @tparam RESOLVE - true to resolve each element before comparing      
   ///   @param right - the memory block to compare against                   
   ///   @return true if the two memory blocks are identical                  
   template<class TYPE> template<bool RESOLVE>
   bool Block<TYPE>::Compare(const CT::Block auto& right) const {
      using RHS = Deref<decltype(right)>;
      VERBOSE_COMPARE_TAB("Comparing ",
         Logger::PushWhite, mCount, " elements of ", GetToken(),
         Logger::Pop, " with ",
         Logger::PushWhite, right.mCount, " elements of ", right.GetToken()
      );

      if constexpr (not TypeErased and not RHS::TypeErased) {
         // Both blocks are statically typed - leverage it              
         if constexpr (not CT::Similar<TYPE, TypeOf<RHS>>) { //TODO but what if differently typed pointers to the same virtual objects?
            // Types are different                                      
            VERBOSE_COMPARE(Logger::Red, "Types differ (compile-time): ",
               NameOf<TYPE>(), " != ", NameOf<TypeOf<RHS>>());
            return false;
         }
         else {
            // Types are similar                                        
            if (mRaw == right.mRaw)
               return mCount == right.mCount;
            else if (mCount != right.mCount) {
               VERBOSE_COMPARE(Logger::Red, "Different count (compile-time): ", mCount, " != ", right.mCount);
               return false;
            }

            if constexpr (CT::POD<TYPE>) {
               // Batch compare pods or pointers                        
               const bool same = 0 == ::std::memcmp(mRaw, right.mRaw, GetBytesize());
               if (not same)
                  VERBOSE_COMPARE(Logger::Red, "Different POD memory after memcmp (compile-time)");
               return same;
            }
            else if constexpr (CT::Comparable<TYPE, TYPE>) {
               // Use comparison operator between all elements          
               auto t1 = GetRaw();
               auto t2 = right.GetRaw();
               const auto t1end = t1 + mCount;
               while (t1 < t1end and *t1 == *t2) {
                  ++t1;
                  ++t2;
               }

               if (t1 != t1end)
                  VERBOSE_COMPARE(Logger::Red, "Element #", t1 - GetRaw(), " differs (compile-time)");
               return t1 == t1end;
            }
            else {
               VERBOSE_COMPARE(Logger::Red, "Type not comparable (compile-time): ", NameOf<TYPE>());
               return false;
            }
         }
      }
      else if constexpr (not TypeErased or not RHS::TypeErased) {
         // One of the blocks is statically typed - a runtime type      
         // check is required                                           
         if ((mCount or right.mCount) and not IsSimilar(right)) { //TODO but what if differently typed pointers to the same virtual objects?
            VERBOSE_COMPARE(Logger::Red, "Types differ (runtime): ",
               GetType(), " != ", right.GetType());
            return false;
         }

         if constexpr (not TypeErased)
            return Compare<RESOLVE>(reinterpret_cast<const Block<TYPE>&>(right));
         else
            return right.template Compare<RESOLVE>(reinterpret_cast<const RHS&>(*this));
      }
      else {
         if (not IsSimilar(right)) { //TODO but what if differently typed pointers to the same virtual objects?
            VERBOSE_COMPARE(Logger::Red, "Types differ (runtime): ",
               mType, " != ", right.mType);
            return false;
         }

         // Types are similar                                           
         if (mRaw == right.mRaw)
            return mCount == right.mCount;
         else if (mCount != right.mCount) {
            VERBOSE_COMPARE(Logger::Red, "Different count (runtime): ", mCount, " != ", right.mCount);
            return false;
         }

         if (mType->mIsPOD or mType->mIsSparse) {
            // Batch-compare memory if POD or sparse                    
            const bool same = 0 == ::std::memcmp(mRaw, right.mRaw, GetBytesize());
            if (not same)
               VERBOSE_COMPARE(Logger::Red, "Different POD memory after memcmp (runtime)");
            return same;
         }
         else if (mType->mComparer) {
            // Call compare operator for each element pair              
            auto lhs = mRaw;
            auto rhs = right.mRaw;
            const auto lhsEnd = GetRawEnd<Byte>();
            while (lhs != lhsEnd) {
               if (not mType->mComparer(lhs, rhs)) {
                  VERBOSE_COMPARE(Logger::Red, "Element #", (lhs - mRaw) / mType->mSize, " differs (runtime)");
                  return false;
               }

               lhs += mType->mSize;
               rhs += mType->mSize;
            }

            return true;
         }
         else {
            VERBOSE_COMPARE(Logger::Red, "Type not comparable (runtime): ", mType);
            LANGULUS_OOPS(Compare, "No == operator reflected for type ", mType);
         }

         return true;
      }
   }
   
   /// Compare with one single value, if exactly one element is contained     
   ///   @param rhs - the value to compare against                            
   ///   @return true if elements are the same                                
   template<class TYPE> LANGULUS(INLINED)
   bool Block<TYPE>::CompareSingleValue(const CT::NoIntent auto& rhs) const {
      using T = Deref<decltype(rhs)>;
      if (mCount != 1)
         return false;

      if constexpr (not TypeErased) {
         // Both sides are statically typed                             
         if constexpr (CT::Similar<TYPE, T> and CT::Comparable<TYPE, T>)
            return *GetRaw() == rhs;
         else if constexpr (CT::Owned<T> and CT::Similar<TYPE, TypeOf<T>>
         and CT::Comparable<TYPE, TypeOf<T>>)
            return *GetRaw() == rhs.Get();
         else
            return false;
      }
      else {
         // THIS is type-erased, do runtime type checks                 
         if (IsUntyped())
            return false;

         if constexpr (CT::Deep<T>) {
            // Deep types can be more loosely compared                  
            if (mType->mIsSparse or not mType->mIsDeep)
               return false;
            return GetDeep() == rhs;
         }
         else if constexpr (CT::StringLiteral<T>) {
            if (mType->template IsSimilar<Text>()) {
               // Implicitly make a text container on string literal    
               return *GetRaw<Text>() == Text {Disown(rhs)};
            }
            else if (mType->template IsSimilar<char*, wchar_t*>()) {
               // Cast away the extent, compare against pointer         
               return GetRaw<void*>() == static_cast<const void*>(rhs);
            }
            else return false;
         }
         else if constexpr (CT::Comparable<T, T>) {
            // Non-deep element compare                                 
            if (mType->template IsSimilar<T>())
               return *GetRaw<T>() == rhs;
            else if constexpr (CT::Owned<T>) {
               if (mType->template IsSimilar<TypeOf<T>>())
                  return *GetRaw<T>() == rhs.Get();
               else
                  return false;
            }
            else return false;
         }
         else return false;
      }
   }
   
   /// Hash data inside memory block                                          
   ///   @attention order matters, so you might want to Neat data first       
   ///   @return the hash                                                     
   template<class TYPE>
   Hash Block<TYPE>::GetHash() const requires (TypeErased or CT::Hashable<TYPE>) {
      if constexpr (not TypeErased) {
         if (not mCount)
            return {};

         if (mCount == 1) {
            // Exactly one element means exactly one hash               
            return HashOf(*GetRaw());
         }

         // Hashing multiple elements                                   
         if constexpr (Sparse) {
            return HashBytes<DefaultHashSeed, false>(
               mRaw, static_cast<int>(GetBytesize()));
         }
         else if constexpr (CT::POD<TYPE> and not CT::Inner::HasGetHashMethod<TYPE>) {
            // Hash all PODs at once                                    
            return HashBytes<DefaultHashSeed, alignof(TYPE) < Bitness / 8> (
               mRaw, static_cast<int>(GetBytesize()));
         }
         else {
            // Hash each element, and then combine hashes in a final one
            TMany<Hash> h;
            h.Reserve(mCount);
            for (auto& element : *this)
               h << HashOf(element);
            return h.GetHash();
         }
      }
      else {
         if (not mType or not mCount)
            return {};

         if (mCount == 1) {
            // Exactly one element means exactly one hash               
            if (mType->mIsSparse)
               return HashOf(*mRawSparse);
            else if (mType->Is<Hash>())
               return Get<Hash>();
            else if (mType->mHasher)
               return mType->mHasher(mRaw);
            else if (mType->mIsPOD) {
               if (mType->mAlignment < Bitness / 8) {
                  return HashBytes<DefaultHashSeed, true>(
                     mRaw, static_cast<int>(mType->mSize));
               }
               else {
                  return HashBytes<DefaultHashSeed, false>(
                     mRaw, static_cast<int>(mType->mSize));
               }
            }
            else LANGULUS_OOPS(Access, "Unhashable type", ": ", GetToken());
         }

         // Hashing multiple elements                                   
         if (mType->mIsSparse) {
            return HashBytes<DefaultHashSeed, false>(
               mRaw, static_cast<int>(GetBytesize()));
         }
         else if (mType->mHasher) {
            // Use the reflected hasher for each element, and then      
            // combine hashes for a final one                           
            TMany<Hash> h;
            h.Reserve(mCount);
            ForEachElement<false>([&](const Block<>& element) {
               h << mType->mHasher(element.mRaw);
            });
            return h.GetHash();
         }
         else if (mType->mIsPOD) {
            if (mType->mAlignment < Bitness / 8) {
               return HashBytes<DefaultHashSeed, true>(
                  mRaw, static_cast<int>(GetBytesize()));
            }
            else {
               return HashBytes<DefaultHashSeed, false>(
                  mRaw, static_cast<int>(GetBytesize()));
            }
         }
         else LANGULUS_OOPS(Access, "Unhashable type", ": ", GetToken());
         return {};
      }
   }

   /// Find a single element's index inside container                         
   ///   @tparam REVERSE - true to perform search in reverse                  
   ///   @param item - the item to search for                                 
   ///   @param cookie - resume search from a given index                     
   ///   @return the index of the found item, or IndexNone if none found      
   template<class TYPE> template<bool REVERSE, CT::NoIntent T1>
   Index Block<TYPE>::Find(const T1& item, size_t cookie) const noexcept
   requires (TypeErased or CT::Comparable<TYPE, T1>) {
      if constexpr (not TypeErased) {
         auto start = REVERSE
            ? GetRawEnd() - 1 - cookie
            : GetRaw() + cookie;
         const auto end = REVERSE
            ? start - mCount
            : start + mCount;

         while (start != end) {
            if (*start == item)
               // Match found                                           
               return start - GetRaw();

            if constexpr (REVERSE)
               --start;
            else
               ++start;
         }
      }
      else {
         size_t i = REVERSE ? mCount - 1 - cookie : cookie;
         while (i < mCount) {
            if (GetElementInner(i) == item) {
               // Match found                                           
               return i;
            }

            if constexpr (REVERSE)
               --i;
            else
               ++i;
         }
      }

      // If this is reached, then no match was found                    
      return IndexNone;
   }
   
   /// Find a matching sequence of one or more matching elements              
   ///   @tparam REVERSE - true to perform search in reverse                  
   ///   @param item - a block containing a sequence of items to search for   
   ///   @param index - continue search from a given offset                   
   ///   @return the index of the found item, or IndexNone if not found       
   template<class TYPE> template<bool REVERSE>
   Index Block<TYPE>::FindBlock(const CT::Block auto& item, CT::Index auto index) const noexcept {
      auto cookie = SimplifyIndex(index);
      if (cookie >= mCount or item.mCount > mCount - cookie)
         return IndexNone;

      using RHS = Deref<decltype(item)>;
      using LB = Block<Conditional<     TypeErased, TypeOf<RHS>, TYPE>>;
      using RB = Block<Conditional<RHS::TypeErased, TYPE, TypeOf<RHS>>>;

      auto& lb = reinterpret_cast<const LB&>(*this);
      auto& rb = reinterpret_cast<const RB&>( item);

      if constexpr (not TypeErased or not RHS::TypeErased) {
         // One of the participating blocks is statically typed         
         // Let's check type compatibility first                        
         if constexpr (not TypeErased and not RHS::TypeErased) {
            // Leverage the fact, that both participants are typed      
            if constexpr (not CT::Comparable<TypeOf<LB>, TypeOf<RB>>)
               return IndexNone;
         }
         else {
            // One or none of the participants is typed                 
            if (not IsSimilar(item))
               return IndexNone;
         }

         // If this is reached reached, then types are comparable       
         auto rhs = rb.GetRaw();
         auto lhs = REVERSE
            ? lb.GetRawEnd() - cookie - item.GetCount()
            : lb.GetRaw() + cookie;

         const auto rhsEnd = rb.GetRawEnd();
         const auto lhsEnd = REVERSE
            ? lb.GetRaw() - 1
            : lb.GetRawEnd() - item.GetCount() + 1;

         // This byte size is used ONLY IF both types are binary        
         // compatible. It is simply precomputed here, so that it isn't 
         // recomputed in the loop.                                     
         [[maybe_unused]] const auto bytesize = lb.GetBytesize();

         while (lhs != lhsEnd) {
            if (*lhs == *rhs) {
               cookie = REVERSE
                  ? lb.GetRawEnd() - lhs - 1
                  : lhs - lb.GetRaw();

               ++lhs;
               ++rhs;

               if constexpr (CT::BinaryCompatible<TypeOf<LB>, TypeOf<RB>>
               and CT::POD<TypeOf<LB>, TypeOf<RB>>) {
                  // We can use batch-compare                           
                  if (0 == memcmp(rhs, lhs, bytesize))
                     // Match found                                     
                     return cookie;
               }
               else {
                  // Types are not batch-comparable, so compare them    
                  // one by one                                         
                  while (rhs != rhsEnd and *lhs == *rhs) {
                     ++lhs;
                     ++rhs;
                  }

                  if (rhs == rhsEnd)
                     // Match found                                     
                     return cookie;
               }

               lhs = REVERSE
                  ? lb.GetRawEnd() - cookie - 1
                  : lb.GetRaw() + cookie;
               rhs = rb.GetRaw();
            }

            if constexpr (REVERSE) --lhs;
            else                   ++lhs;
         }

         return IndexNone;
      }
      else {
         size_t i = REVERSE ? mCount - 1 - cookie : cookie;
         const auto iend = REVERSE
            ? static_cast<size_t>(-1)
            : mCount - item.GetCount() + 1;

         while (i != iend) {
            if (CropInner(i, item.GetCount()) == item)
               // Match found                                           
               return i;

            if constexpr (REVERSE)
               --i;
            else
               ++i;
         }

         // If this is reached, then no match was found                 
         return IndexNone;
      }
   }

   /// Compare the relevant states of two blocks                              
   ///   @param rhs - the memory block to compare against                     
   ///   @return true if the two memory blocks' revelant states are identical 
   template<class TYPE> LANGULUS(INLINED)
   bool Block<TYPE>::CompareStates(const Block& rhs) const noexcept {
      return GetUnconstrainedState() == rhs.GetUnconstrainedState();
   }

   /// Compare types of two blocks, and a produce a common type whose         
   /// comparison function to use                                             
   ///   @attention assumes that both blocks are typed                        
   ///   @param right - the type to the right                                 
   ///   @param common - [out] the common base                                
   ///   @return true if a common base has been found                         
   template<class TYPE>
   bool Block<TYPE>::CompareTypes(const CT::Block auto& right, RTTI::Base& common) const {
      LglsAssumeDev(IsTyped(),
         "LHS block is not typed", " comparing with RHS: ", right.GetType());
      LglsAssumeDev(right.IsTyped(),
         "RHS block is not typed", " comparing with LHS: ", GetType());

      if (not mType->Is(right.mType)) {
         // Types differ, dig deeper to find out why                    
         if (not mType->GetBase(right.mType, 0, common)) {
            // Other type is not base for this one, can't compare them  
            // Let's check in reverse                                   
            if (not right.mType->GetBase(mType, 0, common)) {
               // Other type is not derived from this one, can't        
               // compare them                                          
               return false;
            }

            // Other is derived from this, but it has to be binary      
            // compatible to be able to compare them                    
            if (not common.mBinaryCompatible) {
               VERBOSE_COMPARE(Logger::Red,
                  "Data types are related, but not binary compatible: ",
                  GetToken(), " != ", right.GetToken());
               return false;
            }

            return true;
         }
         else {
            // This is derived from other, but it has to be binary      
            // compatible to be able to compare them                    
            if (not common.mBinaryCompatible) {
               VERBOSE_COMPARE(Logger::Red,
                  "Data types are related, but not binary compatible: ",
                  GetToken(), " != ", right.GetToken());
               return false;
            }

            return true;
         }
      }
      else {
         // Types match exactly, or their origins match exactly         
         if (mType->mOrigin) {
            common.mType = mType->mOrigin;
            common.mBinaryCompatible = true;
            return true;
         }
         else {
            // Types match exactly, but we're interested only in an     
            // origin type base. Unfortunately, the type is incomplete, 
            // so comparison is not possible                            
            return false;
         }
      }
   }

   /// Invoke a comparator in base, comparing this block against right one    
   ///   @param right - the right block                                       
   ///   @param base - the base to use                                        
   ///   @return true if comparison returns true                              
   template<class TYPE> LANGULUS(INLINED)
   bool Block<TYPE>::CallComparer(const Block& right, const RTTI::Base& base) const {
      return mRaw == right.mRaw or (mRaw and right.mRaw
         and base.mType->mComparer(mRaw, right.mRaw));
   }

   /// Gather items from input container, and fill output                     
   /// Output type acts as a filter to what gets gathered                     
   ///   @tparam REVERSE - are we gathering in reverse?                       
   ///   @param input - source container                                      
   ///   @param output - [in/out] container that collects results             
   ///   @return the number of gathered elements                              
   template<class TYPE> template<bool REVERSE>
   size_t Block<TYPE>::GatherInner(CT::Block auto& output) const {
      using OUT = Deref<decltype(output)>;

      if constexpr (not TypeErased and not OUT::TypeErased) {
         // Both containers are statically typed - leverage it          
         TODO();
      }
      else {
         if (IsDeep() and not output.IsDeep()) {
            size_t count = 0;
            ForEach<REVERSE>([&](const Block<>& i) {
               count += i.template GatherInner<REVERSE>(output);
            });
            return count;
         }

         if constexpr (not OUT::TypeErased) {
            // Output container is strictly typed, we can't make loose  
            // matches                                                  
            if (IsSimilar<Block<TYPE>, TypeOf<OUT>>())
               return output.InsertBlock(IndexBack, *this);
            else
               return 0;
         }
         else {
            if (output.IsTypeConstrained()) {
               // Output container is strictly typed, we can't make     
               // loose matches                                         
               if (IsSimilar(output))
                  return output.template InsertBlock<void, false>(IndexBack, *this);
               else
                  return 0;
            }
            else {
               // Output is not strictly typed, so we can afford a      
               // looser coparison                                      
               return output.InsertBlock(IndexBack, *this);
            }
         }
      }
   }

   /// Gather items of specific phase from input container and fill output    
   ///   @tparam REVERSE - are we gathering in reverse?                       
   ///   @param type - type to search for                                     
   ///   @param input - source container                                      
   ///   @param output - [in/out] container that collects results             
   ///   @param state - the data state filter                                 
   ///   @return the number of gathered elements                              
   template<class TYPE> template<bool REVERSE>
   size_t Block<TYPE>::GatherPolarInner(
      DMeta type, CT::Block auto& output, DataState state
   ) const {
      if (GetState() % state) {
         if (IsNow() and IsDeep()) {
            // Phases don't match, but we can dig deeper if deep        
            // and neutral, since Phase::Now is permissive              
            Block<> localOutput {GetUnconstrainedState(), type};
            ForEach<REVERSE>([&](const Block<>& i) {
               i.GatherPolarInner<REVERSE>(type, localOutput, state);
            });
            localOutput.MakeNow();
            const auto inserted = output.SmartPush(IndexBack, Abandon(localOutput));
            localOutput.Free();
            return inserted;
         }

         // Polarity mismatch                                           
         return 0;
      }

      // Input is flat and neutral/same                                 
      if (not type) {
         // Output is any, so no need to iterate                        
         return output.SmartPush(IndexBack, *this);
      }

      // Iterate subpacks if any                                        
      Block<> localOutput {GetState(), type};
      GatherInner<REVERSE>(localOutput);
      localOutput.MakeNow();
      const auto inserted = output.InsertBlock(IndexBack, Abandon(localOutput));
      localOutput.Free();
      return inserted;
   }
   
   /// Compare loosely with another TMany, ignoring case                      
   /// This function applies only if T is character                           
   ///   @param other - container to compare with                             
   ///   @return true if both containers match loosely                        
   template<class TYPE> LANGULUS(INLINED)
   bool Block<TYPE>::CompareLoose(const CT::Block auto& other) const noexcept {
      return (IsEmpty() and other.IsEmpty() and IsSimilar(other))
          or MatchesLoose(other) == mCount;
   }

   /// size_t how many consecutive elements match in two containers            
   ///   @param other - container to compare with                             
   ///   @return the number of matching items                                 
   template<class TYPE> LANGULUS(INLINED)
   size_t Block<TYPE>::Matches(const CT::Block auto& other) const noexcept {
      if (IsEmpty() or other.IsEmpty())
         return 0;

      using OTHER = Deref<decltype(other)>;
      if constexpr (not TypeErased and not OTHER::TypeErased) {
         using T2 = TypeOf<OTHER>;

         if constexpr (CT::Comparable<TYPE, T2>) {
            auto t1 = GetRaw();
            auto t2 = other.GetRaw();
            const auto t1end = GetRawEnd();
            const auto t2end = other.GetRawEnd();
            while (t1 != t1end and t2 != t2end and *t1 == *t2) {
               ++t1;
               ++t2;
            }
            return t1 - GetRaw();
         }
         else return false;
      }
      else TODO();
   }

   /// Compare loosely with another, ignoring upper-case if both blocks       
   /// contain letters                                                        
   ///   @param other - container to compare with                             
   ///   @return the number of loosely matching elements                      
   template<class TYPE> LANGULUS(INLINED)
   size_t Block<TYPE>::MatchesLoose(const CT::Block auto& other) const noexcept {
      if (IsEmpty() or other.IsEmpty())
         return 0;

      using OTHER = Deref<decltype(other)>;
      if constexpr (not TypeErased and not OTHER::TypeErased) {
         using T2 = TypeOf<OTHER>;

         if constexpr (CT::Character<TYPE> and CT::Similar<TYPE, T2>) {
            auto t1 = GetRaw();
            auto t2 = other.GetRaw();
            const auto tend = GetRawEnd();
            while (t1 < tend and ::std::tolower(*t1) == ::std::tolower(*t2)) {
               ++t1;
               ++t2;
            }
            return t1 - GetRaw();
         }
         else TODO();
      }
      else TODO();
   }
   
   /// Check if anything comparable exists in the block                       
   ///   @param what - the element to search for                              
   ///   @return true if element is found, false otherwise                    
   template<class TYPE> LANGULUS(INLINED)
   bool Block<TYPE>::Contains(const CT::NoIntent auto& what) const {
      return Find(what) != IndexNone;
   }

   /// Sort the contents of this container using a static type                
   ///   @attention assumes T is the type of the container                    
   ///   @attention this is the most naive of sorting algorithms              
   ///   @tparam ASCEND - whether to sort in ascending order (123)            
   template<class TYPE> template<bool ASCEND>
   void Block<TYPE>::Sort() requires (TypeErased or CT::Sortable<TYPE, TYPE>) {
      auto lhs = GetRaw();
      const auto lhsEnd = lhs + mCount;
      while (lhs != lhsEnd) {
         auto rhs = GetRaw();
         while (rhs != lhs) {
            if constexpr (ASCEND) {
               if (*lhs < *rhs)
                  ::std::swap(*lhs, *rhs);
            }
            else {
               if (*lhs > *rhs)
                  ::std::swap(*lhs, *rhs);
            }

            ++rhs;
         }

         ++rhs;

         while (rhs != lhsEnd) {
            if constexpr (ASCEND) {
               if (*lhs < *rhs)
                  ::std::swap(*lhs, *rhs);
            }
            else {
               if (*lhs > *rhs)
                  ::std::swap(*lhs, *rhs);
            }

            ++rhs;
         }

         ++lhs;
      }
   }

} // namespace Langulus::Annies