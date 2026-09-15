///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Block.hpp"
#include "../BlockMap.hpp"
#include "../BlockSet.hpp"
#include "../../text/Text.hpp"
#include "../../many/Bytes.hpp"
#include "../../many/Trait.hpp"


namespace Langulus::Annies
{

   /// Convert block's contents to another kind of contents, by iterating     
   /// all elements, and casting them one by one                              
   ///   @param out - what are we converting to?                              
   ///   @return the number of converted elements inserted in 'out'           
   template<class TYPE>
   Count Block<TYPE>::Convert(CT::Block auto& out) const {
      if (IsEmpty())
         return 0;

      using OUT = Deref<decltype(out)>;
      using TO  = TypeOf<OUT>;
      const auto initial = out.mCount;

      if constexpr (not TypeErased and not OUT::TypeErased) {
         // Both containers are statically typed, so leverage it to     
         // generate a well inlined routine for conversion              
         if constexpr (CT::Similar<TYPE, TO>) {
            // Types are already the same, just copy elements           
            out.AllocateMore(out.mCount + mCount);
            out.template InsertBlockInner<void, false>(
               IndexBack, Refer(*this));
         }
         else if constexpr (CT::Convertible<Decay<TYPE>, TO>) {
            // Types are statically convertible                         
            out.AllocateMore(out.mCount + mCount);
            for (auto& from : *this) {
               out.template InsertInner<void, false>(
                  IndexBack, static_cast<TO>(DenseCast(from)));
            }
         }         
      }
      else if (IsSimilar(out)) {
         // Types are already the same, don't convert anything          
         if (out.IsEmpty())
            out = *this;
         else if constexpr ((not TypeErased and CT::ReferMakable<TYPE>)
                   or  (not OUT::TypeErased and CT::ReferMakable<TO>))
            out.InsertBlock(IndexBack, Refer(*this));
         else if constexpr ((not TypeErased and CT::CopyMakable<TYPE>)
                   or  (not OUT::TypeErased and CT::CopyMakable<TO>))
            out.InsertBlock(IndexBack, Copy(*this));
         else LANGULUS_OOPS(Convert, 
            "Unable to append uncopyable elements of type ",
            '`', GetType(), "` - use pointers instead?");
      }
      else {
         // Search for a reflected conversion routine                   
         LANGULUS_ASSERT(out.GetType(),
            Meta, "Can't convert to unknown type");
         LANGULUS_ASSERT(out.GetType()->mOrigin,
            Meta, "Can't convert to incomplete type `", out.GetType(), '`');

         const auto converter = mType->GetConverter(out.GetType()->mOrigin);
         if (not converter)
            return 0;

         out.template AllocateMore<false, true>(out.mCount + mCount);

         if constexpr (not OUT::TypeErased) {
            if constexpr (CT::Sparse<TO>) {
               static_assert(CT::Dense<Deptr<TO>>);

               // We're converting to sparse container                  
               Block<Decay<TO>> coalesced;
               coalesced.AllocateFresh(coalesced.RequestSize(mCount));
               coalesced.mCount = mCount;
               auto temp = coalesced.GetRaw();
               auto to = out.GetHandle(0);

               for (Count i = 0; i < mCount; ++i) {
                  auto from = GetElementDense<CountMax>(i);
                  converter(from.mRaw, temp);
                  to.Assign(temp, coalesced.mEntry);
                  ++to;
                  ++temp;
               }

               const_cast<Allocation*>(coalesced.mEntry)
                  ->Keep(mCount - 1);
            }
            else {
               // We're converting to dense container                   
               auto to = out.mRaw;
               for (Count i = 0; i < mCount; ++i) {
                  // Construct each element                             
                  auto from = GetElementDense<CountMax>(i);
                  converter(from.mRaw, to);
                  to += out.GetType()->mSize;
               }
            }
         }
         else {
            if (out.GetType()->mIsSparse) {
               if (out.GetType()->mDeptr->mIsSparse)
                  TODO();

               // We're converting to sparse container                  
               Block<> coalesced {mType->mOrigin};
               coalesced.AllocateFresh(coalesced.RequestSize(mCount));
               coalesced.mCount = mCount;
               auto temp = coalesced.GetElementInner();
               auto to = out.template GetHandle<void*>(0);

               for (Count i = 0; i < mCount; ++i) {
                  auto from = GetElementDense<CountMax>(i);
                  converter(from.mRaw, temp.mRaw);
                  to.Assign(temp.mRaw, coalesced.mEntry);
                  ++to;
                  ++temp;
               }

               const_cast<Allocation*>(coalesced.mEntry)
                  ->Keep(mCount - 1);
            }
            else {
               // We're converting to dense container                   
               auto to = out.mRaw;
               for (Count i = 0; i < mCount; ++i) {
                  // Construct each element                             
                  auto from = GetElementDense<CountMax>(i);
                  converter(from.mRaw, to);
                  to += out.GetType()->mSize;
               }
            }
         }
      }

      return out.mCount - initial;
   }

