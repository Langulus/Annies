///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include <Langulus/CT/Contiguous.hpp>
#include <Langulus/CT/Typed.hpp>


namespace Langulus::Annies
{
   ///                                                                        
   /// MARK: Editor                                                           
   ///                                                                        
   ///   Interface for editing contiguous containers of any kind.             
   ///   Allows you to select regions and do operations on to them,           
   /// while keeping track of selections and synchronizing changes between    
   /// them. The rationale is, that when modifying a TMany, for example,      
   /// picking and changing a selection will result in a copy the contents.   
   /// That is sometimes not desirable. This interface ensures, that you can  
   /// modify selections safely without branching out - changes will be       
   /// synchronized with the original upon destruction of this interface.     
   ///   The interface is designed to immitate the usual keyboard editing of  
   /// a text file - you can mark text, replace it, insert of the right or    
   /// left of selection, backspace, delete, etc.                             
   template<CT::Contiguous T>
   class Edit {
      using CTTI_Typed     = TypeOf<T>;
      using CTTI_ReflectAs = void;

      // What are we editing?                                           
      T& mSource;
      // Start of the selection                                         
      size_t mStart = 0;
      // End of the selection                                           
      size_t mEnd = 0;

   public:
      Edit() = delete("Always specify what we're editing");
      Edit(Edit const&) = delete("Only one editor can be open at a time");
      Edit(Edit&&) noexcept = default;
      Edit(T* src, size_t start = 0, size_t end = 0) noexcept
         : Edit {*src, start, end} {
         LglsAssert(src->GetUses() == 1,
            "Data is shared, and can't be edited from here");
      }

      Edit(T& src, size_t start = 0, size_t end = 0) noexcept
         : mSource {src}
         , mStart  {start}
         , mEnd    {end} {
         LglsAssert(src.GetUses() == 1,
            "Data is shared, and can't be edited from here");
      }

      Edit& Select(T const&);
      Edit& Select(size_t, size_t);
      Edit& Select(size_t);

      auto GetSource() const noexcept -> T const&;
      auto GetStart()  const noexcept -> size_t;
      auto GetEnd()    const noexcept -> size_t;
      auto GetLength() const noexcept -> size_t;

      auto& operator[] (size_t) const noexcept;
      auto& operator[] (size_t) noexcept;

      Edit& operator << (T const&);
      Edit& operator >> (T const&);
      Edit& Replace(T const&);

      Edit& operator << (const CTTI_Typed&);
      Edit& operator >> (const CTTI_Typed&);
      Edit& Replace(const CTTI_Typed&);

      Edit& Delete();
      Edit& Backspace();
   };

   /// MARK: CTAD                                                             
   template<CT::Contiguous T>
   Edit(T&) -> Edit<T>;

   template<CT::Contiguous T>
   Edit(T*) -> Edit<T>;
}

namespace Langulus
{
   using Annies::Edit;
}




/// MARK: Implementation                                                      
namespace Langulus::Annies
{
   /// Select a subpattern                                                    
   ///   @param pattern the pattern to select                                 
   ///   @return a reference to this editor                                   
   template<CT::Contiguous T>
   auto Edit<T>::Select(T const& pattern) -> Edit& {
      if (mSource.IsEmpty() or pattern.IsEmpty())
         return *this;

      auto h = mSource.FindRange(pattern);
      if (h) {
         mStart = h - mSource.GetHandle();
         mEnd = mStart + pattern.GetCount();
      }
      else mStart = mEnd = 0;
      return *this;
   }

   /// Select a subregion                                                     
   ///   @param start the starting marker of the selection                    
   ///   @param end the ending marker of the selection                        
   ///   @return a reference to this editor                                   
   template<CT::Contiguous T>
   auto Edit<T>::Select(size_t start, size_t end) -> Edit& {
      const auto count = mSource.GetCount();
      mStart = start < count? start : count;
      const auto temps = end < count? end : count;
      mEnd = temps > mStart? temps : mStart;
      return *this;
   }

   /// Select a subregion                                                     
   ///   @param start the starting marker of the selection                    
   ///   @return a reference to this editor                                   
   template<CT::Contiguous T>
   auto Edit<T>::Select(size_t start) -> Edit& {
      const auto count = mSource.GetCount();
      mEnd = mStart = start < count? start : count;
      return *this;
   }

   /// Get the container we're editing                                        
   ///   @return a constant reference to the source container                 
   template<CT::Contiguous T>
   auto Edit<T>::GetSource() const noexcept -> T const& {
      return mSource;
   }

   /// Get the start of the selection                                         
   ///   @return the start of the selection                                   
   template<CT::Contiguous T>
   size_t Edit<T>::GetStart() const noexcept {
      return mStart;
   }

   /// Get the end of the selection                                           
   ///   @return the end of the selection                                     
   template<CT::Contiguous T>
   size_t Edit<T>::GetEnd() const noexcept {
      return mEnd;
   }

   /// Get the size of the selection                                          
   ///   @return the size of the selection                                    
   template<CT::Contiguous T>
   size_t Edit<T>::GetLength() const noexcept {
      return mEnd - mStart;
   }

