///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Text.hpp"


namespace Langulus::Annies
{

   ///                                                                        
   ///   File path container                                                  
   ///                                                                        
   struct Path : Text {
      LANGULUS(NAME) "Path";
      LANGULUS(FILES) "";
      LANGULUS(ACT_AS) Path;
      LANGULUS_BASES(Text);
      LANGULUS_CONVERTS_FROM(Text);

      static constexpr char Separator = '/';

      using Text::Text;
      using Text::operator +=;
      using Text::operator ==;

      Path(const Text&);
      Path(Text&&);

      auto GetExtension() const -> Text;
      auto GetDirectory() const -> Path;
      auto GetFilename()  const -> Path;

      auto operator /  (const Text&) const -> Path;
      auto operator /= (const Text&) -> Path&;

   private:
      using Text::SerializationRules;
   };

} // namespace Langulus::Annies

namespace Langulus
{

   Annies::Path operator ""_path(const char*, ::std::size_t);

} // namespace Langulus
