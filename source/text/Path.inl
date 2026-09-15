///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Path.hpp"
#include "Text.inl"


namespace Langulus::Annies
{

   /// Construct by copying a text container                                  
   ///   @param other - the container to copy                                 
   LANGULUS(INLINED)
   Path::Path(const Text& other)
      : Text {other} {}

   /// Construct by moving a text container                                   
   ///   @param other - the container to copy                                 
   LANGULUS(INLINED)
   Path::Path(Text&& other)
      : Text {Forward<Text>(other)} {}

   /// Return the lowercase file extension (the part after the last '.')      
   ///   @return a cloned text container with the extension                   
   LANGULUS(INLINED)
   auto Path::GetExtension() const -> Text {
      const auto found = Find<true>('.');
      return found ? Text::Select(found.GetOffsetUnsafe() + 1) : Text {};
   }

   /// Return the directory part of the path                                  
   ///   @return the directory part, including the last '/'                   
   LANGULUS(INLINED)
   auto Path::GetDirectory() const -> Path {
      const auto found = Find<true>(Separator);
      return found ? Text::Select(0, found.GetOffsetUnsafe() + 1) : Path {};
   }

   /// Return the filename part of the path                                   
   ///   @return the filename part (after the last '/')                       
   LANGULUS(INLINED)
   auto Path::GetFilename() const -> Path {
      const auto found = Find<true>(Separator);
      return found ? Text::Select(found + 1) : Path {*this};
   }

   /// Append a subdirectory or filename                                      
   ///   @param rhs - the text to append                                      
   ///   @return the combined directory name                                  
   inline auto Path::operator / (const Text& rhs) const -> Path {
      if (IsEmpty())
         return rhs;

      if (*last() == Separator) {
         if (*rhs.last() == Separator)
            return *this + rhs.Select(1);
         else
            return *this + rhs;
      }
      else {
         if (*rhs.last() == Separator)
            return *this + rhs;
         else {
            auto temp = *this + Text {Separator};
            temp += rhs;
            return Path {Abandon(temp)};
         }
      }
   }

   /// Append a subdirectory or filename                                      
   ///   @param rhs - the text to append                                      
   ///   @return the combined directory name                                  
   inline auto Path::operator /= (const Text& rhs) -> Path& {
      if (IsEmpty())
         return operator = (rhs);

      if (*last() == Separator) {
         if (*rhs.last() == Separator)
            *this += rhs.Select(1);
         else
            *this += rhs;
      }
      else {
         if (*rhs.last() == Separator)
            *this += rhs;
         else {
            *this += Text {Separator};
            *this += rhs;
         }
      }

      return *this;
   }

} // namespace Langulus::Annies
