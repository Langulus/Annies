///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Bytes.hpp"
#include <Langulus/CT/Serializer.hpp>
#include <Langulus/HashOf.hpp>


namespace Langulus::CTTI
{
   /// The presence of this structure makes Bytes a CT::Serializer            
   template<>
   struct Serializer<Annies::Bytes> {
      using Context = Annies::Inner::BytesDictionary;
      using T = Annies::Bytes;
      using Cid = Annies::Cid;

      static constexpr bool CriticalFailure = true;
      static constexpr bool SkipElements = false;
      static constexpr size_t MaxDimensions = 2;

      /// Flags used in the headbit                                           
      enum Headbits : uint8_t {
         // Skips an element, default initializes it if needed          
         Skip = 0,

         // Subsequent data is big endian                               
         BigEndian = 1,

         // A type ID is serialized for the next element, for each      
         // individual dimension. The size of the ID starts at 8bit     
         // and increases using Large16/Large32 flags.                  
         // The ID indexes a definition in the context, so this         
         // is usually accompanied with the HasDependencies flag.       
         Typed = 2,

         // A 8bit state is serialized for the next element.            
         // If accompanied with Large16/Large32, the size increases.    
         Stateful = 4,

         // Serializes count, assumed 1 if this flag is missing.        
         // When Large16/Large32 are enabled, the size of the           
         // counter increases.                                          
         Multiple = 8,

         // Signifies that state and count get more bits in order       
         // to serialize more elements.                                 
         Large16 = 16,

         // Signifies that state and count get even more bits in        
         // order to serialize even more elements. Can be combined      
         // with Large16 in order to jump up to 64 bits.                
         Large32 = 32,

         Large64 = Large16 | Large32,

         // Signifies that the serialized data uses the context.        
         HasDependencies = 64,

         // Serializes the number of dimensions, otherwise assumed 1.   
         // Always of size 1 byte, for up to 256 dimensions.            
         Multidimensional = 128
      };

