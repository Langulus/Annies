///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Config.hpp"


namespace Langulus::Annies
{
   
   ///                                                                        
   ///   Verb state flags                                                     
   ///                                                                        
   struct VerbState {
      using CTTI_POD = Yup;
      using CTTI_Nullable = Yup;

      enum Enum : ::std::uint8_t {
         // Default verb state                                          
         // Default state is short-circuited multicast                  
         Default = 0,

         // When verb is long-circuited (as opposed to short-circuited) 
         // it will not cease executing on success, and be executed for 
         // each element in the context if multicasted. Used usually in 
         // interpretation, when you want to guarantee all elements are 
         // converted                                                   
         LongCircuited = 1,

         // When verb is monocast (as opposite to multicast) it will    
         // not iterate deep items, but be executed on the context once 
         // as a whole. Used extensively when executing at compile-time 
         //Monocast = 2
      };

      using Type = TypeOf<Enum>;

      Type mState {Default};

   public:
      constexpr VerbState() noexcept = default;
      constexpr VerbState(const Type&) noexcept;

      explicit constexpr operator bool() const noexcept;
      constexpr bool operator == (const VerbState&) const noexcept = default;
      
      constexpr VerbState  operator +  (const VerbState&) const noexcept;
      constexpr VerbState  operator -  (const VerbState&) const noexcept;
      constexpr VerbState& operator += (const VerbState&) noexcept;
      constexpr VerbState& operator -= (const VerbState&) noexcept;
      
      constexpr bool operator & (const VerbState&) const noexcept;
      constexpr bool operator % (const VerbState&) const noexcept;
      
      constexpr bool IsDefault() const noexcept;
      /*constexpr bool IsMulticast() const noexcept;
      constexpr bool IsMonocast() const noexcept;*/
      constexpr bool IsShortCircuited() const noexcept;
      constexpr bool IsLongCircuited() const noexcept;
      
      constexpr void Reset() noexcept;
   };

} // namespace Langulus::Annies
