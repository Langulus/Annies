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
   /// When container is type-erased, iterator will always be a handle.       
   /// When container is statically-typed sparse, iterator will not be a ref, 
   /// since container might keep track of shared pointers and you can't just 
   /// overwrite them.                                                        
   /// Otherwise, iterator will be a direct reference to the dense element.   
   template<bool REVERSE, class C>
   struct IterateDefault {
      using CTTI_ReflectAs = void;

      static_assert(CT::NoIntent<C>,
         "C can't have an intent");
      static_assert(CT::NotReference<C>,
         "C can't be a reference");
      static_assert(CT::ContainsMany<C>,
         "C is not iteratable because it contains exactly one element");//TODO who cares? maybe just drop this for consistency?
      static_assert(CT::Indexed<C>,
         "C is not indexed");

   protected:
      using T = Tif<CT::Void<TypeOf<C>>, No, TypeOf<C>>;
      using H = Tif<CT::TypeErased<C> or CT::Multidimensional<C> or CT::Neat<C>, DecideHandle<C>, LglsMutIf(C, T*)>;
      using Exposed = Tif<CT::Handle<H>, 
         DecideHandle<C> const&,
         Tif<CT::Sparse<T>, LglsMutIf(C, T), LglsMutIf(C, T&)>
      >;
      using ExposedPtr = Tif<CT::Handle<H>, 
         DecideHandle<C> const*,
         Tif<CT::Sparse<T>, LglsMutIf(C, DeptrAll<T>*), LglsMutIf(C, T*)>
      >;

      static_assert(CT::NotReference<H>,
         "Iterator can't be a reference");
      static_assert(CT::Handle<H> or CT::Sparse<H>,
         "Must be either a pointer, or a handle");

      C& range;

   public:
      explicit constexpr IterateDefault(C& a) noexcept
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

         /// Checks whether the current element mIt is pointing at is valid   
         /// if inside a table. Always true if C is not a table.              
         LANGULUS(ALWAYS_INLINED)
         constexpr bool CheckTable() const noexcept {
            if constexpr (CT::IndexedTable<C>) {
               if constexpr (CT::Handle<H>)
                  return mRange->IsSlotOccupied(mIt - mRange->GetHandle());
               else
                  return mRange->IsSlotOccupied(mIt - mRange->GetRaw());
            }
            else return true;
         }

         LANGULUS(ALWAYS_INLINED)
         constexpr bool CheckTable(size_t slot) const noexcept {
            if constexpr (CT::IndexedTable<C>)
               return mRange->IsSlotOccupied(slot);
            else
               return true;
         }

         constexpr Iterator() noexcept = default;
         constexpr Iterator(Iterator const&) noexcept = default;
         constexpr Iterator(Iterator&&) noexcept = default;

         /*constexpr Iterator(Deptr<H> const& it, C* range) assumptious requires CT::Sparse<H>
            : mIt    {&it}
            , mRange {range} {
            LglsAssumeDev(CheckTable(),
               "Iterators to hash table initialized with an invalid element");
         }

         constexpr Iterator(H const& it, C* range) assumptious
            : mIt    {it}
            , mRange {range} {
            LglsAssumeDev(CheckTable(),
               "Iterators to hash table initialized with an invalid element");
         }*/

         constexpr Iterator(H&& it, C* range) assumptious
            : mIt    {LglsFwd(it)}
            , mRange {range} {
            LglsAssumeDev(CheckTable(),
               "Iterators to hash table initialized with an invalid element");
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
            if constexpr (CT::IndexedTable<C>) {
               if constexpr (REVERSE) {
                  while (rhs.mIt < mIt and not CheckTable())
                     --mIt;
               }
               else {
                  while (rhs.mIt > mIt and not CheckTable())
                     ++mIt;
               }
            }
            return mIt == rhs.mIt;
         }

         constexpr auto operator <=> (CT::Iterator auto const& rhs) const noexcept {
            // We sneakily skip invalid table entries while comparing   
            if constexpr (CT::IndexedTable<C>) {
               if constexpr (REVERSE) {
                  while (rhs.mIt < mIt and not CheckTable())
                     --mIt;
               }
               else {
                  while (rhs.mIt > mIt and not CheckTable())
                     ++mIt;
               }
            }
            return mIt <=> rhs.mIt;
         }
         
         explicit constexpr operator bool() const noexcept {
            if constexpr (CT::Handle<H>)
               return mIt.GetRaw() != mRange->GetRawEnd();
            else
               return mIt != mRange->GetRawEnd();
         }

         auto operator * (this auto&& self) noexcept -> Exposed {
            if constexpr (CT::Handle<H>) return (self.mIt);
            else                         return *self.mIt;
         }

         auto operator -> (this auto&& self) noexcept -> ExposedPtr {
            if      constexpr (CT::Handle<H>)   return &self.mIt;
            else if constexpr (CT::Sparse<H>)   return &DenseCast(self.mIt);
            else                                return  self.mIt;
         }

         auto operator + (difference_type c) const assumptious -> Iterator {
            if (c == 0)
               return *this;

            LglsAssumeDev(mIt, "Can't offset invalid iterator");

            if constexpr (REVERSE)
               c *= -1;

            if constexpr (CT::IndexedLinearly<C>)
               return {mIt + c, mRange};
            else {
               auto next = mIt;
               if (c > 0) {
                  while (c) {
                     ++next;
                     if (CheckTable(next - mRange->GetHandle()))
                        --c;
                  }
               }
               else {
                  while (c) {
                     --next;
                     if (CheckTable(next - mRange->GetHandle()))
                        ++c;
                  }
               }
               return {next, mRange};
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

            LglsAssumeDev(mIt, "Can't offset invalid iterator");

            if constexpr (REVERSE)
               c *= -1;

            if constexpr (CT::IndexedLinearly<C>)
               mIt += c;
            else {
               if (c > 0) {
                  while (c) {
                     ++mIt;
                     if (CheckTable())
                        --c;
                  }
               }
               else {
                  while (c) {
                     --mIt;
                     if (CheckTable())
                        ++c;
                  }
               }
            }
            return *this;
         }

         auto operator -= (difference_type c) assumptious -> Iterator& {
            return operator += (-c);
         }

         auto operator[] (difference_type offset) const assumptious -> Exposed {
            LglsAssumeDev(mIt, "Can't access invalid iterator");
            if constexpr (REVERSE)
               offset *= -1;

            if constexpr (CT::IndexedLinearly<C>) {
               if constexpr (CT::Handle<H>)  return   mIt + offset;
               else                          return *(mIt + offset);
            }
            else {
               auto next = mIt;
               if (offset > 0) {
                  while (offset) {
                     ++next;
                     if (CheckTable(next - mRange->GetHandle()))
                        --offset;
                  }
               }
               else {
                  while (offset) {
                     --next;
                     if (CheckTable(next - mRange->GetHandle()))
                        ++offset;
                  }
               }

               if constexpr (CT::Handle<H>)
                  return next;
               else
                  return *next;
            }
         }

         /// Prefix increment                                                 
         auto operator ++ () assumptious -> Iterator& {
            LglsAssumeDev(mIt, "Can't increment invalid iterator");
            if constexpr (REVERSE) --mIt;
            else                   ++mIt;
            return *this;
         }

         /// Suffix increment                                                 
         auto operator ++ (int) assumptious -> Iterator {
            LglsAssumeDev(mIt, "Can't increment invalid iterator");
            if constexpr (CT::IndexedLinearly<C>) {
               if constexpr (REVERSE) return {mIt--, mRange};
               else                   return {mIt++, mRange};
            }
            else {
               auto next = mIt;

               if constexpr (REVERSE) {
                  do --next;
                  while (not CheckTable(next - mRange->GetHandle()));
               }
               else {
                  do ++next;
                  while (not CheckTable(next - mRange->GetHandle()));
               }

               return {next, mRange};
            }
         }

         /// Prefix decrement                                                 
         auto operator -- () assumptious -> Iterator& {
            LglsAssumeDev(mIt, "Can't decrement invalid iterator");
            if constexpr (CT::IndexedLinearly<C>) {
               if constexpr (REVERSE)  ++mIt;
               else                    --mIt;
            }
            else {
               if constexpr (REVERSE)  ++mIt;
               else                    --mIt;
            }
            return *this;
         }

         /// Suffix decrement                                                 
         auto operator -- (int) assumptious -> Iterator {
            LglsAssumeDev(mIt, "Can't decrement invalid iterator");
            if constexpr (CT::IndexedLinearly<C>) {
               if constexpr (REVERSE) return {mIt++, mRange};
               else                   return {mIt--, mRange};
            }
            else {
               auto next = mIt;

               if constexpr (REVERSE) {
                  do ++next;
                  while (not CheckTable(next - mRange->GetHandle()));
               }
               else {
                  do --next;
                  while (not CheckTable(next - mRange->GetHandle()));
               }

               return {next, mRange};
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
                  const auto range = mIt.template GetRawAs<uint8_t>()
                                   - rhs.mIt.template GetRawAs<uint8_t>();
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
   IterateDefault(C&) -> IterateDefault<false, C>;
}