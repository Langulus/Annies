///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include <Langulus/Many.hpp>
#include <Langulus/Annies/Components/Stack.hpp>
#include <Langulus/Annies/Components/Charged-Stack.hpp>


namespace Langulus::Annies::Inner
{
   /// Recipe extends the usual type-erased Many, by adding charge and        
   /// type as members.                                                       
   using RecipeBase = typename ManyBase::template Include<
      Com::Stack<DMeta, 1>,         // Add the constructed type         
      Com::ChargedStack<>           // Add charge                       
   >;
}

namespace Langulus::Annies
{
   ///                                                                        
   ///   Recipe                                                               
   ///                                                                        
   ///   Used to contain creation instructions for any type. It is just a     
   /// type-erased Many - a descriptor - with a charge and a type.            
   /// Optimized for detecting small differences in descriptions.             
   /// These scripts mean the completely same thing:                          
   /// 1) create*2(input(name("test")) output(Thing))                         
   /// 2) create*2 Thing(name("test"))      // memoized as Recipe             
   /// 3) create Thing*2(name("test"))      // memoized as Recipe             
   /// 4) create Thing*2("test")            // memoized as Recipe             
   /// 5) create Recipe*2(type(Thing), input(name("test")))                   
   /// 6) create*2 Recipe(Thing, name("test"))                                
   /// After execution all these are replaced by the two created Things       
   struct Recipe : Inner::RecipeBase {
      using Base     = Inner::RecipeBase;
      using DeepType = Many;

      constexpr Recipe() noexcept {
         this->ConstructDefault();
      }
      constexpr Recipe(Recipe const& other) {
         this->Absorb(Refer(other));
      }
      constexpr Recipe(Recipe&& other) noexcept  {
         this->Absorb(Move(other));
      }
      constexpr ~Recipe() noexcept {
         this->Destroy();
      }

      /// Create an empty recipe for a particular target type                 
      ///   @param type what is the recipe for?                               
      ///   @param arguments... arguments for the descriptor                  
      ///   @return the new recipe instance                                   
      static Recipe Of(DMeta type, auto&&...arguments) {
         Recipe result {LglsFwd(arguments)...};
         result.SetTarget(type);
         return Abandon(result);
      }
   
      static Recipe Of(CT::Container auto const& typed, auto&&...arguments) {
         Recipe result {LglsFwd(arguments)...};
         result.SetTarget(typed.GetType());
         return Abandon(result);
      }

      /// Copy target and charge from another recipe                          
      static Recipe From(Recipe const& target_and_charge, auto&&...arguments) {
         Recipe result {LglsFwd(arguments)...};
         result.SetTarget(target_and_charge.GetTarget());
         result.SetCharge(target_and_charge.GetCharge());
         return Abandon(result);
      }

      /// Set what this recipe is for                                         
      void SetTarget(DMeta type) noexcept {
         *Com::Stack<DMeta, 1>::Get() = type;
      }

      /// What is this a recipe for?                                          
      DMeta GetTarget() const noexcept {
         return *Com::Stack<DMeta, 1>::Get();
      }
   
      /// Get the descriptor for the recipe                                   
      Many GetDescriptor() const noexcept {
         return Many {Inner::Absorb{}, *this};
      }
   
      /// Construction that either absorbs the provided containers, or        
      /// emplaces all A in the container                                     
      template<NotTag A1, class...AN>
      constexpr Recipe(A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0) {
            if constexpr (Same<Deint<A1>, Recipe>) {
               LglsAssumeUser(false,
                  "Ambiguous use of construction "
                  "- you should use tag-dispatch with first argument either Absorb "
                  "(if you want to overwrite the container itself) or Piecewise "
                  "(if you want to overwrite the first item) in order to clearly "
                  "state your intent. Absorb will be used by default!"
               );
               this->Absorb(LglsFwd(a1));
            }
            else this->EmplaceConstruct(LglsFwd(a1));
         }
         else {
            this->ConstructDefault();
            this->Insert(LglsFwd(a1), LglsFwd(an)...);
         }
      }
      
      /// Construction that absorbs the provided containers                   
      template<class A1, class...AN>
      constexpr Recipe(Inner::Absorb, A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0)
            this->Absorb(LglsFwd(a1));
         else {
            this->ConstructDefault();
            this->Concat(LglsFwd(a1), LglsFwd(an)...);
         }
      }
      
      /// Construction that emplaces all arguments inside                     
      template<class A1, class...AN>
      constexpr Recipe(Inner::Piecewise, A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0)
            this->EmplaceConstruct(LglsFwd(a1));
         else {
            this->ConstructDefault();
            this->Insert(LglsFwd(a1), LglsFwd(an)...);
         }
      }
      
      /// Assignment                                                          
      constexpr Recipe& operator = (Many const& other) {
         return this->AssignAbsorb(Refer(other));
      }
      constexpr Recipe& operator = (Many&& other) noexcept {
         return this->AssignAbsorb(Move(other));
      }
      
      template<class A>
      constexpr Recipe& operator = (A&& argument) {
         if constexpr (Same<Deint<A>, Recipe>) {
            LglsAssumeUser(false,
               "Ambiguous use of assignment "
               "- you should use either AssignAbsorb (if you want to overwrite "
               "the container itself) or Assign (if you want to overwrite the "
               "first item) in order to clearly state your intent. "
               "AssignAbsorb will be used by default!"
            );
            return this->AssignAbsorb(LglsFwd(argument));
         }
         else return this->Assign(LglsFwd(argument));
      }

      using Com::Comparison<>::operator <=>;
      using Com::Comparison<>::operator ==;
   };
}

namespace Langulus
{
   using Annies::Recipe;
}