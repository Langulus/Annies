///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Langulus/CT/Akin.hpp"
#include "Langulus/CT/Executable.hpp"
#include "Verb.hpp"


namespace Langulus::Annies::Inner
{
   /// TVerbs extend the usual type-erased Any, by adding charge and verb ID  
   /// as members.                                                            
   template<CT::DefineVerb V>
   using TVerbBase = typename ManyBase::template Include<
      Com::VerbedStack<VMeta, V>,  // Add verb, make executable         
      Com::ChargedStack<>          // Add charge                        
   >;
}

namespace Langulus::Annies
{
   ///                                                                        
   /// MARK: TVerb                                                            
   ///   A type-erased container specifically designed for fully capsulating  
   /// function calls. This one is verb-constrained: the verb it contains     
   /// is known at compile-time.                                              
   ///   This template is used as base for all verb definitions, so that you  
   /// can directly use specific verbs as containers, like so:                
   /// Verbs::Write{Verbs::Catenate{1,2,3}}.In(file).AndThen(Verbs::Halt{});  
   ///   Cool, huh?                                                           
   template<CT::DefineVerb V>
   struct TVerb : Inner::TVerbBase<V> {
      using CTTI_ReflectAs = Verb;
      
   private:
      // The number of successful executions                            
      size_t successes = 0;

   public:
      // Verb context                                                   
      Many context;
      // The container where output goes after execution                
      Many output;

      using CTTI_Members = Members<&TVerb::context, &TVerb::output>;

      constexpr TVerb() noexcept {
         this->ConstructDefault();
      }
      constexpr TVerb(TVerb const& other) {
         this->Absorb(Refer(other));
      }
      constexpr TVerb(TVerb&& other) noexcept  {
         this->Absorb(Move(other));
      }
      constexpr ~TVerb() noexcept {
         this->Destroy();
      }

      /// Construction that either absorbs the provided containers, or        
      /// emplaces all A in the container                                     
      template<Disambiguate A1, class...AN>
      constexpr TVerb(A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0) {
            if constexpr (CT::DeepDense<Deint<A1>> or CT::Executable<Deint<A1>>) {
               LglsAssumeUser(CT::Executable<Deint<A1>>,
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
      constexpr TVerb(Inner::Absorb, A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0)
            this->Absorb(LglsFwd(a1));
         else {
            this->ConstructDefault();
            this->Concat(LglsFwd(a1), LglsFwd(an)...);
         }
      }
      
      /// Construction that emplaces all arguments inside                     
      template<class A1, class...AN>
      constexpr TVerb(Inner::Piecewise, A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0)
            this->EmplaceConstruct(LglsFwd(a1));
         else {
            this->ConstructDefault();
            this->Insert(LglsFwd(a1), LglsFwd(an)...);
         }
      }
      
      /// Assignment                                                          
      constexpr TVerb& operator = (TVerb const& other) {
         return this->AssignAbsorb(Refer(other));
      }
      constexpr TVerb& operator = (TVerb&& other) noexcept {
         return this->AssignAbsorb(Move(other));
      }
      
      template<class A>
      constexpr TVerb& operator = (A&& argument) {
         if constexpr (CT::DeepDense<Deint<A>> or CT::Executable<Deint<A>>) {
            LglsAssumeUser(CT::Executable<Deint<A>>,
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

      /// MARK: Access                                                        
      auto GetHash() const -> Hash;
      auto GetOperatorToken(bool& tokenized) const -> Text;

      auto GetSource()       noexcept -> Many&;
      auto GetSource() const noexcept -> Many const&;

      auto GetArgument()       noexcept -> Many&;
      auto GetArgument() const noexcept -> Many const&;

      auto GetOutput()       noexcept -> Many&;
      auto GetOutput() const noexcept -> Many const&;

      auto operator -> ()       noexcept -> Many*;
      auto operator -> () const noexcept -> Many const*;
      
      auto GetSuccesses() const noexcept -> size_t;
      bool IsDone() const noexcept;

      bool IsMissing() const noexcept;
      bool IsMissingDeep() const noexcept;
      bool Validate(CT::Index auto&&) const noexcept;

      void Done(size_t) noexcept;
      void Done() noexcept;
      void Undo() noexcept;

      /// MARK: Compare                                                       
      bool operator ==  (const TVerb&) const;
      auto operator <=> (const TVerb&) const -> std::partial_ordering;

      /// MARK: Removal                                                       
      void Reset();
   };
}

namespace Langulus
{
   using Annies::TVerb;
}

//LANGULUS_MORPHISM(Langulus::Annies::TVerb, Langulus::Annies::Text, Langulus::Flow::Code);

/// Define a verb                                                             
///   @param P - positive verb name, as it exists in namespace Langulus::Verbs
///   @param N - negative verb name (optional, same as positive if "")        
///   @param INFOSTRING - information about the verb's purpose                
#define LANGULUS_DEFINE_VERB(P, N, PRECEDENCE, INFOSTRING) \
   namespace Langulus::Verbs { struct P; } \
   namespace Langulus::CTTI  { template<> struct DefineVerb<::Langulus::Verbs::P> : NamedVerb<#P,#N,PRECEDENCE> {}; } \
   namespace Langulus::Verbs { struct P : Annies::TVerb<P> { using CTTI_Info = Yes<INFOSTRING>; }; }

/// Define a verb with operators                                              
///   @param P - positive verb name, as it exists in namespace Langulus::Verbs
///   @param N - negative verb name (optional, same as positive if "")        
///   @param OP - positive verb operator                                      
///   @param ON - negative verb operator                                      
///   @param PRECEDENCE - operator precedence                                 
///   @param INFOSTRING - information about the verb's purpose                
#define LANGULUS_DEFINE_OPERATOR(P, N, OP, ON, PRECEDENCE, INFOSTRING) \
   namespace Langulus::Verbs { struct P; } \
   namespace Langulus::CTTI  { template<> struct DefineVerb<::Langulus::Verbs::P> : NamedVerb<#P,#N,PRECEDENCE> {}; } \
   namespace Langulus::CTTI  { template<> struct DefineVerbOp<::Langulus::Verbs::P> : NamedOperator<OP,ON> {}; } \
   namespace Langulus::Verbs { struct P : Annies::TVerb<P> { using CTTI_Info = Yes<INFOSTRING>; }; }
