///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
//#include "Handle.hpp"
//#include "Langulus/IntentOf.hpp"
#include <source/components/Typed-Static.hpp>
#include <source/components/Heap-Movable.hpp>
#include <source/components/Count-Stack.hpp>
#include <source/components/Reserve-Emergent.hpp>
#include <source/components/Ownership-Stack.hpp>
#include <source/components/Hash-Stack.hpp>
#include <source/components/Dictionary-Heap.hpp>
#include <source/components/Insertion.hpp>
#include <source/components/InsertionOperators.hpp>
#include <source/components/InsertionOperatorsConcat.hpp>
#include <source/components/Removal.hpp>
#include <source/components/Assignment.hpp>
#include <source/components/Comparison.hpp>
#include <source/components/Conversion.hpp>
#include <source/components/IndexedLinear.hpp>
#include <source/components/Iteration-ForEach.hpp>
#include <source/components/Iteration-Range.hpp>
#include <source/states/Disowned.hpp>
#include <source/states/Compressed.hpp>
#include <source/states/Encrypted.hpp>
#include <Langulus/CT/POD.hpp>
#include <Langulus/CT/Convertible.hpp>
#include <Langulus/Utils/Byte.hpp>


namespace Langulus::Annies
{
   struct Bytes;

   namespace Inner
   {
      /// The context holds the header entries, that allow us to              
      /// serialize types, tags, consts and verbs across sessions.            
      struct BytesDictionary {
         enum class BuiltInTypes {
            Bool = 1,
            I8, I16, I32, I64,
            U8, U16, U32, U64,
            Char, Byte, Half, Float, Double,
            _Counter_
         };

         template<class T>
         struct Bank {
            ::std::unordered_map<T, uint64_t> mDefinitions;
            uint64_t mNextId = Same<T, RTTI::DMeta> ? static_cast<uint64_t>(BuiltInTypes::_Counter_) : 1;

            uint64_t Define(T&& meta) {
               // Built-in types are reserved, no need to serialize     
               // them.                                                 
               if (not meta)
                  return 0;
               
               if constexpr (Same<T, RTTI::DMeta>) {
                  if (meta.IsSame(MetaDataOf<bool>()))
                     return static_cast<uint64_t>(BuiltInTypes::Bool);
                  else if (meta.IsSame(MetaDataOf<int8_t>()))
                     return static_cast<uint64_t>(BuiltInTypes::I8);
                  else if (meta.IsSame(MetaDataOf<int16_t>()))
                     return static_cast<uint64_t>(BuiltInTypes::I16);
                  else if (meta.IsSame(MetaDataOf<int32_t>()))
                     return static_cast<uint64_t>(BuiltInTypes::I32);
                  else if (meta.IsSame(MetaDataOf<int64_t>()))
                     return static_cast<uint64_t>(BuiltInTypes::I64);
                  else if (meta.IsSame(MetaDataOf<uint8_t>()))
                     return static_cast<uint64_t>(BuiltInTypes::U8);
                  else if (meta.IsSame(MetaDataOf<uint16_t>()))
                     return static_cast<uint64_t>(BuiltInTypes::U16);
                  else if (meta.IsSame(MetaDataOf<uint32_t>()))
                     return static_cast<uint64_t>(BuiltInTypes::U32);
                  else if (meta.IsSame(MetaDataOf<uint64_t>()))
                     return static_cast<uint64_t>(BuiltInTypes::U64);
                  else if (meta.IsSame(MetaDataOf<char>()))
                     return static_cast<uint64_t>(BuiltInTypes::Char);
                  else if (meta.IsSame(MetaDataOf<Byte>()))
                     return static_cast<uint64_t>(BuiltInTypes::Byte);
                  //else if (meta.IsSame(MetaDataOf<half>())) //TODO
                  //   return static_cast<uint64_t>(BuiltInTypes::Half;
                  else if (meta.IsSame(MetaDataOf<float>()))
                     return static_cast<uint64_t>(BuiltInTypes::Float);
                  else if (meta.IsSame(MetaDataOf<double>()))
                     return static_cast<uint64_t>(BuiltInTypes::Double);
               }

               auto found = mDefinitions.find(meta);
               if (found != mDefinitions.end())
                  return found->second;
               mDefinitions[meta] = mNextId;
               return mNextId++;
            }
         };

