///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../many/TMany.hpp"
#include "../many/Bytes.hpp"
#include <Langulus/CT/Character.hpp>
#include <Langulus/CT/Number.hpp>
#include <Langulus/Logger.hpp> // Logger has some core fmt::formatters defined  


namespace Langulus::Annies::Serial
{
   enum RuleEnum {
      Skip = 0,
      Wrap = 1
   };

   enum MatchEnum {
      Exact = 0,
      BasedOn = 2,
   };

   enum Operator {
      OpenScope = 0,
      CloseScope,
      OpenScopeAlt,
      CloseScopeAlt,
      OpenCode,
      CloseCode,
      OpenComment,
      CloseComment,
      LineComment,
      OpenString,
      CloseString,
      OpenStringAlt,
      CloseStringAlt,
      OpenCharacter,
      CloseCharacter,
      OpenByte,
      SelectIdea,
      SelectThing,

      Future,
      Past,

      Null,

      Mass,
      Rate,
      Time,
      Priority,

      OpCounter,
      NoOperator = OpCounter,
      ReflectedOperator,
      ReflectedVerb
   };

   struct OperatorProperties {
      Token mToken;
      bool mCharge = false;
   };

   /// Built-in operator properties                                           
   static constexpr std::array<OperatorProperties, Operator::OpCounter> Operators {
      OperatorProperties { "(" },         // OpenScope                  
      OperatorProperties { ")" },         // CloseScope                 
      OperatorProperties { "[" },         // OpenScopeAlt               
      OperatorProperties { "]" },         // CloseScopeAlt              
      OperatorProperties { "{" },         // OpenCode                   
      OperatorProperties { "}" },         // CloseCode                  
      OperatorProperties { "/*" },        // OpenComment                
      OperatorProperties { "*/" },        // CloseComment               
      OperatorProperties { "//" },        // LineComment                
      OperatorProperties { "\"" },        // OpenString                 
      OperatorProperties { "\"" },        // CloseString                
      OperatorProperties { "`" },         // OpenStringAlt              
      OperatorProperties { "`" },         // CloseStringAlt             
      OperatorProperties { "'" },         // OpenCharacter              
      OperatorProperties { "'" },         // CloseCharacter             
      OperatorProperties { "0x" },        // OpenByte                   
      OperatorProperties { "##" },        // SelectIdea                 
      OperatorProperties { "#" },         // SelectThing                

      OperatorProperties { "??" },        // Future                     
      OperatorProperties { "?" },         // Past                       

      OperatorProperties { "null" },      // Null                       

      OperatorProperties { "*", true },   // Mass                       
      OperatorProperties { "^", true },   // Rate                       
      OperatorProperties { "@", true },   // Time                       
      OperatorProperties { "!", true }    // Priority                   
   };

   template<RuleEnum RULE, MatchEnum MATCH, class T, Operator START, Operator END>
   struct Rule {
      using Type = T;
      static constexpr RuleEnum  sRule = RULE;
      static constexpr MatchEnum sMatch = MATCH;
      static constexpr Operator  sStart = START;
      static constexpr Operator  sEnd = END;
   };

} // namespace Langulus::Annies::Serial

namespace Langulus::CT
{
   /// Check if all T are literals - always bounded array with an extent   
   template<class...T>
   concept StringLiteral = sizeof...(T) > 0 and ((CT::Array<T>
         and Character<Deext<Deref<T>>>
      ) and ...);

   /// Check if all T are string pointers, hopefully null-terminated       
   /// This account to all string pointers that _do not have extents_      
   template<class...T>
   concept StringPointer = sizeof...(T) > 0 and ((CT::Sparse<T>
         and not CT::Array<T>
         and Character<Deptr<Deref<T>>>
      ) and ...);

   /// Check if all T are either StringLiteral, or StringPointer           
   template<class...T>
   concept String = sizeof...(T) > 0
         and ((StringLiteral<T> or StringPointer<T>) and ...);
         
   /// Concept for any possible standard library representation of a string   
   /// This includes not only std::string, but also any contiguous range      
   /// that's filled with dense characters                                    
   template<class...T>
   concept StdString = (::std::ranges::contiguous_range<T> and ...)
       and Character<TypeOf<T>...>;

