///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Text.hpp"
#include "../many/TMany.inl"
#include <charconv>
#include <limits>
#include <cstring>


namespace Langulus::Annies
{

   /// Default construction with nullptr_t                                    
   LANGULUS(INLINED)
   constexpr Text::Text(::std::nullptr_t) noexcept
      : Text {} {}

   /// Refer constructor                                                      
   ///   @param other - container to reference                                
   LANGULUS(INLINED)
   Text::Text(const Text& other)
      : Text {Refer(other)} { }

   /// Move constructor                                                       
   ///   @param other - container to move                                     
   LANGULUS(INLINED)
   Text::Text(Text&& other) noexcept
      : Text {Move(other)} { }

   /// Generic text constructor                                               
   ///   @param other - the text container to use, with or without intent     
   template<class T> requires CT::TextBased<Deint<T>> LANGULUS(INLINED)
   Text::Text(T&& other) {
      Base::BlockCreate(Forward<T>(other));
   }

   /// Construct from single character - any intent will be ignored           
   ///   @param other - the character to copy                                 
   template<class T> requires CT::Character<Deint<T>> LANGULUS(INLINED)
   Text::Text(T&& other) {
      AllocateFresh(RequestSize(1));
      mCount = 1;
      (*this)[0] = DeintCast(other);
   }

   /// Construct from a bounded character array                               
   ///   @attention assumes that the character sequence is null-terminated    
   ///      - this is usually guaranteed for string literals, but its left    
   ///        to an assumption, as the user can reinterpret_cast as array     
   ///   @param other - the string and intent                                 
   template<class T> requires CT::String<Deint<T>> LANGULUS(INLINED)
   Text::Text(T&& other) {
      using S = IntentOf<decltype(other)>;
      Count count;

      if constexpr (CT::StringLiteral<Deint<T>>) {
         count = DeintCast(other)
            ? strnlen(DeintCast(other), ExtentOf<Deint<T>>)
            : 0;
      }
      else {
         count = DeintCast(other)
            ? ::std::strlen(DeintCast(other))
            : 0;
      }

      if (not count)
         return;

      if constexpr (CT::Copied<S> or CT::Cloned<S>) {
         // Copy/Clone                                                  
         AllocateFresh(RequestSize(count));
         mCount = count;
         memcpy(mRaw, DeintCast(other), count);
      }
      else if constexpr (S::Move or S::Keep) {
         // Will perform search and take authority if not owned by us   
         new (this) Base {
            DataState::Constrained, GetType(), count, DeintCast(other)
         };
         TakeAuthority();
      }
      else {
         // We're disowning it, so no transfer, just a static view      
         new (this) Base {
            DataState::Constrained, GetType(), count, DeintCast(other), nullptr
         };
      }
   }

   /// Construct from any standard contiguous range statically typed with     
   /// characters. This includes std::string, string_view, span, vector,      
   /// array, etc.                                                            
   ///   @attention containers that aren't strings will be strnlen'ed         
   ///   @param other - the string and intent                                 
   template<class T> requires CT::StdString<Deint<T>>
   LANGULUS(INLINED) Text::Text(T&& other) {
      using S = IntentOf<decltype(other)>;
      if (DeintCast(other).empty())
         return;

      Count count;
      if constexpr (not ::std::is_convertible_v<Deint<T>, std::string_view>)
         count = strnlen(DeintCast(other).data(), DeintCast(other).size());
      else
         count = DeintCast(other).size();

      if (not count)
         return;

      if constexpr (CT::Copied<S> or CT::Cloned<S>) {
         // Copy/Clone                                                  
         AllocateFresh(RequestSize(count));
         mCount = count;
         memcpy(mRaw, DeintCast(other).data(), count);
      }
      else if constexpr (S::Move or S::Keep) {
         // Will perform search and take authority if not owned by us   
         new (this) Base {
            DataState::Constrained, GetType(), count, DeintCast(other).data()
         };
         TakeAuthority();
      }
      else {
         // We're disowning it, so no transfer, just a static view      
         new (this) Base {
            DataState::Constrained, GetType(), count, DeintCast(other).data(), nullptr
         };
      }
   }
   
   /// Stringify meta                                                         
   ///   @param meta - the definition to stringify                            
   LANGULUS(INLINED)
   Text::Text(const CT::Meta auto& meta)
      : Text {meta.GetToken()} {}

   /// Stringify a Langulus::Exception                                        
   ///   @param from - the exception to stringify                             
   LANGULUS(INLINED)
   Text::Text(const CT::Exception auto& from) {
      #if LANGULUS(DEBUG)
         new (this) Text{TemplateRt("{}({} at {})",
            from.GetName(),
            from.GetMessage(),
            from.GetLocation()
         )};
      #else
         new (this) Text {Disown(from.GetName())};
      #endif
   }
   
