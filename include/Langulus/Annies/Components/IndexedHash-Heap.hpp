///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Indexed-Common-Hashed.hpp"


namespace Langulus::Annies::Component
{
   /// Refers back to this particular component instance through the deduced  
   /// 'this'. Just for convenience. It is #undef-ed at the end of this file. 
   #define ThisCom self.IndexedHashHeap<ID, HASH, SHARED...>

   ///                                                                        
   /// Provides random element access by hashing a value of the provided ID.  
   /// Uses a modified Robin Hood algorithm to reuse table space and minimize 
   /// reallocations. Uses multiple cascading tables in order to minimze      
   /// moving things around when rehashing. Doesn't keep a local pointer to   
   /// the hash table, and instead recalculates it on demand from the heap.   
   ///   @tparam ID the provider we're indexing                               
   ///   @tparam HASH type of the hash                                        
   ///   @tparam SHARED providers that share the same indexing scheme         
   template<Cid ID, class HASH, Cid...SHARED>
   struct IndexedHashHeap : IndexedCommonHashed<ID, HASH, SHARED...> {
      using Base             = IndexedCommonHashed<ID, HASH, SHARED...>;
      using TableType        = typename Base::TableType;
      using HeapRequest      = PerElement<TableType>;
      using IteratorCategory = typename Base::IteratorCategory;
      using Id               = typename Base::Id;

      template<Cid SID>
      static constexpr bool Relevant = Id::template Contains<SID>;

      /// Get the start of the hash table                                     
      template<Cid SID = ID> requires Relevant<SID>
      constexpr auto GetHashTable(this auto const& self) noexcept -> TableType const* {
         if (self.template GetAllocationInner<SID>())
            return ThisCom::GetHashTableInner();
         return nullptr;
      }

      /// Get the end of the hash table                                       
      template<Cid SID = ID> requires Relevant<SID>
      constexpr auto GetHashTableEnd(this auto const& self) noexcept -> TableType const* {
         if (self.template GetAllocationInner<SID>())
            return ThisCom::GetHashTableInner() + self.template GetReserved<SID>();
         return nullptr;
      }

   protected:
      LglsComHeapMovable(friend);
      LglsComIndexedCommon(friend);
      LglsComIndexedCommonHashed(friend);
      LglsComMerging(friend);
      LglsComRemoval(friend);

      /// Get the start of the hash table (inner)                             
      template<Cid SID = ID> requires Relevant<SID>
      constexpr auto* GetHashTableInner(this auto&& self) noexcept {
         return self.template AccessHeap<IndexedHashHeap, SID>();
      }

      /// This method is called to erase the hash table                       
      template<Cid SID = ID> requires Relevant<SID>
      constexpr void ResetHashTable(this auto& self) noexcept {
         memset(
            ThisCom::GetHashTableInner(), 0,
            self.template GetReserved<SID>() * sizeof(TableType)
         );
      }

      /// This method is called upon allocation to nullify table              
      constexpr void ConstructHeapRequestGlobal(this auto& self) noexcept {
         ThisCom::ResetHashTable();
      }
   };

   #undef ThisCom
}
