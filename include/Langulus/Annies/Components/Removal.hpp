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
#include <Langulus/CT/Index.hpp>


namespace Langulus::Annies::Component
{
   /// Refers back to this particular component instance through the deduced  
   /// 'this'. Just for convenience. It is #undef-ed at the end of this file. 
   #define ThisCom self.Removal<ID, SHARED...>

   ///                                                                        
   /// Implements removal for containers. This includes Trim, Clear, Reset    
   /// and other destruction-associated services.                             
   ///   @tparam ID provider we're removing from                              
   ///   @tparam SHARED additional providers we're removing from              
   //TODO add bool ORDER_PRESERVING as an optimization. if order is not required, we can do 'swap & pop tactic in contiguous containers for faster removal
   template<Cid ID, Cid...SHARED>
   struct Removal {
      using CTTI_Component = Yup;
      using CTTI_ReflectAs = void;
      using Id             = Values<ID, SHARED...>;

      static constexpr int ComponentPrecedence = 3000;

   private:
      //template<CT::Container C>
      //using Count = typename Deref<C>::CountType;
      template<CT::Container C>
      using Iterator = typename Deref<C>::Iterator;

   public:
      /// Erase all elements that match a value.                              
      /// Designed for linearly indexed containers.                           
      ///   @param value the value to match                                   
      ///   @return the number of removed elements that matched the value     
      template<CT::ContainsMany C> requires CT::IndexedLinearly<C>
      auto Erase(this C& self, CT::NoIntent auto const& value) -> size_t {
         if (self.IsEmpty())
            return 0;

         size_t removed = 0;
         if (not self.IsDisowned() and self.GetUses() == 1) {
            // No need to branch-out                                    
            // Start erasing matching elements, filling gaps on our way 
            //TODO gaps won't form if container is not order-preserving by using swap and pop
            using H = DecideHandle<C>;
            H first_reusable;
            self.Apply([&](H& element) {
               if (element == value) {
                  element.Free();
                  if (not removed)
                     first_reusable = element;
                  ++removed;
               }
               else if (removed) {
                  first_reusable.EmplaceWithIntent(Abandon {element}); //TODO multdimensional?
                  ++first_reusable;
               }
            });

            self.SetCountInner(self.GetCountInner() - removed);
         }
         else {
            // Branching out is required - insert nonmatching elements  
            // into a new container.                                    
            C shallow_clone;
            shallow_clone.Reserve(self.GetCountInner());
            self.Apply([&](auto const& element) {
               if (element == value) 
                  ++removed;
               else
                  shallow_clone.Insert(element);
            });

            // Then swap 'self' with the new container                  
            self.Swap(shallow_clone);
         }

         return removed;
      }

      /// Erase a key from a non-linearly indexed container                   
      ///   @param key - the key to search for                                
      ///   @return the number of removed elements                            
      template<CT::ContainsMany C> requires (not CT::IndexedLinearly<C>)
      auto Erase(this C& self, CT::NoIntent auto const& key) -> size_t {
         if (self.IsEmpty())
            return 0;

         auto h = self.Find(key).ForceMutable();
         ThisCom::EraseFromTable(h);
         return 1;
      }

      /// Erase a number of elements starting at a specific position.         
      /// Designed for linearly indexed containers.                           
      ///   @param idx the starting location                                  
      ///   @param count the number of elements to erase (1 by default)       
      ///   @return the number of removed elements                            
      template<CT::ContainsMany C> requires CT::IndexedLinearly<C>
      auto EraseAt(this C& self, CT::Index auto&& idx, size_t count = 1) -> size_t {
         if (self.IsEmpty())
            return 0;
      
         const auto offset = self.SimplifyIndex(idx);
         const auto limits = self.GetCountInner();
         if (count > limits - offset)
            count = limits - offset;
         const auto remainder = limits - count;

         if (not self.IsDisowned() and self.GetUses() == 1) {
            // No need to branch-out                                    
            if (not remainder) {
               ThisCom::Clear();
               return count;
            }

            // Start erasing matching elements, filling gaps on our way 
            // First, destroy all relevant elements.                    
            auto element = self.GetHandle() + offset;
            const auto end = (element + count).GetRaw();
            while(element.GetRaw() != end) {
               element.Free();
               ++element;
            }

            const auto absolute_end = self.GetRawEnd();
            if (end != absolute_end) {
               // A gap was formed, we have to fill it                  
               auto gap = self.GetHandle() + offset;
               while(element.GetRaw() != absolute_end) {
                  gap.EmplaceWithIntent(Abandon {element}); //TODO multdimensional?
                  ++element;
                  ++gap;
               }
            }

            self.SetCountInner(remainder);
         }
         else {
            // Branching out is required - insert relevant elements     
            // into a new container.                                    
            const auto handle     = self.GetHandle() + offset;
            const auto skip_start = handle.GetRaw();
            const auto skip_end   = (handle + count).GetRaw();

            C shallow_clone;
            if (remainder) {
               shallow_clone.Reserve(remainder);
               self.Apply([&](auto const& element) {
                  if (element.GetRaw() < skip_start or element.GetRaw() >= skip_end) 
                     shallow_clone.Insert(element);
               });
            }

            // Then swap 'self' with the new container                  
            self.Swap(shallow_clone);
         }

         return count;
      }

      template<CT::ContainsMany C>
      auto EraseIt(this C&, Iterator<C> const&, size_t = 1) -> Iterator<C>;
      
      template<CT::ContainsMany C>
      auto EraseDeepAt(this C&, CT::Index auto&&) -> size_t;

