///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include <Langulus/IntentOf.hpp>
#include <ranges>


namespace Langulus::Annies
{

   ///                                                                        
   ///   Iterate multiple containers with the same ranged-for                 
   ///                                                                        
   /// Use like this: for(auto i : IterateTogether(pack1, pack2)), where      
   /// 'packN' can be any range, including std one. You can retrieve the      
   /// current element by using i[N], or i.one() i.two() for the first two.   
   template<bool REVERSE, class...C>
   struct IterateTogether {
      using CTTI_ReflectAs = void;
      static_assert(CT::NoIntent<C...>,
         "C can't have an intent");
      static_assert(CT::NotReference<C...>,
         "C can't be a reference");
      static_assert((::std::ranges::range<C> and ...),
         "C is not a range");
      static constexpr size_t Size = sizeof...(C);
      static_assert(Size > 1,
         "IterateTogether needs at least two containers");

   protected:
      using Hs = ::std::tuple<Tif<REVERSE, decltype(LglsFake(C).rbegin()),
                                           decltype(LglsFake(C).begin())>...>;
      using Cs = ::std::tuple<C&...>;

      Cs ranges;

   public:
      explicit constexpr IterateTogether(C&...a) noexcept
         : ranges {a...} {}

      /// A single combined iterator                                          
      struct Iterator {
         using CTTI_ReflectAs = void;
         using CTTI_Iterator = Yup;
         using difference_type = std::ptrdiff_t;

         Hs mIt;
         Cs mRanges;

         template<size_t I>
         constexpr auto* Range() const noexcept {
            return &::std::get<I>(mRanges);
         }

         template<size_t I>
         constexpr auto* Get() const noexcept {
            decltype(auto) it = *::std::get<I>(mIt);
            if constexpr (CT::Handle<decltype(it)>)
               return it.GetRaw();
            else
               return &it;
         }

         decltype(auto) one() noexcept { return *::std::get<0>(mIt); }
         decltype(auto) two() noexcept { return *::std::get<1>(mIt); }

         constexpr Iterator() noexcept = default;
         constexpr Iterator(Iterator const&) noexcept = default;
         constexpr Iterator(Iterator&&) noexcept = default;
         constexpr Iterator(Hs const& it, Cs& ranges) noexcept
            : mIt    {it}
            , mRanges{ranges} {}
         constexpr Iterator(Hs&& it, Cs& ranges) noexcept
            : mIt    {LglsFwd(it)}
            , mRanges{ranges} {}

         constexpr auto operator = (Iterator const& rhs) noexcept -> Iterator& {
            mIt = rhs.mIt;
            return *this;
         }

         constexpr auto operator = (Iterator&& rhs) noexcept -> Iterator& {
            mIt = rhs.mIt;
            return *this;
         }

         constexpr bool operator == (CT::Iterator auto const& rhs) const assumptious {
            return LglsSequence(Size, {
               LglsAssumeUser(((Range<I>() == rhs.template Range<I>()) and ...),
                  "Iterators are for different containers");
               return ((Get<I>() == rhs.template Get<I>()) and ...);
            });
         }

         constexpr bool operator < (CT::Iterator auto const& rhs) const assumptious {
            return LglsSequence(Size, {
               LglsAssumeUser(((Range<I>() == rhs.template Range<I>()) and ...),
                  "Iterators are for different containers");
               return ((Get<I>() < rhs.template Get<I>()) and ...);
            });
         }

         constexpr bool operator <= (CT::Iterator auto const& rhs) const assumptious {
            return LglsSequence(Size, {
               LglsAssumeUser(((Range<I>() == rhs.template Range<I>()) and ...),
                  "Iterators are for different containers");
               return ((Get<I>() <= rhs.template Get<I>()) and ...);
            });
         }

         constexpr bool operator > (CT::Iterator auto const& rhs) const assumptious {
            return LglsSequence(Size, {
               LglsAssumeUser(((Range<I>() == rhs.template Range<I>()) and ...),
                  "Iterators are for different containers");
               return ((Get<I>() > rhs.template Get<I>()) and ...);
            });
         }

         constexpr bool operator >= (CT::Iterator auto const& rhs) const assumptious {
            return LglsSequence(Size, {
               LglsAssumeUser(((Range<I>() == rhs.template Range<I>()) and ...),
                  "Iterators are for different containers");
               return ((Get<I>() >= rhs.template Get<I>()) and ...);
            });
         }

         explicit constexpr operator bool() const noexcept {
            return LglsSequence(Size, {
               return ((Get<I>() != Range<I>().end()) and ...);
            });
         }

         auto operator *  ()       noexcept -> Iterator&       { return *this; }
         auto operator *  () const noexcept -> Iterator const& { return *this; }
         auto operator -> ()       noexcept -> Iterator&       { return *this; }
         auto operator -> () const noexcept -> Iterator const& { return *this; }
         
