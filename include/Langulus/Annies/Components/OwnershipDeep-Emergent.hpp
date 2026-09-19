///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Container.hpp"
#include <Langulus/Assume.hpp>
#include <Langulus/Allocator.hpp>
#include <Langulus/MetaOf.hpp>
#include <Langulus/CT/Allocatable.hpp>
#include <Langulus/CT/Referenced.hpp>
#include <Langulus/CT/Index.hpp>


namespace Langulus::Annies
{
   using DMeta = RTTI::DMeta;
}

namespace Langulus::Annies::Component
{
   /// Refers back to this particular component instance through the deduced  
   /// 'this'. Just for convenience. It is #undef-ed at the end of this file. 
   #define ThisCom self.OwnershipDeepEmergent<STYLE, REF_INDIVIDUAL, ID, SHARED...>

   ///                                                                        
   /// Manages deep ownership by searching for an allocation every time.      
   /// Also used as base for other deep ownership components.                 
   ///   @tparam STYLE whether ownership will be automatically applied on     
   ///      construction, reassignment and destruction. Usually 0 if container
   ///      is just a view, or in other cases where you want to carry an      
   ///      allocation pointer, but not necessarily reference it.             
   ///   @tparam REF_INDIVIDUAL toggles whether contained items that were     
   ///      reflected as CT::Referenced get referenced. Elements will get     
   ///      referenced even if no entry for the element exist, but you can    
   ///      avoid referencing altogether if you use the Disown intent.        
   ///      To be more specific - when GetAllocation() is nullptr and the     
   ///      entire container is considered disowned.                          
   ///   @tparam ID which heap/stack are we keeping track of?                 
   ///   @tparam SHARED additional provider IDs that share the same behavior  
   template<unsigned STYLE, bool REF_INDIVIDUAL, Cid ID, Cid...SHARED>
   struct OwnershipDeepEmergent {
      using CTTI_Component = Yup;
      using CTTI_ReflectAs = void;
      using Id = Values<ID, SHARED...>;

      static constexpr unsigned OwnedDeep = STYLE;
      static constexpr bool ReferenceElements = REF_INDIVIDUAL;
      static constexpr bool Shared = sizeof...(SHARED) > 0;
      static constexpr int  ComponentPrecedence = 2000;
      //template<Cid SID>
      //static constexpr bool Relevant = Id::template Contains<SID>;

      /// MARK: Public                                                        
      /// Emergent deep ownership can't provide an array of entries           
      template<Cid SID = ID> //requires Relevant<SID>
      constexpr auto GetEntries() const noexcept -> AllocationPtr const* {
         return nullptr;
      }
      
      constexpr auto GetKeyEntries() const noexcept -> AllocationPtr const* requires Shared {
         return nullptr;
      }
      constexpr auto GetValEntries() const noexcept -> AllocationPtr const* requires Shared {
         return nullptr;
      }

      constexpr auto GetKeyEntriesAt(CT::Index auto&&) const noexcept requires Shared {
         return nullptr;
      }
      constexpr auto GetValEntriesAt(CT::Index auto&&) const noexcept requires Shared {
         return nullptr;
      }

   protected:
      /// MARK: Protected                                                     
      LglsComHeapReference(friend);
      LglsComHeapMovable(friend);
      LglsComRemoval(friend);
      LglsComEmplacement(friend);
      LglsComOwnershipEmergent(friend);

      template<CT::Container C>
      using Count = typename Deref<C>::CountType;

   #if LANGULUS_FEATURE(MANAGED_MEMORY)
      /// Find _and populate_ an indirection entry                            
      template<bool CUSTOM_POINTERS, CT::Container C>
      static void FindEntry(
         AllocationPtr const* entries, CT::Sparse auto ptr,
         [[maybe_unused]] const DMeta& T = {},
         [[maybe_unused]] const DMeta& nextT = {}
      ) assumptious {
         if (*entries)
            return;

      #if LANGULUS(SAFE)
         // We can find the allocation behind the pointer but in order  
         // to save it, we must make sure that entry array resides in   
         // non-shared memory (when OwnershipDeepHeap is used)          
         auto entry_allocation = Allocator::Find(entries);
         LglsAssumeDev(entry_allocation and entry_allocation->GetUses() == 1);
      #endif

         auto& entry = DecvqAllCast(*entries);
         if constexpr (CT::TypeErased<C>) {
            if constexpr (CUSTOM_POINTERS) {
               const auto ptrSpec = T.GetPointerSpecification();
               if (ptrSpec.IsPacked()) {
                  uintptr_t derefptr = 0;
                  memcpy(&derefptr, ptr, ptrSpec.GetTotalBytes());
                  entry = DecvqAllCast(Allocator::FindPackedPointer(
                     ptrSpec, nextT, derefptr
                  ));
               }
               else entry = DecvqAllCast(Allocator::Find(*static_cast<void**>(ptr)));
            }
            else entry = DecvqAllCast(Allocator::Find(ptr));
         }
         else entry = DecvqAllCast(Allocator::Find(ptr));
      }
   #endif

