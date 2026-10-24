///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Indexed-Common.hpp"


namespace Langulus::Annies::Component
{
   ///                                                                        
   /// Provides bidirectional element access by tracking bitwise coverage.    
   /// Keeps a local pointer to the coverage table for faster and more        
   /// cache-friendly access.                                                 
   ///   @tparam ID the provider we're indexing                               
   ///   @tparam SHARED providers that share the same indexing scheme         
   template<Cid ID, Cid...SHARED>
   struct IndexedCoverageStack : IndexedCommon<ID, SHARED...> {
      /// Refers back to this particular component instance through the       
      /// deduced 'this'. Just for convenience. It is #undef-ed at the end of 
      /// this file.                                                          
      #define ThisCom self.IndexedCoverageStack<ID, SHARED...>

      using Base             = IndexedCommon<ID, SHARED...>;
      using TableType        = uint32_t;
      static constexpr size_t BitsPerCell = sizeof(TableType)*8;
      using HeapRequest      = PerElement<TableType, BitsPerCell>;
      using StackRequest     = TableType*;
      using IteratorCategory = ::std::bidirectional_iterator_tag;
      using Id               = typename Base::Id;

      static constexpr IndexingStyle Indexed = IndexingStyle::IndexedCoverage;
      template<Cid SID>
      static constexpr bool Relevant = Id::template Contains<SID>;

      /// Get the start of the coverage table                                 
      /*template<Cid SID = ID> requires Relevant<SID>
      constexpr auto GetCoverageTable(this auto const& self) noexcept -> TableType const* {
         return ThisCom::GetCoverageTableInner();
      }

      /// Get the end of the coverage table                                   
      template<Cid SID = ID> requires Relevant<SID>
      constexpr auto GetCoverageTableEnd(this auto const& self) noexcept -> TableType const* {
         return ThisCom::GetCoverageTableInner() + self.template GetReserved<SID>();
      }*/

   protected:
      LglsComHeapMovable(friend);
      LglsComIndexedCommon(friend);
      LglsComIndexedCommonHashed(friend);
      LglsComMerging(friend);
      LglsComRemoval(friend);

      /// Get coverage table (inner)                                          
      template<Cid SID = ID> requires Relevant<SID>
      constexpr auto& GetCoverageTableInner(this auto&& self) noexcept {
         return self.template AccessStack<IndexedCoverageStack>();
      }
      
      /// Set the number of initialized elements                              
      template<Cid SID = ID> requires Relevant<SID>
      constexpr void SetCoverageTableInner(this auto& self, TableType const* c) noexcept {
         ThisCom::GetCoverageTableInner() = const_cast<TableType*>(c);
      }

      /// This method is called to erase the coverage table                   
      template<Cid SID = ID> requires Relevant<SID>
      constexpr void ResetCoverageTable(this auto& self) noexcept {
         memset(
            ThisCom::GetCoverageTableInner(), 0,
            self.template GetReserved<SID>() * sizeof(TableType)
         );
      }

      /// Default-initialize hash table to zero                               
      constexpr void ConstructDefault(this auto& self) noexcept {
         ThisCom::SetCoverageTableInner(nullptr);
      }

      /// This method is called upon allocation to nullify table              
      constexpr void ConstructHeapRequestGlobal(this auto& self) noexcept {
         ThisCom::SetCoverageTableInner(self.template AccessHeap<IndexedCoverageStack, ID>());
         ThisCom::ResetCoverageTable();
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
         
            ThisCom::SetCoverageTableInner(from.template GetCoverageTable<ID>());
            
            if constexpr (I::ResetsOnMove())
               if_available(from.template SetCoverageTableInner<ID>(nullptr));
         }
      }
      
