///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../many/TMany.hpp"
#include "../pairs/TPair.hpp"


namespace Langulus
{
   namespace A
   {

      ///                                                                     
      /// An abstract Map structure                                           
      /// It defines the size for CT::Map concept                             
      ///                                                                     
      struct BlockMap {
         LANGULUS(ABSTRACT) true;
         LANGULUS(POD) true;

         using InfoType = ::std::uint8_t;
         using OrderType = Offset;

         static constexpr bool CTTI_Container = true;
         static constexpr bool Sequential = false;
         static constexpr Offset InvalidOffset = -1;

         // Smallest possible table size                                
         // Has to be a power-of-two                                    
         static constexpr Count MinimalAllocation = 8;

         // How many consecutive cells can be dedicated to a single     
         // hash. The bigger the number, the more serial compares may   
         // need to be performed per hash, but maps will be smaller.    
         static constexpr InfoType AllowedMisses = 64;

      protected:
         // A precomputed pointer for the info/ordering bytes           
         // Points to an offset inside mKeys allocation                 
         // Each byte represents a pair, and can be three things:       
         //    0 - the index is not used, data is not initialized       
         //    1 - the index is used, and key is where it should be     
         //   2+ - the index is used, but bucket is info-1 buckets to   
         //         the right of this index                             
         InfoType* mInfo {};

         // The block that contains the keys and info bytes             
         // Also keeps track of count and reserve                       
         Annies::Block<> mKeys;

         // The block that contains the values                          
         // Count and reserve in this block are redundant and shouldn't 
         // be used for any purpose. The benefit is, that we can access 
         // the values block without any cost via pointer arithmetic,   
         // instead of generating Block instances at runtime            
         // This incurs 8 bytes or 16 bytes of memory overhead per map, 
         // depending on architecture. Optimizing this in the future    
         // will definitely break binary compatibility, and would       
         // involve a lot of boilerplate code that duplicates Block     
         // functionality. I've decided to make the sacrifice...        
         Annies::Block<> mValues;

      public:
         constexpr BlockMap() noexcept = default;
         constexpr BlockMap(const BlockMap&) noexcept = default;
         constexpr BlockMap(BlockMap&&) noexcept = default;

         constexpr BlockMap& operator = (const BlockMap&) noexcept = default;
         constexpr BlockMap& operator = (BlockMap&&) noexcept = default;
      };

   } // namespace Langulus::A

   namespace CT
   {

      /// A reflected map type is any type that inherits BlockMap, and is     
      /// binary compatible to a BlockMap                                     
      /// Keep in mind, that sparse types are never considered CT::Map!       
      template<class... T>
      concept Map = ((DerivedFrom<T, A::BlockMap>
          and sizeof(T) == sizeof(A::BlockMap)) and ...);

      /// Check if a type is a statically typed map                           
      template<class... T>
      concept TypedMap = Map<T...> and Typed<T...>;

      /// Check if a type is a type-erased map                                
      template<class... T>
      concept TypeErasedMap = Map<T...> and ((not Typed<T>) and ...);

   } // namespace Langulus::CT

} // namespace Langulus

namespace Langulus::Annies
{

   ///                                                                        
   ///   Type-erased map block, base for all map containers                   
   ///                                                                        
   ///   This is an inner structure, that doesn't reference any memory,       
   /// only provides the functionality to do so. You can use BlockMap as a    
   /// lightweight intermediate structure for iteration of maps - it is       
   /// binary compatible with any other map, be it type-erased or not.        
   ///   Unlike std::map, accessing elements via the subscript operator []    
   /// doesn't implicitly add element, if map is mutable. This has always     
   /// been a source of many subtle bugs, and generally the idea of           
   /// completely changing the behavior of a program, by simply removing a    
   /// 'const' qualifier doesn't seem like a sound design decision in my book 
   ///                                                                        
   struct BlockMap : A::BlockMap {
      LANGULUS(ABSTRACT) false;

      using Key = void;
      using Value = void;
      using Pair = Annies::Pair;

      static constexpr bool Ownership = false;
      static constexpr bool Ordered = false;

      ///                                                                     
      ///   Construction & Assignment                                         
      ///                                                                     
      using A::BlockMap::BlockMap;
      using A::BlockMap::operator =;

   protected:
      template<CT::Map TO, template<class> class S, CT::Map FROM>
      requires CT::Intent<S<FROM>>
      void BlockTransfer(S<FROM>&&);

      template<template<class> class S, CT::Map FROM>
      requires CT::Intent<S<FROM>>
      void CloneValuesInner(S<FROM>&&);

