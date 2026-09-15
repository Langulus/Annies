///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Handle.hpp"

#define TEMPLATE()   template<class T, bool EMBED>
#define HAND()       Handle<T, EMBED>


namespace Langulus::Annies
{

   /// Create an embedded handle while searching for memory entry             
   ///   @param v - pointer to the element's position inside a Block          
   TEMPLATE() LANGULUS(INLINED)
   constexpr HAND()::Handle(ValueType v) noexcept requires (Embedded or Sparse)
      : mValue {v} {
      if constexpr (TypeErased or CT::Allocatable<T>) {
         mEntry = Allocator::Find(MetaDataOf<T>(), mValue);

         if constexpr (Sparse and not Embedded) {
            // Always add a reference when we contain a pointer in a    
            // local handle, because otherwise we might lose the data   
            // THIS IS THE ONLY CASE WHERE A HANDLE EXERCISES OWNERSHIP 
            if (mEntry) {
               const_cast<Allocation*>(mEntry)->Keep();
               if constexpr (CT::Referencable<Deptr<T>>)
                  DecvqCast(mValue)->Reference(1);
            }
         }
      }
      else mEntry = nullptr;
   }

   /// Create an embedded handle by manually specifying entry                 
   ///   @param v - pointer to the element's position inside a Block          
   ///   @param e - pointer to the element's entry inside a Block             
   TEMPLATE() LANGULUS(INLINED)
   constexpr HAND()::Handle(ValueType v, EntryType e) noexcept requires (Embedded or Sparse)
      : mValue {v}
      , mEntry {e} {}

   /// Construct using handle of any compatible type                          
   ///   @param other - the handle and intent to construct with               
   TEMPLATE() template<template<class> class S, CT::Handle H>
   requires CT::IntentMakable<S, T> LANGULUS(INLINED)
   constexpr HAND()::Handle(S<H>&& other)
      : mValue (S<T>(other->Get()))
      , mEntry {nullptr}
   {
      using HT = TypeOf<H>;
      static_assert(CT::Similar<T, HT>, "Type mismatch");

      if constexpr (not Embedded) {
         if constexpr (S<H>::Shallow) {
            // Copy/Refer/Disown/Move/Abandon a handle                  
            if constexpr (S<H>::Move) {
               // Always reset remote entry, when moving (if sparse)    
               if constexpr (CT::Sparse<HT>) {
                  mEntry = other->GetEntry();
                  other->GetEntry() = nullptr;
               }

               // Also reset remote value, if not an abandoned pointer  
               if constexpr (S<H>::Keep and CT::Sparse<HT>)
                  other->Get() = nullptr;
            }
         }
         else {
            // Clone a handle                                           
            TODO();
         }
      }
      else mEntry = other->GetEntry();
   }
      
   TEMPLATE() template<class A>
   constexpr HAND()::Handle(A&& argument) noexcept requires (not Embedded)
      : mValue (Forward<A>(argument))
      , mEntry {nullptr}
   {
      using S = IntentOf<decltype(argument)>;
      using DT = Deptr<T>;
      if constexpr (S::Keep and Sparse and CT::Complete<DT>) {
         if constexpr (CT::Allocatable<DT>)
            mEntry = Allocator::Find(MetaDataOf<DT>(), mValue);
      }

      if constexpr (Sparse) {
         // Always add a reference when we transition a pointer from    
         // an embedded handle, to a non-embedded one, because otherwise
         // we'll lose the data                                         
         // THIS IS THE ONLY CASE WHERE A HANDLE EXERCISES OWNERSHIP    
         if (mEntry) {
            const_cast<Allocation*>(mEntry)->Keep();
            if constexpr (CT::Referencable<DT>)
               DecvqCast(mValue)->Reference(1);
         }
      }
   }

