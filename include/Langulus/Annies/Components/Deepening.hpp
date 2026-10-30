///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Iteration-ForEach.hpp"
#include <Langulus/CT/Deep.hpp>


namespace Langulus::Annies::Component
{
   ///                                                                        
   /// Adds deepening interface and utilities for other components.           
   ///   @tparam DEEP_TYPE the deepening type                                 
   template<class DEEP_TYPE>
   struct Deepening {
      /// Refers back to this particular component instance through the       
      /// deduced 'this'. Just for convenience. It is #undef-ed at the end of 
      /// this file.                                                          
      #define ThisCom self.Deepening<DEEP_TYPE>

      using CTTI_Component = Yup;
      using CTTI_ReflectAs = void;
      using CTTI_Deep      = Yup;

      using DeepType = DEEP_TYPE;

      static constexpr int  ComponentPrecedence = 3000;

      /// MARK: GetCountDeep                                                  
      /// Get the sum of initialized non-deep elements in all sub-blocks      
      ///   @param depth the depth at which to go (-1 for infinite depth)     
      ///   @attention depth == 0 acts as GetCount()                          
      ///   @return the number of contained non-deep elements                 
      template<class C>
      constexpr size_t GetCountDeep(this C const& self, size_t depth = -1) noexcept {
         if (self.IsEmpty())
            return 0;

         if (depth == 0)
            return self.GetCount();

         size_t accumulated_count = 0;
         if_constexpr_type_erased(C) {
            auto const type = self.GetType().GetOrigin();
            if (not type.IsDeep())
               return self.GetCount();

            size_t const inner_depth = depth == size_t(-1) ? depth : depth - 1;
            size_t const* const invoke_args[] {&inner_depth};
            auto method = type.GetMethod("GetCountDeep");
            LglsAssumeDev(method, "Deep container has no reflected GetCountDeep method");
            self.Apply([&accumulated_count,&method,&invoke_args](auto const& item) noexcept {
               size_t sub_count;
               method->invoke(item.GetDense().data, invoke_args, &sub_count);
               accumulated_count += sub_count;
            });
         }
         #if not LANGULUS(FORCE_TYPE_ERASURE)
         else {
            using T = TypeOf<C>;
            if constexpr (CT::Deep<T>) {
               for (auto const& item : self) {
                  accumulated_count += DenseCast(item)
                     .GetCountDeep(depth == size_t(-1) ? depth : depth - 1);
               }
            }
            else return self.GetCount();
         }
         #endif
         return accumulated_count;
      }
      
      /// MARK: Deepen                                                        
      /// Wrap all contained elements inside a sub-block                      
      ///   @return a reference to the newly created inner container          
      template<class C> requires (CT::TypeErased<C> or CT::Deep<TypeOf<C>>)
      auto Deepen(this C& self) -> C& {
         if constexpr (not CT::TypeErased<C>) {
            LglsAssert(not self.IsTypeConstrained() or self.IsDeep(),
               "Can't deepen with incompatible type");
         }
      
         // Allocate a new container and move this one inside of it     
         C temp {Piecewise, Abandon {self}};
         self.Swap(temp);
         return *self.template Get<C>();
      }
      
      /// Execute functions in each sub-block, inclusively.                   
      /// Each function has a distinct argument type, that is tested against  
      /// the contained type. If argument is compatible with the type, the    
      /// block is iterated, and F is executed for all elements. None of the  
      /// provided functions are ignored, unless Loop::Break is returned at   
      /// some point.                                                         
      ///   @param lambdas all potential functions to iterate with            
      ///   @return the number of executions                                  
      template<CT::Container C, class...F>
      auto ForEachDeep(this C&& self, F&&...lambdas) -> ForEachResult {
         static_assert(sizeof...(F) > 0, "No functions in ForEachDeep");
         if (self.IsEmpty())
            return {};

         ForEachResult result {0, Loop::Break};
         (void)(... or (Loop::Break == (
            result.control = self.template
               ForEachDeepInner<false, true>(-1, LglsFwd(lambdas), result.count)
         )));

         if constexpr (CT::Mutable<C>) {
            if (result.control == Loop::Discard)
               self.Reset();
         }
         return result;
      }

