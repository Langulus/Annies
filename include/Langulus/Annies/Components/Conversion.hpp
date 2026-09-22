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
   ///                                                                        
   /// Implements conversion/serialization for containers                     
   ///   @param ID provider whose data will get converted                     
   ///   @param SHARED other providers that will be converted along           
   template<Cid ID, Cid...SHARED>
   struct Conversion {
      using CTTI_Component = Yup;
      using CTTI_ReflectAs = void;
      using Id = Values<ID, SHARED...>;

      static constexpr int ComponentPrecedence = 3000;
      template<Cid SID>
      static constexpr bool Relevant = Id::template Contains<SID>;

   private:
      //template<CT::Container C>
      //using Count = typename Deref<C>::CountType;

   public:
      //TODO add dimensionality

      /// MARK: Public                                                        
      /// Convert block's first element into another block of size 1.         
      ///   @param out What are we converting to?                             
      ///   @return 1 on success                                              
      template<CT::Container C, CT::ContainsOne OUT>
      auto ConvertTo(this C const& self, OUT& out) -> size_t {
         if (self.IsEmpty())
            return 0;

         // If OUT contains a single item, we can avoid inserting       
         // and concatenating, and just assigning every time.           
         IF_NOT_LANGULUS_FORCE_TYPE_ERASURE(if constexpr (CT::TypeErased<C> or CT::TypeErased<OUT>) {)
            const auto TO = out.GetType();
            const auto FROM = self.GetType();
            if (FROM.IsSame(TO)) {
               out.AssignAbsorb(self);
               return 1;
            }

            // Search for a reflected conversion routine                
            LglsAssert((bool) TO, "Can't convert to unknown type");
            const auto converter = FROM.GetMorphism(TO);
            if (not converter.convert)
               return 0;         // Not convertible                     

            if (out.IsEmpty()) {
               out.PrepareForReconstruction();
               if_available(out.SetCountInner(1));
               if_available(out.SetHashInner(0));
            }
            else {
               out.PrepareForReassignment();
               if_available(out.SetHashInner(0));
            }

            try {
               converter.convert(self.GetRawVoid(), out.GetRawVoid());
            }
            catch (...) {
               out.ResetCount();
               throw;
            }

            return 1;
         #if not LANGULUS(FORCE_TYPE_ERASURE)
         } else {
            using TO = TypeOf<OUT>;
            using FROM = TypeOf<C>;

            if constexpr (Same<FROM, TO>) {
               out.AssignAbsorb(self);
               return 1;
            }
            else {
               static_assert(CT::Convertible<FROM, TO> /*CT::Inner::FindMorphism<FROM, TO>() >= 0*/, "Not convertible");
               out.Assign(Langulus::Convert<TO>(*self));
               return 1;
            }
         }
         #endif
      }

      /// Convert block's contents to another block of contents, by iterating 
      /// all elements, and converting them one by one. Each contained item   
      /// will be converted to a corresponding item and appended to 'out'.    
      ///   @param out what are we converting to?                             
      ///   @return the number of converted elements inserted in 'out'.       
      ///      this will be smaller than self.GetCount() on partial success   
      template<CT::Container C, CT::ContainsMany OUT>
      auto ConvertTo(this C const& self, OUT& out) -> size_t {
         if (self.IsEmpty())
            return 0;

         // OUT can contain many items, so we always concatenate        
         // convertions to the back, preserving contents.               
         IF_NOT_LANGULUS_FORCE_TYPE_ERASURE(if constexpr (CT::TypeErased<C> or CT::TypeErased<OUT>) {)
            //                                                          
            // One of the containers is type-erased                     
            const auto TO   = out.GetType();
            const auto FROM = self.GetType();
            if (FROM.IsSame(TO))
               return out.Concat(self);
            
            // Search for a reflected conversion routine                
            LglsAssert((bool) TO, "Can't convert to unknown type");
            const auto converter = FROM.GetMorphism(TO);
            if (not converter.convert)
               return 0;         // Not convertible                     

            out.AllocateMore(out.GetCount() + self.GetCount());

            const auto dstBeg = out.GetHandle() + out.GetCount();
            auto dst = dstBeg;
            try {
               self.Apply([&dst,&converter](auto const& src) {
                  converter.convert(src.GetRawVoid(), dst.GetRawVoid());
                  ++dst;
               });
            }
            catch (...) {
               // Partial success                                       
               auto n = dst - dstBeg;
               out.PartialSuccess(out.GetCount() + n);
               throw;
            }
         #if not LANGULUS(FORCE_TYPE_ERASURE)
         } else {
            //                                                          
            // Both containers are statically-typed, so leverage it     
            // to generate a well inlined routine for conversion        
            using TO   = TypeOf<OUT>;
            using FROM = TypeOf<C>;
            static_assert(CT::Convertible<FROM, TO> /*CT::Inner::FindMorphism<FROM, TO>() >= 0*/, "Not convertible");

            if constexpr (Same<FROM, TO>)
               return out.Concat(self);
            else {
               // Types are statically convertible                      
               out.AllocateMore(out.GetCount() + self.GetCount());

               auto from = self.GetRaw();
               const auto fromEnd = from + self.GetCount();
               auto to = out.GetRaw() + out.GetCount();
               try {
                  while (from != fromEnd) {
                     new (to) TO {Langulus::Convert<TO>(*from)};
                     ++to; ++from;
                  }
               }
               catch (...) {
                  // Partial success                                    
                  auto n = from - self.GetRaw();
                  out.PartialSuccess(out.GetCount() + n);
                  throw;
               }
            }
         }
         #endif

         out.SetCountInner(out.GetCount() + self.GetCount());
         out.SetHashInner(0);
         return self.GetCount();
      }

      template<CT::NotVoid TO, CT::Container C, class OUT = typename C::template Retype<TO>>
      auto ConvertTo(this C const& self) -> OUT {
         OUT result;
         self.ConvertTo(result);
         return Abandon(result);
      }
   };
}
