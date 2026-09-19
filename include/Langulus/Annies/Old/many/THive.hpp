///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "TMany.hpp"


namespace Langulus
{
   namespace A
   {

      /// An abstract hive                                                    
      struct Hive {
         LANGULUS(ABSTRACT) true;
      };

   } // namespace Langulus::A

   namespace CT
   {

      /// Hive is anything derived from A::Hive                               
      template<class...T>
      concept Hive = (DerivedFrom<T, A::Hive> and ...);

   } // namespace Langulus::CT

} // namespace Langulus

namespace Langulus::Annies
{

   ///                                                                        
   ///   Hive                                                                 
   ///                                                                        
   ///   Templated container used to contain, produce, but most importantly   
   /// reuse memory for instances of data. Elements are guaranteed to NEVER   
   /// move, and are reused in-place. Extensively used by Flow::TFactory.     
   /// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p0447r21.html 
   ///                                                                        
   template<CT::NotVoid T>
   class THive : public A::Hive {
   public:
      LANGULUS(TYPED) T;
      LANGULUS(ABSTRACT) false;

      static constexpr size_t DefaultFrameSize = 8;
      static constexpr bool Ownership = true;

   protected:
      class Cell;
      using Frame = TMany<Cell>;

      // Elements are allocated here in frames                          
      // If resizing one frame of cells requires memory to move, then   
      // another frame will be added to the sequence, guaranteeing      
      // that memory underneath any cells never moves                   
      TMany<Frame> mFrames;
      // The start of the reusable chain, in the first frame that has   
      // a free cell                                                    
      Cell* mReusable {};
      // Number of initialized elements across all frames               
      size_t mCount = 0;

   public:
      ///                                                                     
      ///   Construction                                                      
      ///                                                                     
      constexpr THive() noexcept = default;
      THive(const THive&);
      THive(THive&&);

      template<template<class> class S> requires CT::Intent<S<THive<T>>>
      THive(S<THive>&&);

      ~THive();

      ///                                                                     
      ///   Assignment                                                        
      ///                                                                     
      auto operator = (const THive&) -> THive&;
      auto operator = (THive&&) -> THive&;

      template<template<class> class S> requires CT::Intent<S<THive<T>>>
      auto operator = (S<THive>&&) -> THive&;

      ///                                                                     
      ///   Capsulation                                                       
      ///                                                                     
      auto Owns(const void*) const noexcept -> const Frame*;
      auto GetType() const noexcept -> DMeta;
      auto GetCount() const noexcept -> size_t;
      bool IsEmpty() const noexcept;
      constexpr explicit operator bool() const noexcept;

   #if LANGULUS(TESTING)
      auto  GetReusable() const { return mReusable; }
      auto& GetFrames()   const { return mFrames; }
   #endif

      ///                                                                     
      ///   Iteration                                                         
      ///                                                                     
      template<bool MUTABLE>
      struct Iterator;

      constexpr auto begin()       noexcept -> Iterator<true>;
      constexpr auto begin() const noexcept -> Iterator<false>;

      constexpr A::IteratorEnd end() const noexcept { return {}; }

      ///                                                                     
      ///   Insertion                                                         
      ///                                                                     
      template<class...A> requires ::std::constructible_from<T, A...>
      auto New(A&&...) -> T*;

   protected:
      template<class...A> requires ::std::constructible_from<T, A...>
      auto NewInner(A&&...) -> Cell*;

   public:
      ///                                                                     
      ///   Removal                                                           
      ///                                                                     
      void Destroy(Cell*);

      void Reset();

   protected:
      void ResetInner();
   };


   ///                                                                        
   ///   Hive cell (for internal usage)                                       
   ///                                                                        
   /// Intended to be allocated only dynamically, and never instantiated on   
   /// the stack, because it may contain an instance of an uninitialized T.   
   ///                                                                        
   template<CT::NotVoid T>
   class THive<T>::Cell {
   protected:
      friend class THive<T>;

      // If zero, then this cell is in use, and mData is valid          
      // If not zero, then it points to the next free cell              
      // @attention this may point beyond frame's reserved memory, if   
      // cell is the last one                                           
      Cell* mNextFreeCell {};

   public:
      // Data reserved for T's instance                                 
      T mData;

      /// Only THive is capable of creating and destroying these cells        
      Cell() = delete;

      template<class...A> requires ::std::constructible_from<T, A...>
      Cell(A&&...args) : mData(Forward<A>(args)...) {}

      /// @attention after a cell is destroyed, its mNextFreeCell must be     
      /// set to the next free cell, as this informs iterators, that the cell 
      /// isn't initialized                                                   
      ~Cell() = default;
   };


   ///                                                                        
   ///   Hive iterator                                                        
   ///                                                                        
   template<CT::NotVoid T> template<bool MUTABLE>
   struct THive<T>::Iterator {
      static constexpr bool Mutable = MUTABLE;

      LANGULUS(ABSTRACT) false;
      LANGULUS(TYPED) T;

   protected:
      friend class THive<T>;

      // Current iterator position pointer inside current frame         
      Cell* mCell;
      // Iterator position which is considered the 'end' frame iterator 
      Cell const* mCellEnd;

      // Current frame                                                  
      Frame* mFrame;
      // Frame position which is considered the 'end' all iterator      
      Frame const* mFrameEnd;

      constexpr Iterator(Cell*, Cell const*, Frame*, Frame const*) noexcept;

   public:
      Iterator() noexcept = delete;
      constexpr Iterator(const Iterator&) noexcept = default;
      constexpr Iterator(Iterator&&) noexcept = default;
      constexpr Iterator(const A::IteratorEnd&) noexcept;

      constexpr auto operator =  (const Iterator&) noexcept -> Iterator& = default;
      constexpr auto operator =  (Iterator&&) noexcept -> Iterator& = default;

      constexpr bool operator == (const Iterator&) const noexcept;
      constexpr bool operator == (const A::IteratorEnd&) const noexcept;

      constexpr decltype(auto) operator *  () const noexcept;
      constexpr decltype(auto) operator -> () const noexcept;

      // Prefix operator                                                
      constexpr auto operator ++ () noexcept -> Iterator&;

      // Suffix operator                                                
      constexpr auto operator ++ (int) noexcept -> Iterator;

      constexpr explicit operator bool() const noexcept;

      // Implicit cast to an immutable iterator is allowed, but not vice
      // versa                                                          
      constexpr operator Iterator<false>() const noexcept requires Mutable;
   };

} // namespace Langulus::Flow
