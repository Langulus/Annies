///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Langulus/CT/Convertible.hpp"
#include "Many.hpp"
#include "Annies/Components/Charged-Stack.hpp"
#include "Annies/Components/Verbed-Stack.hpp"
#include "Langulus/CT/Able.hpp"
#include "Langulus/CT/Akin.hpp"
#include "Langulus/CT/Executable.hpp"


namespace Langulus::Annies::Inner
{
   /// TVerbs extend the usual type-erased Any, by adding charge and verb ID  
   /// as members.                                                            
   template<class V>
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
   template<class V>
   struct TVerb : Inner::TVerbBase<V> {
      using CTTI_ReflectAs = Verb;
      
   private:
      // The number of successful executions                            
      mutable size_t mSuccesses = 0;
      // The container where output goes after execution                
      mutable Many mOutput;
      // Verb context                                                   
      Many mContext;

   public:
      using CTTI_Members = Members<&TVerb::mContext, &TVerb::mOutput>;

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
            if constexpr (CT::DeepDense<Deint<A1>> and not CT::Executable<Deint<A1>>) {
               /*LglsAssumeUser(CT::Executable<Deint<A1>>,
                  "Ambiguous use of construction "
                  "- you should use tag-dispatch with first argument either Absorb "
                  "(if you want to overwrite the container itself) or Piecewise "
                  "(if you want to overwrite the first item) in order to clearly "
                  "state your intent. Absorb will be used by default!"
               );*/
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
      
      /// Create a tag by manually specifying the tag ID                      
      static TVerb From(TVerb verb, auto&&...arguments) {
         TVerb result {LglsFwd(arguments)...};
         result.SetVerb(verb);
         return result;
      }

      /// Create a tag by extracting tag ID and charge from another container 
      static TVerb From(CT::Executable auto const& source, auto&&...arguments) {
         TVerb result = From(source.GetVerb(), LglsFwd(arguments)...);
         result.SetCharge(source.GetCharge());
         return result;
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
            LglsAssumeUser(not CT::Executable<Deint<A>>,
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

      /// Set source                                                          
      TVerb& In(auto&&...arguments) {
         mContext = Many {LglsFwd(arguments)...};
         return *this;
      }

      /// Execute the verb in stateless mode (ignores context)                
      bool RunStateless() const {
         TODO();
         return false;
      }

      /// Execute the verb                                                    
      bool Run() const {
         if (not mContext) {
            // Context is empty and doesn't have any relevant states,   
            // and execution happens only in stateless mode by using    
            // verb argument as the context. This sometimes happens with
            // unary operators, like -5. Since 5 is a number, stateless 
            // subtraction on numbers will be sought and executed.      
            // Another example is selecting global objects, like the    
            // logger, by using `.logger`                               
            return RunStateless();
         }
   
         auto& abilities = mContext.GetType().GetVerbs();
         auto found = abilities.find(GetVerb().GetDefinition());
         if (found == abilities.end())
            return false;

         /*if (mContext.IsDeep()) { //implemented in LglsImplementAbilitiesFor(Annies::Many)
            // Nest if context is deep                                  
            // There is no escape from this scope                       
            size_t successCount = 0;
            auto output = Many::CopyStates(mContext);
            for (size_t i = 0; i < mContext.GetCount(); ++i) {
               DispatchDeep<RESOLVE, DISPATCH, DEFAULT>(mContext.template Get<Many>(i), verb);
   
               if (verb.IsDone()) {
                  if (verb.GetOutput()) {
                     // Cache output, conserving the context hierarchy  
                     output.Compose(Move(verb.GetOutput()));
                  }
   
                  ++successCount;
                  verb.Undo();
               }
            }
   
            return verb.CompleteDispatch(mContext.IsOr(), successCount, Abandon(output));
         }*/

         /*if (mContext.template Is<Tag>()) { // implemented in LglsImplementAbilitiesFor(Annies::Tag)
            // Nest if context is tag.                                  
            // Tags are considered deep only when executing them, as the
            // contents might be executable and need to be evaluated.   
            // There is no escape from this scope.                      
            size_t successCount = 0;
            auto output = Many::CopyStates(mContext);
            for (size_t i = 0; i < mContext.GetCount(); ++i) {
               auto& t = *mContext.template Get<Tag>(i);
               DispatchDeep<RESOLVE, DISPATCH, DEFAULT>(t.GetData(), verb);
   
               if (verb.IsDone()) {
                  if (verb.GetOutput()) {
                     // Cache output, conserving the context hierarchy  
                     output.Compose(Move(verb.GetOutput()));
                  }
   
                  ++successCount;
                  verb.Undo();
               }
            }
   
            return verb.CompleteDispatch(mContext.IsOr(), successCount, Abandon(output));
         }*/
   
         
         //                                                             
         // If reached, then block is flat                              
         size_t successCount = 0;
         auto output = Many::CopyStates(mContext);
   
         // Iterate elements in the current context                     
         for (size_t i = 0; i < mContext.GetCount(); ++i) {
            //verb.SetSource(context.GetElement(i));
            auto ith = mContext.GetElement(i);
            if constexpr (RESOLVE)
               ith = ith.GetResolved();
            else
               ith = ith.GetDense();
   
            verb.SetSource(ith);
            Execute<DISPATCH, DEFAULT, false>(ith, verb);
            
            if (verb.IsDone()) {
               if (verb.GetOutput()) {
                  // Cache output, conserving the context hierarchy     
                  output.Compose(Move(verb.GetOutput()));
               }
   
               ++successCount;
               verb.Undo();
            }
         }
         
         return verb.CompleteDispatch(mContext.IsOr(), successCount, Abandon(output));
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

      /// Reset progress by marking verb as undone and zeroing output         
      TVerb& Clear() noexcept {
         mSuccesses = 0;
         mOutput.Reset();
         return *this;
      }

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
   using Annies::Verb;
}

LANGULUS_MORPHISM_CONCEPT(CT::Executable, Annies::Text, Flow::Code);

/// Define a verb                                                             
///   @param P - positive verb name, as it exists in namespace Langulus::Verbs
///   @param N - negative verb name (optional, same as positive if "")        
///   @param INFOSTRING - information about the verb's purpose                
///   @attention call this macro only in the global namespace!                
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
///   @attention call this macro only in the global namespace!                
#define LANGULUS_DEFINE_OPERATOR(P, N, OP, ON, PRECEDENCE, INFOSTRING) \
   namespace Langulus::Verbs { struct P; } \
   namespace Langulus::CTTI  { template<> struct DefineVerb<::Langulus::Verbs::P> : NamedVerb<#P,#N,PRECEDENCE> {}; } \
   namespace Langulus::CTTI  { template<> struct DefineVerbOp<::Langulus::Verbs::P> : NamedOperator<OP,ON> {}; } \
   namespace Langulus::Verbs { struct P : Annies::TVerb<P> { using CTTI_Info = Yes<INFOSTRING>; }; }
