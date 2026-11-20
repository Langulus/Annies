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
   /// TVerbs is similar to the usual type-erased Many, which is used   
   /// for the verb arguments (retrievable by GetRHS). Adds charge and  
   /// verb ID components, Many mOutput and Many mContext as members.   
   ///   @attention it is important to notice that insertion operators  
   ///      like << and >>, and merge operators like <<= and >>=, act   
   ///      on mOutput, instead of the verb argument. They also         
   ///      implicitly mark the verb as satisfied.                      
   template<class V>
   using TVerbBase = Com::Container<
      Com::State::Disowned<>,          // Allows disownment             
      Com::State::Future<>,            // Toggle future linking         
      Com::State::Past<>,              // Toggle past linking           
      Com::State::Or<>,                // Toggle disjunction            
      Com::State::Compressed<>,        // Toggle compression            
      Com::State::Encrypted<>,         // Toggle encryption             
      Com::TypedStack<DMeta>,          // Type-erased                   
      Com::VerbedStack<VMeta, V>,      // Add verb, make executable     
      Com::HeapMovable<>,              // Pointer to heap memory        
      Com::CountStack<>,               // Dynamically sized             
      Com::ReserveStack<>,             // Reserve kept as member        
      Com::ChargedStack<>,             // Add charge                    
      Com::IndexedLinear<>,            // Indexed directly              
      Com::OwnershipStack<>,           // Allocation is referenced      
      Com::OwnershipDeepHeap<>,        // Sparse elements are referenced
      /*Com::HashStack<>,*/                // Hash can be cached            
      Com::Deepening<Many>,            // Makes container deep          
      Com::Insertion<>,                // Allows insertion              
      //Com::InsertionOperators<>,       // << and >> insertion           
      //Com::InsertionOperatorsConcat<>,
      Com::Merging<>,                  // Allows merging                
      //Com::MergingOperators<>,         // <<= and >>= merging           
      Com::Emplacement<>,              // Allows emplacement            
      Com::Assignment<>,               // Allows assignment             
      Com::Removal<>,                  // Allows clear/reset            
      Com::Conversion<>,               // Allows conversions            
      Com::Comparison<false ,false>,               // Allows comparisons            
      Com::IterationForEach<>,         // ForEach iteration             
      Com::IterationRange<>,           // Ranged iteration              
      Com::Descriptor<>                // Add descriptor interface      
   >;

   /// TVerbs extend the usual type-erased Any, by adding charge and verb ID  
   /// as members.                                                            
   /*template<class V>
   using TVerbBase = typename ManyBase::template Include<//TODO consider using Com::Composite with Many on the stack, so we can take easy reference of the argument!
      Com::VerbedStack<VMeta, V>,  // Add verb, make executable         
      Com::ChargedStack<>          // Add charge                        
   >;*/
}

namespace Langulus::Annies
{
   ///                                                                        
   /// MARK: TVerb                                                            
   ///   A type-erased container specifically designed for fully capsulating  
   /// function calls. Can be verb-constrained: the verb it contains could    
   /// be known at compile-time.                                              
   ///   This template is used as base for all verb definitions, so that you  
   /// can directly use specific verbs as argument containers, like so:       
   /// Verbs::Write{Verbs::Catenate{1,2,3}}.In(file).Then(Verbs::Halt{});     
   ///   Cool, huh?                                                           
   template<class V>
   struct TVerb : Inner::TVerbBase<V> {
      using CTTI_ReflectAs  = Verb;
      using CTTI_Executable = Yup;
      using CTTI_Deep       = No;
      using DeepType        = Many;
      using ArgumentType    = Inner::TVerbBase<V>;
      
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

      constexpr TVerb(TVerb const& other)
         : mSuccesses {other.mSuccesses}
         , mOutput    {other.mOutput}
         , mContext   {other.mContext} {
         this->Absorb(Refer(other));
      }

      constexpr TVerb(TVerb&& other) noexcept
         : mSuccesses {other.mSuccesses}
         , mOutput    {Move {other.mOutput}}
         , mContext   {Move {other.mContext}} {
         this->Absorb(Move(other));
         other.mSuccesses = 0;
      }

