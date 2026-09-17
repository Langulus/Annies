///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Bytes.hpp"


namespace Langulus::Annies
{

   /// Refer-constructor                                                      
   ///   @param other - container to reference                                
   LANGULUS(ALWAYS_INLINED)
   Bytes::Bytes(const Bytes& other)
      : Bytes {Refer(other)} {}

   /// Move-constructor                                                       
   ///   @param other - container to move                                     
   LANGULUS(ALWAYS_INLINED)
   Bytes::Bytes(Bytes&& other) noexcept
      : Bytes {Move(other)} {}

   /// Semantic bytes constructor                                             
   ///   @param other - the text container to use, with or without intent     
   template<class T> requires CT::Bytes<Deint<T>> LANGULUS(ALWAYS_INLINED)
   Bytes::Bytes(T&& other) {
      Base::BlockCreate(Forward<T>(other));
   }

   /// Serialize a single POD item                                            
   ///   @param item - the item to serialize                                  
   LANGULUS(INLINED)
   Bytes::Bytes(const CT::BinablePOD auto& item) {
      using T = Deref<decltype(item)>;
      constexpr auto size = sizeof(T);

      AllocateFresh(RequestSize(size));
      if constexpr (CT::Array<T>)
         CopyMemory(mRaw, reinterpret_cast<const Byte*>( item), size);
      else
         CopyMemory(mRaw, reinterpret_cast<const Byte*>(&item), size);
      mCount += size;
   }
 
   /// Serialize a meta definition                                            
   ///   @param meta - the definition to serialize                            
   LANGULUS(INLINED)
   Bytes::Bytes(const CT::Meta auto& meta) {
      constexpr auto atom = sizeof(Count);

      if (meta) {
         const auto token = meta->mToken;
         const Count tokensize = static_cast<Count>(token.size());
         const Count count = atom + tokensize;
         AllocateFresh(RequestSize(count));
         mCount = count;
         CopyMemory(GetRaw(),
            reinterpret_cast<const Byte*>(&tokensize), atom);
         CopyMemory(GetRaw() + atom,
            reinterpret_cast<const Byte*>(token.data()), tokensize);
      }
      else {
         const Count tokensize = 0;
         AllocateFresh(RequestSize(atom));
         mCount = atom;
         CopyMemory(GetRaw(),
            reinterpret_cast<const Byte*>(&tokensize), atom);
      }
   }
 
   /// Compose bytes by an arbitrary amount of binable arguments              
   ///   @param t1 - the first argument                                       
   ///   @param t2 - the second argument                                      
   ///   @param tn... - the rest of the arguments (optional)                  
   template<class T1, class T2, class...TN>
   requires CT::Inner::Binable<T1, T2, TN...> LANGULUS(ALWAYS_INLINED)
   Bytes::Bytes(T1&& t1, T2&& t2, TN&&...tn) {
        UnfoldInsert(Forward<T1>(t1));
        UnfoldInsert(Forward<T2>(t2));
      ((UnfoldInsert(Forward<TN>(tn))), ...);
   }

   /// Destructor                                                             
   inline Bytes::~Bytes() {
      Base::Free();
   }

   /// Construction from count-terminated array, with or without intent       
   ///   @param text - text memory to wrap                                    
   ///   @param count - number of characters inside text                      
   ///   @return the byte container                                           
   template<class T> requires (CT::Sparse<Deint<T>> and CT::Byte<Decay<Deint<T>>>)
   LANGULUS(ALWAYS_INLINED) Bytes Bytes::From(T&& text, Count count) {
      return MakeBlock<Bytes>(Forward<T>(text), count);
   }

   /// Refer-assignment                                                       
   ///   @param rhs - the byte container to refer to                          
   ///   @return a reference to this container                                
   LANGULUS(ALWAYS_INLINED)
   Bytes& Bytes::operator = (const Bytes& rhs) {
      static_assert(CT::DeepAssignable<Byte, Referred<Bytes>>);
      return operator = (Refer(rhs));
   }

   /// Move-assignment                                                        
   ///   @param rhs - the byte container to move                              
   ///   @return a reference to this container                                
   LANGULUS(ALWAYS_INLINED)
   Bytes& Bytes::operator = (Bytes&& rhs) {
      static_assert(CT::DeepAssignable<Byte, Moved<Bytes>>);
      return operator = (Move(rhs));
   }

   /// Assign any Bytes block                                                 
   ///   @param rhs - the block to assign                                     
   template<class T> requires CT::Bytes<Deint<T>> LANGULUS(ALWAYS_INLINED)
   Bytes& Bytes::operator = (T&& rhs) {
      return Base::BlockAssign<Bytes>(Forward<T>(rhs));
   }
   
