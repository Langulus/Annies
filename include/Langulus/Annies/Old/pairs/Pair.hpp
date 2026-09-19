///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../many/Many.hpp"


namespace Langulus
{
   namespace A
   {

      ///                                                                     
      ///   An abstract pair                                                  
      ///                                                                     
      struct Pair {
         using CTTI_Abstract = Yup;
         static constexpr bool CTTI_Container = true;
      };

   } // namespace Langulus::A

   namespace CT
   {

      /// Check if T is a pair type                                           
      ///	@attention not a test for binary compatibility!                   
      template<class...T>
      concept Pair = (DerivedFrom<T, A::Pair> and ...);

      /// Check if a type is a statically typed pair                          
      template<class...T>
      concept TypedPair = Pair<T...> and Typed<T...>;

   } // namespace Langulus::CT

   namespace Annies
   {

      ///                                                                        
      ///   A helper structure for pairing keys and values of any type           
      ///                                                                        
      ///   This is the type-erased pair, and it can contain dense or sparse     
      /// values.                                                                
      ///   @attention Pair is not binary-compatible with its statically typed   
      ///      counterpart TPair                                                 
      ///                                                                        
      struct Pair : A::Pair {
         using CTTI_Abstract = No;

      private:
         Many mKey;
         Many mValue;

      public:
         ///                                                                  
         ///   Construction & Assignment                                      
         ///                                                                  
         constexpr Pair() = default;
         Pair(Pair const&) = default;
         Pair(Pair&&) noexcept = default;

         template<class P> requires CT::Pair<Deint<P>>
         Pair(P&&);

         template<class K, class V> requires CT::UnfoldInsertable<K, V>
         Pair(K&&, V&&);

         Pair& operator = (Pair const&) = default;
         Pair& operator = (Pair&&) noexcept = default;
         template<class P> requires CT::Pair<Deint<P>>
         Pair& operator = (P&&);

         ///                                                                  
         ///   Capsulation                                                    
         ///                                                                  
         Hash GetHash() const;

         auto GetKey()        const noexcept -> Many const&;
         auto GetKey()              noexcept -> Many&;
         auto GetKeyBlock()   const noexcept -> Many const&;
         auto GetKeyBlock()         noexcept -> Many&;

         auto GetValue()      const noexcept -> Many const&;
         auto GetValue()            noexcept -> Many&;
         auto GetValueBlock() const noexcept -> Many const&;
         auto GetValueBlock()       noexcept -> Many&;

         ///                                                                  
         ///   Comparison                                                     
         ///                                                                  
         bool operator == (CT::Pair auto const&) const;

         ///                                                                  
         ///   Removal                                                        
         ///                                                                  
         void Clear();
         void Reset();
      };

   } // namespace Langulus::Annies
} // namespace Langulus
