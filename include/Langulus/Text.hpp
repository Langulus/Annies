///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include <Langulus/CT/Text.hpp>
#include <Langulus/CT/Number.hpp>
#include <Langulus/CT/Convertible.hpp>
#include <Langulus/CT/Serializer.hpp>
#include <Langulus/Utils/Byte.hpp>

#include "Handle.hpp"
//#include "Langulus/Typenav.hpp"
#include "Annies/Components/Heap-Movable.hpp"
#include "Annies/Components/Ownership-Stack.hpp"
#include "Annies/Components/IndexedLinear.hpp"
#include "Annies/Components/Insertion.hpp"
#include "Annies/Components/InsertionOperators.hpp"
#include "Annies/Components/InsertionOperatorsConcat.hpp"
#include "Annies/Components/Merging.hpp"
#include "Annies/Components/MergingOperators.hpp"
#include "Annies/Components/Removal.hpp"
#include "Annies/Components/Assignment.hpp"
#include "Annies/Components/Typed-Static.hpp"
#include "Annies/Components/Count-Stack.hpp"
#include "Annies/Components/Reserve-Emergent.hpp"
#include "Annies/Components/Hash-Stack.hpp"
#include "Annies/Components/Iteration-ForEach.hpp"
#include "Annies/Components/Iteration-Range.hpp"
#include "Annies/Components/Comparison.hpp"
#include "Annies/Components/Conversion.hpp"
#include "Annies/States/Disowned.hpp"
#include "Annies/States/Compressed.hpp"
#include "Annies/States/Encrypted.hpp"
//#include <type_traits>
//#include <string_view>
//#include <type_traits>


namespace Langulus::Annies
{
   struct Text;
   struct Bytes;

   namespace Inner
   {
      using TextBase = Com::Container<
         Com::State::Disowned<>,          // Allows disownment          
         Com::TypedStatic<DMeta, char>,   // Type-constrained           
         Com::HeapMovable<0, 0, HeapEntry<0, char*>>,
         Com::CountStack<>,               // Variable count             
         Com::ReserveEmergent<>,          // Capacity derived from alloc
         Com::IndexedLinear<>,            // Indexed directly           
         Com::OwnershipStack<>,           // Allocation is referenced   
         Com::HashStack<>,                // Variable hash (cached)     
         Com::Insertion<true>,            // Serialize + insert         
         Com::InsertionOperators<>,       // << and >> insertion        
         Com::InsertionOperatorsConcat<>, // + and += concat            
         Com::Merging<true>,              // Serialize + merge          
         Com::MergingOperators<>,         // <<= and >>= merging        
         Com::Removal<>,                  // Allows removal             
         Com::Assignment<true>,           // Allows assignment          
         Com::Comparison<true>,           // Allows for comparison      
         Com::Conversion<>,               // Allows conversion          
         Com::IterationForEach<>,         // ForEach iteration          
         Com::IterationRange<>,           // Range iteration            
         Com::State::Compressed<>,        // Toggle compression         
         Com::State::Encrypted<>          // Toggle encryption          
      >;
   }


   ///                                                                        
   /// A continuous text container of variable size                           
   ///                                                                        
   struct Text : Inner::TextBase {
      using CTTI_ReflectAs = Text;
      using CTTI_Text      = Yup;
      using CountType      = Base::CountType;

      constexpr Text() noexcept {
         this->ConstructDefault();
      }

      constexpr Text(nullptr_t) noexcept
         : Text {} {}

      constexpr Text(Text const& other)
         : Text {Refer {other}} {}

      constexpr Text(Text&& other) noexcept
         : Text {Move  {other}} {}

      constexpr ~Text() noexcept {
         this->Destroy();
      }

      /// Construction that absorbs the provided containers                   
      template<class A1, class...AN>
      constexpr Text(Inner::Absorb, A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0)
            this->Absorb(LglsFwd(a1));
         else {
            this->ConstructDefault();
            this->Concat(LglsFwd(a1), LglsFwd(an)...);
         }
      }
      
      /// Construction that emplaces all arguments inside                     
      template<class A1, class...AN>
      constexpr Text(Inner::Piecewise, A1&& a1, AN&&...an) {
         this->ConstructDefault();
         this->Insert(LglsFwd(a1), LglsFwd(an)...);
      }

      /// Construction from Serial::Operator                                  
      ///   @attention this is a non-owning constructor, often used as a      
      ///      temporary                                                      
      explicit constexpr Text(Serial::Operator const& o)
         : Text {o.mToken} {}

      /// Construction from any kind of text that is an Annies container     
      template<CT::Text T> requires CT::Container<T>
      constexpr Text(T&& text) {
         this->Absorb(LglsFwd(text));
      }

