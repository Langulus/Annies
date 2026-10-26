///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Container.hpp"
#include "Langulus/CT/Index.hpp"
#include "Langulus/CT/Serializer.hpp"
#include <Langulus/CT/NullTerminated.hpp>
#include "Langulus/IntentOf.hpp"
#include "Langulus/Typenav.hpp"
#include <cstddef>
#include <vector>


namespace Langulus::Annies::Component
{
   ///                                                                        
   /// Implements merging for containers.                                     
   /// Merging (unlike emplacement) extends the memory space and may move     
   /// things around. It guarantees that nothing gets overwritten.            
   /// Merging (unlike insertion) disallows for duplicated elements.          
   ///   @tparam CONVERT whether to serialize as self before merging item.    
   ///      Useful for byte and text containers. Use 'false' to merge without 
   ///      serialization.                                                    
   ///   @tparam ID, SHARED providers that share the same merge behavior.     
   ///   @attention this component will append a silent zero byte after each  
   ///      modification of the container, if container is CT::NullTerminated.
   template<bool CONVERT, Cid ID, Cid...SHARED>
   struct Merging {
      /// Refers back to this particular component instance through the       
      /// deduced 'this'. Just for convenience. It is #undef-ed at the end of 
      /// this file.                                                          
      #define ThisCom self.Merging<CONVERT, ID, SHARED...>

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
               }

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