   /// Serialize a block into a desired serial format, by following the       
   /// serializer's rules                                                     
   ///   @param out - the resulting serialized data                           
   ///   @return the number of bytes/chars written to 'out'                   
   template<class TYPE>
   Count Block<TYPE>::Serialize(CT::Serial auto& out) const {
      using OUT = Deref<decltype(out)>;
      if constexpr (CT::Bytes<OUT>)
         return SerializeToBinary<void>(out);
      else
         return SerializeToText<void>(out);
   }
   
   /// Serialize block to any string serializer                               
   ///   @tparam NEXT - the type we're serializing - void for type-erasure    
   ///      if both NEXT is type-erased, block header will be serialized      
   ///   @param to - [out] the serialized data goes here                      
   ///   @return the number of written characters                             
   template<class TYPE> template<class NEXT>
   Count Block<TYPE>::SerializeToText(CT::Serial auto& to) const {
      using OUT = Deref<decltype(to)>;
      const auto initial = to.GetCount();

      if (IsEmpty()) {
         if (IsPast())
            to += OUT::Operator::Past;
         else if (IsFuture())
            to += OUT::Operator::Future;
         return to.GetCount() - initial;
      }

      if constexpr (TypeErased) {
         if (IsDeep()) {
            // Nested serialization, wrap it in content scope           
            for (Offset i = 0; i < GetCount(); ++i) {
               auto& subblock = GetDeep(i);
               OUT::SerializationRules::BeginScope(subblock, to);
               subblock.template SerializeToText<void>(to);
               OUT::SerializationRules::EndScope(subblock, to);

               if (i < GetCount() - 1)
                  OUT::SerializationRules::Separate(*this, to);
            }
         }
         else if (CastsTo<Trait>()) {
            // Nest inside traits                                       
            for (Offset i = 0; i < GetCount(); ++i) {
               As<Trait>(i).Serialize(to);

               if (i < GetCount() - 1)
                  OUT::SerializationRules::Separate(*this, to);
            }
         }
         else if (CastsTo<BlockMap>()) {
            // Nest inside maps                                         
            for (Offset i = 0; i < GetCount(); ++i) {
               //auto& map = As<BlockMap>(i);
               TODO();
            }
         }
         else if (CastsTo<BlockSet>()) {
            // Nest inside sets                                         
            for (Offset i = 0; i < GetCount(); ++i) {
               //auto& set = As<BlockSet>(i);
               TODO();
            }
         }
         else if (CastsTo<Construct>()) {
            // Nest inside sets                                         
            for (Offset i = 0; i < GetCount(); ++i) {
               As<Construct>(i).Serialize(to);

               if (i < GetCount() - 1)
                  OUT::SerializationRules::Separate(*this, to);
            }
         }
         else if (CastsTo<Neat>()) {
            // Nest inside sets                                         
            for (Offset i = 0; i < GetCount(); ++i) {
               As<Neat>(i).Serialize(to);

               if (i < GetCount() - 1)
                  OUT::SerializationRules::Separate(*this, to);
            }
         }
         else {
            // If reached, then contents are no longer nested           
            if constexpr (requires { typename OUT::SerializationRules::Rules; }) {
               // Abide by serializer's rules - wrap things accordingly 
               const auto satisfied = SerializeByRules<NEXT>(
                  to, typename OUT::SerializationRules::Rules {});
               if (satisfied) {
                  // Early exit, if conversion was satisfied by rule    
                  //OUT::SerializationRules::EndScope(*this, to);
                  return to.GetCount() - initial;
               }
            }

            if (mType->mNamedValues.size()) {
               // Serialize as a named value                            
               for (Offset i = 0; i < GetCount(); ++i) {
                  for (auto& named : mType->mNamedValues) {
                     const Block<> constant {{}, named};
                     if (GetElementDense(i) == constant) {
                        to += named->mToken;
                        break;
                     }
                  }

                  if (i < GetCount() - 1)
                     OUT::SerializationRules::Separate(*this, to);
               }
               return to.GetCount() - initial;
            }

            // No rules defined, or didn't apply to data, so time to    
            // rely on the reflected converters instead                 
            TMany<OUT> converted;
            if (not Convert(converted)) {
               if constexpr (OUT::SerializationRules::CriticalFailure) {
                  // Couldn't convert elements, and that is marked as   
                  // a critical falure                                  
                  LANGULUS_OOPS(Convert, "Couldn't serialize ", mCount,
                     " item(s) of type `", GetToken(),
                     "` as `", converted.GetToken(), '`');
                  return 0;
               }
               else {
                  // Couldn't convert elements, but since that failure  
                  // isn't marked as critical, we can just inform about 
                  to += OUT("/* Couldn't serialize ", mCount,
                     " item(s) of type `", GetToken(),
                     "` as `", converted.GetToken(), "` */");
                  return to.GetCount() - initial;
               }
            }
            else if constexpr (OUT::SerializationRules::CriticalFailure) {
               // Make sure that all elements are converted to a non-empty 
               // string, as it is disallowed on critical failure          
               for (auto& item : converted) {
                  LANGULUS_ASSERT(item, Convert,
                     "Item(s) of type `", GetToken(),
                     "` were serialized to an empty `", converted.GetToken(), '`');
               }
            }

            // Write all converted elements to the serialized container 
            for (Offset i = 0; i < converted.GetCount(); ++i) {
               if constexpr (LANGULUS(SAFE)) {
                  if (not converted[i]) {
                     // This is reached only if non-critical failure    
                     // Just insert a comment to notify of the error    
                     to += OUT(
                        "/* Item #", i, " of type `", GetToken(),
                        "` was serialized to an empty `", converted.GetToken(), "` */");
                  }
                  else to += converted[i];
               }
               else to += converted[i];

               if (i < GetCount() - 1)
                  OUT::SerializationRules::Separate(*this, to);
            }
         }
      }
      else {
         if constexpr (CT::Deep<Decay<TYPE>>) {
            // Nested serialization, wrap it in content scope           
            for (Offset i = 0; i < GetCount(); ++i) {
               auto& subblock = GetDeep(i);
               OUT::SerializationRules::BeginScope(subblock, to);
               subblock.template SerializeToText<void>(to);
               OUT::SerializationRules::EndScope(subblock, to);

               if (i < GetCount() - 1)
                  OUT::SerializationRules::Separate(*this, to);
            }
         }
         else if constexpr (CT::DerivedFrom<TYPE, Trait>) {
            // Nest inside traits                                       
            for (Offset i = 0; i < GetCount(); ++i) {
               As<Trait>(i).Serialize(to);

               if (i < GetCount() - 1)
                  OUT::SerializationRules::Separate(*this, to);
            }
         }
         else if constexpr (CT::DerivedFrom<TYPE, BlockMap>) {
            // Nest inside maps                                         
            for (Offset i = 0; i < GetCount(); ++i) {
               //auto& map = As<BlockMap>(i);
               TODO();
            }
         }
         else if constexpr (CT::DerivedFrom<TYPE, BlockSet>) {
            // Nest inside sets                                         
            for (Offset i = 0; i < GetCount(); ++i) {
               //auto& set = As<BlockSet>(i);
               TODO();
            }
         }
         else if constexpr (CT::DerivedFrom<TYPE, Construct>) {
            // Nest inside sets                                         
            for (Offset i = 0; i < GetCount(); ++i) {
               As<Construct>(i).Serialize(to);

               if (i < GetCount() - 1)
                  OUT::SerializationRules::Separate(*this, to);
            }
         }
         else if constexpr (CT::DerivedFrom<TYPE, Neat>) {
            // Nest inside sets                                         
            for (Offset i = 0; i < GetCount(); ++i) {
               As<Neat>(i).Serialize(to);

               if (i < GetCount() - 1)
                  OUT::SerializationRules::Separate(*this, to);
            }
         }
         else {
            // If reached, then contents are no longer nested           
            if constexpr (requires { typename OUT::SerializationRules::Rules; }) {
               // Abide by serializer's rules and wrap things accordingly
               const auto satisfied = SerializeByRules<NEXT>(
                  to, typename OUT::SerializationRules::Rules {});
               if (satisfied) {
                  // Early exit, if conversion was satisfied by rule    
                  //OUT::SerializationRules::EndScope(*this, to);
                  return to.GetCount() - initial;
               }
            }

            //TODO optimize this further
            if (mType->mNamedValues.size()) {
               // Serialize as a named value                            
               for (Offset i = 0; i < GetCount(); ++i) {
                  for (auto& named : mType->mNamedValues) {
                     const Block<> constant {{}, named};
                     if (GetElementDense(i) == constant) {
                        to += named->mToken;
                        break;
                     }
                  }

                  if (i < GetCount() - 1)
                     OUT::SerializationRules::Separate(*this, to);
               }
               return to.GetCount() - initial;
            }

            // No rules defined, or didn't apply to data, so time to    
            // rely on the reflected converters instead                 
            TMany<OUT> converted;
            if (not Convert(converted)) {
               if constexpr (OUT::SerializationRules::CriticalFailure) {
                  // Couldn't convert elements, and that is marked as   
                  // a critical falure                                  
                  LANGULUS_OOPS(Convert, "Couldn't serialize ", mCount,
                     " item(s) of type `", GetToken(),
                     "` as `", converted.GetToken(), '`');
                  return 0;
               }
               else {
                  // Couldn't convert elements, but since that failure  
                  // isn't marked as critical, we can just inform about 
                  to += OUT("/* Couldn't serialize ", mCount,
                     " item(s) of type `", GetToken(),
                     "` as `", converted.GetToken(), "` */");
                  return to.GetCount() - initial;
               }
            }
            else if constexpr (OUT::SerializationRules::CriticalFailure) {
               // Make sure that all elements are converted to a non-empty 
               // string, as it is disallowed on critical failure          
               for (auto& item : converted) {
                  LANGULUS_ASSERT(item, Convert,
                     "Item(s) of type `", GetToken(),
                     "` were serialized to an empty `", converted.GetToken(), '`');
               }
            }

            // Write all converted elements to the serialized container 
            for (Offset i = 0; i < converted.GetCount(); ++i) {
               if constexpr (LANGULUS(SAFE)) {
                  if (not converted[i]) {
                     // This is reached only if non-critical failure    
                     // Just insert a comment to notify of the error    
                     to += OUT(
                        "/* Item #", i, " of type `", GetToken(),
                        "` was serialized to an empty `", converted.GetToken(), "` */");
                  }
                  else to += converted[i];
               }
               else to += converted[i];

               if (i < GetCount() - 1)
                  OUT::SerializationRules::Separate(*this, to);
            }
         }
      }

      const bool scoped = GetCount() > 1 or IsInvalid() or IsExecutable();
      if (not scoped) {
         if (IsPast())
            to += OUT::Operator::Past;
         else if (IsFuture())
            to += OUT::Operator::Future;
      }

      return to.GetCount() - initial;
   }