      /// MARK: Keep                                                          
      /// Nests through all indirection layers of the first contained element.
      /// Relies on predeclared array of GetEntriesInner (i.e. not emergent). 
      ///   @tparam FIND_MISSING if an entry is missing, we attempt at finding
      ///      it in the memory manager, if MANAGED_MEMORY feature is enabled 
      ///   @attention individuals will be referenced if REF_INDIVIDUAL is    
      ///      enabled, regardless if an entry was found.                     
      ///   @attention assumes container is not disowned!                     
      ///   @attention assumes there are no custom pointers involved!         
      ///   @attention doesn't change any container state                     
      ///   @attention works on one dimension at a time!                      
      template<bool FIND_MISSING = false, Cid SID = ID, CT::Container C> //requires Relevant<SID>
      void KeepElementDeepStandardPointers(this C& self) assumptious {
         LglsAssumeDev(not self.IsDisowned(),
            "Can't keep anything in a container without ownership");

         auto entries = self.template GetEntriesInner<SID>();
         if (not entries)
            return;

         using H = Decay<decltype(LglsFake(DecideHandle<C>).template PickDimension<SID>())>;
         if constexpr (CT::TypeErased<C>) {
            //                                                          
            // Referencing a type-erased element                        
            const auto T = self.template GetType<SID>();
            LglsAssumeDev(T.IsSparse(),
               "Don't call KeepElementDeepStandardPointers if container isn't sparse");

            const auto subT = T.GetDeptr();
            const auto ptr = *static_cast<void**>(self.template GetRawVoid<SID>());
            LglsAssumeDevAndOptimize(ptr, "Null pointer");

            #if LANGULUS_FEATURE(MANAGED_MEMORY)
               if constexpr (FIND_MISSING)
                  ThisCom::template FindEntry<false, C>(entries, ptr);
            #endif

            if (subT.IsSparse()) {
               // Pointer to pointer                                    
               H temp {Stackwise, subT, ptr, entries + 1};
               temp.template KeepElementDeepStandardPointers<FIND_MISSING>();
            }
            else if constexpr (REF_INDIVIDUAL) {
               if (const auto referencer = subT.GetReferencer()) {
                  // Pointer to dense                                   
                  referencer(ptr, 1);
               }
            }
         }
         else {
            //                                                          
            // Referencing a statically-typed element                   
            using T = TypeOf<C, SID>;         
            static_assert(CT::Sparse<T>,
               "Don't call KeepElementDeepStandardPointers if container isn't sparse");

            using DT = Deptr<T>;
            auto ptr = *self.template GetRaw<SID>();
            LglsAssumeDevAndOptimize(ptr, "Null pointer");

            #if LANGULUS_FEATURE(MANAGED_MEMORY)
               if constexpr (FIND_MISSING)
                  ThisCom::template FindEntry<false, C>(entries, ptr);
            #endif

            if constexpr (CT::Sparse<DT>) {
               // Pointer to pointer                                    
               typename H::Denser temp {Stackwise, ptr, entries + 1};
               temp.template KeepElementDeepStandardPointers<FIND_MISSING>();
            }
            else if constexpr (REF_INDIVIDUAL and CT::Referenced<DT>) {
               // Pointer to dense                                      
               ptr->Reference(1);
            }
         }

         if (*entries)
            DecvqAllCast(*entries)->AddRef(1);
      }

      /// Nests through all indirection layers of the first contained element.
      /// Emergent - every indirection will be sought in the memory manager   
      /// if MANAGED_MEMORY is enabled.                                       
      ///   @attention individuals will be referenced if REF_INDIVIDUAL is    
      ///      enabled, regardless if an entry was found.                     
      ///   @attention assumes there are no custom pointers involved!         
      ///   @attention doesn't change any container state                     
      ///   @attention works on one dimension at a time!                      
      template<Cid SID = ID, CT::Container C> //requires Relevant<SID>
      void KeepElementDeepStandardPointersEmergent(this C& self) assumptious {
         LglsAssumeDev(not self.IsDisowned(),
            "Can't keep anything in a container without ownership");

         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            Allocation const* entry = nullptr;
         #endif

         using H = Decay<decltype(LglsFake(DecideHandle<C>).template PickDimension<SID>())>;
         if constexpr (CT::TypeErased<C>) {
            //                                                          
            // Referencing a type-erased element                        
            const auto T = self.template GetType<SID>();
            LglsAssumeDev(T.IsSparse(),
               "Don't call KeepElementDeepStandardPointersEmergent if container isn't sparse");

            const auto subT = T.GetDeptr();
            const auto ptr = *static_cast<void**>(self.template GetRawVoid<SID>());
            LglsAssumeDevAndOptimize(ptr, "Null pointer");

            #if LANGULUS_FEATURE(MANAGED_MEMORY)
               entry = Allocator::Find(ptr);
            #endif

            if (subT.IsSparse()) {
               // Pointer to pointer                                    
               H temp {Stackwise, subT, ptr};
               temp.KeepElementDeepStandardPointersEmergent();
            }
            else if constexpr (REF_INDIVIDUAL) {
               if (const auto referencer = subT.GetReferencer()) {
                  // Pointer to dense                                   
                  referencer(ptr, 1);
               }
            }
         }
         else {
            //                                                          
            // Referencing a statically-typed element                   
            using T = TypeOf<C, SID>;         
            static_assert(CT::Sparse<T>,
               "Don't call KeepElementDeepStandardPointersEmergent if container isn't sparse");

            using DT = Deptr<T>;
            auto ptr = *self.template GetRaw<SID>();
            LglsAssumeDevAndOptimize(ptr, "Null pointer");

            #if LANGULUS_FEATURE(MANAGED_MEMORY)
               entry = Allocator::Find(ptr);
            #endif

            if constexpr (CT::Sparse<DT>) {
               // Pointer to pointer                                    
               typename H::Denser temp {Stackwise, ptr};
               temp.KeepElementDeepStandardPointersEmergent();
            }
            else if constexpr (REF_INDIVIDUAL and CT::Referenced<DT>) {
               // Pointer to dense                                      
               ptr->Reference(1);
            }
         }

         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            if (entry)
               DecvqAllCast(entry)->AddRef(1);
         #endif
      }

