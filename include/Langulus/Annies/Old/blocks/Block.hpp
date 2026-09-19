///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../DataState.hpp"
#include "../Compare.hpp"
#include "../Index.hpp"
#include "../Iterator.hpp"
#include "../one/Handle.hpp"
#include "../one/Own.hpp"
#include <Langulus/Utils/Sequence.hpp>
#include <Langulus/CT/Describable.hpp>
#include <Langulus/CT/Allocatable.hpp>
#include <Langulus/CT/Meta.hpp>
#include <Langulus/Lambda.hpp>

namespace Langulus::Flow
{
   template<class TYPE>
   struct TVerb;

   template<class VERB, bool NOEXCEPT>
   struct ArithmeticVerb;
}

namespace Langulus
{
   namespace A
   {

      ///                                                                     
      /// An abstract Block structure                                         
      /// It defines the size for CT::Block and CT::Deep concepts             
      ///                                                                     
      struct Block {
         using CTTI_Abstract = Yup;
         using CTTI_Deep     = Yup;
         using CTTI_POD      = Yup;
         static constexpr bool CTTI_Container = true;

      protected:
         using DMeta = Annies::DMeta;
         using CMeta = Annies::CMeta;
         using Allocation = Annies::Allocation;
         using Allocator  = Annies::Allocator;

         union {
         #if LANGULUS(DEBUG)
            char* mRawChar;
         #endif
            // Raw pointer to first element inside the memory block     
            Byte*  mRaw {};
            Byte** mRawSparse;
         };
   
         // Number of initialized elements inside memory block          
         size_t mCount {};
         // Number of allocated elements in the memory block            
         size_t mReserved {};
         // Meta data about the elements inside the memory block        
         mutable DMeta mType {};
         // Pointer to the allocated block. If entry is zero, then data 
         // is static, or we simply have no authority over it (just a   
         // view)                                                       
         const Allocation* mEntry {};
         // The data state                                              
         DataState mState {DataState::Default};

      public:
         constexpr Block() noexcept = default;
         constexpr Block(const Block&) noexcept = default;
         constexpr Block(Block&&) noexcept = default;

         constexpr Block(DMeta) noexcept;
         constexpr Block(const DataState&, DMeta) noexcept;

         Block(const DataState&, CMeta) IF_UNSAFE(noexcept);
         Block(const DataState&, DMeta, size_t) IF_UNSAFE(noexcept);
         Block(const DataState&, DMeta, size_t, const void*) IF_UNSAFE(noexcept);
         Block(const DataState&, DMeta, size_t, void*) IF_UNSAFE(noexcept);
         Block(const DataState&, DMeta, size_t, const void*, const Allocation*) IF_UNSAFE(noexcept);
         Block(const DataState&, DMeta, size_t, void*, const Allocation*) IF_UNSAFE(noexcept);

         constexpr Block& operator = (const Block&) noexcept = default;
         constexpr Block& operator = (Block&&) noexcept = default;
      };

   } // namespace Langulus::A

   namespace CT
   {

      template<class...T>
      concept Iteratable = requires (T...a) {
         {(a.begin(), ...)} -> NotVoid;
         {(a.end(),   ...)} -> NotVoid;
      };

      template<class...T>
      concept IteratableInReverse = requires (T...a) {
         {(a.rbegin(), ...)} -> NotVoid;
         {(a.rend(),   ...)} -> NotVoid;
      };

      /// Any origin type that inherits A::Block                              
      template<class...T>
      concept BlockBased = (DerivedFrom<T, A::Block> and ...);

      /// A reflected block type is any type that is BlockBased, and is       
      /// binary compatible to a Block - this is a mandatory requirement for  
      /// any CT::Deep type                                                   
      /// Keep in mind, that sparse types are never considered Block!         
      template<class...T>
      concept Block = BlockBased<T...>
          and ((sizeof(T) == sizeof(A::Block)) and ...);

      template<class...T>
      concept NotBlock = ((not Block<T>) and ...);

      template<class...T>
      concept TypedBlock = Block<T...> and Typed<T...>;

      template<class...T>
      concept UntypedBlock = Block<T...> and ((not Typed<T>) and ...);

      /// A deep type is any type with a true static member T::CTTI_Deep,     
      /// is binary compatible with Block, and having the same interface      
      /// If no such member/base exists, the type is assumed NOT deep by      
      /// default. Deep types are considered iteratable, and verbs are        
      /// executed in each of their elements/members, instead on the type     
      /// itself. Use LANGULUS(DEEP) macro as member to tag deep types        
      /// Keep in mind, that sparse types are never considered Deep!          
      //template<class...T>
      //concept Deep = ((Block<T> and Decay<T>::CTTI_Deep) and ...);

      /// Check if Ts can be deepened 'WITH' the provided type                
      template<class WITH, class...T>
      concept CanBeDeepened = Deep<T...> and not CT::Void<WITH>
          and ((not CT::Typed<T> or Same<WITH, TypeOf<T>>) and ...);

      /// Type that is not deep, see CT::Deep                                 
      //template<class...T>
      //concept Flat = ((not Deep<T>) and ...);

      /// Check if origin of T(s) are Neat(s)                                 
      template<class...T>
      concept Neat = ((Exact<Decay<T>, Annies::Neat>) and ...);

      /// Check if origin of T(s) aren't Neat(s)                              
      template<class...T>
      concept Messy = ((not Neat<T>) and ...);

      /// Check if origin of T(s) are Construct(s)                            
      template<class...T>
      concept Construct = ((Exact<Decay<T>, Annies::Construct>) and ...);

      /// Check if origin of T(s) aren't Construct(s)                         
      template<class...T>
      concept NotConstruct = ((not Construct<T>) and ...);

