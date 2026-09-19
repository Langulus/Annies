///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Component.hpp"
#include "Langulus/CT/Index.hpp"
#include "Langulus/CT/Serializer.hpp"
#include "Langulus/IntentOf.hpp"
#include "Langulus/Typenav.hpp"
#include <cstddef>
#include <vector>


namespace Langulus::Annies::Component
{
   /// Refers back to this particular component instance through the deduced  
   /// 'this'. Just for convenience. It is #undef-ed at the end of this file. 
   #define ThisCom self.Merging<CONVERT, ID, SHARED...>

   ///                                                                        
   /// Implements merging for containers.                                     
   /// Merging (unlike emplacement) extends the memory space and may move     
   /// things around. It guarantees that nothing gets overwritten.            
   /// Merging (unlike insertion) disallows for duplicated elements.          
   ///   @tparam CONVERT whether to serialize as self before merging item.    
   ///      Useful for byte and text containers. Use 'false' to merge without 
   ///      serialization.                                                    
   ///   @tparam ID, SHARED providers that share the same merge behavior.     
   template<bool CONVERT, Cid ID, Cid...SHARED>
   struct Merging {
      using CTTI_Component = Yup;
      using CTTI_ReflectAs = void;
      using Id             = Values<ID, SHARED...>;

      static constexpr int  ComponentPrecedence = 3000;
      static constexpr bool Shared = sizeof...(SHARED) > 0;
      static constexpr bool AttemptConvertOnMerge = CONVERT;

   private:
      template<CT::Container C>
      using Count = typename C::CountType;
      template<CT::Container C>
      using Deep = typename C::DeepType;

   public:
      /// MARK: MergeAt                                                       
      /// Insert one or more elements at the specified position only if the   
      /// elements don't exist elsewhere. Supports intents and arrays.        
      ///   @param idx the index to insert at                                 
      ///   @param a1, an elements or arrays (and their intents) to merge     
      ///   @return the number of inserted elements (after any conversions)   
      template<class A1, class...AN, CT::IndexedLinearly C>
      auto MergeAt(this C& self, CT::Index auto&& idx, A1&& a1, AN&&...an) -> size_t {
         static_assert(CT::ContainsMany<C>,
            "Container should support multiple elements");

         if constexpr (CONVERT and not Same<TypeOf<C>, Deint<A1>, Deint<AN>...>) {
            // Conversion to C required.                                
            const size_t initial_count = self.GetCountInner();
            size_t offset = self.SimplifyIndex(idx);
            // ConvertMergeInner uses MergeRangeAt, so any exceptions   
            // will be handled there                                    
            ThisCom::ConvertMergeInner(offset, LglsFwd(a1));
           (ThisCom::ConvertMergeInner(offset, LglsFwd(an)), ...);
            return self.GetCountInner() - initial_count;
         }
         else {
            // No conversion required.                                  
            // Check all types, and gather the number of insertions.    
            ::std::vector<size_t> reduced;
            ThisCom::PrepareForMerging(FWDIntent(a1), reduced);
           (ThisCom::PrepareForMerging(FWDIntent(an), reduced), ...);
            if (reduced.empty())
               return 0;
            
            const size_t lhs_count = self.GetCount();
            if (lhs_count == 0) {
               self.AssertZeroIndex(idx);

               if (not self.IsDisowned() and self.GetUses() == 1) {
                  // This is empty, but preallocated                    
                  TODO();
               }
               else {
                  // This is empty and unallocated                      
                  self.AllocateFresh(reduced.size());
                  auto to = self.GetHandle();
                  size_t const* it = reduced.data();
                  size_t counter = 0;

                  try {
                     MergeInnerLinear(to, it, counter, LglsFwd(a1));
                    (MergeInnerLinear(to, it, counter, LglsFwd(an)), ...);
                  }
                  catch (...) {
                     // Account for throws inside constructors          
                     const size_t inserted = to - self.GetHandle();
                     TODO(); //TODO a gap remains, move things back
                     self.SetCountInner(inserted);
                     throw;
                  }
               }

               self.SetCountInner(reduced.size());
               return reduced.size();  
            }

            // Reallocate/branch out                                    
            const size_t all_count = lhs_count + reduced.size();
            const size_t offset    = self.SimplifyIndex(idx);

            if (not self.IsDisowned() and self.GetUses() == 1) {
               // No need to branch-out                                 
               self.AllocateMore(all_count);
               auto to = self.GetHandle() + offset;
               MakeGap(to, offset, lhs_count, reduced.size());
               size_t const* it = reduced.data();
               size_t counter = 0;

               try {
                  MergeInnerLinear(to, it, counter, LglsFwd(a1));
                 (MergeInnerLinear(to, it, counter, LglsFwd(an)), ...);
               }
               catch (...) {
                  // Account for throws inside constructors             
                  const size_t inserted = to - self.GetHandle();
                  TODO(); //TODO a gap (of only one element!!!) remains, move things back
                  self.SetCountInner(inserted);
                  throw;
               }
            }
            else {
               // We need to branch-out: insert old and new elements    
               // in another container, which we will later swap.       
               C temp {Disown{self}};
               temp.Reserve(all_count);
               auto src = self.GetHandle();
               auto dst = temp.GetHandle();
               size_t const* it = reduced.data();
               size_t counter = 0;

               // Copy original before 'offset'                         
               CopyRegion(src, dst, offset);

               // Copy new elements in the gap                          
               try {
                  MergeInnerLinear(dst, it, counter, LglsFwd(a1));
                 (MergeInnerLinear(dst, it, counter, LglsFwd(an)), ...);
               }
               catch (...) {
                  // Account for throws inside constructors             
                  const size_t inserted = dst - self.GetHandle();
                  TODO(); //TODO a gap remains, move things back
                  self.SetCountInner(inserted);
                  throw;
               }

               // Copy original after 'offset'                          
               CopyRegion(src, dst, lhs_count - offset);

               // Swap                                                  
               self.Swap(temp);
            }

            self.SetCountInner(all_count);
            return reduced.size();
         }
      }