   /// Stringify a byte sequence, by writing it in a hexadecimal form         
   ///   @param from - the byte container to stringify                        
   LANGULUS(INLINED)
   Text::Text(const CT::Bytes auto& from) {
      mType = MetaDataOf<Letter>();
      const auto count = from.GetCount() * 2;
      AllocateFresh(RequestSize(count));
      auto to_bytes = mRaw;
      for (auto& byte : from) {
         fmt::format_to_n(to_bytes, 2, "{:X}", byte.mValue);
         to_bytes += 2;
      }
      mCount = count;
   }
      
   /// Stringify a single byte, by writing it in a hexadecimal form           
   ///   @param from - the byte to stringify                                  
   LANGULUS(INLINED)
   Text::Text(Byte from) {
      mType = MetaDataOf<Letter>();
      AllocateFresh(RequestSize(2));
      fmt::format_to_n(mRaw, 2, "{:X}", from.mValue);
      mCount = 2;
   }

   /// Stringify a boolean, by writing either `yes` or `no`                   
   ///   @param from - the boolean to stringify                               
   LANGULUS(INLINED)
   Text::Text(bool from) {
      mType = MetaDataOf<Letter>();
      AllocateFresh(RequestSize(4));
      if (from) {
         fmt::format_to_n(mRaw, 3, "{}", "yes");
         mCount = 3;
      }
      else {
         fmt::format_to_n(mRaw, 2, "{}", "no");
         mCount = 2;
      }
   }

   /// Stringify a serialization operator                                     
   ///   @param from - the op to stringify                                    
   LANGULUS(INLINED)
   Text::Text(Operator from)
      : Text {from < Operator::OpCounter
         ? Disown(SerializationRules::Operators[from].mToken)
         : Disown(Token {"<nonexistent operator token>"})
      } {}

   /// Convert anything that has named values reflected                       
   ///   @param number - the number to stringify                              
   LANGULUS(INLINED)
   Text::Text(const CT::HasNamedValues auto& named) {
      new (this) Text {TemplateRt("{}", named)};
   }

   /// Convert a number type to text                                          
   /// Notice that this constructor explicitly avoids character types         
   ///   @attention this will truncate numbers, and stringify them in a       
   ///      format that is best used for logging. If you want to serialize    
   ///      a number, you should use Flow::Code container cast instead.       
   ///   @param number - the number to stringify                              
   template<CT::BuiltinNumber T> requires (not CT::Character<T>)
   LANGULUS(INLINED) Text::Text(const T& number)
      : Text {FromNumber<2>(number)} {}

   /// Compose text by an arbitrary amount of formattable arguments           
   ///   @param ...args - the arguments                                       
   template<class T1, class T2, class...TN>
   requires CT::Inner::Stringifiable<T1, T2, TN...> LANGULUS(INLINED)
   Text::Text(T1&& t1, T2&& t2, TN&&...tn) {
        UnfoldInsert(Forward<T1>(t1));
        UnfoldInsert(Forward<T2>(t2));
      ((UnfoldInsert(Forward<TN>(tn))), ...);
   }

   /// Destructor                                                             
   inline Text::~Text() {
      Base::Free();
   }

