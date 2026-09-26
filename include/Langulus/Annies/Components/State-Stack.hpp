///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Component.hpp"
#include "Langulus/Assume.hpp"
#include <Langulus/IntentOf.hpp>
#include <Langulus/Utils/Sequence.hpp>


namespace Langulus::Annies::Component
{
   ///                                                                        
   /// Adds a variable state to a container.                                  
   /// Increases the container's bytesize to the smallest possible integer    
   /// capable of containing all state bits.                                  
   /// The states will be gathered from StateRequests in other components.    
   /// There can only be one StateStack/StateHeap/StateStatic component in    
   /// a container.                                                           
   ///   @tparam STATES... the possible states                                
   template<CT::State...STATES>
   struct StateStack {
      using CTTI_Component = Yup;
      using CTTI_ReflectAs = void;

      static constexpr int    ComponentPrecedence = -8000;
      static constexpr size_t StateCount = sizeof...(STATES);
      static constexpr bool   HasStates = StateCount > 0;
      template<CT::State S>
      static constexpr bool   HasState = HasStates and AkinAsOneOf<S, STATES...>;

      static_assert(StateCount < 16, "Too many states");

      struct StateWrapper;
      using  StateList    = Types<STATES...>;
      using  StateType    = Tif<StateCount < 8, uint8_t, uint16_t>;
      using  StackRequest = Tif<HasStates, StateWrapper, void>;

      ///                                                                     
      /// The bitfield capable of containing all variable states              
      struct StateWrapper {
         StateType mState;

         template<CT::State S>
         constexpr StateWrapper& operator += (S) noexcept {
            mState |= StateStack::template GetStateBit<S>();
            return *this;
         }
         
         template<CT::State S>
         constexpr StateWrapper& operator -= (S) noexcept {
            mState &= ~StateStack::template GetStateBit<S>();
            return *this;
         }
         
         template<CT::State S>
         constexpr bool operator & (S) const noexcept {
            return mState & StateStack::template GetStateBit<S>();
         }
         
         template<CT::State S>
         constexpr bool operator == (S) const noexcept {
            return mState == (mState & StateStack::template GetStateBit<S>());
         }

         template<class S> requires requires (S s) { s.mState == 0; }
         constexpr bool operator == (S const& rhs) const noexcept {
            return mState == rhs.mState;
         }

         constexpr explicit operator bool() const noexcept {
            return mState != 0;
         }
      };

      /// MARK: Public                                                        
      /// Get the current state of the container                              
      constexpr int GetState(this auto const& self) noexcept requires HasStates {
         return ToAbsoluteState(self.GetStateInner());
      }

      /// Set the current state of the container. Allowed only while container
      /// is empty.                                                           
      constexpr void SetState(this auto& self, int state) requires HasStates {
         LglsAssert(self.GetAllocation() == nullptr,
            "Changing state of an allocated container is not permitted in this way. "
            "Use Enable/Disable methods one by one instead."
         );
         self.SetStateInner(ToInternalState(state));
         LglsAssert(state == 0,
            "There are leftover states after translating to internal representation. "
            "This indicates that state information was lost during translation. "
            "The involved containers are likely state-incompatible."
         );
      }

      /// Get the relevant state when relaying one container to another.      
      /// Relevant states exclude size and type constraints, as well as       
      /// tracking and disownment.                                            
      ///   @return the current unconstrained container state                 
      constexpr auto GetUnconstrainedState(this auto const& self) noexcept
      -> StateType requires HasStates {
         StateWrapper r = self.GetStateInner();
         ForEach(StateList{}, [&r]<class S>{
            if constexpr (S::UID == Annies::State::Typed
            or            S::UID == Annies::State::Tagged
            or            S::UID == Annies::State::Verbed
            or            S::UID == Annies::State::Tracked
            or            S::UID == Annies::State::Disowned)   r -= S {};
         });
         return ToAbsoluteState(r);
      }

   protected:
      /// Check if container supports any of the mentioned states             
      template<Annies::State...ID>
      static consteval bool CheckStateSupport() {
         return ForEachOr(StateList{}, []<class S> noexcept {
            return ((S::UID == ID and (S::Dynamic or S::Enable)) or ...);
         });
      }

      /// Convert the internal representation of the states into absolute one 
      static constexpr int ToAbsoluteState(StateWrapper r) noexcept {
         int accumulator = 0;
         ForEach(StateList{}, [&]<class S>{
            if (r & S{}) accumulator |= static_cast<int>(S::UID);
         });
         return accumulator;
      }

      /// Convert the absolute representation of the states into internal one 
      ///   @param r [in/out] the state to translate. Each transferred bit    
      ///      is removed, so that you can detect states that weren't         
      ///      supported - if 'r' is not 0 at the end could indicate problems.
      static constexpr auto ToInternalState(int& r) -> StateWrapper {
         StateWrapper accumulator {0};
         ForEach(StateList{}, [&]<class S>{
            if constexpr (S::Dynamic) {
               if (r & static_cast<int>(S::UID)) {
                  accumulator += S{};
                  r &= ~static_cast<int>(S::UID);
               }
            }
            else if constexpr (S::Enable) {
               LglsAssert(0 != (r & static_cast<int>(S::UID)),
                  "A state is statically enabled - you tried to disable it at runtime");
               r &= ~static_cast<int>(S::UID);
            }
            else {
               LglsAssert(0 == (r & static_cast<int>(S::UID)),
                  "A state is statically disabled - you tried to enable it at runtime");
            }
         });
         return accumulator;
      }