   /// Apply serialization rules                                              
   template<class TYPE> template<class T, class...RULES>
   Count Block<TYPE>::SerializeByRules(CT::Serial auto& to, Types<RULES...>) const {
      Count result = 0;
      (void)(... or (result = SerializeApplyRule<T, RULES>(to)));
      return result;
   }

   /// Apply a single serialization rule                                      
   template<class TYPE> template<class T, class RULE>
   Count Block<TYPE>::SerializeApplyRule(CT::Serial auto& to) const {
      using OUT = Deref<decltype(to)>;
      const auto initial = to.GetCount();
      using Type = typename RULE::Type;

      if constexpr (TypeErased) {
         if constexpr (RULE::sMatch == Serial::Exact) {
            if (not IsSimilar<Type>())
               return 0;
         }
         else if constexpr (RULE::sMatch == Serial::BasedOn) {
            if (not CastsTo<Type>())
               return 0;
         }
         else static_assert(false, "Unimplemented matcher");

         if constexpr (RULE::sRule == Serial::Skip)
            return 0;
         else if constexpr (RULE::sRule == Serial::Wrap) {
            // If reached, then the rule is compatible with the type    
            for (Offset i = 0; i < mCount; ++i) {
               if constexpr (RULE::sStart < Serial::Operator::OpCounter)
                  to += RULE::sStart;

               to += static_cast<OUT>(As<Type>(i));

               if constexpr (RULE::sEnd < Serial::Operator::OpCounter)
                  to += RULE::sEnd;

               if (i < GetCount() - 1)
                  OUT::SerializationRules::Separate(*this, to);
            }

            return to.GetCount() - initial;
         }
         else static_assert(false, "Unimplemented rule");
      }
      else {
         if constexpr (RULE::sMatch == Serial::Exact
         and not CT::Similar<TYPE, Type>)
            return 0;
         else if constexpr (RULE::sMatch == Serial::BasedOn
         and not CT::DerivedFrom<TYPE, Type>)
            return 0;
         else {
            if constexpr (RULE::sRule == Serial::Skip)
               return 0;
            else if constexpr (RULE::sRule == Serial::Wrap) {
               // If reached, then the rule is compatible with the type 
               for (Offset i = 0; i < mCount; ++i) {
                  if constexpr (RULE::sStart < Serial::Operator::OpCounter)
                     to += RULE::sStart;

                  to += static_cast<OUT>(As<Type>(i));

                  if constexpr (RULE::sEnd < Serial::Operator::OpCounter)
                     to += RULE::sEnd;

                  if (i < GetCount() - 1)
                     OUT::SerializationRules::Separate(*this, to);
               }

               return to.GetCount() - initial;
            }
            else static_assert(false, "Unimplemented rule");
         }
      }
   }

