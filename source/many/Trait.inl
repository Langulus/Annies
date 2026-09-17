///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Trait.hpp"


namespace Langulus::Annies
{

   /// Refer constructor                                                      
   ///   @param other - the trait to refer to                                 
   LANGULUS(INLINED)
   Trait::Trait(const Trait& other)
      : Trait {Refer(other)} {}

   /// Move constructor                                                       
   ///   @param other - the trait to move                                     
   LANGULUS(INLINED)
   Trait::Trait(Trait&& other) noexcept
      : Trait {Move(other)} {}

   /// Unfold constructor, with or without intents                            
   /// If there's only one argument that is deep or a trait, it will be       
   /// absorbed                                                               
   ///   @param t1 - first argument                                           
   ///   @param tn - the rest of the arguments (optional)                     
   template<class T1, class...TN>
   requires CT::UnfoldInsertable<T1, TN...> LANGULUS(INLINED)
   Trait::Trait(T1&& t1, TN&&...tn) {
      if constexpr (sizeof...(TN) == 0 and not CT::Array<T1>) {
         using S = IntentOf<decltype(t1)>;
         using T = TypeOf<S>;

         if constexpr (CT::TraitBased<T>) {
            Base::BlockTransfer(S::Nest(t1).template Forward<Base>());
            mTraitType = DeintCast(t1).GetTrait();
         }
         else if constexpr (CT::Deep<T>)
            Base::BlockTransfer(S::Nest(t1));
         else
            Base::Insert(IndexBack, Forward<T1>(t1));
      }
      else Base::Insert(IndexBack, Forward<T1>(t1), Forward<TN>(tn)...);
   }

   /// Create a trait from a trait and data types                             
   ///   @tparam T - type of trait                                            
   ///   @tparam D - type of data                                             
   ///   @return a trait preconfigured with the provided types                
   template<CT::Trait T, CT::NotVoid D> LANGULUS(INLINED)
   Trait Trait::From() {
      Trait temp {Block<D> {}};
      temp.SetTrait<T>();
      return Abandon(temp);
   }

   /// Create a trait from a trait definition and data                        
   ///   @tparam T - type of trait                                            
   ///   @param stuff - the data to initialize trait with                     
   ///   @return a trait preconfigured with the provided types and contents   
   template<CT::Trait T> LANGULUS(INLINED)
   Trait Trait::From(auto&& stuff) {
      using S = IntentOf<decltype(stuff)>;
      Trait temp {S {stuff}};
      temp.SetTrait<T>();
      return Abandon(temp);
   }

   /// Create a trait from a runtime trait definition and data                
   ///   @param meta - the trait definition                                   
   ///   @param stuff - the data to initialize trait with                     
   ///   @return the trait                                                    
   LANGULUS(INLINED)
   Trait Trait::From(TMeta meta, auto&& stuff) {
      using S = IntentOf<decltype(stuff)>;
      Trait temp {S {stuff}};
      temp.SetTrait(meta);
      return Abandon(temp);
   }

   /// Create a trait from a runtime trait definition and data type           
   LANGULUS(INLINED)
   Trait Trait::FromMeta(TMeta tmeta, DMeta dmeta) {
      Trait temp {Block(DataState::Default, dmeta)};
      temp.SetTrait(tmeta);
      return Abandon(temp);
   }

   /// Set the trait type statically                                          
   ///   @tparam T - the trait                                                
   template<CT::Trait T> LANGULUS(INLINED)
   void Trait::SetTrait() noexcept {
      mTraitType = MetaTraitOf<T>();
   }

   /// Set the trait type dynamically                                         
   ///   @param trait - the trait                                             
   LANGULUS(INLINED)
   void Trait::SetTrait(TMeta trait) noexcept {
      mTraitType = trait;
   }

   /// Get the trait type                                                     
   ///   @return the trait type                                               
   template<CT::TraitBased THIS> LANGULUS(INLINED)
   TMeta Trait::GetTrait() const noexcept {
      if constexpr (CT::Trait<THIS>)
         return (mTraitType = MetaTraitOf<THIS>());
      else
         return mTraitType;
   }

