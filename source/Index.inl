///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include <Langulus/Assume.hpp>


namespace Langulus::Annies
{

   /// Constructor from signed integer                                        
   ///   @param value - integer to set                                        
   template<CT::BuiltinSignedInteger T> LANGULUS(INLINED)
   constexpr Index::Index(const T& value) noexcept (sizeof(T) < sizeof(Type))
      : mIndex {value} {
      if constexpr (sizeof(T) >= sizeof(Type))
         LANGULUS_ASSERT(IsArithmetic(), Access, "Index is not arithmetic");
      if constexpr (sizeof(T) > sizeof(Type)) {
         constexpr T limit = static_cast<T>(MaxIndex);
         LANGULUS_ASSERT(value <= limit, Access, "Index overflow");
      }
   }
   
   /// Constructor from unsigned integer                                      
   ///   @param value - integer to set                                        
   template<CT::BuiltinUnsignedInteger T> LANGULUS(INLINED)
   constexpr Index::Index(const T& value) noexcept (sizeof(T) <= sizeof(Type) / 2)
      : mIndex {static_cast<Type>(value)} {
      if constexpr (sizeof(T) > sizeof(Type) / 2) {
         constexpr T limit = static_cast<T>(MaxIndex);
         LANGULUS_ASSERT(value <= limit, Access, "Index overflow");
      }
   }

   /// Constructor from real number (round to nearest)                        
   ///   @param value - real to set                                           
   LANGULUS(INLINED)
   constexpr Index::Index(const CT::Real auto& value)
      : mIndex {static_cast<Type>(value)} {
      LANGULUS_ASSERT(IsArithmetic(), Access, "Index is not arithmetic");
   }

   /// Constrain the index to some count (immutable)                          
   ///   If index is out of scope, return None                                
   ///   If index is special - return it as it is                             
   LANGULUS(INLINED)
   constexpr Index Index::Constrained(const size_t count) const noexcept {
      switch (mIndex) {
      case Auto: case First: case Front:
         return {0};
      case All: case Back:
         return {count};
      case Last:
         return count ? Index {count - 1} : Index {None};
      case Middle:
         return count / 2;
      case None:
         return None;
      default: {
         const Type c = static_cast<Type>(count);
         if (IsSpecial()) {
            if (IsArithmetic()) {
               // Index is negative, wrap it around (if in range)       
               return c + mIndex >= 0 ? Index {c + mIndex} : Index {None};
            }

            // Index is a special value, so don't change anything       
            return mIndex;
         }

         return mIndex >= c ? Index {None} : Index {mIndex};
      }}
   }

   /// Get an unsigned offset from the index, if possible                     
   /// Throws Except::Access if not possible to extract index                 
   ///   @return a valid offset                                               
   LANGULUS(INLINED)
   size_t Index::GetOffset() const {
      LANGULUS_ASSERT(not IsSpecial(), Access,
         "Can't convert index to offset");
      return static_cast<size_t>(mIndex);
   }
   
   /// Return the internal value without any safety checks                    
   ///   @return the offset                                                   
   LANGULUS(INLINED)
   size_t Index::GetOffsetUnsafe() const noexcept {
      return static_cast<size_t>(mIndex);
   }

   /// Constrain the index to some count (destructive)                        
   ///   @param count - the count to constrain to                             
   LANGULUS(INLINED)
   constexpr void Index::Constrain(const size_t count) noexcept {
      *this = Constrained(count); 
   }

   /// Concatenate index (destructive)                                        
   ///   @param count - the count to constrain to                             
   LANGULUS(INLINED)
   constexpr void Index::Concat(const Index& other) noexcept {
      if (IsSpecial())
         return;

      auto digits = DigitsOf(other.mIndex);
      while (digits) {
         mIndex *= 10;
         --digits;
      }

      mIndex += other.mIndex;
   }

   /// Check validity                                                         
   LANGULUS(INLINED)
   constexpr bool Index::IsValid() const noexcept {
      return mIndex != None;
   }

   /// Check invalidity                                                       
   LANGULUS(INLINED)
   constexpr bool Index::IsInvalid() const noexcept {
      return mIndex == None;
   }

   /// Check if index is special                                              
   LANGULUS(INLINED)
   constexpr bool Index::IsSpecial() const noexcept {
      return mIndex < 0;
   }

   /// Check if index is special                                              
   LANGULUS(INLINED)
   constexpr bool Index::IsReverse() const noexcept {
      return IsSpecial() and IsArithmetic();
   }