   /// Hash the byte sequence                                                 
   ///   @return a hash of the contained byte sequence                        
   LANGULUS(INLINED)
   Hash Bytes::GetHash() const {
      return HashBytes(GetRaw(), static_cast<int>(GetCount()));
   }
   
   /// Compare with another block of bytes                                    
   /// The base Block::operator == can't be used, because Bytes arent CT::Deep
   ///   @param rhs - the block to compare with                               
   ///   @return true if blocks are the same                                  
   LANGULUS(INLINED)
   bool Bytes::operator == (const CT::Block auto& rhs) const noexcept {
      if (mCount != rhs.mCount)
         return false;
      else if (IsEmpty())
         return true;

      using T = Deref<decltype(rhs)>;
      if constexpr (CT::Bytes<T> or (CT::Typed<T> and CT::Byte<TypeOf<T>>))
         return 0 == ::std::memcmp(mRaw, rhs.mRaw, mCount);
      else if constexpr (not CT::Typed<T>) {
         if (rhs.GetType()->template IsSimilar<Byte>())
            return 0 == ::std::memcmp(mRaw, rhs.mRaw, mCount);
         else
            return false;
      }
      else return false;
   }

   /// Compare with a single character                                        
   ///   @param rhs - the character to compare with                           
   ///   @return true if this container contains this exact character         
   LANGULUS(INLINED)
   bool Bytes::operator == (const CT::BinablePOD auto& rhs) const noexcept {
      using T = Deref<decltype(rhs)>;
      if (mCount != sizeof(T))
         return false;

      if constexpr (CT::Array<T>)
         return 0 == ::std::memcmp(mRaw,  rhs, sizeof(T));
      else
         return 0 == ::std::memcmp(mRaw, &rhs, sizeof(T));
   }

   /// Pick a part of the byte array                                          
   ///   @param start - the starting byte offset                              
   ///   @param count - the number of bytes after 'start' to remain           
   ///   @return a new container that references the original memory          
   LANGULUS(ALWAYS_INLINED)
   Bytes Bytes::Select(Offset start, Count count) IF_UNSAFE(noexcept) {
      return Base::Select<Bytes>(start, count);
   }

   LANGULUS(ALWAYS_INLINED)
   Bytes Bytes::Select(Offset start, Count count) const IF_UNSAFE(noexcept) {
      return Base::Select<Bytes>(start, count);
   }

   /// Serialize to binary, and append to the back                            
   ///   @param rhs - the data to serialize                                   
   ///   @return a reference to this byte container                           
   template<class T> requires CT::Binable<Deint<T>> LANGULUS(ALWAYS_INLINED)
   Bytes& Bytes::operator << (T&& rhs) {
      Base::InsertBlock<void>(IndexBack, Bytes {Forward<T>(rhs)});
      return *this;
   }

   /// Serialize to binary, and append to the front                           
   ///   @param rhs - the data to serialize                                   
   ///   @return a reference to this byte container                           
   template<class T> requires CT::Binable<Deint<T>> LANGULUS(ALWAYS_INLINED)
   Bytes& Bytes::operator >> (T&& rhs) {
      Base::InsertBlock<void>(IndexFront, Bytes {Forward<T>(rhs)});
      return *this;
   }

   /// Concatenate two byte containers                                        
   ///   @param rhs - right hand side                                         
   ///   @return the concatenated byte container                              
   template<class T> requires CT::Binable<Deint<T>> LANGULUS(ALWAYS_INLINED)
   Bytes Bytes::operator + (T&& rhs) const {
      return ConcatInner<Bytes>(Forward<T>(rhs));
   }

   /// Concatenate (destructively) byte containers                            
   ///   @param rhs - right hand side                                         
   ///   @return a reference to this container                                
   template<class T> requires CT::Binable<Deint<T>> LANGULUS(ALWAYS_INLINED)
   Bytes& Bytes::operator += (T&& rhs) {
      return ConcatRelativeInner<Bytes>(Forward<T>(rhs));
   }

   /// Inner concatenation function, used in all Byte derivatives             
   ///   @param rhs - right hand side                                         
   ///   @return the concatenated byte container                              
   template<CT::Bytes THIS, class T>
   THIS Bytes::ConcatInner(T&& rhs) const {
      using S = IntentOf<decltype(rhs)>;
      using B = TypeOf<S>;

      if constexpr (CT::Block<B>) {
         if constexpr (CT::Typed<B>) {
            if constexpr (CT::Similar<Byte, TypeOf<B>>) {
               // We can concat directly                                
               return Base::ConcatBlock<THIS>(S::Nest(rhs));
            }
            else static_assert(false, "Can't concatenate with this container");
         }
         else {
            // Type-erased concat                                       
            return Base::ConcatBlock<THIS>(S::Nest(rhs));
         }
      }
      else {
         // RHS isn't Block, try to convert it to Bytes, and nest       
         return ConcatInner<THIS>(static_cast<THIS>(DeintCast(rhs)));
      }
   }

