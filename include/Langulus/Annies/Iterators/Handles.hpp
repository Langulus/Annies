///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Container.hpp"
#include <Langulus/IntentOf.hpp>
#include <ranges>


namespace Langulus::Annies
{
   ///                                                                        
   ///   Iterate using handles                                                
   ///                                                                        
   ///   When doing for(auto i : container), the statement always uses the    
   /// most optimal iteration approach, but often you want to be able to      
   /// modify values in-place while iterating.                                
   ///   Use like this:                                                       
   ///      `for(auto i : IterateHandles(container))`                         
   ///   where 'container' can be any CT::Container with multiple elements.   
   template<bool REVERSE, class C>
   struct IterateHandles {
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
      //using Count = typename Deref<C>::CountType;
      using H = DecideHandle<C>;

      static_assert(CT::NotReference<H>,
         "Iterator can't be a reference");
      static_assert(CT::Handle<H>,
         "Iterator must always be a handle");

      C& range;

   public:
      struct IteratorContiguous;
      struct IteratorDiscontiguous;

      explicit constexpr IterateHandles(C& a) noexcept : range{a} {}

      constexpr auto begin() const noexcept {
         if constexpr (CT::Contiguous<C>) {
            if (range.IsEmpty())
               return IteratorContiguous{H {}, range};

            if constexpr (REVERSE)
               return IteratorContiguous {range.template AsAt<H>(range.GetCount() - 1), range};
            else
               return IteratorContiguous {range.template AsAt<H>(0), range};
         }
         else {
            if (range.IsEmpty())
               return IteratorDiscontiguous{nullptr, nullptr, /*range.GetHashTable(), range.GetHashTableEnd(),*/ H {}};

            if constexpr (REVERSE)
               return IteratorDiscontiguous {range.GetHashTable(), range.GetHashTableEnd(), range.template AsAt<H>(range.GetCount() - 1)};
            else
               return IteratorDiscontiguous {range.GetHashTable(), range.GetHashTableEnd(), range.template AsAt<H>(0)};
         }
      }

      constexpr auto end() const noexcept {
         if constexpr (CT::Contiguous<C>) {
            if (range.IsEmpty())
               return IteratorContiguous{H {}, range};

            if constexpr (REVERSE)
               return --IteratorContiguous {range.template AsAt<H>(0), range};
            else
               return ++IteratorContiguous {range.template AsAt<H>(range.GetCount() - 1), range};
         }
         else {
            if (range.IsEmpty())
               return IteratorDiscontiguous{nullptr, nullptr, /*range.GetHashTable(), range.GetHashTableEnd(),*/ H {}};

            if constexpr (REVERSE)
               return --IteratorDiscontiguous {range.GetHashTable(), range.GetHashTableEnd(), range.template AsAt<H>(0)};
            else
               return ++IteratorDiscontiguous {range.GetHashTable(), range.GetHashTableEnd(), range.template AsAt<H>(range.GetCount() - 1)};
         }
      }
   };

   template<class C>
   IterateHandles(C&) -> IterateHandles<false, C>;



   ///                                                                        
   /// The contiguous iterator                                                
   ///                                                                        
   template<bool REVERSE, class C>
   struct IterateHandles<REVERSE, C>::IteratorContiguous {
      using CTTI_ReflectAs = void;
      using CTTI_Iterator  = Yup;
      using difference_type = std::ptrdiff_t;
      using value_type = H;
      using reference = H&;

      H  mIt;
      C& mRange;

      constexpr IteratorContiguous() noexcept = default;
      constexpr IteratorContiguous(IteratorContiguous const&) noexcept = default;
      constexpr IteratorContiguous(IteratorContiguous&&) noexcept = default;

      constexpr IteratorContiguous(H const& it, C& range) noexcept
         : mIt    {it}
         , mRange {range} {}

      constexpr IteratorContiguous(H&& it, C& range) noexcept
         : mIt    {LglsFwd(it)}
         , mRange {range} {}

      constexpr auto operator = (IteratorContiguous const& rhs) assumptious -> IteratorContiguous& {
         LglsAssumeUser(&mRange == &rhs.mRange,
            "Iterators are for different containers");
         mIt = rhs.mIt;
         return *this;
      }

      constexpr auto operator = (IteratorContiguous&& rhs) assumptious -> IteratorContiguous& {
         LglsAssumeUser(&mRange == &rhs.mRange,
            "Iterators are for different containers");
         mIt = rhs.mIt;
         return *this;
      }