   /// Create text from a number                                              
   ///   @tparam PRECISION - number of digits after the floating point, use   
   ///      0 for no truncation. Will produce scientific notation for too     
   ///      bit or too small numbers.                                         
   ///   @param number - the number to stringify                              
   ///   @return the text                                                     
   template<Count PRECISION, CT::BuiltinNumber T> LANGULUS(INLINED)
   Text Text::FromNumber(const T& number) {
      Text result;
      result.mType = MetaDataOf<Letter>();

      if constexpr (CT::Real<T>) {
         // Stringify a real number                                     
         constexpr auto size = ::std::numeric_limits<T>::max_digits10 * 2;
         char temp[size];
         auto [lastChar, errorCode] = ::std::to_chars(
            temp, temp + size, number, ::std::chars_format::general);
         LANGULUS_ASSERT(errorCode == ::std::errc(), Convert,
            "std::to_chars failure");

         // Find the dot                                                
         auto dot = temp;
         while (dot < lastChar and *dot != '.')
            ++dot;

         if (dot == lastChar) {
            // There is no dot...                                       
            const auto c = static_cast<Count>(lastChar - temp);
            result.AllocateFresh(result.RequestSize(c));
            memcpy(result.mRaw, temp, c);
            result.mCount = c;
            return Abandon(result);
         }

         // Truncate or just remove all trailing zeroes back until dot  
         --lastChar;
         bool approximate = false;

         while (lastChar >= dot) {
            // If last digit is zero/dot directly skip it               
            if (*lastChar == '.' or *lastChar == '0') {
               --lastChar;
               continue;
            }

            if constexpr (PRECISION) {
               // We can truncate even more                             
               if (lastChar > dot + PRECISION) {
                  if (lastChar == dot + PRECISION + 1 and *lastChar > '4') {
                     // Round up                                        
                     while (*lastChar == '9') {
                        // Propagate up until <9 or .                   
                        --lastChar;
                     }

                     if (*lastChar == '.')
                        ++(*(--lastChar));
                     else
                        ++(*lastChar);
                  }
                  else --lastChar;

                  approximate = true;
                  continue;
               }
               else break;
            }
            else break;
         }

         ++lastChar;
         const auto c = static_cast<Count>(lastChar - temp);
         if (approximate) {
            // We've truncated the number, so prepend a '~' symbol to   
            // signify it's an approximate representation               
            result.AllocateFresh(result.RequestSize(c + 1));
            *result.mRaw = '~';
            memcpy(result.mRaw + 1, temp, c);
            result.mCount = c + 1;
         }
         else {
            result.AllocateFresh(result.RequestSize(c));
            memcpy(result.mRaw, temp, c);
            result.mCount = c;
         }
      }
      else if constexpr (CT::Integer<T>) {
         // Stringify an integer                                        
         constexpr auto size = ::std::numeric_limits<T>::digits10 * 2;
         char temp[size];
         auto [lastChar, errorCode] = ::std::to_chars(temp, temp + size, number);
         LANGULUS_ASSERT(errorCode == ::std::errc(), Convert,
            "std::to_chars failure");

         const auto c = static_cast<Count>(lastChar - temp);
         result.AllocateFresh(result.RequestSize(c));
         memcpy(result.mRaw, temp, c);
         result.mCount = c;
      }
      else static_assert(false, "Unsupported number type");
      return Abandon(result);
   }

   /// Construction from bounded or unbounded array/pointer of characters     
   ///   @attention intent is ignored, this doesn't apply ownership, only     
   ///      interfaces the data - you can TakeAuthority() after this call.    
   ///   @attention assumes text pointer is valid                             
   ///   @param text - text memory to wrap, assumed valid                     
   ///   @param count - number of characters inside 'text' to use             
   ///   @attention count will shrink if a terminating character was found,   
   ///      or if 'text' is a bounded array of smaller size                   
   ///   @return the text wrapped inside a Text container                     
   template<class T> requires CT::String<Deint<T>> LANGULUS(INLINED)
   Text Text::From(T&& text, Count count) {
      if (count == 0)
         return {};

      if constexpr (CT::Array<Deint<T>>) {
         // In case of an array - represent array, and then edit count  
         auto block = MakeBlock<Text>(Disown(text));
         // Make sure string is properly terminated                     
         block.mCount = strnlen(
            block.GetRaw(), ::std::min(count, ExtentOf<Deint<T>>));
         return block;
      }
      else {
         LANGULUS_ASSUME(UserAssumes, text, "Invalid string pointer");
         // Raw pointer                                                 
         auto block = MakeBlock<Text>(Disown(text), count);
         // Make sure string is properly terminated                     
         block.mCount = strnlen(block.GetRaw(), count);
         return block;
      }
   }

   /// Refer assignment                                                       
   ///   @param rhs - the text to refer to                                    
   ///   @return a reference to this container                                
   LANGULUS(INLINED)
   Text& Text::operator = (const Text& rhs) {
      static_assert(CT::DeepAssignable<Letter, Referred<Text>>);
      return operator = (Refer(rhs));
   }

   /// Move assignment                                                        
   ///   @param rhs - the text to move                                        
   ///   @return a reference to this container                                
   LANGULUS(INLINED)
   Text& Text::operator = (Text&& rhs) noexcept {
      static_assert(CT::DeepAssignable<Letter, Moved<Text>>);
      return operator = (Move(rhs));
   }
   
   /// Assign any Text block                                                  
   ///   @param rhs - the block to assign                                     
   template<class T> requires CT::TextBased<Deint<T>> LANGULUS(INLINED)
   Text& Text::operator = (T&& rhs) {
      return Base::BlockAssign<Text>(Forward<T>(rhs));
   }

