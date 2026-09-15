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
   
   /// Iterate each element block and execute F for it                        
   ///   @tparam REVERSE - whether to iterate in reverse                      
   ///   @param call - function to execute for each element block             
   ///   @return the number of executions                                     
   template<class TYPE> template<bool REVERSE, bool MUTABLE>
   Count Block<TYPE>::ForEachElement(auto&& call) const {
      using F = Deref<decltype(call)>;
      using A = ArgumentOf<F>;
      using R = ReturnOf<F>;

      static_assert(CT::Block<A>,
         "Function argument must be a CT::Block binary-compatible type");
      static_assert(CT::Slab<A> or CT::Constant<A> or MUTABLE,
         "Non constant iterator for constant memory block");

      Count index = REVERSE ? mCount - 1 : 0;
      const auto next = [&index] {
         if constexpr (REVERSE)  --index;
         else                    ++index;
      };

      while (index < mCount) {
         A block = GetElement(index);

         if constexpr (CT::Bool<R>) {
            // If F returns bool, you can decide when to break the loop 
            // by simply returning false                                
            if (not call(block))
               return REVERSE ? mCount - index : index + 1;
            next();
         }
         else if constexpr (CT::Exact<R, LoopControl>) {
            // Do things depending on the F's return                    
            const R loop = call(block);

            switch (loop.mControl) {
            case LoopControl::Break:
               return REVERSE ? mCount - index : index + 1;
            case LoopControl::Continue:
               next();
               break;
            case LoopControl::Repeat:
               break;
            case LoopControl::Discard:
               if constexpr (MUTABLE) {
                  // Discard is allowed only if THIS is mutable         
                  // Why bother removing, when there's only one element?
                  if (mCount == 1) {
                     const_cast<Block*>(this)->Reset();
                     return mCount;
                  }

                  const_cast<Block*>(this)->RemoveIndex(index);
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
            call(block);
            next();
         }
      }

      return mCount;
   }

   template<class TYPE> template<bool REVERSE>
   Count Block<TYPE>::ForEachElement(auto&& call) {
      return const_cast<const Block*>(this)->template
         ForEachElement<REVERSE, true>(Forward<decltype(call)>(call));
   }

   /// Execute functions for each element inside container                    
   /// Each function has a distinct argument type, that is tested against the 
   /// contained type. If argument is compatible with the type, the block is  
   /// iterated, and F is executed for all elements. The rest of the provided 
   /// functions are ignored, after the first function with viable argument.  
   ///   @tparam REVERSE - whether to iterate in reverse                      
   ///   @param calls - all potential functions to iterate with               
   ///   @return the number of executions                                     
   template<class TYPE>
   template<bool REVERSE, bool MUTABLE, class...F> LANGULUS(INLINED)
   Count Block<TYPE>::ForEach(F&&...calls) const {
      static_assert(sizeof...(F) > 0, "No iterators in ForEach");
      if (IsEmpty())
         return 0;

      LoopControl loop = Loop::Break;
      Count result = 0;
      (void) (... or (Loop::NextLoop != (loop =
         ForEachInner<MUTABLE, REVERSE>(Forward<F>(calls), result)
      )));

      if (loop == Loop::Discard)
         const_cast<Block*>(this)->Reset();
      return result;
   }

   template<class TYPE> template<bool REVERSE, class...F> LANGULUS(INLINED)
   Count Block<TYPE>::ForEach(F&&...calls) {
      static_assert(sizeof...(F) > 0, "No iterators in ForEach");
      return const_cast<const Block*>(this)->template
         ForEach<REVERSE, true>(Forward<F>(calls)...);
   }

   /// Execute functions in each sub-block, inclusively                       
   /// Unlike the flat variants above, this one reaches into sub-blocks.      
   /// Each function has a distinct argument type, that is tested against the 
   /// contained type. If argument is compatible with the type, the block is  
   /// iterated, and F is executed for all elements. None of the provided     
   /// functions are ignored, unless Loop::Break is called.                   
   ///   @tparam REVERSE - whether to iterate in reverse                      
   ///   @tparam SKIP - set to false, to execute F for intermediate blocks,   
   ///                  too; otherwise will execute only for non-blocks       
   ///   @param calls - all potential functions to iterate with               
   ///   @return the number of executions                                     
   template<class TYPE>
   template<bool REVERSE, bool SKIP, bool MUTABLE, class...F> LANGULUS(INLINED)
   Count Block<TYPE>::ForEachDeep(F&&...calls) const {
      static_assert(sizeof...(F) > 0, "No iterators in ForEachDeep");
      LoopControl loop = Loop::Break;
      Count result = 0;
      (void)(... or (Loop::Break == (loop = 
         ForEachDeepInner<MUTABLE, REVERSE, SKIP>(Forward<F>(calls), result)
      )));

      if (loop == Loop::Discard)
         const_cast<Block*>(this)->Reset();
      return result;
   }

   template<class TYPE>
   template<bool REVERSE, bool SKIP, class...F> LANGULUS(INLINED)
   Count Block<TYPE>::ForEachDeep(F&&...calls) {
      static_assert(sizeof...(F) > 0, "No iterators in ForEachDeep");
      return const_cast<const Block*>(this)->template
         ForEachDeep<REVERSE, SKIP, true>(Forward<F>(calls)...);
   }

   /// Same as ForEachElement, but in reverse                                 
   template<class TYPE> template<bool MUTABLE, class...F> LANGULUS(INLINED)
   Count Block<TYPE>::ForEachElementRev(F&&...f) const {
      static_assert(sizeof...(f) > 0, "No iterators in ForEachElementRev");
      return ForEachElement<true, MUTABLE>(Forward<F>(f)...);
   }

   template<class TYPE> template<class...F> LANGULUS(INLINED)
   Count Block<TYPE>::ForEachElementRev(F&&...f) {
      static_assert(sizeof...(f) > 0, "No iterators in ForEachElementRev");
      return ForEachElement<true, true>(Forward<F>(f)...);
   }

   /// Same as ForEach, but in reverse                                        
   template<class TYPE> template<bool MUTABLE, class...F> LANGULUS(INLINED)
   Count Block<TYPE>::ForEachRev(F&&...f) const {
      static_assert(sizeof...(f) > 0, "No iterators in ForEachRev");
      return ForEach<true, MUTABLE>(Forward<F>(f)...);
   }

   template<class TYPE> template<class...F> LANGULUS(INLINED)
   Count Block<TYPE>::ForEachRev(F&&...f) {
      static_assert(sizeof...(f) > 0, "No iterators in ForEachRev");
      return ForEach<true, true>(Forward<F>(f)...);
   }

   /// Same as ForEachDeep, but in reverse                                    
   template<class TYPE>
   template<bool SKIP, bool MUTABLE, class...F> LANGULUS(INLINED)
   Count Block<TYPE>::ForEachDeepRev(F&&...f) const {
      static_assert(sizeof...(f) > 0, "No iterators in ForEachDeepRev");
      return ForEachDeep<true, SKIP, MUTABLE>(Forward<F>(f)...);
   }

   template<class TYPE> template<bool SKIP, class...F> LANGULUS(INLINED)
   Count Block<TYPE>::ForEachDeepRev(F&&...f) {
      static_assert(sizeof...(f) > 0, "No iterators in ForEachDeepRev");
      return ForEachDeep<true, SKIP, true>(Forward<F>(f)...);
   }

   /// Iterate and execute call for each flat element, counting each          
   /// successfull execution                                                  
   ///   @attention assumes block is not empty                                
   ///   @attention assumes block is typed                                    
   ///   @tparam REVERSE - whether to iterate in reverse                      
   ///   @param f - the function to execute for each element of type A        
   ///   @return the last 'f' result                                          
   template<class T> template<bool MUTABLE, bool REVERSE> LANGULUS(INLINED)
   LoopControl Block<T>::ForEachInner(auto&& f, Count& index) const
   noexcept(NoexceptIterator<decltype(f)>) {
      using F = Deref<decltype(f)>;
      using A = ArgumentOf<F>;
      using R = ReturnOf<F>;
      using DA = Decay<A>;
      using DT = Decay<T>;

      static_assert(CT::Slab<A> or CT::Constant<Deptr<A>> or MUTABLE,
         "Non-constant iterator for constant memory is not allowed");

      [[maybe_unused]]
      static constexpr auto NOE = NoexceptIterator<decltype(f)>;
      LoopControl loop = Loop::NextLoop;

      if constexpr (not TypeErased) {
         // Container is not type-erased                                
         if constexpr (CT::Deep<DA, DT> or (not CT::Deep<DA> and CT::DerivedFrom<T, A>)) {
            loop = IterateInner<MUTABLE, REVERSE>(mCount,
               [&index, &f](T& element) noexcept(NOE) -> R {
                  ++index;

                  //TODO this does only one dereference if needed, but it should actually
                  // check the difference of sparseness between A and T, and dereference as
                  // many times as needed. that way we can iterate int*** for example,
                  // even if int***** is contained
                  // it can be done on compile time without any cost whatsoever
                  if constexpr (CT::Dense<A, T> or CT::Sparse<A, T>)
                     return f( element);
                  else if constexpr (CT::Dense<A>)
                     return f(*element);
                  else
                     return f(&element);
               }
            );
         }
         else return Loop::NextLoop;
      }
      else if constexpr (not CT::Trait<DA>) {
         // Container is type-erased                                    
         // And we're NOT iterating using a trait                       
         if ((CT::Deep<DA> and IsDeep()) or (not CT::Deep<DA> and CastsTo<A, true>())) {
            if (mType->mIsSparse) {
               // Iterate sparse container                              
               loop = IterateInner<MUTABLE, REVERSE>(mCount,
                  [&index, &f](void*& element) noexcept(NOE) -> R {
                     ++index;
                     if constexpr (CT::Dense<A>)
                        return f(*reinterpret_cast<Deref<A>*>(element));
                     else
                        return f( reinterpret_cast<A>(element));
                  }
               );
            }
            else {
               // Iterate dense container where A is binary-compatible  
               // to the type, but may not match it exactly             
               LANGULUS_ASSUME(DevAssumes, GetStride() % sizeof(DA) == 0, "Unaligned iterator");
               loop = IterateInner<MUTABLE, REVERSE>(mCount * (GetStride() / sizeof(DA)),
                  [&index, &f](DA& element) noexcept(NOE) -> R {
                     ++index;
                     if constexpr (CT::Dense<A>)
                        return f( element);
                     else
                        return f(&element);
                  }
               );
            }
         }
      }
      else {
         // Container is type-erased                                    
         // And we're iterating using a trait                           
         if (not CastsTo<Trait, true>())
            return Loop::NextLoop;

         // Container is type-erased and full of traits, iterator is    
         // a static trait, so we iterate all traits, visiting only     
         // those that match the trait type                             
         if (mType->mIsSparse) {
            // Iterate sparse container                                 
            loop = IterateInner<MUTABLE, REVERSE>(mCount,
               [&index, &f](Trait*& element) noexcept(NOE) -> R {
                  if constexpr (CT::Void<R>) {
                     if (not element->template IsTrait<DA>())
                        return;
                  }
                  else if (not element->template IsTrait<DA>())
                     return Loop::Continue;

                  ++index;

                  if constexpr (CT::Dense<A>)
                     return f(*reinterpret_cast<Deref<A>*>(element));
                  else
                     return f( reinterpret_cast<A>(element));
               }
            );
         }
         else {
            // Iterate dense container                                  
            loop = IterateInner<MUTABLE, REVERSE>(mCount,
               [&index, &f](Trait& element) noexcept(NOE) -> R {
                  if constexpr (CT::Void<R>) {
                     if (not element.template IsTrait<DA>())
                        return;
                  }
                  else if (not element.template IsTrait<DA>())
                     return Loop::Continue;

                  ++index;
                  if constexpr (CT::Dense<A>)
                     return f(reinterpret_cast<Deref<A>&>( element));
                  else
                     return f(reinterpret_cast<Deref<A>*>(&element));
               }
            );
         }
      }

      return loop;
   }
   
   /// Iterate and execute call for each deep element                         
   ///   @tparam REVERSE - whether to iterate in reverse                      
   ///   @tparam SKIP - whether to execute call for intermediate blocks       
   ///   @param call - the function to execute for each element of type A     
   ///   @return the number of executions that occured                        
   template<class TYPE> template<bool MUTABLE, bool REVERSE, bool SKIP>
   LoopControl Block<TYPE>::ForEachDeepInner(auto&& call, Count& counter) const {
      using F = Deref<decltype(call)>;
      using A = ArgumentOf<F>;
      using R = ReturnOf<F>;

      static_assert(CT::Slab<A> or CT::Constant<Deptr<A>> or MUTABLE,
         "Non-constant iterator for constant memory is not allowed");
      LoopControl loop = Loop::Continue;

      if constexpr (TypeErased) {
         if constexpr (CT::Deep<A>) {
            if (not SKIP or (not IsDeep() and not Is<Neat>())) {
               // Always execute for intermediate/non-deep *this        
               ++counter;

               auto b = reinterpret_cast<Deref<A>*>(const_cast<Block*>(this));
               if constexpr (CT::Bool<R>) {
                  if (not call(*b))
                     return Loop::Break;
               }
               else if constexpr (CT::Exact<R, LoopControl>) {
                  // Do things depending on the F's return              
                  R loop = call(*b);

                  while (loop == Loop::Repeat)
                     loop = call(*b);

                  switch (loop.mControl) {
                  case LoopControl::Break:
                  case LoopControl::NextLoop:
                     return loop;
                  case LoopControl::Continue:
                  case LoopControl::Repeat:
                     break;
                  case LoopControl::Discard:
                     if constexpr (MUTABLE) {
                        // Discard is allowed only if THIS is mutable   
                        // You can't fully discard the topmost block,   
                        // only reset it. Now, if we reset this block,  
                        // and then remove it up the chain, if          
                        // branching-out happens to occur, we'll end up 
                        // with a branch that contains the empty element
                        // and that is bad. So defer the reset up the   
                        // chain instead!                               
                        return Loop::Discard;
                     }
                     else {
                        // ...otherwise it acts like a Loop::Continue   
                        break;
                     }
                  }
               }
               else call(*b);
            }
         }

         if (IsDeep()) {
            // Iterate subblocks                                        
            Count intermediateCounterSink = 0;
            using SubBlock = Conditional<MUTABLE, Block<>&, const Block<>&>;

            loop = ForEachInner<MUTABLE, REVERSE>(
               [&counter, &call](SubBlock group) {
                  if constexpr (CT::Deep<Decay<A>>) {
                     // Loop control is available only if iterator is   
                     // deep, too...                                    
                     return DenseCast(group).template
                        ForEachDeepInner<MUTABLE, REVERSE, SKIP>(
                           ::std::move(call), counter);
                  }
                  else {
                     // ... otherwise we have to pass through all deep  
                     // sub-blocks                                      
                     DenseCast(group).template
                        ForEachDeepInner<MUTABLE, REVERSE, SKIP>(
                           ::std::move(call), counter);
                  }
               },
               intermediateCounterSink
            );
         }
         else if (Is<Neat>()) {
            // Iterate normalized subblocks                             
            using SubNeat = Conditional<MUTABLE, Neat&, const Neat&>;

            loop = ForEachInner<MUTABLE, REVERSE>(
               [&call](SubNeat neat) {
                  return neat.ForEachDeep(::std::move(call));
               },
               counter
            );
         }
         else if constexpr (not CT::Deep<A>) {
            // Equivalent to non-deep iteration                         
            loop = ForEachInner<MUTABLE, REVERSE>(::std::move(call), counter);
         }
      }
      else {
         if constexpr (CT::Deep<A> and (not SKIP
         or (not CT::Deep<Decay<TYPE>> and not CT::Same<TYPE, Neat>))) {
            // Always execute for intermediate/non-deep *this           
            ++counter;

            auto b = reinterpret_cast<Deref<A>*>(const_cast<Block*>(this));
            if constexpr (CT::Bool<R>) {
               if (not call(*b))
                  return Loop::Break;
            }
            else if constexpr (CT::Exact<R, LoopControl>) {
               // Do things depending on the F's return                 
               R loop = call(*b);

               while (loop == Loop::Repeat)
                  loop = call(*b);

               switch (loop.mControl) {
               case LoopControl::Break:
               case LoopControl::NextLoop:
                  return loop;
               case LoopControl::Continue:
               case LoopControl::Repeat:
                  break;
               case LoopControl::Discard:
                  if constexpr (MUTABLE) {
                     // Discard is allowed only if THIS is mutable      
                     // You can't fully discard the topmost block,      
                     // only reset it. Now, if we reset this block,     
                     // and then remove it up the chain, if             
                     // branching-out happens to occur, we'll end up    
                     // with a branch that contains the empty element   
                     // and that is bad. So defer the reset up the      
                     // chain instead!                                  
                     return Loop::Discard;
                  }
                  else {
                     // ...otherwise it acts like a Loop::Continue      
                     break;
                  }
               }
            }
            else call(*b);
         }

         if constexpr (CT::Deep<Decay<TYPE>>) {
            // Iterate subblocks                                        
            Count intermediateCounterSink = 0;
            using SubBlock = Conditional<MUTABLE, Decay<TYPE>&, const Decay<TYPE>&>;

            loop = ForEachInner<MUTABLE, REVERSE>(
               [&counter, &call](SubBlock group) {
                  return DenseCast(group).template
                     ForEachDeepInner<MUTABLE, REVERSE, SKIP>(
                        ::std::move(call), counter);
               },
               intermediateCounterSink
            );
         }
         else if constexpr (CT::Same<TYPE, Neat>) {
            // Iterate normalized subblocks                             
            using SubNeat = Conditional<MUTABLE, Neat&, const Neat&>;

            loop = ForEachInner<MUTABLE, REVERSE>(
               [&call](SubNeat neat) {
                  return neat.ForEachDeep(::std::move(call));
               },
               counter
            );
         }
         else if constexpr (not CT::Deep<A>) {
            // Equivalent to non-deep iteration                         
            loop = ForEachInner<MUTABLE, REVERSE>(::std::move(call), counter);
         }
      }

      return loop;
   }

   /// Execute a function for each element inside container                   
   /// Lowest-level element iteration function (for internal use only)        
   ///   @attention assumes A is binary compatible with the contained type    
   ///   @attention assumes block is not empty                                
   ///   @attention assumes sparseness matches                                
   ///   @tparam REVERSE - direction we're iterating in                       
   ///   @param call - the constexpr noexcept function to call on each item   
   template<class TYPE> template<bool MUTABLE, bool REVERSE> LANGULUS(INLINED)
   LoopControl Block<TYPE>::IterateInner(Count count, auto&& f) const
   noexcept(NoexceptIterator<decltype(f)>) {
      using F = Deref<decltype(f)>;
      using A = ArgumentOf<F>;
      using R = ReturnOf<F>;

      static_assert(CT::Complete<Decay<A>> or CT::Sparse<A>,
         "Can't iterate with incomplete type, use pointer instead");

      LANGULUS_ASSUME(DevAssumes, IsTyped(),
         "Block is not typed");
      LANGULUS_ASSUME(DevAssumes, not IsEmpty(),
         "Block is empty", " (of type `", mType, "`)");
      LANGULUS_ASSUME(DevAssumes, IsSparse() == CT::Sparse<A>,
         "Sparseness mismatch", " (`", mType, 
         "` compared against `", MetaDataOf<A>(), "`)"
      );

      if constexpr (CT::Dense<A>) {
         using DA = Decay<A>;
         LANGULUS_ASSUME(DevAssumes,
            (CT::Deep<DA> and IsDeep()) or (not CT::Deep<DA> and CastsTo<A, true>()),
            "Incompatible iterator type", " `", MetaDataOf<A>(),
            "` (iterating block of type `", mType, "`)"
         );
      }

      // Prepare for the loop                                           
      using DA = Deref<A>;
      auto raw = const_cast<Block*>(this)->GetRaw<DA>();
      auto data = raw;
      if constexpr (REVERSE)
         data += count - 1;

      const auto next = [&data] {
         if constexpr (REVERSE)  --data;
         else                    ++data;
      };

      auto dataEnd = REVERSE ? raw - 1 : raw + count;
      while (data != dataEnd) {
         // Execute function                                            
         if constexpr (CT::Bool<R>) {
            if (not f(*data))
               return Loop::Break;
            next();
         }
         else if constexpr (CT::Exact<R, LoopControl>) {
            // Do things depending on the F's return                    
            const R loop = f(*data);

            switch (loop.mControl) {
            case LoopControl::Break:
            case LoopControl::NextLoop:
               return loop;
            case LoopControl::Continue:
               next();
               break;
            case LoopControl::Repeat:
               break;
            case LoopControl::Discard:
               if constexpr (MUTABLE) {
                  // Discard is allowed only if THIS is mutable         
                  // Why bother removing, when there's only one element?
                  // Just propagate the discard instead! The pack       
                  // should be reset from above.                        
                  if (mCount == 1)
                     return Loop::Discard;

                  const Offset idx = raw - data;
                  const_cast<Block*>(this)->RemoveIndex(idx);

                  /*if (IsDeep() and mCount == 1) { //TODO this is quite experimental and not fully working right now. can be achieved by Optimize() after the loop for now
                     // Is only one element remaining? Is that element  
                     // deep? Then optimize this container away!        
                     auto temporary = GetDeep();
                     const_cast<Block*>(this)->GetDeep().ResetMemory();
                     const_cast<Block*>(this)->Free();
                     *const_cast<Block*>(this) = temporary;
                     return Loop::Repeat;
                  }
                  else {*/
                     // Block might BranchOut on RemoveIndex - make sure
                     // 'raw', 'data' and 'dataEnd' are up-to-date with 
                     // new block memory                                
                     --count;
                     raw = const_cast<Block*>(this)->GetRaw<DA>();
                     data = raw + idx;
                     dataEnd = REVERSE ? raw - 1 : raw + count;

                     if constexpr (REVERSE)
                        next();
                  //}
               }
               else {
                  // ...otherwise it acts like a Loop::Continue         
                  next();
               }
               break;
            }
         }
         else {
            f(*data);
            next();
         }
      }

      return Loop::Continue;
   }
   
   /// Get iterator to first element                                          
   ///   @return an iterator to the first element, or end if empty            
   template<class TYPE> LANGULUS(INLINED)
   constexpr auto Block<TYPE>::begin() noexcept -> Iterator {
      if (IsEmpty())
         return end();

      if constexpr (TypeErased)
         return {GetElement(), GetRawEnd()};
      else
         return {GetRaw(), GetRawEnd()};
   }

   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   constexpr auto Block<TYPE>::begin() const noexcept -> ConstIterator {
      return const_cast<Block<TYPE>*>(this)->begin();
   }

   /// Get iterator to the last element                                       
   ///   @return an iterator to the last element, or end if empty             
   template<class TYPE> LANGULUS(INLINED)
   constexpr auto Block<TYPE>::last() noexcept -> Iterator {
      if (IsEmpty())
         return end();

      if constexpr (TypeErased)
         return {GetElement(), GetRawEnd()};
      else {
         const auto ptr = GetRaw();
         return {ptr + mCount - 1, ptr + mCount};
      }
   }

   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   constexpr auto Block<TYPE>::last() const noexcept -> ConstIterator {
      return const_cast<Block<TYPE>*>(this)->last();
   }
   
   /// Prefix increment - get next element by incrementing data pointer       
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   auto Block<TYPE>::operator ++ () IF_UNSAFE(noexcept) -> Block& {
      LANGULUS_ASSUME(DevAssumes, mRaw,
         "Block is not allocated");

      if constexpr (TypeErased) {
         LANGULUS_ASSUME(DevAssumes, IsTyped(),
            "Block is not typed");
         LANGULUS_ASSUME(DevAssumes, mRaw + mType->mSize <= mEntry->GetBlockEnd(),
            "Block limit breached");
         mRaw += mType->mSize;
      }
      else {
         LANGULUS_ASSUME(DevAssumes, mRaw + sizeof(TYPE) <= mEntry->GetBlockEnd(),
            "Block limit breached");
         mRaw += sizeof(TYPE);
      }

      return *this;
   }

   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   auto Block<TYPE>::operator ++ () const IF_UNSAFE(noexcept) -> Block const& {
      return const_cast<Block&>(*this).operator++();
   }

   /// Prefix decrement - get previous element by decrementing data pointer   
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   auto Block<TYPE>::operator -- () IF_UNSAFE(noexcept) -> Block& {
      LANGULUS_ASSUME(DevAssumes, mRaw,
         "Block is not allocated");

      if constexpr (TypeErased) {
         LANGULUS_ASSUME(DevAssumes, IsTyped(),
            "Block is not typed");
         LANGULUS_ASSUME(DevAssumes, mRaw - mType->mSize >= mEntry->GetBlockStart(),
            "Block limit breached");
         mRaw -= mType->mSize;
      }
      else {
         LANGULUS_ASSUME(DevAssumes, mRaw - sizeof(TYPE) >= mEntry->GetBlockStart(),
            "Block limit breached");
         mRaw -= sizeof(TYPE);
      }

      return *this;
   }

   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   auto Block<TYPE>::operator -- () const IF_UNSAFE(noexcept) -> Block const& {
      return const_cast<Block&>(*this).operator--();
   }

   /// Suffic increment - get next element by incrementing data pointer       
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   auto Block<TYPE>::operator ++ (int) const IF_UNSAFE(noexcept) -> Block {
      auto copy {*this};
      return ++copy;
   }

   /// Suffic decrement - get previous element by decrementing data pointer   
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   auto Block<TYPE>::operator -- (int) const IF_UNSAFE(noexcept) -> Block {
      auto copy {*this};
      return --copy;
   }
   
   /// Offset the handle                                                      
   ///   @param offset - the offset to apply                                  
   ///   @return the offsetted handle                                         
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   auto Block<TYPE>::operator + (Offset offset) const IF_UNSAFE(noexcept) -> Block {
      auto copy {*this};
      return copy += offset;
   }

   /// Offset the handle                                                      
   ///   @param offset - the offset to apply                                  
   ///   @return the offsetted handle                                         
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   auto Block<TYPE>::operator - (Offset offset) const IF_UNSAFE(noexcept) -> Block {
      auto copy {*this};
      return copy -= offset;
   }
   
   /// Prefix increment operator                                              
   ///   @return the next handle                                              
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   auto Block<TYPE>::operator += (Offset offset) IF_UNSAFE(noexcept) -> Block& {
      LANGULUS_ASSUME(DevAssumes, mRaw,
         "Block is not allocated");

      if constexpr (TypeErased) {
         LANGULUS_ASSUME(DevAssumes, IsTyped(),
            "Block is not typed");
         LANGULUS_ASSUME(DevAssumes, mRaw + offset * mType->mSize <= mEntry->GetBlockEnd(),
            "Block limit breached");
         mRaw += offset * mType->mSize;
      }
      else {
         LANGULUS_ASSUME(DevAssumes, mRaw + offset * sizeof(TYPE) <= mEntry->GetBlockEnd(),
            "Block limit breached");
         mRaw += offset * sizeof(TYPE);
      }

      return *this;
   }

   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   auto Block<TYPE>::operator += (Offset offset) const IF_UNSAFE(noexcept) -> Block const& {
      return const_cast<Block&>(*this).operator+=(offset);
   }

   /// Prefix decrement operator                                              
   ///   @return the next handle                                              
   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   auto Block<TYPE>::operator -= (Offset offset) IF_UNSAFE(noexcept) -> Block& {
      LANGULUS_ASSUME(DevAssumes, mRaw,
         "Block is not allocated");

      if constexpr (TypeErased) {
         LANGULUS_ASSUME(DevAssumes, IsTyped(),
            "Block is not typed");
         LANGULUS_ASSUME(DevAssumes, mRaw - offset * mType->mSize >= mEntry->GetBlockStart(),
            "Block limit breached");
         mRaw -= offset * mType->mSize;
      }
      else {
         LANGULUS_ASSUME(DevAssumes, mRaw - offset * sizeof(TYPE) >= mEntry->GetBlockStart(),
            "Block limit breached");
         mRaw -= offset * sizeof(TYPE);
      }

      return *this;
   }

   template<class TYPE> LANGULUS(ALWAYS_INLINED)
   auto Block<TYPE>::operator -= (Offset offset) const IF_UNSAFE(noexcept) -> Block const& {
      return const_cast<Block&>(*this).operator-=(offset);
   }



   /// Construct an iterator                                                  
   ///   @param start - the current iterator position                         
   ///   @param end - the ending marker                                       
   template<class T> LANGULUS(ALWAYS_INLINED)
   constexpr TBlockIterator<T>::TBlockIterator(const TypeInner& start, Type const* end) noexcept
      : mValue {start}
      , mEnd   {end} {}

   /// Construct an end iterator                                              
   template<class T> LANGULUS(ALWAYS_INLINED)
   constexpr TBlockIterator<T>::TBlockIterator(A::IteratorEnd) noexcept
      : mValue {nullptr}
      , mEnd   {nullptr} {}

   /// Compare two iterators                                                  
   ///   @param rhs - the other iterator                                      
   ///   @return true if iterators point to the same element                  
   template<class T> LANGULUS(ALWAYS_INLINED)
   constexpr bool TBlockIterator<T>::operator == (const TBlockIterator& rhs) const noexcept {
      if constexpr (T::TypeErased)
         return mValue.mRaw == rhs.mValue.mRaw;
      else
         return mValue == rhs.mValue;
   }

   /// Compare iterator with an end marker                                    
   ///   @return true if element is at or beyond the end marker               
   template<class T> LANGULUS(ALWAYS_INLINED)
   constexpr bool TBlockIterator<T>::operator == (A::IteratorEnd) const noexcept {
      if constexpr (T::TypeErased)
         return mValue.mRaw >= mEnd;
      else
         return mValue >= mEnd;
   }

   /// Prefix increment operator                                              
   ///   @attention assumes iterator points to a valid element                
   ///   @return the modified iterator                                        
   template<class T> LANGULUS(ALWAYS_INLINED)
   constexpr auto TBlockIterator<T>::operator ++ () noexcept -> TBlockIterator& {
      ++mValue;
      return *this;
   }

   /// Suffix increment operator                                              
   ///   @attention assumes iterator points to a valid element                
   ///   @return the previous value of the iterator                           
   template<class T> LANGULUS(ALWAYS_INLINED)
   constexpr auto TBlockIterator<T>::operator ++ (int) noexcept -> TBlockIterator {
      const auto backup = *this;
      operator ++ ();
      return backup;
   }

   /// Check if iterator is valid                                             
   template<class T> LANGULUS(ALWAYS_INLINED)
   constexpr TBlockIterator<T>::operator bool() const noexcept {
      return *this != A::IteratorEnd {};
   }
}