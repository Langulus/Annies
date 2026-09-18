///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Block.hpp"


namespace Langulus::Annies
{

   /// Check if type origin is the same as one of the provided types          
   ///   @attention ignores sparsity and cv-qualifiers                        
   ///   @tparam T1, TN... - the types to compare against                     
   ///   @return true if data type is similar to at least one of the types    
   template<class TYPE> template<CT::NotVoid T1, CT::NotVoid...TN> LANGULUS(INLINED)
   constexpr bool Block<TYPE>::Is() const noexcept {
      if constexpr (TypeErased)
         return mType and mType->template Is<T1, TN...>();
      else
         return CT::SameAsOneOf<TYPE, T1, TN...>;
   }

   /// Check if type origin is the same as another                            
   ///   @attention ignores sparsity and cv-qualifiers                        
   ///   @param type - the type to check for                                  
   ///   @return true if this block contains similar data                     
   template<class TYPE> LANGULUS(INLINED)
   bool Block<TYPE>::Is(DMeta type) const noexcept {
      return GetType() | type;
   }

   /// Check if type origin is the same as another block's type origin        
   ///   @attention ignores sparsity and cv-qualifiers                        
   ///   @param other - the type to check for                                 
   ///   @return true if this block contains similar data                     
   template<class TYPE> LANGULUS(INLINED)
   bool Block<TYPE>::Is(const CT::Block auto& other) const noexcept {
      using T = Deref<decltype(other)>;
      if constexpr (TypeErased or T::TypeErased)
         return Is(other.GetType());
      else
         return Is<TypeOf<T>>();
   }

   /// Check if unqualified type is the same as one of the provided types     
   ///   @attention ignores only cv-qualifiers                                
   ///   @tparam T1, TN... - the types to compare against                     
   ///   @return true if data type is similar to at least one of the types    
   template<class TYPE> template<CT::NotVoid T1, CT::NotVoid...TN> LANGULUS(INLINED)
   constexpr bool Block<TYPE>::IsSimilar() const noexcept {
      if constexpr (TypeErased)
         return mType and mType->template IsSimilar<T1, TN...>();
      else
         return CT::SimilarAsOneOf<TYPE, T1, TN...>;
   }

   /// Check if unqualified type is the same as another                       
   ///   @attention ignores only cv-qualifiers                                
   ///   @param type - the type to check for                                  
   ///   @return true if this block contains similar data                     
   template<class TYPE> LANGULUS(INLINED)
   bool Block<TYPE>::IsSimilar(DMeta type) const noexcept {
      return GetType() & type;
   }

   /// Check if unqualified type is the same as another block's type          
   ///   @attention ignores only cv-qualifiers                                
   ///   @param other - the block to check for                                
   ///   @return true if this block contains similar data                     
   template<class TYPE> LANGULUS(INLINED)
   bool Block<TYPE>::IsSimilar(const CT::Block auto& other) const noexcept {
      using T = Deref<decltype(other)>;
      if constexpr (TypeErased or T::TypeErased)
         return IsSimilar(other.GetType());
      else
         return IsSimilar<TypeOf<T>>();
   }

   /// Check if this type is exactly one of the provided types                
   ///   @tparam T1, TN... - the types to compare against                     
   ///   @return true if data type matches at least one type                  
   template<class TYPE> template<CT::NotVoid T1, CT::NotVoid...TN> LANGULUS(INLINED)
   constexpr bool Block<TYPE>::IsExact() const noexcept {
      if constexpr (TypeErased)
         return mType and mType->template IsExact<T1, TN...>();
      else
         return CT::ExactAsOneOf<TYPE, T1, TN...>;
   }

   /// Check if this type is exactly another                                  
   ///   @param type - the type to match                                      
   ///   @return true if data type matches type exactly                       
   template<class TYPE> LANGULUS(INLINED)
   bool Block<TYPE>::IsExact(DMeta type) const noexcept {
      return GetType() == type;
   }

   /// Check if this type is exactly another block's type                     
   ///   @param other - the block to match                                    
   ///   @return true if data type matches type exactly                       
   template<class TYPE> LANGULUS(INLINED)
   bool Block<TYPE>::IsExact(const CT::Block auto& other) const noexcept {
      using T = Deref<decltype(other)>;
      if constexpr (TypeErased or T::TypeErased)
         return IsExact(other.GetType());
      else
         return IsExact<TypeOf<T>>();
   }