      /// Serializes any container                                            
      ///   @param from the container to serialize                            
      ///   @param to where serialized bytes get appended                     
      ///   @param context optional context for storing repeating patterns    
      template<CT::Container C>
      static void BeginScope(C const& from, T& to, Context* context) {
         if (not from.IsValid()) {
            to += T (Headbits::Skip);
            return;
         }

         // Write header                                                
         /// @attention this is the biggest possible header size,       
         ///    but this one in particular doesn't allocate space for   
         ///    more than two types!                                    
         uint8_t header[1 + 1 + MaxDimensions*8 + 8 + 8];
         uint8_t& headbyte = header[0];
         headbyte = 0;
         size_t progress = 1;

         if (std::endian::native == std::endian::big)
            headbyte |= Headbits::BigEndian;

         if (C::Dimensions::Count > 1) {
            static_assert(C::Dimensions::Count <= MaxDimensions,
               "Update MaxDimensions for more dimensions. "
               "This is not set to max to save on stack memory"
            );
            headbyte |= Headbits::Multidimensional;
            header[progress] = static_cast<uint8_t>(C::Dimensions::Count);
            ++progress;
         }

         // First pass goes through all variable-sized counters and     
         // populates the header flags before writing anything.         
         if constexpr (requires { from.GetType(); }) {
            C::Dimensions::ForEach([&]<Cid D> {
               LglsAssert(context,
                  "Context is required for binary serialization of containers");

               headbyte |= Headbits::Typed;

               const uint64_t typeId = context->mDMetaBank.Define(from.template GetType<D>());
               if (typeId >= static_cast<uint64_t>(Context::BuiltInTypes::_Counter_))
                  headbyte |= Headbits::HasDependencies;

               if (typeId < 256)
                  ;
               else if (typeId < 65536)
                  headbyte |= Headbits::Large16;
               else if (typeId < 4294967296)
                  headbyte |= Headbits::Large32;
               else
                  headbyte |= Headbits::Large64;
            });
         }

         if constexpr (requires { from.GetUnconstrainedState(); }) {
            const uint64_t s = from.GetUnconstrainedState();
            if (s != 0) {
               headbyte |= Headbits::Stateful;

               if (s < 256)
                  ;
               else if (s < 65536)
                  headbyte |= Headbits::Large16;
               else if (s < 4294967296)
                  headbyte |= Headbits::Large32;
               else
                  headbyte |= Headbits::Large64;
            }
         }

         if constexpr (requires { from.GetCount(); }) {
            const uint64_t s = from.GetCount();
            if (s != 1) {
               headbyte |= Headbits::Multiple;

               if (s < 256)
                  ;
               else if (s < 65536)
                  headbyte |= Headbits::Large16;
               else if (s < 4294967296)
                  headbyte |= Headbits::Large32;
               else
                  headbyte |= Headbits::Large64;
            }
         }

         // Now write the data                                          
         if constexpr (requires { from.GetType(); }) {
            C::Dimensions::ForEach([&]<Cid D> {
               const uint64_t typeId = context->mDMetaBank.Define(from.template GetType<D>());
               if ((headbyte & Headbits::Large64) == Headbits::Large64) {
                  memcpy(header + progress, &typeId, 8);
                  progress += 8;
               }
               else if (headbyte & Headbits::Large32) {
                  const uint32_t typeId32 = static_cast<uint32_t>(typeId);
                  memcpy(header + progress, &typeId32, 4);
                  progress += 4;
               }
               else if (headbyte & Headbits::Large16) {
                  const uint16_t typeId16 = static_cast<uint16_t>(typeId);
                  memcpy(header + progress, &typeId16, 2);
                  progress += 2;
               }
               else {
                  header[progress] = static_cast<uint8_t>(typeId);
                  ++progress;
               }
            });
         }

         if constexpr (requires { from.GetUnconstrainedState(); }) {
            const uint64_t s = from.GetUnconstrainedState();
            if (s != 0) {
               if ((headbyte & Headbits::Large64) == Headbits::Large64) {
                  memcpy(header + progress, &s, 8);
                  progress += 8;
               }
               else if (headbyte & Headbits::Large32) {
                  const uint32_t s32 = static_cast<uint32_t>(s);
                  memcpy(header + progress, &s32, 4);
                  progress += 4;
               }
               else if (headbyte & Headbits::Large16) {
                  const uint16_t s16 = static_cast<uint16_t>(s);
                  memcpy(header + progress, &s16, 2);
                  progress += 2;
               }
               else {
                  header[progress] = static_cast<uint8_t>(s);
                  ++progress;
               }
            }
         }

         if constexpr (requires { from.GetCount(); }) {
            const uint64_t s = from.GetCount();
            if (s != 1) {
               if ((headbyte & Headbits::Large64) == Headbits::Large64) {
                  memcpy(header + progress, &s, 8);
                  progress += 8;
               }
               else if (headbyte & Headbits::Large32) {
                  const uint32_t s32 = static_cast<uint32_t>(s);
                  memcpy(header + progress, &s32, 4);
                  progress += 4;
               }
               else if (headbyte & Headbits::Large16) {
                  const uint16_t s16 = static_cast<uint16_t>(s);
                  memcpy(header + progress, &s16, 2);
                  progress += 2;
               }
               else {
                  header[progress] = static_cast<uint8_t>(s);
                  ++progress;
               }
            }
         }

         // Finally, write the entire header to the stream              
         to += T::FromBytes(header, progress);
      }
      
      static void EndScope(const CT::Container auto&, T&, Context*) {
         // noop
      }
      
      static void Separate(const CT::Container auto&, T&, Context*) {
         // noop
      }
      
      static void Empty(RTTI::DMeta type, size_t i, T&, Context*) {
         LglsError("Item #", i, " of type `", type.GetName(),
            "` was serialized to an empty `Bytes`");
      }
      
      static void Error(RTTI::DMeta type, size_t i, T&, Context*) {
         LglsError("Item #", i, " of type `", type.GetName(),
            "` failed to convert to `Bytes`");
      }
   };