   #if LANGULUS_FEATURE(MANAGED_MEMORY)
      /// Nests through all indirection layers of the first contained element.
      /// Supports any number of custom pointer indirections.                 
      /// Relies on predeclared array of GetEntriesInner (i.e. not emergent). 
      ///   @tparam FIND_MISSING if an entry is missing, we attempt at finding
      ///      it in the memory manager, if MANAGED_MEMORY feature is enabled 
      ///   @attention individuals will be referenced if REF_INDIVIDUAL is    
      ///      enabled, regardless if an entry was found.                     
      ///   @attention assumes container is not disowned!                     
      ///   @attention doesn't change any container state                     
      ///   @attention works on one dimension at a time!                      
      template<bool FIND_MISSING = false, Cid SID = ID, CT::Container C> //requires Relevant<SID>
      void KeepElementDeepCustomPointers(this C& self) assumptious {
         LglsAssumeDev(not self.IsDisowned(),
            "Can't keep anything in a container without ownership");

         // Check if disowned/outside authority                         
         auto entries = self.template GetEntriesInner<SID>();
         if (not entries)
            return;

         if constexpr (CT::TypeErased<C>) {
            // Check if containing indirections                         
            auto T = self.template GetType<SID>();
            LglsAssumeDev(T.IsSparse(),
               "Don't call KeepElementDeepCustomPointers if container isn't sparse");

            void* src = self.template GetRawVoid<SID>();
            while (src and T.IsSparse()) {
               auto nextT = T.GetDeptr();
               if constexpr (FIND_MISSING)
                  ThisCom::template FindEntry<true, C>(entries, src, T, nextT);

               if (nextT.IsSparse()) {
                  // Pointer T -> Pointer nextT                         
                  T.GetDereffer()(src, &src);
               }
               else if constexpr (REF_INDIVIDUAL) {
                  if (const auto referencer = nextT.GetReferencer()) {
                     // Pointer T -> Dense nextT                        
                     referencer(const_cast<void*>(UnpackPointer(T, nextT, src)), 1);
                  }
               }

               if (*entries)
                  DecvqAllCast(*entries)->AddRef(1);

               // Move to next indirection                              
               T = nextT;
               ++entries;
            }
         }
         else {
            using T = TypeOf<C, SID>;
            static_assert(CT::Sparse<T>,
               "Don't call KeepElementDeepCustomPointers if container isn't sparse");

            auto& ptr = *self.template Get<void, SID>();
            ForEachIndirection(ptr, [&](auto& i) {
               if constexpr (FIND_MISSING)
                  ThisCom::template FindEntry<true, C>(entries, i);

               if (*entries)
                  DecvqAllCast(*entries)->AddRef(1);

               ++entries;
            });

            if constexpr (REF_INDIVIDUAL and CT::Referenced<Decay<T>>)
               DenseCast(ptr).Reference(1);
         }
      }

      /// Nests through all indirection layers of the first contained element.
      /// Supports any number of custom pointer indirections.                 
      /// Emergent - every indirection will be sought in the memory manager.  
      ///   @attention individuals will be referenced if REF_INDIVIDUAL is    
      ///      enabled, regardless if an entry was found.                     
      ///   @attention doesn't change any container state                     
      ///   @attention works on one dimension at a time!                      
      template<Cid SID = ID, CT::Container C> //requires Relevant<SID>
      void KeepElementDeepCustomPointersEmergent(this C& self) assumptious {
         LglsAssumeDev(not self.IsDisowned(),
            "Can't keep anything in a container without ownership");

         if constexpr (CT::TypeErased<C>) {
            // Check if containing indirections                         
            auto T = self.template GetType<SID>();
            LglsAssumeDev(T.IsSparse(),
               "Don't call KeepElementDeepCustomPointersEmergent if container isn't sparse");

            void* src = self.template GetRawVoid<SID>();
            while (src and T.IsSparse()) {
               auto nextT = T.GetDeptr();
               AllocationPtr entry;

               if (nextT.IsSparse()) {
                  // Pointer T -> Pointer nextT                         
                  T.GetDereffer()(src, &src);
                  entry = Allocator::Find(src);
               }
               else {
                  src = const_cast<void*>(UnpackPointer(T, nextT, src));
                  entry = Allocator::Find(src);

                  if constexpr (REF_INDIVIDUAL) {
                     if (const auto referencer = nextT.GetReferencer()) {
                        // Pointer T -> Dense nextT                     
                        referencer(src, 1);
                     }
                  }
               }

               if (entry)
                  DecvqAllCast(entry)->AddRef(1);

               // Move to next indirection                              
               T = nextT;
            }
         }
         else {
            using T = TypeOf<C, SID>;
            static_assert(CT::Sparse<T>,
               "Don't call KeepElementDeepCustomPointersEmergent if container isn't sparse");

            auto& ptr = *self.template Get<void, SID>();
            ForEachIndirection(ptr, [&](auto& i) {
               if (auto entry = Allocator::Find(i))
                  DecvqAllCast(entry)->AddRef(1);
            });

            if constexpr (REF_INDIVIDUAL and CT::Referenced<Decay<T>>)
               DenseCast(ptr).Reference(1);
         }
      }
   #endif

      /// Deep-reference an element                                           
      ///   @tparam FIND_MISSING if an entry is missing, we attempt at finding
      ///      it in the memory manager, if MANAGED_MEMORY feature is enabled.
      ///      Ignored if container is marked Emergent.                       
      ///   @attention individuals will be referenced if REF_INDIVIDUAL is    
      ///      enabled, regardless if an entry was found.                     
      ///   @attention works on one dimension at a time!                      
      template<bool FIND_MISSING = false, Cid SID = ID, CT::Container C> //requires Relevant<SID>
      void KeepElementDeep(this C& self) assumptious {
         if constexpr (not requires { self.template GetEntriesInner<SID>(); }) {
            #if LANGULUS_FEATURE(MANAGED_MEMORY)
               ThisCom::template KeepElementDeepCustomPointersEmergent<SID>();
            #else
               ThisCom::template KeepElementDeepStandardPointersEmergent<SID>();
            #endif
         }
         else {
            #if LANGULUS_FEATURE(MANAGED_MEMORY)
               ThisCom::template KeepElementDeepCustomPointers<FIND_MISSING, SID>();
            #else
               ThisCom::template KeepElementDeepStandardPointers<FIND_MISSING, SID>();
            #endif
         }
      }

      /// MARK: Destroy                                                       
      /// Nests through all indirection layers and destroys elements and      
      /// their entries if they are fully dereferenced.                       
      /// Relies on predeclared array of GetEntriesInner (i.e. not emergent). 
      ///   @tparam FORCE_DESTROY Destroy dense elements if true. Note:       
      ///      sparse elements are always destroyed if fully dereferenced.    
            //TODO FORCE_DESTROY no longer required?

