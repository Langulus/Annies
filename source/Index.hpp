///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Config.hpp"
#include <Langulus/CT/Integer.hpp>
#include <Langulus/CT/Signed.hpp>
#include <Langulus/CT/Real.hpp>


namespace Langulus::Annies
{

   ///                                                                        
   ///   A multipurpose index, used to access common elements in containers   
   ///                                                                        
   struct Index {
      using CTTI_Named     = Yes<"Index">;
      using CTTI_Suffix    = Yes<"i">;
      using CTTI_Info      = Yes<"Used to safely access elements inside containers">;
      using CTTI_POD       = Yup;
      using CTTI_Nullable  = Yup;
      using CTTI_Number    = Yup;

   protected:
      using Type = ::std::ptrdiff_t;

      /// These are defines useful for special indices                        
      static constexpr Type MaxIndex = ::std::numeric_limits<Type>::max();
      static constexpr Type MinIndex = ::std::numeric_limits<Type>::min();

   public:
      enum SpecialIndices : Type {
         // All, Many, and Single must be compared in separate context  
         All = MinIndex,
         Many,
         Single,

         // Back, Middle, Front, and None must be compared separately   
         None,
         Front,
         Middle,
         Back,

         // These can't be compared                                     
         Mode,
         Biggest,
         Smallest,
         Auto,
         Random,

         // This signifies the end of the special indices               
         Counter,

         // These must be wrapped before compared                       
         Last = -1,

         // These fit into the non-special category                     
         First = 0
      };

      using CTTI_Values = Values<
         All,
         Many,
         Single,

         None,
         Front,
         Middle, 
         Back,

         Mode,
         Biggest,
         Smallest,
         Auto,
         Random,

         Last,
         First
      >;

      #if LANGULUS_DEBUG()
         union {
            // Named index (useful for debugging)                       
            SpecialIndices mNamedIndex = SpecialIndices::None;
            // Raw index                                                
            Type mIndex;
         };
      #else
         Type mIndex = SpecialIndices::None;
      #endif

   public:
      constexpr Index() noexcept = default;
      constexpr Index(const Index&) noexcept = default;
      constexpr Index(const SpecialIndices& value) noexcept
         : mIndex {value} { }
      template<CT::Integer T> requires CT::Signed<T>
      constexpr Index(const T&) noexcept (sizeof(T) < sizeof(Type));
      template<CT::Integer T> requires CT::Unsigned<T>
      constexpr Index(const T&) noexcept (sizeof(T) <= sizeof(Type)/2);
      constexpr Index(const CT::Real auto&);

      constexpr Index& operator = (const Index&) noexcept = default;

   public:
      constexpr Index Constrained(size_t) const noexcept;
      size_t GetOffset() const;
      size_t GetOffsetUnsafe() const noexcept;

      constexpr void Constrain(size_t) noexcept;
      constexpr void Concat(const Index&) noexcept;

      constexpr bool IsValid() const noexcept;
      constexpr bool IsInvalid() const noexcept;
      constexpr bool IsSpecial() const noexcept;
      constexpr bool IsReverse() const noexcept;
      constexpr bool IsArithmetic() const noexcept;

      explicit constexpr operator bool() const noexcept;
      explicit constexpr operator const Type& () const noexcept;

      constexpr void operator ++ () noexcept;
      constexpr void operator -- () noexcept;
      constexpr void operator += (const Index&) noexcept;
      constexpr void operator -= (const Index&) noexcept;
      constexpr void operator *= (const Index&) noexcept;
      constexpr void operator /= (const Index&) noexcept;

      constexpr Index operator + (const Index&) const noexcept;
      constexpr Index operator - (const Index&) const noexcept;
      constexpr Index operator * (const Index&) const noexcept;
      constexpr Index operator / (const Index&) const noexcept;
      constexpr Index operator - () const noexcept;

      constexpr bool operator == (const Index&) const noexcept;
      constexpr bool operator <  (const Index&) const noexcept;
      constexpr bool operator >  (const Index&) const noexcept;
      constexpr bool operator <= (const Index&) const noexcept;
      constexpr bool operator >= (const Index&) const noexcept;
   };
   
   constexpr Index IndexAll      {Index::All};
   constexpr Index IndexMany     {Index::Many};
   constexpr Index IndexSingle   {Index::Single};
   constexpr Index IndexNone     {Index::None};
   constexpr Index IndexFront    {Index::Front};
   constexpr Index IndexMiddle   {Index::Middle};
   constexpr Index IndexBack     {Index::Back};
   constexpr Index IndexMode     {Index::Mode};
   constexpr Index IndexBiggest  {Index::Biggest};
   constexpr Index IndexSmallest {Index::Smallest};
   constexpr Index IndexAuto     {Index::Auto};
   constexpr Index IndexRandom   {Index::Random};
   constexpr Index IndexFirst    {Index::First};
   constexpr Index IndexLast     {Index::Last};
}

namespace Langulus::CT
{

   /// Generalized index concept                                              
   template<class T>
   concept Index = Integer<T> or Same<T, Annies::Index>;

} // namespace Langulus::CT
