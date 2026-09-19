///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../BlockSet.hpp"


namespace Langulus::Annies
{
   
   /// Set the data ID - use this only if you really know what you're doing   
   ///   @tparam CONSTRAIN - whether or not to enable type-constraint         
   ///   @param type - the type meta to set                                   
   template<bool CONSTRAIN, CT::Set THIS>
   void BlockSet::SetType(DMeta type) {
      static_assert(not CT::Typed<THIS>,
         "Can't change type of a statically typed set");
      mKeys.SetType<CONSTRAIN>(type);
   }
   
   /// Set the contained data type                                            
   ///   @tparam T - the contained type                                       
   ///   @tparam CONSTRAIN - whether or not to enable type-constraints        
   template<CT::NotVoid T, bool CONSTRAIN, CT::Set THIS> LANGULUS(INLINED)
   void BlockSet::SetType() {
      static_assert(not CT::Typed<THIS>,
         "Can't change type of a statically typed set");
      mKeys.SetType<T, CONSTRAIN>();
   }

   /// Mutate set to a different type, if possible                            
   ///   @tparam T - the type to change to                                    
   ///   @tparam FORCE - insert even if types mismatch, by making this set    
   ///                   deep with provided type - use void to disable        
   ///   @return true if block was deepened to incorporate the new type       
   template<CT::Set THIS, CT::NotVoid T, class FORCE> LANGULUS(INLINED)
   bool BlockSet::Mutate() {
      if constexpr (CT::Typed<THIS>) {
         if constexpr (CT::Similar<TypeOf<THIS>, T>) {
            // No need to mutate - types are compatible                 
            return false;
         }
         else if constexpr (not CT::Void<FORCE> and IsDeep<THIS>()) {
            // Container is already deep, just make it deeper           
            //Deepen<FORCE, true, THIS>();
            TODO(); //TODO deepen sets
            return true;
         }
         else LANGULUS_OOPS(Mutate, "Can't mutate to incompatible type");
      }
      else return Mutate<THIS, FORCE>(MetaDataOf<T>());
   }
   
   /// Mutate to another compatible type, deepening the container if allowed  
   ///   @tparam FORCE - insert even if types mismatch, by making this block  
   ///                   deep with provided type - use void to disable        
   ///   @param meta - the type to mutate into                                
   ///   @return true if set was deepened to incorporate the new type         
   template<CT::Set THIS, class FORCE>
   bool BlockSet::Mutate(DMeta meta) {
      static_assert(not CT::Typed<THIS>,
         "Can't set type of a statically typed set");

      if (IsUntyped<THIS>() or (not mKeys.mState.IsTyped() and mKeys.mType->mIsAbstract
                                and IsEmpty() and meta->CastsTo(mKeys.mType))
      ) {
         // Undefined/abstract containers can mutate freely             
         SetType<false, THIS>(meta);
      }
      else if (mKeys.mType->IsSimilar(meta)) {
         // No need to mutate - types are compatible                    
         return false;
      }
      else if (not IsInsertable<THIS>(meta)) {
         // Not insertable because of reasons                           
         if constexpr (not CT::Void<FORCE>) {
            if (not IsTypeConstrained<THIS>()) {
               // Container is not type-constrained, so we can safely   
               // deepen it, to incorporate the new data                
               //Deepen<FORCE, true, THIS>();
               TODO(); //TODO deepen sets
               return true;
            }

            LANGULUS_OOPS(Mutate, "Attempting to mutate incompatible "
               "type-constrained set");
         }
         else LANGULUS_OOPS(Mutate, "Can't mutate to incompatible type");
      }

      // Block may have mutated, but it didn't happen                   
      return false;
   }

   /// Check if this value type is similar to one of the listed types,        
   /// ignoring density and cv-qualifiers                                     
   ///   @tparam T1, TN... - the types to compare against                     
   ///   @return true if value type is similar to at least one of the types   
   template<CT::Set THIS, CT::NotVoid T1, CT::NotVoid...TN> LANGULUS(INLINED)
   constexpr bool BlockSet::Is() const noexcept {
      return GetValues<THIS>().template Is<T1, TN...>();
   }
   
   /// Check if value type loosely matches a given type, ignoring             
   /// density and cv-qualifiers                                              
   ///   @param meta - the type to check for                                  
   ///   @return true if this set contains similar value data                 
   template<CT::Set THIS> LANGULUS(INLINED)
   bool BlockSet::Is(DMeta meta) const noexcept {
      return GetValues<THIS>().Is(meta);
   }

   /// Check if value type is similar to one of the listed types,             
   /// ignoring cv-qualifiers only                                            
   ///   @tparam T1, TN... - the types to compare against                     
   ///   @return true if value type is similar to at least one of the types   
   template<CT::Set THIS, CT::NotVoid T1, CT::NotVoid...TN> LANGULUS(INLINED)
   constexpr bool BlockSet::IsSimilar() const noexcept {
      return GetValues<THIS>().template IsSimilar<T1, TN...>();
   }
   
   /// Check if value type loosely matches a given type, ignoring             
   /// cv-qualifiers only                                                     
   ///   @param meta - the type to check for                                  
   ///   @return true if this set contains similar value data                 
   template<CT::Set THIS> LANGULUS(INLINED)
   bool BlockSet::IsSimilar(DMeta meta) const noexcept {
      return GetValues<THIS>().IsSimilar(meta);
   }

   /// Check if value type is exactly as one of the listed types,             
   /// including by density and cv-qualifiers                                 
   ///   @tparam T1, TN... - the types to compare against                     
   ///   @return true if value type exactly matches at least one type         
   template<CT::Set THIS, CT::NotVoid T1, CT::NotVoid...TN> LANGULUS(INLINED)
   constexpr bool BlockSet::IsExact() const noexcept {
      return GetValues<THIS>().template IsExact<T1, TN...>();
   }
   
   /// Check if value type is exactly the provided type,                      
   /// including the density and cv-qualifiers                                
   ///   @param type - the type to match                                      
   ///   @return true if this set contains this exact value data              
   template<CT::Set THIS> LANGULUS(INLINED)
   bool BlockSet::IsExact(DMeta meta) const noexcept {
      return GetValues<THIS>().IsExact(meta);
   }
   
   /// Check if types of two sets are compatible for writing                  
   ///   @param other - set to test with                                      
   ///   @return true if both sets are type-compatible                        
   template<CT::Set THIS> LANGULUS(INLINED)
   constexpr bool BlockSet::IsTypeCompatibleWith(CT::Set auto const& other) const noexcept {
      using RHS = Deref<decltype(other)>;
      if constexpr (CT::Typed<THIS, RHS>) {
         // Static type check                                           
         return CT::Similar<TypeOf<THIS>, TypeOf<RHS>>;
      }
      else {
         // Dynamic type check                                          
         return GetValues<THIS>().IsSimilar(other.GetType());
      }
   }

} // namespace Langulus::Annies