      ///   @attention assumes container is not disowned!                     
      ///   @attention assumes there's exactly 1 use of the allocation!       
      ///   @attention assumes there are no custom pointers involved!         
      ///   @attention doesn't change any container state or entry            
      ///   @attention works on one dimension at a time!                      
      template<bool FORCE_DESTROY = true, Cid SID = ID, CT::Container C> //requires Relevant<SID>
      void DestroyElementDeepStandardPointers(this C& self) assumptious {
         LglsAssumeDev(not self.IsEmpty(),
            "Can't destroy anything in an empty container");
         LglsAssumeDev(not self.IsDisowned(),
            "Can't destroy anything in a container without ownership");

         using H = Decay<decltype(LglsFake(DecideHandle<C>).template PickDimension<SID>())>;
         if constexpr (CT::TypeErased<C>) {
            //                                                          
            // Destroying a type-erased element                         
            const auto T = self.template GetType<SID>();
            LglsAssumeDev(T.IsSparse(),
               "Don't call DestroyElementDeepStandardPointers if container isn't sparse");

            auto entries = self.template GetEntriesInner<SID>();
            if (not entries)
               return;

            // If T is Text**, subT is Text*                            
            const auto subT = T.GetDeptr();
            // If T is Text**, ptr becomes Text**                       
            const auto ptr = *static_cast<void**>(self.template GetRaw<SID>());
            if (not ptr)
               return;

            if (*entries and 1 == (*entries)->GetUses()) {
               if (subT.IsSparse()) {
                  // Pointer to pointer.                                
                  // Destroy all nested indirection layers.             
                  if (auto subEntry = entries + 1) {
                     H temp {Stackwise, subT, ptr, subEntry};
                     temp.template DestroyElementDeepStandardPointers<FORCE_DESTROY>();
                  }
               }
               else if (auto destructor = subT.GetDestructor()) {
                  // Pointer to a complete, destroyable dense.          
                  // Call the destructor.                               
                  if constexpr (REF_INDIVIDUAL) {
                     if (const auto referencer = subT.GetReferencer()) {
                        if (referencer(ptr, -1) == 0)
                           destructor(ptr);
                     }
                     else destructor(ptr);
                  }
                  else destructor(ptr);
               }
            }
            else {
               if (subT.IsSparse()) {
                  // Pointer to pointer.                                
                  // Dereference all indirection layers.                
                  if (auto subEntry = entries + 1) {
                     H temp {Stackwise, subT, ptr, subEntry};
                     temp.template DestroyElementDeepStandardPointers<FORCE_DESTROY>();
                  }
               }
               else if constexpr (REF_INDIVIDUAL) {
                  if (const auto referencer = subT.GetReferencer()) {
                     // This element occurs in more than one place.     
                     // We're not allowed to deallocate the memory      
                     // behind it, but we must call destructors if T    
                     // is referencable and its individual references   
                     // have reached 0. This can happen when hive       
                     // elements are dereferenced, for example.         
                     if (referencer(ptr, -1) == 0)
                        subT.GetDestructor()(ptr);
                  }
               }
            }

            // Deallocate or dereference                                
            if (*entries) {
               auto& mutable_entries = DecvqAllCast(*entries);
               if (1 == (*entries)->GetUses())
                  Allocator::Deallocate(mutable_entries);
               else
                  mutable_entries->AddRef(-1);
               //mutable_entries = nullptr; //not allowed! we may be modifying memory owned by another container!!
            }
         }
         else {
            //                                                          
            // Destroying a statically-typed element                    
            using T = TypeOf<C, SID>;
            static_assert(CT::Sparse<T>,
               "Don't call DestroyElementDeepStandardPointers if container isn't sparse");

            using DT = Deptr<T>;
            auto entries = self.template GetEntriesInner<SID>();
            if (not entries)
               return;

            auto& ptr = *self.template GetRawAs<T, SID>();
            if (not ptr)
               return;

            if (*entries and 1 == (*entries)->GetUses()) {
               if constexpr (CT::Sparse<DT>) {
                  // Pointer to pointer.                                
                  // Destroy all nested indirection layers.             
                  typename H::Denser temp {Stackwise, ptr, entries + 1};
                  temp.template DestroyElementDeepStandardPointers<FORCE_DESTROY>();
               }
               else if constexpr (CT::Destroyable<DT>) {
                  // Pointer to a complete, destroyable dense.          
                  // Call the destructor.                               
                  if constexpr (REF_INDIVIDUAL and CT::Referenced<DT>) {
                     if (ptr->Reference(-1) == 0)
                        ptr->~DT();
                  }
                  else ptr->~DT();
               }
            }
            else {
               if constexpr (CT::Sparse<DT>) {
                  // Pointer to pointer.                                
                  // Destroy all nested indirection layers.             
                  typename H::Denser temp {Stackwise, ptr, entries + 1};
                  temp.template DestroyElementDeepStandardPointers<FORCE_DESTROY>();
               }
               else if constexpr (REF_INDIVIDUAL and CT::Referenced<DT>) {
                  // This element occurs in more than one place.        
                  // We're not allowed to deallocate the memory         
                  // behind it, but we must call destructors if T is    
                  // referencable and its individual references have    
                  // reached 0. This can happen when hive elements      
                  // are dereferenced.                                  
                  if (ptr->Reference(-1) == 0)
                     ptr->~DT();
               }
            }

            // Deallocate or dereference                                
            if (*entries) {
               auto& mutable_entries = DecvqAllCast(*entries);
               if (1 == (*entries)->GetUses())
                  Allocator::Deallocate(mutable_entries);
               else
                  mutable_entries->AddRef(-1);
               //mutable_entries = nullptr; //not allowed! we may be modifying memory owned by another container!!
            }
         }
      }

      /// Nests through all indirection layers and destroys elements and      
      /// their entries if they are fully dereferenced.                       
      /// Emergent - every indirection will be sought in the memory manager   
      /// if MANAGED_MEMORY is enabled.                                       
      ///   @attention individuals will be dereferenced if REF_INDIVIDUAL is  
      ///      enabled, regardless if an entry was found.                     
      ///   @tparam FORCE_DESTROY Destroy dense elements if true. Note:       
      ///      sparse elements are always destroyed if fully dereferenced.    
            //TODO FORCE_DESTROY no longer required?

