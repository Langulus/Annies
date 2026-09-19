///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "TEdit.hpp"

#define TEMPLATE()   template<DenseTypedBlock T>
#define TME()        Edit<T>


namespace Langulus::Annies
{

   /// Create an editor interface                                             
   ///   @param container - what container are we editing?                    
   ///   @param start - the starting marker of the selection (optional)       
   ///   @param end - the ending marker of the selection (optional)           
   TEMPLATE() LANGULUS(INLINED)
   TME()::Edit(T& container, size_t start, size_t end) noexcept
      : mSource {container}
      , mStart {start}
      , mEnd {end} {}

   TEMPLATE() LANGULUS(INLINED)
   TME()::Edit(T* container, size_t start, size_t end) noexcept
      : Edit {*container, start, end} {}

   /// Select a subpattern                                                    
   ///   @param pattern - the pattern to select                               
   ///   @return a reference to this editor                                   
   TEMPLATE() LANGULUS(INLINED)
   auto TME()::Select(const T& pattern) -> Edit& {
      if (mSource.IsEmpty() or pattern.IsEmpty())
         return *this;

      auto lhs = mSource.GetRaw();
      auto rhs = pattern.GetRaw();
      const auto endlhs = mSource.GetRawEnd() - pattern.GetCount();
      const auto endrhs = pattern.GetRawEnd();
      while (lhs != endlhs) {
         if (*lhs != *rhs) {
            ++lhs;
            continue;
         }

         const auto lhsBackup = ++lhs;
         ++rhs;

         while (rhs != endrhs) {
            if (*lhs != *rhs) {
               lhs = lhsBackup;
               rhs = pattern.GetRaw();
               break;
            }

            ++lhs;
            ++rhs;
         }

         if (rhs == endrhs) {
            mStart = (lhsBackup - 1) - mSource.GetRaw();
            mEnd = mStart + pattern.GetCount();
            break;
         }
      }

      return *this;
   }

   /// Select a subregion                                                     
   ///   @param start - the starting marker of the selection                  
   ///   @param end - the ending marker of the selection                      
   ///   @return a reference to this editor                                   
   TEMPLATE() LANGULUS(INLINED)
   auto TME()::Select(size_t start, size_t end) -> Edit& {
      mStart = ::std::min(start, mSource.GetCount());
      mEnd = ::std::max(::std::min(end, mSource.GetCount()), mStart);
      return *this;
   }

   /// Select a subregion                                                     
   ///   @param start - the starting marker of the selection                  
   ///   @return a reference to this editor                                   
   TEMPLATE() LANGULUS(INLINED)
   auto TME()::Select(size_t start) -> Edit& {
      mEnd = mStart = ::std::min(start, mSource.GetCount());
      return *this;
   }

   /// Get the container we're editing                                        
   ///   @return a constant reference to the source container                 
   TEMPLATE() LANGULUS(INLINED)
   auto TME()::GetSource() const noexcept -> const T& {
      return mSource;
   }

   /// Get the start of the selection                                         
   ///   @return the start of the selection                                   
   TEMPLATE() LANGULUS(INLINED)
   size_t TME()::GetStart() const noexcept {
      return mStart;
   }

   /// Get the end of the selection                                           
   ///   @return the end of the selection                                     
   TEMPLATE() LANGULUS(INLINED)
   size_t TME()::GetEnd() const noexcept {
      return mEnd;
   }

   /// Get the size of the selection                                          
   ///   @return the size of the selection                                    
   TEMPLATE() LANGULUS(INLINED)
   size_t TME()::GetLength() const noexcept {
      return mEnd - mStart;
   }

   /// Access an element at a given index, relative to the selection (const)  
   ///   @param index - the index to get                                      
   ///   @return a reference to the element at that index                     
   TEMPLATE() LANGULUS(INLINED)
   auto& TME()::operator[] (size_t index) const noexcept {
      return mSource[mStart + index];
   }

   /// Access an element at a given index, relative to the selection          
   ///   @param index - the index to get                                      
   ///   @return a reference to the element at that index                     
   TEMPLATE() LANGULUS(INLINED)
   auto& TME()::operator[] (size_t index) noexcept {
      return mSource[mStart + index];
   }

   /// Concatenate at the end of the selection                                
   ///   @param other - the container to concatenate                          
   ///   @return a reference to the editor for chaining                       
   TEMPLATE() LANGULUS(INLINED)
   auto TME()::operator << (const T& other) -> Edit& {
      mSource.InsertBlock(mEnd, other);
      return *this;
   }