   /// Handle destructor                                                      
   TEMPLATE()
   HAND()::~Handle() {
      if constexpr (not Embedded and not Sparse and CT::Referencable<T>) {
         // Will call value destructor at end of scope, just suppress   
         // the reference warning                                       
         IF_SAFE(DecvqCast(mValue).Reference(-1));
      }

      if constexpr (Sparse and not Embedded) {
         // THIS IS THE ONLY CASE WHERE A HANDLE EXERCISES OWNERSHIP    
         if (mEntry) {
            const_cast<Allocation*>(mEntry)->Free();
            if constexpr (CT::Referencable<Deptr<T>>)
               DecvqCast(mValue)->Reference(-1);
         }
      }
   }

   /// Compare a handle with a comparable value/handle                        
   ///   @attention this compares contents and isn't suitable for iteration   
   TEMPLATE()
   constexpr bool HAND()::operator == (const auto& rhs) const
   noexcept requires (not TypeErased or Sparse) {
      using RHS = Deref<decltype(rhs)>;
      if constexpr (CT::Handle<RHS> and CT::Comparable<T, TypeOf<RHS>>) {
         if constexpr (Embedded) {
            if constexpr (RHS::Embedded)  return *mValue == *rhs.mValue;
            else                          return *mValue == rhs.mValue;
         }
         else {
            if constexpr (RHS::Embedded)  return  mValue == *rhs.mValue;
            else                          return  mValue == rhs.mValue;
         }
      }
      else if constexpr (CT::Comparable<T, RHS>) {
         if constexpr (Embedded)
            return *mValue == rhs;
         else
            return  mValue == rhs;
      }
      else static_assert(false, "Can't compare");
   }
         
   /// Prefix increment operator                                              
   ///   @return the next handle                                              
   TEMPLATE() LANGULUS(INLINED)
   auto HAND()::operator ++ () noexcept -> Handle& requires Embedded {
      ++mValue;
      if constexpr (CT::Sparse<T>)
         ++mEntry;
      return *this;
   }

   /// Prefix decrement operator                                              
   ///   @return the next handle                                              
   TEMPLATE() LANGULUS(INLINED)
   auto HAND()::operator -- () noexcept -> Handle& requires Embedded {
      --mValue;
      if constexpr (CT::Sparse<T>)
         --mEntry;
      return *this;
   }

   /// Suffix increment operator                                              
   ///   @return the previous value of the handle                             
   TEMPLATE() LANGULUS(INLINED)
   auto HAND()::operator ++ (int) const noexcept -> Handle requires Embedded {
      auto backup = *this;
      return ++backup;
   }

   /// Suffix decrement operator                                              
   ///   @return the previous value of the handle                             
   TEMPLATE() LANGULUS(INLINED)
   auto HAND()::operator -- (int) const noexcept -> Handle requires Embedded {
      auto backup = *this;
      return --backup;
   }

   /// Offset the handle                                                      
   ///   @param offset - the offset to apply                                  
   ///   @return the offsetted handle                                         
   TEMPLATE() LANGULUS(INLINED)
   auto HAND()::operator + (Offset offset) const noexcept -> Handle requires Embedded {
      auto backup = *this;
      return backup += offset;
   }

   /// Offset the handle                                                      
   ///   @param offset - the offset to apply                                  
   ///   @return the offsetted handle                                         
   TEMPLATE() LANGULUS(INLINED)
   auto HAND()::operator - (Offset offset) const noexcept -> Handle requires Embedded {
      auto backup = *this;
      return backup -= offset;
   }
   
   /// Prefix increment operator                                              
   ///   @return the next handle                                              
   TEMPLATE() LANGULUS(INLINED)
   auto HAND()::operator += (Offset offset) noexcept -> Handle& requires Embedded {
      mValue += offset;
      if constexpr (CT::Sparse<T>)
         mEntry += offset;
      return *this;
   }

   /// Prefix decrement operator                                              
   ///   @return the next handle                                              
   TEMPLATE() LANGULUS(INLINED)
   auto HAND()::operator -= (Offset offset) noexcept -> Handle& requires Embedded {
      mValue -= offset;
      if constexpr (CT::Sparse<T>)
         mEntry -= offset;
      return *this;
   }

   /// Get a reference to the contents                                        
   TEMPLATE() LANGULUS(INLINED)
   auto HAND()::Get() noexcept -> Type& {
      if constexpr (Embedded) return *mValue;
      else                    return  mValue;
   }
   
