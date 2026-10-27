///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Container.hpp"
#include <Langulus/CT/Index.hpp>
//#include <Langulus/Allocator.hpp>


namespace Langulus::Annies::Component
{
   ///                                                                        
   /// Makes all contained deep containers inside relevant stacks/heaps       
   /// accessible from here as if they form a single contiguous container.    
   ///   @tparam ID1, ID2, IDN... the providers we're indexing                
   template<Cid ID0, Cid ID1, Cid...IDN>
   struct IndexedComposite {
      /// Refers back to this particular component instance through the       
      /// deduced 'this'. Just for convenience. It is #undef-ed at the end of 
      /// this file.                                                          
      #define ThisCom self.IndexedComposite<ID0, ID1, IDN...>

      using CTTI_Component = Yup;
      using CTTI_ReflectAs = void;
      using Id             = Values<ID0, ID1, IDN...>;
      using CompositeIds   = Id;

      static constexpr int ComponentPrecedence = 0;

      using IteratorCategory = ::std::bidirectional_iterator_tag;
      static constexpr IndexingStyle Indexed = IndexingStyle::IndexedComposite;

      /*template<class C>
      constexpr auto GetCompositeProviders(this C const&) noexcept {
         return C::FindProviders(Id{});
      }*/
      
      /// Get the composite count                                             
      size_t GetCount(this auto const& self) noexcept {
         constexpr auto providers = self.FindProviders(Id{});
         size_t accumulated_count = 0;
         ForEach(providers, [&self, &accumulated_count]<class C> noexcept {
            static_assert(CT::Deep<decltype(self.C::GetRaw())>,
               "Non-deep provider marked as relevant: " + CppNameOf<C>());
            accumulated_count += self.C::GetRaw()->GetCount();
         });
         return accumulated_count;
      }

      /// Check if the container is empty                                     
      ///   @return true if empty                                             
      bool IsEmpty(this auto const& self) noexcept {
         constexpr auto providers = self.FindProviders(Id{});
         return ForEachAnd(providers, [&self]<class C> noexcept {
            static_assert(CT::Deep<decltype(self.C::GetRaw())>,
               "Non-deep provider marked as relevant: " + CppNameOf<C>());
            return self.C::GetRaw()->IsEmpty();
         });
      }

      /// Check if the container is not empty                                 
      ///   @return true if not empty                                         
      explicit constexpr operator bool(this auto const& self) noexcept {
         return not self.IsEmpty();
      }

      /// Check if the container has missing scopes, nested                   
      ///   @return true if there's at least one missing entry                
      bool IsMissingDeep(this auto const& self) noexcept {
         constexpr auto providers = self.FindProviders(Id{});
         return ForEachOr(providers, [&self]<class C> noexcept {
            static_assert(CT::Deep<decltype(self.C::GetRaw())>,
               "Non-deep provider marked as relevant: " + CppNameOf<C>());
            return self.C::GetRaw()->IsMissingDeep();
         });
      }

      /// Check if the container has executable items, nested                 
      ///   @return true if there's at least one executable entry             
      bool IsExecutable(this auto const& self) noexcept {
         constexpr auto providers = self.FindProviders(Id{});
         return ForEachOr(providers, [&self]<class C> noexcept {
            static_assert(CT::Deep<decltype(self.C::GetRaw())>,
               "Non-deep provider marked as relevant: " + CppNameOf<C>());
            return self.C::GetRaw()->IsExecutable();
         });
      }

   protected:
      struct Where {
         Cid    provider;
         size_t offset;
      };

      /// MARK: BrowseComposite                                               
      /// Browse providers, converting contiguous index into provider index.  
      ///                                                                     
      /// Composites are indexed the following way:                           
      /// Provider0 has size 5:  [0][1][2][3][4]                              
      /// Provider1 has size 0:  skipped                                      
      /// Provider2 has size 555:[5][6][7][8][9][10][11][12][13][14]...       
      /// etc..                                                               
      ///                                                                     
      /// Basically, all providers are "stitched" together to appear as one   
      /// seamless contiguous sequence of elements. Each provider can have    
      /// its own type, count, even indexing strategy. In other words, you    
      /// can nest indefinitely many different containers and make them       
      /// appear as a single monolithic one.                                  
      ///   @returns the offset _in the appropriate provider_                 
      template<CT::Container C>
      constexpr auto BrowseComposite(this C const& self, size_t index) assumptious -> Where {
         constexpr auto providers = self.FindProviders(Id{});
         Cid selected = unsigned(-1);
         ForEachAnd(providers, [&self,&index,&selected]<class P> noexcept {
            static_assert(CT::Deep<decltype(self.P::GetRaw())>,
               "Non-deep provider marked as relevant: " + CppNameOf<P>());
            const size_t provider_count = self.P::GetRaw()->GetCount();
            if (provider_count and index < provider_count) {
               selected = P::Id::First;
               return false;
            }
            index -= provider_count;
            return true;
         });
         LglsAssumeDev(selected != unsigned(-1), "Index out of range");
         return {selected, index};
      }

