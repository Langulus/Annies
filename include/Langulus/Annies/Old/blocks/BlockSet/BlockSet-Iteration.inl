///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../BlockSet.hpp"


namespace Langulus::Annies
{
   
   /// Iterate and execute call for each flat element, counting each          
   /// successfull execution                                                  
   ///   @attention assumes block is not empty                                
   ///   @attention assumes block is typed                                    
   ///   @tparam REVERSE - whether to iterate in reverse                      
   ///   @param f - the function to execute for each element of type A        
   ///   @return the number of executions that occured                        
   template<CT::Set THIS, bool REVERSE> LANGULUS(INLINED)
   LoopControl BlockSet::ForEachInner(auto&& f, size_t& executions) const
   noexcept(NoexceptIterator<decltype(f)>) {
      using F = Deref<decltype(f)>;
      using A = ArgumentOf<F>;
      using R = ReturnOf<F>;
      constexpr auto NOE = NoexceptIterator<decltype(f)>;

      auto& keys = GetValues<THIS>();
      constexpr bool MUTABLE = CT::Mutable<THIS>;

      if (   (CT::Deep<A> and keys.IsDeep())
      or (not CT::Deep<A> and keys.template CastsTo<A, true>())) {
         size_t index = 0;
         if (mKeys.mType->mIsSparse) {
            // Iterate using pointers of A                              
            using DA = Conditional<CT::Mutable<THIS>, Deref<A>*, const Deref<A>*>;
            return mKeys.IterateInner<MUTABLE, REVERSE>(mKeys.mReserved,
               [&](DA element) noexcept(NOE) -> R {
                  if (not mInfo[index++]) {
                     if constexpr (CT::Bool<R>)
                        return true;
                     else if constexpr (CT::Exact<R, LoopControl>)
                        return Loop::Continue;
                     else
                        return;
                  }

                  ++executions;
                  return f(*element);
               }
            );
         }
         else {
            // Iterate using references of A                            
            using DA = Conditional<CT::Mutable<THIS>, Deref<A>&, const Deref<A>&>;
            return mKeys.IterateInner<MUTABLE, REVERSE>(mKeys.mReserved,
               [&](DA element) noexcept(NOE) -> R {
                  if (not mInfo[index++]) {
                     if constexpr (CT::Bool<R>)
                        return true;
                     else if constexpr (CT::Exact<R, LoopControl>)
                        return Loop::Continue;
                     else
                        return;
                  }

                  ++executions;
                  return f(element);
               }
            );
         }
      }
      else return Loop::NextLoop;
   }
         
   /// Iterate all keys inside the set, and perform f() on them               
   /// You can break the loop, by returning false inside f()                  
   ///   @param f - the function to call for each key block                   
   ///   @return the number of successful f() executions                      
   template<bool REVERSE, CT::Set THIS>
   size_t BlockSet::ForEachElement(auto&& call) const {
      using F = Deref<decltype(call)>;
      using A = ArgumentOf<F>;
      using R = ReturnOf<F>;

      static_assert(CT::Block<A>,
         "Function argument must be a CT::Block binary-compatible type");
      static_assert(CT::Constant<A> or CT::Mutable<THIS>,
         "Non constant iterator for constant memory block");

      size_t counter = 0;
      auto info = REVERSE ? mInfo + GetReserved() - 1 : mInfo;
      const auto infoEnd = REVERSE ? mInfo - 1 : mInfo + GetReserved();
      const auto next = [&info] {
         if constexpr (REVERSE)  --info;
         else                    ++info;
      };

      while (info != infoEnd) {
         if (not *info) {
            next();
            continue;
         }

         ++counter;

         if constexpr (CT::Bool<R>) {
            if (not call(mKeys.GetElement(info - mInfo)))
               return counter;
            next();
         }
         else if constexpr (CT::Exact<R, LoopControl>) {
            const R loop = call(mKeys.GetElement(info - mInfo));

            switch (loop) {
            case LoopControl::Break:
            case LoopControl::NextLoop:
               return counter;
            case LoopControl::Continue:
               next();
               break;
            case LoopControl::Repeat:
               break;
            case LoopControl::Discard:
               if constexpr (CT::Mutable<THIS>) {
                  // Discard is allowed only if THIS is mutable         
                  const_cast<BlockSet*>(this)
                     ->template RemoveInner<THIS>(info - mInfo);
                  if constexpr (REVERSE)
                     next();
               }
               else {
                  // ...otherwise it acts like a Loop::Continue         
                  next();
               }
               break;
            }
         }
         else {
            call(mKeys.GetElement(info - mInfo));
            next();
         }
      }

      return counter;
   }
   
