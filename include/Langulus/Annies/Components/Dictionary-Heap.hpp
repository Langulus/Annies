///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Component.hpp"


namespace Langulus::Annies::Component
{
   /// Refers back to this particular component instance through the deduced  
   /// 'this'. Just for convenience. It is #undef-ed at the end of this file. 
   #define ThisCom self.DictionaryHeap<T, ID, SHARED...>

   ///                                                                        
   /// Serialization context as part of the heap                              
   ///   @tparam T the header type                                            
   ///   @tparam ID the heap provider(s) we will be using                     
   template<class T, Cid ID, Cid...SHARED>
   struct DictionaryHeap {
      using CTTI_Component = Yup;
      using CTTI_ReflectAs = void;
      using Id             = Values<ID, SHARED...>;
      using HeapRequest    = T;

      static constexpr bool Shared = sizeof...(SHARED) > 0;
      static constexpr int  ComponentPrecedence = 2000;
      template<Cid SID>
      static constexpr bool Relevant = Id::template Contains<SID>;

      /// Get the start of the hash table                                     
      template<Cid SID = ID, class C> requires Relevant<SID>
      constexpr auto GetDictionary(this C&& self) noexcept -> Tmut<C, T*, T const*>{
         if (self.template GetAllocationInner<SID>())
            return ThisCom::GetDictionaryInner();
         return nullptr;
      }

   protected:
      /// Get the dictionary (inner)                                          
      template<Cid SID = ID> requires Relevant<SID>
      constexpr auto* GetDictionaryInner(this auto&& self) noexcept {
         return self.template AccessHeap<DictionaryHeap, SID>();
      }
                  
      template<Cid SID = ID> requires Relevant<SID>
      constexpr void ResetDictionary(this auto& self) noexcept {
         *ThisCom::GetDictionaryInner() = T{};
      }

      /// This method is called upon allocation to nullify dictionary         
      constexpr void ConstructHeapRequestGlobal(this auto& self) noexcept {
         new (ThisCom::GetDictionaryInner()) T {};
      }
   };

   #undef ThisCom
}