      /// A serializer is any Block type, that has an inner type called       
      /// SerializationRules, which holds settings on how data is assembled   
      template<class...T>
      concept Serial = Block<T...> and ((requires {
         typename T::SerializationRules; }) and ...);

      
      namespace Inner
      {

         /// Unfolds T, if it is a bounded array, or std::range, and returns  
         /// a nullptr pointer of the type contained inside. Nested for       
         /// ranges containing other ranges, or arrays containing ranges.     
         /// Removes intents and handles, too.                                
         ///   @tparam T - type to unfold                                     
         ///   @tparam UNLESS - stop unfolding if the type is similar         
         ///      useful in cases when you actually want to insert a std::map 
         ///      for example, and not unfold it down to pairs                
         ///   @return a pointer of the most inner type                       
         template<class T, class UNLESS = void>
         consteval auto Unfold() {
            if constexpr (Same<T, UNLESS>)
               return (Deref<Deint<T>>*) nullptr;
            else if constexpr (CT::Sparse<Deint<T>>) {
               if constexpr (CT::Array<Deint<T>>)
                  return Unfold<Deext<Deint<T>>>();
               else
                  return (Deref<Deint<T>>*) nullptr;
            }
            else if constexpr (CT::Handle<Deint<T>>)
               return (TypeOf<Deint<T>>*) nullptr;
            else if constexpr (::std::ranges::range<Deint<T>>)
               return Unfold<TypeOf<Deint<T>>>();
            else
               return (Deref<Deint<T>>*) nullptr;
         }

      } // namespace Langulus::CT::Inner
      
      /// Nest-unfold any bounded array or std::range, and get most inner type
      ///   @tparam T - type to unfold                                        
      ///   @tparam UNLESS - stop unfolding if the type is similar            
      ///      useful in cases when you actually want to insert a std::map    
      ///      for example, and not unfold it down to pairs                   
      template<class T, class UNLESS = void>
      using Unfold = Deptr<decltype(Inner::Unfold<T, UNLESS>())>;

      /// Check if T is constructible with each of the provided arguments,    
      /// either directly, or by unfolding that argument                      
      template<class T, class...A>
      concept UnfoldMakableFrom = ((not Same<A, Describe> and (
               ::std::constructible_from<T, A>
            or ::std::constructible_from<T, Unfold<A>>
         )) and ...) or DescribeConstructible<T>;

      /// Check if T is assignable with the provided argument,                
      /// either directly, or by unfolding that argument                      
      template<class T, class A>
      concept UnfoldAssignableFrom = not Same<A, Describe> and (
              ::std::assignable_from<T&, A>
           or ::std::assignable_from<T&, Unfold<A>>);

      /// Check if T is insertable to containers, either directly, or while   
      /// wrapped in an intent                                                
      template<class...TN>
      concept UnfoldInsertable = ((Reflectable<Deref<Deint<TN>>> or Handle<Deint<TN>>) and ...);

      namespace Inner
      {

         /// Test whether a TMany is constructible with the given arguments   
         ///   @tparam T - the contained type in TMany<T>                     
         ///   @tparam ...A - the arguments to test                           
         ///   @return true if TMany<T> is constructible using {A...}         
         template<class T, class...A>
         consteval bool DeepMakable() noexcept {
            using FA = FirstOf<A...>;
            using SA = IntentOf(LglsFake(FA));

            if constexpr (not CT::Typed<T>) {
               // Type-erased containers accept almost any type - they  
               // will report errors at runtime instead, if any         
               return UnfoldInsertable<A...>;
            }
            else if constexpr (sizeof...(A) == 1 and CT::Block<Deint<FA>>) {
               // If only one A provided, it HAS to be a CT::Block      
               if constexpr (SA::Shallow) {
                  // Generally, shallow intents are always supported,   
                  // but copying will call element constructors, so we  
                  // have to check if the contained type supports it    
                  if constexpr (CT::Copied<SA>)
                     return ReferConstructible<T>;
                  else
                     return true;
               }
               else {
                  // Cloning always calls element constructors, and     
                  // we have to check whether contained elements can    
                  // do it                                              
                  return IntentConstructible<Langulus::Clone, T>;
               }
            }
            else return UnfoldMakableFrom<T, A...>;
         };

         /// Test whether a TMany is assignable with the given argument       
         ///   @tparam T - the contained type in TMany<T>                     
         ///   @tparam A - the argument to test                               
         ///   @return true if TMany<T> is assignable using = A               
         template<class T, class A>
         consteval bool DeepAssignable() noexcept {
            using SA = IntentOf(LglsFake(A));

            if constexpr (not CT::Typed<T>) {
               // Type-erased containers accept almost any type - they  
               // will report errors at runtime instead, if any         
               return UnfoldInsertable<A>;
            }
            else if constexpr (CT::Block<Deint<A>>) {
               if constexpr (SA::Shallow) {
                  // Generally, shallow intents are always supported,   
                  // but copying will call element assigners, so we     
                  // have to check if the contained type supports it    
                  if constexpr (CT::Copied<SA>)
                     return ReferAssignable<T>;
                  else
                     return true;
               }
               else {
                  // Cloning always calls element assigners, and we     
                  // have to check whether contained elements can do it 
                  return IntentAssignable<Langulus::Clone, T>;
               }
            }
            else return UnfoldAssignableFrom<T, A>;
         };

      } // namespace Langulus::CT::Inner

      /// Concept for recognizing arguments, with which a statically typed    
      /// container can be constructed                                        
      template<class T, class...A>
      concept DeepMakable = Inner::DeepMakable<T, A...>();

