///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Indexed-Common.hpp"
#include <Langulus/CT/Signed.hpp>
#include <Langulus/CT/Contiguous.hpp>
#include <limits>


namespace Langulus::Annies::Component
{
   ///                                                                        
   /// Provides random element access based on a linear index, that is        
   /// mapped directly onto contiguous memory.                                
   ///   @tparam ID the provider we're indexing                               
   ///   @tparam SHARED providers that share the same indexing scheme         
   template<Cid ID, Cid...SHARED>
   struct IndexedLinear : IndexedCommon<ID, SHARED...> {
      using CTTI_Contiguous  = Yup;
      using IteratorCategory = ::std::contiguous_iterator_tag;

      static constexpr IndexingStyle Indexed = IndexingStyle::IndexedLinearly;

   protected:
      LglsComIndexedCommon(friend);

      template<CT::Container C>
      using Count = typename Deref<C>::CountType;

      template<CT::Container C>
      using PickRange = Tmut<C, typename Deref<C>::PickRangeMut, typename Deref<C>::PickRange>;

      //template<CT::Container C>
      //static constexpr auto CountMax = ::std::numeric_limits<Count<C>>::max();
      
      /// Convert an index to an offset.                                      
      /// Special indices will be contextualized.                             
      /// Unsigned/signed indices are directly forwarded without any overhead.
      ///   @param index the index to simplify                                
      ///   @return a simple element offset into contiguous memory            
      template<CT::Container C, CT::Index INDEX>
      constexpr auto SimplifyIndex(this C const& self, INDEX index) -> Count<C> {
         //LglsAssumeDev(not self.IsEmpty(), "Container can't be empty");
         /*if (self.IsEmpty())
            return 0;*/

         static_assert(not ::std::same_as<INDEX, Index::Inner::All>,
            "Index::All can't be used here");
         static_assert(not ::std::same_as<INDEX, Index::Inner::Many>,
            "Index::Many can't be used here");
         static_assert(not ::std::same_as<INDEX, Index::Inner::Single>,
            "Index::Single can't be used here");
         static_assert(not ::std::same_as<INDEX, Index::Inner::None>,
            "Index::None can't be used here");

         if constexpr (::std::same_as<INDEX, Index::Inner::Front>)
            return 0;
         else if constexpr (::std::same_as<INDEX, Index::Inner::Middle>)
            return self.GetCount() / 2;
         else if constexpr (::std::same_as<INDEX, Index::Inner::Back>)
            return self.GetCount();
         else if constexpr (::std::same_as<INDEX, Index::Inner::Mode>)
            return self.GetIndexMode();
         else if constexpr (::std::same_as<INDEX, Index::Inner::Biggest>)
            return self.GetIndexLargest();
         else if constexpr (::std::same_as<INDEX, Index::Inner::Smallest>)
            return self.GetIndexSmallest();
         else if constexpr (::std::same_as<INDEX, Index::Inner::Random>)
            return self.GetIndexRandom();
         else if constexpr (::std::same_as<INDEX, Index::Inner::First>)
            return 0;
         else if constexpr (::std::same_as<INDEX, Index::Inner::Last>) {
            const auto c = self.GetCount();
            LglsAssert(c > 0, "Can't get last index");
            return c - 1;
         }
         else if constexpr (requires { index.index; }) {
            // If index is negative, wrap it around (if in range)       
            const auto c = self.GetCount();
            if (index.index < 0) {
               LglsAssert(c + index.index < c and c + index.index >= 0,
                  "Integer index out of range");
               return c + index.index;
            }

            LglsAssert(index.index < c,
               "Integer index out of range");
            return index.index;
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
            return index;
         }
      }
      
      /// Select a contiguous region from the memory block. Unsafe and may    
      /// return memory that has not been initialized yet! The resulting      
      /// data will be disowned.                                              
      ///   @attention assumes container is typed and allocated               
      ///   @param start starting element index (included)                    
      ///   @param count number of sequential elements                        
      ///   @return the selected disowned contiguous range                    
      template<CT::Container C>
      auto SelectInner(this C&& self, Count<C> start, Count<C> count)
      assumptious -> Decay<C> {
         LglsAssumeDev(self.GetRaw(),  "Block is not allocated");
         LglsAssumeDev(self.IsTyped(), "Block is not typed");
         LglsAssumeDev(count,          "Invalid count");
         
         Decay<C> result {Disown(self)};
         result.SetCountInner(count);
         if constexpr (CT::TypeErased<C>)
            result.SetHeapInner(result.template GetRawAs<uint8_t>() + start * result.GetStride());
         else
            result.SetHeapInner(result.GetRaw() + start);
         return result;
      }

      /// Same as above, but implies that count is the remainder              
      ///   @param start starting element index (included)                    
      ///   @return the selected disowned contiguous range                    
      template<CT::Container C>
      auto SelectInner(this C&& self, Count<C> start) assumptious -> Decay<C> {
         return self.SelectInner(start, self.GetCount() - start);
      }

   public:
      template<CT::Container C>
      auto GetIndexMode(this C const&, Count<C>&) assumptious -> Count<C>;

      template<CT::Container C>
      auto Select(this C&&, CT::Index auto&&, Count<C>) assumptious -> PickRange<C>;

      template<CT::Container C>
      void SwapContentsAt(this C&, CT::Index auto&&, CT::Index auto) assumptious;
   };
}