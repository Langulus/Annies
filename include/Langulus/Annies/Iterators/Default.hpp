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
#include "Langulus/Typenav.hpp"
#include <Langulus/IntentOf.hpp>
#include <Langulus/CT/Index.hpp>
#include <ranges>


namespace Langulus::Annies
{
   ///                                                                        
   ///   Default iteration                                                    
   ///                                                                        
   /// Used by default when doing `for(auto i : container)`.                  
   /// When container is type-erased, or mutable and sparse, 'i' will be a    
   /// handle. Otherwise, 'i' will be a direct reference to the element.      
   template<bool REVERSE, class C>
   struct IterateDefault {
      using CTTI_ReflectAs = void;

      static_assert(CT::NoIntent<C>,
         "C can't have an intent");
      static_assert(CT::NotReference<C>,
         "C can't be a reference");
      static_assert(CT::ContainsMany<C>,
         "C is not iteratable because it contains exactly one element");
      static_assert(CT::Indexed<C>,
         "C is not indexed");

   protected:
      using Pick = DecidePick<C>;
      using H    = Tif<CT::Reference<Pick>, Deref<Pick>*, Pick>;

      static_assert(CT::NotReference<H>,
         "Iterator can't be a reference");
      static_assert(CT::Handle<H> or CT::Sparse<H>,
         "Must be either a pointer, or a handle");

      C& range;

      struct NoHashtable {};
      struct AddHashtable {
         using table_type = decltype(Fake<C>().GetHashTable());
         mutable table_type mTable;
      };

   public:
      explicit constexpr IterateDefault(C& a) noexcept
         : range {a} {}

      /// MARK: Iterator                                                      
      struct Iterator : Tif<CT::IndexedLinearly<C>, NoHashtable, AddHashtable> {
         using CTTI_ReflectAs    = void;
         using CTTI_Iterator     = Yup;
         using difference_type   = std::ptrdiff_t;
         using iterator_category = typename C::IteratorCategory;
         using value_type        = Deptr<H>;
         using reference         = Deptr<H>&;

         mutable H mIt;
         C* mRange;

         constexpr Iterator() noexcept = default;
         constexpr Iterator(Iterator const&) noexcept = default;
         constexpr Iterator(Iterator&&) noexcept = default;

         constexpr Iterator(Deptr<H> const& it, C* range) noexcept requires CT::Sparse<H>
            : mIt    {&it}
            , mRange {range} {
            if constexpr (not CT::IndexedLinearly<C>) {
               AddHashtable::mTable = range->GetHashTable();

               if (AddHashtable::mTable) {
                  AddHashtable::mTable += DecvqAllCast(mIt) - DecvqAllCast(range->GetRaw());

                  LglsAssumeDev(*AddHashtable::mTable,
                     "Iterators to hash table initialized with an invalid element");
               }
            }
         }

         constexpr Iterator(H const& it, C* range) noexcept
            : mIt    {it}
            , mRange {range} {
            if constexpr (not CT::IndexedLinearly<C>) {
               AddHashtable::mTable = range->GetHashTable();

               if (AddHashtable::mTable) {
                  if constexpr (CT::Handle<H>)
                     AddHashtable::mTable += mIt - range->GetHandle();
                  else
                     AddHashtable::mTable += DecvqAllCast(mIt) - DecvqAllCast(range->GetRaw());

                  LglsAssumeDev(*AddHashtable::mTable,
                     "Iterators to hash table initialized with an invalid element");
               }
            }
         }

         constexpr Iterator(H&& it, C* range) noexcept
            : mIt    {LglsFwd(it)}
            , mRange {range} {
            if constexpr (not CT::IndexedLinearly<C>) {
               AddHashtable::mTable = range->GetHashTable();

               if (AddHashtable::mTable) {
                  if constexpr (CT::Handle<H>)
                     AddHashtable::mTable += mIt - range->GetHandle();
                  else
                     AddHashtable::mTable += DecvqAllCast(mIt) - DecvqAllCast(range->GetRaw());

                  LglsAssumeDev(*AddHashtable::mTable,
                     "Iterators to hash table initialized with an invalid element");
               }
            }
         }

         constexpr auto operator = (Iterator const& rhs) assumptious -> Iterator& {
            LglsAssumeUser(mRange == rhs.mRange,
               "Iterators are for different containers");
            mIt = rhs.mIt;
            if constexpr (not CT::IndexedLinearly<C>)
               AddHashtable::mTable = rhs.mTable;
            return *this;
         }

         constexpr auto operator = (Iterator&& rhs) assumptious -> Iterator& {
            LglsAssumeUser(mRange == rhs.mRange,
               "Iterators are for different containers");
            mIt = rhs.mIt;
            if constexpr (not CT::IndexedLinearly<C>)
               AddHashtable::mTable = rhs.mTable;
            return *this;
         }