   /// Concept for differentiating managed text types, based on Annies::Text 
   /// Text containers are always binary compatible to Block                  
   template<class...T>
   concept TextBased = ((Decay<T>::CTTI_TextTrait and CT::Block<T>) and ...);

   /// Concept for differentiating built-in text types, that are provided by  
   /// C++ itself (including standard library ones)                           
   template<class...T>
   concept BuiltinText = ((String<T> or Character<T> or StdString<T>) and ...);

   /// Concept for differentiating any form of text                           
   /// This includes: Text containers; any contiguous standard containers,    
   /// that are statically typed and filled with characters; dense character  
   /// types; null-terminated (and unsafe) c-strings; string literals (aka    
   /// bounded character arrays)                                              
   template<class...T>
   concept Text = ((TextBased<T> or BuiltinText<T>) and ...);

   namespace Inner
   {

      /// Do types have an explicit or implicit cast operator to Text         
      template<class...T>
      concept StringifiableByOperator = (std::is_object_v<T> and ...)
         and requires (const T&...a) {
            ((a.operator ::Langulus::Annies::Text()), ...);
         };

      /// Does Text has an explicit/implicit constructor that accepts T       
      template<class...T>
      concept StringifiableByConstructor = requires (const T&...a) {
         ((::Langulus::Annies::Text {a}), ...); };

      /// Used internally in Text, to sum up all types a variadic Text        
      /// constructor can accept                                              
      template<class...T>
      concept Stringifiable = ((Text<T>
           or Number<T>
           or Exception<T>
           or Meta<T>
           or Bytes<T>
           or HasNamedValues<T>
           or Same<T, Annies::Serial::Operator>
           or Inner::StringifiableByOperator<T>
         ) and ...);

      /// Used internally for specializing fmt::formatter without causing     
      /// ambiguities - excludes CT::BuiltinText, as those are already        
      /// defined by {fmt}, as well as other types, like CT::Exception, which 
      /// are defined by Langulus::Logger, etc.                               
      template<class...T>
      concept FmtStringifiable = ((not BuiltinText<T>
          and not Nullptr<T>
          and not Meta<T>
          and not Exception<T>
          and not BuiltinNumber<T>
          and not HasNamedValues<T>
          and (StringifiableByOperator<T> or StringifiableByConstructor<T>)
         ) and ...);

   } // namespace Langulus::CT::Inner

   /// A stringifiable type is one that has either an implicit or explicit    
   /// cast operator to Text type, or can be used to explicitly initialize a  
   /// Text container                                                         
   template<class...T>
   concept Stringifiable = ((Inner::StringifiableByOperator<T>
        or Inner::StringifiableByConstructor<T>) and ...);

} // namespace Langulus::CT

namespace Langulus::A
{

   /// Check if a type is compatible with CT::Character concept at runtime    
   struct Text {
      using CTTI_Abstract = Yup;
      using CTTI_Concrete = Annies::Text;

      static constexpr bool CTTI_TextTrait = true;

      ~Text() = delete;
   };

   struct Code;
}

namespace Langulus::Annies
{

   ///                                                                        
   ///   size_t-terminated UTF text container                                  
   ///                                                                        
   ///   This is a general purpose text container. It can contain serialized  
   /// data, but converting to it is a one way process. While serialization   
   /// aims at being isomorphic, converting to Text implies readability only. 
   /// Consider it a general day-to-day speech container, that may or may not 
   /// be formal. If you want to serialize your data in a both readable and   
   /// formal format, convert to Flow::Code (or other isomorphic format)      
   /// instead.                                                               
   ///                                                                        
   struct Text : Block<char> {
      using Base = Block<char>;
      static constexpr bool CTTI_TextTrait = true;
      static constexpr bool Ownership = true;

      using CTTI_Named = Yes<"Text">;
      using CTTI_Deep = No;
      using CTTI_POD = No;
      using CTTI_Files = Yes<"txt">;
      using CTTI_ReflectAs = Text;
      using CTTI_Bases = Types<A::Text, Base>;
      