      template<template<class> class S, CT::Map FROM>
      requires CT::Intent<S<FROM>>
      void CloneValuesReinsertInner(CT::Block auto&, S<FROM>&&);

      template<CT::Map>
      bool BranchOut();

   public:
      ///                                                                     
      ///   Capsulation                                                       
      ///                                                                     
      template<CT::Map = UnorderedMap>
      DMeta GetKeyType() const noexcept;
      template<CT::Map = UnorderedMap>
      DMeta GetValueType() const noexcept;

      template<CT::Map = UnorderedMap>
      constexpr bool IsKeyTyped() const noexcept;
      template<CT::Map = UnorderedMap>
      constexpr bool IsValueTyped() const noexcept;

      template<CT::Map = UnorderedMap>
      constexpr bool IsKeyUntyped() const noexcept;
      template<CT::Map = UnorderedMap>
      constexpr bool IsValueUntyped() const noexcept;

      template<CT::Map = UnorderedMap>
      constexpr bool IsKeyTypeConstrained() const noexcept;
      template<CT::Map = UnorderedMap>
      constexpr bool IsValueTypeConstrained() const noexcept;

      template<CT::Map = UnorderedMap>
      constexpr bool IsKeyDeep() const noexcept;
      template<CT::Map = UnorderedMap>
      constexpr bool IsValueDeep() const noexcept;

      template<CT::Map = UnorderedMap>
      constexpr bool IsKeySparse() const noexcept;
      template<CT::Map = UnorderedMap>
      constexpr bool IsValueSparse() const noexcept;

      template<CT::Map = UnorderedMap>
      constexpr bool IsKeyDense() const noexcept;
      template<CT::Map = UnorderedMap>
      constexpr bool IsValueDense() const noexcept;

      template<CT::Map = UnorderedMap>
      constexpr Size GetKeyStride() const noexcept;
      template<CT::Map = UnorderedMap>
      constexpr Size GetValueStride() const noexcept;

      template<CT::Map = UnorderedMap>
      Count GetKeyCountDeep() const noexcept;
      template<CT::Map = UnorderedMap>
      Count GetKeyCountElementsDeep() const noexcept;
      template<CT::Map = UnorderedMap>
      Count GetValueCountDeep() const noexcept;
      template<CT::Map = UnorderedMap>
      Count GetValueCountElementsDeep() const noexcept;

      constexpr auto GetKeyState() const noexcept -> DataState;
      constexpr auto GetValueState() const noexcept -> DataState;
      constexpr bool IsKeyCompressed() const noexcept;
      constexpr bool IsValueCompressed() const noexcept;
      constexpr bool IsKeyEncrypted() const noexcept;
      constexpr bool IsValueEncrypted() const noexcept;
      constexpr bool IsKeyConstant() const noexcept;
      constexpr bool IsValueConstant() const noexcept;
      constexpr auto GetCount() const noexcept -> Count;
      constexpr auto GetReserved() const noexcept -> Count;
      constexpr bool IsEmpty() const noexcept;
      constexpr bool IsValid() const noexcept;
      constexpr bool IsInvalid() const noexcept;
      constexpr bool IsAllocated() const noexcept;

      bool IsKeyMissing() const noexcept;
      bool IsValueMissing() const noexcept;
      template<CT::Map = UnorderedMap>
      bool IsKeyMissingDeep() const;
      template<CT::Map = UnorderedMap>
      bool IsValueMissingDeep() const;

      template<CT::Map = UnorderedMap>
      bool IsKeyExecutable() const noexcept;
      template<CT::Map = UnorderedMap>
      bool IsValueExecutable() const noexcept;
      template<CT::Map = UnorderedMap>
      bool IsKeyExecutableDeep() const;
      template<CT::Map = UnorderedMap>
      bool IsValueExecutableDeep() const;

      constexpr explicit operator bool() const noexcept;

      template<CT::Map>
      void Dump() const;

      template<CT::Map = UnorderedMap>
      auto& GetKeys() const noexcept;
      template<CT::Map = UnorderedMap>
      auto& GetKeys() noexcept;
      template<CT::Map = UnorderedMap>
      auto  GetVals() const noexcept;

      auto GetInfo() const noexcept -> const InfoType*;
      auto GetInfo()       noexcept -> InfoType*;
      auto GetInfoEnd() const noexcept -> const InfoType*;

   protected:
      Count GetCountDeep(const CT::Block auto&) const noexcept;
      Count GetCountElementsDeep(const CT::Block auto&) const noexcept;