   /// Check if trait is valid, that is, it's typed and has contents          
   ///   @return true if trait is valid                                       
   template<CT::TraitBased THIS> LANGULUS(INLINED)
   bool Trait::IsTraitValid() const noexcept {
      if constexpr (CT::Trait<THIS>)
         return not IsEmpty();
      else
         return mTraitType and not IsEmpty();
   }

   /// Check if trait and data types match another trait                      
   ///   @param other - the trait to test against                             
   ///   @return true if traits are similar                                   
   template<CT::TraitBased THIS> LANGULUS(INLINED)
   bool Trait::IsTraitSimilar(const CT::TraitBased auto& other) const noexcept {
      using OTHER = Deref<decltype(other)>;
      if constexpr (CT::Trait<THIS, OTHER>) {
         return CT::Exact<typename THIS::TraitType, typename OTHER::TraitType>
            and other.CastsToMeta(GetType());
      }
      else {
         return GetTrait<THIS>() == other.GetTrait()
            and other.CastsToMeta(GetType());
      }
   }

   /// Check if a trait matches the given trait type                          
   ///   @tparam T1 - the trait to compare against                            
   ///   @return true trait matches                                           
   template<CT::Trait T1, CT::TraitBased THIS> LANGULUS(INLINED)
   constexpr bool Trait::IsTrait() const {
      if constexpr (CT::Trait<THIS>)
         return CT::SimilarAsOneOf<THIS, T1>;
      else
         return IsTrait(MetaTraitOf<T1>());
   }

   /// Check if a trait matches one of a set of trait types                   
   ///   @param t1 - the first trait to compare against                       
   ///   @param tN - the rest of trait to compare against (optional)          
   ///   @return true if this trait is one of the given types                 
   template<CT::TraitBased THIS, class...TN>
   requires CT::Exact<TMeta, TMeta, TN...> LANGULUS(INLINED)
   bool Trait::IsTrait(TMeta t1, TN...tN) const {
      if constexpr (CT::Trait<THIS>)
         (void) GetTrait<THIS>();

      return mTraitType == t1 or ((mTraitType == tN) or ...);
   }

   /// Check if trait has correct data (always true if trait has no filter)   
   ///   @return true if trait definition filter is compatible                
   template<CT::TraitBased THIS> LANGULUS(INLINED)
   bool Trait::HasCorrectData() const {
      if constexpr (CT::Trait<THIS>)
         (void) GetTrait<THIS>();

      if (not mTraitType)
         return true;
      return CastsToMeta(mTraitType->mDataType);
   }

   /// Pick a constant region and reference it from another container         
   ///   @param start - starting element index                                
   ///   @param count - number of elements                                    
   ///   @return the container                                                
   LANGULUS(INLINED)
   Trait Trait::Select(Offset start, Count count) const IF_UNSAFE(noexcept) {
      return {Base::Select(start, count)};
   }
   
   LANGULUS(INLINED)
   Trait Trait::Select(Offset start, Count count) IF_UNSAFE(noexcept) {
      return {Base::Select(start, count)};
   }

   /// Compare traits with other traits, or by contents                       
   ///   @attention function signature must match Block::operator ==          
   ///      otherwise function resolution doesn't work properly on MSVC       
   ///   @param other - the thing to compare with                             
   ///   @return true if things are the same                                  
   template<CT::TraitBased THIS, CT::NoIntent T> requires CT::NotOwned<T>
   LANGULUS(INLINED) bool Trait::operator == (const T& rhs) const {
      if constexpr (CT::Trait<THIS, T>) {
         return CT::Exact<typename THIS::TraitType, typename T::TraitType>
            and Many::operator == (static_cast<const Many&>(rhs));
      }
      else if constexpr (CT::TraitBased<T>) {
         return IsTrait<THIS>(rhs.GetTrait())
            and Many::operator == (static_cast<const Many&>(rhs));
      }
      else return Many::operator == (rhs);
   }