      constexpr bool operator == (CT::Iterator auto const& rhs) const assumptious {
         LglsAssumeUser(&mRange == &rhs.mRange,
            "Iterators are for different containers");
         return mIt.GetRaw() == rhs.mIt.GetRaw();
      }

      constexpr auto operator <=> (CT::Iterator auto const& rhs) const noexcept {
         return mIt.GetRaw() <=> rhs.mIt.GetRaw();
      }

      explicit constexpr operator bool() const noexcept {
         return mIt.GetRaw() != mRange.GetRawEnd();
      }
         
      decltype(auto) operator *  ()       noexcept { return (mIt); }
      decltype(auto) operator *  () const noexcept { return (mIt); }
      decltype(auto) operator -> ()       noexcept { return &mIt; }
      decltype(auto) operator -> () const noexcept { return &mIt; }

      auto operator + (difference_type c) const noexcept -> IteratorContiguous {
         if constexpr (REVERSE) return {mIt - c, mRange};
         else                   return {mIt + c, mRange};
      }

      auto operator - (difference_type c) const noexcept -> IteratorContiguous {
         if constexpr (REVERSE) return {mIt + c, mRange};
         else                   return {mIt - c, mRange};
      }

      auto operator += (difference_type c) noexcept -> IteratorContiguous& {
         if constexpr (REVERSE) mIt -= c;
         else                   mIt += c;
         return *this;
      }

      auto operator -= (difference_type c) noexcept -> IteratorContiguous& {
         if constexpr (REVERSE) mIt += c;
         else                   mIt -= c;
         return *this;
      }

      decltype(auto) operator[] (const difference_type offset) const noexcept {
         if constexpr (REVERSE) return mIt - offset;
         else                   return mIt + offset;
      }

      /// Prefix increment                                                    
      auto operator ++ () noexcept -> IteratorContiguous& {
         if constexpr (REVERSE) --mIt;
         else                   ++mIt;
         return *this;
      }

      /// Suffix increment                                                    
      auto operator ++ (int) noexcept -> IteratorContiguous {
         if constexpr (REVERSE) return {mIt--, mRange};
         else                   return {mIt++, mRange};
      }

      /// Prefix decrement                                                    
      auto operator -- () noexcept -> IteratorContiguous& {
         if constexpr (REVERSE) ++mIt;
         else                   --mIt;
         return *this;
      }

      /// Suffix decrement                                                    
      auto operator -- (int) noexcept -> IteratorContiguous {
         if constexpr (REVERSE) return {mIt++, mRange};
         else                   return {mIt--, mRange};
      }

      /// Get the integer element difference between two iterators            
      auto operator - (CT::Iterator auto const& rhs) const assumptious
      -> difference_type {
         LglsAssumeUser(&mRange == &rhs.mRange,
            "Iterators are for different containers");

         if constexpr (CT::TypeErased<C>) {
            const auto range = mIt.template GetRawAs<uint8_t>() - rhs.mIt.template GetRawAs<uint8_t>();
            return static_cast<difference_type>(range / mRange.GetStride());
         }
         else {
            const auto range = mIt.GetRaw() - rhs.mIt.GetRaw();
            return static_cast<difference_type>(range);
         }
      }
   };


   ///                                                                        
   /// The discontiguous iterator                                             
   ///                                                                        
   template<bool REVERSE, class C>
   struct IterateHandles<REVERSE, C>::IteratorDiscontiguous {
      using CTTI_ReflectAs = void;
      using CTTI_Iterator  = Yup;
      using difference_type = std::ptrdiff_t;
      using value_type = H;
      using I = typename C::TableType const;
      using reference = H&;

      I* mTable;
      I* const mTableEnd;
      H  mIt;

      constexpr IteratorDiscontiguous() noexcept = default;
      constexpr IteratorDiscontiguous(IteratorDiscontiguous const&) noexcept = default;
      constexpr IteratorDiscontiguous(IteratorDiscontiguous&&) noexcept = default;

      constexpr IteratorDiscontiguous(I* table, I* tableEnd, H const& it) noexcept
         : mTable    {table}
         , mTableEnd {tableEnd}
         , mIt       {it} {}

      constexpr IteratorDiscontiguous(I* table, I* tableEnd, H&& it) noexcept
         : mTable    {table}
         , mTableEnd {tableEnd}
         , mIt       {LglsFwd(it)} {}