         constexpr bool operator == (CT::Iterator auto const& rhs) const noexcept {
            // We sneakily skip invalid table entries while comparing   
            if constexpr (CT::IndexedTable<C>) {
               if constexpr (REVERSE) {
                  if constexpr (CT::Handle<H>) {
                     while (rhs.mIt.GetRaw() < mIt.GetRaw() and not *AddHashtable::mTable) {
                        --AddHashtable::mTable;
                        --mIt;
                     }
                  }
                  else {
                     while (rhs.mIt < mIt and not *AddHashtable::mTable) {
                        --AddHashtable::mTable;
                        --mIt;
                     }
                  }
               }
               else {
                  if constexpr (CT::Handle<H>) {
                     while (rhs.mIt.GetRaw() > mIt.GetRaw() and not *AddHashtable::mTable) {
                        ++AddHashtable::mTable;
                        ++mIt;
                     }
                  }
                  else {
                     while (rhs.mIt > mIt and not *AddHashtable::mTable) {
                        ++AddHashtable::mTable;
                        ++mIt;
                     }
                  }
               }
            }

            if constexpr (CT::Handle<H>)
               return mIt.GetRaw() == rhs.mIt.GetRaw();
            else
               return mIt == rhs.mIt;
         }

         constexpr auto operator <=> (CT::Iterator auto const& rhs) const noexcept {
            // We sneakily skip invalid table entries while comparing   
            if constexpr (CT::IndexedTable<C>) {
               if constexpr (REVERSE) {
                  if constexpr (CT::Handle<H>) {
                     while (rhs.mIt.GetRaw() < mIt.GetRaw() and not *AddHashtable::mTable) {
                        --AddHashtable::mTable;
                        --mIt;
                     }
                  }
                  else {
                     while (rhs.mIt < mIt and not *AddHashtable::mTable) {
                        --AddHashtable::mTable;
                        --mIt;
                     }
                  }
               }
               else {
                  if constexpr (CT::Handle<H>) {
                     while (rhs.mIt.GetRaw() > mIt.GetRaw() and not *AddHashtable::mTable) {
                        ++AddHashtable::mTable;
                        ++mIt;
                     }
                  }
                  else {
                     while (rhs.mIt > mIt and not *AddHashtable::mTable) {
                        ++AddHashtable::mTable;
                        ++mIt;
                     }
                  }
               }
            }

            if constexpr (CT::Handle<H>)
               return mIt.GetRaw() <=> rhs.mIt.GetRaw();
            else
               return mIt <=> rhs.mIt;
         }
         
         explicit constexpr operator bool() const noexcept {
            if constexpr (CT::Handle<H>)
               return mIt.GetRaw() != mRange->GetRawEnd();
            else
               return mIt != mRange->GetRawEnd();
         }

         decltype(auto) operator * () noexcept {
            if constexpr (CT::Handle<H>) return (mIt);
            else                         return *mIt;
         }

         decltype(auto) operator * () const noexcept {
            if constexpr (CT::Handle<H>) return (mIt);
            else                         return *mIt;
         }

         decltype(auto) operator -> () noexcept {
            if constexpr (CT::Handle<H>) return &mIt;
            else                         return mIt;
         }

         decltype(auto) operator -> () const noexcept {
            if constexpr (CT::Handle<H>) return &mIt;
            else                         return mIt;
         }

         auto operator + (difference_type c) const assumptious -> Iterator {
            if (c == 0)
               return *this;

            if constexpr (CT::NotHandle<H>) {
               LglsAssumeDevAndOptimize(mIt, "Can't offset invalid iterator");
            };

            if constexpr (REVERSE)
               c *= -1;

            if constexpr (CT::IndexedLinearly<C>)
               return {mIt + c, mRange};
            else {
               auto next = AddHashtable::mTable;
               LglsAssumeDevAndOptimize(next, "Can't offset invalid iterator");

               if (c > 0) {
                  while (c) {
                     ++next;
                     if (*next)
                        --c;
                  }

                  return {mIt + (next - AddHashtable::mTable), mRange};
               }
               else {
                  while (c) {
                     --next;
                     if (*next)
                        ++c;
                  }

                  return {mIt - (AddHashtable::mTable - next), mRange};
               }
            }
         }

         friend auto operator + (difference_type lhs, Iterator const& rhs) assumptious -> Iterator {
            return rhs.operator + (lhs);
         }

         auto operator - (difference_type c) const assumptious -> Iterator {
            return operator + (-c);
         }

