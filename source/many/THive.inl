///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "THive.hpp"
#include "TMany.inl"

#define TEMPLATE()   template<CT::NotVoid T>
#define TME()        THive<T>


namespace Langulus::Annies
{

   /// Refer-constructor                                                      
   ///   @param other - the hive to refer to                                  
   TEMPLATE() LANGULUS(INLINED)
   TME()::THive(const THive& other)
      : THive {Refer(other)} {}

   /// Move-constructor                                                       
   ///   @param other - the hive to move                                      
   TEMPLATE() LANGULUS(INLINED)
   TME()::THive(THive&& other)
      : THive {Move(other)} {}

   /// Intent constructor                                                     
   ///   @param other - the hive and intent to use                            
   TEMPLATE() template<template<class> class S>
   requires CT::Intent<S<THive<T>>> LANGULUS(INLINED)
   THive<T>::THive(S<THive>&&) {
      TODO();
   }

   /// Hive destructor                                                        
   TEMPLATE() LANGULUS(INLINED)
   TME()::~THive() {
      static_assert(CT::Complete<T>,     "T must be a complete type");
      static_assert(CT::Dense<T>,        "T must be a dense type");
      static_assert(CT::Data<T>,         "T can't be void");
      static_assert(CT::Referencable<T>, "T must be referencable");
      static_assert(not CT::Abstract<T>, "T can't be abstract");
      ResetInner();
   }

   /// Refer assignment                                                       
   ///   @param other - the hive to refer to                                  
   TEMPLATE() LANGULUS(INLINED)
   auto TME()::operator = (const THive& other) -> THive& {
      return operator = (Refer(other));
   }

   /// Move assignment                                                        
   ///   @param other - the hive to move                                      
   TEMPLATE() LANGULUS(INLINED)
   auto TME()::operator = (THive&& other) -> THive& {
      return operator = (Move(other));
   }

   /// Intent assignment                                                      
   ///   @param other - the hive to assign                                    
   TEMPLATE() template<template<class> class S> 
   requires CT::Intent<S<THive<T>>> LANGULUS(INLINED)
   auto THive<T>::operator = (S<THive>&& other) -> THive& {
      mFrames = other.Nest(other->mFrames);
      mReusable = other->mReusable;
      mCount = other->mCount;

      if constexpr (ResetsOnMove<S>()) {
         other->mCount = 0;
         other->mReusable = nullptr;
      }
      return *this;
   }

   /// Reset the factory                                                      
   TEMPLATE() LANGULUS(INLINED)
   bool TME()::IsEmpty() const noexcept {
      return mCount == 0;
   }

   /// Explicit bool cast operator, for use in if statements                  
   ///   @return true if block contains at least one valid element            
   TEMPLATE() LANGULUS(INLINED)
   constexpr TME()::operator bool() const noexcept {
      return not IsEmpty();
   }

   /// Returns a valid pointer to the frame that owns the memory pointer, if  
   /// that memory pointer is at all owned by this hive                       
   ///   @param ptr - the pointer to check                                    
   ///   @return nullptr if not found, or a pointer to the owning frame       
   TEMPLATE() LANGULUS(INLINED)
   auto TME()::Owns(const void* ptr) const noexcept -> const Frame* {
      for (auto& frame : mFrames)
         if (frame.Owns(ptr))
            return &frame;
      return nullptr;
   }

   /// Get the type of the contained data                                     
   ///   @return the meta data                                                
   TEMPLATE() LANGULUS(INLINED)
   auto TME()::GetType() const noexcept -> DMeta {
      return MetaDataOf<T>();
   }

   /// Get the number of initialized cells                                    
   ///   @return the number of valid entries                                  
   TEMPLATE() LANGULUS(INLINED)
   auto TME()::GetCount() const noexcept -> Count {
      return mCount;
   }

   /// Get iterator to first element                                          
   ///   @return an iterator to the first element, or end if empty            
   TEMPLATE() LANGULUS(INLINED)
   constexpr auto THive<T>::begin() noexcept -> Iterator<true> {
      if (IsEmpty())
         return end();

      auto& firstFrame = mFrames[0];
      return {
         firstFrame.GetRaw(),
         firstFrame.GetRaw() + firstFrame.GetReserved(),
         &firstFrame,
         &firstFrame + mFrames.GetCount()
      };
   }

