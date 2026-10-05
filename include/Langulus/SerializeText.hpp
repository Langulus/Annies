///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Text.hpp"
#include <Langulus/CT/Serializer.hpp>
#include <Langulus/CT/Derived.hpp>
#include <Langulus/CT/Charged.hpp>


namespace Langulus::Flow
{
   struct Code;
}

namespace Langulus::CTTI
{
   /// The presence of this structure makes Text a CT::Serializer             
   template<>
   struct Serializer<Annies::Text> {
      // Text serializer can be lossy to omit unnecessary details,      
      // and you can configure how many elements to show by defining    
      // LANGULUS_MAX_DEBUGGABLE_ELEMENTS.                              
      #ifdef LANGULUS_MAX_DEBUGGABLE_ELEMENTS
         static constexpr size_t MaxIterations = LANGULUS_MAX_DEBUGGABLE_ELEMENTS;
      #elif LANGULUS(DEBUG) or LANGULUS(SAFE)
         static constexpr size_t MaxIterations = 32;
      #else
         static constexpr size_t MaxIterations = 8;
      #endif

      using T = Annies::Text;

      struct Context {};
      
      static constexpr bool CriticalFailure = false;
      static constexpr bool SkipElements = true;

      static bool BeginScope(const CT::Container auto& from, T& to, Context* = nullptr) {
         //TODO multidimensional containers like maps have multiple types
         const bool scoped = from.GetCount() > 1 or not from.IsValid() or from.IsExecutable(); //TODO could carry in context and check verb precedence to avoid scoping in some cases
         if (scoped)
            to += Serial::OpenScope;
         return scoped;
      }
      
      static bool EndScope(const CT::Container auto& from, T& to, Context* = nullptr) {
         //TODO multidimensional containers like maps have multiple types
         const bool scoped = from.GetCount() > 1 or not from.IsValid() or from.IsExecutable(); //TODO could carry in context and check verb precedence to avoid scoping in some cases
         if (scoped)
            to += Serial::CloseScope;
         return scoped;
      }
      
      static void Separate(const CT::Container auto& from, T& to, Context* = nullptr) {
         if constexpr (requires { from.IsOrdered(); }) {
            if constexpr (requires { from.IsOr(); })
               to += (from.IsOr() ? Serial::Or.Token : (from.IsOrdered() ? Serial::And.Token : Serial::AndUnordered.Token));
            else
               to += (from.IsOrdered() ? Serial::And.Token : Serial::AndUnordered.Token);
         }
         else if constexpr (requires { from.IsOr(); })
            to += (from.IsOr() ? Serial::Or.Token : Serial::And.Token);
         else 
            to += Serial::And.Token;
      }
      
      static void Empty(RTTI::DMeta type, size_t i, T& to, Context* = nullptr) {
         if constexpr (CriticalFailure) {
            LglsError("Item #", i, " of type `", type.GetName(),
               "` was serialized to an empty `Text`");
         }
         else {
            to += Serial::OpenComment.Token;
            to += type.GetName();
            to += " -> empty Text";
            to += Serial::CloseComment.Token;
         }
      }
      
      static void Error(RTTI::DMeta type, size_t i, T& to, Context* = nullptr) {
         if constexpr (CriticalFailure) {
            LglsError("Item #", i, " of type `", type.GetName(),
               "` failed to convert to `Text`");
         }
         else {
            to += Serial::OpenComment.Token;
            to += type.GetName();
            to += " -> Text failed";
            to += Serial::CloseComment.Token;
         }
      }
   };

   /// A rule for serializing any deep container, regardless of sparsity.     
   /// This includes Any, Many, Map, Set, Pair, Neat, Tag, etc...             
   /// as well as any templated equivalents. It basically places scopes,      
   /// separators and state decorators, depending on the kind of container.   
   template<CT::Deep C>
   struct SerializationRule<Annies::Text, C> {
      static_assert(Exact<DecvqAll<C>, C>,
         "Strip all decorations on all indirections first");

      using S = Serializer<Annies::Text>;
      using Context = typename S::Context;
      
      static void Serialize(ConstAll<C&>, Annies::Text&, Context*) requires CT::ContainsMany<Decay<C>>;
      static void Serialize(ConstAll<C&>, Annies::Text&, Context*) requires CT::ContainsOne<Decay<C>>;
   };

   /// Rule for serializing Code to Text. Wraps it in {} symbols.             
   template<CT::Container C> requires (not CT::Deep<C>)
   struct SerializationRule<Annies::Text, C> {
      using S = Serializer<Annies::Text>;
      using Context = typename S::Context;

      static void Serialize(ConstAll<C&>, Annies::Text&, Context*);
   };
   
   /// Rule for serializing characters to Text. Wraps them in ''.             
   template<CT::Character C>
   struct SerializationRule<Annies::Text, C> {
      static_assert(CT::Decayed<C>, "Strip all decorations first");
      using S = Serializer<Annies::Text>;
      using Context = typename S::Context;