   /// Inner concatenation function, used in all Text derivatives             
   ///   @param rhs - right hand side                                         
   ///   @return a reference to this container                                
   template<CT::Bytes THIS, class T>
   THIS& Bytes::ConcatRelativeInner(T&& rhs) {
      using S = IntentOf<decltype(rhs)>;
      using B = TypeOf<S>;

      if constexpr (CT::Block<B>) {
         if constexpr (CT::Typed<B>) {
            if constexpr (CT::Similar<Byte, TypeOf<B>>) {
               // We can concat directly                                
               Base::InsertBlock<void>(IndexBack, S::Nest(rhs));
            }
            else static_assert(false, "Can't concatenate with this container");
         }
         else {
            // Type-erased concat                                       
            Base::InsertBlock<void>(IndexBack, S::Nest(rhs));
         }
      }
      else {
         // RHS isn't Block, try to convert it to Text, and nest        
         return ConcatRelativeInner<THIS>(static_cast<THIS>(DeintCast(rhs)));
      }

      return static_cast<THIS&>(*this);
   }

   /// Extend the byte sequence, change count, and return the new range       
   ///   @attention if you extend static container, it will diverge           
   ///   @param count - the number of bytes to append                         
   ///   @return the extended part                                            
   LANGULUS(ALWAYS_INLINED)
   Bytes Bytes::Extend(Count count) {
      return Base::Extend<Bytes>(count);
   }
   
   /// Insert an element, or an array of elements                             
   ///   @param item - the argument and intent to unfold and insert           
   void Bytes::UnfoldInsert(auto&& item) {
      using S = IntentOf<decltype(item)>;
      using T = TypeOf<S>;
      
      if constexpr (CT::Array<T>) {
         using DT = Deext<T>;

         if constexpr (CT::POD<DT> and CT::Dense<DT>) {
            // Insert as byte array                                     
            Base::InsertBlockInner<void, true>(IndexBack,
               MakeBlock<Block<Byte>>(S::Nest(item)));
         }
         else {
            // Unfold and serialize elements, one by one                
            for (auto& element : DeintCast(item))
               UnfoldInsert(S::Nest(element));
         }
      }
      else if constexpr (CT::Block<T>) {
         if constexpr (CT::Bytes<T>) {
            // Insert another byte block's contents                     
            Base::AllocateMore(mCount + DeintCast(item).GetCount());
            CopyMemory(GetRaw() + mCount, DeintCast(item).GetRaw());
            mCount += DeintCast(item).GetCount();
         }
         else if constexpr (CT::Typed<T>) {
            // Nest-insert typed block's elements one by one            
            for (auto& element : DeintCast(item))
               UnfoldInsert(S::Nest(element));
         }
         else {
            // Check a type-erased block                                
            if (DeintCast(item).GetType()->template IsSimilar<Byte>()) {
               // Insert another byte block's contents directly         
               Base::AllocateMore(mCount + DeintCast(item).GetCount());
               CopyMemory(GetRaw() + mCount, DeintCast(item).template GetRaw<Byte>());
               mCount += DeintCast(item).GetCount();
            }
            else for (auto subblock : DeintCast(item))
               UnfoldInsert(S::Nest(subblock));
         }
      }
      else if constexpr (CT::POD<T> and CT::Dense<T>) {
         // Insert as byte array                                        
         Base::InsertBlockInner<void, true>(IndexBack,
            MakeBlock<Block<Byte>>(S::Nest(item)));
      }
      else static_assert(false, "Unable to insert as bytes");
   }
   
   /// Deserialize a byte container to a desired type                         
   ///   @tparam result - [out] data/container to deserialize into            
   ///   @return the number of parsed bytes                                   
   Count Bytes::Deserialize(CT::NotVoid auto& result) const {
      Header header;
      return Base::DeserializeBinary<void>(result, header);
   }

   /// Byte container can always be represented by a type-erased one          
   LANGULUS(ALWAYS_INLINED)
   Bytes::operator Many& () const noexcept {
      // Just make sure that type member has been populated             
      (void) Base::GetType();
      return const_cast<Many&>(reinterpret_cast<const Many&>(*this));
   }

} // namespace Langulus::Annies