   #if LANGULUS_FEATURE(UNICODE)
      /// Widen the text container to the utf16                               
      ///   @return the widened text container                                
      TMany<char16_t> Text::Widen16() const {
         if (IsEmpty())
            return {};

         TMany<char16_t> to;
         to.AllocateFresh(to.RequestSize(mCount));
         Count newCount = 0;
         try {
            newCount = utf8::utf8to16(begin(), end(), to.begin()) - to.begin();
         }
         catch (utf8::exception&) {
            LANGULUS_THROW(Convert, "utf8 -> utf16 conversion error");
         }

         to.Trim(newCount);
         return to;
      }

      /// Widen the text container to the utf32                               
      ///   @return the widened text container                                
      TMany<char32_t> Text::Widen32() const {
         if (IsEmpty())
            return {};

         TMany<char32_t> to;
         to.AllocateFresh(to.RequestSize(mCount));
         Count newCount = 0;
         try {
            newCount = utf8::utf8to32(begin(), end(), to.begin()) - to.begin();
         }
         catch (utf8::exception&) {
            LANGULUS_THROW(Convert, "utf8 -> utf16 conversion error");
         }

         to.Trim(newCount);
         return to;
      }
   #endif
      
   /// Count the number of newline characters                                 
   ///   @return the number of newline characters + 1, or zero if empty       
   LANGULUS(INLINED)
   Count Text::GetLineCount() const noexcept {
      if (IsEmpty())
         return 0;

      Count lines = 1;
      for (Count i = 0; i < mCount; ++i) {
         if ((*this)[i] == '\n')
            ++lines;
      }

      return lines;
   }

   /// Terminate text so that it ends with a zero character at the end        
   /// This works even if the text container was empty                        
   ///   @return null-terminated equivalent to this Text                      
   LANGULUS(INLINED)
   Text Text::Terminate() const {
      // This greatly improves performance, but gives inconsistent      
      // results if MEMORY_STATISTICS feature is enabled (gives false   
      // positives for leaks if by random chance, the end of the string 
      // turns out to be \0 due to memory junk).                        
      // So, when MEMORY_STATISTICS is enabled we sacrifice performance 
      // for memory consistency. Generally, MEMORY_STATISTICS           
      // sacrifices performance anyways                                 
      #if not LANGULUS_FEATURE(MEMORY_STATISTICS)
         if (mReserved > mCount and GetRaw()[mCount] == '\0')
            return *this;
      #endif

      if (GetUses() == 1) {
         auto mutableThis = const_cast<Text*>(this);

         // If we have ownership and this is the only place where data  
         // is referenced, then we can simply resize this container,    
         // and attach a zero at the end of it                          
         if (mReserved > mCount) {
            // Required memory is already available                     
            mutableThis->GetRaw()[mCount] = '\0';
            return *this;
         }

         Base previousBlock {*this};
         const auto request = RequestSize(mCount + 1);
         mutableThis->mEntry = Allocator::Reallocate(
            request.mByteSize, const_cast<Allocation*>(mEntry));
         LANGULUS_ASSERT(mEntry, Allocate, "Out of memory");
         mutableThis->mReserved = request.mElementCount;

         if (mEntry != previousBlock.mEntry) {
            // Memory moved, and we should move all elements in it      
            mutableThis->mRaw = const_cast<Byte*>(mEntry->GetBlockStart());
            CopyMemory(mutableThis->mRaw, previousBlock.mRaw, mCount);
            previousBlock.Free();
         }

         // Add the null-termination                                    
         mutableThis->GetRaw()[mCount] = '\0';
         return *this;
      }
      else {
         // We have to branch-off and make another allocation           
         Text result;
         const auto request = RequestSize(mCount + 1);
         result.mType = mType;
         result.AllocateFresh(request);
         result.mCount = mCount;
         CopyMemory(result.mRaw, mRaw, mCount);
         result.GetRaw()[mCount] = '\0';
         return Abandon(result);
      }
   }

   /// Make all letters lowercase                                             
   ///   @return a new text container with all letter made lowercase          
   LANGULUS(INLINED)
   Text Text::Lowercase() const {
      Text result = Clone(*this);
      for (auto& i : result)
         i = static_cast<Letter>(::std::tolower(i));
      return result;
   }

   /// Make all letters uppercase                                             
   ///   @return a new text container with all letter made uppercase          
   LANGULUS(INLINED)
   Text Text::Uppercase() const {
      Text result = Clone(*this);
      for (auto& i : result)
         i = static_cast<Letter>(::std::toupper(i));
      return result;
   }

   /// Select a substring                                                     
   ///   @param start - offset of the starting character                      
   ///   @param count - the number of characters after 'start'                
   ///   @return new text that references the original memory                 
   LANGULUS(INLINED)
   Text Text::Select(CT::Index auto start, Count count) const IF_UNSAFE(noexcept) {
      return Block::Select<Text>(start, count);
   }