      /// Concept for recognizing argument, with which a statically typed     
      /// container can be assigned                                           
      template<class T, class A>
      concept DeepAssignable = Inner::DeepAssignable<T, A>();

   } // namespace Langulus::CT

} // namespace Langulus


namespace Langulus::Annies
{

   template<class>
   struct TBlockIterator;

   #if LANGULUS_FEATURE(COMPRESSION)
      /// Compression types, analogous to zlib's                              
      enum class Compression {
         None = 0,
         Fastest = 1,
         Balanced = 5,
         Smallest = 9,
      
         Default = Fastest
      };
   #endif

   /// A reference to an embedded sparse element inside a block               
   template<CT::Sparse T>
   struct LocalRef {
      T*                 mPointer;
      Allocation const** mEntry;

      LANGULUS(ALWAYS_INLINED)
      operator T () const noexcept { return *mPointer; }

      auto operator = (T newPointer) noexcept -> LocalRef&;

      template<class...A>
      auto New(A&&...) -> LocalRef&;

      LANGULUS(ALWAYS_INLINED)
      auto operator -> () const noexcept { return *mPointer; }

      LANGULUS(ALWAYS_INLINED)
      decltype(auto) operator * () const IF_UNSAFE(noexcept) {
         LglsAssumeUser(*mPointer, "Dereferening null pointer");
         return **mPointer;
      }

      LANGULUS(ALWAYS_INLINED)
      decltype(auto) operator & () const noexcept {
         return mPointer;
      }
   };

   ///                                                                        
   ///	Memory block                                                         
   ///                                                                        
   ///   Wraps an allocated memory block; acts as base to all containers.     
   ///   This is an inner structure, that doesn't reference any memory,       
   /// only provides the functionality to do so. You can use Block as a       
   /// lightweight intermediate structure for iteration, etc.                 
   ///                                                                        
   template<class TYPE>
   struct Block : A::Block {
      using CTTI_Typed = TYPE;
      using CTTI_Abstract = No;
      using CTTI_ReflectAs = Block<void>;
      using CTTI_Bases = A::Block;

      static constexpr bool Ownership  = false;
      static constexpr bool Sequential = true;
      static constexpr bool TypeErased = CT::Void<TYPE>;
      static constexpr bool Sparse     = CT::Sparse<TYPE>;
      static constexpr bool Dense      = not Sparse;

      template<class>
      friend struct Block;

      template<class>
      friend struct TBlockIterator;

      friend struct Many;
      template<CT::NotVoid>
      friend class TMany;

      friend struct BlockMap;
      template<bool>
      friend struct Map;
      template<CT::NotVoid, CT::NotVoid, bool>
      friend struct TMap;

      friend struct BlockSet;
      template<bool>
      friend struct Set;
      template<CT::NotVoid, bool>
      friend struct TSet;

      friend struct Bytes;
      friend struct Text;
      friend struct Path;

      template<CT::NotVoid>
      friend class Own;
      template<class>
      friend class Ref;

      friend class Neat;
      friend class Construct;

      friend struct ::Langulus::Flow::Verb;
      template<class>
      friend struct ::Langulus::Flow::TVerb;
      template<class, bool>
      friend struct ::Langulus::Flow::ArithmeticVerb;

      ///                                                                     
      ///   Construction & Assignment                                         
      ///                                                                     
      using A::Block::Block;

      constexpr Block(const A::Block&) noexcept;

      constexpr ~Block() {
         // A destructor is always compiled, so keep these here to      
         // avoid incomplete type errors on Block declaration           
         static_assert(TypeErased or CT::Reflectable<TYPE>,
            "Contained type must be reflectable");
         static_assert(TypeErased or CT::Allocatable<TYPE>,
            "Contained type must be allocatable");
         static_assert(not CT::Reference<TYPE>,
            "Contained type can't be a reference");
      }

      constexpr Block& operator = (const A::Block&) noexcept;

      #if LANGULUS(DEBUG)
         template<class MASK, class...MSGs>
         void TrackingReport(MASK, MSGs&&...) const;
      #endif

   protected:
      template<class T1, class...TN> requires CT::DeepMakable<TYPE, T1, TN...>
      void BlockCreate(T1&&, TN&&...);

      template<class B> requires CT::Block<Deint<B>>
      void BlockTransfer(B&&);

      template<CT::Block THIS, class T1>
      THIS& BlockAssign(T1&&) requires CT::DeepAssignable<TYPE, T1>;

      void BranchOut();

   public:
      ///                                                                     
      ///   Capsulation                                                       
      ///                                                                     
      constexpr void SetState(DataState) noexcept;
      constexpr void AddState(DataState) noexcept;
      constexpr void RemoveState(DataState) noexcept;

      constexpr explicit operator bool() const noexcept;