      template<CT::IndexedLinearly C>
      auto MergeRangeAt(this C&, CT::Index auto, CT::Container auto&&)
         -> Count<C>;

      /// MARK: Merge                                                         
      /// Insert one or more elements at the performance-optimal position only
      /// if the elements don't exist elsewhere. This usually means at the    
      /// back of a contiguous container. Supports intents and arrays.        
      ///   @param a1, an elements (and their intents) to insert              
      ///   @return the number of inserted elements (after any conversions)   
      template<class A1, class...AN, class C>
      auto Merge(this C& self, A1&& a1, AN&&...an) -> size_t {
         static_assert(CT::ContainsMany<C>,
            "Container should support multiple elements");

         if constexpr (CONVERT and not Same<TypeOf<C>, Deint<A1>, Deint<AN>...>) {
            // Conversion to C required.                                
            const size_t initial_count = self.GetCountInner();
            size_t offset = initial_count;
            // ConvertInsertInner uses ConcatAt, so any exceptions will 
            // be handled there                                         
            ThisCom::ConvertMergeInner(offset, LglsFwd(a1));
           (ThisCom::ConvertMergeInner(offset, LglsFwd(an)), ...);
            return offset - initial_count;
         }
         else {
            // No conversion required.                                  
            // Gather the number of all elements and types.             
            // Empty containers can't change type. If one of the type   
            // changes raises a conflict, this function will throw.     
            ::std::vector<size_t> reduced;
            size_t counter = 0;
            ThisCom::PrepareForMerging(FWDIntent(a1), counter, reduced);
           (ThisCom::PrepareForMerging(FWDIntent(an), counter, reduced), ...);
            if (reduced.empty())
               return 0;
            
            // Reallocate/branch out                                    
            const size_t lhs_count = self.GetCount();
            const size_t all_count = lhs_count + reduced.size();
            self.BranchOut(all_count);
            
            // Insert the new                                           
            size_t const* it = reduced.data();
            counter = 0;

            if constexpr (CT::IndexedLinearly<C>) {
               // Insert as a sequence at the back of the memory        
               auto to = self.GetHandle().ForceMutable() + lhs_count;
               try {
                  MergeInnerLinear(to, it, counter, LglsFwd(a1));
                 (MergeInnerLinear(to, it, counter, LglsFwd(an)), ...);
               }
               catch (...) {
                  // Account for throws inside constructors             
                  const ptrdiff_t inserted = it - reduced.data();
                  self.SetCountInner(inserted);
                  throw;
               }
            }
            else {
               // Insert in a hash table                                
               try {
                  ThisCom::MergeInnerTable(it, counter, LglsFwd(a1));
                 (ThisCom::MergeInnerTable(it, counter, LglsFwd(an)), ...);
               }
               catch (...) {
                  // Account for throws inside constructors             
                  const ptrdiff_t inserted = it - reduced.data();
                  self.SetCountInner(inserted);
                  throw;
               }
            }

            self.SetCountInner(all_count);
            return reduced.size();
         }
      }

