///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Config.hpp"


namespace Langulus
{

   ///                                                                        
   ///   Data state flags                                                     
   ///                                                                        
   #pragma pack(push, 1)
   struct DataState {
      LANGULUS(POD) true;
      LANGULUS(NULLIFIABLE) true;

      enum Enum : ::std::uint8_t {
         // Default data state                                          
         // Default state is inclusive, mutable, nonmissing             
         // nonstatic, nonencrypted, noncompressed, and dense           
         Default = 0,

         // Tags data as missing                                        
         // Missing data is considered only a hint, that is used to     
         // direct data expansion, when combined with Future/Past       
         Missing = 1,

         // The data is compressed, and it is your respondibility to    
         // decompress it before using it                               
         Compressed = 2,

         // The data is encrypted, and it is your respondibility to     
         // decrypt it with the correct key before using it             
         Encrypted = 4,

         // Enables inhibition (or so called exclusive (OR) container)  
         // An OR container means that data can be used in multiple     
         // parallel ways. Verbs in such containers, for example, are   
         // considered branched. Beware, using OR containers might      
         // cause	huge overhead, because in some contexts a full        
         // stack cloning might occur                                   
         Or = 8,

         // Future missing symbol, when missing                         
         Future = 16,

         // If state is only Missing, then it is missing past           
         Past = Missing,

         // Data won't move, reallocate, deallocate, or even change     
         // Used to constrain the memory manipulations for safety       
         // That way you can interface constant memory as any other     
         Constant = 32,

         // Data won't ever change type - useful for templated packs    
         // Used to constrain the memory manipulations for safety       
         Typed = 64,

      #if LANGULUS(DEBUG)
         // Tracks the memory, useful for detecting abnormalities       
         Tracked = 128,
      #endif

         // Data is fully constrained                                   
         // Useful set of states to interface a constant member         
         Constrained = Constant | Typed,

         MissingFuture = Missing | Future,
         MissingPast = Missing | Past
      };

      using Type = TypeOf<Enum>;

      Type mState {Default};

   public:
      constexpr DataState() noexcept = default;
      constexpr DataState(const Type&) noexcept;

      explicit constexpr operator bool() const noexcept;
      constexpr bool operator == (const DataState&) const noexcept = default;
      
      constexpr DataState  operator +  (const DataState&) const noexcept;
      constexpr DataState  operator -  (const DataState&) const noexcept;
      constexpr DataState& operator += (const DataState&) noexcept;
      constexpr DataState& operator -= (const DataState&) noexcept;
      constexpr DataState& operator &= (const DataState&) noexcept;
      
      constexpr bool operator & (const DataState&) const noexcept;
      constexpr bool operator % (const DataState&) const noexcept;
      
      constexpr bool IsDefault() const noexcept;
      constexpr bool IsMissing() const noexcept;
      constexpr bool IsCompressed() const noexcept;
      constexpr bool IsEncrypted() const noexcept;
      constexpr bool IsOr() const noexcept;
      constexpr bool IsNow() const noexcept;
      constexpr bool IsFuture() const noexcept;
      constexpr bool IsPast() const noexcept;
      constexpr bool IsConstant() const noexcept;
      constexpr bool IsTyped() const noexcept;
      constexpr bool IsConstrained() const noexcept;
      
      constexpr void Reset() noexcept;
   };
   #pragma pack(pop)

} // namespace Langulus