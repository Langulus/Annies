///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Iteration-Range.hpp"
//#include <Langulus/CT/Deep.hpp>
#include <Langulus/CT/ReflectAs.hpp>
#include <Langulus/CT/DefineTag.hpp>
#include <Langulus/CT/DefineVerb.hpp>
#include <Langulus/Lambda.hpp>
#include <Langulus/Assume.hpp>
//#include <Langulus/MetaOf.hpp>


namespace Langulus::Annies::Component
{
   /// A helper structure that shows how ForEach iteration went               
   //template<CT::Container C>
   struct ForEachResult {
      //static_assert(CT::Decayed<C>, "Trip all decorations from C first");

      //using Count = typename Deref<C>::CountType;

      // Number of iterations                                           
      /*Count*/ size_t count = 0;

      // Last loop control return - useful only when you want to        
      // control an outer loop depending on the iteration result        
      LoopControl control = Loop::Continue;

      // Implicitly cast to the count member, because that's the        
      // most likely use                                                
      operator /*Count*/ size_t() const noexcept { return count; }
   };

   ///                                                                        
   /// Implements ForEach iteration interface for containers.                 
   /// This iteration is much more flexible than the usual ranged-based one.  
   /// It allows you to:                                                      
   ///  - act only on types that you want;                                    
   ///  - remove, add and swap elements while iterating;                      
   ///  - control the flow of the loop.                                       
   ///   @tparam ID, SHARED providers that get iterated together              
   template<Cid ID, Cid...SHARED>
   struct IterationForEach {
      using CTTI_Component = Yup;
      using CTTI_ReflectAs = void;
      using Id             = Values<ID, SHARED...>;

      static constexpr int ComponentPrecedence = 3000;

   private:
      //template<CT::Container C>
      //using Count = typename Deref<C>::CountType;
      //template<CT::Container C>
      //using Deep = LglsMutIf(C, typename Deref<C>::DeepType&);
      //template<CT::Container C>
      //using ForEachResult = Inner::ForEachResult;//<Decay<C>>;

   public:
      /*template<CT::Container C>
      auto ForEachElement(this C&&, auto&&...) -> ForEachResult<C>;
      template<CT::Container C>
      auto ForEachElementRev(this C&&, auto&&...) -> ForEachResult<C>;*/ //TODO just use Apply now?

      /// Execute functions for each element inside container.                
      /// Each function has a distinct argument type, that is tested against  
      /// the contained type. If argument is compatible with the type, the    
      /// container is iterated and the function - executed for all elements. 
      /// The rest of the provided functions are ignored after the first      
      /// function with viable argument.                                      
      ///   @param lambdas all potential functions to iterate with            
      ///   @return the number of executions and the control end code         
      template<CT::Container C, class...F>
      auto ForEach(this C&& self, F&&...lambdas) -> ForEachResult/*<C>*/ {
         static_assert(sizeof...(F) > 0, "No functions in ForEach");
         if (self.IsEmpty())
            return {};

         ForEachResult/*<C>*/ result {0, Loop::Break};
         (void)(... or (Loop::NextLoop != (
            result.control = self.template
               ForEachInner<false>(LglsFwd(lambdas), result.count)
         )));

         if constexpr (CT::Mutable<C>) {
            if (result.control == Loop::Discard)
               self.Reset();
         }
         return result;
      }

      /// Do it in reverse                                                    
      template<CT::Container C, class...F>
      auto ForEachRev(this C&& self, F&&...lambdas) -> ForEachResult/*<C>*/ {
         static_assert(sizeof...(F) > 0, "No functions in ForEachRev");
         if (self.IsEmpty())
            return {};

         ForEachResult/*<C>*/ result {0, Loop::Break};
         (void)(... or (Loop::NextLoop != (
            result.control = self.template
               ForEachInner<true>(LglsFwd(lambdas), result.count)
         )));

         if constexpr (CT::Mutable<C>) {
            if (result.control == Loop::Discard)
               self.Reset();
         }
         return result;
      }


