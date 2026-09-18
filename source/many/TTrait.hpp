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

   ///                                                                        
   /// A statically named trait, used for integrating any custom trait by     
   /// using it as a CRTP                                                     
   ///                                                                        
   template<class TRAIT>
   struct TTrait : Trait {
      using CTTI_Tags = TRAIT;
      using CTTI_Bases = Trait;
      using TraitType = TRAIT;

      //template<class T>
      //using Tag = RTTI::Tag<T, TRAIT>;

   public:
      using Trait::Trait;
      TTrait(const TTrait&);
      TTrait(TTrait&&);

      template<class T> requires (CT::Trait<Deint<T>> and not Akin<typename T::TraitType, TRAIT>)
      TTrait(T&&);

      template<CT::NotVoid>
      static TRAIT OfType();
      static TRAIT OfType(DMeta);

      TRAIT& operator = (const TTrait&);
      TRAIT& operator = (TTrait&&);
      TRAIT& operator = (CT::UnfoldInsertable auto&&);

   public:
      template<CT::Trait>
      constexpr bool IsTrait() const;
      constexpr bool IsTrait(TMeta, auto...) const;

      TMeta GetTrait() const noexcept;

      constexpr bool IsTraitValid() const noexcept;
      constexpr bool IsTraitSimilar(const CT::TraitBased auto&) const noexcept;
      constexpr bool HasCorrectData() const;

      ///                                                                     
      ///   Compare                                                           
      ///                                                                     
      template<CT::NoIntent T> requires CT::NotOwned<T>
      bool operator == (const T&) const;

      ///                                                                     
      ///   Concatenation                                                     
      ///                                                                     
      TRAIT  operator +  (CT::UnfoldInsertable auto&&) const;
      TRAIT& operator += (CT::UnfoldInsertable auto&&);

      ///                                                                     
      ///   Conversion                                                        
      ///                                                                     
      size_t Serialize(CT::Serial auto&) const;

   private:
      using Trait::From;
      using Trait::FromMeta;
      using Trait::SetTrait;
   };

} // namespace Langulus::Annies


/// Define a static trait                                                     
///   @param T - the trait, as it appears in namespace Langulus::Traits       
///   @param INFOSTRING - information about the trait's purpose               
#define LANGULUS_DEFINE_TRAIT(T, INFOSTRING) \
   namespace Langulus::Traits \
   { \
      struct T : Annies::TTrait<T> { \
         using CTTI_DefineTag = Yes<#T>; \
         using CTTI_Info = Yes<INFOSTRING>; \
         using TTrait<T>::TTrait; \
         using TTrait<T>::operator =; \
         using TTrait<T>::operator ==; \
         using TTrait<T>::operator +; \
         using TTrait<T>::operator +=; \
         T Select(size_t s, size_t c)       IF_UNSAFE(noexcept) { return {Many::Select(s, c)}; } \
         T Select(size_t s, size_t c) const IF_UNSAFE(noexcept) { return {Many::Select(s, c)}; } \
      }; \
   }


/// Define a static trait with any additional properties                      
///   @param T - the trait, as it appears in namespace Langulus::Traits       
///   @param INFOSTRING - information about the trait's purpose               
///   @param PROPERTIES - any properties tha will be added to T               
#define LANGULUS_DEFINE_TRAIT_WITH_PROPERTIES(T, INFOSTRING, PROPERTIES) \
   namespace Langulus::Traits \
   { \
      struct T : Annies::TTrait<T> { \
         using CTTI_DefineTag = Yes<#T>; \
         using CTTI_Info = Yes<INFOSTRING>; \
         using TTrait<T>::TTrait; \
         using TTrait<T>::operator =; \
         using TTrait<T>::operator ==; \
         using TTrait<T>::operator +; \
         using TTrait<T>::operator +=; \
         T Select(size_t s, size_t c)       IF_UNSAFE(noexcept) { return {Many::Select(s, c)}; } \
         T Select(size_t s, size_t c) const IF_UNSAFE(noexcept) { return {Many::Select(s, c)}; } \
         PROPERTIES; \
      }; \
   }


LANGULUS_DEFINE_TRAIT(Logger,
   "Logger trait, used to access the logger instance");
LANGULUS_DEFINE_TRAIT(Count,
   "Count trait, used to access container size, or other similar properties");
LANGULUS_DEFINE_TRAIT(Name,
   "Name trait, used to access names, or other similar properties");
LANGULUS_DEFINE_TRAIT(Path,
   "Path trait, used to access files and folders, or other file-system related stuff");
LANGULUS_DEFINE_TRAIT(Data,
   "Raw data trait, used to access raw container data, or other similar properties");
LANGULUS_DEFINE_TRAIT(Index,
   "Index trait, used to access the index of elements, or other similar properties");
LANGULUS_DEFINE_TRAIT(Context,
   "Context trait, used to access verb source, the current environment, or other similar properties");
LANGULUS_DEFINE_TRAIT(Trait, 
   "Accesses traits (static or dynamic variables) of an instantiated object of any kind");
LANGULUS_DEFINE_TRAIT(State, 
   "State trait, used to access the state of an object");
LANGULUS_DEFINE_TRAIT(Child,
   "Accesses children in any kind of hierarchy");
LANGULUS_DEFINE_TRAIT(Parent,
   "Accesses parents in any kind of hierarchy");
LANGULUS_DEFINE_TRAIT(Clipboard,
   "Accesses the system clipboard");
LANGULUS_DEFINE_TRAIT(Color,
   "Accesses associated color properties");
LANGULUS_DEFINE_TRAIT(Min,
   "Accesses smallest element in a container, or in other similar contexts");
LANGULUS_DEFINE_TRAIT(Max,
   "Accesses biggest element in a container, or in other similar contexts");
LANGULUS_DEFINE_TRAIT(Input,
   "For accessing verb arguments, or general inputs of some operation");
LANGULUS_DEFINE_TRAIT(Output,
   "For accessing the outputs of a verb, or general output of some operation");
LANGULUS_DEFINE_TRAIT(Mass,
   "Mass of anything with charge, amplitude, or literally physical mass");
LANGULUS_DEFINE_TRAIT(Rate,
   "Rate of anything with charge, or with physical frequency");
LANGULUS_DEFINE_TRAIT(Time,
   "Time of anything with charge, or with a temporal component");
LANGULUS_DEFINE_TRAIT(Priority,
   "Priority of anything with charge, or some kind of priority");