   public:
      ///                                                                     
      ///   Indexing                                                          
      ///                                                                     
      template<CT::Map>
      decltype(auto) GetKey(CT::Index auto);
      template<CT::Map>
      decltype(auto) GetKey(CT::Index auto) const;
      template<CT::Map>
      decltype(auto) GetValue(CT::Index auto);
      template<CT::Map>
      decltype(auto) GetValue(CT::Index auto) const;
      template<CT::Map>
      auto GetPair (CT::Index auto);
      template<CT::Map>
      auto GetPair (CT::Index auto) const;

   protected:
      template<CT::Map, CT::Index INDEX>
      Offset SimplifyIndex(INDEX) const
      noexcept(not LANGULUS_SAFE() and CT::BuiltinInteger<INDEX>);

      template<CT::Map>
      Offset GetBucket(Offset, const CT::NoIntent auto&) const IF_UNSAFE(noexcept);
      Offset GetBucketUnknown(Offset, const Block<>&)    const IF_UNSAFE(noexcept);

      template<CT::Map>
      decltype(auto) GetRawKey(Offset) const IF_UNSAFE(noexcept);
      template<CT::Map>
      decltype(auto) GetRawKey(Offset)       IF_UNSAFE(noexcept);
      template<CT::Map>
      decltype(auto) GetKeyRef(Offset) const IF_UNSAFE(noexcept);
      template<CT::Map>
      decltype(auto) GetKeyRef(Offset)       IF_UNSAFE(noexcept);

      template<CT::Map>
      decltype(auto) GetRawVal(Offset) const IF_UNSAFE(noexcept);
      template<CT::Map>
      decltype(auto) GetRawVal(Offset)       IF_UNSAFE(noexcept);
      template<CT::Map>
      decltype(auto) GetValRef(Offset) const IF_UNSAFE(noexcept);
      template<CT::Map>
      decltype(auto) GetValRef(Offset)       IF_UNSAFE(noexcept);

      template<CT::Map>
      auto GetKeyHandle(Offset)       IF_UNSAFE(noexcept);
      template<CT::Map>
      auto GetKeyHandle(Offset) const IF_UNSAFE(noexcept);
      template<CT::Map>
      auto GetValHandle(Offset)       IF_UNSAFE(noexcept);
      template<CT::Map>
      auto GetValHandle(Offset) const IF_UNSAFE(noexcept);

   public:
      ///                                                                     
      ///   Iteration                                                         
      ///                                                                     
      template<class MAP>
      struct Iterator;

      template<CT::Map MAP>
      auto begin()       noexcept -> Iterator<MAP>;
      template<CT::Map MAP>
      auto begin() const noexcept -> Iterator<const MAP>;

      template<CT::Map MAP>
      auto last()       noexcept -> Iterator<MAP>;
      template<CT::Map MAP>
      auto last() const noexcept -> Iterator<const MAP>;

      constexpr A::IteratorEnd end() const noexcept { return {}; }

      template<bool REVERSE = false, CT::Map>
      Count ForEach(auto&&) const;

      template<bool REVERSE = false, CT::Map>
      Count ForEachKeyElement(auto&&) const;

      template<bool REVERSE = false, CT::Map>
      Count ForEachValueElement(auto&&) const;

      template<bool REVERSE = false, CT::Map>
      Count ForEachKey(auto&&...) const;

      template<bool REVERSE = false, CT::Map>
      Count ForEachValue(auto&&...) const;
   
      template<bool REVERSE = false, bool SKIP = true, CT::Map>
      Count ForEachKeyDeep(auto&&...) const;

      template<bool REVERSE = false, bool SKIP = true, CT::Map>
      Count ForEachValueDeep(auto&&...) const;

   protected:
      template<CT::Map, bool REVERSE>
      LoopControl ForEachElementInner(const CT::Block auto&, auto&&, Count&) const;

      template<CT::Map, bool REVERSE>
      LoopControl ForEachInner(const CT::Block auto&, auto&&, Count&) const;

      template<CT::Map, bool REVERSE, bool SKIP>
      LoopControl ForEachDeepInner(const CT::Block auto&, auto&&, Count&) const;

   public:
      ///                                                                     
      ///   RTTI                                                              
      ///                                                                     
      template<CT::Map THIS, CT::Data, CT::Data...>
      constexpr bool IsKey() const noexcept;
      template<CT::Map THIS>
      bool IsKey(DMeta) const noexcept;

      template<CT::Map THIS, CT::Data, CT::Data...>
      constexpr bool IsKeySimilar() const noexcept;
      template<CT::Map THIS>
      bool IsKeySimilar(DMeta) const noexcept;

