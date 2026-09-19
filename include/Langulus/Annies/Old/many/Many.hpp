///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../blocks/Block.hpp"
//#include <Langulus/CT/Text.hpp>


namespace Langulus::Annies
{
   
   ///                                                                        
   ///   Many                                                                 
   ///                                                                        
   ///   Equivalent to an std::vector - it can contain any number of          
   /// similarly-typed type-erased elements. It gracefully wraps sparse and   
   /// dense arrays, keeping track of static and constant data blocks.        
   ///   For a faster statically-optimized equivalent of this, use TMany      
   ///   You can always ReinterpretAs a statically optimized equivalent for   
   /// the cost of one runtime type check, because all Many variants are      
   /// binary-compatible.                                                     
   ///                                                                        
   struct Many : public Block<> {
      using Base           = Block<>;
      using CTTI_POD       = No;
      using CTTI_ReflectAs = Many;
      using CTTI_Bases     = Base;

   protected:
	   template<class>
	   friend struct Block;
	   friend struct BlockSet;
	   friend struct BlockMap;
	   template<CT::NotVoid>
	   friend class THive;
	   
	   #if LANGULUS_DEBUG()
         using Base::mRawChar;
      #endif

      using Base::mRaw;
      using Base::mRawSparse;
      using Base::mState;
      using Base::mCount;
      using Base::mReserved;
      using Base::mType;
      using Base::mEntry;

   public:
      static constexpr bool Ownership = true;

      ///                                                                     
      ///   Construction                                                      
      ///                                                                     
      constexpr Many() noexcept = default;
      Many(const Many&);
      Many(Many&&) noexcept;

      template<class T1, class...TN> requires CT::UnfoldInsertable<T1, TN...>
      Many(T1&&, TN&&...);

      ~Many();

      static Many FromMeta(DMeta, DataState = {}) noexcept;
      static Many FromBlock(const CT::Block auto&, DataState = {}) noexcept;
      static Many FromState(const CT::Block auto&, DataState = {}) noexcept;
      template<CT::NotVoid T>
      static Many From(DataState = {}) noexcept;

      template<class AS = void, CT::NotVoid...TN>
      static Many Wrap(TN&&...);

      template<class...>
      static Many Past() noexcept;
      template<class...>
      static Many Future() noexcept;

      #if LANGULUS_FEATURE(MANAGED_REFLECTION)
         static Many Past(const char*);
         static Many Future(const char*);
      #endif

      #if LANGULUS(DEBUG)
         using Base::TrackingReport;
      #endif

      ///                                                                     
      ///   Assignment                                                        
      ///                                                                     
      Many& operator = (const Many&);
      Many& operator = (Many&&) noexcept;
      Many& operator = (CT::UnfoldInsertable auto&&);

      ///                                                                     
      ///   Indexing                                                          
      ///                                                                     
      Many Select(size_t, size_t) const IF_UNSAFE(noexcept);
      Many Select(size_t, size_t)       IF_UNSAFE(noexcept);

      ///                                                                     
      ///   Comparison                                                        
      ///                                                                     
      using Block::operator==;

      ///                                                                     
      ///   Insertion                                                         
      ///                                                                     
      Many& operator <<  (CT::UnfoldInsertable auto&&);
      Many& operator >>  (CT::UnfoldInsertable auto&&);

      Many& operator <<= (CT::UnfoldInsertable auto&&);
      Many& operator >>= (CT::UnfoldInsertable auto&&);

      ///                                                                     
      ///   Concatenation                                                     
      ///                                                                     
      Many  operator +  (CT::UnfoldInsertable auto&&) const;
      Many& operator += (CT::UnfoldInsertable auto&&);
   };

} // namespace Langulus::Annies