   /// Concatenate at the start of the selection                              
   ///   @param other - the container to concatenate                          
   ///   @return a reference to the editor for chaining                       
   TEMPLATE() LANGULUS(INLINED)
   auto TME()::operator >> (const T& other) -> Edit& {
      const auto concatenated = mSource.InsertBlock(mStart, other);
      mStart += concatenated;
      mEnd += concatenated;
      return *this;
   }

   /// Replace the selection with an element sequence                         
   /// After replacement, the selection will collapse to the end of the       
   /// replacement                                                            
   ///   @param other - the container to use for replacement                  
   TEMPLATE() LANGULUS(INLINED)
   auto TME()::Replace(const T& other) -> Edit& {
      if constexpr (CT::POD<CTTI_InnerType> or CT::Sparse<CTTI_InnerType>) {
         const auto offset = mStart * mSource.GetStride();

         // Optimization for POD/sparse containers that spares the      
         // execution of move constructors of all elements              
         if (other.GetCount() > GetLength()) {
            // Replacement is bigger, we need a bit more space          
            const auto surplus = (other.GetCount() + mStart) - mEnd;
            mSource.template AllocateMore<false, true>(mSource.GetCount() + surplus);

            MoveMemory(
               mSource.GetRaw() + offset + surplus,
               mSource.GetRaw() + offset,
               mSource.GetCount() - offset - surplus
            );
         }
         else if (other.GetCount() < GetLength()) {
            // Replacement is smaller, move data backwards              
            const auto excess = GetLength() - other.GetCount();
            mSource.RemoveIndexAt(mStart + other.GetCount(), excess);
         }

         // Copy new data over the old one                              
         CopyMemory(
            mSource.GetRaw() + offset,
            other.GetRaw(), 
            other.GetBytesize()
         );
      }
      else {
         // Generalized, but safe replacement                           
         if (GetLength()) {
            //TODO can be optimized further, by avoiding the redundant move
            mSource.RemoveIndex(mStart, GetLength());
            mSource.InsertBlockAt(other, mStart);
         }
      }

      // Move marker and collapse selection                             
      mStart += other.GetCount();
      mEnd = mStart;
      return *this;
   }

   /// Insert single element at the end of the selection                      
   ///   @param other - the element to insert                                 
   ///   @return a reference to the editor for chaining                       
   TEMPLATE() LANGULUS(INLINED)
   auto TME()::operator << (const TypeOf<T>& other) -> Edit& {
      mSource.InsertAt(other, mEnd);
      return *this;
   }

   /// Insert single element at the start of the selection                    
   ///   @param other - the element to insert                                 
   ///   @return a reference to the editor for chaining                       
   TEMPLATE() LANGULUS(INLINED)
   auto TME()::operator >> (const TypeOf<T>& other) -> Edit& {
      const auto concatenated = mSource.InsertAt(other, mStart);
      mStart += concatenated;
      mEnd += concatenated;
      return *this;
   }

   /// Replace the selection with a single element                            
   /// After replacement, the selection will collapse to the end of the       
   /// replacement                                                            
   ///   @param other - the element to use for replacement                    
   ///   @return a reference to the editor for chaining                       
   TEMPLATE() LANGULUS(INLINED)
   auto TME()::Replace(const TypeOf<T>& other) -> Edit& {
      return Replace(T {other});
   }

   /// Delete selection (collapsing it), or delete symbol after collapsed     
   /// selection marker                                                       
   ///   @return a reference to the editor for chaining                       
   TEMPLATE() LANGULUS(INLINED)
   auto TME()::Delete() -> Edit& {
      const auto length = GetLength();
      if (length) {
         mSource.RemoveIndex(mStart, length);
         mEnd = mStart;
      }
      else if (mStart < mSource.GetCount()) {
         mSource.RemoveIndex(mStart, 1);
      }
      
      if (mSource.IsEmpty())
         mStart = mEnd = 0;
      else if (mStart >= mSource.GetCount())
         mStart = mEnd = mSource.GetCount() - 1;
      return *this;
   }

   /// Delete selection (collapsing it), or delete symbol before a collapsed  
   /// selection marker                                                       
   ///   @return a reference to the editor for chaining                       
   TEMPLATE() LANGULUS(INLINED)
   auto TME()::Backspace() -> Edit& {
      const auto length = GetLength();
      if (length) {
         mSource.RemoveIndex(mStart, length);
         mEnd = mStart;
      }
      else if (mStart > 0 and not mSource.IsEmpty()) {
         mSource.RemoveIndex(mStart - 1, 1);
         mEnd = --mStart;
      }

      if (mSource.IsEmpty())
         mStart = mEnd = 0;
      else if (mStart >= mSource.GetCount())
         mStart = mEnd = mSource.GetCount() - 1;
      return *this;
   }
}

#undef TEMPLATE
#undef TME