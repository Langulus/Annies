///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include <Langulus/CT/POD.hpp>
#include <Langulus/CT/Nullable.hpp>


namespace Langulus
{

   /// Wrapper for memcpy                                                     
   ///   @param to - [out] destination memory                                 
   ///   @param from - source of data to copy                                 
   template<class TO, class FROM> LANGULUS(INLINED)
   void CopyMemory(TO* to, const FROM* from) noexcept {
      static_assert(CT::Void<TO> or CT::Sparse<TO> or CT::POD<TO>, 
         "TO must be either pointer, reflected as POD, or trivial "
         "(you can suppress this error by casting pointer to void*)");

      static_assert(CT::Void<TO> or CT::Similar<TO, FROM>,
         "TO and FROM must be similar types "
         "(you can suppress this error by casting pointer to void*)");

      if constexpr (CT::Void<TO>)
         static_assert(false, "Bytecount not specified when copying void pointers");

      ::std::memcpy(
         static_cast<void*>(to),
         static_cast<const void*>(from),
         sizeof(TO)
      );
   }

   /// Wrapper for memcpy                                                     
   ///   @param to - [out] destination memory                                 
   ///   @param from - source of data to copy                                 
   ///   @param count - number of elements to copy                            
   ///   @attention count becomes bytecount, when TO is void                  
   template<class TO, class FROM> LANGULUS(INLINED)
   void CopyMemory(TO* to, const FROM* from, const size_t& count) noexcept {
      static_assert(CT::Void<TO> or CT::Sparse<TO> or CT::POD<TO>,
         "TO must be either pointer, reflected as POD, or trivial "
         "(you can suppress this error by casting pointer to void*)");

      static_assert(CT::Void<TO> or CT::Similar<TO, FROM>,
         "TO and FROM must be similar types "
         "(you can suppress this error by casting pointer to void*)");

      if constexpr (CT::Void<TO>) {
         ::std::memcpy(
            static_cast<void*>(to),
            static_cast<const void*>(from),
            count
         );
      }
      else {
         ::std::memcpy(
            static_cast<void*>(to),
            static_cast<const void*>(from),
            sizeof(TO) * count
         );
      }
   }
   
   /// Wrapper for memset                                                     
   ///   @tparam FILLER - value to fill in with                               
   ///   @param to - [out] destination memory                                 
   template<int FILLER, class TO> LANGULUS(INLINED)
   void FillMemory(TO* to) noexcept {
      static_assert(FILLER or CT::Nullifiable<TO> or CT::Void<TO>,
         "Filling with zeroes requires the type to be reflected as nullifiable, "
         "or be a pointer/fundamental (you can suppress this error by casting to void*)");

      if constexpr (CT::Void<TO>)
         static_assert(false, "Bytecount not specified when filling void pointer");
      
      ::std::memset(static_cast<void*>(to), FILLER, sizeof(TO));
   }
   
   /// Wrapper for memset                                                     
   ///   @tparam FILLER - value to fill in with                               
   ///   @param to - [out] destination memory                                 
   ///   @param count - number of elements to fill                            
   ///   @attention count becomes bytecount, when TO is void                  
   template<int FILLER, class TO> LANGULUS(INLINED)
   void FillMemory(TO* to, const size_t& count) noexcept {
      static_assert(FILLER or CT::Nullifiable<TO> or CT::Void<TO>,
         "Filling with zeroes requires the type to be reflected as nullifiable, "
         "or be a pointer/fundamental (you can suppress this error by casting to void*)");

      if constexpr (CT::Void<TO>)
         ::std::memset(static_cast<void*>(to), FILLER, count);
      else
         ::std::memset(static_cast<void*>(to), FILLER, sizeof(TO) * count);
   }

   /// Wrapper for memset 0                                                   
   ///   @param to - [out] destination memory                                 
   template<class TO> LANGULUS(INLINED)
   void ZeroMemory(TO* to) noexcept {
      return FillMemory<0>(to);
   }
      