      ///   @attention assumes there are no custom pointers involved!         
      ///   @attention doesn't change any container state or entry            
      ///   @attention works on one dimension at a time!                      
      template<bool FORCE_DESTROY = true, Cid SID = ID, CT::Container C> //requires Relevant<SID>
      void DestroyElementDeepStandardPointersEmergent(this C& self) assumptious {
         LglsAssumeDev(not self.IsEmpty(),
            "Can't destroy anything in an empty container");
         LglsAssumeDev(not self.IsDisowned(),
            "Can't destroy anything in a container without ownership");

         #if LANGULUS_FEATURE(MANAGED_MEMORY)
            Allocation const* entry = nullptr;
         #endif

         using H = Decay<decltype(LglsFake(DecideHandle<C>).template PickDimension<SID>())>;
         if constexpr (CT::TypeErased<C>) {
            //                                                          
            // Destroying a type-erased element                         
            const auto T = self.template GetType<SID>();
            LglsAssumeDev(T.IsSparse(),
               "Don't call DestroyElementDeepStandardPointersEmergent if container isn't sparse");

            const auto subT = T.GetDeptr();
            const auto ptr = *static_cast<void**>(self.template GetRaw<SID>());
            if (not ptr) 
               return;

            #if LANGULUS_FEATURE(MANAGED_MEMORY)
            entry = Allocator::Find(ptr);

            if (entry and 1 == entry->GetUses()) {
               if (subT.IsSparse()) {
                  // Pointer to pointer.                                
                  // Destroy all nested indirection layers.             
                  H temp {Stackwise, subT, ptr};
                  temp.template DestroyElementDeepStandardPointersEmergent<FORCE_DESTROY>();
               }
               else if (auto destructor = subT.GetDestructor()) {
                  // Pointer to a complete, destroyable dense.          
                  // Call the destructor.                               
                  if constexpr (REF_INDIVIDUAL) {
                     if (const auto referencer = subT.GetReferencer()) {
                        if (referencer(ptr, -1) == 0)
                           destructor(ptr);
                     }
                     else destructor(ptr);
                  }
                  else destructor(ptr);
               }
            }
            else
            #endif
            {
               if (subT.IsSparse()) {
                  // Pointer to pointer.                                
                  // Dereference all indirection layers.                
                  H temp {Stackwise, subT, ptr};
                  temp.template DestroyElementDeepStandardPointersEmergent<FORCE_DESTROY>();
               }
               else if constexpr (REF_INDIVIDUAL) {
                  if (const auto referencer = subT.GetReferencer()) {
                     // This element occurs in more than one place.     
                     // We're not allowed to deallocate the memory      
                     // behind it, but we must call destructors if T    
                     // is referencable and its individual references   
                     // have reached 0. This can happen when hive       
                     // elements are dereferenced, for example.         
                     if (referencer(ptr, -1) == 0)
                        subT.GetDestructor()(ptr);
                  }
               }
            }

            #if LANGULUS_FEATURE(MANAGED_MEMORY)
            // Deallocate or dereference                                
            if (entry) {
               if (1 == entry->GetUses())
                  Allocator::Deallocate(DecvqAllCast(entry));
               else
                  DecvqAllCast(entry)->AddRef(-1);
            }
            #endif
         }
         else {
            //                                                          
            // Destroying a statically-typed element                    
            using T = TypeOf<C, SID>;
            static_assert(CT::Sparse<T>,
               "Don't call DestroyElementDeepStandardPointersEmergent if container isn't sparse");

            using DT = Deptr<T>;
            auto& ptr = *self.template GetRawAs<T, SID>();
            if (not ptr)
               return;

            #if LANGULUS_FEATURE(MANAGED_MEMORY)
            entry = Allocator::Find(ptr);

            if (entry and 1 == entry->GetUses()) {
               if constexpr (CT::Sparse<DT>) {
                  // Pointer to pointer.                                
                  // Destroy all nested indirection layers.             
                  typename H::Denser temp{Stackwise, ptr};
                  temp.template DestroyElementDeepStandardPointersEmergent<FORCE_DESTROY>();
               }
               else if constexpr (CT::Destroyable<DT>) {
                  // Pointer to a complete, destroyable dense.          
                  // Call the destructor.                               
                  if constexpr (REF_INDIVIDUAL and CT::Referenced<DT>) {
                     if (ptr->Reference(-1) == 0)
                        ptr->~DT();
                  }
                  else ptr->~DT();
               }
            }
            else {
            #endif
            if constexpr (CT::Sparse<DT>) {
               // Pointer to pointer.                                   
               // Destroy all nested indirection layers.                
               typename H::Denser temp {Stackwise, ptr};
               temp.template DestroyElementDeepStandardPointersEmergent<FORCE_DESTROY>();
            }
            else if constexpr (REF_INDIVIDUAL and CT::Referenced<DT>) {
               // This element occurs in more than one place.           
               // We're not allowed to deallocate the memory            
               // behind it, but we must call destructors if T is       
               // referencable and its individual references have       
               // reached 0. This can happen when hive elements         
               // are dereferenced.                                     
               if (ptr->Reference(-1) == 0)
                  ptr->~DT();
            }
            #if LANGULUS_FEATURE(MANAGED_MEMORY)
            }

            // Deallocate or dereference                                
            if (entry) {
               if (1 == entry->GetUses())
                  Allocator::Deallocate(DecvqAllCast(entry));
               else
                  DecvqAllCast(entry)->AddRef(-1);
            }
            #endif
         }
      }
      
   #if LANGULUS_FEATURE(MANAGED_MEMORY)
      /// Nests through all indirection layers and destroys elements and      
      /// their entries if they are fully dereferenced.                       
      /// Relies on predeclared array of GetEntriesInner (i.e. not emergent). 
      ///   @tparam FORCE_DESTROY Destroy dense elements if true. Note:       
      ///      sparse elements are always destroyed if fully dereferenced.    
            //TODO FORCE_DESTROY no longer required?