      bool Owns(const void*) const noexcept;
      constexpr auto GetAllocation() const noexcept -> const Allocation*;
      constexpr size_t GetUses() const noexcept;
      constexpr DMeta GetType() const noexcept;
      constexpr size_t GetCount() const noexcept;
      constexpr size_t GetReserved() const noexcept;
      constexpr size_t GetReservedSize() const noexcept;
      size_t GetCountDeep() const noexcept;
      size_t GetCountElementsDeep() const noexcept;
      constexpr bool IsAllocated() const noexcept;
      constexpr bool IsPast() const noexcept;
      constexpr bool IsFuture() const noexcept;
      constexpr bool IsNow() const noexcept;
      constexpr bool IsMissing() const noexcept;
      constexpr bool IsTyped() const noexcept;
      constexpr bool IsUntyped() const noexcept;
      constexpr bool IsTypeConstrained() const noexcept;
      constexpr bool IsEncrypted() const noexcept;
      constexpr bool IsCompressed() const noexcept;
      constexpr bool IsConstant() const noexcept;
      constexpr bool IsMutable() const noexcept;
      constexpr bool IsStatic() const noexcept;
      constexpr bool IsOr() const noexcept;
      constexpr bool IsEmpty() const noexcept;
      constexpr bool IsValid() const noexcept;
      constexpr bool IsInvalid() const noexcept;
      constexpr bool IsDense() const noexcept;
      constexpr bool IsSparse() const noexcept;
      constexpr bool IsPOD() const noexcept;
      constexpr bool IsResolvable() const noexcept;
      constexpr bool IsDeep() const noexcept;
      constexpr bool IsBlock() const noexcept;
      constexpr bool CanFitState(const CT::Block auto&) const noexcept;
      constexpr size_t GetBytesize() const noexcept;
      constexpr Token GetToken() const noexcept;
      constexpr size_t GetStride() const noexcept;
      constexpr DataState GetState() const noexcept;
      constexpr DataState GetUnconstrainedState() const noexcept;
      constexpr bool IsMissingDeep() const;
      constexpr bool IsConcatable(const CT::Block auto&) const noexcept;

      template<CT::NotVoid>
      constexpr bool IsInsertable() const noexcept;
      constexpr bool IsInsertable(DMeta) const noexcept;

      bool IsExecutable() const noexcept;
      bool IsExecutableDeep() const noexcept;

      constexpr void MakeConst(bool enable = true) noexcept;
      constexpr void MakeTypeConstrained(bool enable = true) noexcept;
      constexpr void MakeOr() noexcept;
      constexpr void MakeAnd() noexcept;
      constexpr void MakePast() noexcept;
      constexpr void MakeFuture() noexcept;
      constexpr void MakeNow() noexcept;
      constexpr void MakeMissing() noexcept;
      constexpr void ResetState() noexcept;
	  
      template<class T = TYPE>
      auto GetRaw()          IF_UNSAFE(noexcept) -> T*;
      template<class T = TYPE>
      auto GetRaw()    const IF_UNSAFE(noexcept) -> T const*;
      template<class T = TYPE>
      auto GetRawEnd() const IF_UNSAFE(noexcept) -> T const*;
      
      auto GetEntries() const IF_UNSAFE(noexcept) -> Allocation const* const*;
      auto GetEntries()       IF_UNSAFE(noexcept) -> Allocation const**;

      ///                                                                     
      ///   Descriptor interface                                              
      ///                                                                     
      template<class, CT::NotVoid D>
      void SetDefaultTrait(D&&);

      template<class...>
      bool ExtractTrait(CT::NotVoid auto&...) const;
      auto ExtractData(CT::NotVoid auto&) const -> size_t;

      // Intentionally undefined, because it requires Langulus::Flow    
      // and relies on Verbs::Interpret                                 
      // If you receive missing externals, include the following:       
      //    #include <Flow/Verbs/Interpret.hpp>                         
      auto ExtractDataAs(CT::NotVoid auto&) const -> size_t;

      template<CT::NotVoid>
      auto FindType()      const -> DMeta;
      auto FindType(DMeta) const -> DMeta;

      void SetTrait(auto&&, size_t = 0);

   protected:
      template<class>
      bool ExtractTraitInner(CT::NotVoid auto&...) const;
      template<class, size_t...IDX>
      bool ExtractTraitInner(ExpandedSequence<IDX...>, CT::NotVoid auto&...) const;
      template<class, size_t>
      bool ExtractTraitInnerInner(CT::NotVoid auto&) const;

   public:
      ///                                                                     
      ///   Indexing                                                          
      ///                                                                     
      decltype(auto) operator[] (CT::Index auto);
      decltype(auto) operator[] (CT::Index auto) const;

      template<CT::NotVoid>
      decltype(auto) As(CT::Index auto);
      template<CT::NotVoid>
      decltype(auto) As(CT::Index auto) const;

      template<CT::NotVoid T> LANGULUS(ALWAYS_INLINED)
      decltype(auto) As() {
         return As<T>(0);
      }

      template<CT::NotVoid T> LANGULUS(ALWAYS_INLINED)
      decltype(auto) As() const {
         return As<T>(0);
      }

      decltype(auto) Last();
      decltype(auto) Last() const;

      // Intentionally undefined, because it requires Langulus::Flow    
      // and relies on Verbs::Interpret                                 
      // If you receive missing externals, include the following:       
      //    #include <Flow/Verbs/Interpret.hpp>                         
      template<CT::NotVoid T, bool FATAL_FAILURE = true>
      T AsCast(CT::Index auto) const;

      template<CT::NotVoid T, bool FATAL_FAILURE = true> LANGULUS(INLINED)
      T AsCast() const {
         return AsCast<T, FATAL_FAILURE>(0);
      }
   
      template<CT::Block THIS> IF_UNSAFE(constexpr)
      THIS Select(size_t, size_t) IF_UNSAFE(noexcept);
      template<CT::Block THIS> IF_UNSAFE(constexpr)
      THIS Select(size_t, size_t) const IF_UNSAFE(noexcept);

      template<size_t = -1>
      auto GetElementDense(size_t = 0) -> Block<>;
      template<size_t = -1>
      auto GetElementDense(size_t = 0) const -> Block<>;
   
      auto GetElementResolved(size_t = 0)       -> Block<>;
      auto GetElementResolved(size_t = 0) const -> Block<>;

      auto GetElement(size_t = 0)       IF_UNSAFE(noexcept) -> Block<>;
      auto GetElement(size_t = 0) const IF_UNSAFE(noexcept) -> Block<>;
   
      auto GetBlockDeep(size_t)       noexcept -> Block<>*;
      auto GetBlockDeep(size_t) const noexcept -> Block<> const*;
   