      constexpr ~TVerb() noexcept {
         this->Destroy();
      }

      /// Construction that either absorbs the provided containers, or        
      /// emplaces all A in place of verb's argument                          
      template<Disambiguate A1, class...AN>
      constexpr TVerb(A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0 and not CT::BoundedArray<A1>) {
            if constexpr ((CT::DeepDense<A1> and not CT::Handle<A1>) or SameAsOneOf<Deint<A1>, TVerb, Verb>) {
               LglsAssumeUserAndBreak(Same<Deint<A1>, TVerb>,
                  "Ambiguous use of construction "
                  "- you should use tag-dispatch with first argument either Absorb "
                  "(if you want to overwrite the verb itself) or Piecewise "
                  "(if you want to insert the verb as argument) in order to clearly "
                  "state your intent. Absorb will be used by default!"
               );
               this->Absorb(LglsFwd(a1));

               if constexpr (CT::Executable<A1>) {
                  decltype(auto) other = DeintCast(a1);
                  mSuccesses = other.mSuccesses;
                  mOutput = NestIntentOf(a1, other.mOutput);
                  mContext = NestIntentOf(a1, other.mContext);

                  if constexpr (IntentOf(a1)::ResetsOnMove())
                     other.mSuccesses = 0;
               }
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
      template<class...A>
      constexpr TVerb(Inner::Absorb, A&&...args) {
         if constexpr (sizeof...(A) == 0)
            this->ConstructDefault();
         else if constexpr (sizeof...(A) == 1) {
            this->Absorb(LglsFwd(args)...);

            if constexpr (CT::Executable<A...>) {
               decltype(auto) other = DeintCast(args...[0]);
               mSuccesses = other.mSuccesses;
               mOutput = NestIntentOf(args...[0], other.mOutput);
               mContext = NestIntentOf(args...[0], other.mContext);

               if constexpr (IntentOf(args...[0])::ResetsOnMove())
                  other.mSuccesses = 0;
            }
         }
         else {
            this->ConstructDefault();
            this->Concat(LglsFwd(args)...);
         }
      }
      
      /// Construction that emplaces all arguments inside the verb's argument 
      template<class...A>
      constexpr TVerb(Inner::Piecewise, A&&...args) {
         if constexpr (sizeof...(A) == 0)
            this->ConstructDefault();
         else if constexpr (sizeof...(A) == 1 and not CT::BoundedArray<A...>)
            this->EmplaceConstruct(LglsFwd(args)...);
         else {
            this->ConstructDefault();
            this->Insert(LglsFwd(args)...);
         }
      }
      
      /// Create a verb by manually specifying the verb ID and arguments      
      static TVerb From(VMeta verb, auto&&...arguments) {
         TVerb result {Piecewise, LglsFwd(arguments)...};
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

      /// Partial copy, copies only charge and verb, provides new arguments.  
      /// Context and output remain empty.                                    
      ///   @return the partially copied verb                                 
      template<class SELF>
      constexpr SELF Fork(this SELF const& self, auto&&...args) {
         return SELF::From(self, LglsFwd(args)...);
      }

      /// Assignment                                                          
      constexpr TVerb& operator = (TVerb const& other) {
         this->AssignAbsorb(Refer(other));
         mSuccesses = other.mSuccesses;
         mOutput = other.mOutput;
         mContext = other.mContext;
         return *this;
      }

      constexpr TVerb& operator = (TVerb&& other) noexcept {
         this->AssignAbsorb(Move(other));
         mSuccesses = other.mSuccesses;
         mOutput = Move(other.mOutput);
         mContext = Move(other.mContext);
         other.mSuccesses = 0;
         return *this;
      }
      
      template<class A>
      constexpr TVerb& operator = (A&& argument) {
         if constexpr ((CT::DeepDense<A> and not CT::Handle<A>) or SameAsOneOf<Deint<A>, TVerb, Verb>) {
            LglsAssumeUserAndBreak(Same<Deint<A>, TVerb>,
               "Ambiguous use of assignment "
               "- you should use either AssignAbsorb (if you want to overwrite "
               "the verb itself) or Assign (if you want to insert the verb as argument) "
               "in order to clearly state your intent. "
               "AssignAbsorb will be used by default!"
            );

            this->AssignAbsorb(LglsFwd(argument));

            if constexpr (CT::Executable<A>) {
               decltype(auto) other = DeintCast(argument);
               mSuccesses = other.mSuccesses;
               mOutput = NestIntentOf(argument, other.mOutput);
               mContext = NestIntentOf(argument, other.mContext);

               if constexpr (IntentOf(argument)::ResetsOnMove())
                  other.mSuccesses = 0;
            }
            return *this;
         }
         else return this->Assign(LglsFwd(argument));
      }

      /// Set source                                                          
      ///   @attention setting source resets output and satisfaction          
      template<class SELF>
      auto In(this SELF&& self, auto&&...arguments) -> SELF&& {
         self.mContext = Many {LglsFwd(arguments)...};
         self.Clear();
         return LglsFwd(self);
      }
      
      /// Set arguments                                                       
      template<class SELF>
      auto SetArgument(this SELF&& self, auto&&...arguments) -> SELF&& {
         self.AssignAbsorb(Many {LglsFwd(arguments)...});
         return LglsFwd(self);
      }

      /// Set output                                                          
      template<class SELF>
      auto SetOutput(this SELF&& self, auto&&...arguments) -> SELF&& {
         self.mOutput = Many {LglsFwd(arguments)...};
         return LglsFwd(self);
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
      /*auto GetHash() const -> Hash {
         return HashOf(//TODO should charge be ignored?
            this->GetVerb(),
            mContext, 
            ArgumentType::GetHash(),
            mOutput
         );
      }*/

      /// Get the verb token/operator and charge, depending on energy         
      ///   @param tokenized [in/out] informs the outside world that a token  
      ///      instead of an operator was chosen for representing verb.       
      ///   @return the token/operator, and charge appended to it, if relevant
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

         LglsAssumeDev(not v.GetPositiveName().empty(), "Invalid verb token");

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
               if (not v.GetPositiveOperator().empty()) {
                  // Write as operator                                  
                  out += v.GetPositiveOperator();
               }
               else {
                  out += v.GetPositiveName();
                  tokenized = true;
               }
            }
            else {
               // Write as token & charge                               
               out += v.GetPositiveName();
               out += c;
               tokenized = true;
            }
         }

         return out;//.Lowercase(); //TODO guarantee it is lowercased at compile-time
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
             or ArgumentType::IsMissing()
             or mOutput.IsMissing();
      }

      /// Check if anything inside the verb is missing deeply                 
      ///   @return true if anything is missing                               
      bool IsMissingDeep() const noexcept {
         return mContext.IsMissingDeep()
             or ArgumentType::IsMissingDeep()
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
         bool same_verb = this->GetVerb() == rhs.GetVerb();
         bool same_context = mContext == rhs.mContext;
         bool same_argument = ArgumentType::operator == (rhs.GetRHS());
         bool same_output = mOutput == rhs.mOutput;
         return same_verb and same_context and same_argument and same_output;
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
         this->GetCharge() *= scalar;
         return *this;
      }

      constexpr auto operator ^= (Real const& scalar) noexcept -> TVerb& {
         this->GetCharge() ^= scalar;
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
         ArgumentType::Reset();
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
      //operator Verb const&&() const noexcept { return reinterpret_cast<Verb     &&>(*this); }
      //operator Verb&&      () noexcept { return reinterpret_cast<Verb     &&>(*this); }
      operator Verb&       ()       noexcept { return reinterpret_cast<Verb      &>(*this); }
      operator Verb const& () const noexcept { return reinterpret_cast<Verb const&>(*this); }

      /// Execute a verb in the resulting output of this one                  
      ///   @attention this verb will be executed if not executed yet         
      ///   @param verb the verb to execute next, in the context of this      
      ///      verb's output                                                  
      ///   @return a reference to the new verb after execution               
      template<CT::Executable NEXT>
      auto Then(this auto&& self, NEXT& verb) -> NEXT& {
         if (not self.IsDone())
            LglsAssert(self.Run(), "Execution failed");

         verb.In(self.mOutput).Run();
         return verb;
      }

      /// Push anything to end of the outputs, satisfying the verb            
      ///   @attention nullptrs are never pushed and don't satisfy verb       
      ///   @param rhs the data and intent to push                            
      ///   @return a reference to this verb for chaining                     
      template<class SELF>
      SELF& operator << (this SELF& self, auto&& rhs) {
         using S = IntentOf(rhs);
         using T = Deint<S>;

         if constexpr (not CT::Null<T>) {
            if constexpr (CT::Sparse<T>) {
               // Push a pointer only if valid                          
               if (not DeintCast(rhs))
                  return self;
            }

            if (self.mOutput.Compose(LglsFwd(rhs)))
               self.Done();
         }

         return self;
      }

      /// Push anything to the front of the outputs, satisfying the verb      
      ///   @attention nullptrs are never pushed and don't satisfy verb       
      ///   @param rhs the data and intent to push                            
      ///   @return a reference to this verb for chaining                     
      /*template<class SELF>
      SELF& operator >> (this SELF& self, auto&& rhs) {
         using S = IntentOf(rhs);
         using T = Deint<S>;

         if constexpr (not CT::Null<T>) {
            if constexpr (CT::Sparse<T>) {
               // Push a pointer only if valid                          
               if (not DeintCast(rhs))
                  return self;
            }

            if (self.mOutput.ComposeAt(Index::Front, LglsFwd(rhs)))
               self.Done();
         }

         return self;
      }*/

      /// Merge anything to output's back, with or without an intent          
      ///   @param rhs the data and intent to merge                           
      ///   @return a reference to this verb for chaining                     
      template<class SELF>
      SELF& operator <<= (this SELF& self, auto&& rhs) {
         using S = IntentOf(rhs);
         using T = Deint<S>;

         if constexpr (not CT::Null<T>) {
            if constexpr (CT::Sparse<T>) {
               // Push a pointer, but check if valid first              
               if (not DeintCast(rhs))
                  return self;
            }

            if (self.mOutput.Find(DeintCast(rhs)))
               return self;

            if (self.mOutput.Compose(LglsFwd(rhs)))
               self.Done();
         }

         return self;
      }

      /// Merge anything to output's front, with or without an intent         
      ///   @param rhs - the data and intent to merge                         
      ///   @return a reference to this verb for chaining                     
      /*template<class SELF>
      SELF& operator >>= (this SELF& self, auto&& rhs) {
         using S = IntentOf(rhs);
         using T = Deint<S>;

         if constexpr (not CT::Null<T>) {
            if constexpr (CT::Sparse<T>) {
               // Push a pointer, but check if valid first              
               if (not DeintCast(rhs))
                  return self;
            }

            if (self.mOutput.Find(DeintCast(rhs)))
               return self;

            if (self.mOutput.Compose(Index::Front, LglsFwd(rhs)))
               self.Done();
         }

         return self;
      }*/
   };
}

/// Define a verb                                                             
///   @param P positive verb name, as it exists in namespace Langulus::Verbs  
///   @param N negative verb name (optional, same as positive if "")          
///   @param INFOSTRING information about the verb's purpose                  
///   @attention call this macro only in the global namespace!                
#define LANGULUS_DEFINE_VERB(P, N, PRECEDENCE, INFOSTRING) \
   namespace Langulus::Verbs { struct P; } \
   namespace Langulus::CTTI  { template<> struct DefineVerb<::Langulus::Verbs::P> : NamedVerb<#P,#N,PRECEDENCE> {}; } \
   namespace Langulus::Verbs { struct P : Annies::TVerb<P> { \
      using CTTI_Info = Yes<INFOSTRING>; \
      using Annies::TVerb<P>::TVerb; \
   };}

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
   namespace Langulus::Verbs { struct P : Annies::TVerb<P> { \
      using CTTI_Info = Yes<INFOSTRING>; \
      using Annies::TVerb<P>::TVerb; \
   }; }