      ///   @attention assumes container is not disowned!                     
      ///   @attention assumes there's exactly 1 use of the allocation!       
      ///   @attention doesn't change any container state                     
      ///   @attention works on one dimension at a time!                      
      template<bool FORCE_DESTROY = true, Cid SID = ID, CT::Container C> //requires Relevant<SID>
      void DestroyElementDeepCustomPointers(this C& self) assumptious {
         LglsAssumeDev(not self.IsEmpty(),
            "Can't destroy anything in an empty container");
         LglsAssumeDev(not self.IsDisowned(),
            "Can't destroy anything in a container without ownership");

         //                                                             
         // Destroying a type-erased element                            
         auto T = self.template GetType<SID>();
         LglsAssumeDev(T.IsSparse(),
            "Don't call DestroyElementDeepCustomPointers if container isn't sparse");

         auto entries = self.template GetEntriesInner<SID>();
         if (not entries)
            return;
         
         void const* src = self.template GetRaw<SID>();
         while (src and T.IsSparse()) {
            auto nextT = T.GetDeptr();
            const bool nextDense = nextT.IsDense();
            if (not nextDense) {
               // Pointer T -> Pointer nextT                            
               T.GetDereffer()(const_cast<void*>(src), &src);
            }
            else if (*entries and 1 == (*entries)->GetUses()) {
               // Pointer T -> Dense nextT, with license to destroy     
               if (auto destructor = nextT.GetDestructor()) {
                  // Pointer to a complete, destroyable dense.          
                  // Call the destructor.                               
                  src = UnpackPointer(T, nextT, src);
                  if constexpr (REF_INDIVIDUAL) {
                     if (const auto referencer = nextT.GetReferencer()) {
                        if (referencer(const_cast<void*>(src), -1) == 0)
                           destructor(const_cast<void*>(src));
                     }
                     else destructor(const_cast<void*>(src));
                  }
                  else destructor(const_cast<void*>(src));
               }
            }
            else if constexpr (REF_INDIVIDUAL) {
               // Pointer T -> Dense nextT, destroy only if deref       
               if (const auto referencer = nextT.GetReferencer()) {
                  // This element occurs in more than one place.        
                  // We're not allowed to deallocate the memory         
                  // behind it, but we must call destructors if T is    
                  // referencable and its individual references have    
                  // reached 0. This can happen when hive elements      
                  // are dereferenced.                                  
                  src = UnpackPointer(T, nextT, src);
                  if (src and referencer(const_cast<void*>(src), -1) == 0)
                     nextT.GetDestructor()(const_cast<void*>(src));
               }
            }

            // Deallocate or dereference                                
            if (*entries) {
               auto& mutable_entries = DecvqAllCast(*entries);
               if (1 == (*entries)->GetUses()) {
                  Allocator::Deallocate(mutable_entries);
                  //mutable_entries = nullptr; //TODO allowed, but probably not necessary?
               }
               else {
                  mutable_entries->AddRef(-1);
                  //mutable_entries = nullptr; //not allowed!
               }
            }

            // Move to next indirection                                 
            T = nextT;
            ++entries;
         }
      }

      /// Nests through all indirection layers and destroys elements and      
      /// their entries if they are fully dereferenced.                       
      /// Emergent - every indirection will be sought in the memory manager   
      /// if MANAGED_MEMORY is enabled.                                       
      ///   @attention individuals will be dereferenced if REF_INDIVIDUAL is  
      ///      enabled, regardless if an entry was found.                     
      ///   @tparam FORCE_DESTROY Destroy dense elements if true. Note:       
      ///      sparse elements are always destroyed if fully dereferenced.    
      //TODO FORCE_DESTROY no longer required?
      ///   @attention doesn't change any container state                     
      ///   @attention works on one dimension at a time!                      
      template<bool FORCE_DESTROY = true, Cid SID = ID, CT::Container C> //requires Relevant<SID>
      void DestroyElementDeepCustomPointersEmergent(this C& self) assumptious {
         LglsAssumeDev(not self.IsEmpty(),
            "Can't destroy anything in an empty container");
         LglsAssumeDev(not self.IsDisowned(),
            "Can't destroy anything in a container without ownership");

         //                                                             
         // Destroying a type-erased element                            
         auto T = self.template GetType<SID>();
         LglsAssumeDev(T.IsSparse(),
            "Don't call DestroyElementDeepCustomPointersEmergent if container isn't sparse");

         void const* src = self.template GetRaw<SID>();
         while (src and T.IsSparse()) {
            AllocationPtr entry;
            auto nextT = T.GetDeptr();
            const bool nextDense = nextT.IsDense();
            if (not nextDense) {
               // Pointer T -> Pointer nextT                            
               T.GetDereffer()(const_cast<void*>(src), &src);
               entry = Allocator::Find(src);
            }
            else {
               src = UnpackPointer(T, nextT, src);
               entry = Allocator::Find(src);

               if (entry and 1 == entry->GetUses()) {
                  // Pointer T -> Dense nextT, with license to destroy  
                  if (auto destructor = nextT.GetDestructor()) {
                     // Pointer to a complete, destroyable dense.       
                     // Call the destructor.                            
                     if constexpr (REF_INDIVIDUAL) {
                        if (const auto referencer = nextT.GetReferencer()) {
                           if (referencer(const_cast<void*>(src), -1) == 0)
                              destructor(const_cast<void*>(src));
                        }
                        else destructor(const_cast<void*>(src));
                     }
                     else destructor(const_cast<void*>(src));
                  }
               }
               else if constexpr (REF_INDIVIDUAL) {
                  // Pointer T -> Dense nextT, destroy only if deref    
                  if (const auto referencer = nextT.GetReferencer()) {
                     // This element occurs in more than one place.     
                     // We're not allowed to deallocate the memory      
                     // behind it, but we must call destructors if T is 
                     // referencable and its individual references have 
                     // reached 0. This can happen when hive elements   
                     // are dereferenced.                               
                     if (src and referencer(const_cast<void*>(src), -1) == 0)
                        nextT.GetDestructor()(const_cast<void*>(src));
                  }
               }
            }

            // Deallocate or dereference                                
            if (entry) {
               if (1 == entry->GetUses())
                  Allocator::Deallocate(DecvqAllCast(entry));
               else
                  DecvqAllCast(entry)->AddRef(-1);
            }

            // Move to next indirection                                 
            T = nextT;
         }
      }
   #endif

      //TODO FORCE_DESTROY no longer required?
      template<bool FORCE_DESTROY = true, Cid SID = ID, CT::Container C> //requires Relevant<SID>
      void DestroyElementDeep(this C& self) assumptious {
         if constexpr (not requires { self.template GetEntriesInner<SID>(); }) {
            #if LANGULUS_FEATURE(MANAGED_MEMORY)
               ThisCom::template DestroyElementDeepCustomPointersEmergent<FORCE_DESTROY, SID>();
            #else
               ThisCom::template DestroyElementDeepStandardPointersEmergent<FORCE_DESTROY, SID>();
            #endif
         }
         else {
            #if LANGULUS_FEATURE(MANAGED_MEMORY)
               ThisCom::template DestroyElementDeepCustomPointers<FORCE_DESTROY, SID>();
            #else
               ThisCom::template DestroyElementDeepStandardPointers<FORCE_DESTROY, SID>();
            #endif
         }
      }