                  if constexpr (CT::NullTerminated<C>)
                     memset(to.GetRaw(), 0, 1);
                  throw;
               }

               self.SetCountInner(reduced.size());
               if constexpr (CT::NullTerminated<C>)
                  memset(to.GetRaw(), 0, 1);
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

                  if constexpr (CT::NullTerminated<C>)
                     memset(to.GetRaw(), 0, 1);
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
                  
                  if constexpr (CT::NullTerminated<C>)
                     memset(dst.GetRaw(), 0, 1);
                  throw;
               }

               // Copy original after 'offset'                          
               CopyRegion(src, dst, lhs_count - offset);

               // Swap                                                  
               self.Swap(temp);
            }

            self.SetCountInner(all_count);
            if constexpr (CT::NullTerminated<C>)
               memset((self.GetHandle() + all_count).GetRaw(), 0, 1);
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
      ///   @attention this is for linearly indexed containers only           
      ///   @return the number of inserted elements (after any conversions)   
      template<class A1, class...AN, class C> requires (C::Dimensions::Count == 1)
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

                  if constexpr (CT::NullTerminated<C>)
                     memset(to.GetRaw(), 0, 1);
                  throw;
               }
            }
            else {
               // Insert in a hash table                                
               static_assert(not CT::NullTerminated<C>,
                  "Hash tables can't be null-terminated");

               try {
                  ThisCom::MergeInnerTable1D(it, counter, LglsFwd(a1));
                 (ThisCom::MergeInnerTable1D(it, counter, LglsFwd(an)), ...);
               }
               catch (...) {
                  // Account for throws inside constructors             
                  const ptrdiff_t inserted = it - reduced.data();
                  self.SetCountInner(inserted);
                  throw;
               }
            }

            self.SetCountInner(all_count);
            if constexpr (CT::NullTerminated<C>)
               memset((self.GetHandle() + all_count).GetRaw(), 0, 1);
            return reduced.size();
         }
      }

      /// MARK: Merge inside table                                            
      /// Insert first argument if the elements don't exist in the table yet. 
      /// Emplace the rest of the arguments in all shared dimensions. If only 
      /// first argument is available, default-initialize the rest, or fail.  
      /// Supports intents.                                                   
      ///   @param a1, an elements for each dimension, first being the key    
      ///   @return a multidimensional handle to the inserted/found entry     
      ///   @attention doesn't support arrays or any kind of unfolding        
      template<class A1, class...AN, CT::IndexedHashTable C> requires (C::Dimensions::Count > 1)
      auto Merge(this C& self, A1&& a1, AN&&...an) -> DecideHandle<C> {
         static_assert(CT::ContainsMany<C>,
            "Container should support multiple elements");
         static_assert(not CT::NullTerminated<C>,
            "Hash tables can't be null-terminated");

         auto found = self.template TableSearch<ID>(DeintCast(a1));
         if (found)
            return found;
         
         // Reallocate/branch out                                       
         const size_t lhs_count = self.GetCount();
         const size_t all_count = lhs_count + 1;
         self.BranchOut(all_count);
         
         // Insert in a hash table                                      
         found = ThisCom::MergeInnerTableMD(LglsFwd(a1), LglsFwd(an)...);
         self.SetCountInner(all_count);
         return found;
      }

      /// MARK: Merge every entry of one table into another                   
      /// Insert each element if the elements don't exist in the table yet.   
      /// Nothing will get inserted, if the first dimension already exists.   
      /// Supports intents.                                                   
      ///   @param other container to merge                                   
      ///   @return the number of inserted elements                           
      template<CT::Container C>
      auto MergeRange(this C& self, CT::Container auto&& other) -> size_t {
         const size_t prev_size = self.GetCount();
         DeintCast(other).Apply([&](auto& h) {
            self.Merge(NestIntentOf(other, h));
         });
         return self.GetCount() - prev_size;
      }
      
      /// MARK: Merge every entry of one table into another                   
      /// Insert each element if the elements don't exist in the table yet.   
      /// If the first dimension already exists, the conflict resolver will   
      /// be executed. Supports intents.                                      
      ///   @param other container to merge                                   
      ///   @param conflict custom merge routine in case key already exists   
      ///      Example: [](auto&& old_value, auto&& new_value) { ... }        
      ///   @return the number of inserted elements                           
      template<CT::Container C> requires (C::Dimensions::Count > 1)
      auto MergeRange(this C& self, CT::Container auto&& other, auto&& conflict) -> size_t {
         const size_t prev_size = self.GetCount();
         DeintCast(other).Apply([&](auto& h) {
            auto found = self.Find(*h.GetKeyHandle());
            if (found)
               conflict(*found.GetValHandle(), NestIntentOf(other, *h.GetValHandle()));
            else
               self.Merge(NestIntentOf(other, h));
         });
         return self.GetCount() - prev_size;
      }
      
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
                  if constexpr (CT::Sparse<TypeOf<A>>) {
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

      /// MARK: MergeInnerTable1D                                             
      /// Inserts 'a' into a hash set at the appropriate bucket. Supports T   
      /// being a bounded array. Does not perform conversion.                 
      ///   @attention designed to work in one-dimensional containers         
      template<class C, class T> requires (C::Dimensions::Count == 1)
      void MergeInnerTable1D(this C& self, size_t const*& it, size_t& counter, T&& a) {
         using I  = IntentOf(a);
         using IT = DeextAll<Deint<T>>;
         static_assert(not CONVERT or Same<TypeOf<C>, IT>,
            "Use ConvertMergeInnerTable1D instead"); //TODO function not implemented yet

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

      /// MARK: MergeInnerTableMD                                             
      /// Inserts 'd1' into a hash map at the appropriate bucket. Emplaces    
      /// all 'dn' under the same bucket, in each dimension. Dimensions with  
      /// missing argument are default-initialized, or error out. Does not    
      /// perform conversion.                                                 
      ///   @attention operates on all dimensions at once                     
      template<class C, class D1, class...DN> requires (C::Dimensions::Count > 1)
      auto MergeInnerTableMD(this C& self, D1&& d1, DN&&...dn) -> DecideHandle<C> {
         static_assert(sizeof...(SHARED) >= sizeof...(DN), "Too many arguments");
         static_assert(not CONVERT or Same<TypeOf<C, ID>, DeextAll<Deint<D1>>>,
            "Use ConvertMergeInnerTableMD instead" //TODO function not implemented yet
         ); 

         size_t offset;
         if constexpr (CT::Copied<IntentOf(d1)>)
            offset = self.TableEmplace(Refer(LglsFwd(d1)));
         else
            offset = self.TableEmplace(FWDIntent(d1));

         auto h = self.GetHandle() + offset;
         constexpr size_t overlapped_count
            = sizeof...(DN) < sizeof...(SHARED) ? sizeof...(DN)
                                                : sizeof...(SHARED);

         // Emplace available                                           
         LglsSequence(overlapped_count, {
            static_assert(not CONVERT
               or ((Same<TypeOf<C, Id::template At<I+1>>, DeextAll<Deint<DN>>>) and ...),
               "Use ConvertMergeInnerTableMD instead" //TODO function not implemented yet
            );
            (h.template EmplaceWithIntent<Id::template At<I+1>>(FWDIntent(dn)), ...);
         });

         // Default the rest                                            
         LglsSequence(sizeof...(SHARED) - overlapped_count, {
            (h.template EmplaceDefault<Id::template At<I+1+overlapped_count>, AllocationStrategy::DontAllocate>(), ...);
         });
         return h;
      }
   };

   #undef ThisCom
}