   /// Refer-assignment                                                       
   ///   @param other - the trait to refer to                                 
   ///   @return a reference to this trait                                    
   LANGULUS(INLINED)
   Trait& Trait::operator = (const Trait& rhs) {
      return operator = (Refer(rhs));
   }

   /// Move-assignment                                                        
   ///   @param other - the trait to move                                     
   ///   @return a reference to this trait                                    
   LANGULUS(INLINED)
   Trait& Trait::operator = (Trait&& rhs) {
      return operator = (Move(rhs));
   }

   /// Unfold assignment, with or without intents                             
   /// If argument is deep or trait, it will be absorbed                      
   ///   @param rhs - right hand side and intent                              
   ///   @return a reference to this trait                                    
   LANGULUS(INLINED)
   Trait& Trait::operator = (CT::UnfoldInsertable auto&& rhs) {
      using S = IntentOf<decltype(rhs)>;
      using T = TypeOf<S>;

      if constexpr (CT::TraitBased<T>) {
         Base::operator = (S::Nest(rhs).template Forward<Base>());
         mTraitType = DeintCast(rhs).GetTrait();
      }
      else if constexpr (CT::Deep<T>)
         Base::operator = (S::Nest(rhs).template Forward<Base>());
      else
         Base::operator = (S::Nest(rhs));
      return *this;
   }

   /// Concatenate with traits/deep types, with or without intents            
   ///   @attention trait type will be set, if not set yet, and rhs is trait  
   ///   @param rhs - the right operand                                       
   ///   @return the combined container                                       
   template<CT::TraitBased THIS> LANGULUS(INLINED)
   THIS Trait::operator + (CT::UnfoldInsertable auto&& rhs) const {
      using S = IntentOf<decltype(rhs)>;
      using T = TypeOf<S>;

      if constexpr (CT::TraitBased<T>) {
         auto result = Many::operator + (S::Nest(rhs).template Forward<Many>());

         if constexpr (CT::Trait<THIS>)
            return THIS {Abandon(result)};
         else {
            return Trait::From(
               GetTrait<THIS>() ? mTraitType : DeintCast(rhs).GetTrait(),
               Abandon(result)
            );
         }
      }
      else {
         auto result = Many::operator + (S::Nest(rhs).Forward());

         if constexpr (CT::Trait<THIS>)
            return THIS {Abandon(result)};
         else
            return Trait::From(GetTrait<THIS>(), Abandon(result));
      }
   }

   /// Destructively concatenate with traits/deep types                       
   ///   @attention trait type will be set, if not set yet and rhs is a trait 
   ///   @param rhs - the right operand and intent                            
   ///   @return a reference to this modified container                       
   template<CT::TraitBased THIS> LANGULUS(INLINED)
   THIS& Trait::operator += (CT::UnfoldInsertable auto&& rhs) {
      using S = IntentOf<decltype(rhs)>;
      using T = TypeOf<S>;

      if constexpr (CT::TraitBased<T>) {
         Many::operator += (S::Nest(rhs).template Forward<Many>());

         if constexpr (not CT::Trait<THIS>) {
            if (not mTraitType)
               mTraitType = DeintCast(rhs).GetTrait();
         }
      }
      else Many::operator += (S::Nest(rhs).Forward());
      return *this;
   }
   
   /// Serialize the trait to anything text-based                             
   template<CT::TraitBased THIS> LANGULUS(INLINED)
   Count Trait::Serialize(CT::Serial auto& to) const {
      const auto initial = to.GetCount();
      using OUT = Deref<decltype(to)>;
      to += GetTrait<THIS>();
      to += OUT::Operator::OpenScope;
      Base::SerializeToText<void>(to);
      to += OUT::Operator::CloseScope;
      return to.GetCount() - initial;
   }

} // namespace Langulus::Annies