   /// Serialize block to binary                                              
   ///   @attention any change in this routine should be reflected in the     
   ///      corresponding Block::DeserializeBinary                            
   ///   @tparam NEXT - the type we're serializing - void for type-erasure    
   ///      if both NEXT and THIS are type-erased, type will be serialized    
   ///   @param to - [out] the serialized data goes here                      
   ///   @return the number of written bytes                                  
   template<class TYPE> template<class NEXT>
   Count Block<TYPE>::SerializeToBinary(CT::Serial auto& to1) const {
      auto& to = to1; //Workaround: needed due to really weird clang error  
      //using OUT = Deref<decltype(to)>;
      const auto initial = to.GetCount();

      if constexpr (CT::TypeErased<NEXT>) {
         to += Bytes {GetCount()};
         to += Bytes {GetUnconstrainedState()};
         to += Bytes {GetType()};
      }

      if (IsEmpty() or IsUntyped())
         return to.GetCount() - initial;

      if (IsDeep()) {
         // If data is deep, nest-serialize each sub-block              
         ForEach([&to](const Block<>& block) {
            block.SerializeToBinary<void>(to);
         });

         return to.GetCount() - initial;
      }
      else if (CastsTo<AMeta>()) {
         // Serialize meta                                              
         ForEach(
            [&to](DMeta meta) noexcept {to += Bytes {meta};},
            [&to](VMeta meta) noexcept {to += Bytes {meta};},
            [&to](TMeta meta) noexcept {to += Bytes {meta};},
            [&to](CMeta meta) noexcept {to += Bytes {meta};}
         );

         return to.GetCount() - initial;
      }
      else if (IsPOD()) {
         // If data is POD, optimize by directly memcpying it           
         const auto denseStride = GetStride();
         const auto byteCount = denseStride * GetCount();
         to.AllocateMore(to.GetCount() + byteCount);

         if (IsSparse()) {
            // ... pointer by pointer if sparse                         
            auto p = mRawSparse;
            const auto pEnd = p + GetCount();
            while (p != pEnd)
               to += Bytes::From(Disown(p++), denseStride);
         }
         else {
            // ... at once if dense                                     
            to += Bytes::From(Disown(mRaw), byteCount);
         }

         return to.GetCount() - initial;
      }
      else if (mType->mDefaultConstructor
      and not  mType->mProducerRetriever) {
         // Serialize specialized containers here                       
         const auto satisfied = ForEach(
            [&to](const Text& text) {
               to += Bytes {text.GetCount()};
               to += Bytes::From(Disown(text.mRaw), text.mCount);
            },
            [&to](const Bytes& bytes) {
               to += Bytes {bytes.mCount};
               to += bytes;
            },
            [&to](const Trait& trait) {
               to += Bytes {trait.GetTrait()};
               trait.SerializeToBinary<void>(to);
            }
         );

         if (satisfied)
            return to.GetCount() - initial;

         // Type is statically creatable, and has default constructor   
         // therefore we can serialize it by serializing each           
         // reflected base and member                                   
         for (Count i = 0; i < GetCount(); ++i) {
            auto element = GetElementResolved(i);
            if (IsResolvable())
               to += Bytes {element.GetType()};

            // Serialize all reflected bases                            
            for (auto& base : element.GetType()->mBases) {
               // Imposed bases are never serialized                    
               if (base.mImposed or base.mType->mIsAbstract)
                  continue;

               const auto baseBlock = element.GetBaseMemory(base);
               baseBlock.template SerializeToBinary<RTTI::Base>(to);
            }

            // Serialize all reflected members                          
            for (auto& member : element.GetType()->mMembers) {
               const auto memberBlock = element.GetMember(member, 0);
               memberBlock.template SerializeToBinary<RTTI::Member>(to);
            }
         }

         return to.GetCount() - initial;
      }

      // Failure if reached                                             
      LANGULUS_OOPS(Convert, "Can't serialize ",
         " type `", GetToken(), "` as `", to.GetToken(), "`");
      return 0;
   }

