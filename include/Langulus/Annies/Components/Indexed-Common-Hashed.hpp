///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Component.hpp"
#include "Indexed-Common.hpp"
#include "Langulus/IntentOf.hpp"
#include "Langulus/Typenav.hpp"
#include <Langulus/HashOf.hpp>


namespace Langulus::Annies::Component
{
   /// Refers back to this particular component instance through the deduced  
   /// 'this'. Just for convenience. It is #undef-ed at the end of this file. 
   #define ThisCom self.IndexedCommonHashed<ID, HASH, SHARED...>

   ///                                                                        
   /// Provides a common hashed-table based access & insertion interface.     
   ///   @tparam ID the provider we're indexing                               
   ///   @tparam HASH type of the hash                                        
   ///   @tparam SHARED providers that share the same indexing scheme         
   template<Cid ID, class HASH, Cid...SHARED>
   struct IndexedCommonHashed : IndexedCommon<ID, SHARED...> {
      using TableType        = uint8_t;
      using IteratorCategory = ::std::random_access_iterator_tag;
      using Id               = typename IndexedCommon<ID, SHARED...>::Id;

      static constexpr IndexingStyle Indexed = IndexingStyle::IndexedTable;
      static constexpr bool Shared = sizeof...(SHARED) > 0;
      template<Cid SID>
      static constexpr bool Relevant = Id::template Contains<SID>;

   protected:
      template<CT::Container C>
      using Count = typename Deref<C>::CountType;

      //template<CT::Container C>
      //static constexpr auto CountMax = ::std::numeric_limits<Count<C>>::max();

      LglsComHeapMovable(friend);
      LglsComIndexedCommon(friend);
      LglsComMerging(friend);

