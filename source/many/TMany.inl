///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "TMany.hpp"
#include "Many.inl"
#include <cctype>

#define TEMPLATE() template<CT::NotVoid T>


namespace Langulus::Annies
{

   /// Default construction                                                   
   /// TMany is always type-constrained, but its type is set on demand to     
   /// avoid requesting meta definitions before meta database initialization, 
   /// and to significanty improve TMany initialization time (also to allow   
   /// for constexpr default construction)                                    
   TEMPLATE() LANGULUS(ALWAYS_INLINED)
   constexpr TMany<T>::TMany() {
      if constexpr (CT::Constant<T>)
         mState = DataState::Typed | DataState::Constant;
      else
         mState = DataState::Typed;
   }

   /// Refer constructor                                                      
   ///   @param other - the TMany to reference                                
   TEMPLATE() LANGULUS(ALWAYS_INLINED)
   TMany<T>::TMany(const TMany& other)
      : TMany {Refer(other)} {}
    
   /// Move constructor                                                       
   ///   @param other - the TMany to move                                     
   TEMPLATE() LANGULUS(ALWAYS_INLINED)
   TMany<T>::TMany(TMany&& other) noexcept
      : TMany {Move(other)} {}
   
   /// Create from a list of elements, an array, as well as any other kinds   
   /// of data. Each can have an individual intent or not.                    
   ///   @param t1 - first element and intent                                 
   ///   @param tn - tail of elements (optional, can have intents)            
   TEMPLATE() template<class T1, class...TN>
   requires CT::DeepMakable<T, T1, TN...> LANGULUS(INLINED)
   TMany<T>::TMany(T1&& t1, TN&&...tn) {
      Base::BlockCreate(Forward<T1>(t1), Forward<TN>(tn)...);
   }

   /// Destructor                                                             
   TEMPLATE() LANGULUS(ALWAYS_INLINED)
   TMany<T>::~TMany() {
      Base::Free();
   }

   /// Insert the provided elements, making sure to insert and never absorb   
   ///   @param items... - items to insert                                    
   ///   @returns the new container containing the data                       
   TEMPLATE() template<CT::NotVoid...TN> LANGULUS(ALWAYS_INLINED)
   auto TMany<T>::Wrap(TN&&...items) -> TMany {
      return WrapBlock<TMany>(Forward<TN>(items)...);
   }

   /// Refer assignment                                                       
   ///   @param rhs - the container to refer to                               
   ///   @return a reference to this container                                
   TEMPLATE() LANGULUS(INLINED)
   auto TMany<T>::operator = (const TMany& rhs) -> TMany& {
      static_assert(CT::DeepAssignable<T, Referred<TMany>>);
      return operator = (Refer(rhs));
   }

   /// Move assignment                                                        
   ///   @param rhs - the container to move                                   
   ///   @return a reference to this container                                
   TEMPLATE() LANGULUS(INLINED)
   auto TMany<T>::operator = (TMany&& rhs) -> TMany& {
      static_assert(CT::DeepAssignable<T, Moved<TMany>>);
      return operator = (Move(rhs));
   }

   /// Generic assignment                                                     
   ///   @param rhs - the element/array/container to assign                   
   ///   @return a reference to this container                                
   TEMPLATE() template<class T1>
   requires CT::DeepAssignable<T, T1> LANGULUS(INLINED)
   auto TMany<T>::operator = (T1&& rhs) -> TMany& {
      return Base::template BlockAssign<TMany>(Forward<T1>(rhs));
   }

   /// Insert an element at the back of the container                         
   ///   @param rhs - the element to insert                                   
   ///   @return a reference to this container for chaining                   
   TEMPLATE() template<class T1>
   requires CT::UnfoldMakableFrom<T, T1> LANGULUS(INLINED)
   auto TMany<T>::operator << (T1&& rhs) -> TMany& {
      Base::Insert(IndexBack, Forward<T1>(rhs));
      return *this;
   }

