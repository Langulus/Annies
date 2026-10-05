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
   /// File path                                                              
   ///                                                                        
   struct Path : Text {
      using CTTI_ReflectAs = Path;
      using CTTI_Bases     = Text;

      static constexpr char Separator = '/';

      using Text::Text;

      Path(Text const&);
      Path(Text&&);

      auto GetExtension() const -> Text;
      auto GetDirectory() const -> Path;
      auto GetFilename()  const -> Path;

      auto operator /  (Text const&) const -> Path;
      auto operator /= (Text const&) -> Path&;
   };
}

namespace Langulus
{
   using Annies::Path;

   Path operator ""_path(const char*, size_t);
}