      /// MARK: SimplifyIndex                                                 
      /// Convert an index to an offset.                                      
      /// Special indices will be contextualized.                             
      ///   @param index the index to simplify                                
      ///   @return a simple element offset into contiguous memory            
      template<CT::Container C, CT::Index INDEX>
      constexpr auto SimplifyIndex(this C const& self, INDEX index) -> Where {
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
            return ThisCom::BrowseComposite(0);
         else if constexpr (::std::same_as<INDEX, Index::Inner::Middle>)
            return ThisCom::BrowseComposite(self.GetCount() / 2);
         else if constexpr (::std::same_as<INDEX, Index::Inner::Back>)
            return ThisCom::BrowseComposite(self.GetCount());
         else if constexpr (::std::same_as<INDEX, Index::Inner::Biggest>)
            return self.GetIndexLargest();
         else if constexpr (::std::same_as<INDEX, Index::Inner::Smallest>)
            return self.GetIndexSmallest();
         else if constexpr (::std::same_as<INDEX, Index::Inner::Random>)
            return self.GetIndexRandom();
         else if constexpr (::std::same_as<INDEX, Index::Inner::First>)
            return ThisCom::BrowseComposite(0);
         else if constexpr (::std::same_as<INDEX, Index::Inner::Last>) {
            const auto c = self.GetCount();
            LglsAssert(c > 0, "Can't get last index");
            return ThisCom::BrowseComposite(c - 1);
         }
         else if constexpr (requires { index.index; }) {
            // If index is negative, wrap it around (if in range)       
            const auto c = self.GetCount();
            if (index.index < 0) {
               LglsAssert(c + index.index < c and c + index.index >= 0,
                  "Integer index out of range");
               return ThisCom::BrowseComposite(c + index.index);
            }

            LglsAssert(index.index < c,
               "Integer index out of range");
            return ThisCom::BrowseComposite(index.index);
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
            return ThisCom::BrowseComposite(index);
         }
      }

   public:
      /// Get pointer to Nth element of a specific dimension.                 
      /// This is a lower-level routine that does only sparseness checking.   
      /// No conversion or copying occurs, only pointer arithmetic.           
      ///   @attention no type-safety                                         
      ///   @attention assumes the container is typed                         
      ///   @attention assumes the container has valid memory                 
      ///   @tparam AS the type of data we're accessing - use void to use the 
      ///      type of the container, if statically typed                     
      ///   @tparam SID can be used to access specific dimension              
      ///   @param idx the index                                              
      ///   @return pointer to the chosen element                             
      /*template<class AS = void, CT::Container C>
      auto* GetAt(this C&& self, CT::Index auto&& idx) assumptious {
         auto where = self.SimplifyIndex(idx);
      }*/

      /// Get Nth element as a handle, or any desired wrapping type.          
      /// Conversion or copying may occur depending on type.                  
      ///   @attention will throw if incompatible type is provided            
      ///   @tparam AS the type we're wrapping in                             
      ///   @tparam SID can be used to access specific dimension. It is       
      ///      irrelevant if AS is a handle.                                  
      ///   @param idx the index                                              
      ///   @return the element, as a reference if possible                   
      template<CT::NotVoid AS, CT::Container C>
      AS AsAt(this C&& self, CT::Index auto&& idx) {
         auto where = self.SimplifyIndex(idx);
         AS result;
         constexpr auto providers = self.FindProviders(Id{});
         ForEachAnd(providers, [&self,&where,&result]<class P> {
            if (P::Id::First == where.provider) {
               result = self.P::GetRaw()->template AsAt<AS>(where.offset);
               return false;
            }
            return true;
         });
         return result;
      }

      /// Subscript operator for accessing element at a specific index        
      ///   @param idx the index                                              
      ///   @return the picked element                                        
      template<CT::Container C>
      decltype(auto) operator[] (this C&& self, CT::Index auto&& idx) assumptious {
         //if constexpr (CT::TypeErased<C>)
            return ThisCom::template AsAt<DecidePick<C>>(LglsFwd(idx));
         //else
         //   return *ThisCom::GetAt(LglsFwd(idx));
      }
   };
}

#undef ThisCom