         Bank<RTTI::DMeta> mDMetaBank;
         Bank<RTTI::TMeta> mTMetaBank;
         Bank<RTTI::CMeta> mCMetaBank;
         Bank<RTTI::VMeta> mVMetaBank;
      };

      using BytesBase = Com::Container<
         Com::State::Disowned<>,             // Allows disownment       
         Com::TypedStatic<DMeta, Byte>,      // Type-constrained        
         Com::HeapMovable<0, 0, HeapEntry<0, Byte*>>,
         Com::CountStack<>,                  // Variable count          
         Com::ReserveEmergent<>,             // Emergent reserve        
         Com::IndexedLinear<>,               // Indexed directly        
         Com::OwnershipStack<>,              // Allocation is referenced
         Com::HashStack<>,                   // Variable hash (cached)  
         Com::DictionaryHeap<BytesDictionary>,
         Com::Insertion<true>,               // Serialize + insert      
         Com::InsertionOperators<>,          // << and >> insertion     
         Com::InsertionOperatorsConcat<>,    // + and += concat         
         Com::Removal<>,                     // Allows removal          
         Com::Assignment<true>,              // Allows assignment       
         Com::Comparison<true>,              // Allows for comparison   
         Com::Conversion<>,                  // Allows conversion       
         Com::IterationForEach<>,            // ForEach iteration       
         Com::IterationRange<>,              // Range iteration       😊
         Com::State::Compressed<>,           // Toggle compression      
         Com::State::Encrypted<>             // Toggle encryption       
      >;
   }
   

   ///                                                                        
   /// A contiguous byte container of variable size                           
   ///                                                                        
   #pragma pack(push, 4)
   struct Bytes : Inner::BytesBase {
      using CTTI_ReflectAs  = Bytes;
      using CountType       = Base::CountType;

      constexpr Bytes() noexcept {
         this->ConstructDefault();
      }

      constexpr Bytes(Bytes const& other)
         : Bytes {Refer {other}} {}

      constexpr Bytes(Bytes&& other) noexcept
         : Bytes {Move  {other}} {}

      constexpr ~Bytes() noexcept {
         this->Destroy();
      }
      
      /// Construction that absorbs the provided containers                   
      template<class A1, class...AN>
      constexpr Bytes(Inner::Absorb, A1&& a1, AN&&...an) {
         if constexpr (sizeof...(AN) == 0)
            this->Absorb(LglsFwd(a1));
         else {
            this->ConstructDefault();
            this->Concat(LglsFwd(a1), LglsFwd(an)...);
         }
      }
      
      /// Construction that emplaces all arguments inside                     
      template<class A1, class...AN>
      constexpr Bytes(Inner::Piecewise, A1&& a1, AN&&...an) {
         this->ConstructDefault();
         this->Insert(LglsFwd(a1), LglsFwd(an)...);
      }

      /// Construction from any kind of other bytes with intent               
      template<template<class> class I> requires CT::Intent<I<Bytes>>
      constexpr Bytes(I<Bytes>&& bytes) {
         this->Absorb(LglsFwd(bytes));
      }

