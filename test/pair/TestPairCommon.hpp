///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           

/// INTENTIONALLY NOT GUARDED                                                 
/// Include this file once in each cpp file, after all other headers          
#include <Langulus/Annies/Text.hpp>
#include <Langulus/Annies/Tag.hpp>
#include <Langulus/Annies/Pair.hpp>
#include <Langulus/Annies/TPair.hpp>
#include <unordered_map>
#include "../many/TestManyCommon.hpp"


///                                                                           
/// Possible states:                                                          
///   - uninitialized                                                         
///   - default                                                               
template<class K, class V>
void Pair_CheckState_Default(auto const&);
///   - invariant                                                             
template<class K, class V>
void Pair_CheckState_Invariant(auto const&);
///   - owned-full                                                            
template<class K, class V>
void Pair_CheckState_OwnedFull(auto const&);
///   - owned-full-const                                                      
template<class K, class V>
void Pair_CheckState_OwnedFullConst(auto const&);
///   - owned-empty                                                           
template<class K, class V>
void Pair_CheckState_OwnedEmpty(auto const&);
///   - disowned-full                                                         
template<class K, class V>
void Pair_CheckState_DisownedFull(auto const&);
///   - disowned-full-const                                                   
template<class K, class V>
void Pair_CheckState_DisownedFullConst(auto const&);
///   - abandoned                                                             
template<class K, class V>
void Pair_CheckState_Abandoned(auto const&);


template<class K, class V>
void Pair_CheckState_Default(auto const& pair) {
   if constexpr (CT::Typed<decltype(pair)>) {
      Any_CheckState_DisownedFullConst<K>(pair.GetKeyBlock());
      Any_CheckState_DisownedFullConst<V>(pair.GetValueBlock());
   }
   else {
      Any_CheckState_Default<K>(pair.GetKeyBlock());
      Any_CheckState_Default<V>(pair.GetValueBlock());
   }
}

template<class K, class V>
void Pair_CheckState_OwnedFull(auto const& pair) {
   if constexpr (CT::Typed<decltype(pair)>) {
      Any_CheckState_DisownedFullConst<K>(pair.GetKeyBlock());
      Any_CheckState_DisownedFullConst<V>(pair.GetValueBlock());
   }
   else {
      Any_CheckState_OwnedFull<K>(pair.GetKeyBlock());
      Any_CheckState_OwnedFull<V>(pair.GetValueBlock());
   }
}

template<class K, class V>
void Pair_CheckState_OwnedEmpty(auto const& pair) {
   if constexpr (CT::Typed<decltype(pair)>) {
      Any_CheckState_DisownedFullConst<K>(pair.GetKeyBlock());
      Any_CheckState_DisownedFullConst<V>(pair.GetValueBlock());
   }
   else {
      Any_CheckState_OwnedEmpty<K>(pair.GetKeyBlock());
      Any_CheckState_OwnedEmpty<V>(pair.GetValueBlock());
   }
}