      template<CT::Map THIS, CT::Data, CT::Data...>
      constexpr bool IsKeyExact() const noexcept;
      template<CT::Map THIS>
      bool IsKeyExact(DMeta) const noexcept;

      template<CT::Map THIS, CT::Data, CT::Data...>
      constexpr bool IsValue() const noexcept;
      template<CT::Map THIS>
      bool IsValue(DMeta) const noexcept;

      template<CT::Map THIS, CT::Data, CT::Data...>
      constexpr bool IsValueSimilar() const noexcept;
      template<CT::Map THIS>
      bool IsValueSimilar(DMeta) const noexcept;

      template<CT::Map THIS, CT::Data, CT::Data...>
      constexpr bool IsValueExact() const noexcept;
      template<CT::Map THIS>
      bool IsValueExact(DMeta) const noexcept;

   protected:
      template<CT::Map, CT::NoIntent, CT::NoIntent>
      void Mutate();
      template<CT::Map>
      void Mutate(DMeta, DMeta);

      template<CT::Map>
      constexpr bool IsTypeCompatibleWith(CT::Map  auto const&) const noexcept;
      template<CT::Map>
      constexpr bool IsTypeCompatibleWith(CT::Pair auto const&) const noexcept;

   public:
      ///                                                                     
      ///   Comparison                                                        
      ///                                                                     
      template<CT::Map = UnorderedMap>
      bool operator == (CT::Map  auto const&) const;
      template<CT::Map = UnorderedMap>
      bool operator == (CT::Pair auto const&) const;

      template<CT::Map = UnorderedMap>
      Hash GetHash() const;

      template<CT::Map = UnorderedMap>
      bool ContainsKey(const CT::NoIntent auto&) const;
      template<CT::Map = UnorderedMap>
      bool ContainsValue(const CT::NoIntent auto&) const;
      template<CT::Map = UnorderedMap>
      bool ContainsPair(const CT::Pair auto&) const;

      template<CT::Map = UnorderedMap>
      auto Find(const CT::NoIntent auto&) const -> Index;
      template<CT::Map THIS = UnorderedMap>
      auto FindIt(const CT::NoIntent auto&) -> Iterator<THIS>;
      template<CT::Map THIS = UnorderedMap>
      auto FindIt(const CT::NoIntent auto&) const -> Iterator<const THIS>;

      template<CT::Map = UnorderedMap>
      decltype(auto) At(const CT::NoIntent auto&);
      template<CT::Map = UnorderedMap>
      decltype(auto) At(const CT::NoIntent auto&) const;

      template<CT::Map = UnorderedMap>
      decltype(auto) operator[] (const CT::NoIntent auto&);
      template<CT::Map = UnorderedMap>
      decltype(auto) operator[] (const CT::NoIntent auto&) const;

   protected:
      template<CT::Map>
      Offset FindInner(const CT::NoIntent auto&) const;
      template<CT::Map>
      Offset FindBlockInner(const Block<>&) const;

   public:
      ///                                                                     
      ///   Memory management                                                 
      ///                                                                     
      template<CT::Map>
      void Reserve(Count);

   protected:
      /// @cond show_protected                                                
      template<CT::Map>
      void AllocateFresh(Count);
      template<CT::Map, bool REUSE>
      bool AllocateData(Count);
      template<CT::Map>
      void AllocateMore();

      template<CT::Map, bool DEEP = false>
      void Keep() const noexcept;
      template<CT::Map>
      void Free();
      /// @endcond                                                            

   public:
      ///                                                                     
      ///   Insertion                                                         
      ///                                                                     
      template<CT::Map>
      Count Insert(auto&&, auto&&);

      template<CT::Map, class T1, class T2>
      requires CT::Block<Deint<T1>, Deint<T2>>
      Count InsertBlock(T1&&, T2&&);

      template<CT::Map, class T1, class...TAIL>
      Count InsertPair(T1&&, TAIL&&...);

   protected:
      template<CT::Map>
      auto CreateKeyHandle(auto&&);
      template<CT::Map>
      auto CreateValHandle(auto&&);

      template<CT::Map>
      Size RequestKeyAndInfoSize(Count, Offset&) const IF_UNSAFE(noexcept);
      Size RequestValuesSize(Count) const IF_UNSAFE(noexcept);
      
      template<CT::Map, class KEY_SOURCE, class VAL_SOURCE>
      bool Rehash(const InfoType* oldInfo, Count oldCount, KEY_SOURCE&, VAL_SOURCE&);

      template<CT::Map>
      void ShiftPairs();

      template<CT::Map, bool CHECK_FOR_MATCH>
      Offset InsertInner(Offset, auto&&, auto&&);

