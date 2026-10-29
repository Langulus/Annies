///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Container.hpp"
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
   ///   Used by default when doing `for(auto i : container)`.                
   /// When container is type-erased, or mutable and sparse, 'i' will be a    
   /// handle. Otherwise, 'i' will be a direct reference to the element.      
   template<bool REVERSE, class C>
   struct IterateHiveDefault {
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

   public:
      explicit constexpr IterateHiveDefault(C& a) noexcept
         : range {a} {}

      /// MARK: Iterator                                                      
      struct Iterator {
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

         bool CheckSlot() {
            auto slot = DecvqAllCast(mIt) - DecvqAllCast(mRange->GetRaw());
            return mRange->IsSlotOccupied(slot);
         }

         constexpr Iterator(Deptr<H> const& it, C* range) noexcept requires CT::Sparse<H>
            : mIt    {&it}
            , mRange {range} {
            LglsAssumeDev(CheckSlot(),
               "Iterators to hive initialized with an invalid element");
         }

         constexpr Iterator(H const& it, C* range) noexcept
            : mIt    {it}
            , mRange {range} {
            LglsAssumeDev(CheckSlot(),
               "Iterators to hive initialized with an invalid element");
         }

         constexpr Iterator(H&& it, C* range) noexcept
            : mIt    {LglsFwd(it)}
            , mRange {range} {
            LglsAssumeDev(CheckSlot(),
               "Iterators to hive initialized with an invalid element");
         }

         constexpr auto operator = (Iterator const& rhs) assumptious -> Iterator& {
            LglsAssumeUser(mRange == rhs.mRange,
               "Iterators are for different containers");
            mIt = rhs.mIt;
            return *this;
         }

         constexpr auto operator = (Iterator&& rhs) assumptious -> Iterator& {
            LglsAssumeUser(mRange == rhs.mRange,
               "Iterators are for different containers");
            mIt = rhs.mIt;
            return *this;
         }

         constexpr bool operator == (CT::Iterator auto const& rhs) const noexcept {
            // We sneakily skip invalid table entries while comparing   
            if constexpr (CT::IndexedHashTable<C>) {
               if constexpr (REVERSE) {
                  if constexpr (CT::Handle<H>) {
                     while (rhs.mIt.GetRaw() < mIt.GetRaw() and not CheckSlot())//TODO these operators can be added to handles and this branch is pointless
                        --mIt;
                  }
                  else {
                     while (rhs.mIt < mIt and not CheckSlot())
                        --mIt;
                  }
               }
               else {
                  if constexpr (CT::Handle<H>) {
                     while (rhs.mIt.GetRaw() > mIt.GetRaw() and not CheckSlot())//TODO these operators can be added to handles and this branch is pointless
                        ++mIt;
                  }
                  else {
                     while (rhs.mIt > mIt and not CheckSlot())
                        ++mIt;
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
            if constexpr (CT::IndexedHashTable<C>) {
               if constexpr (REVERSE) {
                  if constexpr (CT::Handle<H>) {
                     while (rhs.mIt.GetRaw() < mIt.GetRaw() and not CheckSlot())
                        --mIt;
                  }
                  else {
                     while (rhs.mIt < mIt and not CheckSlot())
                        --mIt;
                  }
               }
               else {
                  if constexpr (CT::Handle<H>) {
                     while (rhs.mIt.GetRaw() > mIt.GetRaw() and not CheckSlot())
                        ++mIt;
                  }
                  else {
                     while (rhs.mIt > mIt and not CheckSlot())
                        ++mIt;
                  }
               }
            }

            if constexpr (CT::Handle<H>)
               return mIt.GetRaw() <=> rhs.mIt.GetRaw();//TODO these operators can be added to handles and this branch is pointless
            else
               return mIt <=> rhs.mIt;
         }
         
         explicit constexpr operator bool() const noexcept {
            if constexpr (CT::Handle<H>)
               return mIt.GetRaw() != mRange->GetRawEnd();//TODO these operators can be added to handles and this branch is pointless
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

            LglsAssume(mIt, "Can't offset invalid iterator");

            if constexpr (REVERSE)
               c *= -1;

            size_t next = DecvqAllCast(mIt) - DecvqAllCast(mRange->GetRaw());
            if (c > 0) {
               while (c) {
                  ++next;
                  if (mRange->IsSlotOccupied(next))
                     --c;
               }

               return {mIt + next, mRange};
            }
            else {
               while (c) {
                  --next;
                  if (mRange->IsSlotOccupied(next))
                     ++c;
               }

               return {mIt - next, mRange};
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

            LglsAssume(mIt, "Can't offset invalid iterator");

            if constexpr (REVERSE)
               c *= -1;

            if (c > 0) {
               while (c) {
                  ++mIt;
                  if (CheckSlot())
                     --c;
               }
            }
            else {
               while (c) {
                  --mIt;
                  if (CheckSlot())
                     ++c;
               }
            }
            return *this;
         }

         auto operator -= (difference_type c) assumptious -> Iterator& {
            return operator += (-c);
         }

         decltype(auto) operator[] (difference_type offset) const assumptious {
            LglsAssume(mIt, "Can't offset invalid iterator");

            if constexpr (REVERSE)
               offset *= -1;

            size_t next = DecvqAllCast(mIt) - DecvqAllCast(mRange->GetRaw());
            if (offset > 0) {
               while (offset) {
                  ++next;
                  if (mRange->IsSlotOccupied(next))
                     --offset;
               }

               if constexpr (CT::Handle<H>)
                  return mIt + next;
               else
                  return *(mIt + next);
            }
            else {
               while (offset) {
                  --next;
                  if (mRange->IsSlotOccupied(next))
                     ++offset;
               }

               if constexpr (CT::Handle<H>)
                  return mIt + next;
               else
                  return *(mIt + next);
            }
         }

         /// Prefix increment                                                 
         auto operator ++ () assumptious -> Iterator& {
            LglsAssume(mIt, "Can't offset invalid iterator");
            if constexpr (REVERSE)
               --mIt;
            else
               ++mIt;
            return *this;
         }

         /// Suffix increment                                                 
         auto operator ++ (int) assumptious -> Iterator {
            LglsAssume(mIt, "Can't offset invalid iterator");
            IF_SAFE(const size_t end = mRange->GetReserved() - 1);
            size_t next = DecvqAllCast(mIt) - DecvqAllCast(mRange->GetRaw());

            if constexpr (REVERSE) {
               do {
                  --next;
                  LglsAssumeDev(next > end, "Iterator went beyond limits");
               }
               while (not mRange->IsSlotOccupied(next));
               return {mIt - next, mRange};
            }
            else {
               do {
                  ++next;
                  LglsAssumeDev(next < end, "Iterator went beyond limits");
               }
               while (not mRange->IsSlotOccupied(next));
               return {mIt + next, mRange};
            }
         }

         /// Prefix decrement                                                 
         auto operator -- () assumptious -> Iterator& {
            LglsAssume(mIt, "Can't offset invalid iterator");
            if constexpr (REVERSE)
               ++mIt;
            else
               --mIt;
            return *this;
         }

         /// Suffix decrement                                                 
         auto operator -- (int) assumptious -> Iterator {
            LglsAssume(mIt, "Can't offset invalid iterator");
            IF_SAFE(const size_t end = mRange->GetReserved() - 1);
            size_t next = DecvqAllCast(mIt) - DecvqAllCast(mRange->GetRaw());

            if constexpr (REVERSE) {
               do {
                  ++next;
                  LglsAssumeDev(next < end, "Iterator went beyond limits");
               }
               while (not mRange->IsSlotOccupied(next));
               return {mIt + next, mRange};
            }
            else {
               do {
                  --next;
                  LglsAssumeDev(next > end, "Iterator went beyond limits");
               }
               while (not mRange->IsSlotOccupied(next));
               return {mIt - next, mRange};
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
            return temp;
         }
         else {
            Iterator temp {range.template AsAt<H>(Index::Last), &range};
            ++temp.mIt;
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
            return temp;
         }
         else {
            Iterator temp {range.template AsAt<H>(Index::Last), &range};
            ++temp.mIt;
            return temp;
         }
      }
   };

   template<class C>
   IterateHiveDefault(C&) -> IterateHiveDefault<false, C>;
}