   TEMPLATE() LANGULUS(INLINED)
   auto HAND()::Get() const noexcept -> const Type& {
      if constexpr (Embedded) return *mValue;
      else                    return  mValue;
   }
   
   /// Get the entry                                                          
   TEMPLATE() LANGULUS(INLINED)
   auto HAND()::GetEntry() noexcept -> AllocType& {
      if constexpr (Embedded and Sparse)  return *mEntry;
      else                                return  mEntry;
   }

   TEMPLATE() LANGULUS(INLINED)
   auto HAND()::GetEntry() const noexcept -> AllocType const& {
      if constexpr (Embedded and Sparse)  return *mEntry;
      else                                return  mEntry;
   }

   /// Instantiate anything at the handle, with or without an intent          
   ///   @attention this overwrites previous handle without dereferencing it, 
   ///      and without destroying anything - that's your responsibility      
   ///   @param rhs - what are we instantiating                               
   ///   @param type - type of the contained data, used only if handle is     
   ///      type-erased                                                       
   TEMPLATE() LANGULUS(INLINED)
   void HAND()::CreateWithIntent(auto&& rhs, DMeta type) {
      using S  = IntentOf<decltype(rhs)>;
      using ST = TypeOf<S>;

      if constexpr (TypeErased) {
         LANGULUS_ASSUME(DevAssumes, type,
            "Invalid type provided for type-erased handle");

         if (type->mIsSparse) {
            if constexpr (S::Shallow) {
               // Do a copy/disown/abandon/move sparse LHS              
               if constexpr (CT::Handle<ST>) {
                  // RHS is a handle                                    
                  using HT = TypeOf<ST>;
                  static_assert(CT::Sparse<T> == CT::Sparse<HT>);
                  Get() = rhs->Get();

                  if constexpr (S::Keep or S::Move)
                     GetEntry() = rhs->GetEntry();
                  else
                     GetEntry() = nullptr;

                  if constexpr (S::Move) {
                     // We're moving from an embedded RHS, so we need   
                     // to clear it up - we're transferring ownership   
                     if constexpr (S::Keep)
                        rhs->Get() = nullptr;
                     rhs->GetEntry() = nullptr;
                  }
                  else if constexpr (S::Keep and Embedded) {
                     // Copying RHS, but keep it only if not disowning  
                     if (GetEntry()) {
                        const_cast<Allocation*>(GetEntry())->Keep();
                        if (type->mReference)
                           type->mReference(Get(), 1);
                     }
                  }
               }
               else if constexpr (CT::Nullptr<ST>) {
                  // RHS is a simple nullptr                            
                  Get() = nullptr;
                  GetEntry() = nullptr;
               }
               else {
                  // RHS is not a handle, but we'll wrap it in a handle,
                  // in order to find its entry (if managed memory is   
                  // enabled)                                           
                  static_assert(CT::Sparse<T> == CT::Sparse<ST>);
                  HandleLocal<T> rhsh {rhs.Forward()};
                  Get() = rhsh.Get();
                  GetEntry() = rhsh.GetEntry();

                  if constexpr (S::Keep and Embedded) {
                     // Raw pointers are always referenced, even when   
                     // moved (as long as it's a keeper intent)         
                     if (GetEntry()) {
                        const_cast<Allocation*>(GetEntry())->Keep();
                        if (type->mReference)
                           type->mReference(Get(), 1);
                     }
                  }
               }
            }
            else {
               //TODO clone pointers
               TODO();
            }
         }
         else {
            // Do a copy/disown/abandon/move/clone inside a dense handle
            if constexpr (CT::Handle<ST>) {
               // RHS is a handle                                       
               using HT = TypeOf<ST>;
               static_assert(CT::Sparse<T> == CT::Sparse<HT>);
               TODO();
            }
            else {
               static_assert(CT::Sparse<T> == CT::Sparse<ST>);

               if constexpr (S::Move) {
                  if constexpr (S::Keep)
                     type->mMoveAssigner(&Get(), &*rhs);
                  else
                     type->mAbandonAssigner(&Get(), &*rhs);
               }
               else if constexpr (S::Shallow) {
                  if constexpr (S::Keep) {
                     if constexpr (CT::Referred<S>)
                        type->mReferAssigner(&Get(), const_cast<void*>(reinterpret_cast<const void*>(&*rhs)));
                     else
                        type->mCopyAssigner(&Get(), &*rhs);
                  }
                  else type->mDisownAssigner(&Get(), &*rhs);
               }
               else type->mCloneAssigner(&Get(), &*rhs);
            }
         }
      }
      else {
         if constexpr (S::Shallow and CT::Sparse<T>) {
            // Do a copy/refer/disown/abandon/move sparse RHS           
            if constexpr (CT::Handle<ST>) {
               // RHS is a handle                                       
               using HT = TypeOf<ST>;
               static_assert(CT::Similar<T, HT>, "Handle type mismatch");
               Get() = rhs->Get();

               if constexpr (S::Keep or S::Move)
                  GetEntry() = rhs->GetEntry();
               else
                  GetEntry() = nullptr;

               if constexpr (S::Move) {
                  // We're moving from an embedded RHS, so we need      
                  // to clear it up - we're transferring ownership      
                  if constexpr (S::Keep)
                     rhs->Get() = nullptr;
                  rhs->GetEntry() = nullptr;
               }
               else if constexpr (S::Keep and Embedded) {
                  // Copying RHS, but keep it only if not disowning it  
                  if (GetEntry()) {
                     const_cast<Allocation*>(GetEntry())->Keep();
                     if constexpr (CT::Referencable<Deptr<T>>)
                        DecvqCast(Get())->Reference(1);
                  }
               }
            }
            else if constexpr (CT::Nullptr<ST>) {
               // RHS is a simple nullptr                               
               Get() = nullptr;
               GetEntry() = nullptr;
            }
            else if constexpr (CT::MakableFrom<T, ST>) {
               using DT = Deptr<T>;
               Get() = DeintCast(rhs);
               if constexpr (CT::Allocatable<DT> and (S::Keep or S::Move))
                  GetEntry() = Allocator::Find(MetaDataOf<DT>(), Get());
               else
                  GetEntry() = nullptr;

               if constexpr (S::Keep and Embedded) {
                  // Raw pointers are always referenced, even when moved
                  // (as long as it's a keeper intent)                  
                  if (GetEntry()) {
                     const_cast<Allocation*>(GetEntry())->Keep();
                     if constexpr (CT::Referencable<Deptr<T>>)
                        DecvqCast(Get())->Reference(1);
                  }
               }
            }
            else static_assert(false, "Can't initialize sparse T");
         }
         else if constexpr (CT::Dense<T>) {
            // Do a copy/disown/abandon/move/clone inside a dense handle
            if constexpr (CT::Handle<ST> and CT::MakableFrom<T, TypeOf<ST>>)
               new ((void*) &Get()) T(S::Nest(rhs->Get()));
            else if constexpr (CT::MakableFrom<T, S>)
               new ((void*) &Get()) T(S::Nest(rhs));
            else
               static_assert(false, "Can't initialize dense T");
         }
         else if constexpr (CT::Dense<Deptr<T>>) {
            // Clone sparse/dense data                                  
            if constexpr (CT::Resolvable<Decay<T>>) {
               // If T is resolvable, we need to always clone the       
               // resolved (a.k.a the most concrete) type               
               TODO();
            }
            else {
               // Otherwise attempt cloning DT conventionally           
               using DT = Decay<T>;
               auto meta = MetaDataOf<DT>();
               auto entry = Allocator::Allocate(meta, meta->RequestSize(1).mByteSize);
               auto pointer = entry->template As<DT>();

               if constexpr (CT::Handle<ST>) {
                  static_assert(CT::Similar<T, TypeOf<ST>>, "Type mismatch");
                  IntentNew(pointer, S::Nest(*rhs->Get()));
               }
               else {
                  static_assert(CT::Similar<T, ST>, "Type mismatch");
                  IntentNew(pointer, S::Nest(**rhs));
               }

               Get() = pointer;
               GetEntry() = entry;
            }
         }
         else {
            // Pointers of pointers                                     
            // Clone indirection layers by nesting                      
            TODO();
         }
      }
   }
   