   /// Wrapper for memset 0                                                   
   ///   @param to - [out] destination memory                                 
   ///   @param count - number of elements to fill                            
   ///   @attention count becomes bytecount, when TO is void                  
   template<class TO> LANGULUS(INLINED)
   void ZeroMemory(TO* to, const size_t& count) noexcept {
      return FillMemory<0>(to, count);
   }
      
   /// Wrapper for memset 0, with overlapping memory check                    
   ///   @param from - [out] source memory                                    
   ///   @param to - destination memory                                       
   ///   @param count - number of elements to fill                            
   ///   @attention count becomes bytecount, when TO is void                  
   template<class TO> LANGULUS(INLINED)
   void ZeroMemoryOverlapped(TO* from, const TO* to, const size_t& count) noexcept {
      if (from >= to + count or from + count <= to) {
         // No overlap                                                  
         // From: [][][][][][]                                          
         // To:                 [][][][][][]                            
         // Zero: [][][][][][]                                          
         ZeroMemory(from, count);
      }
      else {
         // Zero only the difference                                    
         if (from > to) {
            // From:         [][][][][][][][][][][][]                   
            // To:   [][][][][][][][][][][][]                           
            // Zero:                         [][][][]                   
            const auto diff = from - to;
            ZeroMemory(from + count - diff, diff);
         }
         else {
            // From: [][][][][][][][][][][][]                           
            // To:           [][][][][][][][][][][][]                   
            // Zero: [][][][]                                           
            ZeroMemory(from, to - from);
         }
      }
   }
      
   /// Wrapper for memmove                                                    
   ///   @param to - [out] destination memory                                 
   ///   @param from - source of data to move                                 
   template<class TO, class FROM> LANGULUS(INLINED)
   void MoveMemory(TO* to, const FROM* from) noexcept {
      static_assert(CT::Void<TO> or CT::Sparse<TO> or CT::POD<TO>,
         "TO must be either pointer, reflected as POD, or trivial "
         "(You can suppress this error by casting pointer to void*)");

      static_assert(CT::Void<TO> or CT::Similar<TO, FROM>,
         "TO and FROM must be similar types "
         "(you can suppress this error by casting pointer to void*)");

      if constexpr (CT::Void<TO>)
         static_assert(false, "Bytecount not specified when filling void pointer");

      ::std::memmove(
         static_cast<void*>(to),
         static_cast<const void*>(from),
         sizeof(TO)
      );

      #if LANGULUS(PARANOID)
         TODO() // zero old memory, but beware - `from` and `to` might overlap
      #endif
   }

   /// Wrapper for memmove                                                    
   ///   @param to - [out] destination memory                                 
   ///   @param from - source of data to move                                 
   ///   @param count - number of elements to move                            
   ///   @attention count becomes bytecount, when TO is void                  
   template<class TO, class FROM> LANGULUS(INLINED)
   void MoveMemory(TO* to, const FROM* from, const size_t& count) noexcept {
      static_assert(CT::Void<TO> or CT::Sparse<TO> or CT::POD<TO>,
         "TO must be either pointer, reflected as POD, or trivial "
         "(You can suppress this error by casting pointer to void*)");

      static_assert(CT::Void<TO> or CT::Similar<TO, FROM>,
         "TO and FROM must be similar types "
         "(you can suppress this error by casting pointer to void*)");

      if constexpr (CT::Void<TO>) {
         ::std::memmove(
            static_cast<void*>(to),
            static_cast<const void*>(from),
            count
         );
      }
      else {
         ::std::memmove(
            static_cast<void*>(to),
            static_cast<const void*>(from),
            sizeof(TO) * count
         );
      }

      #if LANGULUS(PARANOID)
         TODO() // zero old memory, but beware - `from` and `to` might overlap
      #endif
   }

} // namespace Langulus