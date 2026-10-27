///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Many.hpp"
#include "Text.hpp"
#include "Annies/Components/Charged-Stack.hpp"
#include "Annies/Components/Verbed-Stack.hpp"
//#include "Langulus/CT/Able.hpp"
//#include "Langulus/CT/Akin.hpp"
#include <Langulus/CT/Executable.hpp>
#include <Langulus/CT/Convertible.hpp>
#include <Langulus/CT/Members.hpp>


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
   /// function calls. Can be verb-constrained: the verb it contains could    
   /// be known at compile-time.                                              
   ///   This template is used as base for all verb definitions, so that you  
   /// can directly use specific verbs as containers, like so:                
   /// Verbs::Write{Verbs::Catenate{1,2,3}}.In(file).AndThen(Verbs::Halt{});  
   ///   Cool, huh?                                                           
   template<class V>
   struct TVerb : Inner::TVerbBase<V> {
      //TODO hmm, should we just inherit Many, so that we can easily get a reference to it? 
      //TODO and just keep everything else on the stack?
      //TODO well, we have composable containers, so we can seamlessly test that out and 
      //TODO measure compile-time and runtime speed. let's try it once we pass all current tests
      using CTTI_ReflectAs = Verb;
      using DeepType       = Many;
      
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
      /// emplaces all A in place of verb's argument                          
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
      
      /// Construction that absorbs the provided containers into the verb's   
      /// argument.                                                           
      template<class A1, class...AN>
      constexpr TVerb(Inner::Absorb, A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0)
            this->Absorb(LglsFwd(a1));
         else {
            this->ConstructDefault();
            this->Concat(LglsFwd(a1), LglsFwd(an)...);
         }
      }
      
      /// Construction that emplaces all arguments inside the verb's argument 
      template<class A1, class...AN>
      constexpr TVerb(Inner::Piecewise, A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0)
            this->EmplaceConstruct(LglsFwd(a1));
         else {
            this->ConstructDefault();
            this->Insert(LglsFwd(a1), LglsFwd(an)...);
         }
      }
      
      /// Create a verb by manually specifying the verb ID and arguments      
      static TVerb From(VMeta verb, auto&&...arguments) {
         TVerb result {LglsFwd(arguments)...};
         result.SetVerb(verb);
         return result;
      }

      /// Create a verb by extracting verb ID and charge from another verb,   
      /// supplying custom arguments.                                         
      static TVerb From(CT::Executable auto const& source, auto&&...arguments) {
         TVerb result = From(source.GetVerb(), LglsFwd(arguments)...);
         result.SetCharge(source.GetCharge());
         return result;
      }

      /// Create a verb by extracting verb ID, charge and argument from verb. 
      /// Source and output remain empty.                                     
      static TVerb Like(CT::Executable auto const& source) {
         return From(source, source.GetRHS());
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
      ///   @attention setting source resets verb completion                  
      auto In(auto&&...arguments) -> TVerb& {
         mContext = Many {LglsFwd(arguments)...};
         Clear();
         return *this;
      }
      
      /// Set arguments                                                       
      template<class...A>
      auto SetArgument(A&&...arguments) -> TVerb& {
         if constexpr (sizeof...(A) == 1) {
            if constexpr (((CT::DeepDense<Deint<A>> and not CT::Executable<Deint<A>>) and ...))
               this->Absorb(LglsFwd(arguments)...);
            else
               this->EmplaceConstruct(LglsFwd(arguments)...);
         }
         else this->Insert(LglsFwd(arguments)...);
         return *this;
      }

      // Set output                                                           
      auto SetOutput(auto&&...arguments) -> TVerb& {
         mOutput = Many {LglsFwd(arguments)...};
         return *this;
      }

      /// Execute the verb in unary mode (ignores context).                   
      /// The ability is determined by the contents of the argument instead.  
      /// Useful for instantiating constants, unary operations, etc.          
      /// Implemented in Langulus::Flow, as it requires some prime verbs      
      /// being defined, such as Verbs::Do.                                   
      bool RunUnary();

      /// Execute the verb.                                                   
      /// Implemented in Langulus::Flow, as it requires some prime verbs      
      /// being defined, such as Verbs::Do.                                   
      bool Run();

      /// MARK: Access                                                        
      /// Hash the verb (emergent). Each sub container should cache their own,
      /// to make things relatively fast.                                     
      auto GetHash() const -> Hash {
         return HashOf(//TODO should charge be ignored?
            this->GetVerb(),//TODO can't we xor these, so that if one changes we just do xor push/pop?
            mContext, 
            Inner::TVerbBase<V>::GetHash(),
            mOutput
         );
      }

      /// Get the verb token or operator depending on context and energy      
      auto GetOperatorToken(bool& tokenized) const -> Text {
         const auto v = this->GetVerb();
         const auto c = this->GetCharge();
         Text out;
         if (not v) {
            // An invalid verb is always written as token               
            out += NameOf<Verb>();//TODO should we just use DefinitionVerb::InvalidName here?
            tokenized = true;
            return out.Lowercase();
         }

         // A valid verb is written either as token, or as operator     
         if (c.mass < 0) {
            if (not v.GetNegativeOperator().empty() and not (-c).IsCharged()) {
               // Write as operator                                     
               out += v.GetNegativeOperator();
            }
            else if (not v.GetNegativeName().empty()) {
               // Write as token                                        
               out += v.GetNegativeName();
               out += -c;
               tokenized = true;
            }
            else {
               // Write as token                                        
               out += v.GetPositiveName();
               out += c;
               tokenized = true;
            }
         }
         else {
            if (not c.IsCharged()) {
               // Write as operator                                     
               out += v.GetPositiveOperator();
            }
            else {
               // Write as token                                        
               out += v.GetPositiveName();
               out += c;
               tokenized = true;
            }
         }

         return out.Lowercase();
      }

      /// Get verb left operand, a.k.a. the context                           
      auto& GetLHS(this auto&& self) noexcept {
         return self.mContext;
      }

      /// Get verb right operand, a.k.a the argument.                         
      ///   @attention notice it's not by reference. Once retrieved, it will  
      ///      be referenced, and thus locked in memory.                      
      auto GetRHS() const noexcept -> Many {
         return {Absorb, *this};
      }

      /// Get verb output                                                     
      auto& GetOutput(this auto&& self) noexcept {
         return self.mOutput;
      }

      /// Access outputs                                                      
      ///   @attention will execute the verb if not executed yet              
      auto* operator -> (this auto&& self) {
         if (not self.IsDone())
            LglsAssert(self.Run(), "Execution failed");
         return &self.mOutput;
      }
      
      /// Get the number of successful satisfactions of the verb              
      constexpr auto GetSuccesses() const noexcept -> size_t {
         return mSuccesses;
      }

      /// Check if verb has been satisfied at least once                      
      ///   @attention a successfully executed verb may or may not have output
      constexpr bool IsDone() const noexcept {
         return mSuccesses > 0;
      }

      /// Check if anything inside the verb is missing on the surface level   
      ///   @return true if anything is missing                               
      bool IsMissing() const noexcept {
         return mContext.IsMissing()
             or Inner::TVerbBase<V>::IsMissing()
             or mOutput.IsMissing();
      }

      /// Check if anything inside the verb is missing deeply                 
      ///   @return true if anything is missing                               
      bool IsMissingDeep() const noexcept {
         return mContext.IsMissingDeep()
             or Inner::TVerbBase<V>::IsMissingDeep()
             or mOutput.IsMissingDeep();
      }

      bool Validate(CT::Index auto&&) const noexcept;

      /// Satisfy verb a number of times                                      
      void Done(size_t c) noexcept {
         mSuccesses = c;
      }

      /// Satisfy verb once                                                   
      void Done() noexcept {
         ++mSuccesses;
      }

      /// MARK: Compare                                                       
      /// Compare verbs for equality                                          
      ///   @param rhs the verb to compare against                            
      ///   @return true if verbs match                                       
      ///   @attention charges are ignored                                    
      constexpr bool operator == (const TVerb& rhs) const {
         return this->GetVerb() == rhs.GetVerb()
            and mContext == rhs.mContext
            and Inner::TVerbBase<V>::operator == (rhs.GetRHS())
            and mOutput == rhs.mOutput;
      }

      /// Three-way comparison by precedence only                             
      ///   @param rhs the verb to compare against                            
      ///   @returns the ordering by precedence                               
      constexpr auto operator <=> (const TVerb& rhs) const noexcept {
         return this->GetPrecedence() <=> rhs.GetPrecedence();
      }

      /// MARK: Charge modification                                           
      constexpr auto operator * (Real const& scalar) const noexcept -> TVerb {
         auto copy = *this;
         copy.GetCharge() *= scalar;
         return copy;
      }

      constexpr auto operator ^ (Real const& scalar) const noexcept -> TVerb {
         auto copy = *this;
         copy.GetCharge() ^= scalar;
         return copy;
      }

      constexpr auto operator *= (Real const& scalar) noexcept -> TVerb& {
         GetCharge() *= scalar;
         return *this;
      }

      constexpr auto operator ^= (Real const& scalar) noexcept -> TVerb& {
         GetCharge() ^= scalar;
         return *this;
      }

      /// Multiply verb charges                                               
      ///   @param rhs the verb to multiply by                                
      ///   @return a new verb, with the modified charge                      
      auto operator * (CT::Executable auto const& rhs) const -> TVerb {
         TVerb shallowCopy = *this;
         shallowCopy.GetCharge().mass *= rhs.GetCharge().mass;
         shallowCopy.GetCharge().time += rhs.GetCharge().time;    // addition is intentional          

         if (not shallowCopy.GetCharge().rate and rhs.GetCharge().rate)
            shallowCopy.GetCharge().rate = rhs.GetCharge().rate;  // overwrite is intentional         
         else if (shallowCopy.GetCharge().rate and rhs.GetCharge().rate)
            shallowCopy.GetCharge().rate *= rhs.GetCharge().rate;
         return shallowCopy;
      }

      /// MARK: Removal                                                       
      void Reset() {
         this->ResetVerb();
         this->ResetCharge();
         mContext.Reset();
         Inner::TVerbBase<V>::Reset();
         mOutput.Reset();
         mSuccesses = 0;
      };

      /// Reset progress by marking verb as undone and zeroing output         
      auto Clear() noexcept -> TVerb& {
         mSuccesses = 0;
         mOutput.Reset();
         return *this;
      }

      /// Type-erasure operator to TVerb<void>                                
      operator Verb&       () noexcept { return reinterpret_cast<Verb      &>(*this); }
      operator Verb const& () noexcept { return reinterpret_cast<Verb const&>(*this); }
   };
}