   ///                                                                        
   template<class TYPE> LANGULUS(INLINED)
   void Block<TYPE>::ReadInner(Offset start, Count count, Loader loader) const {
      if (start >= mCount or mCount - start < count) {
         LANGULUS_ASSERT(loader, Access, "Reader lacks loader");
         loader(const_cast<Block&>(*this), count - (mCount - start));
      }
   }

   /// Read an atom-sized unsigned integer, based on the provided header      
   ///   @param result - [out] the resulting deserialized number              
   ///   @param read - offset to apply to serialized byte array               
   ///   @param header - environment header                                   
   ///   @param loader - loader for streaming                                 
   ///   @return the number of read bytes from byte container                 
   template<class TYPE>
   Offset Block<TYPE>::DeserializeAtom(
      Offset& result, Offset read, const Header& header, Loader loader
   ) const {
      if (header.mAtomSize == 4) {
         // We're deserializing data, that was serialized on a 32-bit   
         // architecture                                                
         uint32_t count4 = 0;
         ReadInner(read, 4, loader);
         ::std::memcpy(&count4, At(read), 4);
         read += 4;
         result = static_cast<Offset>(count4);
      }
      else if (header.mAtomSize == 8) {
         // We're deserializing data, that was serialized on a 64-bit   
         // architecture                                                
         uint64_t count8 = 0;
         ReadInner(read, 8, loader);
         ::std::memcpy(&count8, At(read), 8);
         read += 8;
         LANGULUS_ASSERT(
            count8 <= std::numeric_limits<Offset>::max(),
            Convert, "Deserialized atom contains a value "
            "too powerful for your architecture");
         result = static_cast<Offset>(count8);
      }
      else {
         LANGULUS_OOPS(Convert,
            "Unsupported atomic size ", header.mAtomSize,
            " was deserialized from source - is the source corrupted?"
         );
      }

      return read;
   }