         friend auto operator + (difference_type lhs, Iterator const& rhs) noexcept -> Iterator {
            static_assert(not REVERSE);
            return LglsSequence(Size, {
               return Iterator(Hs{(::std::get<I>(rhs.mIt) + lhs)...}, rhs.mRanges);
            });
         }

         auto operator + (difference_type c) const noexcept -> Iterator {
            if constexpr (REVERSE) {
               return LglsSequence(Size, {
                  return Iterator(Hs{(::std::get<I>(mIt) + c)...}, mRanges);
               });
            }
            else {
               return LglsSequence(Size, {
                  return Iterator(Hs{(::std::get<I>(mIt) + c)...}, mRanges);
               });
            }
         }

         auto operator - (difference_type c) const noexcept -> Iterator {
            if constexpr (REVERSE) {
               return LglsSequence(Size, {
                  return Iterator(Hs{(::std::get<I>(mIt) - c)...}, mRanges);
               });
            }
            else {
               return LglsSequence(Size, {
                  return Iterator(Hs{(::std::get<I>(mIt) - c)...}, mRanges);
               });
            }
         }

         auto operator += (difference_type c) noexcept -> Iterator& {
            if constexpr (REVERSE)
               LglsSequence(Size, { ((::std::get<I>(mIt) -= c), ...); });
            else
               LglsSequence(Size, { ((::std::get<I>(mIt) += c), ...); });
            return *this;
         }

         auto operator -= (difference_type c) noexcept -> Iterator& {
            if constexpr (REVERSE)
               LglsSequence(Size, { ((::std::get<I>(mIt) += c), ...); });
            else
               LglsSequence(Size, { ((::std::get<I>(mIt) -= c), ...); });
            return *this;
         }

         decltype(auto) operator[] (const difference_type offset) const noexcept {
            if constexpr (REVERSE) {
               return LglsSequence(Size, {
                  return Iterator(Hs{(::std::get<I>(mIt) - offset)...}, mRanges);
               });
            }
            else {
               return LglsSequence(Size, {
                  return Iterator(Hs{(::std::get<I>(mIt) + offset)...}, mRanges);
               });
            }
         }

         /// Prefix increment                                                 
         auto operator ++ () noexcept -> Iterator& {
            if constexpr (REVERSE)
               LglsSequence(Size, { ((--::std::get<I>(mIt)), ...); });
            else
               LglsSequence(Size, { ((++::std::get<I>(mIt)), ...); });
            return *this;
         }

         /// Suffix increment                                                 
         auto operator ++ (int) noexcept -> Iterator {
            if constexpr (REVERSE) {
               return LglsSequence(Size, {
                  return Iterator(Hs{(::std::get<I>(mIt)--)...}, mRanges);
               });
            }
            else {
               return LglsSequence(Size, {
                  return Iterator(Hs{(::std::get<I>(mIt)++)...}, mRanges);
               });
            }
         }

         /// Prefix decrement                                                 
         auto operator -- () noexcept -> Iterator& {
            if constexpr (REVERSE)
               LglsSequence(Size, { ((++::std::get<I>(mIt)), ...); });
            else
               LglsSequence(Size, { ((--::std::get<I>(mIt)), ...); });
            return *this;
         }

         /// Suffix decrement                                                 
         auto operator -- (int) noexcept -> Iterator {
            if constexpr (REVERSE) {
               return LglsSequence(Size, {
                  return Iterator(Hs{(::std::get<I>(mIt)++)...}, mRanges);
               });
            }
            else {
               return LglsSequence(Size, {
                  return Iterator(Hs{(::std::get<I>(mIt)--)...}, mRanges);
               });
            }
         }

         /// Get the integer element difference between two iterators         
         auto operator - (CT::Iterator auto const& rhs) const assumptious
         -> difference_type {
            auto& lhs_range = ::std::get<0>(    mRanges);
            auto& rhs_range = ::std::get<0>(rhs.mRanges);
            LglsAssumeUser(&lhs_range == &rhs_range,
               "Iterators are for different containers");

            if constexpr (CT::TypeErased<decltype(lhs_range)>) {
               const auto range = one().template GetRawAs<uint8_t>() - rhs.one().template GetRawAs<uint8_t>();
               return static_cast<difference_type>(range / lhs_range.GetStride());
            }
            else {
               const auto range = one().GetRaw() - rhs.one().GetRaw();
               return static_cast<difference_type>(range);
            }
         }
      };

      auto begin() -> Iterator {
         return ::std::apply([&](auto&...i) {
            return Iterator{Hs{i.begin()...}, ranges};
         }, ranges);
      }

      auto end() -> Iterator {
         return ::std::apply([&](auto&...i) {
            return Iterator{Hs{i.end()...}, ranges};
         }, ranges);
      }
   };

   template<class...C>
   IterateTogether(C&...) -> IterateTogether<false, C...>;
}