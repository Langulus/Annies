///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Export.hpp"
#include <Langulus/CT/Void.hpp>
#include <Langulus/CT/Contiguous.hpp>
#include <Langulus/CT/Handle.hpp>


/// Include this file when you deal only with container references and        
/// pointers. Containers will remain incomplete after including this, but     
/// it's especially useful when you don't want to bring in a lot of inlined   
/// code.                                                                     
namespace Langulus::Annies
{
   /// Each state can be either dynamically set at runtime, be statically     
   /// enabled, or statically disabled. Only the 'Variable' value reserves    
   /// a single bit inside the state component representation.                
   enum class StateValue {
      Variable = 0,
      Enabled  = 1,
      Disabled = 2
   };

   struct Handle;
   struct HandleMut;
   struct HandleDisowned;
   struct HandleDisownedMut;

   #if not LANGULUS(FORCE_TYPE_ERASURE)
      template<class> struct THandle;
      template<class> struct THandleEmergent;
      template<class> struct THandleDisowned;
   #endif
   
   template<CT::Handle, CT::Handle>
   struct THandlePair;

   template<CT::Contiguous T>
   class Edit;

   struct Any;
   template<CT::NotVoid>
   struct TAny;

   struct Bytes;

   struct Recipe;

   struct Many;
   template<CT::NotVoid>
   struct TMany;

   struct Neat;
   struct Path;
   struct Text;

   template<class> 
   struct TTag;
   using Tag = TTag<void>;

   template<class> 
   struct TVerb;
   using Verb = TVerb<void>;

   template<CT::NotVoid, class...>
   struct TOwn;
   template<CT::NotVoid T, class...TAGS>
   using Own = TOwn<T, TAGS...>;

   template<class, class...>
   struct TRef;
   template<class T>
   using Ref = TRef<T*>;

   template<CT::NotVoid, class...>
   struct TPin;
   template<class...T>
   using Pin = TPin<T...>;

   template<CT::NotVoid>
   struct THive;
   template<CT::NotVoid T> 
   struct TSlots;

   struct Table;
   template<CT::NotVoid T>
   struct TTable;

   struct Pair;
   template<CT::NotVoid, CT::NotVoid>
   struct TPair;

   namespace Inner
   {
      template<StateValue SORT = StateValue::Variable>
      struct Map;

      template<StateValue SORT = StateValue::Variable>
      struct Set;
   }

   using Map         = Inner::Map<StateValue::Variable>;
   using MapSorted   = Inner::Map<StateValue::Enabled>;
   using MapUnsorted = Inner::Map<StateValue::Disabled>;

   template<class, class, StateValue SORT = StateValue::Variable>
   struct TMap;
   template<class K, class V>
   using TMapSorted = TMap<K, V, StateValue::Enabled>;
   template<class K, class V>
   using TMapUnsorted = TMap<K, V, StateValue::Disabled>;

   using Set         = Inner::Set<StateValue::Variable>;
   using SetSorted   = Inner::Set<StateValue::Enabled>;
   using SetUnsorted = Inner::Set<StateValue::Disabled>;

   template<CT::NotVoid, StateValue SORT = StateValue::Variable>
   struct TSet;
   template<CT::NotVoid T>
   using TSetSorted = TSet<T, StateValue::Enabled>;
   template<CT::NotVoid T>
   using TSetUnsorted = TSet<T, StateValue::Disabled>;
}

namespace Langulus
{
   struct Downcastable;
   using Annies::Own;
   using Annies::Pin;
   using Annies::Ref;

   using Annies::Any;
   using Annies::TAny;

   using Annies::Many;
   using Annies::TMany;

   using Annies::Bytes;

   using Annies::Map;
   using Annies::MapSorted;
   using Annies::MapUnsorted;
   using Annies::TMap;
   using Annies::TMapSorted;
   using Annies::TMapUnsorted;

   using Annies::Set;
   using Annies::SetSorted;
   using Annies::SetUnsorted;
   using Annies::TSet;
   using Annies::TSetSorted;
   using Annies::TSetUnsorted;

   using Annies::Pair;
   using Annies::TPair;

   using Annies::Path;
   using Annies::Text;

   using Annies::Table;
   using Annies::TTable;

   using Annies::Recipe;
   using Annies::Neat;
   using Annies::THive;
   using Annies::TSlots;

   using Annies::Tag;
   using Annies::Verb;
}