///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include <Langulus/Annies/Components/Removal.hpp>

#define TEMPLATE template<Cid ID, Cid...SHARED>
#define LGLSCOM  Removal<ID, SHARED...>
#define ThisCom  self.Removal<ID, SHARED...>


namespace Langulus::Annies::Component
{
   /// Erase all elements that match a value.                              
   /// Designed for linearly indexed containers.                           
   ///   @param value the value to match                                   
   ///   @return the number of removed elements that matched the value     
   TEMPLATE template<CT::ContainsMany C, CT::NoIntent A>
   requires (CT::IndexedLinearly<C> and not CT::Handle<A>)
   auto LGLSCOM::Erase(this C& self, A const& value) -> size_t {
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
            if (element.CompareOneEqual(value)) {
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
            if (element.CompareOneEqual(value)) 
               ++removed;
            else
               shallow_clone.Insert(element);
         });

         // Then swap 'self' with the new container                  
         self.Swap(shallow_clone);
      }

      if constexpr (CT::NullTerminated<C>)
         memset((self.GetHandle() + self.GetCount()).GetRaw(), 0, 1);//TODO bit out there, but notice that these always occur after a SetCount/SetCountInner. Maybe just add null byte there? makes perfect sense when you think about it.
      return removed;
   }

   /// Erase a handle that belongs to this container.                      
   /// Designed for linearly indexed containers.                           
   ///   @param value the handle to erase                                  
   ///   @return a handle to the previous element                          
   ///   @attention previous element might not exist before being          
   ///      incremented (when erasing the first element), so be careful.   
   TEMPLATE template<CT::ContainsMany C, CT::NoIntent A>
   requires (CT::IndexedLinearly<C> and CT::Handle<A>)
   [[nodiscard("Make sure you reassign your interator if inside loop")]]
   A LGLSCOM::Erase(this C& self, A const& value) {
      LglsAssumeDev(not self.IsEmpty(),
         "How can you have a handle from an emtpy container?");
      LglsAssumeDev(value.GetRaw()    >= self.GetRaw()
                  and value.GetRawEnd() <= self.GetRawEnd(),
         "Handle doesn't belong to this container");

      size_t const offset = value - self.GetHandle();
      if (not self.IsDisowned() and self.GetUses() == 1) {
         // No need to branch-out                                    
         value.ForceMutable().Free();
         A gap_filler = value;
         while (gap_filler.GetRaw() != self.GetRawEnd()) {
            A next = gap_filler + 1;
            gap_filler.EmplaceWithIntent(Abandon {next}); //TODO multdimensional? batch optimize for PODs?
            gap_filler = next;
         }
         self.SetCountInner(self.GetCountInner() - 1);
      }
      else {
         // Branching out is required - insert nonmatching elements  
         // into a new container.                                    
         C shallow_clone;
         shallow_clone.Reserve(self.GetCountInner());
         self.Apply([&](auto const& element) {
            if (element.GetRaw() == value.GetRaw()) 
               return;
               
            shallow_clone.Insert(element);
         });

         // Then swap 'self' with the new container                  
         self.Swap(shallow_clone);
      }

      // Handle might move in new memory, so reconstructing it here  
      A result = self.GetHandle();
      if constexpr (CT::NullTerminated<C>)
         memset((result + self.GetCount()).GetRaw(), 0, 1);//TODO bit out there, but notice that these always occur after a SetCount/SetCountInner. Maybe just add null byte there? makes perfect sense when you think about it.
      return (result + offset) - 1;
   }

   /// Erase a key from a non-linearly indexed container                   
   ///   @param key the key to search for, use a pointer to an element     
   ///      to reduce the search to a memory check.                        
   ///   @return 1 if the element was found and removed, 0 otherwise       
   TEMPLATE template<CT::ContainsMany C, CT::NoIntent A>
   requires (not CT::IndexedLinearly<C> and not CT::Handle<A>)
   auto LGLSCOM::Erase(this C& self, A const& key) -> size_t {
      if (self.IsEmpty())
         return 0;

      DecideHandle<C> found;
      if constexpr (CT::Sparse<A>) {
         // We are allowed to generate a handle if key is a pointer  
         // that's pointing to an element of this container          
         if constexpr (CT::TypeErased<C>) {
            if (self.template IsSame<Deptr<A>>())
               found = self.GetHandleFromPointer(key);
            else
               found = self.Find(key);
         }
         else if constexpr (Same<TypeOf<C>, Deptr<A>>)
            found = self.GetHandleFromPointer(key);
         else
            found = self.Find(key);
      }
      else found = self.Find(key);

      if (not found)
         return 0;

      ThisCom::EraseFromTable(found.ForceMutable());
      return 1;
   }

   /// Erase a handle from a non-linearly indexed container                
   ///   @param key the handle to erase                                    
   ///   @return a handle to the previous table cell                       
   ///   @attention previous element might not exist before being          
   ///      incremented (when erasing the first element), so be careful.   
   TEMPLATE template<CT::ContainsMany C, CT::NoIntent A>
   requires (not CT::IndexedLinearly<C> and CT::Handle<A>)
   A LGLSCOM::Erase(this C& self, A const& key) {
      LglsAssumeDev(not self.IsEmpty(),
         "How can you have a handle from an emtpy container?");
      LglsAssumeDev(key.GetRaw()    >= self.GetRaw()
                  and key.GetRawEnd() <= self.GetRawEnd(),
         "Handle doesn't belong to this container");

      auto h = key.ForceMutable();
      ThisCom::EraseFromTable(h);
      return h - 1;
   }

   /// If key is mutable and can be reassigned, we can notify on discard   
   TEMPLATE template<CT::ContainsMany C, CT::NoIntent A>
   requires (not CT::IndexedLinearly<C> and CT::Handle<A>)
   [[nodiscard("Make sure you reassign your iterator if inside loop")]]
   A LGLSCOM::Erase(this C& self, A& key) {
      return self.Erase(const_cast<A const&>(key));
   }

   /// Erase a number of elements starting at a specific position.         
   /// Designed for linearly indexed containers.                           
   ///   @param idx the starting location                                  
   ///   @param count the number of elements to erase (1 by default)       
   ///   @return the number of removed elements                            
   TEMPLATE template<CT::ContainsMany C> requires CT::IndexedLinearly<C>
   auto LGLSCOM::EraseAt(this C& self, CT::Index auto&& idx, size_t count) -> size_t {
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

      if constexpr (CT::NullTerminated<C>)
         memset((self.GetHandle() + self.GetCount()).GetRaw(), 0, 1);//TODO bit out there, but notice that these always occur after a SetCount/SetCountInner. Maybe just add null byte there? makes perfect sense when you think about it.
      return count;
   }
   
   /*template<CT::ContainsMany C>
   auto EraseDeepAt(this C&, CT::Index auto&&) -> size_t;*/

   /// Sets a new smaller count by destroying elements on the back.        
   /// Does nothing if 'desiredCount' is larger or equals the current.     
   ///   @attention never reallocates                                      
   ///   @param desiredCount the new count                                 
   TEMPLATE template<CT::ContainsMany C> requires CT::IndexedLinearly<C>
   void LGLSCOM::Trim(this C& self, size_t desiredCount) noexcept {
      const auto currentCount = self.GetCount();
      if (desiredCount >= currentCount)
         return;

      // If data doesn't need destructors just reduce count          
      if constexpr (C::TypeErased) {
         if (not self.GetType().GetDestructor()) {
            self.SetCount(desiredCount);

            if constexpr (CT::NullTerminated<C>) {
               if (desiredCount)
                  memset((self.GetHandle() + desiredCount).GetRaw(), 0, 1);
            }
            return;
         }
      }
      else {
         if constexpr (not CT::Destroyable<TypeOf<C>>) {
            self.SetCount(desiredCount);

            if constexpr (CT::NullTerminated<C>) {
               if (desiredCount)
                  memset((self.GetHandle() + desiredCount).GetRaw(), 0, 1);
            }
            return;
         }
      }

      // Call destructors and change count                           
      LglsAssert(self.GetAllocation(),
         "Can't trim disowned container");
      LglsAssert(self.GetAllocation()->GetUses() != 1,
         "Can't trim container used elsewhere");

      self.SelectInner(desiredCount, currentCount - desiredCount).FreeInner(); //TODO doesn't select disown? if so, will FreeInner work? if not, maybe we need HandleRange?
      self.SetCount(desiredCount);

      if constexpr (CT::NullTerminated<C>) {
         if (desiredCount)
            memset((self.GetHandle() + desiredCount).GetRaw(), 0, 1);//TODO bit farfetched, but notice that these always occur after a SetCount/SetCountInner. Maybe just add null byte there? makes perfect sense when you think about it.
      }
   }

   /// Destroy all elements but don't deallocate memory unless we really   
   /// have to.                                                            
   ///   @attention will never reset state except disownment               
   ///   @attention will never reset type                                  
   TEMPLATE
   void LGLSCOM::Clear(this auto& self) {
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
   TEMPLATE
   void LGLSCOM::Reset(this auto& self) {
      if_available(self.Free());
      if_available(self.ResetAllAllocations());
      if_available(self.ResetState());
      if_available(self.ResetAllTypes());
   }
   
   /// Erases an element at a specific index from a hash table             
   ///   @attention assumes that index points to a valid element           
   ///   @attention operates on all dimensions at once                     
   ///   @param h the handle to erase (intentionally by copy)              
   TEMPLATE template<CT::ContainsMany C> requires CT::IndexedHashTable<C>
   void LGLSCOM::EraseFromTable(this C& self, CT::Handle auto h) assumptious {
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

   /// Erases an element at a specific index from a coverage table         
   ///   @attention assumes that index points to a valid element           
   ///   @attention operates on all dimensions at once                     
   ///   @param h the handle to erase                                      
   TEMPLATE template<CT::ContainsMany C> requires CT::IndexedCoverage<C>
   void LGLSCOM::EraseFromTable(this C& self, CT::Handle auto const& h) assumptious {
      size_t slot = h - self.GetHandle();
      LglsAssumeDev(self.IsSlotOccupied(slot),
         "Removing an invalid key");
      LglsAssumeDev(self.GetUses() == 1,
         "Must branch-out before calling this method");

      h.Free();
      self.DisableSlot(slot);
      --self.GetCountInner();
   }
}

#undef TEMPLATE
#undef LGLSCOM
#undef ThisCom