         auto operator += (difference_type c) assumptious -> Iterator& {
            if (c == 0)
               return *this;

            LglsAssumeDevAndOptimize(mIt, "Can't offset invalid iterator");
            if constexpr (REVERSE)
               c *= -1;

            if constexpr (CT::IndexedLinearly<C>)
               mIt += c;
            else {
               LglsAssumeDevAndOptimize(AddHashtable::mTable,
                  "Can't offset invalid iterator");
               if (c > 0) {
                  while (c) {
                     ++AddHashtable::mTable;
                     ++mIt;
                     if (*AddHashtable::mTable)
                        --c;
                  }
               }
               else {
                  while (c) {
                     --AddHashtable::mTable;
                     --mIt;
                     if (*AddHashtable::mTable)
                        ++c;
                  }
               }
            }
            return *this;
         }

         auto operator -= (difference_type c) assumptious -> Iterator& {
            return operator += (-c);
         }

         decltype(auto) operator[] (difference_type offset) const assumptious {
            if constexpr (CT::NotHandle<H>) {
               LglsAssumeDevAndOptimize(mIt, "Can't access invalid iterator");
            }

            if constexpr (REVERSE)
               offset *= -1;

            if constexpr (CT::IndexedLinearly<C>) {
               if constexpr (CT::Handle<H>)  return   mIt + offset;
               else                          return *(mIt + offset);
            }
            else {
               auto next = AddHashtable::mTable;
               LglsAssumeDevAndOptimize(next, "Can't access invalid iterator");

               if (offset > 0) {
                  while (offset) {
                     ++next;
                     if (*next)
                        --offset;
                  }

                  if constexpr (CT::Handle<H>)
                     return mIt + (next - AddHashtable::mTable);
                  else
                     return *(mIt + (next - AddHashtable::mTable));
               }
               else {
                  while (offset) {
                     --next;
                     if (*next)
                        ++offset;
                  }

                  if constexpr (CT::Handle<H>)
                     return mIt + (AddHashtable::mTable - next);
                  else
                     return *(mIt + (AddHashtable::mTable - next));
               }
            }
         }

         /// Prefix increment                                                 
         auto operator ++ () assumptious -> Iterator& {
            if constexpr (CT::NotHandle<H>) {
               LglsAssumeDevAndOptimize(mIt, "Can't increment invalid iterator");
            }

            if constexpr (CT::IndexedLinearly<C>) {
               if constexpr (REVERSE) --mIt;
               else                   ++mIt;
            }
            else {
               LglsAssumeDevAndOptimize(AddHashtable::mTable,
                  "Can't increment invalid iterator");

               if constexpr (REVERSE) {
                  /*IF_SAFE(const auto end = mRange->GetHashTable() - 1);
                  do {*/
                     --AddHashtable::mTable;
                     --mIt;

                     /*LglsAssumeDev(AddHashtable::mTable > end,
                        "Iterator went beyond limits");
                  }
                  while (not *AddHashtable::mTable);*/
               }
               else {
                  /*IF_SAFE(const auto end = mRange->GetHashTableEnd());
                  do {*/
                     ++AddHashtable::mTable;
                     ++mIt;

                     /*LglsAssumeDev(AddHashtable::mTable < end,
                        "Iterator went beyond limits");
                  }
                  while (not *AddHashtable::mTable);*/
               }
            }
            return *this;
         }

         /// Suffix increment                                                 
         auto operator ++ (int) assumptious -> Iterator {
            if constexpr (CT::NotHandle<H>) {
               LglsAssumeDevAndOptimize(mIt, "Can't increment invalid iterator");
            }

            if constexpr (CT::IndexedLinearly<C>) {
               if constexpr (REVERSE) return {mIt--, mRange};
               else                   return {mIt++, mRange};
            }
            else {
               LglsAssumeDevAndOptimize(AddHashtable::mTable,
                  "Can't increment invalid iterator");

               if constexpr (REVERSE) {
                  IF_SAFE(const auto end = mRange->GetHashTable() - 1);
                  auto next = AddHashtable::mTable;
                  do {
                     --next;
                     LglsAssumeDev(next > end,
                        "Iterator went beyond limits");
                  }
                  while (not *next);
                  return {mIt - (AddHashtable::mTable - next), mRange};
               }
               else {
                  IF_SAFE(const auto end = mRange->GetHashTableEnd());
                  auto next = AddHashtable::mTable;
                  do {
                     ++next;
                     LglsAssumeDev(next < end,
                        "Iterator went beyond limits");
                  }
                  while (not *next);
                  return {mIt + (next - AddHashtable::mTable), mRange};
               }
            }
         }

