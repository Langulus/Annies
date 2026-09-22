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
#include "Langulus/IntentOf.hpp"
#include "Langulus/Typenav.hpp"
#include "Langulus/Utils/Types.hpp"
#include <Langulus/CT/Unfold.hpp>
#include <Langulus/CT/Index.hpp>
#include <Langulus/CT/ReflectAs.hpp>
#include <Langulus/CT/Contiguous.hpp>
#include <Langulus/CT/Nullable.hpp>
#include <Langulus/CT/Defaultable.hpp>
#include <Langulus/CT/Serializer.hpp>
#include <Langulus/CT/Deep.hpp>
#include <type_traits>


namespace Langulus::Annies::Component
{
   /// Refers back to this particular component instance through the deduced  
   /// 'this'. Just for convenience. It is #undef-ed at the end of this file. 
   #define ThisCom self.Insertion<CONVERT, ID, SHARED...>

   ///                                                                        
   /// Implements insertion for containers.                                   
   /// Insertion (unlike emplacement) extends the memory space and may move   
   /// things around. It guarantees that nothing gets overwritten.            
   /// Insertion (unlike merging) allows for duplicated elements. That's why  
   /// this component is omitted for containers like Map and Set.             
   /// Supports five kinds of insertion:                                      
   ///   1. Insert/InsertAt - conventional insertion, where the container is  
   ///      expanded to incorporate the new elements at the desired position. 
   ///   2. Concat/ConcatAt - concatenation that inserts the contents of      
   ///      any container at the desired position, disregarding any state     
   ///      differences.                                                      
   ///   3. Compose/ComposeAt - a structure-preserving insertion that respects
   ///      states and disownment in order to form more complex containers.   
   ///   4. And/AndAt - an or-preserving insertion that deepens the           
   ///      container if `IsOr() == true`, and then inserts the new content.  
   ///   5. Or/OrAt - an or-preserving insertion that deepens the             
   ///      container if `IsOr() == false`, and then inserts the new content. 
   ///   @tparam CONVERT whether to serialize as self before inserting item.  
   ///      Useful for byte and text containers. Use 'false' to insert without
   ///      serialization.                                                    
   ///   @tparam ID, SHARED providers that share the same insertion behavior. 
   template<bool CONVERT, Cid ID, Cid...SHARED>
   struct Insertion {
      using CTTI_Component = Yup;
      using CTTI_ReflectAs = void;
      using Id             = Values<ID, SHARED...>;

      static constexpr bool AttemptConvertOnInsert = CONVERT;
      static constexpr int ComponentPrecedence = 3000;

   private:
      template<CT::Container C>
      using Deep  = typename Deref<C>::DeepType;
      template<CT::Container C>
      using State = typename Deref<C>::StateType;
      template<CT::Container C>
      using PickRangeMut = typename Deref<C>::PickRangeMut;

      template<CT::Handle H>
      static void MakeGap(H const& start, size_t offset, size_t lhs_count, size_t rhs_count) {
         if (offset >= lhs_count)
            return;
      
         // We're moving to the right, so make sure we do it in         
         // reverse to avoid any potential overlap                      
         //TODO batch optimization for PODs
         const size_t moved = lhs_count - offset;
         auto src = start + (moved - 1);
         auto dst = src + rhs_count;
         auto const end = (src - moved).GetRaw();
         while (src.GetRaw() != end) {
            Id::ForEach([&]<Cid D>{
               dst.template EmplaceWithIntent<D>(Abandon {src});
            });
            --src; --dst;
         }
      }

      template<CT::Handle H1, CT::Handle H2>
      static void CopyRegion(H1& src, H2& dst, size_t count) {
         //TODO batch optimization for PODs
         auto const end = (DeintCast(src) + count).GetRaw();
         while (src.GetRaw() != end) {
            Id::ForEach([&]<Cid D>{
               dst.template EmplaceWithIntent<D>(Refer(src));
            });
            ++dst; ++src;
         }
      }
      
      template<CT::Container C1, CT::Handle H2>
      static void CopyContainer(C1&& src_container, H2& dst) {
         //TODO batch optimization for PODs
         using I = IntentOf(src_container);
         auto src = DeintCast(src_container).GetHandle();
         auto const end = DeintCast(src_container).GetRawEnd();
         while (src.GetRaw() != end) {
            Id::ForEach([&]<Cid D>{
               if constexpr (CT::Copied<I>)
                  dst.template EmplaceWithIntent<D>(Refer(src));
               else
                  dst.template EmplaceWithIntent<D>(I::Nest(src));
            });
            ++dst; ++src;
         }
      }