   LANGULUS(INLINED)
   Text Text::Select(CT::Index auto start, Count count) IF_UNSAFE(noexcept) {
      return Block::Select<Text>(start, count);
   }

   /// Select a substring on the right of the index                           
   ///   @param start - offset of the starting character                      
   ///   @return new text that references the original memory                 
   LANGULUS(INLINED)
   Text Text::Select(CT::Index auto start) const IF_UNSAFE(noexcept) {
      const auto index = SimplifyIndex(start);
      return Select(index, mCount - index);
   }

   LANGULUS(INLINED)
   Text Text::Select(CT::Index auto start) IF_UNSAFE(noexcept) {
      const auto index = SimplifyIndex(start);
      return Select(index, mCount - index);
   }

   /// Remove all instances of 'what' from the text container                 
   ///   @param what - the character/string to remove                         
   ///   @return a new container with the text stripped                       
   Text Text::Strip(const CT::Text auto& what) const {
      const Text pattern = Disowned(what);
      if (IsEmpty() or pattern.IsEmpty())
         return *this;

      Text result;
      result.Reserve(mCount);

      Count copyStart = 0, copyEnd = 0;
      for (Count i = 0; i <= mCount - pattern.mCount; ++i) {
         const auto matches = CropInner(i, mCount - i).Matches(pattern);
         if (matches == pattern.mCount) {
            // Found a match, skip it                                   
            // Copy any text that was skipped                           
            const auto copy = copyEnd - copyStart;
            if (copy) {
               CopyMemory(
                  result.GetRaw() + result.mCount,
                  GetRaw() + copyStart,
                  copy
               );
               result.mCount += copy;
            }

            copyStart = copyEnd = i + matches;
            i += matches - 1;
         }
         else ++copyEnd;
      }

      // Account for any leftover                                       
      const auto copy = copyEnd - copyStart;
      if (copy) {
         CopyMemory(
            result.GetRaw() + result.mCount,
            GetRaw() + copyStart,
            copy
         );
         result.mCount += copy;
      }

      return result;
   }
   
   /// Replace every occurence of 'what' with the provided string             
   ///   @param what - characters/strings to search for                       
   ///   @param with - characters/strings to replace with                     
   ///   @return a new container with the text replaced                       
   Text Text::Replace(const CT::Text auto& what, const CT::Text auto& with) const {
      const Text pattern = Disowned(what);
      const Text replacement = Disowned(with);
      if (IsEmpty() or pattern.IsEmpty())
         return *this;

      Text result;
      result.Reserve(mCount); // Not exact, but a good heuristic        

      Count copyStart = 0, copyEnd = 0;
      for (Count i = 0; i <= mCount - pattern.mCount; ++i) {
         const auto matches = CropInner(i, mCount - i).Matches(pattern);

         if (matches == pattern.mCount) {
            // Found a match, replace it                                
            const auto copy = copyEnd - copyStart;
            auto segment = result.Extend(copy + replacement.mCount);

            if (copy) {
               CopyMemory(
                  segment.GetRaw(),
                  GetRaw() + copyStart,
                  copy
               );
               CopyMemory(
                  segment.GetRaw() + copy,
                  replacement.GetRaw(),
                  replacement.mCount
               );
            }
            else CopyMemory(
               segment.GetRaw(),
               replacement.GetRaw(),
               replacement.mCount
            );

            copyStart = copyEnd = i + matches;
            i += matches - 1;
         }
         else ++copyEnd;
      }

      // Account for any leftover                                       
      const auto copy = copyEnd - copyStart;
      if (copy) {
         CopyMemory(
            result.GetRaw() + result.mCount,
            GetRaw() + copyStart,
            copy
         );
         result.mCount += copy;
      }
      return result;
   }

   /// Extend the text container and return a referenced part of it           
   ///   @return a container that represents the extended part                
   LANGULUS(INLINED)
   Text Text::Extend(const Count count) {
      return Block::Extend<Text>(count);
   }

   /// Insert text, or element convertible to text                            
   ///   @param what - the text container/convertible element                 
   void Text::UnfoldInsert(auto&& what) {
      using T = Deref<decltype(what)>;
      if constexpr (CT::TextBased<T>) {
         Base::AllocateMore(mCount + what.mCount);
         CopyMemory(GetRaw() + mCount, what.GetRaw(), what.mCount);
         mCount += what.mCount;
      }
      else {
         const auto size = fmt::formatted_size("{}", what);
         Base::AllocateMore(mCount + size);
         fmt::format_to_n(GetRaw() + mCount, size, "{}", what);
         mCount += size;
      }
   }