   /// Iterate and execute call for each deep element                         
   ///   @tparam REVERSE - whether to iterate in reverse                      
   ///   @param call - the function to execute for each element of type A     
   ///   @return the number of executions that occured                        
   template<CT::Set THIS, bool REVERSE, bool SKIP>
   LoopControl BlockSet::ForEachDeepInner(auto&& call, size_t& counter) const {
      using F = Deref<decltype(call)>;
      using A = ArgumentOf<F>;
      using SubBlock = Conditional<CT::Mutable<THIS>, Block<>&, const Block<>&>;

      if (IsDeep<THIS>()) {
         // Iterate deep keys/values using non-block type               
         return ForEachInner<THIS, REVERSE>(
            [&counter, &call](SubBlock group) -> LoopControl {
               return DenseCast(group).template
                  ForEachDeepInner<SubBlock, REVERSE, SKIP>(
                     ::std::move(call), counter);
            }, counter
         );
      }
      else if constexpr (not CT::Deep<A>) {
         // Equivalent to non-deep iteration                            
         return ForEachInner<THIS, REVERSE>(::std::move(call), counter);
      }

      return Loop::Continue;
   }

   /// Iterate keys inside the map, and perform a set of functions on them    
   /// depending on the contained type                                        
   /// You can break the loop, by returning false inside f()                  
   ///   @param f - the functions to call for each key block                  
   ///   @return the number of successful f() executions                      
   template<bool REVERSE, CT::Set THIS, class...F> LANGULUS(INLINED)
   size_t BlockSet::ForEach(F&&...f) const {
      static_assert(sizeof...(F) > 0, "No iterators in ForEach");
      if (IsEmpty())
         return 0;

      size_t result = 0;
      (void) (... or (Loop::NextLoop != 
         ForEachInner<THIS, REVERSE>(Forward<F>(f), result)
      ));
      return result;
   }

   /// Iterate each subblock of keys inside the set, and perform a set of     
   /// functions on them                                                      
   ///   @param calls - the functions to call for each key block              
   ///   @return the number of successful f() executions                      
   template<bool REVERSE, bool SKIP, CT::Set THIS, class...F>
   LANGULUS(INLINED) size_t BlockSet::ForEachDeep(F&&...calls) const {
      static_assert(sizeof...(F) > 0, "No iterators in ForEachDeep");
      size_t result = 0;
      (void)(... or (Loop::Break != 
         ForEachDeepInner<THIS, REVERSE, SKIP>(Forward<F>(calls), result)
      ));
      return result;
   }

   /// Get iterator to first element                                          
   ///   @return an iterator to the first element, or end if empty            
   template<CT::Set SET> LANGULUS(INLINED)
   typename BlockSet::Iterator<SET> BlockSet::begin() noexcept {
      if (IsEmpty())
         return end();

      // Seek first valid info, or hit sentinel at the end              
      auto info = GetInfo();
      while (not *info)
         ++info;

      const auto offset = info - GetInfo();
      return {info, GetInfoEnd(), GetRaw<SET>(offset)};
   }

   /// Get iterator to the last element                                       
   ///   @return an iterator to the last element, or end if empty             
   template<CT::Set SET> LANGULUS(INLINED)
   typename BlockSet::Iterator<SET> BlockSet::last() noexcept {
      if (IsEmpty())
         return end();

      // Seek first valid info in reverse, until one past first is met  
      auto info = GetInfoEnd();
      while (info >= GetInfo() and not *--info)
         ;

      const auto offset = info - GetInfo();
      return {info, GetInfoEnd(), GetRaw<SET>(offset)};
   }

   /// Get iterator to first element                                          
   ///   @return a constant iterator to the first element, or end if empty    
   template<CT::Set SET> LANGULUS(INLINED)
   typename BlockSet::Iterator<const SET> BlockSet::begin() const noexcept {
      if (IsEmpty())
         return end();

      // Seek first valid info, or hit sentinel at the end              
      auto info = GetInfo();
      while (not *info)
         ++info;

      const auto offset = info - GetInfo();
      return {info, GetInfoEnd(), GetRaw<SET>(offset)};
   }