      template<CT::Container C>
      auto MergeRange(this C&, CT::Container auto&&) -> Count<C>;
      
   protected:
      /// MARK: PrepareForMerging                                             
      /// Helper function that gathers the number of elements and types.      
      /// 'a' is checked if already exists either in 'self' or in 'a' itself  
      /// if array. Only non-duplicates will be merged.                       
      ///   @attention operates in all relevant dimensions simultaneously     
      ///   @attention if `a` is a pointer that is being cloned, we can avoid 
      ///      searching for it altogether, because it is guaranteed          
      ///      that a new unique pointer will be generated, unless `nullptr`. 
      template<class C, CT::Intent A>
      void PrepareForMerging(this C& self, A&& a, size_t& counter, ::std::vector<size_t>& out_count) {
         if constexpr (CT::Handle<A>) {
            // Inserting handles                                        
            if constexpr (CT::Cloned<A>) {
               if constexpr (CT::TypeErased<A>) {
                  if (a->IsSparse()) {
                     auto ptr = a->template As<void*>();
                     if (ptr == nullptr and self.Contains(ptr)) {
                        ++counter;
                        return;
                     }
                  }
                  else if (self.Contains(a.what)) {
                     ++counter;
                     return;
                  }
               }
               else {
                  if constexpr (CT::Sparse<TypeOf<Deint<A>>>) {
                     auto ptr = a->template As<void*>();
                     if (ptr == nullptr and self.Contains(ptr)) {
                        ++counter;
                        return;
                     }
                  }
                  else {
                     if (self.Contains(a.what)) {
                        ++counter;
                        return;
                     }
                  }
               }
            }
            else {
               if (self.Contains(a.what)) {
                  ++counter;
                  return;
               }
            }

            self.AbsorbType(Copy(a));
            out_count.emplace_back(counter);
            ++counter;
         }
         else if constexpr (CT::Array<A>) {
            // Inserting array                                          
            auto contained_in_array_itself = [&](auto& i) -> bool {
               const ptrdiff_t idx = &i - a.what;
               for (ptrdiff_t it = 0; it < idx; ++it) {
                  if (a.what[it] == i)
                     return true;
               }
               return false;
            };

            for (auto& i : a.what) {
               if constexpr (CT::Cloned<A> and CT::Sparse<decltype(i)>) {
                  if (i == nullptr and (self.Contains(i) or contained_in_array_itself(i))) {
                     ++counter;
                     continue;
                  }
               }
               else {
                  if (self.Contains(i) or contained_in_array_itself(i)) {
                     ++counter;
                     continue;
                  }
               }
   
               self.DeduceType(i);
               out_count.emplace_back(counter);
               ++counter;
            }
         }
         else {
            // Inserting element                                        
            if constexpr (CT::Cloned<A> and CT::Sparse<A>) {
               if (a.what == nullptr and self.Contains(a.what)) {
                  ++counter;
                  return;
               }
            }
            else {
               if (self.Contains(a.what)) {
                  ++counter;
                  return;
               }
            }

            self.DeduceType(LglsFwd(a));
            out_count.emplace_back(counter);
            ++counter;
         }
      }
      