      template<CT::Map, bool CHECK_FOR_MATCH, template<class> class S1, template<class> class S2, CT::Block T>
      requires CT::Intent<S1<T>, S2<T>>
      Offset InsertBlockInner(Offset, S1<T>&&, S2<T>&&);

      template<CT::Map, bool CHECK_FOR_MATCH, template<class> class S, CT::Pair T>
      requires CT::Intent<S<T>>
      Count InsertPairInner(Count, S<T>&&);

      template<CT::Map>
      Count UnfoldInsert(auto&&);

   public:
      ///                                                                     
      ///   Removal                                                           
      ///                                                                     
      template<CT::Map>
      Count RemoveKey(const CT::NoIntent auto&);
      template<CT::Map>
      Count RemoveValue(const CT::NoIntent auto&);
      template<CT::Map>
      Count RemovePair(const CT::Pair auto&);
      template<CT::Map THIS>
      auto RemoveIt(const Iterator<THIS>&) -> Iterator<THIS>;

      template<CT::Map>
      void Clear();
      template<CT::Map>
      void Reset();
      template<CT::Map>
      void Compact();

   protected:
      template<CT::Map>
      Count RemoveKeyInner(const CT::NoIntent auto&);
      template<CT::Map>
      Count RemoveValInner(const CT::NoIntent auto&);
      template<CT::Map>
      Count RemovePairInner(const CT::Pair auto&);

      template<CT::Map>
      void RemoveInner(Offset);

   #if LANGULUS(TESTING)
      public: constexpr const void* GetRawKeysMemory() const noexcept;
      public: constexpr const void* GetRawValsMemory() const noexcept;
   #endif
   };


   ///                                                                        
   ///   Map iterator                                                         
   ///                                                                        
   template<class MAP>
   struct BlockMap::Iterator : A::Iterator {
      static_assert(CT::Map<MAP>, "MAP must be a CT::Map type");
      static constexpr bool Mutable = CT::Mutable<MAP>;

      // Key type is always constant, because changing it will mean     
      // rehashing the entire table, so we forbid it while iterating    
      using Key   = const typename MAP::Key;
      using Value = Conditional<Mutable, typename MAP::Value,
                                         const typename MAP::Value>;
      using Pair  = Conditional<Mutable, typename MAP::PairRef,
                                         typename MAP::PairConstRef>;
      using KA = Conditional<CT::TypeErased<Key>,   Block<>, Key*>;
      using VA = Conditional<CT::TypeErased<Value>, Block<>, Value*>;

      LANGULUS(ABSTRACT) false;
      LANGULUS(TYPED)    Pair;

   protected:
      KA mKey;
      VA mValue;

      friend struct BlockMap;
      const InfoType* mInfo = nullptr;
      const InfoType* mSentinel = nullptr;

      constexpr Iterator(const InfoType*, const InfoType*, const KA&, const VA&) noexcept;

   public:
      constexpr Iterator() noexcept = default;
      constexpr Iterator(const Iterator&) noexcept = default;
      constexpr Iterator(Iterator&&) noexcept = default;
      constexpr Iterator(const A::IteratorEnd&) noexcept;

      auto& GetKey() noexcept {
         if constexpr (CT::TypeErased<Key>)     return  mKey;
         else                                   return *mKey;
      }
      
      auto& GetKey() const noexcept {
         if constexpr (CT::TypeErased<Key>)     return  mKey;
         else                                   return *mKey;
      }

      auto& GetValue() noexcept {
         if constexpr (CT::TypeErased<Value>)   return  mValue;
         else                                   return *mValue;
      }
      
      auto& GetValue() const noexcept {
         if constexpr (CT::TypeErased<Value>)   return  mValue;
         else                                   return *mValue;
      }

      constexpr auto operator = (const Iterator&) noexcept -> Iterator& = default;
      constexpr auto operator = (Iterator&&)      noexcept -> Iterator& = default;

      constexpr bool operator == (const Iterator&) const noexcept;
      constexpr bool operator == (const A::IteratorEnd&) const noexcept;

      constexpr auto operator *  () const;
      constexpr auto operator -> () const noexcept { return &GetValue(); }

      // Prefix operator                                                
      constexpr auto operator ++ () noexcept -> Iterator&;

      // Suffix operator                                                
      constexpr auto operator ++ (int) noexcept -> Iterator;

      constexpr explicit operator bool() const noexcept;
      constexpr operator Iterator<const MAP>() const noexcept requires Mutable {
         return {mInfo, mSentinel, mKey, mValue};
      }
   };
}