   /// Check if contained data can be interpreted as a given type             
   ///   @attention direction matters, if block is dense                      
   ///   @tparam BINARY_COMPATIBLE - do we require for the type to be         
   ///      binary compatible with this container's type                      
   ///   @param type - the type check if current type interprets to           
   ///   @return true if able to interpret current type to 'type'             
   template<class TYPE> template<bool BINARY_COMPATIBLE, bool ADVANCED>
   LANGULUS(INLINED) bool Block<TYPE>::CastsToMeta(DMeta type) const {
      if constexpr (TypeErased) {
         return mType and (ADVANCED or mType->mIsSparse
            ? mType->CastsTo<BINARY_COMPATIBLE, true >(type)
            : mType->CastsTo<BINARY_COMPATIBLE, false>(type));
      }
      else {
         return GetType()->template
            CastsTo<BINARY_COMPATIBLE, ADVANCED or CT::Sparse<TYPE>>(type);
      }
   }

   /// Check if contained data can be interpreted as a number of a type       
   /// For example: a Vec4 can interpret as float[4]                          
   ///   @attention direction matters, if block is dense                      
   ///   @tparam BINARY_COMPATIBLE - do we require for the type to be         
   ///      binary compatible with this container's type                      
   ///   @param type - the type check if current type interprets to           
   ///   @param count - the number of elements to interpret as                
   ///   @return true if able to interpret current type to 'type'             
   template<class TYPE> template<bool BINARY_COMPATIBLE>
   LANGULUS(INLINED) bool Block<TYPE>::CastsToMeta(DMeta type, size_t count) const {
      if constexpr (TypeErased) {
         return not mType or not type
             or mType->template CastsTo<BINARY_COMPATIBLE>(type, count);
      }
      else {
         return not type
             or GetType()->template CastsTo<BINARY_COMPATIBLE>(type, count);
      }
   }

   /// Check if this container's data can be represented as type T            
   /// with nothing more than pointer arithmetic                              
   ///   @tparam T - the type to compare against                              
   ///   @tparam BINARY_COMPATIBLE - do we require for the type to be         
   ///      binary compatible with this container's type                      
   ///   @return true if contained data is reinterpretable as T               
   template<class TYPE> template<CT::NotVoid T, bool BINARY_COMPATIBLE, bool ADVANCED>
   LANGULUS(INLINED) bool Block<TYPE>::CastsTo() const {
      //TODO can be further optimized
      return CastsToMeta<BINARY_COMPATIBLE, ADVANCED>(MetaDataOf<Decay<T>>());
   }

   /// Check if this container's data can be represented as a specific number 
   /// of elements of type T, with nothing more than pointer arithmetic       
   ///   @tparam T - the type to compare against                              
   ///   @tparam BINARY_COMPATIBLE - do we require for the type to be         
   ///      binary compatible with this container's type                      
   ///   @param count - the number of elements of T                           
   ///   @return true if contained data is reinterpretable as T               
   template<class TYPE> template<CT::NotVoid T, bool BINARY_COMPATIBLE>
   LANGULUS(INLINED) bool Block<TYPE>::CastsTo(const size_t count) const {
      //TODO can be further optimized
      return CastsToMeta<BINARY_COMPATIBLE>(MetaDataOf<Decay<T>>(), count);
   }
   