      auto GetElementDeep(size_t)       noexcept -> Block<>;
      auto GetElementDeep(size_t) const noexcept -> Block<>;

      auto GetResolved()       -> Block<>;
      auto GetResolved() const -> Block<>;

      template<size_t = -1>
      auto GetDense() -> Block<>;
      template<size_t = -1>
      auto GetDense() const -> Block<>;

      auto operator * () -> Block<>;
      auto operator * () const -> Block<>;

      void SwapIndices(CT::Index auto, CT::Index auto);
      template<class T> requires CT::Block<Deint<T>>
      void Swap(T&&);

      template<bool REVERSE = false>
      size_t GatherFrom(const CT::Block auto&);
      template<bool REVERSE = false>
      size_t GatherFrom(const CT::Block auto&, DataState);

      template<Index>
      Index GetIndex() const IF_UNSAFE(noexcept);
      Index GetIndexMode(size_t&) const IF_UNSAFE(noexcept);
	  
      template<CT::NotVoid = TYPE> IF_UNSAFE(constexpr)
      decltype(auto) Get(size_t = 0)       IF_UNSAFE(noexcept);
      template<CT::NotVoid = TYPE> IF_UNSAFE(constexpr)
      decltype(auto) Get(size_t = 0) const IF_UNSAFE(noexcept);
   
      IF_UNSAFE(constexpr)
      decltype(auto) GetDeep(size_t = 0)       IF_UNSAFE(noexcept);
      IF_UNSAFE(constexpr)
      decltype(auto) GetDeep(size_t = 0) const IF_UNSAFE(noexcept);

   protected: 
      Block<> GetElementInner(size_t = 0)       IF_UNSAFE(noexcept);
      Block<> GetElementInner(size_t = 0) const IF_UNSAFE(noexcept);

      IF_UNSAFE(constexpr)
      auto At(size_t = 0) IF_UNSAFE(noexcept) -> Byte*;
      IF_UNSAFE(constexpr)
      auto At(size_t = 0) const IF_UNSAFE(noexcept) -> Byte const*;
   
      Index Constrain(Index) const IF_UNSAFE(noexcept);
      Block CropInner(size_t, size_t) const IF_UNSAFE(noexcept);

      template<bool SAFE = true, CT::Index INDEX>
      size_t SimplifyIndex(INDEX) const
      noexcept(not LANGULUS_SAFE() and CT::Integer<INDEX>);

   public:
      template<class = TYPE>
      auto GetHandle(size_t = 0)       IF_UNSAFE(noexcept);
      template<class = TYPE>
      auto GetHandle(size_t = 0) const IF_UNSAFE(noexcept);
   
      ///                                                                     
      ///   Iteration                                                         
      ///                                                                     
      using Iterator      = TBlockIterator<Block>;
      using ConstIterator = TBlockIterator<const Block>;

      constexpr auto begin()       noexcept -> Iterator;
      constexpr auto begin() const noexcept -> ConstIterator;
      constexpr auto last()       noexcept -> Iterator;
      constexpr auto last() const noexcept -> ConstIterator;

      constexpr A::IteratorEnd end() const noexcept { return {}; }

      template<bool REVERSE = false, bool MUTABLE = false>
      size_t ForEachElement(auto&&) const;
      template<bool REVERSE = false>
      size_t ForEachElement(auto&&);

      template<bool MUTABLE = false>
      size_t ForEachElementRev(auto&&...) const;
      size_t ForEachElementRev(auto&&...);

      template<bool REVERSE = false, bool MUTABLE = false>
      size_t ForEach(auto&&...) const;
      template<bool REVERSE = false>
      size_t ForEach(auto&&...);

      template<bool MUTABLE = false>
      size_t ForEachRev(auto&&...) const;
      size_t ForEachRev(auto&&...);

      template<bool REVERSE = false, bool SKIP = true, bool MUTABLE = false>
      size_t ForEachDeep(auto&&...) const;
      template<bool REVERSE = false, bool SKIP = true>
      size_t ForEachDeep(auto&&...);

      template<bool SKIP = true, bool MUTABLE = false>
      size_t ForEachDeepRev(auto&&...) const;
      template<bool SKIP = true>
      size_t ForEachDeepRev(auto&&...);

   protected:
      template<class F>
      static constexpr bool NoexceptIterator = not LANGULUS_SAFE()
         and noexcept(Fake<F&&>().operator() (Fake<ArgumentOf<F>>()));

      template<bool MUTABLE, bool REVERSE>
      LoopControl ForEachInner(auto&& f, size_t&) const noexcept(NoexceptIterator<decltype(f)>);

      template<bool MUTABLE, bool REVERSE, bool SKIP>
      LoopControl ForEachDeepInner(auto&&, size_t&) const;

      template<bool MUTABLE, bool REVERSE>
      LoopControl IterateInner(size_t, auto&& f) const noexcept(NoexceptIterator<decltype(f)>);

      // Prefix operators                                               
      auto operator ++ ()       IF_UNSAFE(noexcept) -> Block&;
      auto operator ++ () const IF_UNSAFE(noexcept) -> Block const&;
      auto operator -- ()       IF_UNSAFE(noexcept) -> Block&;
      auto operator -- () const IF_UNSAFE(noexcept) -> Block const&;

      // Suffix operators                                               
      auto operator ++ (int) const IF_UNSAFE(noexcept) -> Block;
      auto operator -- (int) const IF_UNSAFE(noexcept) -> Block;

      auto operator +  (size_t) const IF_UNSAFE(noexcept) -> Block;
      auto operator -  (size_t) const IF_UNSAFE(noexcept) -> Block;