      LANGULUS_CONVERTS_FROM(
         Index, Byte, bool, float, double,
         uint8_t, uint16_t, uint32_t, uint64_t,
         int8_t, int16_t, int32_t, int64_t,
         DMeta, TMeta, VMeta, CMeta, AMeta
      );

      /// The presence of this structure makes Text a serializer              
      struct SerializationRules {
         // Text serializer can be lossy to omit unnecessary details,   
         // and you can configure how many elements to show             
         #ifdef LANGULUS_MAX_DEBUGGABLE_ELEMENTS
            static constexpr size_t MaxIterations
               = LANGULUS_MAX_DEBUGGABLE_ELEMENTS;
         #elif LANGULUS(DEBUG) or LANGULUS(SAFE)
            static constexpr size_t MaxIterations = 32;
         #else
            static constexpr size_t MaxIterations = 8;
         #endif

         using Operator = Serial::Operator;

         static constexpr auto Operators = Serial::Operators;
         static constexpr bool CriticalFailure = false;
         static constexpr bool SkipElements = true;

         static bool BeginScope(const CT::Block auto&, Text&);
         static bool EndScope(const CT::Block auto&, Text&);
         static bool Separate(const CT::Block auto&, Text&);
         
         using Rules = Types<
            Serial::Rule<Serial::Wrap, Serial::BasedOn, A::Code,Operator::OpenCode,      Operator::CloseCode>,
            Serial::Rule<Serial::Wrap, Serial::BasedOn, Text,   Operator::OpenString,    Operator::CloseString>,
            Serial::Rule<Serial::Wrap, Serial::BasedOn, Bytes,  Operator::OpenByte,      Operator::NoOperator>,
            Serial::Rule<Serial::Wrap, Serial::Exact,   Letter, Operator::OpenCharacter, Operator::CloseCharacter>
         >;
      };

      using Operator = SerializationRules::Operator;

      ///                                                                     
      ///   Construction                                                      
      ///                                                                     
      constexpr Text() noexcept = default;
      constexpr Text(::std::nullptr_t) noexcept;
      Text(const Text&);
      Text(Text&&) noexcept;

      template<class T> requires CT::TextBased<Deint<T>>
      Text(T&&);

      template<class T> requires CT::String<Deint<T>>
      Text(T&&);

      template<class T> requires CT::Character<Deint<T>>
      Text(T&&);

      template<class T> requires CT::StdString<Deint<T>>
      Text(T&&);

      Text(const CT::Meta auto&);
      Text(const CT::Exception auto&);
      Text(const CT::Bytes auto&);

      explicit Text(Byte);
      explicit Text(bool);
      explicit Text(Operator);
      explicit Text(const CT::HasNamedValues auto&);

      template<CT::Number T> requires (not CT::Character<T>)
      explicit Text(const T&);

      template<class T1, class T2, class...TN>
      requires CT::Inner::Stringifiable<T1, T2, TN...>
      Text(T1&&, T2&&, TN&&...);

      ~Text();

      template<class T> requires CT::String<Deint<T>>
      static Text From(T&&, size_t);

      template<size_t PRECISION = 0, CT::Number T>
      static Text FromNumber(const T&);

      ///                                                                     
      ///   Assignment                                                        
      ///                                                                     
      Text& operator = (const Text&);
      Text& operator = (Text&&) noexcept;

      template<class T> requires CT::TextBased<Deint<T>>
      Text& operator = (T&&);

      ///                                                                     
      ///   Capsulation                                                       
      ///                                                                     
      size_t GetLineCount() const noexcept;

      operator Token () const noexcept;

      ///                                                                     
      ///   Indexing                                                          
      ///                                                                     
      Text Select(CT::Index auto, size_t) const IF_UNSAFE(noexcept);
      Text Select(CT::Index auto, size_t)       IF_UNSAFE(noexcept);
      Text Select(CT::Index auto) const IF_UNSAFE(noexcept);
      Text Select(CT::Index auto)       IF_UNSAFE(noexcept);