   protected:
      /// Iterate and execute call for each flat element, counting each       
      /// successfull execution.                                              
      ///   @attention assumes block is typed and non empty                   
      ///   @tparam REVERSE whether to iterate in reverse                     
      ///   @param f the function to execute for each element of type A       
      ///   @param index [out] counts the successful executions               
      ///   @return the last 'f' result - dictates whether loop continues     
      template<bool REVERSE, CT::Container C, class F>
      LoopControl ForEachInner(this C&& self, F&& f, /*Count<C>*/size_t& index) noexcept_if(f) {
         using A = ArgumentOf<F>;
         static_assert(CT::Slab<A> or CT::Mutable<C> or CT::ConstantEverywhere<A>,
            "Mutable reference/pointer iterator not allowed for constant container");
         LglsAssumeDev(not self.IsEmpty(), "Can't iterate empty container");
         //LglsAssumeDev(    self.IsTyped(), "Can't iterate untyped container");
         using R = ReturnOf<F>;

         if constexpr (not CT::TypeErased<C>) {
            // Container is statically-typed.                           
            // Leverage compile-time optimizations.                     
            using T = TypeOf<C>;

            if constexpr (CT::Deep<A, T> or (not CT::Deep<A> and CT::DerivedFrom<T, A>)) {
               return self.template IterateInner<REVERSE>(
                  self.GetCount(),
                  [&index, &f](T& element) noexcept_if(f) -> R {
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
         else {
            // Container is type-erased. We're NOT iterating with tag.  
            const auto T = self.GetType();
            if (not (CT::Deep<A>       and T.IsDeep())
            and not (CT::DefineTag<A>  and T.Is(MetaDataOf<A /*Decay<A>*/>()))
            and not (CT::DefineVerb<A> and T.Is(MetaDataOf<A /*Decay<A>*/>()))
            and not (not CT::Deep<A>   and self.template CastsTo<A>(1, true)))
               return Loop::NextLoop;

            // Iterate container where A is binary-compatible to the    
            // type, but may not be it exactly.                         
            using IT_STRAT = IterateHandles<REVERSE, Deref<C>>;
            for (auto handle : IT_STRAT(self)) {
               decltype(auto) element = handle.template As<Deref<A>>();
               if constexpr (CT::DefineTag<A>) {
                  // We're iterating using a statically defined tag.    
                  // We don't execute in tags that don't match.         
                  if (DenseCast(element).GetTagInner() != MetaTagOf<A /*Decay<A>*/>())
                     continue;
               }
               if constexpr (CT::DefineVerb<A>) {
                  // We're iterating using a statically defined verb.   
                  // We don't execute in verbs that don't match.        
                  if (DenseCast(element).GetVerbInner() != MetaVerbOf<A /*Decay<A>*/>())
                     continue;
               }

               ++index;

               if constexpr (CT::Bool<R>) {
                  // Execute and consider 'f' returning true/false      
                  if (not f(element))
                     return Loop::Break;
               }
               else if constexpr (Exact<R, LoopControl>) {
                  // Execute and consider 'f' returning LoopControl     
                  const R loop = f(element);
                  switch (loop.mControl) {
                  case LoopControl::Break:
                  case LoopControl::NextLoop: return loop;
                  case LoopControl::Continue: break;
                  case LoopControl::Repeat: --handle; break;
                  case LoopControl::Discard:
                     if constexpr (CT::Mutable<C>) {
                        // Discard is allowed only if THIS is mutable   
                        // Why bother removing, when there's only one   
                        // element? Just propagate discard instead!     
                        // The pack should be reset from above.         
                        if (self.GetCount() == 1)
                           return Loop::Discard;

                        handle = self.Erase(handle);
                        --handle;
                     }
                     else {
                        LglsAssumeUserWarn(false,
                           "Attempting to Loop::Discard while iterating constant container. "
                           "No discard will be performed."
                        );
                     }
                     break;
                  }
               }
               else {
                  // Just execute, always loop through everything       
                  f(element);
               }
            }
            return Loop::NextLoop;
         }
      }

      /// Execute a function for each element inside container.               
      /// Lowest-level element iteration function (for internal use only).    
      ///   @attention assumes A is binary compatible with the contained type 
      ///   @attention assumes container is not empty                         
      ///   @attention assumes sparseness matches                             
      ///   @tparam REVERSE direction we're iterating in                      
      ///   @param f the function to call on each item                        
      template<bool REVERSE, CT::Container C, class F>
      LoopControl IterateInner(this C&& self, /*Count<C>*/size_t count, F&& f) noexcept(IsNoexcept<F>) {
         using A = ArgumentOf<F>;
         static_assert(CT::Complete<Decay<A>> or CT::Sparse<A>,
            "Can't iterate with incomplete type, use pointer instead");
         LglsAssumeDev(self.IsTyped(), 
            "Block is not typed");
         LglsAssumeDev(not self.IsEmpty(),
            "Block is empty (of type `", self.GetType(), "`)");
         LglsAssumeDev(self.IsSparse() == CT::Sparse<A>,
            "Sparseness mismatch (`", self.GetType(),
            "` compared against `", MetaDataOf<A>(), "`)");

         if constexpr (CT::Dense<A>) {
            LglsAssumeDev((self.template CastsTo<A>(1, true)),
               "Incompatible iterator type", " `", MetaDataOf<A>(), 
               "` (iterating block of type `", self.GetType(), "`)");
         }

         // Prepare for the loop                                        
         using DA = Deref<A>;
         auto raw = self.template GetRawAs<DA>();
         auto data = raw;
         if constexpr (REVERSE)
            data += count - 1;
         const auto next = [&data] {
            if constexpr (REVERSE)  --data;
            else                    ++data;
         };
         auto dataEnd = REVERSE ? raw - 1 : raw + count;

         using R = ReturnOf<F>;
         while (data != dataEnd) {
            // Execute function                                         
            if constexpr (CT::Bool<R>) {
               if (not f(*data))
                  return Loop::Break;
               next();
            }
            else if constexpr (Exact<R, LoopControl>) {
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
                  if constexpr (CT::Mutable<C>) {
                     // Discard is allowed only if THIS is mutable      
                     // Why bother removing, when there's only one      
                     // element? Just propagate the discard instead!    
                     // The pack should be reset from above either way  
                     if (self.GetCount() == 1)
                        return Loop::Discard;

                     const /*Count<C>*/ size_t idx = raw - data;
                     self.EraseAt(idx);

                     // Block might BranchOut on RemoveIndex - make     
                     // sure 'raw', 'data' and 'dataEnd' are up-to-     
                     // date with new block memory                      
                     --count;
                     raw = self.template GetRawAs<DA>();
                     data = raw + idx;
                     dataEnd = REVERSE ? raw - 1 : raw + count;

                     if constexpr (REVERSE)
                        next();
                  }
                  else {
                     // ...otherwise it acts like a Loop::Continue      
                     LglsAssumeUserWarn(false,
                        "Attempting to Loop::Discard while iterating constant container. "
                        "No discard will be performed."
                     );
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
   };
}