   public:
      /// MARK: InsertAt                                                      
      /// Insert one or more elements at the specified position. Supports     
      /// intents and arrays.                                                 
      /// Insertion, unlike concatenation, may attempt to convert arguments   
      /// (if AS is specified), or deepen the container, in order to be able  
      /// to insert.                                                          
      ///   @tparam FORCE if true, the container is allowed to deepen in      
      ///      order to incorporate elements of different types. Otherwise    
      ///      a compile-time or runtime exception will be thrown, if an      
      ///      incompatible type is encountered.                              
      ///   @attention If FORCE is enabled, and the index is somewhere in     
      ///      the middle of the container, it will be split in more than 2   
      ///      deep containers, so that insertion order is preserved.         
      ///   @param idx the index to insert at                                 
      ///   @param a1, an elements or arrays (and their intents) to insert    
      ///   @return the number of inserted elements (after any conversions)   
      template<bool FORCE = true, class A1, class...AN, CT::IndexedLinearly C>
      auto InsertAt(this C& self, CT::Index auto&& idx, A1&& a1, AN&&...an) -> size_t {
         static_assert(CT::ContainsMany<C>,
            "Container should support multiple elements");

         if constexpr (CONVERT
         and not Same<TypeOf<C>, DeextAll<Deint<A1>>, DeextAll<Deint<AN>>...>) {
            // Conversion to AS required.                               
            const size_t initial_count = self.GetCountInner();
            size_t offset = self.SimplifyIndex(idx);
            // ConvertInsertInner uses ConcatAt, so any exceptions will 
            // be handled there                                         
            ThisCom::ConvertInsertInner(offset, LglsFwd(a1));
           (ThisCom::ConvertInsertInner(offset, LglsFwd(an)), ...);
            return self.GetCountInner() - initial_count;
         }
         else {
            // No conversion required.                                  
            // Check all types, and gather the number of insertions.    
            bool deepened = false;
            size_t rhs_count = 0;
            ThisCom::PrepareForInsertion(LglsFwd(a1), rhs_count, deepened);
           (ThisCom::PrepareForInsertion(LglsFwd(an), rhs_count, deepened), ...);
            if (not rhs_count)
               return 0;
            
            const size_t lhs_count = self.GetCount();
            if (lhs_count == 0) {
               self.AssertZeroIndex(idx);

               if (not self.IsDisowned() and self.GetUses() == 1 and not deepened) {
                  // This is empty, but preallocated                    
                  TODO();
               }
               else {
                  // This is empty and unallocated                      
                  self.AllocateFresh(rhs_count);
                  auto to = self.GetHandle();

                  try {
                     InsertInner(to, LglsFwd(a1));
                    (InsertInner(to, LglsFwd(an)), ...);
                  }
                  catch (...) {
                     // Account for throws inside constructors          
                     const size_t inserted = to - self.GetHandle();
                     TODO(); //TODO a gap remains, move things back
                     self.SetCountInner(inserted);
                     throw;
                  }
               }

               self.SetCountInner(rhs_count);
               return rhs_count;  
            }

            // Reallocate/branch out                                    
            const size_t all_count = lhs_count + rhs_count;
            const size_t offset    = self.SimplifyIndex(idx);

            if (not self.IsDisowned() and self.GetUses() == 1 and not deepened) {
               // No need to branch-out                                 
               self.AllocateMore(all_count);
               auto to = self.GetHandle() + offset;
               MakeGap(to, offset, lhs_count, rhs_count);

               try {
                  InsertInner(to, LglsFwd(a1));
                 (InsertInner(to, LglsFwd(an)), ...);
               }
               catch (...) {
                  // Account for throws inside constructors             
                  const size_t inserted = to - self.GetHandle();
                  TODO(); //TODO a gap remains, move things back
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

               // Copy original before 'offset'                         
               CopyRegion(src, dst, offset);

               // Copy new elements in the gap                          
               try {
                  InsertInner(dst, LglsFwd(a1));
                 (InsertInner(dst, LglsFwd(an)), ...);
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
            return rhs_count;
         }
      }

      /// Insert a number of elements at a specific place, nullifying them if 
      /// able to                                                             
      template<CT::IndexedLinearly C>
      auto InsertNulledAt(this C&, CT::Index auto, size_t) -> size_t;

      /// Insert a number of elements at a specific place, default-           
      /// constructing them if able to                                        
      template<CT::IndexedLinearly C>
      auto InsertDefaultAt(this C&, CT::Index auto, size_t) -> size_t;

      /// MARK: ComposeAt                                                     
      /// Structure-preserving insertion that uses the best approach in order 
      /// to keep current hierarchy and states, but also reuse memory.        
      ///   @attention any disowned data will remain disowned                 
      ///   @tparam CONCAT whether or not concatenation is allowed            
      ///   @tparam FORCE insert even if types mismatch by deepening          
      ///   @param index the index at which to insert (if needed)             
      ///   @param value the value to smart-push                              
      ///   @param state a state to apply after pushing is done               
      ///   @return the number of pushed items (zero if unsuccessful)         
      template<bool CONCAT = true, bool FORCE = true, CT::IndexedLinearly C>
      auto ComposeAt(
         this C& self, CT::Index auto&& idx, auto&& value
      ) -> size_t {
         using I = IntentOf(value);
         using T = Deint<I>;
         auto& other = DeintCast(value);
         const bool stateCompliant = ThisCom::CheckState(other);
   
         if constexpr (CT::DeepDense<T>) {
            // We're inserting a deep item, so we can do various smart  
            // things before inserting, like absorbing and concatenating
            if (not other.IsValid())
               return 0;
   
            if (stateCompliant) {
               if (self.IsEmpty()) {
                  // We can directly absorb                             
                  self.AssignAbsorb(LglsFwd(value));
                  return 1;
               }
      
               if constexpr (CONCAT) {
                  // We are allowed to attempt concatenatenation        
                  if (ThisCom::ConcatAt(LglsFwd(idx), LglsFwd(value)))
                     return 1;
               }
            }
         }
   
         // If reached, then none of the above succeeded - just push.   
         // If FORCE is enabled, this will deepen in order to preserve  
         // state, in the case there's conflict.                        
         return ThisCom::template ComposeInner<FORCE>(
            stateCompliant, LglsFwd(idx), LglsFwd(value)
         );
      }

      /// MARK: Insert                                                        
      /// Insert one or more elements at the performance-optimal position.    
      /// This usually means at the back of a contiguous container. Supports  
      /// intents and arrays.                                                 
      /// Insertion, unlike concatenation, may attempt to convert arguments   
      /// (if AS is specified), or deepen the container, in order to be able  
      /// to insert.                                                          
      ///   @tparam FORCE if true, the container is allowed to deepen in      
      ///      order to incorporate elements of different types. Otherwise    
      ///      a compile-time or runtime exception will be thrown, if an      
      ///      incompatible type is encountered.                              
      ///   @param a1, an elements (and their intents) to insert              
      ///   @return the number of inserted elements (after any conversions)   
      template<bool FORCE = true, class A1, class...AN, CT::IndexedLinearly C>
      auto Insert(this C& self, A1&& a1, AN&&...an) -> size_t {
         static_assert(CT::ContainsMany<C>,
            "Container should support multiple elements");

         if constexpr (CONVERT and not Same<TypeOf<C>, Deint<A1>, Deint<AN>...>) {
            // Conversion to AS required.                               
            const size_t initial_count = self.GetCountInner();
            size_t offset = initial_count;
            // ConvertInsertInner uses ConcatAt, so any exceptions will 
            // be handled there                                         
            ThisCom::ConvertInsertInner(offset, LglsFwd(a1));
           (ThisCom::ConvertInsertInner(offset, LglsFwd(an)), ...);
            return offset - initial_count;
         }
         else {
            // No conversion required.                                  
            // Gather the number of all elements and types.             
            // Empty containers can't change type. If one of the type   
            // changes raises a conflict, this function will throw.     
            bool deepened = false;
            size_t rhs_count = 0;
            ThisCom::PrepareForInsertion(LglsFwd(a1), rhs_count, deepened);
           (ThisCom::PrepareForInsertion(LglsFwd(an), rhs_count, deepened), ...);
            if (not rhs_count)
               return 0;
            
            // Reallocate/branch out                                    
            const size_t lhs_count = self.GetCount();
            const size_t all_count = lhs_count + rhs_count;
            self.BranchOut(all_count);
            
            // Insert the new                                           
            auto to = self.GetHandle() + lhs_count;
            try {
               InsertInner(to, LglsFwd(a1));
              (InsertInner(to, LglsFwd(an)), ...);
            }
            catch (...) {
               // Account for throws inside constructors                
               const size_t inserted = to - self.GetHandle();
               self.SetCountInner(inserted);
               throw;
            }

            self.SetCountInner(all_count);
            return rhs_count;
         }
      }

      /// Insert a number of elements at the performance-optimal position.    
      /// This usually means at the back of a contiguous container. The       
      /// inserted elements will be nullified.                                
      ///   @param count the number of elements to insert                     
      template<CT::IndexedLinearly C>
      auto InsertNulled(this C&, size_t count) -> size_t;

      /// MARK: InsertDefault                                                 
      /// Insert a number of elements at the performance-optimal position.    
      /// This usually means at the back of a contiguous container. The       
      /// inserted elements will be default-constructed.                      
      ///   @param count the number of elements to insert                     
      template<CT::IndexedLinearly C>
      auto InsertDefault(this C& self, size_t count) -> size_t {
         const auto previousCount = self.GetCount();
         self.AllocateMore(previousCount + count);

         if constexpr (not C::TypeErased) {
            using T = TypeOf<C>;
            if constexpr (CT::Nullable<T>) {
               // Zero the dense memory (optimization)                  
               memset(self.GetRaw() + previousCount, 0, count * sizeof(T));
            }
            else if constexpr (CT::Defaultable<T>) {
               // Construct requested elements one by one               
               auto to = self.GetRaw() + previousCount;
               const auto toEnd = to + count;
               try {
                  while (to != toEnd) {
                     new (to) T {};
                     ++to;
                  }
               } catch (...) {
                  // Partial success                                    
                  const auto constructed = to - self.GetRaw();
                  self.SetCountInner(previousCount + constructed);
                  throw;
               }
            }
            else static_assert(false,
               "Trying to default-construct elements that are "
               "incapable of default-construction/nullification"
            );
         }
         else {
            const auto T = self.GetType();
            if (T.IsNullable()) {
               // Zero the dense memory (optimization)                  
               const auto stride = T.GetSize();
               memset(
                  self.template GetRawAs<uint8_t>() + previousCount * stride,
                  0, count * stride
               );
            }
            else {
               const auto defaultConstructor = T.GetDefaultConstructor();
               LglsAssert(defaultConstructor,
                  "Can't default-construct elements"
                  " - no default constructor/nullification reflected"
               );

               // Construct requested elements one by one               
               const auto stride = T.GetSize();
               auto to = self.template GetRawAs<uint8_t>() + previousCount * stride;
               const auto toEnd = to + count * stride;
               try {
                  while (to != toEnd) {
                     defaultConstructor(to);
                     to += stride;
                  }
               } catch (...) {
                  // Partial success                                    
                  const auto constructed = (to - self.template GetRawAs<uint8_t>()) / stride;
                  self.SetCountInner(previousCount + constructed);
                  throw;
               }
            }
         }

         // Complete success                                            
         self.SetCountInner(previousCount + count);
         return count;
      }

      /// MARK: Compose                                                       
      /// Structure-preserving insertion that uses the best approach in order 
      /// to keep current hierarchy and states, but also reuse memory as much 
      /// as possible.                                                        
      ///   @attention any disowned data will remain disowned                 
      ///   @tparam CONCAT whether or not concatenation is allowed            
      ///   @tparam FORCE insert even if types mismatch by deepening          
      ///   @param value the value to insert                                  
      ///   @return the number of pushed items (zero if unsuccessful)         
      template<bool CONCAT = true, bool FORCE = true, CT::IndexedLinearly C>
      auto Compose(this C& self, auto&& value) -> size_t {
         using I = IntentOf(value);
         using T = Deint<I>;
         auto& other = DeintCast(value);
         const bool stateCompliant = ThisCom::CheckState(other);
   
         if constexpr (CT::DeepDense<T>) {
            // We're inserting a deep item, so we can do various smart  
            // things before inserting, like absorbing and concatenating
            if (not other.IsValid())
               return 0;
   
            if (stateCompliant) {
               if (self.IsEmpty()) {
                  // We can directly absorb                             
                  self.AssignAbsorb(LglsFwd(value));
                  return 1;
               }
      
               if constexpr (CONCAT) {
                  // We are allowed to attempt concatenatenation        
                  if (ThisCom::Concat(LglsFwd(value)))
                     return 1;
               }
            }
         }
   
         // If reached, then none of the above succeeded - just push.   
         // If FORCE is enabled, this will deepen in order to preserve  
         // state, in the case there's conflict.                        
         return ThisCom::template ComposeInner<FORCE>(
            stateCompliant, Index::Back, LglsFwd(value)
         );
      }

      /// MARK: Deepen                                                        
      /// Wrap all contained elements inside a sub-block                      
      ///   @return a reference to the newly created inner container          
      template<CT::Deep C>
      auto Deepen(this C& self) -> C& {
         LglsAssert(not self.IsTypeConstrained() or self.template IsSame<C>(),
            "Can't deepen with incompatible type");
      
         // Allocate a new container and move this one inside of it     
         C temp {Piecewise, Abandon {self}};
         self.Swap(temp);
         return *self.template Get<C>();
      }

      /// Extend the container's memory and return the newly allocated range  
      ///   @attention if extending memory without ownership, the container   
      ///      will diverge into a new allocation and copy the data           
      ///   @param count the number of elements to extend by                  
      ///   @param arguments the arguments to use for each constructor call   
      ///      - no arguments will result in default construction             
      ///   @return the newly allocated mutable range                         
      /*template<CT::Container C, class...A>
      auto Extend(this C& self, Count<C> count = 1, A&&...arguments)
      -> Decay<C> { // PickRangeMut<C>
         const auto previousCount = self.GetCount();
         if constexpr (sizeof...(A) == 0)
            self.InsertDefault(count);
         else if (count == 1)
            self.InsertConstruct(LglsFwd(arguments)...);
         else {
            LglsAssert(
               ((not IntentOfT<decltype(arguments)>::IsMoved()) and ...),
               "Can't use move semantics here - "
               "the arguments need to be reused multiple times"
            );
            
            self.AllocateMore(previousCount + count);
            for (Count<C> i = 0; i < count; i++)
               self.InsertConstruct(LglsFwd(arguments)...); //TODO this is a pretty slow way to batch-insert, lots of overhead
         }
         return self.SelectInner(previousCount, count);
      }*/

      
      /// MARK: ConcatAt                                                      
      /// Concatenation at specific index.                                    
      /// Unlike InsertAt, concatenation always inserts the contents of the   
      /// argument containers one by one, without conversion.                 
      /// Possible only for contiguous containers with multiple elements.     
      ///   @param idx the index to insert at                                 
      ///   @param a1_intent, an_intent the containers to insert at the given 
      ///      position                                                       
      ///   @return the number of concatenated elements                       
      template<CT::IndexedLinearly C, CT::IndexedLinearly T>
      auto ConcatAt(this C& self, CT::Index auto&& idx, T&& data) -> size_t {
         static_assert(CT::ContainsMany<C>,
            "Container should support multiple elements");

         const size_t rhs_count = DeintCast(data).GetCount();
         if (not rhs_count)
            return 0;
         
         const size_t lhs_count = self.GetCount();
         if (lhs_count == 0) {
            self.AssertZeroIndex(idx);

            if (not self.IsDisowned() and self.GetUses() == 1) {
               // This is empty, but preallocated                       
               if (rhs_count > self.GetReserved())
                  self.AllocateMore(rhs_count);
            }
            else {
               // This is empty and unallocated                         
               self.AllocateFresh(rhs_count);
            }
            
            auto dst = self.GetHandle();
            auto src = DeintCast(data).GetHandle();
            try {
               CopyRegion(src, dst, rhs_count);
            }
            catch (...) {
               // Account for throws inside constructors                
               const size_t inserted = dst - self.GetHandle();
               TODO(); //TODO a gap remains, move things back
               self.SetCountInner(inserted);
               throw;
            }

            self.SetCountInner(rhs_count);
            return rhs_count;  
         }

         // Reallocate/branch out                                       
         const size_t all_count = lhs_count + rhs_count;
         const size_t offset    = self.SimplifyIndex(idx);

         if (not self.IsDisowned() and self.GetUses() == 1) {
            // No need to branch-out                                    
            self.AllocateMore(all_count);
            auto dst = self.GetHandle() + offset;
            MakeGap(dst, offset, lhs_count, rhs_count);
            auto src = DeintCast(data).GetHandle();
            try {
               CopyRegion(src, dst, rhs_count);
            }
            catch (...) {
               // Account for throws inside constructors                
               const size_t inserted = dst - self.GetHandle();
               TODO(); //TODO a gap remains, move things back
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

            // Copy original before 'offset'                            
            CopyRegion(src, dst, offset);

            // Copy new elements in the gap                             
            try {
               CopyRegion(src, dst, rhs_count);
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
         return rhs_count;
      }

      /// MARK: Concat                                                        
      /// Concatenation at the back.                                          
      /// Unlike Insert, concatenation always inserts the contents of the     
      /// argument containers one by one, without conversion.                 
      /// Possible only for contiguous containers with multiple elements.     
      ///   @param a1_intent, an_intent the containers to concatenate to the  
      ///      right of 'this'                                                
      ///   @return the number of concatenated elements                       
      template<CT::IndexedLinearly C, CT::Container A1, CT::Container...AN>
      auto Concat(this C& self, A1&& a1_intent, AN&&...an_intent) -> size_t {
         static_assert(CT::ContainsMany<C>,
            "Container should support multiple elements");

         // Gather the number of all elements and types.                
         // Empty containers can't change type. If one of the type      
         // changes raises a conflict, this function will throw.        
         size_t rhs_count = 0;
          ThisCom::PrepareForAbsorption(LglsFwd(a1_intent), rhs_count);
         (ThisCom::PrepareForAbsorption(LglsFwd(an_intent), rhs_count), ...);
         if (not rhs_count)
            return 0;
         
         // Reallocate/branch out                                       
         const size_t lhs_count = self.GetCount();
         const size_t all_count = lhs_count + rhs_count;
         self.BranchOut(all_count);
         
         // Concatenate all new containers                              
         auto to = self.GetHandle() + lhs_count;
         try {
            CopyContainer(LglsFwd(a1_intent), to);
           (CopyContainer(LglsFwd(an_intent), to), ...);
         }
         catch (...) {
            // Account for throws inside constructors                   
            const size_t inserted = to - self.GetHandle();
            self.SetCountInner(inserted);
            throw;
         }

         self.SetCountInner(all_count);
         return rhs_count;
      }
         
      /// Gather items from source container, and fill this one               
      ///   @param source container to gather from, type acts as filter       
      ///   @return the number of gathered elements                           
      template<CT::Container C, CT::Container SRC>
      size_t GatherFrom(this C& self, SRC&& source) {
         return ThisCom::template GatherInner<false>(FWDIntent(source));
      }
   
      /// Same as above, but in reverse                                       
      template<CT::Container C, CT::Container SRC>
      size_t GatherFromReverse(this C& self, SRC&& source) {
         return ThisCom::template GatherInner<true>(FWDIntent(source));
      }
   
      /// Gather items of specific state from source container, and fill this 
      ///   @tparam D custom deep type to nest in. WIll use C::DeepType if    
      ///      not specified or void                                          
      ///   @param source container to gather from, type acts as filter       
      ///   @param state state filter                                         
      ///   @return the number of gathered elements                           
      template<class D = void, CT::Container C, CT::Container SRC>
      size_t GatherFrom(this C& self, SRC&& source, auto&& condition) {
         using DEEP = Tif<::std::is_void_v<D>, typename C::DeepType, D>;
         return ThisCom::template GatherPolarInner<false, DEEP>(
            FWDIntent(source), LglsFwd(condition)
         );
      }

      /// Same as above, but in reverse                                       
      template<class D = void, CT::Container C, CT::Container SRC>
      size_t GatherFromReverse(this C& self, SRC&& source, int state) {
         using DEEP = Tif<::std::is_void_v<D>, typename C::DeepType, D>;
         return ThisCom::template GatherPolarInner<true, DEEP>(
            FWDIntent(source), state
         );
      }
   
   protected:
      /// MARK: Protected                                                     
      /// Gather items from source container, and fill this one.              
      /// Local type acts as a filter to what gets gathered.                  
      ///   @tparam REVERSE are we gathering in reverse?                      
      ///   @tparam D custom deep type to nest in. No nesting will happen if  
      ///      void                                                           
      ///   @param source source container and intent                         
      ///   @return the number of gathered elements                           
      template<bool REVERSE, class D, CT::Container C, CT::Container SRC> requires CT::Intent<SRC>
      size_t GatherInner(this C& self, SRC&& source) {
         if constexpr (not ::std::is_void_v<D>) {
            static_assert(CT::Deep<D> and CT::Decayed<D>, "D is not deep");
            if (source->template Is<D>() and not self.IsDeep()) {
               // Gather from each subcontainer, regardless if sparse   
               // or not                                                
               size_t count = 0;
               source.Apply([&](auto& i) {//TODO reverse?
                  count += ThisCom::template GatherInner<REVERSE>(NestIntentOf(source, *i.GetDense().template Get<D>()));
               });
               return count;
            }
         }

         //TODO why all these checks? can't we just Concat and disregard any exceptions?
         if constexpr (not CT::TypeErased<C>) {
            // Output container is strictly typed, we can't make loose  
            // matches                                                  
            if (source->IsSame(self))
               return ThisCom::Concat(LglsFwd(source));
            else
               return 0;
         }
         else {
            if (self.IsTypeConstrained()) {
               // Output container is strictly typed, we can't make     
               // loose matches                                         
               if (source->IsSame(self))
                  return ThisCom::Concat(LglsFwd(source));
               else
                  return 0;
            }
            else {
               // Output is not strictly typed, so we can afford a      
               // looser comparison                                     
               return ThisCom::Concat(LglsFwd(source));
            }
         }
      }
   
      /// Gather items of specific phase from input container and fill output 
      ///   @tparam REVERSE are we gathering in reverse?                      
      ///   @tparam D custom deep type to nest in. No nesting will happen if  
      ///      void                                                           
      ///   @param source source container and intent                         
      ///   @param state the data state filter                                
      ///   @return the number of gathered elements                           
      template<bool REVERSE, class D, CT::Container C, CT::Container SRC> requires CT::Intent<SRC>
      size_t GatherPolarInner(this C& self, SRC&& source, auto&& condition) {
         if (not condition(*source)) {
            if constexpr (not ::std::is_void_v<D>) {
               static_assert(CT::Deep<D> and CT::Decayed<D>, "D is not deep");  
               if (source->IsNow() and source->template Is<D>()) {
                  // States don't match, but we can dig deeper if Deep  
                  // and Now, since Now state is permissive, regardless 
                  // if sparse or not.                                  
                  C localOutput;
                  localOutput.SetType(self.GetType());
                  localOutput.SetState(source->GetUnconstrainedState());
                  source.Apply([&](auto& i) {//TODO reverse?
                     localOutput.template GatherPolarInner<REVERSE, D>(NestIntentOf(source, *i.GetDense().template Get<D>()), condition);
                  });
                  localOutput.MakeNow();
                  return ThisCom::Compose(Abandon(localOutput));
               }
            }
   
            // State mismatch                                           
            return 0;
         }
   
         //                                                             
         // If reached, then source is flat and neutral/same            
         if (not self.IsTyped()) {
            // Any output will do, so no need to iterate at all         
            return ThisCom::Compose(LglsFwd(source));
         }
   
         // Iterate subpacks if any                                     
         C localOutput;
         localOutput.SetType(self.GetType());
         localOutput.SetState(source->GetState());
         localOutput.template GatherInner<REVERSE, D>(LglsFwd(source));
         localOutput.MakeNow();
         return ThisCom::Concat(Abandon(localOutput));
      }

      /// Helper function that gathers the number of elements and types.      
      /// An incompatible type will result in 'deepened' being true, and      
      /// 'out_count' being rewritten to reflect the number of required sub-  
      /// containers.                                                         
      ///   @attention operates in all relevant dimensions simultaneously     
      template<CT::IndexedLinearly C, class A>
      void PrepareForInsertion(this C& self, A&& a, size_t& out_count, bool& deepened) {
         if constexpr (CT::Handle<A>) {
            // Inserting handles                                        
            if (not deepened) {
               try {
                  self.AbsorbType(Copy(a));
               } catch(...) {
                  deepened = true;
                  out_count = 2;
                  return;
               }
               ++out_count;
            }
            else ++out_count;
         }
         else {
            // Inserting element or an array                            
            if (not deepened) {
               try {
                  self.DeduceType(LglsFwd(a));
               } catch(...) {
                  deepened = true;
                  out_count = 2;
                  return;
               }
               out_count += GetAllExtentsOf(a);
            }
            else ++out_count;
         }
      }

      /// Helper function that gathers the number of elements and types.      
      /// Empty containers can't change this container's type. If one of the  
      /// type changes raises a conflict the function will throw.             
      ///   @attention operates in all relevant dimensions simultaneously     
      template<CT::IndexedLinearly C, CT::Container A>
      void PrepareForAbsorption(this C& self, A&& a, size_t& out_count) {
         const auto c = DeintCast(a).GetCount();
         if (not c)
            return;

         self.AbsorbType(Copy(a));
         out_count += c;
      }

      /// MARK: ConvertInsertInner                                            
      template<CT::IndexedLinearly C, class T>
      void ConvertInsertInner(this C& self, size_t& at, T&& a) {
         using I  = IntentOf(a);
         using IT = DeextAll<Deint<I>>;
         static_assert(CONVERT and not Same<TypeOf<C>, IT>,
            "Use InsertInner instead");
   
         // Convert all arguments and then concatenate the results      
         C converted;
         if constexpr (requires { self.GetDictionary(); }) {
            auto dictionary = self.GetDictionary();
            if (not dictionary) {
               self.Reserve(1);
               dictionary = self.GetDictionary();
            }

            if constexpr (CT::Array<T>) {
               for (size_t i = 0; i < ExtentOf<T>; ++i)
                  Langulus::Serialize(DeintCast(a)[i], converted, dictionary);
            }
            else Langulus::Serialize(DeintCast(a), converted, dictionary);
         }
         else {
            if constexpr (CT::Array<T>) {
               for (size_t i = 0; i < ExtentOf<T>; ++i)
                  Langulus::Serialize(DeintCast(a)[i], converted);
            }
            else Langulus::Serialize(DeintCast(a), converted);
         }

         const size_t offset = converted.GetCount();
         ThisCom::ConcatAt(at, Abandon {converted});
         at += offset;
      }

      /// MARK: InsertInner                                                   
      /// A deeply unsafe function, that places 'a' at handle 'to'            
      /// and moves handle further. Supports T being a bounded array.         
      /// Does not perform conversion.                                        
      ///   @attention works in all dimensions at once                        
      template<CT::Handle H, class T>
      static void InsertInner(H& to, T&& a) {
         using I  = IntentOf(a);
         
         if constexpr (CONVERT) {
            LglsAssumeDev(to.template IsSame<DeextAll<Deint<T>>>(),
               "Use ConvertInsertInner instead");
         }

         // Non-converting insertion                                    
         if constexpr (CT::Array<T>) {
            for (size_t i = 0; i < ExtentOf<T>; ++i) {
               Id::ForEach([&]<Cid D>{
                  if constexpr (CT::Copied<I>)
                     to.template EmplaceWithIntent<D>(Refer(DeintCast(a)[i]));
                  else
                     to.template EmplaceWithIntent<D>(I::Nest(DeintCast(a)[i]));
               });
               
               ++to;
            }
         }
         else {
            Id::ForEach([&]<Cid D>{
               if constexpr (CT::Copied<I>)
                  to.template EmplaceWithIntent<D>(Refer(LglsFwd(a)));
               else
                  to.template EmplaceWithIntent<D>(FWDIntent(a));
            });

            ++to;
         }
      }
      
      /// MARK: CheckState                                                    
      /// Check state compatibility for composition. Rules are:               
      /// 1. An invalid container is compatible with any other                
      /// 2. If one or more of the containers is disowned, they are always    
      ///    incompatible.                                                    
      /// 3. Missing containers are not compatible with non-missing ones.     
      ///    If both containers are missing, they are compatible only if they 
      ///    both are either past or future at the same time.                 
      /// 4. In order to be compatible, a container has to be either capable  
      ///    of being deepened, or matching the other type exactly.           
      /// 5. Containers of different or-ness are only compatible if one of    
      ///    the containers has count <= 1. That's the one case when or-ness  
      ///    does not matter at all.                                          
      ///   @param other - the block to check                                 
      ///   @return true if state is compatible                               
      constexpr bool CheckState(this auto const& self, CT::Container auto const& other) noexcept {
         return not self.IsValid() or self.IsDisowned() or other.IsDisowned() or (
                (not self.IsTypeConstrained() or other.IsExact(self)) //TODO IsTypeConstrained is allowed as long as it is deepenable
            and (self.GetCount() <= 1 or other.GetCount() <= 1 or self.IsOr() == other.IsOr())
            and ((not self.IsMissing() and not other.IsMissing()) or self.IsFuture() == other.IsFuture())
         );
      }

      /// Inner composition function                                          
      ///   @tparam FORCE - insert even if types mismatch, by making this     
      ///      container deeper.                                              
      ///   @param stateCompliant whether states are compatible               
      ///   @param index the place to insert at                               
      ///   @param value the value to concatenate                             
      ///   @return the number of inserted elements                           
      template<bool FORCE, CT::IndexedLinearly C>
      auto ComposeInner(
          this C& self, bool stateCompliant, CT::Index auto&& index, auto&& value
      ) -> size_t {
         using I = IntentOf(value);
         using T = Deint<I>;

         if constexpr (CT::TypeErased<C>) {
            if ((not self.IsTyped() and not self.IsValid()) or self.template IsSame<T>()) {
               // Mutate-insert inside untyped container                
               return ThisCom::template InsertAt<false>(index, LglsFwd(value));
            }
            else if (self.IsEmpty() and self.IsTyped() and not self.IsTypeConstrained()) {
               // If incompatibly typed but empty and not constrained,  
               // we can still reset the container and reuse it         
               self.Reset();
               return ThisCom::template InsertAt<false>(index, LglsFwd(value));
            }
            else if (self.IsDeep()) {
               // Already deep, push value wrapped in a container       
               if (not stateCompliant) {
                  // If container is not or-compliant after insertion,  
                  // we need to add another layer                       
                  ThisCom::Deepen();
               }

               return ThisCom::template InsertAt<false>(
                  index, Deep<C> {LglsFwd(value)} //TODO are we allowed to absorb here? should we use Piecewise?
               );
            }

            if constexpr (FORCE) {
               // If this is reached, all else failed, but we are       
               // allowed to deepen, so just do it                      
               ThisCom::Deepen();
               return ThisCom::template InsertAt<false>(
                  index, Deep<C> {LglsFwd(value)} //TODO are we allowed to absorb here? should we use Piecewise?
               );
            }
            else return 0;
         }
         else {
            if constexpr (Same<TypeOf<C>, T>) {
               // Insert to a same-typed container                      
               return ThisCom::template InsertAt<false>(
                  index, LglsFwd(value)
               );
            }
            else if constexpr (CT::Deep<TypeOf<C>>) {
               // Already deep, push value wrapped in a container       
               if (not stateCompliant) {
                  // If container is not or-compliant after insertion,  
                  // we need to add another layer                       
                  ThisCom::Deepen();
               }

               return ThisCom::template InsertAt<false>(
                  index, Deep<C> {LglsFwd(value)} //TODO are we allowed to absorb here? should we use Piecewise?
               );
            }
            else return 0;
         }
      }
   };

   #undef ThisCom
}