   /// Insert an element at the front of the container                        
   ///   @param rhs - the element to insert                                   
   ///   @return a reference to this container for chaining                   
   TEMPLATE() template<class T1>
   requires CT::UnfoldMakableFrom<T, T1> LANGULUS(INLINED)
   auto TMany<T>::operator >> (T1&& rhs) -> TMany& {
      Base::Insert(IndexFront, Forward<T1>(rhs));
      return *this;
   }

   /// Merge an element at the back of the container                          
   ///   @param rhs - the element to insert                                   
   ///   @return a reference to this container for chaining                   
   TEMPLATE() template<class T1>
   requires CT::UnfoldMakableFrom<T, T1> LANGULUS(INLINED)
   auto TMany<T>::operator <<= (T1&& rhs) -> TMany& {
      Base::Merge(IndexBack, Forward<T1>(rhs));
      return *this;
   }

   /// Merge an element at the front of the container                         
   ///   @param rhs - the element to insert                                   
   ///   @return a reference to this container for chaining                   
   TEMPLATE() template<class T1>
   requires CT::UnfoldMakableFrom<T, T1> LANGULUS(INLINED)
   auto TMany<T>::operator >>= (T1&& rhs) -> TMany& {
      Base::Merge(IndexFront, Forward<T1>(rhs));
      return *this;
   }

   /// Pick a constant region and reference it from another container         
   ///   @param start - starting element index                                
   ///   @param count - number of elements                                    
   ///   @return the container                                                
   TEMPLATE() LANGULUS(INLINED)
   auto TMany<T>::Select(Offset start, Count count) const IF_UNSAFE(noexcept) -> TMany {
      return Base::template Select<TMany>(start, count);
   }
   
   TEMPLATE() LANGULUS(INLINED)
   auto TMany<T>::Select(Offset start, Count count) IF_UNSAFE(noexcept) -> TMany {
      return Base::template Select<TMany>(start, count);
   }
   
   /// Extend the container via default construction, and return the new part 
   ///   @param count - the number of elements to extend by                   
   ///   @return a container that represents only the extended part           
   TEMPLATE() LANGULUS(INLINED)
   auto TMany<T>::Extend(const Count count) -> TMany {
      return Base::template Extend<TMany>(count);
   }
  
   /// Concatenate anything, with or without intents                          
   ///   @param rhs - the element/block/array and intent to concatenate with  
   ///   @return a new container, containing both blocks                      
   TEMPLATE() template<class T1>
   requires CT::DeepMakable<T, T1> LANGULUS(INLINED)
   auto TMany<T>::operator + (T1&& rhs) const -> TMany {
      using S = IntentOf<decltype(rhs)>;
      if constexpr (CT::Block<TypeOf<S>>)
         return Base::template ConcatBlock<TMany>(S::Nest(rhs));
      else
         return Base::template ConcatBlock<TMany>(Abandon(TMany {S::Nest(rhs)}));
   }

   /// Concatenate destructively, with or without intents                     
   ///   @param rhs - the element/block/array and intent to concatenate with  
   ///   @return a reference to this container                                
   TEMPLATE() template<class T1>
   requires CT::DeepMakable<T, T1> LANGULUS(INLINED)
   auto TMany<T>::operator += (T1&& rhs) -> TMany& {
      using S = IntentOf<decltype(rhs)>;
      if constexpr (CT::Block<TypeOf<S>>)
         Base::template InsertBlock<void>(IndexBack, S::Nest(rhs));
      else
         Base::template Insert<void>(IndexBack, S::Nest(rhs));
      return *this;
   }

   /// Statically typed container can always be represented by a type-erased  
   TEMPLATE()
   TMany<T>::operator Many& () const noexcept {
      // Just make sure that type member has been populated             
      (void) Base::GetType();
      return const_cast<Many&>(reinterpret_cast<const Many&>(*this));
   }

} // namespace Langulus::Annies

#undef TEMPLATE