///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Container.hpp"
#include <Langulus/CT/Signed.hpp>
#include <Langulus/CT/Integer.hpp>
#include <Langulus/CT/NullTerminated.hpp>


namespace Langulus::Annies::Component
{
   ///                                                                        
   ///   A compile-time reserve                                               
   ///                                                                        
   ///   Reserve shows how many elements inside a container are allocated.    
   ///   Compile-time reserving isn't really reserving, and doesn't take up   
   /// space, but is useful for defining single-element containers that       
   /// still need the API required to function alongside other components.    
   ///   In these cases, reserve is equal to SIZE if container has a valid    
   /// GetAllocationInner - otherwise it is 0. If a non-emergent allocation   
   /// pointer doesn't exist, then the reserve is always SIZE.                
   ///   @tparam SIZE the reserve type and value                              
   ///   @tparam ID provider ID to keep reserve of                            
   ///   @tparam SHARED provider IDs that share the same reserve variable     
   ///   @attention when container is CT::NullTerminated, the reserved count  
   ///      will always report one element less, as it is reserved for zero.  
   template<auto SIZE, Cid ID, Cid...SHARED>
   struct ReserveStatic {
      using CTTI_Component = Yup;
      using CTTI_ReflectAs = void;
      using ReserveType    = decltype(SIZE);
      using Id             = Values<ID, SHARED...>;

      static constexpr int  ComponentPrecedence = -1000;

      static_assert(SIZE > 0,
         "Can't have a container of zero or negative capacity");
      static_assert(CT::Integer<ReserveType> and not CT::Signed<ReserveType>,
         "Reserve type must be an unsigned integer");
   
      /// Get the number of reserved (maybe uninitialized) elements           
      template<Cid SID = ID, CT::Container C>
      constexpr auto GetReserved(this C const& self) noexcept -> ReserveType {
         static_assert(not CT::NullTerminated<C> or SIZE > 1,
            "Container with static capacity of 1 can't be null-terminated");

         if constexpr (requires { self.template GetAllocationInner<SID>(); })
            return self.template GetAllocationInner<SID>() ? SIZE - CT::NullTerminated<C> : 0;
         else
            return SIZE - CT::NullTerminated<C>;
      }
   };
}