   /// A snippet for conveniently deserializing a meta from binary            
   ///   @param result - [out] the deserialized meta goes here                
   ///   @param read - byte offset inside 'from'                              
   ///   @param header - environment header                                   
   ///   @param loader - loader for streaming                                 
   ///   @return number of read bytes                                         
   template<class TYPE>
   Offset Block<TYPE>::DeserializeMeta(
      CT::Meta auto& result, Offset read, const Header& header, Loader loader
   ) const {
      Count count = 0;
      read = DeserializeAtom(count, read, header, loader);
      if (count) {
         ReadInner(read, count, loader);
         const Token token {GetRaw<Letter>() + read, count};

      #if LANGULUS_FEATURE(MANAGED_REFLECTION)
         using META = Deref<decltype(result)>;

         if constexpr (CT::Same<META, DMeta>)
            result = RTTI::GetMetaData(token);
         else if constexpr (CT::Same<META, VMeta>)
            result = RTTI::GetMetaVerb(token);
         else if constexpr (CT::Same<META, TMeta>)
            result = RTTI::GetMetaTrait(token);
         else if constexpr (CT::Same<META, CMeta>)
            result = RTTI::GetMetaConstant(token);
         else
            static_assert(false, "Unsupported meta deserialization");

         LANGULUS_ASSERT(result, Meta,
            "Deserialized meta for token `", token, "` doesn't exist");
         return read + count;
      #else
         LANGULUS_OOPS(Meta,
            "The build doesn't include managed reflection, "
            "so it can't deserialize meta from token: ", token,
            " unless it's a built-in type"
         );
         return read;
      #endif
      }

      result = {};
      return read;
   }