   TEMPLATE() LANGULUS(INLINED)
   constexpr auto THive<T>::begin() const noexcept -> Iterator<false> {
      return const_cast<THive*>(this)->begin();
   }

   /// Emplace a new instance inside the hive                                 
   ///   @param args... - arguments to forward to T's constructor             
   ///   @return a pointer to the newly constructed instance of T             
   TEMPLATE()
   template<class...A> requires ::std::constructible_from<T, A...>
   auto THive<T>::New(A&&...args) -> T* {
      return &(NewInner(Forward<A>(args)...)->mData);
   }

   /// Emplace a new instance inside the hive                                 
   ///   @param args... - arguments to forward to T's constructor             
   ///   @return a pointer to the newly constructed instance of T             
   TEMPLATE()
   template<class...A> requires ::std::constructible_from<T, A...>
   auto THive<T>::NewInner(A&&...args) -> Cell* {
      Cell* result;

      if (mReusable) {
         // Reuse a slot                                                
         const auto nextReusable = mReusable->mNextFreeCell;
         try { result = new (mReusable) Cell {Forward<A>(args)...}; }
         catch (...) {
            mReusable->mNextFreeCell = nextReusable;
            return nullptr;
         }

         auto frame = Owns(mReusable);
         const_cast<Frame*>(frame)->mCount += 1;

         // Make sure that the mReusable is inside limits, as it may    
         // go out of bounds in edge cases                              
         mReusable->mNextFreeCell = nullptr;
         mReusable = Owns(nextReusable) ? nextReusable : nullptr;
      }
      else {
         // Add new frame                                               
         const auto nextReserved = not mFrames.IsEmpty()
            ? mFrames.Last().GetReserved() * 2
            : DefaultFrameSize;

         // Use first cell to initialize our object                     
         Frame* volatile frame = nullptr;
         try {
            mFrames.New(1);
            frame = &mFrames.Last();
            frame->Reserve(nextReserved);

            result = new (frame->GetRaw()) Cell {Forward<A>(args)...};
         }
         catch (...) {
            // Pass through all new unused cells, and set their markers 
            // We allocated a new frame, let's not let it go to waste   
            if (frame->GetRaw()) {
               mReusable = frame->GetRaw();
               const auto cellEnd = frame->GetRaw() + frame->GetReserved();
               for (auto cell = mReusable; cell < cellEnd; ++cell)
                  cell->mNextFreeCell = cell + 1;
            }
            return nullptr;
         }

         ++frame->mCount;

         // Pass through all new unused cells, and set their markers    
         mReusable = frame->GetRaw() + 1;
         const auto cellEnd = frame->GetRaw() + frame->GetReserved();
         for (auto cell = mReusable; cell < cellEnd; ++cell)
            cell->mNextFreeCell = cell + 1;
      }

      ++mCount;
      return result;
   }

   /// Destroys a valid cell from the hive                                    
   ///   @attention item pointer is no longer valid after this call           
   ///   @attention assumes that Cell is initialized                          
   ///   @attention assumes that Cell is owned by the hive                    
   ///   @param cell - cell to destroy                                        
   TEMPLATE()
   void THive<T>::Destroy(Cell* cell) {
      LglsAssumeDev(cell,
         "Pointer is not valid");
      LglsAssumeDev(Owns(cell),
         "Pointer is not valid");
      LglsAssumeDev(not cell->mNextFreeCell,
         "Cell is not initialized");

      // Destroy the cell, if owned by this hive                        
      cell->~Cell();
      cell->mNextFreeCell = mReusable;
      mReusable = cell;
      --mCount;
   }

   /// Free all valid cells and frames                                        
   ///   @attention this doesn't modify any THive state                       
   TEMPLATE() LANGULUS(INLINED)
   void TME()::ResetInner() {
      for (auto& frame : mFrames) {
         if (frame.IsEmpty())
            continue;

         auto raw = frame.GetRaw();
         const auto rawEnd = frame.GetRaw() + frame.GetReserved();
         while (raw != rawEnd and frame.mCount) {
            if (not raw->mNextFreeCell) {
               if (raw->mData.Reference(-1) == 0) {
                  // Safe to destroy the instance from here             
                  // Otherwise hive cell is in use somewhere else. It   
                  // will continue to live as a reference somewhere,    
                  // until the handle's destructor is called, and the   
                  // last reference is released                         
                  LglsAssumeDev(frame.GetUses() >= 1,
                     "A populated frame must have references");
                  raw->~Cell();
               }

               --frame.mCount;
            }

            ++raw;
         }

         LglsAssumeDev(frame.mCount == 0,
            "Frame should be empty at this point");
      }
   }