namespace Langulus
{
   using Annies::TVerb;
   using Annies::Verb;
}

/// Define a verb                                                             
///   @param P positive verb name, as it exists in namespace Langulus::Verbs  
///   @param N negative verb name (optional, same as positive if "")          
///   @param INFOSTRING information about the verb's purpose                  
///   @attention call this macro only in the global namespace!                
#define LANGULUS_DEFINE_VERB(P, N, PRECEDENCE, INFOSTRING) \
   namespace Langulus::Verbs { struct P; } \
   namespace Langulus::CTTI  { template<> struct DefineVerb<::Langulus::Verbs::P> : NamedVerb<#P,#N,PRECEDENCE> {}; } \
   namespace Langulus::Verbs { struct P : Annies::TVerb<P> { using CTTI_Info = Yes<INFOSTRING>; }; }

/// Define a verb with operators                                              
///   @param P positive verb name, as it exists in namespace Langulus::Verbs  
///   @param N negative verb name (optional, same as positive if "")          
///   @param OP positive verb operator                                        
///   @param ON negative verb operator                                        
///   @param PRECEDENCE operator precedence                                   
///   @param INFOSTRING information about the verb's purpose                  
///   @attention call this macro only in the global namespace!                
#define LANGULUS_DEFINE_OPERATOR(P, N, OP, ON, PRECEDENCE, INFOSTRING) \
   namespace Langulus::Verbs { struct P; } \
   namespace Langulus::CTTI  { template<> struct DefineVerb<::Langulus::Verbs::P> : NamedVerb<#P,#N,PRECEDENCE> {}; } \
   namespace Langulus::CTTI  { template<> struct DefineVerbOp<::Langulus::Verbs::P> : NamedOperator<OP,ON> {}; } \
   namespace Langulus::Verbs { struct P : Annies::TVerb<P> { using CTTI_Info = Yes<INFOSTRING>; }; }