   /// Refer-assign a new value and entry at the handle                       
   ///   @attention this overwrites previous handle without dereferencing it, 
   ///      and without destroying anything                                   
   ///   @param value - the new value to assign                               
   ///   @param entry - the allocation that the value is part of              
   TEMPLATE() LANGULUS(INLINED)
   void HAND()::Assign(const Type& value, AllocType entry) noexcept requires (Embedded and Mutable) {
      Get() = value;
      GetEntry() = entry;
   }
   
   /// Dereference/destroy the current handle contents, and set new ones      
   ///   @param rhs - new contents to assign                                  
   ///   @param type - type of the contained data, used only if handle is     
   ///      type-erased                                                       
   TEMPLATE() LANGULUS(INLINED)
   void HAND()::AssignWithIntent(auto&& rhs, DMeta type) requires Mutable {
      using S = IntentOf<decltype(rhs)>;
      FreeInner(type);
      CreateWithIntent(S::Nest(rhs), type);
   }
   
   /// Swap any two handles, gracefully handling transitions between embedded 
   /// and unembedded handles                                                 
   ///   @param rhs - right hand side                                         
   ///   @param type - type of the contained data, used only if handle is     
   ///      type-erased                                                       
   TEMPLATE() LANGULUS(INLINED)
   void HAND()::Swap(CT::Handle auto& rhs, DMeta type) requires Mutable {
      if constexpr (Sparse) {
         std::swap(Get(), rhs.Get());
         std::swap(GetEntry(), rhs.GetEntry());
      }
      else {
         HandleLocal<T> tmp {Abandon(*this)};
         FreeInner<false, true>(type);
         CreateWithIntent(Abandon(rhs), type);
         rhs.CreateWithIntent(Abandon(tmp), type);
      }
   }

