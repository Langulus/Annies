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
   #define ThisCom self.IndexedHashStack<ID, HASH, SHARED...>

   ///                                                                        
   /// Provides random element access by hashing a value of the provided ID.  
   /// Uses a modified Robin Hood algorithm to reuse table space and minimize 
   /// movement on rehash. Keeps a local pointer to the hash table for faster 
   /// and more cache-friendly access.                                        
   ///   @tparam ID the provider we're indexing                               
   ///   @tparam HASH type of the hash                                        
   ///   @tparam SHARED providers that share the same indexing scheme         
   template<Cid ID, class HASH, Cid...SHARED>
   struct IndexedHashStack : IndexedCommonHashed<ID, HASH, SHARED...> {
      using Base             = IndexedCommonHashed<ID, HASH, SHARED...>;
      using TableType        = typename Base::TableType;
      using HeapRequest      = PerElement<TableType>;
      using StackRequest     = TableType*;
      using IteratorCategory = typename Base::IteratorCategory;
      using Id               = typename Base::Id;

      template<Cid SID>
      static constexpr bool Relevant = Id::template Contains<SID>;

      /// Get the start of the hash table                                     
      template<Cid SID = ID> requires Relevant<SID>
      constexpr auto GetHashTable(this auto const& self) noexcept -> TableType const* {
         return ThisCom::GetHashTableInner();
      }

      /// Get the end of the hash table                                       
      template<Cid SID = ID> requires Relevant<SID>
      constexpr auto GetHashTableEnd(this auto const& self) noexcept -> TableType const* {
         return ThisCom::GetHashTableInner() + self.template GetReserved<SID>();
      }

   protected:
      LglsComHeapMovable(friend);
      LglsComIndexedCommon(friend);
      LglsComIndexedCommonHashed(friend);
      LglsComMerging(friend);
      LglsComRemoval(friend);

      /// Get hash table (inner)                                              
      template<Cid SID = ID> requires Relevant<SID>
      constexpr auto& GetHashTableInner(this auto&& self) noexcept {
         return self.template AccessStack<IndexedHashStack>();
      }
      
      /// Set the number of initialized elements                              
      template<Cid SID = ID> requires Relevant<SID>
      constexpr void SetHashTableInner(this auto& self, TableType const* c) noexcept {
         ThisCom::GetHashTableInner() = const_cast<TableType*>(c);
      }

      /// This method is called to erase the hash table                       
      template<Cid SID = ID> requires Relevant<SID>
      constexpr void ResetHashTable(this auto& self) noexcept {
         memset(
            ThisCom::GetHashTableInner(), 0,
            self.template GetReserved<SID>() * sizeof(TableType)
         );
      }

      /// Default-initialize hash table to zero                               
      constexpr void ConstructDefault(this auto& self) noexcept {
         ThisCom::SetHashTableInner(nullptr);
      }

      /// This method is called upon allocation to nullify table              
      constexpr void ConstructHeapRequestGlobal(this auto& self) noexcept {
         ThisCom::SetHashTableInner(self.template AccessHeap<IndexedHashStack, ID>());
         ThisCom::ResetHashTable();
      }

      /// Transfer from any kind of container, respecting intents             
      ///   @attention this is noop when constructing from deep intents,      
      ///      since element constructors might throw and stuff be partially  
      ///      inserted. In those cases, table is set by the heap components. 
      ///   @param intent the intent and container to transfer from           
      template<class SELF, CT::Intent I> requires CT::Container<I>
      void ConstructFrom(this SELF& self, I&& intent) {
         if constexpr (not CT::Copied<I> and not CT::Cloned<I> and CT::HeapAllocated<I>) {
            decltype(auto) from = LglsFwd(intent.what);
         
            ThisCom::SetHashTableInner(from.template GetHashTable<ID>());
            
            if constexpr (I::ResetsOnMove())
               if_available(from.template SetHashTableInner<ID>(nullptr));
         }
      }
   };

   #undef ThisCom
}