      auto operator += (size_t)       IF_UNSAFE(noexcept) -> Block&;
      auto operator += (size_t) const IF_UNSAFE(noexcept) -> Block const&;
      auto operator -= (size_t)       IF_UNSAFE(noexcept) -> Block&;
      auto operator -= (size_t) const IF_UNSAFE(noexcept) -> Block const&;

   public:
      ///                                                                     
      ///   RTTI                                                              
      ///                                                                     
      template<CT::NotVoid, CT::NotVoid...>
      constexpr bool Is() const noexcept;
      bool Is(DMeta) const noexcept;
      bool Is(const CT::Block auto&) const noexcept;

      template<CT::NotVoid, CT::NotVoid...>
      constexpr bool IsSimilar() const noexcept;
      bool IsSimilar(DMeta) const noexcept;
      bool IsSimilar(const CT::Block auto&) const noexcept;

      template<CT::NotVoid, CT::NotVoid...>
      constexpr bool IsExact() const noexcept;
      bool IsExact(DMeta) const noexcept;
      bool IsExact(const CT::Block auto&) const noexcept;

      template<bool BINARY_COMPATIBLE = false, bool ADVANCED = false>
      bool CastsToMeta(DMeta) const;
      template<bool BINARY_COMPATIBLE = false>
      bool CastsToMeta(DMeta, size_t) const;

      template<CT::NotVoid, bool BINARY_COMPATIBLE = false, bool ADVANCED = false>
      bool CastsTo() const;
      template<CT::NotVoid, bool BINARY_COMPATIBLE = false>
      bool CastsTo(size_t) const;

      template<CT::Block B>
      B ReinterpretAs(const B&) const;
      template<CT::NotVoid T>
      TMany<T> ReinterpretAs() const;

      Block<> GetMember(const RTTI::DefinitionData::Member&, CT::Index auto);
      Block<> GetMember(const RTTI::DefinitionData::Member&, CT::Index auto) const;
   
      template<bool CONSTRAIN = false>
      void SetType(DMeta) requires TypeErased;
      template<CT::NotVoid, bool CONSTRAIN = false>
      void SetType() requires TypeErased;

   protected:
      template<CT::NotVoid, class FORCE = Many>
      bool Mutate();
      template<class FORCE = Many>
      bool Mutate(DMeta);

      constexpr void ResetType() noexcept;

   public:
      Block<> GetBaseMemory(DMeta, const RTTI::DefinitionData::Base&);
      Block<> GetBaseMemory(DMeta, const RTTI::DefinitionData::Base&) const;
      Block<> GetBaseMemory(const RTTI::DefinitionData::Base&);
      Block<> GetBaseMemory(const RTTI::DefinitionData::Base&) const;

      ///                                                                     
      ///   Comparison                                                        
      ///                                                                     
      bool operator == (const CT::Block auto&) const;

      template<CT::NotBlock T1>
      bool operator == (const T1&) const
      requires (TypeErased or CT::Comparable<TYPE, T1>);

      template<bool RESOLVE = true>
      bool Compare(const CT::Block auto&) const;
      Hash GetHash() const requires (TypeErased or CT::Hashable<TYPE>);

      template<bool REVERSE = false, CT::NoIntent T1>
      Index Find(const T1&, size_t = 0) const noexcept
      requires (TypeErased or CT::Comparable<TYPE, T1>);

      auto FindIt(const CT::NoIntent auto&)       -> Iterator;
      auto FindIt(const CT::NoIntent auto&) const -> ConstIterator;

      template<bool REVERSE = false>
      Index FindBlock(const CT::Block auto&, CT::Index auto) const noexcept;

      template<bool ASCEND = false>
      void Sort() requires (TypeErased or CT::Comparable<TYPE, TYPE>);

      bool  CompareLoose(const CT::Block auto&) const noexcept;
      size_t Matches(const CT::Block auto&) const noexcept;
      size_t MatchesLoose(const CT::Block auto&) const noexcept;
      bool  Contains(const CT::NoIntent auto&) const;

   protected:
      bool CompareSingleValue(const CT::NoIntent auto&) const;
      bool CompareStates(const Block&) const noexcept;
      bool CompareTypes(const CT::Block auto&, RTTI::DefinitionData::Base&) const;
      bool CallComparer(const Block&, const RTTI::DefinitionData::Base&) const;

      template<bool REVERSE = false>
      size_t GatherInner(CT::Block auto&) const;
      template<bool REVERSE = false>
      size_t GatherPolarInner(DMeta, CT::Block auto&, DataState) const;

   public:
      ///                                                                     
      ///   Memory management                                                 
      ///                                                                     
      template<bool SETSIZE = false>
      void Reserve(size_t);
      void TakeAuthority();

   protected:
      /// @cond show_protected                                                
      struct AllocationRequest {
         
      };

      auto RequestSize(size_t) const IF_UNSAFE(noexcept) -> AllocationRequest;

      template<bool CREATE = false, bool SETSIZE = false>
      void AllocateMore(size_t);
      void AllocateLess(size_t);

      template<bool CREATE = false>
      void AllocateInner(size_t);
      void AllocateFresh(const AllocationRequest&);

      template<bool DEEP = false>
      void Keep() const noexcept;
      template<class MASK = std::nullptr_t>
      void KeepInner(MASK = {}) const noexcept;

      void Free();
      template<bool DESTROY = true, class MASK = std::nullptr_t>
      void FreeInner(MASK = {});
      template<class MASK>
      void FreeInnerSparse(MASK);

      constexpr void ResetMemory() noexcept;
      /// @endcond                                                            

   public:
      ///                                                                     
      ///   Insertion                                                         
      ///                                                                     
      template<class FORCE = Many, bool MOVE_ASIDE = true, class T1, class...TN>
      size_t Insert(CT::Index auto, T1&&, TN&&...)
      requires (TypeErased or CT::UnfoldMakableFrom<TYPE, T1, TN...>);