   /// Rule for serializing any container that isn't deep.                    
   template<class C> requires CT::Container<Decay<C>>
   struct SerializationRule<Annies::Bytes, C> {
      static_assert(Exact<DecvqAll<C>, C>,
         "Strip all decorations on all indirections first");

      using S = Serializer<Annies::Bytes>;
      using Context = typename S::Context;

      static void Serialize(ConstAll<C&>, Annies::Bytes&, Context*);
   };

   /// A rule for serializing meta data.                                      
   /// Will register it in the Context, and write it as an ID where needed.   
   template<>
   struct SerializationRule<Annies::Bytes, RTTI::DMeta> {
      using S = Serializer<Annies::Bytes>;
      using Context = typename S::Context;
      
      static void Serialize(RTTI::DMeta const&, Annies::Bytes&, Context*);
   };
   
   /// A rule for serializing meta tags.                                      
   /// Will register it in the Context, and write it as an ID where needed.   
   template<>
   struct SerializationRule<Annies::Bytes, RTTI::TMeta> {
      using S = Serializer<Annies::Bytes>;
      using Context = typename S::Context;
      
      static void Serialize(RTTI::TMeta const&, Annies::Bytes&, Context*);
   };
   
   /// A rule for serializing meta constants.                                 
   /// Will register it in the Context, and write it as an ID where needed.   
   template<>
   struct SerializationRule<Annies::Bytes, RTTI::CMeta> {
      using S = Serializer<Annies::Bytes>;
      using Context = typename S::Context;
      
      static void Serialize(RTTI::CMeta const&, Annies::Bytes&, Context*);
   };
   
   /// A rule for serializing meta verbs.                                     
   /// Will register it in the Context, and write it as an ID where needed.   
   template<>
   struct SerializationRule<Annies::Bytes, RTTI::VMeta> {
      using S = Serializer<Annies::Bytes>;
      using Context = typename S::Context;
      
      static void Serialize(RTTI::VMeta const&, Annies::Bytes&, Context*);
   };

   /// A rule for serializing any deep container that contains multiple items.
   /// This includes Text, Bytes, Many, Map, Set, Pair, Neat, Tag, etc...     
   /// as well as their templated equivalents.                                
   /*template<CT::Deep C>
   void SerializationRule<Annies::Bytes, C>::Serialize(
      ConstAll<C&> may_be_sparse, Annies::Bytes& out, Context* context
   ) requires CT::ContainsMany<Decay<C>> {
      using DC = Decay<C>;
      static_assert(CT::NotHandle<DC>);
      DC const& self = DenseCast(may_be_sparse);
      S::BeginScope(self, out, context);

      self.Apply([&](auto const& item) {
         Langulus::Serialize(item, out, context);
      });

      S::EndScope(self, out, context);
   }*/

   /// A rule for serializing any deep container that contains single item.   
   /// This includes Any, Handle, Own, Ref and their templated equivalents.   
   /*template<CT::Deep C>
   void SerializationRule<Annies::Bytes, C>::Serialize(
      ConstAll<C&> may_be_sparse, Annies::Bytes& out, Context* context
   ) requires CT::ContainsOne<Decay<C>> {
      using DC = Decay<C>;
      DC const& self = DenseCast(may_be_sparse);
      S::BeginScope(self, out, context);

      // Iterate all dimensions                                         
      DC::Dimensions::ForEach([&]<uint ID> {
         if constexpr (CT::TypeErased<DC>) {
            //                                                          
            // Serialize a type-erased container                        
            const auto T = self.template GetType<ID>();
            const auto bytes_meta = MetaDataOf<Annies::Bytes>();
            const auto serializer = T.GetMorphism(bytes_meta).serialize;
            LglsAssert(serializer, "Missing serializer",
               " from ", T.GetName(), " to ", bytes_meta.GetName());
            serializer(DecvqAllCast(self.template GetRaw<ID>()), &out, context);
         }
         else {
            //                                                          
            // Serialize a statically-typed container                   
            using T = Decay<TypeOf<DC, ID>>;
            auto* item = self.template Get<T, ID>();
            Langulus::Serialize(*item, out, context);
         }

         S::EndScope(self, out, context);
      });
   }*/

