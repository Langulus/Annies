///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Container.hpp"


namespace Langulus::Annies::Component
{
   ///                                                                        
   /// A dynamic reserve derived from the allocation size directly.           
   /// As such, it will not increase container's stack size, but will require 
   /// an indirection (and a division) in order to read it.                   
   ///   @tparam T the reserve type                                           
   ///   @tparam ID provider ID to keep reserve of                            
   ///   @tparam SHARED provider IDs that share the same reserve variable     
   template<class T, Cid ID, Cid...SHARED>
   struct ReserveEmergent {
      using CTTI_Component = Yup;
      using CTTI_ReflectAs = void;
      using ReserveType = T;
      using Id = Values<ID, SHARED...>;

      static constexpr int ComponentPrecedence = -1000;
      //template<Cid SID>
      //static constexpr bool Relevant = Id::template Contains<SID>;

      static_assert(CT::Integer<T> and not CT::Signed<T>,
         "Reserve type must be an unsigned integer");

      /// Get the number of reserved (maybe uninitialized) elements           
      template<Cid SID = ID, CT::Container C>// requires Relevant<SID>
      constexpr T GetReserved(this const C& self) noexcept {
         if constexpr (requires { self.template GetAllocation<SID>(); }) {
            const auto al = self.template GetAllocation<SID>();
            if (not al)
               return 0;

            if constexpr (CT::ContainsOne<C>)
               return 1;
            else {
               static_assert(C::template CountHeapFooterRequests<SID>() == 0,
                  "ReserveEmergent can't be used in containers with heap footer, "
                  "because it causes a circular dependency - "
                  "reserved count becomes impossible to get without knowing the "
                  "reserved elements beforehand."
               );

               size_t header = C::template DefineHeapHeader<SID>();
               using PROVIDER = typename decltype(C::template FindProvider<SID>())::First;
               header = Align(header, self.template GetAlignment<PROVIDER::Id::First>());
               return (al->GetSize() - header) / self.template GetStride<SID>();
            }
         }
         else if constexpr (CT::ContainsOne<C>)
            return 1;
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
      template<Cid SID = ID, CT::ContainsMany C>// requires Relevant<SID>
      C& Reserve(this C& self, const T reserve) {
         if (reserve < self.template GetCount<SID>())
            self.template AllocateLess<SID>(reserve);
         else
            self.template AllocateMore<SID>(reserve);
         return self;
      }
   };
}