   /// Interpret text container as a string_view                              
   ///   @attention the string is null-terminated only after Terminate()      
   LANGULUS(INLINED)
   Text::operator Token() const noexcept {
      return {GetRaw(), mCount};
   }

   /// Compare with another block                                             
   ///   @param rhs - the block to compare with                               
   ///   @return true if blocks are the same                                  
   LANGULUS(INLINED)
   bool Text::operator == (const CT::Block auto& rhs) const noexcept {
      using B = Deref<decltype(rhs)>;

      if constexpr (not B::TypeErased) {
         if constexpr (CT::Similar<Letter, TypeOf<B>>) {
            // Comparing with another Text or TMany<Letter> - we can    
            // compare directly                                         
            return Base::operator == (static_cast<const Base&>(rhs));
         }
         else if constexpr (CT::Character<TypeOf<B>>) {
            // We're comparing with a different type of characters -    
            // do UTF conversions here                                  
            TODO();
         }
         else return false;
      }
      else {
         // Type-erased compare                                         
         return Base::operator == (rhs);
      }
   }

   /// Compare with a single character                                        
   ///   @param rhs - the character to compare with                           
   ///   @return true if this container contains this exact character         
   LANGULUS(INLINED)
   bool Text::operator == (const CT::Character auto& rhs) const noexcept {
      return operator == (Text::From(Disown(&rhs), 1));
   }

   /// Compare with a null-terminated or bounded literal string               
   ///   @param rhs - the string to compare with                              
   ///   @return true if this container contains this exact string            
   LANGULUS(INLINED)
   bool Text::operator == (const CT::String auto& rhs) const noexcept {
      using T = Deref<decltype(rhs)>;
      if constexpr (not CT::Array<T>) {
         if (rhs == nullptr or *rhs == 0)
            return IsEmpty();
      }
      else if (rhs[0] == 0)
         return IsEmpty();
      return operator == (Text {Disown(rhs)});
   }

   /// Compare with standard contiguous range statically typed with           
   /// characters. This includes std::string, string_view, span, vector,      
   /// array, etc. Containers that aren't playing by string rules will have   
   /// their data strnlen'd                                                   
   ///   @param rhs - the string to compare with                              
   ///   @return true if this container contains this exact string            
   LANGULUS(INLINED)
   bool Text::operator == (const CT::StdString auto& rhs) const noexcept {
      return operator == (Text {Disown(rhs)});
   }

   /// Comparing against nullptr_t checks if container is empty               
   ///   @return true if container is empty                                   
   LANGULUS(INLINED)
   bool Text::operator == (::std::nullptr_t) const noexcept {
      return IsEmpty();
   }
   
   /// Insert an element at the back of the container                         
   ///   @param rhs - the element to insert                                   
   ///   @return a reference to this container for chaining                   
   template<class T> requires CT::Stringifiable<Deint<T>> LANGULUS(ALWAYS_INLINED)
   Text& Text::operator << (T&& rhs) {
      Base::InsertBlockInner<void, true>(IndexBack, Text {Forward<T>(rhs)});
      return *this;
   }

   /// Insert an element at the front of the container                        
   ///   @param rhs - the element to insert                                   
   ///   @return a reference to this container for chaining                   
   template<class T> requires CT::Stringifiable<Deint<T>> LANGULUS(ALWAYS_INLINED)
   Text& Text::operator >> (T&& rhs) {
      Base::InsertBlockInner<void, true>(IndexFront, Text {Forward<T>(rhs)});
      return *this;
   }

   /// Merge an element at the back of the container                          
   ///   @param rhs - the element to insert                                   
   ///   @return a reference to this container for chaining                   
   template<class T> requires CT::Stringifiable<Deint<T>> LANGULUS(ALWAYS_INLINED)
   Text& Text::operator <<= (T&& rhs) {
      Base::MergeBlock<void>(IndexBack, Text {Forward<T>(rhs)});
      return *this;
   }

   /// Merge an element at the front of the container                         
   ///   @param rhs - the element to insert                                   
   ///   @return a reference to this container for chaining                   
   template<class T> requires CT::Stringifiable<Deint<T>> LANGULUS(ALWAYS_INLINED)
   Text& Text::operator >>= (T&& rhs) {
      Base::MergeBlock<void>(IndexFront, Text {Forward<T>(rhs)});
      return *this;
   }

   /// Concatenate with anything convertible to text on the right             
   ///   @param rhs - right hand side                                         
   ///   @return the concatenated text container                              
   template<class T> requires CT::Stringifiable<Deint<T>> LANGULUS(ALWAYS_INLINED)
   Text Text::operator + (T&& rhs) const {
      return ConcatInner<Text>(Forward<T>(rhs));
   }