      /// Do it in reverse                                                    
      template<CT::Container C, class...F>
      auto ForEachDeepRev(this C&& self, F&&...lambdas) -> ForEachResult {
         static_assert(sizeof...(F) > 0, "No functions in ForEachDeepRev");
         if (self.IsEmpty())
            return {};

         ForEachResult result {0, Loop::Break};
         (void)(... or (Loop::Break == (
            result.control = self.template
               ForEachDeepInner<true, true>(-1, LglsFwd(lambdas), result.count)
         )));

         if constexpr (CT::Mutable<C>) {
            if (result.control == Loop::Discard)
               self.Reset();
         }
         return result;
      }

      /// Do it without skipping the intermediate containers                  
      template<CT::Container C, class...F>
      auto ForEachDeepNoskip(this C&& self, F&&...lambdas) -> ForEachResult {
         static_assert(sizeof...(F) > 0, "No functions in ForEachDeepNoskip");

         ForEachResult result {0, Loop::Break};
         (void)(... or (Loop::Break == (
            result.control = self.template
               ForEachDeepInner<false, false>(-1, LglsFwd(lambdas), result.count)
         )));

         if constexpr (CT::Mutable<C>) {
            if (result.control == Loop::Discard)
               self.Reset();
         }
         return result;
      }

      /// Do it without skipping the intermediate containers in reverse       
      template<CT::Container C, class...F>
      auto ForEachDeepNoskipRev(this C&& self, F&&...lambdas) -> ForEachResult {
         static_assert(sizeof...(F) > 0, "No functions in ForEachDeepNoskipRev");

         ForEachResult result {0, Loop::Break};
         (void)(... or (Loop::Break == (
            result.control = self.template
               ForEachDeepInner<true, false>(-1, LglsFwd(lambdas), result.count)
         )));

         if constexpr (CT::Mutable<C>) {
            if (result.control == Loop::Discard)
               self.Reset();
         }
         return result;
      }

   protected:
      /// Always execute for intermediate (or non-deep) self                  
      template<class R, class A, CT::Container C, class F>
      LoopControl ForEachDeepInnerLocal(this C&& self, F&& f, size_t& counter) noexcept_if(f) {
         ++counter;

         decltype(auto) argument = self.template ReinterpretAs<A>();
         if constexpr (CT::Bool<R>) {
            if (not f(argument))
               return Loop::Break;
         }
         else if constexpr (Exact<R, LoopControl>) {
            // Do things depending on the F's return                    
            R loop = f(argument);

            while (loop == Loop::Repeat)
               loop = f(argument);

            switch (loop.mControl) {
            case LoopControl::Break:
            case LoopControl::NextLoop:
               return loop;
            case LoopControl::Continue:
            case LoopControl::Repeat:
               break;
            case LoopControl::Discard:
               if constexpr (CT::Mutable<C>) {
                  // Discard is allowed only if C is mutable.           
                  // You can't fully discard the topmost block,         
                  // only reset it. Now, if we reset this               
                  // block, and then remove it up the chain, if         
                  // branching-out happens to occur, we'll end          
                  // up with a branch that contains the empty           
                  // element and that is bad. So defer the              
                  // reset up the chain instead!                        
                  return Loop::Discard;
               }
               else {
                  // ...otherwise it acts like a Loop::Continue         
                  LglsAssumeUserWarn(false,
                     "Attempting to Loop::Discard while iterating "
                     "constant container. "
                     "No discard will be performed."
                  );
                  break;
               }
            }
         }
         else f(argument);
         return Loop::Continue;
      }

