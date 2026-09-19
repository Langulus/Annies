///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Hash-Emergent.hpp"


namespace Langulus::Annies::Component
{
   /// Refers back to this particular component instance through the deduced  
   /// 'this'. Just for convenience. It is #undef-ed at the end of this file. 
   #define ThisCom self.HashHeap<ID, H, SHARED...>

   ///                                                                        
   /// Stores a precomputed hash inside the heap with the given ID.           
   /// The hash is recomputed if GetHash() is invoked when stored hash is 0.  
   ///   @attention since hash is stored on the heap, it is recomputed as     
   ///      emergent when we have no ownership of that heap, because we will  
   ///      not be allowed to write that hash value down and cache it. This   
   ///      means that disowned containers will suffer a big performance cost 
   ///      every time they're hashed.                                        
   ///   @tparam ID the provider ID whose data will be hashed                 
   ///   @tparam H the hash type used                                         
   ///   @tparam SHARED additional provider IDs that are hashed together.     
   ///      They will all share the same cached hash variable.                
   template<Cid ID, class H, Cid...SHARED>
   struct HashHeap : HashEmergent<ID, H, SHARED...> {
      using HeapRequest = H;
      using Id = typename HashEmergent<ID, H, SHARED...>::Id;

      template<Cid SID>
      static constexpr bool Relevant = Id::template Contains<SID>;

      /// MARK: Public                                                        
      /// Reset the hash. It will be recomputed on next comparison.           
      template<Cid SID = ID> requires Relevant<SID>
      void ResetHash(this auto& self) assumptious {
         ThisCom::SetHashInner(self.template IsEmpty<SID>() ? 1 : 0);
      }

      /// Get the hash, recompute it if uninitialized or of we don't own it.  
      template<Cid SID = ID> requires Relevant<SID>
      H GetHash(this auto const& self) assumptious {
         if (self.template IsEmpty<SID>())
            return H {1};

         if (self.IsDisowned())
            return ThisCom::HashRecompute();

         if (self.template GetUses<SID>() == 0)
            return ThisCom::HashRecompute();

         const auto heap = self.template AccessHeap<HashHeap, SID>();
         LglsAssumeDevAndOptimize(heap, "Invalid heap");
         if (not *heap)
            const_cast<H&>(*heap) = ThisCom::HashRecompute();
         return *heap;
      }

   protected:
      /// MARK: Protected                                                     
      LglsComHeapMovable(friend);
      LglsComConversion(friend);

      /// Get hash (inner) - will never recompute it                          
      template<Cid SID = ID> requires Relevant<SID>
      constexpr auto GetHashInner(this auto&& self) noexcept -> H const {
         if (self.template IsEmpty<SID>())
            return H {1};

         if (self.IsDisowned())
            return H {0};
         if (self.template GetUses<SID>() == 0)
            return H {0};

         const auto heap = self.template AccessHeap<HashHeap, SID>();
         return heap ? *heap : H {0};
      }
      
      /// Set the hash (inner)                                                
      ///   @attention will not work for disowned containers                  
      template<Cid SID = ID> requires Relevant<SID>
      constexpr void SetHashInner(this auto& self, H h) assumptious {
         if (self.template IsEmpty<SID>())
            return;

         if (self.IsDisowned())
            return;
         if (self.template GetUses<SID>() == 0)
            return;

         const auto heap = self.template AccessHeap<HashHeap, SID>();
         LglsAssumeDev(heap, "Invalid heap");
         LglsAssumeDev(self.template GetUses<SID>() == 1);
         const_cast<H&>(*heap) = h;
      }
   };

   #undef ThisCom
}
