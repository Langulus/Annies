///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include <Langulus/MetaOf.hpp>


#if defined(LANGULUS_EXPORT_ALL) or defined(LANGULUS_EXPORT_ANNIES)
   #define LANGULUS_API_ANNIES() LANGULUS_EXPORT()
#else
   #define LANGULUS_API_ANNIES() LANGULUS_IMPORT()
#endif

/// Enable memory manager                                                     
#include <Langulus/Allocator.hpp>

/// Make the rest of the code aware, that Langulus::Annies has been included  
#define LANGULUS_LIBRARY_ANNIES() 1


namespace Langulus
{

   /// Loop controls from inside ForEach lambdas when iterating containers    
   struct LoopControl {
      enum Command : int {
         Break = 0,     // Break the loop                               
         Continue = 1,  // Continue the loop                            
         Repeat = 2,    // Repeat the current element                   
         Discard = 3,   // Remove the current element                   
         NextLoop = 4   // Skip to next function in the visitor pattern 
      } mControl;

      LoopControl() = delete;

      constexpr LoopControl(bool a) noexcept
         : mControl {static_cast<Command>(a)} {}
      constexpr LoopControl(Command a) noexcept
         : mControl {a} {}

      explicit constexpr operator bool() const noexcept {
         return mControl == Continue or mControl == Repeat;
      }

      constexpr bool operator == (const LoopControl& rhs) const noexcept {
         return mControl == rhs.mControl;
      }
   };

   namespace Loop
   {

      constexpr LoopControl Break      = LoopControl::Break;
      constexpr LoopControl Continue   = LoopControl::Continue;
      constexpr LoopControl Repeat     = LoopControl::Repeat;
      constexpr LoopControl Discard    = LoopControl::Discard;
      constexpr LoopControl NextLoop   = LoopControl::NextLoop;

   } // namespace Langulus::Loop

   namespace CT
   {

      /// The ultimate Annies container tag                                   
      /// Checks if all T are marked as Annies containers                     
      template<class...T>
      concept Container = (requires { Decay<T>::CTTI_Container; } and ...);

      /// Checks if none of the Ts are marked as Annies containers            
      template<class...T>
      concept NotContainer = ((not Container<T>) and ...);

   } // namespace Langulus::CT

   namespace Annies
   {
      #if LANGULUS_FEATURE(MANAGED_MEMORY)
         using Allocator  = ::Langulus::Fractalloc::Allocator;
         using Allocation = ::Langulus::Fractalloc::Allocation;
      #else
         using Allocator  = ::Langulus::Annies::Allocator;
         using Allocation = ::Langulus::Annies::Allocation;
      #endif

      using RTTI::DMeta;
      using RTTI::CMeta;
      using RTTI::TMeta;
      using RTTI::VMeta;
      //using RTTI::AMeta;
      //using RTTI::AllocationRequest;
      
      template<class TYPE = void>
      struct Block;

      template<class, bool EMBED = true>
      struct Handle;

      struct Many;
      using Messy = Many;
      template<CT::NotVoid>
      class TMany;

      struct BlockMap;

      template<bool>
      struct Map;
      using UnorderedMap = Map<false>;
      using OrderedMap = Map<true>;

      template<CT::NotVoid, CT::NotVoid, bool>
      struct TMap;
      template<CT::NotVoid K, CT::NotVoid V>
      using TOrderedMap = TMap<K, V, true>;
      template<CT::NotVoid K, CT::NotVoid V>
      using TUnorderedMap = TMap<K, V, false>;

      struct BlockSet;

      template<bool>
      struct Set;
      using UnorderedSet = Set<false>;
      using OrderedSet = Set<true>;

      template<CT::NotVoid, bool>
      struct TSet;
      template<CT::NotVoid T>
      using TOrderedSet = TSet<T, true>;
      template<CT::NotVoid T>
      using TUnorderedSet = TSet<T, false>;

      struct Bytes;
      struct Text;
      struct Path;

      template<CT::NotVoid>
      class Own;
      template<class>
      class Ref;

      class Construct;
      class Neat;

   }
}

#if 0
   #define VERBOSE_COMPARE(...)     Logger::Verbose(__VA_ARGS__)
   #define VERBOSE_COMPARE_TAB(...) const auto tab = Logger::Verbose(__VA_ARGS__, Logger::Tabs {})
#else
   #define VERBOSE_COMPARE(...)     LANGULUS(NOOP)
   #define VERBOSE_COMPARE_TAB(...) LANGULUS(NOOP)
#endif