   /// Concatenate with anything convertible to text on the left              
   ///   @param lhs - left hand side                                          
   ///   @param rhs - right hand side                                         
   ///   @return the concatenated text container                              
   template<class T> requires (CT::Stringifiable<Deint<T>> and not CT::TextBased<T>)
   LANGULUS(ALWAYS_INLINED)
   Text operator + (T&& lhs, const Text& rhs) {
      return Text {Forward<T>(lhs)}.ConcatInner<Text>(rhs);
   }

   /// Concatenate (destructively) text containers                            
   ///   @param rhs - right hand side                                         
   ///   @return a reference to this container                                
   template<class T> requires CT::Stringifiable<Deint<T>> LANGULUS(ALWAYS_INLINED)
   Text& Text::operator += (T&& rhs) {
      return ConcatRelativeInner<Text>(Forward<T>(rhs));
   }

   /// Inner concatenation function, used in all Text derivatives             
   ///   @param rhs - right hand side                                         
   ///   @return the concatenated text container                              
   template<CT::TextBased THIS, class T>
   THIS Text::ConcatInner(T&& rhs) const {
      using S = IntentOf<decltype(rhs)>;
      using B = TypeOf<S>;

      if constexpr (CT::Block<B>) {
         if constexpr (not B::TypeErased) {
            if constexpr (CT::Similar<Letter, TypeOf<B>>) {
               // We can concat directly                                
               return Block::ConcatBlock<THIS>(S::Nest(rhs));
            }
            else if constexpr (CT::Character<TypeOf<B>>) {
               // We're concatenating with different type of characters 
               // - do UTF conversions here                             
               TODO();
            }
            else static_assert(false, "Can't concatenate with this container");
         }
         else {
            // Type-erased concat                                       
            return Block::ConcatBlock<THIS>(S::Nest(rhs));
         }
      }
      else {
         // RHS isn't Block, try to convert it to Text, and nest        
         return ConcatInner<THIS>(static_cast<THIS>(DeintCast(rhs)));
      }
   }

   /// Inner concatenation function, used in all Text derivatives             
   ///   @param rhs - right hand side                                         
   ///   @return a reference to this container                                
   template<CT::TextBased THIS, class T>
   THIS& Text::ConcatRelativeInner(T&& rhs) {
      using S = IntentOf<decltype(rhs)>;
      using B = TypeOf<S>;

      if constexpr (CT::Block<B>) {
         if constexpr (not B::TypeErased) {
            if constexpr (CT::Similar<Letter, TypeOf<B>>) {
               // We can concat directly                                
               Base::InsertBlock<void>(IndexBack, S::Nest(rhs));
            }
            else if constexpr (CT::Character<TypeOf<B>>) {
               // We're concatenating with different type of characters 
               // - do UTF conversions here                             
               TODO();
            }
            else static_assert(false, "Can't concatenate with this container");
         }
         else {
            // Type-erased concat                                       
            Block::InsertBlock<void>(IndexBack, S::Nest(rhs));
         }
      }
      else {
         // RHS isn't Block, try to convert it to Text, and nest        
         return ConcatRelativeInner<THIS>(static_cast<THIS>(DeintCast(rhs)));
      }

      return static_cast<THIS&>(*this);
   }

   /// Byte container can always be represented by a type-erased one          
   /// This is however an explicit coverter, so that Text doesn't get         
   /// silently treated as a Many when passed around as an argument           
   LANGULUS(ALWAYS_INLINED)
   Text::operator Many& () noexcept {
      // Just make sure that type member has been populated             
      (void) Base::GetType();
      return reinterpret_cast<Many&>(*this);
   }

   /// Generate hexadecimal string from a given value                         
   ///   @param format - the template string                                  
   ///   @param args... - the arguments                                       
   ///   @return the instantiated template                                    
   Text Text::Hex(const auto& from) {
      Text result;
      result.AllocateFresh(result.RequestSize(sizeof(from) * 2));
      auto from_bytes = reinterpret_cast<const std::byte*>(&from);
      auto to_bytes = result.GetRaw();
      for (Offset i = 0; i < sizeof(from); ++i)
         fmt::format_to_n(to_bytes + i * 2, 2, "{:02X}", from_bytes[i]);
      result.mCount = sizeof(from) * 2;
      return result;
   }