   /// Check if index is special                                              
   LANGULUS(INLINED)
   constexpr bool Index::IsArithmetic() const noexcept {
      return mIndex >= Counter;
   }

   /// Return true if index is valid                                          
   LANGULUS(INLINED)
   constexpr Index::operator bool () const noexcept {
      return IsValid();
   }

   /// Convert to any kind of number                                          
   LANGULUS(INLINED)
   constexpr Index::operator const Type& () const noexcept {
      return mIndex;
   }

   /// Destructive increment by 1                                             
   LANGULUS(INLINED)
   constexpr void Index::operator ++ () noexcept {
      if (IsArithmetic())
         ++mIndex; 
   }

   /// Destructive decrement by 1                                             
   LANGULUS(INLINED)
   constexpr void Index::operator -- () noexcept {
      if (IsArithmetic())
         --mIndex;
   }

   /// Index - Index Arithmetics                                              
   LANGULUS(INLINED)
   constexpr void Index::operator += (const Index& v) noexcept {
      if (not IsArithmetic() or not v.IsArithmetic())
         return; 
      mIndex += v.mIndex; 
   }

   LANGULUS(INLINED)
   constexpr void Index::operator -= (const Index& v) noexcept {
      if (not IsArithmetic() or not v.IsArithmetic())
         return;
      mIndex -= v.mIndex; 
   }

   LANGULUS(INLINED)
   constexpr void Index::operator *= (const Index& v) noexcept {
      if (not IsArithmetic() or not v.IsArithmetic())
         return;
      mIndex *= v.mIndex; 
   }

   LANGULUS(INLINED)
   constexpr void Index::operator /= (const Index& v) noexcept {
      if (not IsArithmetic() or not v.IsArithmetic())
         return;
      mIndex /= v.mIndex; 
   }

   LANGULUS(INLINED)
   constexpr Index Index::operator + (const Index& v) const noexcept {
      if (not IsArithmetic() or not v.IsArithmetic())
         return *this;
      return mIndex + v.mIndex; 
   }

   LANGULUS(INLINED)
   constexpr Index Index::operator - (const Index& v) const noexcept {
      if (not IsArithmetic() or not v.IsArithmetic()
      or mIndex - v.mIndex < Counter)
         return *this; 
      return mIndex - v.mIndex; 
   }

   LANGULUS(INLINED)
   constexpr Index Index::operator * (const Index& v) const noexcept {
      if (not IsArithmetic() or not v.IsArithmetic())
         return *this;
      return mIndex * v.mIndex; 
   }

   LANGULUS(INLINED)
   constexpr Index Index::operator / (const Index& v) const noexcept {
      if (not IsArithmetic() or not v.IsArithmetic())
         return *this;
      return mIndex / v.mIndex; 
   }

   /// Invert the index                                                       
   LANGULUS(INLINED)
   constexpr Index Index::operator - () const noexcept {
      if (IsArithmetic())
         return *this; 
      return -mIndex; 
   }

   /// Comparison                                                             
   LANGULUS(INLINED)
   constexpr bool Index::operator == (const Index& v) const noexcept {
      return mIndex == v.mIndex;
   }

   LANGULUS(INLINED)
   constexpr bool Index::operator < (const Index& v) const noexcept {
      switch (mIndex) {
      case All: case Many: case Single:
         // Single < Many < All                                         
         switch (v.mIndex) {
         case All: case Many: case Single:
            return mIndex < v.mIndex;
         default:
            return false;
         };

      case Back: case Middle: case Front: case None:
         // None < Front < Middle < Back                                
         switch (v.mIndex) {
         case Back: case Middle: case Front: case None:
            return mIndex < v.mIndex;
         default:
            return false;
         };

      case Mode: case Biggest: case Smallest: case Auto: case Random:
         // Uncomparable                                                
         return false;
         
      default:
         // Index is not special                                        
         if (mIndex < 0) {
            // Comparison is inverted                                   
            if (v.mIndex < 0 and v.mIndex >= Counter)
               return mIndex > v.mIndex;
         }
         else {
            // Comparison is not inverted                               
            if (v.mIndex > 0)
               return mIndex < v.mIndex;
         }
         return false;
      }
   }

   LANGULUS(INLINED)
   constexpr bool Index::operator > (const Index& v) const noexcept {
      return *this != v and not (*this < v);
   }

   LANGULUS(INLINED)
   constexpr bool Index::operator <= (const Index& v) const noexcept {
      return *this == v or (*this < v);
   }

   LANGULUS(INLINED)
   constexpr bool Index::operator >= (const Index& v) const noexcept {
      return *this == v or not (*this < v);
   }
}