      ///                                                                     
      ///   Comparison                                                        
      ///                                                                     
      bool operator == (const CT::Block auto&) const noexcept;
      bool operator == (const CT::Character auto&) const noexcept;
      bool operator == (const CT::String auto&) const noexcept;
      bool operator == (const CT::StdString auto&) const noexcept;
      bool operator == (::std::nullptr_t) const noexcept;

      ///                                                                     
      ///   Insertion                                                         
      ///                                                                     
      Text Extend(size_t);
      Text Terminate() const;

      template<class T> requires CT::Stringifiable<Deint<T>>
      Text& operator << (T&&);
      template<class T> requires CT::Stringifiable<Deint<T>>
      Text& operator >> (T&&);

      template<class T> requires CT::Stringifiable<Deint<T>>
      Text& operator <<= (T&&);
      template<class T> requires CT::Stringifiable<Deint<T>>
      Text& operator >>= (T&&);

      ///                                                                     
      ///   Removal                                                           
      ///                                                                     
      Text Strip  (const CT::Text auto&) const;
      Text Replace(const CT::Text auto& what, const CT::Text auto& with) const;

      ///                                                                     
      ///   Concatenation                                                     
      ///                                                                     
      template<class T> requires CT::Stringifiable<Deint<T>>
      Text operator + (T&&) const;

      template<class T> requires (CT::Stringifiable<Deint<T>> and not CT::TextBased<T>)
      friend Text operator + (T&&, const Text&);

      template<class T> requires CT::Stringifiable<Deint<T>>
      Text& operator += (T&&);

      ///                                                                     
      ///   Conversion                                                        
      ///                                                                     
      explicit operator Many& () noexcept;

   protected:
      template<CT::TextBased THIS, class T>
      THIS ConcatInner(T&&) const;
      template<CT::TextBased THIS, class T>
      THIS& ConcatRelativeInner(T&&);

      void UnfoldInsert(auto&&);

   public:
      ///                                                                     
      ///   Services                                                          
      ///                                                                     
      Text Lowercase() const;
      Text Uppercase() const;

      #if LANGULUS_FEATURE(UNICODE)
         TMany<char16_t> Widen16() const;
         TMany<char32_t> Widen32() const;
      #endif

      static Text Hex(const auto&);
      template<class...ARGS>
      static Text Template(const Token&, ARGS&&...);
      template<class...ARGS>
      static Text TemplateRt(const Token&, ARGS&&...);
      template<class...ARGS>
      static constexpr auto TemplateCheck(const Token&, ARGS&&...);

   protected:
      template<::std::size_t...N>
      static constexpr auto CheckPattern(const Token&, ::std::index_sequence<N...>);
   };

} // namespace Langulus::Annies

namespace Langulus
{

   Annies::Text operator ""_text(const char*, ::std::size_t);

   namespace A
   {

      ///                                                                     
      ///   Abstract code container                                           
      ///                                                                     
      struct Code : Annies::Text {
         LANGULUS(NAME) "A::Code";
         LANGULUS(ACT_AS) A::Code;
         LANGULUS(FILES) "";
         LANGULUS_BASES(Annies::Text);

         using Annies::Text::Text;

         Code(const Annies::Text& a)
            : Annies::Text {a} {}

         Code(Annies::Text&& a)
            : Annies::Text {Forward<Annies::Text>(a)} {}
      };

   } // namespace Langulus::A

} // namespace Langulus

namespace fmt
{

   ///                                                                        
   /// Extend FMT to be capable of logging anything that is convertible to    
   /// Annies::Text. Constness of T doesn't matter.                          
   ///                                                                        
   template<Langulus::CT::Inner::FmtStringifiable T>
   struct formatter<T> {
      template<class CONTEXT>
      constexpr auto parse(CONTEXT& ctx) {
         return ctx.begin();
      }

      template<class CONTEXT> LANGULUS(INLINED)
      auto format(const T& element, CONTEXT& ctx) const {
         using namespace Langulus;
         static_assert(CT::Complete<T>, "T isn't complete");
         auto& me = const_cast<T&>(element);

         Annies::Text asText {me};
         const auto token = asText.operator Token();
         return fmt::format_to(ctx.out(), "{}", token);
      }
   };

} // namespace fmt