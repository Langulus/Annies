///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Component.hpp"
#include <Langulus/RTTI/MetaData.hpp>
#include <Langulus/HashOf.hpp>
#include <vector>


namespace Langulus::Annies::Component
{
   ///                                                                        
   /// Doesn't cache hash - recalculates it every time.                       
   ///   @tparam ID the provider ID whose data will be hashed                 
   ///   @tparam H the hash type used                                         
   ///   @tparam SHARED additional provider IDs that are hashed together      
   template<Cid ID, class H, Cid...SHARED>
   struct HashEmergent {
      using CTTI_Component = Yup;
      using CTTI_ReflectAs = void;
      using Id = Values<ID, SHARED...>;

      static constexpr int ComponentPrecedence = 2000;
      template<Cid SID>
      static constexpr bool Relevant = Id::template Contains<SID>;

      /// Get the hash, recompute it every time                               
      template<Cid SID = ID> requires Relevant<SID>
      H GetHash(this auto const& self) {
         return self.template HashRecompute<SID>();
      }

      /// Generate a hash from contiguous data. Allows for batch optimization.
      ///   @attention order matters                                          
      template<Cid SID = ID, CT::Container C> requires (CT::Contiguous<C> and Relevant<SID>)
      H HashRecompute(this C const& self) {
         if (self.template IsEmpty<SID>())
            return {1};

         if constexpr (CT::TypeErased<C>) {
            //                                                          
            // Container is type-erased                                 
            const DMeta T = self.template GetType<SID>();
            LglsAssumeDev((bool) T, "Can't hash untyped container");
            const auto hasher = T.GetHasher();
            LglsAssumeDev(hasher, "Not hashable");
            void* const data = const_cast<void*>(
               static_cast<void const*>(self.template GetRaw<SID>())
            );

            if (self.template GetCount<SID>() == 1) {
               // Exactly one element means exactly one hash            
               return hasher(data);
            }

            // Hashing multiple elements                                
            // Do some batch optimizations wherever possible            
            if (T.IsPOD() and not T.HasGetHashMethod()) {
               // Hash all PODs at once, this includes any pointers     
               // That is unless T::GetHash() method exists             
               return HashBytes(
                  {static_cast<uint8_t*>(data), self.template GetBytesize<SID>()},
                  DefaultHashSeed
               );
            }
            
            if constexpr (CT::ContainsOne<C>) {
               // Return the hash of the single element                 
               // @note this is reached only if GetCount() > 1, so      
               // technically shouldn't ever be reached, but iteration  
               // won't work on single-element containers either way.   
               return hasher(data);
            }
            else {
               // Hash each element, and then combine hashes            
               // @note this is reached only if GetCount() > 1          
               ::std::vector<H> h;
               h.reserve(self.template GetCount<SID>());
               for (auto element : self)
                  h.emplace_back(hasher(element.GetRaw()));

               return HashBytes(
                  { reinterpret_cast<const uint8_t*>(h.data()), h.size() * sizeof(H) },
                  DefaultHashSeed
               );
            }
         }
         else {
            //                                                          
            // Container is not type-erased                             
            using T = TypeOf<C>;
            static_assert(CT::Hashable<T>, "Not hashable");

            if (self.template GetCount<SID>() == 1) {
               // Exactly one element means exactly one hash            
               return HashOf(*self.template GetRaw<SID>());
            }

            // Hashing multiple elements                                
            // Do some batch optimizations wherever possible            
            if constexpr (CT::POD<T> and not CT::HasGetHashMethod<T>) {
               // Hash all PODs at once, this includes any pointers     
               // That is unless T::GetHash() method exists             
               return HashBytes(
                  {reinterpret_cast<const uint8_t*>(self.template GetRaw<SID>()), self.template GetBytesize<SID>()},
                  DefaultHashSeed
               );
            }
            else if constexpr (CT::ContainsOne<C>) {
               // Return the hash of the single element                 
               // @note this is reached only if GetCount() > 1, so      
               // technically shouldn't ever be reached, but iteration  
               // won't work on single-element containers either way.   
               return HashOf(*self.template GetRaw<SID>());
            }
            else {
               // Hash each element, and then combine hashes            
               // @note this is reached only if GetCount() > 1          
               ::std::vector<H> h;
               h.reserve(self.template GetCount<SID>());
               for (T const& element : self)
                  h.emplace_back(HashOf(element));
               
               return HashBytes(
                  {reinterpret_cast<const uint8_t*>(h.data()), h.size() * sizeof(H)},
                  DefaultHashSeed
               );
            }
         }
      }

      /// Generate a hash from discontiguous data. Basically disables batch   
      /// optimizations.                                                      
      ///   @attention order matters                                          
      template<Cid SID = ID, CT::Container C> requires (CT::NotContiguous<C> and Relevant<SID>)
      H HashRecompute(this C const& self) {
         if (self.template IsEmpty<SID>())
            return {1};

         // Do some assumption checking                                 
         if constexpr (CT::TypeErased<C>) {
            const DMeta T = self.template GetType<SID>();
            LglsAssumeDev((bool) T, "Can't hash untyped container");
            LglsAssumeDev(T.GetHasher(), "Not hashable");
         }
         else {
            using T = TypeOf<C>;
            static_assert(CT::Hashable<T>, "Not hashable");
         }

         // Hash all elements                                           
         ::std::vector<H> h;
         h.reserve(self.template GetCount<SID>());
         self.Apply([&h](auto const& item) {
            h.emplace_back(HashOf(item));
         });

         if (h.size() == 1) {
            // Single element always results in single hash             
            return h[0];
         }
         else {
            // Hash the array of hashes to get the final hash           
            return HashBytes(
               {reinterpret_cast<const uint8_t*>(h.data()), h.size() * sizeof(H)},
               DefaultHashSeed
            );
         }
      }
      
   protected:
      LglsComHeapMovable(friend);

      /// This always returns an invalid hash to enforce regeneration         
      template<Cid SID = ID> requires Relevant<SID>
      constexpr H GetHashInner() const noexcept {
         return 0;
      }
   };
}