   /// Inner deserialization routine from binary                              
   ///   @tparam NEXT - the type we're deserializing - void for type-erasure  
   ///      if both NEXT and 'to' are type-erased, type will be deserialized  
   ///   @param to - [out] the resulting deserialized data                    
   ///   @param header - environment header                                   
   ///   @param readOffset - offset to apply to serialized byte array         
   ///   @param loader - loader for streaming                                 
   ///   @return the number of read/peek bytes from byte container            
   template<class TYPE> template<class NEXT>
   Offset Block<TYPE>::DeserializeBinary(
      CT::Block auto& to, const Header& header1, Offset readOffset, Loader loader1
   ) const {
      auto& header = header1; //Workaround: needed due to really weird clang error  
      auto& loader = loader1; //Workaround: needed due to really weird clang error  
      using OUT = Deref<decltype(to)>;
      using T   = Conditional<OUT::TypeErased, NEXT, TypeOf<OUT>>;

      static_assert(TypeErased or CT::Byte<TYPE>,
         "THIS isn't a byte container");
      static_assert(OUT::TypeErased or CT::TypeErased<NEXT>
         or CT::Similar<TypeOf<OUT>, NEXT>, "Type mismatch");
      LANGULUS_ASSUME(DevAssumes, IsSimilar<Byte>(),
         "THIS isn't a byte container");

      Count deserializedCount = 0;
      Offset read = readOffset;

      if constexpr (CT::TypeErased<T>) {
         // We have unpredictable data, so the deserializer expects     
         // that next bytes contain instructions of what kind of data   
         // to deserialize                                              
         read = DeserializeAtom(deserializedCount, read, header, loader);

         // First read the serialized data state                        
         DataState state {};
         ReadInner(read, sizeof(DataState), loader);
         memcpy(static_cast<void*>(&state), At(read), sizeof(DataState));
         read += sizeof(DataState);
         to.AddState(state);

         // Finally, read type                                          
         DMeta type;
         read = DeserializeMeta(type, read, header, loader);
         if (not type)
            return read;

         // And mutate the resulting container apppropriately           
         // (this also acts as a runtime type-check, in case 'to'       
         // already contains stuff)                                     
         to.template Mutate<void>(type);
      }
      else {
         // We have predictable data                                    
         // In this case, 'to' should already be allocated and known    
         if constexpr (not CT::SameAsOneOf<T, RTTI::Base, RTTI::Member>) {
            LANGULUS_ASSUME(DevAssumes, to.template IsSimilar<T>(),
               "Bad binary deserializing block type: ",
               to.GetType(), " instead of ", MetaDataOf<T>()
            );
         }

         LANGULUS_ASSUME(DevAssumes, not to.IsEmpty(),
            "Binary deserializing block isn't preinitialized");

         deserializedCount = to.GetCount();
      }

      if (not deserializedCount)
         return read;

      // Fill memory                                                    
      if (to.IsDeep()) {
         // If data is deep, nest each sub-block                        
         if constexpr (CT::TypeErased<T>)
            to.New(deserializedCount);

         to.ForEach([&](Block<>& block) {
            read = DeserializeBinary<void>(block, header, read, loader);
         });

         return read;
      }
      else if (to.template CastsTo<AMeta>()) {
         // Deserialize data definitions                                
         //TODO register them as dependencies
         if constexpr (CT::TypeErased<T>)
            to.New(deserializedCount);

         to.ForEach(
            [&](DMeta& meta) noexcept {
               read = DeserializeMeta(meta, read, header, loader);
            },
            [&](VMeta& meta) noexcept {
               read = DeserializeMeta(meta, read, header, loader);
            },
            [&](CMeta& meta) noexcept {
               read = DeserializeMeta(meta, read, header, loader);
            },
            [&](TMeta& meta) noexcept {
               read = DeserializeMeta(meta, read, header, loader);
            }
         );

         return read;
      }
      else if (to.IsPOD()) {
         // If data is POD, optimize by directly memcpying it           
         if constexpr (CT::TypeErased<T>)
            to.template AllocateMore<false, true>(deserializedCount);

         const auto byteSize = to.GetBytesize();
         ReadInner(read, byteSize, loader);

         if (to.IsSparse()) {
            // Allocate a separate block for the elements               
            const auto temporary = Allocator::Allocate(nullptr, byteSize);
            auto start = temporary->GetBlockStart();
            ::std::memcpy(start, At(read), byteSize);
            read += byteSize;
            temporary->Keep(deserializedCount - 1);

            // Write a pointer to each element                          
            auto p = to.template GetHandle<Byte*>(0);
            const auto pEnd = p + to.GetCount();
            const auto size = to.GetType()->mSize;
            while (p != pEnd) {
               p.Assign(start, temporary);
               start += size;
            }
         }
         else {
            // Data is dense, parse it all at once                      
            ::std::memcpy(to.GetRaw(), At(read), byteSize);
            read += byteSize;
         }

         return read;
      }
      else if (to.mType->mDefaultConstructor
      and not  to.mType->mProducerRetriever) {
         if (to.template CastsTo<Text>()) {
            // Deserialize a text based container                       
            if constexpr (CT::TypeErased<T>)
               to.AllocateMore(deserializedCount);

            for (Count i = 0; i < deserializedCount; ++i) {
               Count count = 0;
               read = DeserializeAtom(count, read, header, loader);
               to.template InsertInner<void, false>(
                  IndexBack, Text::From(Disown(
                     reinterpret_cast<const Letter*>(mRaw + read)), count)
               );
               read += count * sizeof(Letter);
            }

            return read;
         }
         else if (to.template CastsTo<Bytes>()) {
            // Deserialize a bytes based container                      
            if constexpr (CT::TypeErased<T>)
               to.AllocateMore(deserializedCount);

            for (Count i = 0; i < deserializedCount; ++i) {
               Count count = 0;
               read = DeserializeAtom(count, read, header, loader);
               to.template InsertInner<void, false>(
                  IndexBack, Bytes::From(Disown(mRaw + read), count)
               );
               read += count;
            }

            return read;
         }
         else if (to.template CastsTo<Trait>()) {
            // Deserialize a Trait based container                      
            if constexpr (CT::TypeErased<T>)
               to.New(deserializedCount);

            if (to.template IsSimilar<Trait>()) {
               // Each trait can be different                           
               to.ForEach([&](Trait& trait) {
                  TMeta ttype;
                  read = DeserializeMeta(ttype, read, header, loader);
                  trait.SetTrait(ttype);

                  auto& block = static_cast<Block<>&>(trait);
                  read = DeserializeBinary<void>(block, header, read, loader);
               });
            }
            else {
               // All traits are the same                               
               to.ForEach([&](Trait& trait) {
                  auto& block = static_cast<Block<>&>(trait);
                  read = DeserializeBinary<void>(block, header, read, loader);
               });
            }

            return read;
         }

         // Type is statically producible, and has default constructor, 
         // therefore we can deserialize it by making a default         
         // instance, and filling in the reflected members and bases    
         if constexpr (CT::TypeErased<T>)
            to.AllocateMore(deserializedCount);

         for (Count i = 0; i < deserializedCount; ++i) {
            Many element;
            if constexpr (CT::TypeErased<T>) {
               // Default-initialize an instance to write on top of     
               // Make sure that the type is the most concrete one, in  
               // the case that instances are resolvable                
               auto resolvedType = to.GetType();
               if (to.IsResolvable())
                  read = DeserializeMeta(resolvedType, read, header, loader);
               element = Many::FromMeta(resolvedType);
               element.New(1);
            }
            else {
               // Data is predictable and assumed initialized, just     
               // write on top of it                                    
               element = to.GetElementDense(i);
            }

            // Deserialize all reflected bases                          
            // Skip abstract/imposed bases                              
            for (auto& base : element.GetType()->mBases) {
               if (base.mImposed or base.mType->mIsAbstract)
                  continue;

               auto baseBlock = element.GetBaseMemory(base);
               read = DeserializeBinary<RTTI::Base>(
                  baseBlock, header, read, loader);
            }

            // Deserialize all reflected members                        
            for (auto& member : element.GetType()->mMembers) {
               auto memberBlock = element.GetMember(member, 0);
               read = DeserializeBinary<RTTI::Member>(
                  memberBlock, header, read, loader);
            }

            if constexpr (CT::TypeErased<T>) {
               to.template InsertBlockInner<void, false>(
                  IndexBack, Abandon(static_cast<Block<>&>(element)));
            }
         }

         return read;
      }

      // Failure if reached                                             
      LANGULUS_OOPS(Convert, "Can't deserialize `", GetToken(),
         "` as `", to.GetToken(), '`');
      return 0;
   }

} // namespace Langulus::Flow::Serial

