///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "SerializeText.hpp"


namespace Langulus::Annies
{
   ///                                                                        
   /// MARK: File path                                                        
   struct Path : Text {
      using CTTI_ReflectAs = Path;
      using CTTI_Named     = Yes<"Path">;
      using CTTI_Bases     = Text;

      static constexpr char Separator = '/';

      using Text::Text;
      using Text::operator ==;
      /*using Text::operator +;
      using Text::operator +=;
      */

      /// Construction from any kind of text that is an Annies container      
      template<CT::Text T> requires CT::Container<T>
      constexpr Path(T&& text) {
         this->Absorb(LglsFwd(text));
      }

      auto GetExtension() const -> Text {
         const auto found = Text::FindReverse('.');
         return found ? Text::Select((found - Text::GetHandle()) + 1) : Text {};
      }

      auto GetDirectory() const -> Path {
         const auto found = Text::FindReverse(Separator);
         return found ? this->Select(0, (found - Text::GetHandle()) + 1) : Path {};
      }

      auto GetFilename() const -> Path {
         const auto found = Text::FindReverse(Separator);
         return found ? this->Select((found - Text::GetHandle()) + 1) : Path {*this};
      }

      /// Append a subdirectory or filename                                   
      ///   @param rhs the text to append                                     
      ///   @return the combined directory name                               
      auto operator / (CT::Text auto&& rhs) const -> Path {
         if (IsEmpty())
            return rhs;

         if ((*this)[-1] == Separator) {
            if (rhs[-1] == Separator)
               return *this + rhs.Select(1);
            else
               return *this + rhs;
         }
         else {
            if (rhs[-1] == Separator)
               return *this + rhs;
            else {
               Path temp = *this;
               temp += Separator;
               temp += rhs;
               return temp;
            }
         }
      }

      /// Append a subdirectory or filename                                   
      ///   @param rhs the text to append                                     
      ///   @return the combined directory name                               
      auto operator /= (CT::Text auto&& rhs) -> Path& {
         if (IsEmpty())
            return (*this = rhs);

         if ((*this)[-1] == Separator) {
            if (rhs[-1] == Separator)
               *this += rhs.Select(1);
            else
               *this += rhs;
         }
         else {
            if (rhs[-1] == Separator)
               *this += rhs;
            else {
               *this += Separator;
               *this += rhs;
            }
         }

         return *this;
      }
   };
}

namespace Langulus
{
   using Annies::Path;

   Path operator ""_path(const char*, size_t);
}

LANGULUS_MORPHISM(Annies::Path, Annies::Text);

namespace Langulus::CTTI
{
   /// Rule for serializing Path to Text. Wraps it in `` symbols.             
   template<>
   struct SerializationRule<Annies::Text, Annies::Path> {
      using S = Serializer<Annies::Text>;
      using Context = typename S::Context;

      static void Serialize(Annies::Path const& item, Annies::Text& out, Context*) {
         out += Serial::OpenStringAlt.Token;
         out += item;
         out += Serial::CloseStringAlt.Token;
      }
   };
}