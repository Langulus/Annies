///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Container.hpp"
#include <Langulus/CT/NullTerminated.hpp>


namespace Langulus::Annies::Component
{
   ///                                                                        
   /// A dynamic reserve derived from the allocation size directly.           
   /// As such, it will not increase container's stack size, but will require 
   /// an indirection (and a division) in order to read it.                   
   ///   @tparam T the reserve type                                           
   ///   @attention when container is CT::NullTerminated, the reserved count  
   ///      will always report one element less, as it is reserved for zero.  
   ///   @attention only one reserve component is allowed per container       
   template<class T>
   struct ReserveEmergent {
      using CTTI_Component = Yup;
      using CTTI_ReflectAs = void;
      using ReserveType    = T;

      static constexpr int ComponentPrecedence = -1000;

      static_assert(CT::Integer<T> and not CT::Signed<T>,
         "Reserve type must be an unsigned integer");

      /// Get the number of reserved (maybe uninitialized) elements           
      template<CT::Container C>
      constexpr size_t GetReserved(this const C& self) noexcept {
         if constexpr (requires { self.GetAllocation(); }) {
            const auto al = self.GetAllocation();
            if (not al)
               return 0;

            if constexpr (CT::ContainsOne<C>) {
               static_assert(not CT::NullTerminated<C>,
                  "Container with static capacity of 1 can't be null-terminated");
               return 1;
            }
            else {
               //TODO probably generalize this for multidimensionals?
               static_assert(C::template CountHeapFooterRequests<0>() == 0,
                  "ReserveEmergent can't be used in containers with heap footer, "
                  "because it causes a circular dependency - "
                  "reserved count becomes impossible to get without knowing the "
                  "reserved elements beforehand."
               );

               size_t header = C::template DefineHeapHeader<0>();
               using PROVIDER = typename decltype(C::template FindProvider<0>())::First;
               header = Align(header, self.template GetAlignment<PROVIDER::Id::First>());
               return (al->GetSize() - header) / self.GetStride() - CT::NullTerminated<C>;
            }
         }
         else if constexpr (CT::ContainsOne<C>) {
            static_assert(not CT::NullTerminated<C>,
               "Container with static capacity of 1 can't be null-terminated");
            return 1;
         }
         else {
            static_assert(false,
               "Emergent reserve can't derive the amount of reserved items, "
               "because container supporting multiple elements "
               "has no ownership component"
            );
         }
      }

      /// Reserve a number of elements without initializing them.             
      /// If reserved data is smaller than currently initialized count, the   
      /// excess elements will be dereferenced/destroyed.                     
      ///   @param reserve number of elements to reserve                      
      template<CT::ContainsMany C>
      C& Reserve(this C& self, const T reserve) {
         if (reserve < self.GetCount())
            self.AllocateLess(reserve);
         else
            self.AllocateMore(reserve);
         return self;
      }
   };
}
