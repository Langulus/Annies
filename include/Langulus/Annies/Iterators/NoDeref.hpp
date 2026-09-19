///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Component.hpp"
#include <Langulus/IntentOf.hpp>
#include <ranges>


namespace Langulus::Annies
{
   ///                                                                        
   ///   Keep iterator when using ranged-for                                  
   ///                                                                        
   /// When doing for(auto i : container), the statement always dereferences  
   /// the iterator and 'i' always ends up with the contained type.           
   /// Counteract this, and make 'i' be the iterator type instead.            
   /// Use like this: for(auto i : IterateNoDeref(container)), where          
   /// 'container' can be any range, including std one                        
   template<bool REVERSE, class C>
   struct IterateNoDeref {
      using CTTI_ReflectAs = void;
      static_assert(CT::NoIntent<C>,
         "C can't have an intent");
      static_assert(CT::NotReference<C>,
         "C can't be a reference");
      static_assert(::std::ranges::range<C>,
         "C is not a range");

   protected:
      using Count = typename Deref<C>::CountType;
      using H = Tif<REVERSE, decltype(LglsFake(C).rbegin()),
                             decltype(LglsFake(C).begin())>;

      static_assert(CT::NotReference<H>,
         "Iterator can't be a reference");
      static_assert(CT::Iterator<H>,
         "Must be an iterator");

      C& range;

   public:
      constexpr IterateNoDeref(C& a) noexcept
         : range {a} {}

      /// The iterator                                                        
      struct Iterator {
         using CTTI_ReflectAs    = void;
         using CTTI_Iterator     = Yup;
         using difference_type   = std::ptrdiff_t;
         using iterator_category = typename C::IteratorCategory;
         using value_type        = H;
         using reference         = H&;

         H mIt;
         C* mRange;

         constexpr Iterator() noexcept = default;
         constexpr Iterator(Iterator const&) noexcept = default;
         constexpr Iterator(Iterator&&) noexcept = default;

         constexpr Iterator(H const& it, C* range) noexcept
            : mIt    {it}
            , mRange {range} {}

         constexpr Iterator(H&& it, C* range) noexcept
            : mIt    {LglsFwd(it)}
            , mRange {range} {}

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

         constexpr bool operator == (CT::Iterator auto const& rhs) const assumptious {
            LglsAssumeUser(mRange == rhs.mRange,
               "Iterators are for different containers");
            return mIt == rhs.mIt;
         }

         constexpr auto operator <=> (CT::Iterator auto const& rhs) const noexcept {
            return mIt <=> rhs.mIt;
         }

         explicit constexpr operator bool() const noexcept {
            if constexpr (REVERSE) return mIt != mRange->rend();
            else                   return mIt != mRange->end();
         }

         decltype(auto) operator *  () const noexcept { return (mIt); /* *mIt;*/   }
         decltype(auto) operator -> () const noexcept { return &(*mIt); }

         
         friend auto operator + (difference_type lhs, Iterator const& rhs) noexcept -> Iterator {
            static_assert(not REVERSE);
            return {rhs.mIt + lhs, rhs.mRange};
         }

         auto operator + (difference_type c) const noexcept -> Iterator {
            if constexpr (REVERSE) return {mIt - c, mRange};
            else                   return {mIt + c, mRange};
         }

         auto operator - (difference_type c) const noexcept -> Iterator {
            if constexpr (REVERSE) return {mIt + c, mRange};
            else                   return {mIt - c, mRange};
         }

         auto operator += (difference_type c) noexcept -> Iterator& {
            if constexpr (REVERSE) mIt -= c;
            else                   mIt += c;
            return *this;
         }

         auto operator -= (difference_type c) noexcept -> Iterator& {
            if constexpr (REVERSE) mIt += c;
            else                   mIt -= c;
            return *this;
         }

         decltype(auto) operator[] (const difference_type offset) const noexcept {
            if constexpr (REVERSE) {
               if constexpr (CT::Handle<H>) return   mIt - offset;
               else                         return *(mIt - offset);
            }
            else {
               if constexpr (CT::Handle<H>) return   mIt + offset;
               else                         return *(mIt + offset);
            }
         }

         /// Prefix increment                                                 
         auto operator ++ () noexcept -> Iterator& {
            if constexpr (REVERSE) --mIt;
            else                   ++mIt;
            return *this;
         }

         /// Suffix increment                                                 
         auto operator ++ (int) noexcept -> Iterator {
            if constexpr (REVERSE) return {mIt--, mRange};
            else                   return {mIt++, mRange};
         }

         /// Prefix decrement                                                 
         auto operator -- () noexcept -> Iterator& {
            if constexpr (REVERSE) ++mIt;
            else                   --mIt;
            return *this;
         }

         /// Suffix decrement                                                 
         auto operator -- (int) noexcept -> Iterator {
            if constexpr (REVERSE) return {mIt++, mRange};
            else                   return {mIt--, mRange};
         }

         /// Get the integer element difference between two iterators         
         auto operator - (CT::Iterator auto const& rhs) const assumptious
         -> difference_type {
            LglsAssumeUser(mRange == rhs.mRange,
               "Iterators are for different containers");

            if constexpr (CT::TypeErased<C>) {
               const auto range = mIt.template GetRawAs<uint8_t>() - rhs.mIt.template GetRawAs<uint8_t>();
               return static_cast<difference_type>(range / mRange->GetStride());
            }
            else {
               const auto range = mIt.GetRaw() - rhs.mIt.GetRaw();
               return static_cast<difference_type>(range);
            }
         }
      };

      auto begin() -> Iterator  {
         if constexpr (REVERSE) return {range.rbegin(), &range};
         else                   return {range.begin(),  &range};
      }

      auto end() -> Iterator {
         if constexpr (REVERSE) return {range.rend(),   &range};
         else                   return {range.end(),    &range};
      }
   };

   template<class C>
   IterateNoDeref(C&) -> IterateNoDeref<false, C>;
}