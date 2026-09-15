///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Config.hpp"


namespace Langulus
{
   namespace A
   {

      /// An abstract handle                                                  
      struct Handle {
         LANGULUS(ACT_AS) void;
      };

   } // namespace Langulus::A

   namespace CT
   {

      /// Concept for differentiating Handle types                            
      template<class...T>
      concept Handle = (DerivedFrom<T, A::Handle> and ...);

      /// Opposite of a CT::Handle                                            
      template<class...T>
      concept NotHandle = ((not Handle<T>) and ...);

   } // namespace Langulus::CT

} // namespace Langulus

namespace Langulus::Annies
{
   
   ///                                                                        
   ///   An element & allocation pair                                         
   ///                                                                        
   ///   Used as intermediate type when managed memory is enabled, to keep    
   /// track of pointers inserted or accessed to/from containers. This does   
   /// not have ownership, and can be used as iterator only when EMBEDed.     
   ///                                                                        
   template<class T, bool EMBED>
   struct Handle : A::Handle {
      LANGULUS(TYPED) T;
      LANGULUS(ABSTRACT) false;
      LANGULUS_BASES(A::Handle);

   public:
      static_assert(CT::NotHandle<T>, "Handles can't be nested");
      static constexpr bool Embedded   = EMBED;
      static constexpr bool TypeErased = CT::TypeErased<Decay<T>>;
      static constexpr bool Sparse     = CT::Sparse<T>;
      static constexpr bool Dense      = not Sparse;
      static constexpr bool Mutable    = CT::Mutable<T>;
      static constexpr bool CTTI_Container = true;

      static_assert(Embedded or not TypeErased or Sparse,
         "Can't have a type-erased local handle, unless it is sparse");

      using Type = Conditional<TypeErased and Dense, Byte, T>;
      using AllocType = const Allocation*;
      using ValueType = Conditional<Embedded, Type*, Type>;
      using EntryType = Conditional<Embedded and Sparse,
            Conditional<Mutable, AllocType*, AllocType const*>,
            AllocType>;

      friend struct Block<T>;

      /// @cond show_protected                                                
      // The value                                                      
      ValueType mValue;
      // The entry                                                      
      EntryType mEntry;
      /// @endcond show_protected                                             

   public:
      Handle() = delete;

      constexpr Handle(const Handle&) noexcept = default;
      constexpr Handle(Handle&&) noexcept = default;

      constexpr Handle(ValueType) noexcept requires (Embedded or Sparse);
      constexpr Handle(ValueType, EntryType) noexcept requires (Embedded or Sparse);

      constexpr Handle(auto&&) noexcept requires (not Embedded);

      // Construct from handle                                          
      template<template<class> class S, CT::Handle H>
      requires CT::IntentMakable<S, T>
      constexpr Handle(S<H>&&);

      ~Handle();

      auto MakeConst() const noexcept -> Handle<const T, EMBED> {
         return {mValue, mEntry};
      }

      constexpr Handle& operator = (Handle const&) noexcept = default;
      constexpr Handle& operator = (Handle&&)      noexcept = default;

      constexpr bool operator == (const auto&) const noexcept
      requires (not TypeErased or Sparse);

      auto Get()       noexcept -> Type&;
      auto Get() const noexcept -> Type const&;

      auto GetEntry()       noexcept -> AllocType&;
      auto GetEntry() const noexcept -> AllocType const&;

      void CreateWithIntent(auto&&, DMeta = {});
      void Assign(const Type&, AllocType = nullptr) noexcept requires (Embedded and Mutable);
      void AssignWithIntent(auto&&, DMeta = {}) requires Mutable;
      void Swap(CT::Handle auto&, DMeta = {}) requires Mutable;
      bool Compare(const auto&, DMeta = {}) const;

      template<bool RESET = false, bool DEALLOCATE = true>
      void FreeInner(DMeta = {}) requires Mutable;

      // Prefix operators                                               
      auto operator ++ () noexcept -> Handle& requires Embedded;
      auto operator -- () noexcept -> Handle& requires Embedded;

      // Suffix operators                                               
      auto operator ++ (int) const noexcept -> Handle requires Embedded;
      auto operator -- (int) const noexcept -> Handle requires Embedded;

      auto operator +  (Offset) const noexcept -> Handle requires Embedded;
      auto operator -  (Offset) const noexcept -> Handle requires Embedded;

      auto operator += (Offset) noexcept -> Handle& requires Embedded;
      auto operator -= (Offset) noexcept -> Handle& requires Embedded;
   };
   
   template<CT::NotHandle T>
   using HandleLocal = Handle<T, false>;

} // namespace Langulus::Annies