      /// MARK: BrowseTable                                                   
      /// Browse table, converting contiguous index into table index.         
      ///                                                                     
      /// Table is indexed the following way:                                 
      /// 0-8:  [ ][ ][ ][ ][ ][ ][ ][ ]                                      
      /// 9-24: [ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ]              
      /// 25-56:[ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ]...  
      /// etc..:[ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ]...  
      ///                                                                     
      ///   It's a so called cascading table structure, designed this way     
      /// to minimize movement and avoid rehashing when table is resized.     
      ///   When an element is sought in this cascading table structure, it is
      /// sought first in the smallest (first) table, and if not found, the   
      /// next (bigger) tables are searched using the appropriate hash part.  
      ///   When inserted, elements are inserted to the cascade level that is 
      /// guaranteed to not be fully occupied yet, and then other attempts    
      /// are made to the rest of the cascades. The map strives to fill the   
      /// lower tables first. Some fragmentation might occur when items       
      /// are removed, but this shouldn't cause any harm. Worst case is, the  
      /// map would perform as bad as a conventional one. :)                  
      ///   Some ideas were extracted from here:                              
      ///      https://arxiv.org/pdf/2501.02305                               
      template<CT::Container C>
      constexpr auto BrowseTable(this C const& self, Count<C> index) assumptious -> Count<C> {
         LglsAssumeDev(self.GetReserved(),       "Invalid reserve");
         LglsAssumeDev(self.GetHashTableInner(), "Invalid hash table");

         const auto reserved = self.GetReserved();
         auto const tableBeg = self.GetHashTableInner();
         auto const tableEnd = tableBeg + reserved;
         auto table = tableBeg;
         while (table < tableEnd) {
            if (*table) {
               if (index == 0)
                  return table - tableBeg;
               --index;
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
      ///   @return a simple element offset into contiguous memory            
      template<CT::Container C, CT::Index INDEX>
      constexpr auto SimplifyIndex(this C const& self, INDEX index) -> Count<C> {
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
            return ThisCom::BrowseTable(0);
         else if constexpr (::std::same_as<INDEX, Index::Inner::Last>) {
            const auto c = self.GetCount();
            LglsAssert(c > 0, "Can't get last index");
            return ThisCom::BrowseTable(c - 1);
         }
         else if constexpr (requires { index.index; }) {
            // If index is negative, wrap it around (if in range)       
            const auto c = self.GetCount();
            if (index.index < 0) {
               LglsAssert(c + index.index < c and c + index.index >= 0,
                  "Integer index out of range");
               return ThisCom::BrowseTable(c + index.index);
            }

            LglsAssert(index.index < c,
               "Integer index out of range");
            return ThisCom::BrowseTable(index.index);
         }
         else {
            static_assert(CT::Integer<INDEX>, "Unsupported index type");

            // Unsafe, works only on assumptions.                       
            // Using an integer index explicitly makes a statement,     
            // that you know what you're doing.                         
            LglsAssert(static_cast<Count<C>>(index) <= self.GetCount(),
               "Integer index out of range");

            if constexpr (CT::Signed<INDEX>) {
               LglsAssumeUser(index >= 0,
                  "Integer index is below zero, "
                  "use Index::At for reverse indices instead"
               );
            }
            return ThisCom::BrowseTable(index);
         }
      }

      /// Get the offset, based on the provided value's hash. The offset is   
      /// truncated down to the size of the biggest cascading table.          
      /// You have to disable the most significant bit for each other table.  
      ///   @param value - the value to hash                                  
      ///   @return the bucket index                                          
      /*template<CT::Container C, CT::NoIntent T>
      auto GetOffset(this C const& self, T const& value) noexcept {
         const auto mask = ::std::bit_floor(self.GetReserved()) - 1u;
         return HashOf(value).value & mask;
      }*/

      /// Rehashes and reinserts each element, optimizing the table           
      ///   @param oldReserve - the old table size                            
      ///   @attention assumes reserve > oldReserve                           
      /*template<CT::Container C>
      void Rehash(this C& self, const Count<C> oldReserve) {
         LglsAssumeDev(self.GetReserved() > oldReserve,
            "New reserve is not larger than oldReserve");

         auto& count = self.GetCountInner();
         auto handle = self.GetHandle();
         const auto tableBeg = self.GetHashTableInner();
         const auto tableEnd = tableBeg + oldReserve;

         // First run: move elements closer to their new buckets        
         auto table = tableBeg;
         while (table != tableEnd) {
            if (*table) {
               // Rehash and check if hashes match                      
               Count<C> const oldIndex = table - tableBeg;
               Count<C> oldBucket = (oldReserve + oldIndex) - *table + 1;
               Count<C> newBucket = self.GetOffset(handle);

               if (oldBucket < oldReserve or oldBucket - oldReserve != newBucket) {
                  // Move it only if it won't end up in same bucket     
                  if constexpr (CT::TypeErased<C>) {
                     // Move the element to a temporary swapper first   
                     Any swapper {Piecewise, Abandon(handle)};
                     // Destroy the old element                         
                     handle.FreeInner();
                     *table = 0;
                     --count;
                     // Reinsert at the new offset                      
                     self.TableInsert(newBucket, swapper.GetHandle());
                  }
                  else {
                     // Move the element to a temporary swapper first   
                     THandle<Decvq<Deref<TypeOf<C>>>> swapper {Abandon(handle)};
                     // Destroy the old element                         
                     handle.FreeInner();
                     *table = 0;
                     --count;
                     // Reinsert at the new offset                      
                     self.TableInsert(newBucket, swapper);
                  }
               }
            }

            ++handle;
            ++table;
         }

         // Second run: shift elements left whereever possible to fill  
         // any gaps produced by the first run.                         
         self.ShiftEntries();
      }*/
   
      /// MARK: ShiftEntries                                                  
      /// Shift elements left whereever possible                              
      ///   @attention works in all dimensions simultaneously!                
      template<Cid SID = ID, CT::Container C> requires Relevant<SID>
      void ShiftEntries(this C& self) {
         const auto reserved = self.template GetReserved<SID>();
         int moves_performed;
         do {
            moves_performed = 0;
            const auto tableBeg = self.GetHashTableInner();
            const auto tableEnd = tableBeg + reserved;

            auto table = tableBeg;
            while (table != tableEnd) {
               if (*table > 1) {
                  // Entry can be moved *table - 1 cells to the left    
                  const Count<C> oldIndex = table - tableBeg;
                  Count<C> newIndex = reserved + oldIndex - *table + 1;
                  if (newIndex >= reserved)
                     newIndex -= reserved;   // Might loop around       

                  TableType attempt = 1;
                  while (tableBeg[newIndex] and attempt < *table) {
                     // Might loop around                               
                     ++newIndex;
                     if (newIndex >= reserved)
                        newIndex -= reserved;
                     ++attempt;
                  }

                  if (not tableBeg[newIndex] and attempt < *table) {
                     // Empty spot found, so move element there         
                     auto handle = self.GetHandle().ForceMutable();
                     auto from   = handle + oldIndex;
                     auto to     = handle + newIndex;
                     Id::ForEach([&to,&from]<Cid D>{
                        to.template EmplaceWithIntent<D>(Abandon(from));
                     });

                     from.template Free<false>();

                     tableBeg[newIndex] = attempt;
                     *table = 0;
                     ++moves_performed;
                  }
               }

               ++table;
            }
         } while (moves_performed);
      }

      /// MARK: TableEmplace                                                  
      /// Table insertion function - picks a strategy of insertion and goes   
      /// through all cascading tables until a free spot is found.            
      ///   @param item item to insert                                        
      ///   @attention assumes item doesn't yet exist in the table            
      ///   @attention assumes there's at least one free spot, in any one of  
      ///      the cascades                                                   
      ///   @attention works in all dimensions simultaneously                 
      ///   @attention assumes that reserved count is the same across all     
      ///      relevant dimensions                                            
      ///   @attention assumes that the same hash table is used across all    
      ///      relevant dimensions                                            
      ///   @return the absolute offset at which item was inserted            
      template<Cid SID = ID, class C, CT::Intent H> requires Relevant<SID>
      size_t TableEmplace(this C& self, H&& item) {
         static_assert(not CT::Array<H>);

         // Get the item's hash.                                        
         // This hash will be truncated and used for bucketing in each  
         // cascading table.                                            
         Hash hash;
         if constexpr (not Shared)
            hash = HashOf(DeintCast(item));
         else
            hash = HashOf(DeintCast(item).GetKeyHandle()); //TODO this presumes the key dimension is the one the hash table is associated with

         // Pick insertion strategy                                     
         size_t occupied = self.GetCount();
         size_t reserved_table = C::InitialSize;
         size_t reserved_total = C::InitialSize;
         const size_t absoluteReserved = self.GetReserved();
         TableType* const absoluteTableBeg = self.template GetHashTableInner<SID>();
         TableType* tableBeg = absoluteTableBeg;

         // While one or multiple tables are filled to the brim         
         // start with a table that is more likely to not be full       
         // yet, and then search in both directions from there.         
         while (occupied > reserved_total) {
            tableBeg += reserved_table;
            reserved_table *= C::GrowthFactor;
            reserved_total += reserved_table;
         }


         //                                                             
         // We try lazily inserting first, avoiding moving any elements 
         size_t mask = ::std::bit_floor(reserved_table) - 1u;
         auto inserted_at = ThisCom::TableEmplaceLazy(hash.value & mask, tableBeg, reserved_table, LglsFwd(item));
         if (inserted_at != reserved_table)
            return (tableBeg + inserted_at) - absoluteTableBeg;
      
         // First attempt failed, try other tables                      
         auto reserved_table_loop = reserved_table;
         auto reserved_total_loop = reserved_total;
         auto tableBeg_loop = tableBeg;
         auto mask_loop = mask;
         while (reserved_total_loop < absoluteReserved) {
            // We can go right first (most likely to be empty)          
            tableBeg_loop += reserved_table_loop;
            reserved_table_loop *= C::GrowthFactor;
            reserved_total_loop += reserved_table_loop;
            mask_loop <<= 1u; mask_loop += 1u;

            inserted_at = ThisCom::TableEmplaceLazy(hash.value & mask_loop, tableBeg_loop, reserved_table_loop, LglsFwd(item));
            if (inserted_at != reserved_table_loop)
               return (tableBeg_loop + inserted_at) - absoluteTableBeg;
         }

         reserved_table_loop = reserved_table;
         reserved_total_loop = reserved_total;
         tableBeg_loop = tableBeg;
         mask_loop = mask;
         while (reserved_total_loop > C::InitialSize) {
            // We can go left                                           
            reserved_total_loop -= reserved_table_loop;
            reserved_table_loop /= C::GrowthFactor;
            tableBeg_loop -= reserved_table_loop;
            mask_loop >>= 1u;

            inserted_at = ThisCom::TableEmplaceLazy(hash.value & mask_loop, tableBeg_loop, reserved_table_loop, LglsFwd(item));
            if (inserted_at != reserved_table_loop)
               return (tableBeg_loop + inserted_at) - absoluteTableBeg;
         }


         //                                                             
         // If this is reached, we need to be more insistent            
         reserved_table_loop = reserved_table;
         reserved_total_loop = reserved_total;
         tableBeg_loop = tableBeg;
         mask_loop = mask;

         // Start with the biggest table, because it will be less       
         // likely an element will have to be moved.                    
         while (reserved_total_loop < absoluteReserved) {
            tableBeg_loop += reserved_table_loop;
            reserved_table_loop *= C::GrowthFactor;
            reserved_total_loop += reserved_table_loop;
            mask_loop <<= 1u; mask_loop += 1u;
         }

         // Then go left.                                               
         while (reserved_total_loop >= C::InitialSize) {
            for (auto i = tableBeg_loop; i < tableBeg_loop + reserved_table_loop; ++i) {
               if (not *i) {
                  // Must guarantee at least one free spot              
                  inserted_at = ThisCom::TableEmplaceForce(hash.value & mask_loop, tableBeg_loop, reserved_table_loop, LglsFwd(item));
                  LglsAssumeDev(inserted_at != reserved_table_loop, "Shouldn't happen");
                  return (tableBeg_loop + inserted_at) - absoluteTableBeg;
               }
            }

            reserved_total_loop -= reserved_table_loop;
            reserved_table_loop /= C::GrowthFactor;
            tableBeg_loop -= reserved_table_loop;
            mask_loop >>= 1u;
         }

         LglsAssumeDev(false, "Shouldn't happen");
         return absoluteReserved;
      }

      /// MARK: TableEmplaceLazy                                              
      /// Table insertion function for e specific cascade level.              
      /// Doesn't move anything around, as it only seeks an empty spot that   
      /// can be easily filled.                                               
      ///   @attention works in all dimensions simultaneously!                
      ///   @return the offset at which item was inserted, relative to the    
      ///      current tableBeg. Returns 'reserved' if unable to insert here  
      //TODO maybe also collect data about where would be best to do the TableEmplaceForce on the way
      template<Cid SID = ID, CT::Intent H> requires Relevant<SID>
      size_t TableEmplaceLazy(
         this auto& self,
         size_t const start,
         TableType* const tableBeg,
         size_t const reserved,
         H&& item
      ) {
         // Get the starting index based on the key hash                
         auto table = tableBeg + start;
         if (not *table) {
            // Optimal path - the first bucket spot is already empty    
            auto absolute_idx = table - self.template GetHashTableInner<SID>();
            auto handle = self.GetHandle().ForceMutable() + absolute_idx;
            Id::ForEach([&handle,&item]<Cid D>{
               handle.template EmplaceWithIntent<D>(LglsFwd(item));
            });
            
            *table = 1;
            return start;
         }
      
         // Container is not empty and we need to browse for empty spot 
         /// @attention empty spot is _NOT_ guaranteed!                 
         const auto tableEnd = tableBeg + reserved - 1;
         ++table;
         TableType attempts = 2;
         while (table <= tableEnd and *table) {
            if (attempts > *table) {
               // Another chain detected, just abort. This is the       
               // lazy table emplacement, and there will be other       
               // attempts after it.                                    
               return reserved;
            }

            ++attempts;
            ++table;
         }

         if (table > tableEnd)
            return reserved;         // No empty slot was found         

         // If reached, then empty slot found, so put the value there   
         auto absolute_idx = table - self.template GetHashTableInner<SID>();
         auto handle = self.GetHandle().ForceMutable() + absolute_idx;
         Id::ForEach([&handle,&item]<Cid D>{
            handle.template EmplaceWithIntent<D>(LglsFwd(item));
         });
         
         *table = attempts;
         return table - tableBeg;
      }

      /// MARK: TableEmplaceForce                                             
      /// Table insertion function for e specific cascade level. It will      
      /// insert at all cost, moving elements around if it has to.            
      ///   @attention assumes there's at least one free spot guaranteed!     
      ///   @attention works in all dimensions simultaneously!                
      ///   @return the offset at which item was inserted, relative to the    
      ///      current tableBeg. Returns 'reserved' if unable to insert here  
      template<Cid SID = ID, CT::Intent H> requires Relevant<SID>
      size_t TableEmplaceForce(
         this auto& self,
         size_t const start,
         TableType* const tableBeg,
         size_t const reserved,
         H&& item
      ) {
         // Get the starting index based on the key hash                
         auto table = tableBeg + start;
         if (not *table) {
            // No swapping will happen                                  
            auto absolute_idx = table - self.template GetHashTableInner<SID>();
            auto handle = self.GetHandle().ForceMutable() + absolute_idx;
            Id::ForEach([&handle,&item]<Cid D>{
               handle.template EmplaceWithIntent<D>(LglsFwd(item));
            });
            
            *table = 1;
            return start;
         }

         // Container is not empty and swapping will occur              
         const auto tableEnd = tableBeg + reserved;
         auto handle = self.GetHandle().ForceMutable()
                     + (tableBeg - self.template GetHashTableInner<SID>());
         auto swapper = self.CreateSwapper(LglsFwd(item));
         auto swapper_handle = swapper.GetHandle();
         TableType attempts = 1;
         auto insertedAt = reserved;
         while (*table) {
            if (attempts > *table) {
               // We're inserting closer to bucket, so swap.            
               // Suppose this initial state is given:                  
               // ...[1][2][3][2][3][0]...                              
               //     ^        ^                                        
               //     |->2->3->4th attempt                              
               //     |        |                                        
               //     item wants to go here, but can't                  
               //              |                                        
               //          so it arrives at [2] on its 4th attempt.     
               //          4 > 2, so swap the element at the 4th attempt
               //          with our new 'item'. from that point on,     
               //          'swapper' contains the old item, and we have 
               //          to insert it in the next spot.               
               //                                                       
               // The new state looks like:                             
               // ...[1][2][3][4][3][0]...                              
               //              ^     ^                                  
               //              |->3->4th attempt for 'swapper'          
               //              |                                        
               //              original 2nd attempt for the 'swapper'   
               //                                                       
               // Since the 4th attempt for the 'swapper' is empty [0]  
               // the swapping ceases after emplacing the contents of   
               // the swapper, and we're done moving things around.     
               //                                                       
               // The final state looks like:                           
               // ...[1][2][3][4][3][4]...                              
               //              ^---->^                                  
               //              |     |                                  
               //  new goes here     old after being displaced by new   
               const auto index = table - tableBeg;
               auto h = handle + index;
               Id::ForEach([&h,&swapper_handle]<Cid D>{
                  h.template SwapInner<D>(swapper_handle);
               });

               ::std::swap(attempts, *table);
               if (insertedAt == reserved)
                  insertedAt = index;
            }

            ++attempts;

            // Wrap around and start from the beginning if we have to   
            if (table < tableEnd - 1) ++table;
            else table = tableBeg;
         }

         // If reached, then empty slot found, so put the value there   
         const auto index = table - tableBeg;
         handle += index;
         Id::ForEach([&handle,&swapper_handle]<Cid D>{
            handle.template EmplaceWithIntent<D>(Abandon(swapper_handle));
         });

         if (insertedAt == reserved)
            insertedAt = index;

         *table = attempts;
         return insertedAt;
      }

      /// MARK: TableSearch                                                   
      /// Locate element handle inside the cascading hash table               
      ///   @attention assumes container is not empty                         
      ///   @attention assumes that container is of the same comparable type  
      ///   @attention operates on a single dimension at a time               
      ///   @param item the item to search for                                
      ///   @return handle of the found item                                  
      template<Cid SID = ID, class C, CT::NoIntent H> requires Relevant<SID>
      auto TableSearch(this C const& self, H const& item) assumptious -> DecideHandle<C> {
         static_assert(not CT::Array<H>);

         // Get the item's hash.                                        
         // This hash will be truncated and used for bucketing in each  
         // cascading table.                                            
         Hash hash;

         // We start at the smallest table                              
         size_t reserved_table = C::InitialSize;
         auto   tableBeg = self.template GetHashTableInner<SID>();
         size_t mask = ::std::bit_floor(reserved_table) - 1u;

         // Decide the comparison function for type-erased tables       
         RTTI::DefinitionData::FCompareEqual comparer = nullptr;
         if constexpr (CT::Handle<H>) {
            if constexpr (H::Dimensions::Count == 1)
               hash = HashOf(item);
            else
               hash = HashOf(item.GetKeyHandle()); //TODO this presumes the key dimension is the one the hash table is associated with

            if constexpr (CT::TypeErased<C> or CT::TypeErased<H>) {
               const auto type = self.template GetType<SID>();
               LglsAssumeDev(type.IsSame(item.template GetType<SID>()),
                  "Type mismatch");
               comparer = type.GetComparerEqual();
               LglsAssumeDev(comparer, "Type-erased data not comparable");
            }
            else {
               static_assert(CT::Comparable<TypeOf<C, SID>, TypeOf<H, SID>>,
                  "Type not comparable");
            }
         }
         else {
            hash = HashOf(item);

            if constexpr (CT::TypeErased<C>) {
               const auto type = self.template GetType<SID>();
               LglsAssumeDev(type.IsSame(MetaDataOf<H>()),
                  "Type mismatch");
               comparer = type.GetComparerEqual();
               LglsAssumeDev(comparer, "Type-erased data not comparable");
            }
            else {
               static_assert(CT::Comparable<TypeOf<C, SID>, H>,
                  "Type not comparable");
            }
         }

         // First attempt                                               
         auto found = ThisCom::TableSearchInner(hash.value & mask, tableBeg, reserved_table, item, comparer);
         if (found)
            return found;
      
         // First attempt failed, try other tables                      
         size_t reserved_total = C::InitialSize;
         const size_t max_reserved = self.GetReserved();
         while (reserved_total < max_reserved) {
            tableBeg += reserved_table;
            reserved_table *= C::GrowthFactor;
            reserved_total += reserved_table;
            mask <<= 1u; mask += 1u;

            found = ThisCom::TableSearchInner(hash.value & mask, tableBeg, reserved_table, item, comparer);
            if (found)
               return found;
         }

         // Nothing found if reached                                    
         return {};
      }
      
      /// MARK: TableSearchInner                                              
      /// Table insertion function for e specific cascade level.              
      /// Doesn't move anything around, as it only seeks an empty spot that   
      /// can be easily filled.                                               
      ///   @attention works in all dimensions simultaneously!                
      ///   @return the offset at which item was inserted                     
      template<Cid SID = ID, class C, CT::NoIntent H> requires Relevant<SID>
      auto TableSearchInner(
         this C const& self,
         size_t const start,
         TableType const* const tableBeg,
         const size_t reserved,
         H const& item,
         [[maybe_unused]] RTTI::DefinitionData::FCompareEqual comparer
      ) -> DecideHandle<C> {
         auto table = tableBeg + start;
         const auto tableEnd = tableBeg + reserved - 1;
         TableType attempts = 1;
         while (*table) {
            if (attempts > *table) {
               // Another bucket chain detected, no point in doing      
               // any more comparisons                                  
               return {};
            }

            // Test value                                               
            auto test = self.GetHandle() + (table - self.template GetHashTableInner<SID>());
            if constexpr (CT::Handle<H>) {
               if constexpr (CT::TypeErased<C> or CT::TypeErased<H>) {
                  if (comparer(test.template GetRaw<SID>(), item.template GetRaw<SID>()))
                     return test;
               }
               else {
                  if (*test.template GetRaw<SID>() == *item.template GetRaw<SID>())
                     return test;
               }
            }
            else {
               if constexpr (CT::TypeErased<C>) {
                  if (comparer(test.template GetRaw<SID>(), &item))
                     return test;
               }
               else {
                  if (*test.template GetRaw<SID>() == item)
                     return test;
               }
            }
            
            ++attempts;

            // Wrap around and start from the beginning if we have to   
            if (table < tableEnd) ++table;
            else table = tableBeg;
         }

         // If reached, then item wasn't found                          
         return {};
      }
   };

   #undef ThisCom
}