   /// Reset the factory                                                      
   TEMPLATE() LANGULUS(INLINED)
   void TME()::Reset() {
      ResetInner();
      mFrames.Reset();
      mReusable = nullptr;
      mCount = 0;
   }




#define TEMPLATE_IT() TEMPLATE() template<bool MUTABLE>
#define TME_IT() THive<T>::Iterator<MUTABLE>


   /// Construct an iterator                                                  
   ///   @param start - the current iterator position                         
   ///   @param end - the ending marker                                       
   TEMPLATE_IT() LANGULUS(INLINED)
   constexpr TME_IT()::Iterator(
      Cell* start, Cell const* end, Frame* startf, Frame const* endf
   ) noexcept
      : mCell      {start}
      , mCellEnd   {end}
      , mFrame     {startf}
      , mFrameEnd  {endf} {
      while (mCell and mCell->mNextFreeCell)
         operator ++ ();
   }

   /// Construct an end iterator                                              
   TEMPLATE_IT() LANGULUS(INLINED)
   constexpr TME_IT()::Iterator(const A::IteratorEnd&) noexcept
      : mCell      {nullptr}
      , mCellEnd   {nullptr}
      , mFrame     {nullptr}
      , mFrameEnd  {nullptr} {}

   /// Compare two iterators                                                  
   ///   @param rhs - the other iterator                                      
   ///   @return true if iterators point to the same element                  
   TEMPLATE_IT() LANGULUS(INLINED)
   constexpr bool TME_IT()::operator == (const Iterator& rhs) const noexcept {
      return mCell == rhs.mCell;
   }

   /// Compare iterator with an end marker                                    
   ///   @param rhs - the end iterator                                        
   ///   @return true element is at or beyond the end marker                  
   TEMPLATE_IT() LANGULUS(INLINED)
   constexpr bool TME_IT()::operator == (const A::IteratorEnd&) const noexcept {
      return mFrame == mFrameEnd;
   }
   
   /// Iterator access operator                                               
   ///   @return a reference to the element at the current iterator position  
   TEMPLATE_IT() LANGULUS(INLINED)
   constexpr decltype(auto) TME_IT()::operator * () const noexcept {
      return (mCell->mData);
   }

   /// Iterator access operator                                               
   ///   @return a reference to the element at the current iterator position  
   TEMPLATE_IT() LANGULUS(INLINED)
   constexpr decltype(auto) TME_IT()::operator -> () const noexcept {
      return &(mCell->mData);
   }

   /// Prefix increment operator                                              
   ///   @attention assumes iterator points to a valid element                
   ///   @return the modified iterator                                        
   TEMPLATE_IT() LANGULUS(INLINED)
   constexpr auto TME_IT()::operator ++ () noexcept -> Iterator& {
      ++mCell;

      // Skip uninitialized cells                                       
   skip_empty:
      while (mCell->mNextFreeCell and mCell < mCellEnd)
         ++mCell;

      if (mCell == mCellEnd) {
         // If end of frame was reached, move to the next frame         
         ++mFrame;

         if (mFrame != mFrameEnd) {
            mCell = mFrame->GetRaw();
            mCellEnd = mCell + mFrame->GetReserved();
            goto skip_empty;
         }
      }

      return *this;
   }

   /// Suffix increment operator                                              
   ///   @attention assumes iterator points to a valid element                
   ///   @return the previous value of the iterator                           
   TEMPLATE_IT() LANGULUS(INLINED)
   constexpr auto TME_IT()::operator ++ (int) noexcept -> Iterator {
      const auto backup = *this;
      operator ++ ();
      return backup;
   }

   /// Check if iterator is valid                                             
   TEMPLATE_IT() LANGULUS(INLINED)
   constexpr TME_IT()::operator bool() const noexcept {
      return *this != A::IteratorEnd {};
   }

   /// Implicitly convert to a immutable iterator                             
   TEMPLATE_IT() LANGULUS(INLINED)
   constexpr TME_IT()::operator Iterator<false>() const noexcept requires Mutable {
      return {mCell, mCellEnd, mFrame, mFrameEnd};
   }

} // namespace Langulus::Flow

#undef TEMPLATE_IT
#undef TME_IT

#undef TEMPLATE
#undef TME