   /// Reinterpret contents of this Block as the type and state of another    
   /// You can interpret Vec4 as float[4] for example, or any other such      
   /// reinterpretation, as long as data remains tightly packed and aligned   
   /// No real conversion is performed, only pointer arithmetic               
   ///   @param pattern - the type of data to try interpreting as             
   ///   @return a block representing this block, interpreted as the pattern  
   template<class TYPE> template<CT::Block B> LANGULUS(INLINED)
   B Block<TYPE>::ReinterpretAs(const B& pattern) const {
      if (IsEmpty() or IsSparse() or IsUntyped() or pattern.IsUntyped())
         return B {};
      else if (IsSimilar(pattern))
         return reinterpret_cast<const B&>(*this);

      if constexpr (not TypeErased and not B::TypeErased) {
         // Both containers are statically typed                        
         using T = TypeOf<B>;

         if constexpr (CT::BinaryCompatible<TYPE, T>) {
            // 1:1 view for binary compatible types                     
            return B {Disown(Block<> {
               pattern.GetState(), pattern.GetType(), mCount, mRaw
            })};
         }
         else if constexpr (CT::POD<TYPE, T>) {
            if constexpr (sizeof(TYPE) >= sizeof(T)
                     and (sizeof(TYPE) %  sizeof(T)) == 0) {
               // Larger view for binary compatible types               
               return B {Disown(Block<> {
                  pattern.GetState(), pattern.GetType(),
                  mCount * (sizeof(TYPE) / sizeof(T)), mRaw
               })};
            }
            else if constexpr (sizeof(TYPE) <= sizeof(T)
                          and (sizeof(T)    %  sizeof(TYPE)) == 0) {
               // Smaller view for binary compatible types              
               return B {Disown(Block<> {
                  pattern.GetState(), pattern.GetType(),
                  mCount / (sizeof(T) / sizeof(TYPE)), mRaw
               })};
            }
            else static_assert(false, "Can't reinterpret POD types - not alignable");
         }
         else static_assert(false, "Can't reinterpret blocks - types are not binary compatible");
         //TODO add imposed base reinterprets here too, by statically scanning reflected bases?
      }
      else {
         // One of the blocks is type-erased, so do RTTI checks         
         // This also includes imposed base reinterpretations           
         // First, compare types and get a common base type if any      
         RTTI::Base common {};
         if (not CompareTypes(pattern, common) or not common.mBinaryCompatible)
            return B {};

         // Find how elements fit from one to another                   
         const size_t baseBytes = (common.mType->mSize * common.mCount * mCount)
            / pattern.GetStride();
         const size_t resultSize = pattern.IsEmpty()
            ? baseBytes : (baseBytes / pattern.mCount) * pattern.mCount;

         // Create a static view of the desired type                    
         return B {Disown(Block<> {
            pattern.mState, pattern.mType, resultSize, mRaw
         })};
      }
   }

   template<class TYPE> template<CT::NotVoid T> LANGULUS(INLINED)
   TMany<T> Block<TYPE>::ReinterpretAs() const {
      static_assert(CT::Dense<T>, "T must be dense");
      return ReinterpretAs(Block<T> {});
   }

   /// Get the memory block corresponding to a local member variable          
   ///   @attention assumes block is not empty                                
   ///   @param member - the member to get                                    
   ///   @param idx - the element to get member from                          
   ///   @return a static memory block                                        
   template<class TYPE> LANGULUS(INLINED)
   Block<> Block<TYPE>::GetMember(const RTTI::Member& member, CT::Index auto idx) {
      LglsAssumeDev(not IsEmpty(),
         "Getting member from an empty block");
      const auto index = SimplifyIndex(idx);
      return { 
         DataState::Typed, member.GetType(), member.mCount,
         member.Get(mRaw + mType->mSize * index), mEntry
      };
   }

   template<class TYPE> LANGULUS(INLINED)
   Block<> Block<TYPE>::GetMember(const RTTI::Member& member, CT::Index auto idx) const {
      auto result = const_cast<Block*>(this)->GetMember(member, idx);
      result.MakeConst();
      return result;
   }

   /// Get the memory block corresponding to a base                           
   ///   @attention assumes block is not empty                                
   ///   @param meta - the type of the resulting base block                   
   ///   @param base - the base reflection to use                             
   ///   @return the static block for the base                                
   template<class TYPE> LANGULUS(INLINED)
   Block<> Block<TYPE>::GetBaseMemory(DMeta meta, const RTTI::Base& base) {
      return {
         DataState::Typed, meta,
         base.mCount * (base.mBinaryCompatible ? GetCount() : 1),
         mRaw + base.mOffset, mEntry
      };
   }

   template<class TYPE> LANGULUS(INLINED)
   Block<> Block<TYPE>::GetBaseMemory(DMeta meta, const RTTI::Base& base) const {
      auto result = const_cast<Block*>(this)->GetBaseMemory(meta, base);
      result.MakeConst();
      return result;
   }

   /// Get the memory block corresponding to a base                           
   ///   @attention assumes block is not empty                                
   ///   @param base - the base reflection to use                             
   ///   @return the static block for the base                                
   template<class TYPE> LANGULUS(INLINED)
   Block<> Block<TYPE>::GetBaseMemory(const RTTI::Base& base) {
      return GetBaseMemory(base.mType, base);
   }

   template<class TYPE> LANGULUS(INLINED)
   Block<> Block<TYPE>::GetBaseMemory(const RTTI::Base& base) const {
      return GetBaseMemory(base.mType, base);
   }
   
