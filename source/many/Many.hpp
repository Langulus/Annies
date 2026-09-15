///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../blocks/Block.hpp"


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
   class Many : public Block<> {
      using Base = Block<>;
      LANGULUS(POD) false;
      LANGULUS(ACT_AS) Many;
      LANGULUS_BASES(Base);

   protected:
	   template<class>
	   friend struct Block;
	   friend struct BlockSet;
	   friend struct BlockMap;
	   template<CT::Data>
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
      template<CT::Data T>
      static Many From(DataState = {}) noexcept;

      template<class AS = void, CT::Data...TN>
      static Many Wrap(TN&&...);

      template<class...>
      static Many Past() noexcept;
      template<class...>
      static Many Future() noexcept;

      #if LANGULUS_FEATURE(MANAGED_REFLECTION)
         template<CT::String...T> requires (sizeof...(T) > 0)
         static Many Past(T&&...);
         template<CT::String...T> requires (sizeof...(T) > 0)
         static Many Future(T&&...);
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
      Many Select(Offset, Count) const IF_UNSAFE(noexcept);
      Many Select(Offset, Count)       IF_UNSAFE(noexcept);

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