      /// Construction from any kind of POD value.                            
      /// Works for bounded arrays as well.                                   
      ///   @attention non-owning constructor unless you use Copy/Clone. Data 
      ///      lifetime is _your_ responsibility, unless you use Copy/Clone.  
      template<class T> requires CT::POD<Deint<T>>
      explicit constexpr Bytes(T&& source) {
         decltype(auto) data = DeintCast(source);
         constexpr size_t bytesize = sizeof(Deint<T>);
         this->ResetState();

         if constexpr (CT::Copied<T> or CT::Cloned<T>) {
            // Take ownership if the intent requires it                 
            this->AllocateFresh(bytesize);
            if constexpr (CT::Array<T>)
               memcpy(this->GetRaw(),  data, bytesize);
            else
               memcpy(this->GetRaw(), &data, bytesize);

            this->SetCountInner(bytesize);
         }
         else {
            // Only interface data, reference if able to and if desired 
            if constexpr (CT::Array<T>)
               this->SetHeapInner(static_cast<const void*>( data));
            else
               this->SetHeapInner(static_cast<const void*>(&data));

            this->SetCountInner(bytesize);

            // We may still own this data                               
            #if LANGULUS_FEATURE(MANAGED_MEMORY)
               if constexpr (CT::Disowned<T>)
                  this->SetAllocationInner(nullptr);
               else
                  this->FindAllocationInner();
            #else
               this->SetAllocationInner(nullptr);
            #endif
         }

         this->ResetHash();
      }

      /// MARK: =                                                             
      constexpr Bytes& operator = (Bytes const& other) {
         return this->AssignAbsorb(Refer {other});
      }
      constexpr Bytes& operator = (Bytes&& other) noexcept {
         return this->AssignAbsorb(Move {other});
      }

      template<class A>
      constexpr Bytes& operator = (A&& argument) {
         if constexpr (Same<Deint<A>, Bytes>)
            return this->AssignAbsorb(LglsFwd(argument));
         else
            return this->Assign(LglsFwd(argument));
      }

      /// Construction from raw bytes data                                    
      ///   @attention this doesn't apply ownership, only interfaces the data.
      ///      You can TakeOwnership() after this call if you want.           
      ///   @attention data lifetime is _your_ responsibility                 
      ///   @param data data to wrap, assumed valid                           
      ///   @param count number of bytes inside 'data' to use                 
      ///   @return the raw bytes wrapped inside a Bytes container            
      static Bytes FromBytes(void const* data, size_t count) noexcept {
         if (count == 0)
            return {};

         Bytes result;
         result.SetHeapInner(data);
         result.SetCountInner(count);
         return result;
      }

      /// Construction by interfacing POD data                                
      ///   @attention intent is ignored - this doesn't apply ownership, only 
      ///      interfaces the data - you can TakeOwnership() after this call  
      ///   @attention data lifetime is _your_ responsibility                 
      ///   @param data data to wrap, assumed valid, supports arrays          
      ///   @param count number of bytes inside 'data' to use                 
      ///   @return the raw bytes wrapped inside a Bytes container            
      template<CT::NoIntent T> requires CT::POD<DeextAll<T>>
      static Bytes FromPOD(T&& data) noexcept {
         Bytes result;
         if constexpr (CT::Array<T>)
            result.SetHeapInner(static_cast<const void*>( data));
         else
            result.SetHeapInner(static_cast<const void*>(&data));
         result.SetCountInner(sizeof(T));
         return result;
      }

      /// Conversion to standard string as a sequence of hex bytes            
      explicit operator ::std::string() const {
         if (this->IsEmpty())
            return {};

         ::std::string result;
         result.resize(this->GetCount() * 2);
         auto from_bytes = this->template GetRawAs<uint8_t>();
         auto to_bytes = result.data();
         for (size_t i = 0; i < result.size(); ++i)
            ::fmt::format_to(to_bytes + i * 2, ::fmt::runtime("{:02X}"), from_bytes[i]);
         return result;
      }

      template<Cid> void GetResolved()         = delete;
      template<Cid> void GetDense(size_t = -1) = delete;
   };
   #pragma pack(pop)
}

/// Convert POD -> Bytes (this includes pointers as well)                     
LANGULUS_MORPHISM_CONCEPT(Langulus::CT::POD, Langulus::Annies::Bytes);

#include "SerializeBytes.hpp"