      template<class FORCE = Many, bool MOVE_ASIDE = true, class T>
      requires CT::Block<Deint<T>>
      size_t InsertBlock(CT::Index auto, T&&);

      template<class FORCE = Many, bool MOVE_ASIDE = true, class T1, class...TN>
      size_t Merge(CT::Index auto, T1&&, TN&&...)
      requires (TypeErased or CT::UnfoldMakableFrom<TYPE, T1, TN...>);

      template<class FORCE = Many, bool MOVE_ASIDE = true, class T>
      requires CT::Block<Deint<T>>
      size_t MergeBlock(CT::Index auto, T&&);
   
      template<bool MOVE_ASIDE = true, class...A>
      decltype(auto) Emplace(CT::Index auto, A&&...)
      requires (TypeErased or ::std::constructible_from<TYPE, A...>);

      template<class...A>
      size_t New(size_t, A&&...)
      requires (TypeErased or ::std::constructible_from<TYPE, A...>);

      size_t New(size_t = 1) requires (TypeErased or CT::Defaultable<TYPE>);

      template<bool CONCAT = true, class FORCE = Many>
      size_t SmartPush(CT::Index auto, auto&&, DataState = {});

      template<CT::Deep T, bool TRANSFER_OR = true>
      T& Deepen();

      void Null(size_t);

      template<class A>
      void Fill(A&&) requires (TypeErased or CT::AssignableFrom<TYPE, A>);

      template<CT::Block THIS>
      THIS Extend(size_t);

   protected:
      template<class FORCE, bool MOVE_ASIDE>
      void InsertInner(CT::Index auto, auto&&);

      template<class FORCE, bool MOVE_ASIDE, class T> requires CT::Block<Deint<T>>
      void InsertBlockInner(CT::Index auto, T&&);

      template<class FORCE, bool MOVE_ASIDE>
      size_t UnfoldInsert(CT::Index auto, auto&&);
      template<class FORCE, bool MOVE_ASIDE>
      size_t UnfoldMerge(CT::Index auto, auto&&);

      template<class FORCE, class T> requires CT::Deep<Deint<T>>
      size_t SmartConcat(const CT::Index auto, bool, T&&, DataState);

      template<class FORCE>
      size_t SmartPushInner(const CT::Index auto, auto&&, DataState);

      template<CT::Block THIS, class T> requires CT::Block<Deint<T>>
      THIS ConcatBlock(T&&) const;

      void CreateDefault();

      template<class...A>
      void CreateDescribe(A&&...);

      template<class...A>
      void Create(A&&...);

      template<bool REVERSE = false, class T> requires CT::Block<Deint<T>>
      void CreateWithIntent(T&&);

      template<class T> requires CT::Handle<Deint<T>>
      void CreateWithIntent(T&&);

      template<class T> requires CT::Block<Deint<T>>
      void ShallowBatchPointerConstruction(T&&);

   public:
      template<class T>
      void AssignWithIntent(T&&) requires CT::Block<Deint<T>>;

      ///                                                                     
      ///   Removal                                                           
      ///                                                                     
      template<bool REVERSE = false>
      size_t Remove(const CT::NoIntent auto&);
      size_t RemoveIndex(CT::Index auto, size_t = 1);
      size_t RemoveIndexDeep(CT::Index auto);
      auto  RemoveIt(const Iterator&, size_t = 1) -> Iterator;

      void Trim(size_t);
      void Optimize();
      void Clear();
      void Reset();

   public:
      ///                                                                     
      ///   Compression                                                       
      ///                                                                     
      #if LANGULUS_FEATURE(COMPRESSION)
         size_t Compress(Block&, Compression = Compression::Default) const;
         size_t Decompress(Block&) const;
      #endif

      ///                                                                     
      ///   Encryption                                                        
      ///                                                                     
      size_t Encrypt(Block&, const ::std::size_t*, size_t) const;
      size_t Decrypt(Block&, const ::std::size_t*, size_t) const;

      ///                                                                     
      ///   Conversion                                                        
      ///                                                                     
      size_t Convert(CT::Block auto&) const;
      size_t Serialize(CT::Serial auto&) const;

   protected:
      #pragma pack(push, 1)
      struct Header {
         enum { Default, BigEndian };

         ::std::uint8_t  mAtomSize = sizeof(size_t);
         ::std::uint8_t  mFlags    = std::endian::native == std::endian::big ? BigEndian : Default;
         ::std::uint16_t mVersion  = 0;
         ::std::uint32_t mDefinitionCount = 0;
      };
      #pragma pack(pop)

      using Loader = void(*)(Block&, size_t);

      template<class>
      size_t SerializeToText(CT::Serial auto&) const;
      template<class>
      size_t SerializeToBinary(CT::Serial auto&) const;
      template<class, class...RULES>
      size_t SerializeByRules(CT::Serial auto&, Types<RULES...>) const;
      template<class, class RULE>
      size_t SerializeApplyRule(CT::Serial auto&) const;

      template<class>
      size_t DeserializeBinary(CT::Block auto&, const Header&, size_t = 0, Loader = nullptr) const;
      void   ReadInner(size_t, size_t, Loader) const;
      size_t DeserializeAtom(size_t&, size_t, const Header&, Loader) const;
      size_t DeserializeMeta(CT::Meta auto&, size_t, const Header&, Loader) const;
   };

   template<class BLOCK = void>
   auto MakeBlock(auto&&, size_t = 1);

   template<class BLOCK = void, CT::NotVoid...TN>
   auto WrapBlock(TN&&...);

