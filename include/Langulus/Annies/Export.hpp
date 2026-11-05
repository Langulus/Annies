///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include <Langulus/Core.hpp>

#if defined(LANGULUS_EXPORT_ALL) or defined(LANGULUS_EXPORT_ANNIES)
   #define LANGULUS_API_ANNIES() LANGULUS_EXPORT()
#else
   #define LANGULUS_API_ANNIES() LANGULUS_IMPORT()
#endif

/// Make the rest of the code aware, that Langulus::Annies has been included  
#define LANGULUS_LIBRARY_ANNIES() 1
#define LANGULUS_ANNIES_VERBOSITY_MASTER_SWITCH() 0