      /// Construction from any kind of text that isn't an Annies container  
      ///   @attention non-owning constructor unless you use Copy/Clone. Data 
      ///      lifetime is _your_ responsibility, unless you use Copy/Clone.  
      template<CT::Text T> requires CT::NotContainer<T>
      constexpr Text(T&& text) {
         using I  = IntentOf(text);
         using IT = Deint<I>;
         decltype(auto) source = DeintCast(LglsFwd(text));

         this->ResetState();

         if constexpr (CT::TextLiteral<IT>) {
            // Create from a text literal/bounded array                 
            using CHAR = TypeOf<IT>;
            static_assert(Same<CHAR, char>, "Type mismatch");

            CHAR const* src = source;
            CHAR const* const srcEnd = src + ExtentOf<IT>;
            while (src < srcEnd and *src)
               ++src;

            const auto count = src - source;
            if (not count) {
               this->ConstructDefault();
               return;
            }

            this->SetHeapInner(source);
            this->SetCountInner(count);

            // Bounded arrays and literals are always considered        
            // constexpr, thus no point in searching for their managed  
            // memory.                                                  
            this->SetAllocationInner(nullptr); //TODO hmm, not sure about that. maybe check if const?
         }
         else if constexpr (CT::TextPointer<IT>) {
            // Create from a null-terminated char pointer               
            if (not source) {
               this->ConstructDefault();
               return;
            }

            using CHAR = Deptr<IT>;
            static_assert(Same<CHAR, char>, "Type mismatch");

            size_t count;
            if constexpr (CT::CustomPointer<decltype(source)>)
               count = ::std::char_traits<char>::length(source.Unpack());
            else
               count = ::std::char_traits<char>::length(source);
            
            if (not count) {
               this->ConstructDefault();
               return;
            }
            this->SetHeapInner(source);
            this->SetCountInner(count);

            // We may own this pointer                                  
            #if LANGULUS_FEATURE(MANAGED_MEMORY)
               if constexpr (CT::Disowned<I> or CT::Copied<I> or CT::Cloned<I>)
                  this->SetAllocationInner(nullptr);
               else
                  this->FindAllocationInner();
            #else
               this->SetAllocationInner(nullptr);
            #endif
         }
         else {
            // Create from an std container                             
            static_assert(::std::ranges::contiguous_range<IT>,
               "Unsupported text constructor");

            if (source.empty()) {
               this->ConstructDefault();
               return;
            }

            using CHAR = Deptr<decltype(source.data())>;
            static_assert(Same<CHAR, char>, "Type mismatch");
            this->SetHeapInner(source.data());
            this->SetCountInner(source.size());

            // Assumed never owned by us, no point in searching for the 
            // allocation.                                              
            this->SetAllocationInner(nullptr);
         }

         this->ResetHash();

         // Take ownership if the intent requires it                    
         if constexpr (CT::Copied<I> or CT::Cloned<I>)
            this->TakeOwnership();
      }

      /// Construction from all kinds of characters                           
      ///   @attention this is an owning constructor                          
      template<CT::Character T>
      constexpr Text(T&& ch) {
         this->ResetState();
         this->AllocateFresh(1);
         *this->GetRawAs<char>() = DeintCast(ch);
         this->SetCountInner(1);
         this->ResetHash();
      }
      
      /// MARK: =                                                             
      constexpr Text& operator = (Text const& other) {
         return this->AssignAbsorb(Refer {other});
      }
      constexpr Text& operator = (Text&& other) noexcept {
         return this->AssignAbsorb(Move {other});
      }

      template<class A>
      constexpr Text& operator = (A&& argument) {
         if constexpr (CT::Text<A> and CT::Container<A>)
            return this->AssignAbsorb(LglsFwd(argument));
         else
            return this->Assign(LglsFwd(argument));
      }
      
      /// Construction from all kinds of text, trim length to desired count   
      ///   @attention intent is ignored - this doesn't apply ownership, only 
      ///      interfaces the data - you can TakeOwnership() after this call. 
      ///      Data lifetime is _your_ responsibility.                        
      ///   @attention count will shrink if a terminating character was found,
      ///      or if 'text' is a bounded array of smaller size                
      ///   @param text text to wrap, assumed valid                           
      ///   @param count number of characters inside 'text' to use            
      ///   @return the text wrapped inside a Text container                  
      template<CT::Text T> requires CT::NoIntent<T>
      static Text FromText(T&& text, CountType count) {
         if (count == 0)
            return {};

         Text result {Disown {text}};
         if (count < result.GetCountInner())
            result.SetCountInner(count);
         return result;
      }
      