   /// Rule for serializing any container to bytes, deep or sparse            
   template<class C> requires CT::Container<Decay<C>>
   void SerializationRule<Annies::Bytes, C>::Serialize(
      ConstAll<C&> may_be_sparse, Annies::Bytes& out, Context* context
   ) {
      using DC = Decay<C>;
      DC const& self = DenseCast(may_be_sparse);
      S::BeginScope(self, out, context);

      IF_NOT_LANGULUS_FORCE_TYPE_ERASURE(if constexpr (CT::TypeErased<DC>) {)
         //                                                             
         // Serialize a type-erased container                           
         const auto T = self.GetType();
         const auto bytes_meta = MetaDataOf<Annies::Bytes>();
         auto serializer = T.GetMorphism(bytes_meta).serialize;
         LglsAssert(serializer, "Missing serializer",
            " from ", T.GetName(), " to ", bytes_meta.GetName());

         self.Apply([&](auto const& item) {
            serializer(item.GetRaw(), &out, context);
         });
      #if not LANGULUS(FORCE_TYPE_ERASURE)
      } else {
         //                                                             
         // Serialize a statically-typed container                      
         self.Apply([&](auto const& item) {
            Langulus::Serialize(*item, out, context);
         });
      }
      #endif

      S::EndScope(self, out, context);
   }

   /// Rule for serializing DMeta to Bytes                                    
   inline void SerializationRule<Annies::Bytes, RTTI::DMeta>::Serialize(
      RTTI::DMeta const& item, Annies::Bytes& out, Context* context
   ) {
      if (not item) {
         out += uint32_t {0};
         return;
      }

      auto registered = context->mDMetaBank.mDefinitions.find(item);
      if (registered == context->mDMetaBank.mDefinitions.end()) {
         context->mDMetaBank.mDefinitions[item] = context->mDMetaBank.mNextId;
         out += context->mDMetaBank.mNextId;
         ++context->mDMetaBank.mNextId;
      }
      else out += registered->second;
   }

   /// Rule for serializing TMeta to Bytes                                    
   inline void SerializationRule<Annies::Bytes, RTTI::TMeta>::Serialize(
      RTTI::TMeta const& item, Annies::Bytes& out, Context* context
   ) {
      if (not item) {
         out += uint32_t {0};
         return;
      }

      auto registered = context->mTMetaBank.mDefinitions.find(item);
      if (registered == context->mTMetaBank.mDefinitions.end()) {
         context->mTMetaBank.mDefinitions[item] = context->mTMetaBank.mNextId;
         out += context->mTMetaBank.mNextId;
         ++context->mTMetaBank.mNextId;
      }
      else out += registered->second;
   }

   /// Rule for serializing CMeta to Bytes                                    
   inline void SerializationRule<Annies::Bytes, RTTI::CMeta>::Serialize(
      RTTI::CMeta const& item, Annies::Bytes& out, Context* context
   ) {
      if (not item) {
         out += uint32_t {0};
         return;
      }

      auto registered = context->mCMetaBank.mDefinitions.find(item);
      if (registered == context->mCMetaBank.mDefinitions.end()) {
         context->mCMetaBank.mDefinitions[item] = context->mCMetaBank.mNextId;
         out += context->mCMetaBank.mNextId;
         ++context->mCMetaBank.mNextId;
      }
      else out += registered->second;
   }

   /// Rule for serializing VMeta to Bytes                                    
   inline void SerializationRule<Annies::Bytes, RTTI::VMeta>::Serialize(
      RTTI::VMeta const& item, Annies::Bytes& out, Context* context
   ) {
      if (not item) {
         out += uint32_t {0};
         return;
      }

      auto registered = context->mVMetaBank.mDefinitions.find(item);
      if (registered == context->mVMetaBank.mDefinitions.end()) {
         context->mVMetaBank.mDefinitions[item] = context->mVMetaBank.mNextId;
         out += context->mVMetaBank.mNextId;
         ++context->mVMetaBank.mNextId;
      }
      else out += registered->second;
   }
}