   /// Fill template arguments using libfmt                                   
   /// If you get a constexpr error in this function, use TemplateRt instead  
   ///   @param format - the template string                                  
   ///   @param args... - the arguments                                       
   ///   @return the instantiated template                                    
   template<class...ARGS>
   Text Text::Template(const Token& format, ARGS&&...args) {
      const auto size = fmt::formatted_size(format, Forward<ARGS>(args)...);
      Text result;
      result.AllocateFresh(result.RequestSize(size));
      fmt::format_to_n(
         result.GetRaw(), size, format,
         Forward<ARGS>(args)...
      );
      result.mCount = size;
      return Abandon(result);
   }
   
   /// Fill template arguments using libfmt (at runtime)                      
   ///   @tparam ...ARGS - arguments for the template                         
   ///   @param format - the template string                                  
   ///   @param args... - the arguments                                       
   ///   @return the instantiated template                                    
   template<class...ARGS>
   Text Text::TemplateRt(const Token& format, ARGS&&...args) {
      const auto size = fmt::formatted_size(
         fmt::runtime(format),
         Forward<ARGS>(args)...
      );

      Text result;
      result.AllocateFresh(result.RequestSize(size));
      fmt::format_to_n(
         result.GetRaw(), size,
         fmt::runtime(format),
         Forward<ARGS>(args)...
      );
      result.mCount = size;
      return Abandon(result);
   }

   /// Should complain at compile-time if format string has mismatching       
   /// number of arguments                                                    
   template<::std::size_t...N> LANGULUS(INLINED)
   constexpr auto Text::CheckPattern(const Token& f, ::std::index_sequence<N...>) {
      return fmt::format_string<decltype(N)...> {fmt::runtime(f)};
   }

   /// Attempt filling the template, statically checking if argument count is 
   /// satisfied, and other erroneous conditions. Since arguments contain     
   /// unformattable data, that has more to do with seeking actual data from  
   /// the node hierarchy later, arguments are substituted with an index      
   /// sequence.                                                              
   ///   @tparam ...ARGS - arguments for the template                         
   ///   @param format - the template string                                  
   ///   @param args... - the arguments                                       
   template<class...ARGS> LANGULUS(INLINED)
   constexpr auto Text::TemplateCheck(const Token& f, ARGS&&...) {
      return CheckPattern(f, ::std::make_index_sequence<sizeof...(ARGS)> {});
   }

   /// Begins a content scope, decides whether to wrap contents in parentheses
   ///   @param from - the data to serialize                                  
   ///   @param to - the serialized data                                      
   ///   @return the number of written characters                             
   bool Text::SerializationRules::BeginScope(const CT::Block auto& from, Text& to) {
      const bool scoped = from.GetCount() > 1 or from.IsInvalid() or from.IsExecutable(); //TODO could check verb precedence to avoid scoping in some cases
      if (scoped) {
         if (from.IsPast())
            to += Operator::Past;
         else if (from.IsFuture())
            to += Operator::Future;

         to += Operator::OpenScope;
      }

      return scoped;
   }

   /// Ends a content scope, decides whether to wrap contents in parentheses  
   ///   @param from - the data to serialize                                  
   ///   @param to - the serialized data                                      
   ///   @return the number of written characters                             
   bool Text::SerializationRules::EndScope(const CT::Block auto& from, Text& to) {
      const bool scoped = from.GetCount() > 1 or from.IsInvalid() or from.IsExecutable(); //TODO could check verb precedence to avoid scoping in some cases
      if (scoped)
         to += Operator::CloseScope;
      return scoped;
   }

   /// Adds a separator between elements inside a scope, depending on the     
   /// block state - can be OR/AND separator                                  
   ///   @param from - the data to serialize                                  
   ///   @param to - the serialized data                                      
   ///   @return the number of written characters                             
   bool Text::SerializationRules::Separate(const CT::Block auto& from, Text& to) {
      const auto initial = to.GetCount();
      to += (from.IsOr() ? " or " : ", ");
      return to.GetCount() - initial;
   }

   /// Serialize charge as text                                               
   inline Charge::operator Text() const {
      Text text;
      if (mMass != Charge::DefaultMass)
         text += Text {Text::Operator::Mass, mMass};

      if (mRate != Charge::DefaultRate)
         text += Text {Text::Operator::Rate, mRate};

      if (mTime != Charge::DefaultTime)
         text += Text {Text::Operator::Time, mTime};

      if (mPriority != Charge::DefaultPriority)
         text += Text {Text::Operator::Priority, mPriority};

      return text;
   }

} // namespace Langulus::Annies

namespace Langulus
{

   /// Make a text literal                                                    
   LANGULUS(INLINED)
   Annies::Text operator ""_text(const char* text, ::std::size_t size) {
      return Annies::Text::From(Disown(text), size);
   }

} // namespace Langulus