      /// Create text from a number                                           
      ///   @param number the number to stringify                             
      ///   @param precision number of digits after the floating point, use   
      ///      0 for no truncation. Will produce scientific notation for too  
      ///      big or too small numbers                                       
      ///   @return the text                                                  
      template<CT::Number T> requires CT::NoIntent<T>
      static Text FromNumber(T&& number, int precision = 0) {
         Text result;
         using DT = Decay<T>;

         if constexpr (CT::Real<T>) {
            // Stringify a real number                                  
            constexpr auto size = ::std::numeric_limits<DT>::max_digits10 * 2;
            char temp[size];
            auto [lastChar, errorCode] = ::std::to_chars(
               temp, temp + size, number, ::std::chars_format::general
            );
            LglsAssert(errorCode == ::std::errc(), "std::to_chars failure");

            // Find the dot                                             
            auto dot = temp;
            while (dot < lastChar and *dot != '.')
               ++dot;

            if (dot == lastChar) {
               // There is no dot...                                    
               const auto c = static_cast<CountType>(lastChar - temp);
               result.AllocateFresh(c /*result.RequestHeap(c)*/);
               memcpy(result.GetHeapInner(), temp, c);
               result.SetCountInner(c);
               result.ResetHash();
               return result;
            }

            // Truncate or just remove all trailing zeroes back to dot  
            --lastChar;
            bool approximate = false;

            while (lastChar >= dot) {
               // If last digit is zero/dot directly skip it            
               if (*lastChar == '.' or *lastChar == '0') {
                  --lastChar;
                  continue;
               }

               if (precision) {
                  // We can truncate even more                          
                  if (lastChar > dot + precision) {
                     if (lastChar == dot + precision + 1 and *lastChar > '4') {
                        // Round up                                     
                        while (*lastChar == '9') {
                           // Propagate up until <9 or .                
                           --lastChar;
                        }

                        if (*lastChar == '.')
                           ++(*(--lastChar));
                        else
                           ++(*lastChar);
                     }
                     else --lastChar;

                     approximate = true;
                     continue;
                  }
               }
               break;
            }

            ++lastChar;
            const auto c = static_cast<CountType>(lastChar - temp);
            if (approximate) {
               // We've truncated the number, so prepend a '~' symbol   
               // to signify it's an approximate representation         
               result.AllocateFresh(c + 1);
               auto heap = result.GetRawAs<char>();
               *heap = '~';
               memcpy(heap + 1, temp, c);
               result.SetCountInner(c + 1);
            }
            else {
               result.AllocateFresh(c);
               memcpy(result.GetHeapInner(), temp, c);
               result.SetCountInner(c);
            }
         }
         else if constexpr (CT::Integer<T>) {
            // Stringify an integer                                     
            constexpr auto size = ::std::numeric_limits<DT>::digits10 * 2;
            char temp[size];
            auto [lastChar, errorCode] = ::std::to_chars(temp, temp + size, number);
            LglsAssert(errorCode == ::std::errc(), "std::to_chars failure");

            const auto c = static_cast<CountType>(lastChar - temp);
            result.AllocateFresh(c);
            memcpy(result.GetHeapInner(), temp, c);
            result.SetCountInner(c);
         }
         else static_assert(false, "Unsupported number type");

         result.ResetHash();
         return result;
      }

      /// Generate hexadecimal string from a given value                      
      ///   @param from - the argument                                        
      ///   @return the resulting text                                        
      template<bool REVERSE = false, class T> requires CT::NoIntent<T>
      static Text Hex(T const& from) {
         Text result;
         result.AllocateFresh(sizeof(T) * 2);
         auto from_bytes = reinterpret_cast<const char*>(&from);
         auto to_bytes = result.GetRaw();
         for (size_t i = 0; i < sizeof(T); ++i) {
            if constexpr (REVERSE)
               ::fmt::format_to_n(to_bytes + i * 2, 2, "{:02X}", from_bytes[sizeof(T) - (i + 1)]);
            else
               ::fmt::format_to_n(to_bytes + i * 2, 2, "{:02X}", from_bytes[i]);
         }
         result.SetCountInner(sizeof(T) * 2);
         return result;
      }

      /// Interpret text container as a std::string_view                      
      ///   @attention the string is null-terminated only after Terminate()   
      constexpr operator Token() const noexcept {
         return {this->GetRaw(), this->GetCount()};
      }

      /// Comparing against nullptr_t checks if text is empty                 
      constexpr bool operator == (nullptr_t) const noexcept {
         return this->IsEmpty();
      }

      /// Comparing against null-terminated strings                           
      constexpr bool operator == (const CT::TextPointer auto& rhs) const noexcept {
         if (rhs == nullptr or *rhs == 0)
            return this->IsEmpty();
         return operator == (Text {Disown(rhs)});
      }

      /// Comparing against std containers with characters                    
      constexpr bool operator == (const CT::TextRange auto& rhs) const noexcept {
         return operator == (Text {Disown(rhs)});
      }