      /// Iterate and execute call for each deep element, counting each       
      /// successfull execution.                                              
      ///   @tparam REVERSE whether to iterate in reverse                     
      ///   @tparam SKIP whether to execute call for intermediate blocks      
      ///   @param f the function to execute for each element of type A       
      ///   @param counter [out] counts the successful executions             
      ///   @return the last 'f' result                                       
      template<bool REVERSE, bool SKIP, CT::Container C, class F>
      LoopControl ForEachDeepInner(this C&& self, size_t depth, F&& f, size_t& counter) noexcept_if(f) {
         using A = ArgumentOf<F>;
         static_assert(CT::Slab<A> or CT::Mutable<C> or CT::ConstantEverywhere<A>,
            "Mutable reference/pointer iterator not allowed for constant container");
         using R = ReturnOf<F>;

         LoopControl loop = Loop::Continue;
         if_constexpr_type_erased(C) {
            const auto T = self.GetType();
            if constexpr (CT::Deep<A>) {
               if (not SKIP or not T.IsDeep() or depth == 0) {
                  LoopControl local_result = self.template ForEachDeepInnerLocal<R, A>(LglsFwd(f), counter);
                  if (local_result != Loop::Continue)
                     return local_result;
               }
            }

            if constexpr (not CT::Deep<A>) {
               // Equivalent to non-deep iteration                      
               loop = self.template ForEachInner<REVERSE>(LglsFwd(f), counter);
            }
            else if (T.IsDeep()) {
               if (depth == 0)
                  return loop;

               // Iterate subblocks                                     
               /*size_t const inner_depth = depth == size_t(-1) ? depth : depth - 1;
               RTTI::DMeta const arg_type = MetaDataOf<Decay<A>>();
               void const* const invoke_args[] {
                  &inner_depth, &arg_type
               };

               auto method = REVERSE
                  ? (SKIP ? T.GetOrigin().GetMethod("ForEachDeepRev")
                          : T.GetOrigin().GetMethod("ForEachDeepRevNoskip"))
                  : (SKIP ? T.GetOrigin().GetMethod("ForEachDeep")
                          : T.GetOrigin().GetMethod("ForEachDeepNoskip"));

               LglsAssumeDev(method,
                  "Deep container has no reflected deep iteration method");*/

               /*LoopControl local_loop = Loop::Continue;
               self.ApplyReverse([&method, &invoke_args](auto const& item) noexcept {
                  if constexpr (CT::Deep<A>) {
                     // Loop control is available only if iterator      
                     // is deep as well...                              
                     method->invoke(item.GetDense().data, invoke_args, &local_loop);
                     if (local_loop != Loop::Continue)
                        return false;
                  }
                  else {
                     // ... otherwise we have to just pass through all  
                     // deep sub-blocks                                 
                     group.template ForEachDeepInner<REVERSE, SKIP>(
                        depth - 1, LglsMov(f), counter
                     );
                  }
                  return true;
               });*/

               size_t intermediateCounterSink = 0;
               using SubBlock = LglsMutIf(C, Decay<A>&);

               loop = self.template ForEachInner<REVERSE>(
                  [&depth, &counter, &f](SubBlock group) {
                     //if constexpr (CT::Deep<SubBlock>) {
                        // Loop control is available only if iterator   
                        // is deep as well...                           
                        return group.template ForEachDeepInner<REVERSE, SKIP>(
                           depth - 1, LglsFwd(f), counter
                        );
                     /*}
                     else {
                        // ... otherwise we just pass through all       
                        // deep sub-blocks                              
                        group.template ForEachDeepInner<REVERSE, SKIP>(
                           depth - 1, LglsFwd(f), counter
                        );
                     }*/
                  },
                  intermediateCounterSink
               );
            }
         }
         #if not LANGULUS(FORCE_TYPE_ERASURE)
         else {   
            using T = TypeOf<C>;
            if constexpr (CT::Deep<A>) {
               if (not SKIP or not CT::Deep<T> or depth == 0) {
                  LoopControl local_result = self.template ForEachDeepInnerLocal<R, A>(LglsFwd(f), counter);
                  if (local_result != Loop::Continue)
                     return local_result;
               }
            }

            if constexpr (not CT::Deep<A>) {
               // Equivalent to non-deep iteration                      
               loop = self.template ForEachInner<REVERSE>(LglsFwd(f), counter);
            }
            else if constexpr (CT::Deep<T>) {
               if (depth == 0)
                  return loop;

               // Iterate subblocks                                     
               size_t intermediateCounterSink = 0;
               using SubBlock = LglsMutIf(C, Decay<T>&);

               loop = self.template ForEachInner<REVERSE>(
                  [&depth, &counter, &f](SubBlock group) {
                     return group.template ForEachDeepInner<REVERSE, SKIP>(
                        depth - 1, LglsFwd(f), counter
                     );
                  },
                  intermediateCounterSink
               );
            }
         }
         #endif
         return loop;
      }
   };

   #undef ThisCom
}