         /// Prefix decrement                                                 
         auto operator -- () assumptious -> Iterator& {
            if constexpr (CT::NotHandle<H>) {
               LglsAssumeDevAndOptimize(mIt, "Can't decrement invalid iterator");
            }

            if constexpr (CT::IndexedLinearly<C>) {
               if constexpr (REVERSE) ++mIt;
               else                   --mIt;
            }
            else {
               LglsAssumeDevAndOptimize(AddHashtable::mTable,
                  "Can't decrement invalid iterator");

               if constexpr (REVERSE) {
                  /*IF_SAFE(const auto end = mRange->GetHashTableEnd());
                  do {*/
                     ++AddHashtable::mTable;
                     ++mIt;

                     /*LglsAssumeDev(AddHashtable::mTable < end,
                        "Iterator went beyond limits");
                  }
                  while (not *AddHashtable::mTable);*/
               }
               else {
                  /*IF_SAFE(const auto end = mRange->GetHashTable() - 1);
                  do {*/
                     --AddHashtable::mTable;
                     --mIt;

                     /*LglsAssumeDev(AddHashtable::mTable > end,
                        "Iterator went beyond limits");
                  }
                  while (not *AddHashtable::mTable);*/
               }
            }
            return *this;
         }

         /// Suffix decrement                                                 
         auto operator -- (int) assumptious -> Iterator {
            if constexpr (CT::NotHandle<H>) {
               LglsAssumeDevAndOptimize(mIt, "Can't decrement invalid iterator");
            }

            if constexpr (CT::IndexedLinearly<C>) {
               if constexpr (REVERSE) return {mIt++, mRange};
               else                   return {mIt--, mRange};
            }
            else {
               LglsAssumeDevAndOptimize(AddHashtable::mTable,
                  "Can't decrement invalid iterator");

               if constexpr (REVERSE) {
                  IF_SAFE(const auto end = mRange->GetHashTableEnd());
                  auto next = AddHashtable::mTable;
                  do {
                     ++next;
                     LglsAssumeDev(next < end,
                        "Iterator went beyond limits");
                  }
                  while (not *next);
                  return {mIt + (next - AddHashtable::mTable), mRange};
               }
               else {
                  IF_SAFE(const auto end = mRange->GetHashTable() - 1);
                  auto next = AddHashtable::mTable;
                  do {
                     --next;
                     LglsAssumeDev(next > end,
                        "Iterator went beyond limits");
                  }
                  while (not *next);
                  return {mIt - (AddHashtable::mTable - next), mRange};
               }
            }
         }

         /// Get the integer element difference between two iterators         
         ///   @attention the result might look odd if C is not linear        
         auto operator - (CT::Iterator auto const& rhs) const assumptious
         -> difference_type {
            LglsAssumeUser(mRange == rhs.mRange,
               "Iterators are for different containers");

            if constexpr (CT::Handle<H>) {
               if constexpr (CT::TypeErased<C>) {
                  const auto range = mIt.template GetRawAs<uint8_t>() - rhs.mIt.template GetRawAs<uint8_t>();
                  return static_cast<difference_type>(range / mRange->GetStride());
               }
               else {
                  const auto range = mIt.GetRaw() - rhs.mIt.GetRaw();
                  return static_cast<difference_type>(range);
               }
            }
            else return static_cast<difference_type>(mIt - rhs.mIt);
         }
      };

      /// MARK: begin()                                                       
      constexpr auto begin() noexcept -> Iterator {
         if (range.IsEmpty())
            return {{}, &range};

         if constexpr (REVERSE)
            return {range.template AsAt<H>(Index::Last), &range};
         else
            return {range.template AsAt<H>(Index::First), &range};
      }

      /// MARK: end()                                                         
      constexpr auto end() noexcept -> Iterator {
         if (range.IsEmpty())
            return {{}, &range};

         if constexpr (REVERSE) {
            Iterator temp {range.template AsAt<H>(Index::First), &range};
            --temp.mIt;
            if constexpr (CT::IndexedTable<C>)
               --temp.mTable;
            return temp;
         }
         else {
            Iterator temp {range.template AsAt<H>(Index::Last), &range};
            ++temp.mIt;
            if constexpr (CT::IndexedTable<C>)
               ++temp.mTable;
            return temp;
         }
      }

      /// MARK: rbegin()                                                      
      constexpr auto rbegin() noexcept -> Iterator {
         if (range.IsEmpty())
            return {{}, &range};

         if constexpr (REVERSE)
            return {range.template AsAt<H>(Index::First), &range};
         else
            return {range.template AsAt<H>(Index::Last), &range};
      }

      /// MARK: rend()                                                        
      constexpr auto rend() noexcept -> Iterator {
         if (range.IsEmpty())
            return {{}, &range};

         if constexpr (not REVERSE) {
            Iterator temp {range.template AsAt<H>(Index::First), &range};
            --temp.mIt;
            if constexpr (CT::IndexedTable<C>)
               --temp.mTable;
            return temp;
         }
         else {
            Iterator temp {range.template AsAt<H>(Index::Last), &range};
            ++temp.mIt;
            if constexpr (CT::IndexedTable<C>)
               ++temp.mTable;
            return temp;
         }
      }
   };

   template<class C>
   IterateDefault(C&) -> IterateDefault<false, C>;
}