   /// Access an element at a given index, relative to the selection (const)  
   ///   @param index the index to get                                        
   ///   @return a reference to the element at that index                     
   template<CT::Contiguous T>
   auto& Edit<T>::operator[] (size_t index) const noexcept {
      return mSource[mStart + index];
   }

   /// Access an element at a given index, relative to the selection          
   ///   @param index the index to get                                        
   ///   @return a reference to the element at that index                     
   template<CT::Contiguous T>
   auto& Edit<T>::operator[] (size_t index) noexcept {
      return mSource[mStart + index];
   }

   /// Concatenate at the end of the selection                                
   ///   @param other the container to concatenate                            
   ///   @return a reference to the editor for chaining                       
   template<CT::Contiguous T>
   auto Edit<T>::operator << (T const& other) -> Edit& {
      mSource.InsertRangeAt(mEnd, other);
      return *this;
   }

   /// Concatenate at the start of the selection                              
   ///   @param other the container to concatenate                            
   ///   @return a reference to the editor for chaining                       
   template<CT::Contiguous T>
   auto Edit<T>::operator >> (T const& other) -> Edit& {
      const auto concatenated = mSource.InsertRangeAt(mStart, other);
      mStart += concatenated;
      mEnd += concatenated;
      return *this;
   }

   /// Replace the selection with an element sequence.                        
   /// After replacement, the selection will collapse to the end of the       
   /// replacement.                                                           
   ///   @param other the container to use for replacement                    
   template<CT::Contiguous T>
   auto Edit<T>::Replace(T const& other) -> Edit& {
      if constexpr (CT::POD<CTTI_Typed> or CT::Sparse<CTTI_Typed>) {
         const auto offset = mStart * mSource.GetStride();

         // Optimization for POD/sparse containers that spares the      
         // execution of move constructors of all elements              
         if (other.GetCount() > GetLength()) {
            // Replacement is bigger, we need a bit more space          
            const auto surplus = (other.GetCount() + mStart) - mEnd;
            mSource.template AllocateMore<false, true>(mSource.GetCount() + surplus);

            memmove(//TODO not acceptable if we aim TEdit to be generic
               mSource.GetRaw() + offset + surplus,
               mSource.GetRaw() + offset,
               mSource.GetCount() - offset - surplus
            );
         }
         else if (other.GetCount() < GetLength()) {
            // Replacement is smaller, move data backwards              
            const auto excess = GetLength() - other.GetCount();
            mSource.EraseRangeAt(mStart + other.GetCount(), excess);
         }

         // Copy new data over the old one                              
         memcpy(//TODO not acceptable if we aim TEdit to be generic
            mSource.GetRaw() + offset,
            other.GetRaw(), 
            other.GetBytesize()
         );
      }
      else {
         // Generalized, but safe replacement                           
         if (GetLength()) {
            //TODO can be optimized further, by avoiding the redundant move
            mSource.EraseRangeAt(mStart, GetLength());
            mSource.InsertRangeAt(other, mStart);
         }
      }

      // Move marker and collapse selection                             
      mStart += other.GetCount();
      mEnd = mStart;
      return *this;
   }

   /// Insert single element at the end of the selection                      
   ///   @param other the element to insert                                   
   ///   @return a reference to the editor for chaining                       
   template<CT::Contiguous T>
   auto Edit<T>::operator << (const CTTI_Typed& other) -> Edit& {
      mSource.InsertAt(other, mEnd);
      return *this;
   }

   /// Insert single element at the start of the selection                    
   ///   @param other the element to insert                                   
   ///   @return a reference to the editor for chaining                       
   template<CT::Contiguous T>
   auto Edit<T>::operator >> (const CTTI_Typed& other) -> Edit& {
      const auto concatenated = mSource.InsertAt(other, mStart);
      mStart += concatenated;
      mEnd += concatenated;
      return *this;
   }

   /// Replace the selection with a single element                            
   /// After replacement, the selection will collapse to the end of the       
   /// replacement                                                            
   ///   @param other the element to use for replacement                      
   ///   @return a reference to the editor for chaining                       
   template<CT::Contiguous T>
   auto Edit<T>::Replace(const CTTI_Typed& other) -> Edit& {
      return Replace(T {other});
   }

   /// Delete selection (collapsing it), or delete symbol after collapsed     
   /// selection marker                                                       
   ///   @return a reference to the editor for chaining                       
   template<CT::Contiguous T>
   auto Edit<T>::Delete() -> Edit& {
      const auto length = GetLength();
      if (length) {
         mSource.EraseRangeAt(mStart, length);
         mEnd = mStart;
      }
      else if (mStart < mSource.GetCount()) {
         mSource.EraseAt(mStart);
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
   template<CT::Contiguous T>
   auto Edit<T>::Backspace() -> Edit& {
      const auto length = GetLength();
      if (length) {
         mSource.EraseRangeAt(mStart, length);
         mEnd = mStart;
      }
      else if (mStart > 0 and not mSource.IsEmpty()) {
         mSource.EraseAt(mStart - 1);
         mEnd = --mStart;
      }

      if (mSource.IsEmpty())
         mStart = mEnd = 0;
      else if (mStart >= mSource.GetCount())
         mStart = mEnd = mSource.GetCount() - 1;
      return *this;
   }
}