      static void Serialize(C const&, Annies::Text&, Context*);
   };

   /// MARK: Serialize many                                                   
   /// A rule for serializing any deep container that contains multiple items.
   /// This includes Many, Map, Set, Neat etc...                              
   /// as well as their templated equivalents. It basically places scopes,    
   /// separators and state decorators, depending on the kind of container.   
   template<CT::Deep C>
   void SerializationRule<Annies::Text, C>::Serialize(
      ConstAll<C&> may_be_sparse, Annies::Text& out, Context* context
   ) requires CT::ContainsMany<Decay<C>> {
      using DC = Decay<C>;
      static_assert(CT::NotHandle<DC>);
      DC const& self = DenseCast(may_be_sparse);
      S::BeginScope(self, out, context);

      size_t counter = 0;
      self.Apply([&](auto const& item) {
         if (counter)
            S::Separate(self, out, context);

         Langulus::Serialize(item, out, context);
         ++counter;
      });

      S::EndScope(self, out, context);

      if constexpr (requires { self.IsPast(); }) {
         if (self.IsPast())
            out += Serial::Past;
         else if (self.IsFuture())
            out += Serial::Future;
      }
   }

   /// MARK: Serialize one                                                    
   /// A rule for serializing any deep container that contains single item.   
   /// This includes Any, Handle, Own, Ref, Pair and their templated          
   /// equivalents. Notice that Pair technically contains one item, but with  
   /// two dimensions.                                                        
   template<CT::Deep C>
   void SerializationRule<Annies::Text, C>::Serialize(
      ConstAll<C&> may_be_sparse, Annies::Text& out, Context* context
   ) requires CT::ContainsOne<Decay<C>> {
      using DC = Decay<C>;
      DC const& self = DenseCast(may_be_sparse);

      // Iterate all dimensions                                         
      bool first = true;
      DC::Dimensions::ForEach([&]<unsigned ID> {
         if (first) first = false;
         else out += ": ";                // Dimension separator        

         if constexpr (CT::TypeErased<DC>) {
            //                                                          
            // Serialize a type-erased container                        
            const auto T = self.template GetType<ID>();
            const auto text_meta = MetaDataOf<Annies::Text>();
            const auto serializer = T.GetMorphism(text_meta).serialize;
            try {
               LglsAssert(serializer, "Missing serializer",
                  " from ", T.GetName(), " to ", text_meta.GetName());
               serializer(DecvqAllCast(self.template GetRaw<ID>()), &out, context);
            }
            catch (...) {
               // Catch everything so that we can close any scopes      
               // Text serialization has non-fatal failures             
               S::Error(T, ID, out, context);
            }

            if (T.Is(MetaDataOf<Annies::Any>())) {
               // Annies::Any is the only container that matches       
               // the requirements: CT::Deep<T> and CT::ContainsOne<T>  
               // and having past/future state. Note: TAny is reflected 
               // as Any and is binary compatible as well.              
               auto* item = self.template Get<Annies::Any, ID>();
               if (item->IsPast())
                  out += Serial::Past;
               else if (item->IsFuture())
                  out += Serial::Future;
            }
         }
         else {
            //                                                          
            // Serialize a statically-typed container                   
            using T = Decay<TypeOf<DC, ID>>;
            auto* item = self.template Get<T, ID>();
            try {
               Langulus::Serialize(*item, out, context);
            }
            catch (...) {
               // Catch everything so that we can close any scopes      
               // Text serialization has non-fatal failures             
               S::Error(MetaDataOf<T>(), ID, out, context);
            }

            if constexpr (CT::Deep<T> and CT::ContainsOne<T> and requires { item->IsPast(); }) {
               static_assert(CT::NotHandle<T>);
               if (item->IsPast())
                  out += Serial::Past;
               else if (item->IsFuture())
                  out += Serial::Future;
            }
         }
      });
   }

   /// MARK: Code                                                             
   /// Rule for serializing Code to Text. Wraps it in {} symbols.             
   template<CT::Container C> requires (not CT::Deep<C>)
   void SerializationRule<Annies::Text, C>::Serialize(
      ConstAll<C&> item, Annies::Text& out, [[maybe_unused]] Context* context
   ) {
      if constexpr (Same<C, Flow::Code>) {
         out += Serial::OpenCode;
         out += item;
         out += Serial::CloseCode;
      }
      else if constexpr (Same<C, Annies::Text>) {
         out += Serial::OpenString;
         out += item;
         out += Serial::CloseString;
      }
      else if constexpr (Same<C, Annies::Bytes>) {
         out += Serial::OpenByte;
         out.Reserve(item.GetCount()*2);
         ::std::array<char, sizeof(Byte) * 2> temp;
         auto from = item.GetRaw();
         const auto fromEnd = item.GetRawEnd();
         while (from != fromEnd) {
            ::fmt::format_to_n(temp.data(), 2, "{:02X}", from->value);
            out += Annies::Text(temp);
            ++from;
         }
         out += Serial::CloseByte;   
      }
      else {
         static_assert(false, "Unhandled non-deep container");
         /*if (item.IsEmpty()) // risk of infinite recursion
            return;

         Annies::Text result;
         Serialize(item, result, context);
         out += result;*/
      }
   }
   