   public:
      static constexpr bool CanBeMissing  = CheckStateSupport<Annies::State::Past, Annies::State::Future>();
      static constexpr bool CanBeDisowned = CheckStateSupport<Annies::State::Disowned>();

      /// Check if container is marked as missing past/future                 
      ///   @return true if this container is marked as missing               
      constexpr bool IsMissing(this auto const& self) noexcept requires CanBeMissing { //TODO dimensions?
         bool r = false;
         ForEachConstOr(StateList{}, [&]<class S>{
            if constexpr (S::UID == Annies::State::Past or S::UID == Annies::State::Future) {
               if constexpr (S::Static) {
                  if constexpr (S::Enable) {
                     r = true;
                     return true;
                  }
                  else return No {};
               }
               else {
                  r |= self.GetStateInner() & S {};
                  return No {};
               }
            }
            else return No {};
         });
         return r;
      }

      /// Check if container is marked as disowned                            
      ///   @attention this is triggered mainly by Disown intent. A container 
      ///      can still be disowned, even if the state itself isn't enabled. 
      ///      When there's no allocation, for example.                       
      ///   @return true if this container is marked as disowned              
      template<class C>
      constexpr bool IsDisowned(this C const& self) noexcept { //TODO dimensions?
         if constexpr (CT::Handle<C>) {
            // Handles can't be disowned, they have exclusive rights    
            return false;
         }
         else {
            bool r = false;

            if constexpr (CanBeDisowned) {
               // Disown state component exists in the container        
               ForEachConstOr(StateList{}, [&]<class S>{
                  if constexpr (S::UID == Annies::State::Disowned) {
                     if constexpr (S::Static) {
                        if constexpr (S::Enable) {
                           r = true;
                           return true;
                        }
                        else return No{};
                     }
                     else {
                        r |= self.GetStateInner() & S {};
                        return No{};
                     }
                  }
                  else return No{};
               });
            }

            if constexpr (requires { self.GetAllocation(); }) {
               // We can consider full containers without owned memory  
               // to be disowned.                                       
               return r or (not self.IsEmpty() and self.GetAllocation() == nullptr);
            }
            else return r;
         }
      }

      /// Check if container has either created elements, or a relevant state 
      ///   @return true if either contains state, or has stuff inserted      
      constexpr bool IsValid(this auto const& self) noexcept { //TODO dimensions?
         if constexpr (HasStates)
            return not self.IsEmpty() or static_cast<bool>(self.GetUnconstrainedState());
         else
            return not self.IsEmpty();
      }

      /// Check if container is in the default state                          
      ///   @return true if either contains state, or has stuff inserted      
      constexpr bool IsDefaultState(this auto const& self) noexcept { //TODO dimensions?
         if constexpr (HasStates)
            return self.GetState() == GetDefaultState();
         else
            return true;
      }

   protected:
      /// MARK: Protected                                                     
      LglsComEmplacement(friend);
      LglsComRemoval(friend);

      LglsStateCompressed(friend);
      LglsStateEncrypted(friend);
      LglsStateFuture(friend);
      LglsStateOr(friend);
      LglsStatePast(friend);
      LglsStateSorted(friend);
      LglsStateTracked(friend);
      LglsStateTyped(friend);
      LglsStateDisowned(friend);

      /// Get the value of a specific state                                   
      template<CT::State B>
      static consteval StateType GetStateBit() requires HasStates {
         StateType i = 0;
         StateType accumulator = 0;
         ForEach(StateList{}, [&]<class S>{
            if constexpr (B::UID == S::UID)
               accumulator = (StateType {1} << i);
            ++i;
         });
         return accumulator;
      }

      /// Get the default set of state bits                                   
      static consteval auto GetDefaultState() -> StateWrapper requires HasStates  {
         StateType i = 0;
         StateType accumulator = 0;
         ForEach(StateList{}, [&]<class S>{
            if constexpr (S::Enable)
               accumulator |= (StateType {1} << i);
            ++i;
         });
         return {accumulator};
      }

      /// Clear the state to the default value                                
      constexpr void ResetState(this auto&& self) noexcept requires HasStates {
         self.SetStateInner(GetDefaultState());
      }

      /// Get the contained state (inner)                                     
      constexpr auto& GetStateInner(this auto&& self) noexcept requires HasStates {
         return self.template AccessStack<StateStack>();
      }

      /// Set the contained state (inner)                                     
      constexpr void SetStateInner(this auto& self, const StateWrapper& type) noexcept requires HasStates {
         self.GetStateInner() = type;
      }
      
      /// Default-initialize state                                            
      constexpr void ConstructDefault(this auto& self) noexcept requires HasStates {
         self.ResetState();
      }
      
      /// Transfer from any kind of container, respecting intents             
      ///   @param intent the intent and container to transfer from           
      template<class C, CT::Intent I> requires (HasStates and CT::Container<I>)
      void ConstructFrom(this C& self, I&& intent) {
         decltype(auto) from = LglsFwd(intent.what);
         
         if constexpr (requires { from.GetStateInner(); }) {
            self.SetState(from.GetState());

            // Don't propagate disowned state unless explicitly required
            if constexpr (CanBeDisowned and not CT::Disowned<I>)
               self.DisableDisowned();
         }
         else self.SetStateInner(GetDefaultState());
      }
   };
}