      /// MARK: BrowseTable                                                   
      /// Browse table, converting contiguous index into table index and mask.
      ///                                                                     
      /// Table is indexed the following way. The 5 filled elements:          
      /// 0-16:  [X][ ][X][X][ ][ ][ ][ ]       [ ][ ][ ][X][X][ ][ ][ ]      
      /// Correspond to coverage bitmask:                                     
      ///    [0]: 1  0  1  1  0  0  0  0    [1]: 0  0  0  1  1  0  0  0       
      /// So indices are:                                                     
      ///         0     1  2                              3  4                
      ///   @param index the index to access with                             
      ///   @param mask [out] mask showing the exact bit of the table         
      ///   @return offset into the coverage table                            
      template<CT::Container C>
      constexpr size_t BrowseTable(this C const& self, size_t index, TableType& mask) assumptious {
         LglsAssumeDev(self.GetReserved(), "Invalid reserve");
         LglsAssumeDev(self.GetCoverageTableInner(), "Invalid coverage table");

         const auto reserved = self.GetReserved() / BitsPerCell;
         auto const tableBeg = self.GetCoverageTableInner();
         auto const tableEnd = tableBeg + reserved;
         auto table = tableBeg;
         while (table < tableEnd) {
            auto enabled_bits = ::std::popcount(*table);
            if (enabled_bits) {
               if (index < BitsPerCell) {
                  if (enabled_bits == 1) {
                     LglsAssumeDev(index == 0);
                     mask = *table;
                  }
                  else {
                     TableType temp = *table;
                     while (index > 0) {
                        temp &= ~::std::bit_floor(temp);
                        --index;
                     }
                     LglsAssumeDev(::std::has_single_bit(temp));
                     mask = temp;
                  }
                  return table - tableBeg;
               }
               index -= BitsPerCell;
            }
            ++table;
         }

         LglsError("Should not be reached");
         return 0;
      }

      /// MARK: SimplifyIndex                                                 
      /// Convert an index to an offset.                                      
      /// Special indices will be contextualized.                             
      ///   @param index the index to simplify                                
      ///   @param mask [out] mask showing the exact bit of the table         
      ///   @return a simple element offset into contiguous memory            
      template<CT::Container C, CT::Index INDEX>
      constexpr size_t SimplifyIndex(this C const& self, INDEX index, TableType& mask) {
         LglsAssumeDev(not self.IsEmpty(), "Container can't be empty");

         if constexpr      (::std::same_as<INDEX, Index::Inner::All>)
            static_assert(false, "Index::All can't be used here");
         else if constexpr (::std::same_as<INDEX, Index::Inner::Many>)
            static_assert(false, "Index::Many can't be used here");
         else if constexpr (::std::same_as<INDEX, Index::Inner::Single>)
            static_assert(false, "Index::Single can't be used here");
         else if constexpr (::std::same_as<INDEX, Index::Inner::None>)
            static_assert(false, "Index::None can't be used here");
         else if constexpr (::std::same_as<INDEX, Index::Inner::Mode>)
            static_assert(false, "Index::Mode can't be used here");
         else if constexpr (::std::same_as<INDEX, Index::Inner::Front>)
            return ThisCom::BrowseTable(0);
         else if constexpr (::std::same_as<INDEX, Index::Inner::Middle>)
            return ThisCom::BrowseTable(self.GetCount() / 2);
         else if constexpr (::std::same_as<INDEX, Index::Inner::Back>)
            return ThisCom::BrowseTable(self.GetCount());
         else if constexpr (::std::same_as<INDEX, Index::Inner::Biggest>)
            return self.GetIndexLargest();
         else if constexpr (::std::same_as<INDEX, Index::Inner::Smallest>)
            return self.GetIndexSmallest();
         else if constexpr (::std::same_as<INDEX, Index::Inner::Random>)
            return self.GetIndexRandom();
         else if constexpr (::std::same_as<INDEX, Index::Inner::First>)
            return ThisCom::BrowseTable(0, mask);
         else if constexpr (::std::same_as<INDEX, Index::Inner::Last>) {
            const auto c = self.GetCount();
            LglsAssert(c > 0, "Can't get last index");
            return ThisCom::BrowseTable(c - 1, mask);
         }
         else if constexpr (requires { index.index; }) {
            // If index is negative, wrap it around (if in range)       
            const auto c = self.GetCount();
            if (index.index < 0) {
               LglsAssert(c + index.index < c and c + index.index >= 0,
                  "Integer index out of range");
               return ThisCom::BrowseTable(c + index.index, mask);
            }

            LglsAssert(index.index < c,
               "Integer index out of range");
            return ThisCom::BrowseTable(index.index, mask);
         }
         else {
            static_assert(CT::Integer<INDEX>, "Unsupported index type");

            // Unsafe, works only on assumptions.                       
            // Using an integer index explicitly makes a statement,     
            // that you know what you're doing.                         
            LglsAssert(static_cast<size_t>(index) <= self.GetCount(),
               "Integer index out of range");

            if constexpr (CT::Signed<INDEX>) {
               LglsAssumeUser(index >= 0,
                  "Integer index is below zero, "
                  "use Index::At for reverse indices instead"
               );
            }
            return ThisCom::BrowseTable(index, mask);
         }
      }
   };

   #undef ThisCom
}