      /// MARK: Services                                                      
      /// Emplace on top of the first element using an intent                 
      ///   @attention Works in one dimension at a time!                      
      ///   @attention This overwrites previous entries without dereferencing 
      ///   @attention Emplacing using a handle is faster due to carrying     
      ///      allocation data with itself when sparse, rather than searching 
      ///      for it on demand.                                              
      ///   @attention Items (not entries) are referenced even if disowned,   
      ///      when REF_INDIVIDUAL is enabled and items are CT::Referenced.   
      ///   @param intent entries will be copied/sought if handle/sparse,     
      ///      unless I is disowned                                           
      template<Cid SID = ID, CT::Container C, CT::Intent I>
      requires(/*Relevant<SID> and*/ (CT::TypeErased<C> or CT::Sparse<TypeOf<C, SID>>))
      void EmplaceEntries(this C& self, I&& intent) {
         if constexpr (CT::TypeErased<C>) {
            // If container is type-erased, we need to make a runtime   
            // sparsity check for an early exit.                        
            if (not self.template IsSparse<SID>())
               return;
         }

         decltype(auto) rhs = LglsFwd(intent.what);
         const auto indirections = self.template GetIndirections<SID>();
         const auto entries_size = sizeof(AllocationPtr) * indirections;
         const auto entries = self.template GetEntriesInner<SID>();
         constexpr bool sought = not CT::Disowned<I>;

         if constexpr (CT::Handle<I>) {
            // Copy all entries and reference them, unless we're moving 
            // a handle                                                 
            LglsAssumeDev(self.template IsSame<SID>(rhs.template GetType<SID>()),
               "Type mismatch", ": ", self.template GetType<SID>(),
               " is not same as ", rhs.template GetType<SID>()
            );

            if constexpr (CT::Disowned<I>)
               memset(DecvqAllCast(entries), 0, entries_size);
            else if constexpr (requires { rhs.template GetEntriesInner<SID>(); }) {
               // We can copy entries from RHS handle                   
               auto entries_src = rhs.template GetEntriesInner<SID>();
               if (entries_src) {
                  memcpy(DecvqAllCast(entries), entries_src, entries_size);

                  if constexpr (I::IsMoved()) {
                     // We are moving/abandoning, and we have to make   
                     // sure that source entries are zeroes, because    
                     // otherwise they will be dereferenced when H goes 
                     // out of scope.                                   
                     if constexpr (requires { rhs.template GetUses<SID>(); }) {
                        LglsAssumeDev(rhs.template GetUses<SID>() == 1,
                           "Can't move out from used memory");
                     }
                     else {
                        #if LANGULUS_FEATURE(MANAGED_MEMORY)
                           LglsAssumeDev(Allocator::Find(entries_src)->GetUses() == 1,
                              "Can't move out from used memory");
                        #endif
                     }
                     memset(DecvqAllCast(entries_src), 0, entries_size);
                  }
               }
            }

            if constexpr (not I::IsMoved() or not requires { rhs.template GetEntriesInner<SID>(); }) {
               // We are not moving or rhs is emergent, so we have to   
               // reference all elements.                               
               ThisCom::template KeepElementDeep<sought, SID>();
            }
            else if constexpr (REF_INDIVIDUAL) {
               // We are moving/abandoning, but since individual items  
               // are referenced (even if they have no corresponding    
               // entry), we need to zero the source pointers, so that  
               // we avoid them getting dereferenced later.             
               if (rhs.IsSparse()) {
                  auto pointers_src = rhs.template GetRaw<SID>();
                  if constexpr (requires { rhs.template GetUses<SID>(); }) {
                     LglsAssumeDev(rhs.template GetUses<SID>() == 1,
                        "Can't move out from used memory");
                  }
                  else {
                     #if LANGULUS_FEATURE(MANAGED_MEMORY)
                        LglsAssumeDev(Allocator::Find(pointers_src)->GetUses() == 1,
                           "Can't move out from used memory");
                     #endif
                  }
                  memset(DecvqAllCast(pointers_src), 0, rhs.template GetBytesize<SID>());
               }
            }
         }
         else if constexpr (CT::Sparse<Deint<I>>) {
            // Reference each indirection of a raw pointer              
            #if LANGULUS_FEATURE(MANAGED_MEMORY) or LANGULUS(SAFE) > 1
            using T = Decvq<Deref<Deint<I>>>;
            LglsAssumeDev((self.template IsSame<T, SID>()),
               "Type mismatch", ": ", self.template GetType<SID>(),
               " is not same as ", NameOf<T>()
            );
            #endif

            // We're forced to reference even on abandon/move           
            // because we can't abandon/move a raw pointer. Missing     
            // entries will be sought and referenced as well, unless    
            // inserted pointer is disowned.                            
            memset(DecvqAllCast(entries), 0, entries_size);
            #if LANGULUS_FEATURE(MANAGED_MEMORY)
               if constexpr (CT::CustomPointer<T>)
                  ThisCom::template KeepElementDeepCustomPointers<sought, SID>();
               else
                  ThisCom::template KeepElementDeepStandardPointers<sought, SID>();
            #else
               ThisCom::template KeepElementDeepStandardPointers<sought, SID>();
            #endif
         }
      }

      /// Reference all entries                                               
      ///   @attention operates on all relevant dimensions at once!           
      template<CT::Container C>
      void Keep(this C& self) assumptious {
         LglsAssumeDev(not self.IsDisowned(),
            "Can't keep anything in a container without ownership");

         if (self.IsEmpty())
            return;

         // Reference all indirections and (optionally) items           
         Id::ForEach([&self]<Cid D> {
            if constexpr (CT::TypeErased<C>) {
               if (self.template IsSparse<D>()) {
                  self.Apply([](auto&& item) {
                     item.template KeepElementDeep<false, D>();
                  });
               }
            }
            else if constexpr (CT::Sparse<TypeOf<C, D>>) {
               self.Apply([](auto&& item) {
                  item.template KeepElementDeep<false, D>();
               });
            }
         });
      }