   /// Mutate the block to a different type, if possible                      
   ///   @tparam T - the type to change to                                    
   ///   @tparam FORCE - insert even if types mismatch, by making this block  
   ///                   deep with provided type - use void to disable        
   ///   @return true if block was deepened to incorporate the new type       
   template<class TYPE> template<CT::NotVoid T, class FORCE> LANGULUS(INLINED)
   bool Block<TYPE>::Mutate() {
      using TT = Conditional<CT::Handle<T>, TypeOf<T>, T>;

      if constexpr (TypeErased) {
         // Do a runtime mutation                                       
         return Mutate<FORCE>(MetaDataOf<TT>()); //TODO handle<void*> force sparse block to become deep
      }
      else if constexpr (CT::Similar<TYPE, TT>) {
         // No need to mutate - types are compatible                    
         return false;
      }
      else if constexpr (not CT::Void<FORCE> and IsDeep()) {
         // Container is already deep, just make it deeper              
         Deepen<FORCE, true>();
         return true;
      }
      else LANGULUS_OOPS(Mutate, "Can't mutate to incompatible type");
   }
   
   /// Mutate to another compatible type, deepening the container if allowed  
   ///   @tparam FORCE - insert even if types mismatch, by making this block  
   ///                   deep with provided type - use void to disable        
   ///   @param meta - the type to mutate into                                
   ///   @return true if block was deepened to incorporate the new type       
   template<class TYPE> template<class FORCE>
   bool Block<TYPE>::Mutate(DMeta meta) {
      static_assert(TypeErased, "Can't change type of a typed container");

      if (IsUntyped() or (not mState.IsTyped() and mType->mIsAbstract
                          and IsEmpty() and meta->CastsTo(mType))
      ) {
         // Undefined/abstract containers can mutate freely             
         SetType<false>(meta);
      }
      else if (mType->IsSimilar(meta)) {
         // No need to mutate - types are compatible                    
         return false;
      }
      else {
         if constexpr (not CT::Void<FORCE>) {
            LANGULUS_ASSERT(not IsTypeConstrained(), Mutate,
               "Attempting to mutate type-locked container", 
               " of type ", mType, " to type ", meta
            );

            // Container is not type-constrained, so we can safely      
            // deepen it to incorporate the new data, unless it is      
            // already deep that is. We also make sure to deepen if     
            // block is deep, but sparse.                               
            if (not IsDeep() or IsSparse())
               Deepen<FORCE, true>();
            return true;
         }
         else LANGULUS_OOPS(Mutate, "Can't mutate ", mType,
            " to incompatible type ", meta);
      }

      // Block may have mutated, but it didn't happen                   
      return false;
   }
   
   /// Set the data ID - use this only if you really know what you're doing   
   ///   @tparam CONSTRAIN - whether or not to enable type-constraint         
   ///   @param type - the type meta to set                                   
   template<class TYPE> template<bool CONSTRAIN>
   void Block<TYPE>::SetType(DMeta type) requires TypeErased {
      if (mType == type) {
         if constexpr (CONSTRAIN)
            MakeTypeConstrained();
         return;
      }
      else if (not mType) {
         mType = type;
         if constexpr (CONSTRAIN)
            MakeTypeConstrained();
         return;
      }

      LANGULUS_ASSERT(not IsTypeConstrained(), Mutate,
         "Attempting to mutate type-locked container"
         " of type ", mType, " to type ", type);

      if (mType->CastsTo(type)) {
         // Type is compatible, but only sparse data can mutate freely  
         // Dense containers can't mutate because their destructors     
         // might be wrong later                                        
         LANGULUS_ASSERT(IsSparse(), Mutate, "Can't mutate ", mType,
            " to incompatible type ", type);
         mType = type;
      }
      else {
         // Type is not compatible, but container is not typed, so if   
         // it has no constructed elements, we can still mutate it      
         LANGULUS_ASSERT(IsEmpty(), Mutate, "Can't mutate ", mType,
            " to incompatible type ", type);
         mType = type;
      }

      if constexpr (CONSTRAIN)
         MakeTypeConstrained();
   }
   
   /// Set the contained data type                                            
   ///   @tparam T - the contained type                                       
   ///   @tparam CONSTRAIN - whether or not to enable type-constraints        
   template<class TYPE> template<CT::NotVoid T, bool CONSTRAIN> LANGULUS(INLINED)
   void Block<TYPE>::SetType() requires TypeErased {
      SetType<CONSTRAIN>(MetaDataOf<T>());
   }
   
   /// Reset the type of the block, unless it's type-constrained              
   /// If this block isn't type-erased, this call is a no-op                  
   template<class TYPE> LANGULUS(INLINED)
   constexpr void Block<TYPE>::ResetType() noexcept {
      if constexpr (TypeErased) {
         if (not IsTypeConstrained())
            mType = nullptr;
      }
   }

} // namespace Langulus::Annies