      /// MARK: ConvertMergeInner                                             
      template<class C, class T>
      void ConvertMergeInner(this C& self, size_t& at, T&& a) {
         using I  = IntentOf(a);
         using IT = DeextAll<Deint<I>>;
         static_assert(CONVERT and not Same<TypeOf<C>, IT>,
            "Use MergeInnerLinear instead");
      
         C converted;
         if constexpr (CT::Array<T>) {
            for (size_t i = 0; i < ExtentOf<T>; ++i)
               Langulus::Serialize(DeintCast(a)[i], converted);
         }
         else Langulus::Serialize(DeintCast(a), converted);

         const size_t offset = converted.GetCount();
         ThisCom::MergeRangeAt(at, Abandon {converted});
         at += offset;
      }

      /// MARK: MergeInnerLinear                                              
      /// A deeply unsafe function, that places 'a' at handle 'to'            
      /// and moves handle further. Supports T being a bounded array.         
      /// Does not perform conversion.                                        
      ///   @attention works in all dimensions at once                        
      template<CT::Handle H, class T>
      static void MergeInnerLinear(H& to, size_t const*& it, size_t& counter, T&& a) {
         using I  = IntentOf(a);
         using IT = DeextAll<Deint<T>>;
         static_assert(not CONVERT or Same<TypeOf<H>, IT>,
            "Use ConvertMergeInner instead");

         if constexpr (CT::Array<T>) {
            for (size_t i = 0; i < ExtentOf<T>; ++i) {
               if (*it == counter) {
                  Id::ForEach([&]<Cid D>{
                     if constexpr (CT::Copied<I>)
                        to.template EmplaceWithIntent<D>(Refer(DeintCast(a)[i]));
                     else
                        to.template EmplaceWithIntent<D>(I::Nest(DeintCast(a)[i]));
                  });

                  ++to;
                  ++it;   
               }
               
               ++counter;
            }
         }
         else {
            if (*it == counter) {
               Id::ForEach([&]<Cid D>{
                  if constexpr (CT::Copied<I>)
                     to.template EmplaceWithIntent<D>(Refer(LglsFwd(a)));
                  else
                     to.template EmplaceWithIntent<D>(FWDIntent(a));
               });

               ++to;
               ++it;   
            }
            
            ++counter;
         }
      }

      /// MARK: MergeInnerTable                                               
      /// Inserts 'a' into a hash map at the appropriate bucket. Supports T   
      /// being a bounded array. Does not perform conversion.                 
      ///   @attention works in all dimensions at once                        
      template<class C, class T>
      void MergeInnerTable(this C& self, size_t const*& it, size_t& counter, T&& a) {
         using I  = IntentOf(a);
         using IT = DeextAll<Deint<T>>;
         static_assert(not CONVERT or Same<TypeOf<C>, IT>,
            "Use ConvertMergeInnerTable instead"); //TODO function not implemented yet

         if constexpr (CT::Array<T>) {
            for (size_t i = 0; i < ExtentOf<T>; ++i) {
               if (*it == counter) {
                  decltype(auto) element = DeintCast(a)[i];
                  if constexpr (CT::Copied<I>)
                     self.TableEmplace(Refer(element));
                  else
                     self.TableEmplace(I::Nest(element));
                  ++it;
               }
               
               ++counter;
            }
         }
         else {
            if (*it == counter) {
               //decltype(auto) element = DeintCast(a);
               if constexpr (CT::Copied<I>)
                  self.TableEmplace(Refer(LglsFwd(a)));
               else
                  self.TableEmplace(FWDIntent(a));
               ++it;
            }
            
            ++counter;
         }
      }
   };

   #undef ThisCom
}
