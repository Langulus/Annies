///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Reserve-Emergent.hpp"
#include <Langulus/CT/NullTerminated.hpp>


namespace Langulus::Annies::Component
{
   ///                                                                        
   /// A dynamic reserve, stored as a member variable.                        
   /// Will increase container's stack size.                                  
   ///   @tparam T the reserve counter type                                   
   ///   @tparam ID provider ID to keep reserve of                            
   ///   @tparam SHARED provider IDs that share the same reserve variable     
   ///   @attention when container is CT::NullTerminated, the reserved count  
   ///      will always report one element less, as it is reserved for zero.  
   template<class T, Cid ID, Cid...SHARED>
   struct ReserveStack : ReserveEmergent<T, ID, SHARED...> {
      /// Refers back to this particular component instance through the       
      /// deduced 'this'. Just for convenience. It is #undef-ed at the end of 
      /// this file.                                                          
      #define ThisCom self.ReserveStack<T, ID, SHARED...>

      using StackRequest = T;
      using Id = typename ReserveEmergent<T, ID, SHARED...>::Id;

      /// Get the number of reserved (maybe uninitialized) elements           
      template<Cid SID = ID, class C>
      constexpr T GetReserved(this C const& self) noexcept {
         if constexpr (CT::NullTerminated<C>) {
            T result = ThisCom::GetReservedInner();
            return result ? result - 1 : 0;
         }
         else return ThisCom::GetReservedInner();
      }

      /// Reserve a number of elements without initializing them.             
      /// If reserved data is smaller than currently initialized count, the   
      /// excess elements will be dereferenced/destroyed.                     
      ///   @param reserve number of elements to reserve                      
      template<Cid SID = ID, CT::ContainsMany C>
      C& Reserve(this C& self, const T reserve) {
         if (reserve < self.template GetCount<SID>())
            self.template AllocateLess<SID>(reserve);
         else
            self.template AllocateMore<SID>(reserve);
         return self;
      }

   protected:
      LglsComHeapMovable(friend);
      LglsComRemoval(friend);
      LglsComEmplacement(friend);

      /// Get reserved (inner)                                                
      constexpr auto& GetReservedInner(this auto&& self) noexcept {
         return self.template AccessStack<ReserveStack>();
      }
      
      /// Set the number of reserved elements                                 
      template<Cid SID = ID>
      constexpr void SetReservedInner(this auto& self, T c) noexcept {
         ThisCom::GetReservedInner() = c;
      }
      
      /// Default-initialize reserve to zero                                  
      constexpr void ConstructDefault(this auto& self) noexcept {
         ThisCom::SetReservedInner(0);
      }
      
      /// Transfer from any kind of container, respecting intents             
      ///   @attention notice that Copy and Clone intents are not handled     
      ///      here. They're handled in heap components instead, in case      
      ///      something throws an exception while constructing. Same applies 
      ///      to source containers that are allocated on the stack.          
      ///   @param intent the intent and container to transfer from           
      //TODO what happens if we transfer from not CT::NullTerminated to CT::NullTerminated - we must anticipate a resize and do shallow copy instead of absorb, or we lose last byte!
      template<class SELF, CT::Intent I> requires CT::Container<I>
      void ConstructFrom(this SELF& self, I&& intent) {
         if constexpr (not CT::Copied<I> and not CT::Cloned<I> and CT::HeapAllocated<I>) {
            decltype(auto) from = LglsFwd(intent.what);
         
            // Always propagate custom reserve if available             
                 if_available(ThisCom::SetReservedInner(from.template GetReservedInner<ID>()))
            else if_available(ThisCom::SetReservedInner(from.template GetReserved<ID>())) 
            else {
               // Otherwise derive reserve from current heap pointer,   
               // the start of the allocation, and by element size.     
               // Note: Same as Com::ReserveEmergent::GetReserved.      
               if constexpr (requires { from.template GetAllocation<ID>(); }) {
                  const auto al = from.template GetAllocation<ID>();
                  if (not al) {
                     ThisCom::SetReservedInner(0);
                     return;
                  }

                  if constexpr (CT::ContainsOne<I>)
                     ThisCom::SetReservedInner(1);
                  else {
                     const size_t header = from.template GetHeapHeaderSize<ID>();
                     ThisCom::SetReservedInner((al->GetSize() - header) / self.template GetStride<ID>());
                  }
               }
               else {
                  static_assert(CT::ContainsOne<I>,
                     "Can't derive the amount of reserved items from source container, "
                     "because it has neither a reserve, nor ownership components"
                  );
                  ThisCom::SetReservedInner(1);
               }
            }

            if constexpr (I::ResetsOnMove())
               if_available(from.template SetReservedInner<ID>(0));
         }
      }
   };

   #undef ThisCom
}