      /// Comparison                                                          
      constexpr auto operator <=> (CT::TextRange auto const& other) const noexcept -> ::std::partial_ordering {
         return this->Compare(other);
      }

      constexpr auto operator <=> (Text const& other) const noexcept -> ::std::partial_ordering {
         return ToPartialOrdering(this->Compare(other));
      }

      constexpr bool operator == (Text const& other) const noexcept {
         return this->CompareEqual(other);
      }

      explicit operator ::std::string() const {
         return {this->GetRaw(), this->GetCount()};
      }

      template<Cid> void GetResolved()         = delete;
      template<Cid> void GetDense(size_t = -1) = delete;
   };

   //struct Code : Text {};
   
   inline Text operator ""_text(const char* token, size_t size) noexcept {
      return Text::FromText(token, size);
   }
}

namespace Langulus::CT
{
   namespace Inner
   {
      /// Do types have an explicit/implicit cast operator to Text            
      template<class...T>
      concept StringifiableByOperator = (std::is_object_v<T> and ...)
          and requires (const T&...a) {
            ((a.operator ::Langulus::Annies::Text()), ...);
          };

      /// Does Text has an explicit/implicit constructor that accepts T       
      template<class...T>
      concept StringifiableByConstructor = requires (const T&...a) {
         ((::Langulus::Annies::Text {a}), ...); };
   }

   /// A stringifiable type is one that has either an implicit or explicit    
   /// cast operator to Text type, or can be used to explicitly initialize a  
   /// Text container                                                         
   template<class...T>
   concept Stringifiable = ((Inner::StringifiableByOperator<T>
                          or Inner::StringifiableByConstructor<T>) and ...);
}

/// Convert std::string_view -> Text                                          
LANGULUS_MORPHISM(std::string_view, Langulus::Annies::Text);

/// Convert Serial::Operator -> Text                                          
LANGULUS_MORPHISM(Langulus::Serial::Operator, Langulus::Annies::Text);

/// Convert bool -> Text                                                      
LANGULUS_MORPHISM_CUSTOM(bool, { return from ? "yes" : "no"; }, 
   Langulus::Annies::Text
);

/// Convert Byte -> Text                                                      
LANGULUS_MORPHISM_CUSTOM(Langulus::Byte, { return Annies::Text::Hex(from); }, 
   Langulus::Annies::Text
);

/// Convert Hash -> Text                                                      
LANGULUS_MORPHISM_CUSTOM(Langulus::Hash, { return Annies::Text::Hex(from.value); }, 
   Langulus::Annies::Text
);

/// Convert Number -> Text                                                    
LANGULUS_MORPHISM_CONCEPT_CUSTOM(Langulus::CT::Number, {
      return Annies::Text::FromNumber(from);
   },
   Langulus::Annies::Text
);

/// Convert DMeta -> Text                                                     
LANGULUS_MORPHISM_CUSTOM(Langulus::RTTI::DMeta, { return from.GetName(); }, 
   Langulus::Annies::Text
);

/// Convert TMeta -> Text                                                     
LANGULUS_MORPHISM_CUSTOM(Langulus::RTTI::TMeta, { return from.GetName(); }, 
   Langulus::Annies::Text
);

/// Convert CMeta -> Text                                                     
LANGULUS_MORPHISM_CUSTOM(Langulus::RTTI::CMeta, { return from.GetName(); }, 
   Langulus::Annies::Text
);

/// Convert VMeta -> Text                                                     
LANGULUS_MORPHISM_CUSTOM(Langulus::RTTI::VMeta, { return from.GetCppName(); }, 
   Langulus::Annies::Text
);

/// Convert Literal -> Text                                                   
LANGULUS_MORPHISM_CONCEPT(Langulus::CT::Literal, Langulus::Annies::Text);

/// Map all pointers as convertible to text                                   
LANGULUS_MORPHISM_CONCEPT_CUSTOM(Langulus::CT::Sparse, {
      if constexpr (CT::Complete<Deptr<T>>) {
         if constexpr (CT::Character<Deptr<T>>)
            return {from};
         else
            return NameOf<T>() + "(" + Annies::Text::Hex<true>(from) + ")";
      }
      else return NameOf<T>() + "(" + Annies::Text::Hex<true>(from) + ")";
   },
   Langulus::Annies::Text
);

/// Map all bounded arrays as convertible to text                             
LANGULUS_MORPHISM_CONCEPT_CUSTOM(Langulus::CT::Array, {
      if constexpr (CT::Character<Deext<T>>)
         return {from};
      else {
         TO result;
         for (auto& i : from)
            Langulus::Serialize(i, result);
         return result;
      }
   },
   Langulus::Annies::Text
);

#include "SerializeText.hpp"