   /// Compare the contents of the handle with content                        
   ///   @param rhs - data to compare against                                 
   ///   @param type - type of the contained data, used only if handle is     
   ///      type-erased                                                       
   ///   @return true if contents are equal                                   
   TEMPLATE() LANGULUS(INLINED)
   bool HAND()::Compare(const auto& rhs, DMeta) const {
      using RHS = Deref<decltype(rhs)>;

      if constexpr (CT::Handle<RHS>)
         return Get() == rhs.Get();
      else
         return Get() == rhs;
   }

   /// Reset the handle, by dereferencing entry, and destroying value, if     
   /// entry has been fully dereferenced                                      
   /// Does absolutely nothing for dense (unembedded???) handles, they are destroyed when     
   /// handle is destroyed (TODO look at note in func body)                                                    
   ///   @tparam RESET - whether or not to reset pointers to null             
   ///   @tparam DEALLOCATE - are we allowed to deallocate the memory?        
   ///   @param meta - type of the contained data, used only if handle is     
   ///      type-erased                                                       
   TEMPLATE() template<bool RESET, bool DEALLOCATE>
   void HAND()::FreeInner(DMeta meta) requires Mutable {
      if constexpr (TypeErased) {
         LANGULUS_ASSUME(DevAssumes, meta,
            "Invalid type provided for type-erased handle");

         if constexpr (Sparse) {
            // Handle is sparse, we should handle each indirection layer
            LANGULUS_ASSUME(DevAssumes, meta->mIsSparse,
               "Provided meta must match T sparseness");

            if (GetEntry()) {
               if (1 == GetEntry()->GetUses()) {
                  // This is the last occurence of that element         
                  LANGULUS_ASSUME(DevAssumes, Get(), "Null pointer");

                  if (meta->mDeptr->mIsSparse) {
                     // Pointer to pointer                              
                     // Release all nested indirection layers           
                     HandleLocal<void*> {Get()}.FreeInner(meta->mDeptr);
                  }
                  else if (meta->mDestructor) {
                     // Pointer to a complete, destroyable dense        
                     // Call the destructor                             
                     if (meta->mReference) {
                        if (meta->mReference(Get(), -1) == 0)
                           meta->mDestructor(Get());
                     }
                     else meta->mDestructor(Get());
                  }

                  if constexpr (DEALLOCATE)
                     Allocator::Deallocate(const_cast<Allocation*>(GetEntry()));
               }
               else {
                  // This element occurs in more than one place         
                  // We're not allowed to deallocate the memory behind  
                  // it, but we must call destructors if T is           
                  // referencable, and its individual references have   
                  // reached 0. This usually happens when elements from 
                  // a THive are referenced.                            
                  if (not meta->mDeptr->mIsSparse and meta->mReference) {
                     if (meta->mReference(Get(), -1) == 0)
                        meta->mDestructor(Get());
                  }

                  const_cast<Allocation*>(GetEntry())->Free();
               }
            }

            if constexpr (RESET) {
               // Handle is dense and embedded, we should call remote   
               // destructor, but don't touch the entry, its irrelevant 
               const_cast<Type&>(Get()) = nullptr;
               const_cast<AllocType&>(GetEntry()) = nullptr;
            }
         }
      }
      else {
         using DT = Decay<T>;

         if constexpr (Sparse) {
            // Handle is sparse, we should handle each indirection layer
            if (GetEntry()) {
               if (1 == GetEntry()->GetUses()) {
                  // This is the last occurence of that element         
                  LANGULUS_ASSUME(DevAssumes, Get(), "Null pointer");

                  if constexpr (CT::Sparse<Deptr<T>>) {
                     // Pointer to pointer                              
                     // Release all nested indirection layers           
                     HandleLocal<Deptr<T>> {*Get()}.FreeInner();
                  }
                  else if constexpr (not CT::Complete<DT> and not CT::Function<DT>) {
                     // CT::Destroyable<DT> will fail silently if DT    
                     // isn't defined yet, causing nasty leaks. So make 
                     // it not-so-silent...                             
                     static_assert(false, "Attempting to destroy an incomplete type");
                  }
                  else if constexpr (CT::Destroyable<DT>) {
                     // Pointer to a complete, destroyable dense        
                     // Call the destructor                             
                     if constexpr (CT::Referencable<DT>) {
                        if (DecvqCast(Get())->Reference(-1) == 0)
                           Get()->~DT();
                     }
                     else Get()->~DT();
                  }

                  if constexpr (DEALLOCATE)
                     Allocator::Deallocate(const_cast<Allocation*>(GetEntry()));
               }
               else {
                  // This element occurs in more than one place         
                  // We're not allowed to deallocate the memory behind  
                  // it, but we must call destructors if T is           
                  // referencable, and its individual references have   
                  // reached 1. This usually happens when elements from 
                  // a THive are referenced.                            
                  if constexpr (CT::Dense<Deptr<T>> and CT::Referencable<DT>) {
                     if (DecvqCast(Get())->Reference(-1) == 0)
                        Get()->~DT();
                  }

                  const_cast<Allocation*>(GetEntry())->Free();
               }
            }

            if constexpr (RESET) {
               const_cast<Type&>(Get()) = nullptr;
               const_cast<AllocType&>(GetEntry()) = nullptr;
            }
         }
         else if constexpr (not CT::Complete<DT> and not CT::Function<DT>) {
            // CT::Destroyable<DT> will fail silently if DT isn't       
            // defined yet, causing nasty leaks. So make it             
            // not-so-silent...                                         
            static_assert(false, "Attempting to destroy an incomplete type");
         }
         else if constexpr (EMBED and CT::Destroyable<DT>) {
            // Handle is dense and embedded, we should call the remote  
            // destructor, but don't touch the entry, its irrelevant    
            //TODO the function above states that this does nothing if dense, but apparently that isn't true
            // firgure it out!
            if constexpr (CT::Referencable<DT>)
               Get().Reference(-1);
            Get().~DT();
         }
      }
   }

} // namespace Langulus::Annies

#undef TEMPLATE
#undef HAND
