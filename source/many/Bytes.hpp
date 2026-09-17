///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "TMany.hpp"


namespace Langulus::CT
{

   /// Any dense value or array of values, that isn't CT::Block               
   template<class...T>
   concept BinablePOD = ((POD<Deext<T>> and Dense<Deext<T>>
          and not Block<Deext<T>> and not Meta<Deext<T>>
      ) and ...);

   namespace Inner
   {
   
      /// Workaround, because of MSVC ICEs introduced in 19.40.33811.0        
      /// Hopefully it will be resolved by them one day                       
      template<class T>
      consteval bool BinableByOperator_AvoidMSVC_ICE() {
         return std::is_object_v<T> and requires (const T& a) {
            a.operator ::Langulus::Annies::Bytes();
         };
      }

      /// Do types have an explicit or implicit cast operator to Bytes        
      template<class...T>
      concept BinableByOperator =
         (BinableByOperator_AvoidMSVC_ICE<T>() and ...);

      /// Does Bytes has an explicit/implicit constructor that accepts T      
      template<class...T>
      concept BinableByConstructor = requires (const T&...a) {
         ((::Langulus::Annies::Bytes {a}), ...); };

      /// Used internally in Bytes, to sum up all types a variadic Bytes      
      /// constructor can accept                                              
      template<class...T>
      concept Binable = ((DerivedFrom<T, Annies::Bytes>
           or BinablePOD<T>
           or Meta<T>
           or Inner::BinableByOperator<T>) and ...);

   } // namespace Langulus::CT::Inner

   /// A binable type is one that has either an implicit or explicit          
   /// cast operator to Bytes type, or can be used to explicitly initialize a 
   /// Bytes container                                                        
   template<class...T>
   concept Binable = ((Inner::BinableByOperator<T>
        or Inner::BinableByConstructor<T>) and ...);
   
   /// Concept for differentiating managed Annies::Bytes                     
   template<class...T>
   concept Bytes = (DerivedFrom<T, Annies::Bytes> and ...);

} // namespace Langulus::CT

namespace Langulus::Annies
{
   
   ///                                                                        
   ///   Byte sequence container                                              
   ///                                                                        
   ///   Convenient wrapper for count-terminated raw byte sequences. Can      
   /// represent any POD type as a sequence of bytes. Also used as a binary   
   /// serializer.                                                            
   ///                                                                        
   struct Bytes : Block<Byte> {
      using Base = Block<Byte>;
      static constexpr bool Ownership = true;

      LANGULUS(DEEP) false;
      LANGULUS(ACT_AS) Bytes;
      LANGULUS(FILES) "lgls";
      LANGULUS_BASES(Base);

      /// The presence of this structure makes Bytes a serializer             
      struct SerializationRules {

      };

      ///                                                                     
      ///   Construction                                                      
      ///                                                                     
      constexpr Bytes() = default;
      Bytes(const Bytes&);
      Bytes(Bytes&&) noexcept;

      template<class T> requires CT::Bytes<Deint<T>>
      Bytes(T&&);

      explicit Bytes(const CT::BinablePOD auto&);
      Bytes(const CT::Meta auto&);

      template<class T1, class T2, class...TN>
      requires CT::Inner::Binable<T1, T2, TN...>
      Bytes(T1&&, T2&&, TN&&...);

      ~Bytes();

      template<class T> requires (CT::Sparse<Deint<T>> and CT::Byte<Decay<Deint<T>>>)
      static Bytes From(T&&, Count);

      ///                                                                     
      ///   Assignment                                                        
      ///                                                                     
      Bytes& operator = (const Bytes&);
      Bytes& operator = (Bytes&&);

      template<class T> requires CT::Bytes<Deint<T>>
      Bytes& operator = (T&&);
      
      ///                                                                     
      ///   Capsulation                                                       
      ///                                                                     
      Hash GetHash() const;

      ///                                                                     
      ///   Indexing                                                          
      ///                                                                     
      Bytes Select(Offset, Count) const IF_UNSAFE(noexcept);
      Bytes Select(Offset, Count)       IF_UNSAFE(noexcept);

      ///                                                                     
      ///   Comparison                                                        
      ///                                                                     
      bool operator == (const CT::Block auto&) const noexcept;
      bool operator == (const CT::BinablePOD auto&) const noexcept;

      ///                                                                     
      ///   Insertion                                                         
      ///                                                                     
      Bytes Extend(Count);

      template<class T> requires CT::Binable<Deint<T>>
      Bytes& operator << (T&&);
      template<class T> requires CT::Binable<Deint<T>>
      Bytes& operator >> (T&&);

      ///                                                                     
      ///   Concatenation                                                     
      ///                                                                     
      template<class T> requires CT::Binable<Deint<T>>
      Bytes operator + (T&&) const;

      template<class T> requires CT::Binable<Deint<T>>
      Bytes& operator += (T&&);

   protected:
      template<CT::Bytes THIS, class T>
      THIS ConcatInner(T&&) const;
      template<CT::Bytes THIS, class T>
      THIS& ConcatRelativeInner(T&&);

      void UnfoldInsert(auto&&);

   public:
      ///                                                                     
      ///   Deserialization                                                   
      ///                                                                     
      Count Deserialize(CT::NotVoid auto&) const;

      ///                                                                     
      ///   Conversion                                                        
      ///                                                                     
      operator Many& () const noexcept;
   };

} // namespace Langulus::Annies