      /// Sets a new smaller count by destroying elements on the back.        
      /// Does nothing if 'desiredCount' is larger or equals the current.     
      ///   @attention never reallocates                                      
      ///   @param desiredCount the new count                                 
      template<CT::ContainsMany C> requires CT::IndexedLinearly<C>
      void Trim(this C& self, size_t desiredCount) noexcept {
         const auto currentCount = self.GetCount();
         if (desiredCount >= currentCount)
            return;

         // If data doesn't need destructors just reduce count          
         if constexpr (C::TypeErased) {
            if (not self.GetType().GetDestructor()) {
               self.SetCount(desiredCount);
               return;
            }
         }
         else {
            if constexpr (not CT::Destroyable<TypeOf<C>>) {
               self.SetCount(desiredCount);
               return;
            }
         }

         // Call destructors and change count                           
         LglsAssert(self.GetAllocation(),
            "Can't trim disowned container");
         LglsAssert(self.GetAllocation()->GetUses() != 1,
            "Can't trim container used elsewhere");

         self.SelectInner(desiredCount, currentCount - desiredCount).FreeInner();
         self.SetCount(desiredCount);
      }

      template<CT::ContainsMany C>
      void Optimize(this C&);

      /// Destroy all elements but don't deallocate memory unless we really   
      /// have to.                                                            
      ///   @attention will never reset state except disownment               
      ///   @attention will never reset type                                  
      void Clear(this auto& self) {
         const auto al = self.GetAllocation();
         if (self.IsEmpty()) {
            // Container is already empty. Just make sure that we're    
            // not gatekeeping an allocation that's used elsewhere.     
            if (al and al->GetUses() > 1) {
               // Since container is empty, all that this does is       
               // dereference and reset all allocations                 
               self.Free();
               self.ResetAllAllocations();
            }
            if_available(self.DisableDisowned());
            return;
         }
         
         if (not al) {
            // Data is either unallocated, static, or emergent.         
            // Free any emergent items, or static sparse items.         
            // Allocations are already nonexistent, so no need to reset.
            self.Free();
            if_available(self.SetReservedInner(0));
            if_available(self.SetHashTableInner(nullptr));
            self.ResetCount();
            if_available(self.DisableDisowned());
            return;
         }

         if (al->GetUses() == 1) {
            // Entry is used only in this block, so it's safe to        
            // destroy all elements. We will reuse the memory and type  
            // only if the container keeps track of the count separately
            self.template Free<false>();
            if_available(self.ResetHashTable());
            self.ResetCount();
         }
         else {
            // If reached, then data is referenced from multiple places.
            // Don't call local destructors, just dereference and clear 
            // allocation, because it isn't ours. Indirections will     
            // always get destroyed if they are fully dereferenced,     
            // unless disowned.                                         
            self.Free();
            self.ResetAllAllocations();
         }

         if_available(self.DisableDisowned());
      }

      /// Destroy all elements, deallocate memory and reset state and type.   
      ///   @attention notice that heap pointer is not zeroed here, as it     
      ///      is not a requirement. It is UB if you GetRaw while count is 0! 
      void Reset(this auto& self) {
         if_available(self.Free());
         if_available(self.ResetAllAllocations());
         if_available(self.ResetState());
         if_available(self.ResetAllTypes());
      }

   protected:
      /// Erases an element at a specific index from a hash table             
      ///   @attention assumes that index points to a valid element           
      ///   @attention operates on all dimensions at once                     
      ///   @param index - the index to remove                                
      template<CT::ContainsMany C> requires (not CT::IndexedLinearly<C>)
      void EraseFromTable(this C& self, CT::Handle auto& h) assumptious {
         const auto tableBeg = DecvqAllCast(self.template GetHashTable<ID>()); //TODO what if different tables per dimension?
         auto psl = tableBeg + (h - self.GetHandle());
         LglsAssumeDev(*psl,
            "Removing an invalid key");
         LglsAssumeDev(self.GetUses() == 1,
            "Must branch-out before calling this method");

         // Destroy the key and info at the start                       
         //auto h = self.GetHandle() + index;
         h.Free();
         ++h;

         *(psl++) = 0;

      try_again:
         // Shift backwards, until a zero or 1 is reached.              
         // That way we move every entry that is far from its bucket    
         // closer to it.                                               
         while (*psl > 1) {
            psl[-1] = (*psl) - 1;

            auto prev_h = h - 1;
            Id::ForEach([&prev_h, &h]<Cid D> {
               prev_h.template EmplaceWithIntent<D>(Abandon(h));
            });

            h.Free();
            ++h;

            *(psl++) = 0;
         }

         // Be aware, that iterator might loop around                   
         const auto tableEnd = self.template GetHashTableEnd<ID>(); //TODO what if different tables per dimension?
         if (psl == tableEnd and *tableBeg > 1) {
            const auto last = self.GetReserved() - 1;
            psl = tableBeg;
            tableBeg[last] = (*psl) - 1;

            // Shift first entry to the back                            
            h = self.GetHandle();
            auto last_h = self.GetHandle().ForceMutable() + last;
            Id::ForEach([&last_h, &h]<Cid D> {
               last_h.template EmplaceWithIntent<D>(Abandon(h));
            });

            h.Free();
            ++h;

            *(psl++) = 0;

            // And continue the vicious cycle                           
            goto try_again;
         }

         // Success                                                     
         --self.GetCountInner();
      }
   };

   #undef ThisCom
}