   /// MARK: Characters                                                       
   /// Rule for serializing characters to Text. Wraps them in ''.             
   template<CT::Character C>
   void SerializationRule<Annies::Text, C>::Serialize(
      C const& item, Annies::Text& out, Context*
   ) {
      out += Serial::OpenCharacter;
      out += item;
      out += Serial::CloseCharacter;
   }
}

#if LANGULUS_FEATURE(LOGGING)
namespace fmt
{
   /// MARK: {fmt}                                                            
   /// Extend FMT to be capable of logging any Annies container.              
   template<::Langulus::CT::Container T>
   struct formatter<T> {
      template<class CONTEXT>
      constexpr auto parse(CONTEXT& ctx) { return ctx.begin(); }

      template<class CONTEXT>
      auto format(T const& e, CONTEXT& ctx) const {
         try {
            ::Langulus::Annies::Text result;
            ::Langulus::Serialize(e, result);
            return format_to(ctx.out(), "{}", static_cast<::Langulus::Token>(result));
         }
         catch(...) {
            // Don't allow any exceptions to leak out of here           
            return format_to(ctx.out(), "<error while serializing to text>");
         }
      }
   };

   /// Extend FMT to be capable of logging anything convertible to Text, that 
   /// isn't a container, and isn't already implemented in {fmt} itself.      
   template<class T> requires (::Langulus::CT::Convertible<T, ::Langulus::Annies::Text>
                       and not ::Langulus::CT::Container<T>
                       and not ::std::is_fundamental_v<T>)
   struct formatter<T> {
      template<class CONTEXT>
      constexpr auto parse(CONTEXT& ctx) { return ctx.begin(); }

      template<class CONTEXT>
      auto format(T const& e, CONTEXT& ctx) const {
         try {
            auto result = ::Langulus::Convert<::Langulus::Annies::Text>(e);
            return format_to(ctx.out(), "{}", static_cast<::Langulus::Token>(result));
         }
         catch(...) {
            // Don't allow any exceptions to leak out of here           
            return format_to(ctx.out(), "<error while serializing to text>");
         }
      }
   };

   /// Extend FMT to be capable of logging anything with Text operator        
   /*template<class T> requires requires (T const& cast) { cast.operator ::Langulus::Annies::Text(); }
   struct formatter<T> {
      template<class CONTEXT>
      constexpr auto parse(CONTEXT& ctx) { return ctx.begin(); }

      template<class CONTEXT>
      auto format(T const& e, CONTEXT& ctx) const {
         try {
            auto result = e.operator ::Langulus::Annies::Text();
            return format_to(ctx.out(), "{}", static_cast<::Langulus::Token>(result));
         }
         catch(...) {
            // Don't allow any exceptions to leak out of here           
            return format_to(ctx.out(), "<error while serializing to text>");
         }
      }
   };

   /// Extend FMT to be capable of logging anything based on Text             
   template<class T> requires ::Langulus::CT::DerivedFrom<T, ::Langulus::Annies::Text>
   struct formatter<T> {
      template<class CONTEXT>
      constexpr auto parse(CONTEXT& ctx) { return ctx.begin(); }

      template<class CONTEXT>
      auto format(T const& e, CONTEXT& ctx) const {
         auto& result = static_cast<::Langulus::Annies::Text const&>(e);
         try {
            return format_to(ctx.out(), "{}", static_cast<::Langulus::Token>(result));
         }
         catch(...) {
            // Don't allow any exceptions to leak out of here           
            return format_to(ctx.out(), "<error while serializing to text>");
         }
      }
   };*/
}
#endif

/// Convert Charge -> Text                                                    
LANGULUS_MORPHISM_CUSTOM(Langulus::Charge, { 
      Langulus::Annies::Text text;
      if (from.mass != Charge::DefaultMass) {
         text += Serial::Mass.Token; 
         text += from.mass;
      }

      if (from.rate != Charge::DefaultRate) {
         text += Serial::Rate.Token; 
         text += from.rate;
      }

      if (from.time != Charge::DefaultTime) {
         text += Serial::Time.Token; 
         text += from.time;
      }

      if (from.precedence != Charge::DefaultPrecedence) {
         text += Serial::Precedence.Token; 
         text += from.precedence;
      }
      
      return text;
   }, 
   Langulus::Annies::Text
);