   /// Get iterator to the last valid element                                 
   ///   @return a constant iterator to the last element, or end if empty     
   template<CT::Set SET> LANGULUS(INLINED)
   typename BlockSet::Iterator<const SET> BlockSet::last() const noexcept {
      if (IsEmpty())
         return end();

      // Seek first valid info in reverse, until one past first is met  
      auto info = GetInfoEnd();
      while (info >= GetInfo() and not *--info)
         ;

      const auto offset = info - GetInfo();
      return {info, GetInfoEnd(), GetRaw<SET>(offset)};
   }
   
   
   ///                                                                        
   ///   Set iterator                                                         
   ///                                                                        

   /// Construct a set iterator                                               
   ///   @param info - the info pointer                                       
   ///   @param sentinel - the end of info pointers                           
   ///   @param key - pointer/block to the key element                        
   template<class SET> LANGULUS(INLINED)
   constexpr BlockSet::Iterator<SET>::Iterator(
      const InfoType* info, 
      const InfoType* sentinel, 
      const InnerT&   key
   ) noexcept
      : mInfo {info}
      , mSentinel {sentinel}
      , mKey {key} {}

   /// Construct from end point                                               
   template<class SET> LANGULUS(INLINED)
   constexpr BlockSet::Iterator<SET>::Iterator(const A::IteratorEnd&) noexcept
      : mInfo {}
      , mSentinel {}
      , mKey {} {}

   /// Prefix increment operator                                              
   /// Moves pointers to the right, unless end has been reached               
   ///   @return the modified iterator                                        
   template<class SET> LANGULUS(INLINED)
   constexpr BlockSet::Iterator<SET>& BlockSet::Iterator<SET>::operator ++ () noexcept {
      if (mInfo == mSentinel)
         return *this;

      // Seek next valid info, or hit sentinel at the end               
      const auto previous = mInfo;
      while (not *++mInfo)
         ;

      mKey += mInfo - previous;
      return *this;
   }

   /// Suffix increment operator                                              
   /// Moves pointers to the right, unless end has been reached               
   ///   @return the previous value of the iterator                           
   template<class SET> LANGULUS(INLINED)
   constexpr BlockSet::Iterator<SET> BlockSet::Iterator<SET>::operator ++ (int) noexcept {
      const auto backup = *this;
      operator ++ ();
      return backup;
   }

   /// Compare iterators                                                      
   ///   @param rhs - the other iterator                                      
   ///   @return true if entries match                                        
   template<class SET> LANGULUS(INLINED)
   constexpr bool BlockSet::Iterator<SET>::operator == (const Iterator& rhs) const noexcept {
      return mInfo == rhs.mInfo;
   }

   /// Check if iterator has reached the end                                  
   ///   @return true if entries match                                        
   template<class SET> LANGULUS(INLINED)
   constexpr bool BlockSet::Iterator<SET>::operator == (const A::IteratorEnd&) const noexcept {
      return mInfo >= mSentinel;
   }

   /// Iterator access operator                                               
   /// It is required for ranged-for expressions                              
   /// In our case, it just generates a temporary pair of references          
   ///   @return the pair at the current iterator position                    
   template<class SET> LANGULUS(INLINED)
   constexpr decltype(auto) BlockSet::Iterator<SET>::operator * () const {
      if (mInfo >= mSentinel)
         LANGULUS_OOPS(Access, "Trying to access end of iteration");

      if constexpr (CT::Typed<SET>)
         return *mKey;
      else
         return (mKey);
   }

   /// Explicit bool operator, to check if iterator is valid                  
   template<class SET> LANGULUS(INLINED)
   constexpr BlockSet::Iterator<SET>::operator bool() const noexcept {
      return mInfo < mSentinel;
   }

   /// Implicitly convert to a constant iterator                              
   template<class SET> LANGULUS(INLINED)
   constexpr BlockSet::Iterator<SET>::operator Iterator<const SET>() const noexcept requires Mutable {
      return {mInfo, mSentinel, mKey};
   }

} // namespace Langulus::Annies
