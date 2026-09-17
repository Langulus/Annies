///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Block.hpp"
#include "../../many/Trait.hpp"
#include "../../many/Construct.hpp"
#include "../../many/Neat.hpp"
#include <Langulus/CT/Vector.hpp>


namespace Langulus::Annies
{

   /// Set a default trait, if such wasn't already set                        
   ///   @tparam TRAIT - trait to set                                         
   ///   @param value - the value to assign                                   
   template<class TYPE> template<class TRAIT, CT::Data D>
   void Block<TYPE>::SetDefaultTrait(D&& value) {
      static_assert(CT::Trait<TRAIT>, "TRAIT is not a trait type");
      bool satisfied = false;
      ForEachDeep([&](const TRAIT& trait) {
         if (trait) {
            satisfied = true;
            return Loop::Break;
         }
         else return Loop::Continue;
      });

      if (satisfied)
         return;

      // Trait wasn't found - search for a Neat to create it in         
      ForEachDeep([&](Neat& neat) {
         neat.AddTrait(Abandon(TRAIT {Forward<D>(value)}));
         satisfied = true;
      });

      if (satisfied)
         return;

      // No Neat was found, so just push one containing the trait       
      SmartPush(IndexBack, Neat {TRAIT {Forward<D>(value)}});
   }

   /// Extract a trait from the descriptor                                    
   ///   @tparam TRAIT... - trait(s) we're searching for                      
   ///   @param values - [out] where to save the value, if found              
   ///   @return true if value changed                                        
   template<class TYPE> template<class...TRAIT>
   bool Block<TYPE>::ExtractTrait(CT::Data auto&...values) const {
      static_assert(CT::Trait<TRAIT...>, "TRAIT is not a trait type");
      return (ExtractTraitInner<TRAIT>(values...) or ...);
   }

   /// Extract data of an exact type, doing only pointer arithmetic           
   ///   @param value - [out] where to save the value(s), if found            
   ///   @return the number of extracted values (always 1 if not an array)    
   template<class TYPE>
   auto Block<TYPE>::ExtractData(CT::Data auto& value) const -> Count {
      using D = Deref<decltype(value)>;
      Count progress = 0;

      if constexpr (CT::Vector<D> or CT::Array<D>) {
         ForEachDeep([&](const TypeOf<D>& data) {
            //TODO can be optimized-out for POD
            value[progress] = data;
            ++progress;
            return (progress >= CountOf<D>) ? Loop::Break : Loop::Continue;
         });
      }
      else {
         ForEachDeep([&](const D& data) {
            value = data;
            ++progress;
            return Loop::Break;
         });
      }

      return progress;
   }

   /// Extract any data that is convertible to D                              
   ///   @param value - [out] where to save the value, if found               
   ///   @return the number of extracted values (always 1 if not an array)    
   template<class TYPE>
   auto Block<TYPE>::ExtractDataAs(CT::Data auto& value) const -> Count {
      using D = Deref<decltype(value)>;
      Count progress = 0;

      ForEachDeep([&](const Many& group) {
         if constexpr (CT::Vector<D> or CT::Array<D>) {
            const auto toscan = ::std::min(CountOf<D> - progress, group.GetCount());
            for (Offset i = 0; i < toscan; ++i) {
               //TODO can be optimized-out for POD
               try {
                  value[progress] = group.template AsCast<TypeOf<D>>(i);
                  ++progress;
               }
               catch (...) {}
            }

            return (progress >= CountOf<D>) ? Loop::Break : Loop::Continue;
         }
         else {
            try {
               value = group.template AsCast<D>();
               ++progress;
               return Loop::Break;
            }
            catch (...) {}
            return Loop::Continue;
         }
      });

      return progress;
   }

   /// Find data in constructs or tail, that casts to T                       
   ///   @tparam T - type requirement                                         
   ///   @return the first type that matches                                  
   template<class TYPE> template<CT::Data T>
   auto Block<TYPE>::FindType() const -> DMeta {
      return FindType(MetaDataOf<T>());
   }

   /// Find data in constructs or tail, that casts to a type                  
   ///   @param type - type requirement                                       
   ///   @return the first type that matches                                  
   template<class TYPE>
   auto Block<TYPE>::FindType(DMeta type) const -> DMeta {
      bool  ambiguous = false;
      DMeta found;

      ForEachDeep([&](const Many& group) noexcept {
         group.ForEach([&](const Construct& c) noexcept {
            if (not c.CastsTo(type))
               return;

            if (not found) found = c.GetType();
            else ambiguous = true;
         });

         if (not group.CastsToMeta(type))
            return;

         if (not found) found = group.GetType();
         else ambiguous = true;
      });

      if (ambiguous) {
         Logger::Warning(
            "Multiple types found in block - all except the first `",
            found, "` will be ignored on FindType"
         );
      }

      return found;
   }

   ///                                                                        
   template<class TYPE> template<class TRAIT>
   bool Block<TYPE>::ExtractTraitInner(CT::Data auto&...values) const {
      static_assert(CT::Trait<TRAIT>, "TRAIT is not a trait type");
      return ExtractTraitInner<TRAIT>(
         Sequence<sizeof...(values)>::Expand, values...
      );
   }

   ///                                                                        
   template<class TYPE> template<class TRAIT, Offset...IDX>
   bool Block<TYPE>::ExtractTraitInner(
      ExpandedSequence<IDX...>, CT::Data auto&...values
   ) const {
      static_assert(CT::Trait<TRAIT>, "TRAIT is not a trait type");
      return (ExtractTraitInnerInner<TRAIT, IDX>(values) or ...);
   }

   ///                                                                        
   template<class TYPE> template<class TRAIT, Offset IDX>
   bool Block<TYPE>::ExtractTraitInnerInner(CT::Data auto& value) const {
      static_assert(CT::Trait<TRAIT>, "TRAIT is not a trait type");
      using D = Deref<decltype(value)>;
      bool satisfied = false;
      Count counter = 0;

      ForEachDeep([&](const TRAIT& trait) {
         if (counter < IDX) {
            // We're only interested in the Nth trait                   
            ++counter;
            return Loop::Continue;
         }

         if constexpr (CT::Deep<D>) {
            value = static_cast<const Many&>(trait);
            satisfied = true;
         }
         else if (trait.ExtractDataAs(value))
            satisfied = true;

         return Loop::Break;
      });

      return satisfied;
   }
   
   /// Set a tagged argument inside descriptor                                
   ///   @param trait - trait to set                                          
   ///   @param index - the index we're interested with if repeated           
   template<class TYPE> template<class TRAIT>
   void Block<TYPE>::SetTrait(TRAIT&& trait, Offset index) {
      using S = IntentOf<TRAIT>;
      using T = TypeOf<S>;
      static_assert(CT::TraitBased<T>, "T is not trait-based");

      // First attempt setting an already existing trait at given index 
      bool satisfied = false;
      Offset counter = 0;
      ForEachDeep([&](T& found) {
         if (counter == index) {
            found = Forward<TRAIT>(trait);
            satisfied = true;
            return Loop::Break;
         }

         ++counter;
         return Loop::Continue;
      });

      if (satisfied)
         return;

      // Then try pushing a new trait into a contained Neat scope       
      ForEachDeep([&](Neat& neat) {
         neat.SetTrait(Forward<TRAIT>(trait));
         satisfied = true;
         return Loop::Break;
      });

      if (satisfied)
         return;

      // Finally, just push a Neat scope containing the trait           
      SmartPush(IndexBack, Neat {Forward<TRAIT>(trait)});
   }
}