      /// Dereferences all entries and destroy all indirections whose entries 
      /// were fully dereferenced.                                            
      ///   @tparam DEALLOCATE not used, here only for ABI compatibility      
      ///   @attention this never modifies any state                          
      ///   @attention operates on all relevant dimensions at once!           
      template<bool DEALLOCATE = true, CT::Container C>
      void Free(this C& self) assumptious {
         LglsAssumeDev(not self.IsDisowned(),
            "Can't keep anything in a container without ownership");

         if (self.IsEmpty())
            return;

         // Reference all indirections and (optionally) items           
         Id::ForEach([&self]<Cid D> {
            if constexpr (CT::TypeErased<C>) {
               if (self.template IsSparse<D>()) {
                  self.Apply([](auto&& item) {
                     item.template DestroyElementDeep<false, D>();
                  });
               }
            }
            else if constexpr (CT::Sparse<TypeOf<C, D>>) {
               self.Apply([](auto&& item) {
                  item.template DestroyElementDeep<false, D>();
               });
            }
         });

         if constexpr (not CT::Owned<C> and CT::HeapAllocated<C>) {
            // The container has deep ownership, but no shallow         
            // ownership. We are allowed to destroy immediate elements  
            // here, because it won't happen otherwise, as long as the  
            // elements are on the heap.                                
            Id::ForEach([&self]<Cid D> {
               if constexpr (CT::TypeErased<C>) {
                  const auto T = self.template GetType<D>();
                  if (const auto destructor = T.GetDestructor()) {
                     self.Apply([&destructor, &T](auto&& item) {
                        (void) T;
                        const auto ptr = item.template GetRaw<D>();
                        if constexpr (REF_INDIVIDUAL) {
                           IF_SAFE(if (const auto referencer = T.GetReferencer())
                              referencer(ptr, -1));
                        }
                        destructor(ptr);
                     });
                  }
               }
               else {
                  using T = TypeOf<C, D>;
                  if constexpr (CT::Destroyable<T>) {
                     self.Apply([](auto&& item) {
                        auto* element = item.template Get<void, D>();
                        IF_SAFE(if constexpr (REF_INDIVIDUAL and CT::Referenced<T>)
                           element->Reference(-1));
                        element->~T();
                     });
                  }
               }
            });
         }
      }

      /// Called on container destruction                                     
      ///   @attention this never modifies any state                          
      ///   @attention operates on all relevant dimensions at once!           
      template<class SELF>
      void Destroy(this SELF& self) assumptious requires ((STYLE & OnCreateAndDestroy) != 0) {
         ThisCom::Free();
      }

      /// Reset all entries for a given dimension.                            
      /// Enabled only if not emergent ownership.                             
      ///   @attention this zeroes entries without dereferencing them         
      template<Cid SID = ID, CT::Container C>
      void ResetEntries(this C& self) assumptious //TODO is setting count to 0 a better alternative to this?
      requires requires { self.template GetEntriesInner<SID>(); } {
         if constexpr (CT::TypeErased<C>) {
            // If container is type-erased, we need to make a runtime   
            // sparsity check for an early exit.                        
            if (not self.template IsSparse<SID>())
               return;
         }

         static_assert(CT::ContainsOne<C>,
            "Resetting entries for first element in a container with many. "
            "GetHandle() first?");

         if constexpr (not CT::Handle<C>) {
            LglsAssumeDev(self.template GetUses<SID>() == 1,
               "ResetEntries shouldn't be called for shared memory");
         }

         const auto indirections = self.template GetIndirections<SID>();
         size_t entries_size;
         if constexpr (requires { self.template GetReserved<SID>(); })
            entries_size = sizeof(AllocationPtr) * indirections * self.template GetReserved<SID>();
         else
            entries_size = sizeof(AllocationPtr) * indirections;

         auto entries = self.template GetEntriesInner<SID>();
         memset(DecvqAllCast(entries), 0, entries_size);
      }

      /// Executes ResetEntries for all relevant dimensions                   
      ///   @attention This zeroes entries without dereferencing them.        
      template<class SELF>
      void ResetAllEntries(this SELF& self) assumptious { //TODO is setting count to 0 a better alternative to this?
         using C = OwnershipDeepEmergent<STYLE, REF_INDIVIDUAL, ID, SHARED...>;
         Id::ForEach([&]<Cid D> assumptious {
            if_available_gcc(C::template ResetEntries<D, SELF>)();
         });
      }

      /// Refer all allocations pointed to by all indirections on absorption. 
      ///   @note This does emergent referencing. This method is replaced     
      ///      with cached referencing in derived components.                 
      ///   @param intent The intent and container to transfer from.          
      ///   @important Notice that Copy and Clone intents are not handled     
      ///      here. They're handled in heap components instead, in case      
      ///      something throws an exception while constructing.              
      template<class SELF, CT::Intent I>
      requires (CT::Container<I> and (STYLE & OnCreateAndDestroy) != 0
           and not CT::Copied<I> and not CT::Cloned<I> and not CT::Disowned<I>
           and CT::HeapAllocated<I>
           and (CT::TypeErased<Deint<I>> or CT::Sparse<TypeOf<Deint<I>, ID>>))
      void ConstructFrom(this SELF& self, I&& intent) {
         using IT = Decvq<Deref<Deint<I>>>;
         decltype(auto) from = LglsFwd(intent.what);

         if constexpr (CT::Referred<I> or (from.OwnedDeep & OnCreateAndDestroy) == 0) {
            // Refer                                                    
            ThisCom::Keep();
         }
         else if constexpr (CT::Moved<I> or CT::Abandoned<I>) {
            // Move/Abandon                                             
            if (from.IsDisowned()) {
               // Right was never owned, now we own it                  
               ThisCom::Keep();
            }
            else if constexpr (CT::Moved<I> or not IT::CanBeDisowned) {
               // Transfer ownership if we can, otherwise refer         
               // Deep ownership can be reset in two ways: either reset 
               // the entries pointer, or reset the count.              
               if constexpr (CT::HasVariableCount<I>) {
                  LglsAssumeDev(from.IsEmpty(),
                     "Remote count should've been reset prior to this call");
               }
               else if_available(from.template SetEntriesInner<ID>(nullptr))
               else ThisCom::Keep();
            }
         }
      }
   };

   #undef ThisCom
}