   /// Cast between block types - does only reinterpret_cast, with some       
   /// additional safety checks. Preserves qualifiers.                        
   ///   @tparam AS - what block are we casting to?                           
   ///   @param from - block we're casting from                               
   ///   @return the reinterpreted block                                      
   template<CT::Block AS>
   decltype(auto) BlockCast(CT::Block auto&& from) {
      //TODO move all kinds of checks here instead?
      //TODO utilize cast operators to type-erased references here if available - they might set type
      using DAS  = Decay<AS>;
      using FROM = Deref<decltype(from)>;
      if constexpr (CT::Mutable<FROM>)
         return reinterpret_cast<DAS&>(from);
      else
         return reinterpret_cast<const DAS&>(from);
   }

   namespace Inner
   {

      ///                                                                     
      ///   Reverse iteration adapter                                         
      ///                                                                     
      /// Use like this: for(auto i : Reverse(container)), where              
      /// 'container' can be any CT::IteratableInReverse type                 
      ///                                                                     
      template<CT::IteratableInReverse T>
      class TReverse {
         T& mContainer;

      public:
         auto begin() { return mContainer.rbegin(); }
         auto end()   { return mContainer.rend();   }
      };


      ///                                                                     
      ///   Keep iterator when using ranged-for                               
      ///                                                                     
      /// When doing for(auto i : container), the statement always            
      /// dereferences the iterator and 'i' always ends up with the contained 
      /// type - counteract this, and make 'i' be the iterator type instead   
      /// Use like this: for(auto i : KeepIterator(container)), where         
      /// 'container' can be any CT::Iteratable type                          
      ///                                                                     
      template<CT::Iteratable T>
      class TKeepIterator {
         T& mContainer;

      public:
         TKeepIterator() = delete;
         TKeepIterator(T& a) : mContainer {a} {}

         struct WrapEnd;

         struct WrapBegin {
         protected:
            friend struct WrapEnd;
            using Type = decltype(Fake<T&>().begin());
            Type mIt;

         public:
            WrapBegin(const Type& it) : mIt {it} {}

            bool operator == (const WrapBegin& rhs) const noexcept {
               return mIt == rhs.mIt;
            }

            decltype(auto) operator *  () const noexcept { return mIt; }
            decltype(auto) operator -> () const noexcept { return mIt; }

            // Prefix operator                                          
            WrapBegin& operator ++ () noexcept { ++mIt; return *this; }

            // Suffix operator                                          
            WrapBegin operator ++ (int) noexcept { return mIt++; }
         };

         struct WrapEnd {
         private:
            using Type = decltype(Fake<T&>().end());
            Type mIt;

         public:
            WrapEnd(const Type& it) : mIt {it} {}

            bool operator == (const WrapBegin& rhs) const noexcept {
               return mIt == rhs.mIt;
            }
         };

      public:
         WrapBegin begin() { return mContainer.begin(); }
         WrapEnd   end  () { return mContainer.end();   }
      };

   }

   LANGULUS(INLINED)
   constexpr auto Reverse(CT::IteratableInReverse auto&& what) noexcept {
      return Inner::TReverse<Deref<decltype(what)>> {what};
   }

   LANGULUS(INLINED)
   constexpr auto KeepIterator(CT::Iteratable auto&& what) noexcept {
      return Inner::TKeepIterator<Deref<decltype(what)>> {what};
   }
   

   ///                                                                        
   ///   Contiguous block iterator                                            
   ///                                                                        
   template<class B>
   struct TBlockIterator : A::Iterator {
      static_assert(CT::Block<B>, "B must be a Block type");
      static constexpr bool Mutable = CT::Mutable<B>;
      using Type           = std::conditional_t<Mutable, TypeOf<B>, const TypeOf<B>>;
      using CTTI_Abstract  = No;
      using CTTI_Typed     = Type;

   protected:
      template<class>
      friend struct Block;
      template<class>
      friend struct TBlockIterator;

      using TypeInner = std::conditional_t<B::TypeErased, B, Type*>;

      // Current iterator position pointer                              
      TypeInner   mValue = nullptr;
      // Iterator position which is considered the 'end' iterator       
      Type const* mEnd = nullptr;

      constexpr TBlockIterator(const TypeInner&, Type const*) noexcept;

   public:
      constexpr TBlockIterator() noexcept = default;
      constexpr TBlockIterator(const TBlockIterator&) noexcept = default;
      constexpr TBlockIterator(TBlockIterator&&) noexcept = default;
      constexpr TBlockIterator(A::IteratorEnd) noexcept;

      constexpr auto operator = (const TBlockIterator&) noexcept -> TBlockIterator& = default;
      constexpr auto operator = (TBlockIterator&&) noexcept -> TBlockIterator& = default;

      constexpr bool operator == (const TBlockIterator&) const noexcept;
      constexpr bool operator == (A::IteratorEnd) const noexcept;

      constexpr decltype(auto) operator *  () const noexcept {
         if constexpr (CT::Typed<B>) return *mValue;
         else return (mValue);
      }

      constexpr decltype(auto) operator -> () const noexcept {
         if constexpr (CT::Typed<B>) return *mValue;
         else return &mValue;
      }

      // Prefix operator                                                
      constexpr auto operator ++ () noexcept -> TBlockIterator&;

      // Suffix operator                                                
      constexpr auto operator ++ (int) noexcept -> TBlockIterator;

      constexpr explicit operator bool() const noexcept;

      // Implicit cast to a constant iterator                           
      constexpr operator TBlockIterator<const B>() const noexcept
      requires Mutable { return {mValue, mEnd}; }
   };
}

namespace Langulus::CTTI
{
   template<CT::Sparse T>
   struct Sparse<::Langulus::Annies::LocalRef<T>> {};
}