      constexpr auto operator = (IteratorDiscontiguous const& rhs) noexcept
      -> IteratorDiscontiguous& {
         mTable = rhs.mTable;
         mTableEnd = rhs.mTableEnd;
         mIt = rhs.mIt;
         return *this;
      }

      constexpr auto operator = (IteratorDiscontiguous&& rhs) noexcept
      -> IteratorDiscontiguous& {
         mTable = rhs.mTable;
         mTableEnd = rhs.mTableEnd;
         mIt = LglsMov(rhs.mIt);
         return *this;
      }

      constexpr bool operator == (CT::Iterator auto const& rhs) const assumptious {
         return mTable == rhs.mTable;
      }

      constexpr auto operator <=> (CT::Iterator auto const& rhs) const noexcept {
         return mTable <=> rhs.mTable;
      }

      explicit constexpr operator bool() const noexcept {
         return mTable != mTableEnd;
      }
         
      decltype(auto) operator *  ()       noexcept { return (mIt); }
      decltype(auto) operator *  () const noexcept { return (mIt); }
      decltype(auto) operator -> ()       noexcept { return &mIt;  }
      decltype(auto) operator -> () const noexcept { return &mIt;  }

      auto operator + (difference_type c) const noexcept -> IteratorDiscontiguous {
         if (mTable == mTableEnd)
            return {mTableEnd, mTableEnd, H{}};

         auto table = mTable;
         while (c) {
            if constexpr (REVERSE) {
               c -= *table ? 1 : 0;
               --table;
            }
            else {
               c += *table ? 1 : 0;
               ++table;
            }

            if (table == mTableEnd)
               return {mTableEnd, mTableEnd, H{}};
         }

         while (not *table) {
            if constexpr (REVERSE) --table;
            else                   ++table;

            if (table == mTableEnd)
               return {mTableEnd, mTableEnd, H{}};
         }

         return {table, mTableEnd, mIt + (table - mTable)};
      }

      auto operator - (difference_type c) const noexcept -> IteratorDiscontiguous {
         return operator + (-c);
      }

      auto operator += (difference_type c) noexcept -> IteratorDiscontiguous& {
         if (mTable == mTableEnd)
            return *this;

         auto const oldTable = mTable;
         while (c) {
            if constexpr (REVERSE) {
               c -= *mTable ? 1 : 0;
               --mTable;
            }
            else {
               c += *mTable ? 1 : 0;
               ++mTable;
            }

            if (mTable == mTableEnd) {
               IF_SAFE(mIt = {});
               return *this;
            }
         }

         while (not *mTable) {
            if constexpr (REVERSE) --mTable;
            else                   ++mTable;

            if (mTable == mTableEnd) {
               IF_SAFE(mIt = {});
               return *this;
            }
         }

         mIt += mTable - oldTable;
         return *this;
      }

      auto operator -= (difference_type c) noexcept -> IteratorDiscontiguous& {
         return operator += (-c);
      }

      decltype(auto) operator[] (const difference_type c) const assumptious {
         LglsAssumeUser(mTable != mTableEnd, "Subscript out of range");
         auto table = mTable;
         while (c) {
            if constexpr (REVERSE) {
               c -= *table ? 1 : 0;
               --table;
            }
            else {
               c += *table ? 1 : 0;
               ++table;
            }

            LglsAssumeUser(table != mTableEnd, "Subscript out of range");
         }

         while (not *table) {
            if constexpr (REVERSE) --table;
            else                   ++table;

            LglsAssumeUser(table != mTableEnd, "Subscript out of range");
         }

         return mIt + (table - mTable);
      }

      /// Prefix increment                                                    
      auto operator ++ () noexcept -> IteratorDiscontiguous& {
         return operator += (1);
      }

      /// Suffix increment                                                    
      auto operator ++ (int) noexcept -> IteratorDiscontiguous {
         IteratorDiscontiguous backup = *this;
         operator += (1);
         return backup;
      }

      /// Prefix decrement                                                    
      auto operator -- () noexcept -> IteratorDiscontiguous& {
         return operator += (-1);
      }

      /// Suffix decrement                                                    
      auto operator -- (int) noexcept -> IteratorDiscontiguous {
         IteratorDiscontiguous backup = *this;
         operator += (-1);
         return backup;
      }

      /// Get the integer element difference between two iterators            
      auto operator - (CT::Iterator auto const& rhs) const noexcept
      -> difference_type {
         return mTable - rhs.mTable;
      }
   };
}