///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../BlockMap.hpp"


namespace Langulus::Annies
{
   
   /// Check if map has its key type set                                      
   ///   @return true if type is available                                    
   template<CT::Map THIS> LANGULUS(INLINED)
   constexpr bool BlockMap::IsKeyTyped() const noexcept {
      return GetKeys<THIS>().IsTyped();
   }
   
   /// Check if map has its value type set                                    
   ///   @return true if type is available                                    
   template<CT::Map THIS> LANGULUS(INLINED)
   constexpr bool BlockMap::IsValueTyped() const noexcept {
      return GetVals<THIS>().IsTyped();
   }

   /// Check if map has its key type set                                      
   ///   @return true if type is not available                                
   template<CT::Map THIS> LANGULUS(INLINED)
   constexpr bool BlockMap::IsKeyUntyped() const noexcept {
      return GetKeys<THIS>().IsUntyped();
   }
   
   /// Check if map has its value type set                                    
   ///   @return true if type is not available                                
   template<CT::Map THIS> LANGULUS(INLINED)
   constexpr bool BlockMap::IsValueUntyped() const noexcept {
      return GetVals<THIS>().IsUntyped();
   }
   
   /// Check if map has its key type-constrained                              
   ///   @return true if type is constrained                                  
   template<CT::Map THIS> LANGULUS(INLINED)
   constexpr bool BlockMap::IsKeyTypeConstrained() const noexcept {
      return GetKeys<THIS>().IsTypeConstrained();
   }
   
   /// Check if map has its value type-constrained                            
   ///   @return true if type is constrained                                  
   template<CT::Map THIS> LANGULUS(INLINED)
   constexpr bool BlockMap::IsValueTypeConstrained() const noexcept {
      return GetVals<THIS>().IsTypeConstrained();
   }
   
   /// Check if key type is deep                                              
   template<CT::Map THIS> LANGULUS(INLINED)
   constexpr bool BlockMap::IsKeyDeep() const noexcept {
      return GetKeys<THIS>().IsDeep();
   }
   
   /// Check if value type is deep                                            
   template<CT::Map THIS> LANGULUS(INLINED)
   constexpr bool BlockMap::IsValueDeep() const noexcept {
      return GetVals<THIS>().IsDeep();
   }

   /// Check if the key type is a pointer                                     
   template<CT::Map THIS> LANGULUS(INLINED)
   constexpr bool BlockMap::IsKeySparse() const noexcept {
      return GetKeys<THIS>().IsSparse();
   }
   
   /// Check if the value type is a pointer                                   
   template<CT::Map THIS> LANGULUS(INLINED)
   constexpr bool BlockMap::IsValueSparse() const noexcept {
      return GetVals<THIS>().IsSparse();
   }

   /// Check if the key type is not a pointer                                 
   template<CT::Map THIS> LANGULUS(INLINED)
   constexpr bool BlockMap::IsKeyDense() const noexcept {
      return GetKeys<THIS>().IsDense();
   }

   /// Check if the value type is not a pointer                               
   template<CT::Map THIS> LANGULUS(INLINED)
   constexpr bool BlockMap::IsValueDense() const noexcept {
      return GetVals<THIS>().IsDense();
   }

   /// Get the size of a single key, in bytes                                 
   ///   @return the number of bytes a single key contains                    
   template<CT::Map THIS> LANGULUS(INLINED)
   constexpr size_t BlockMap::GetKeyStride() const noexcept {
      return GetKeys<THIS>().GetStride();
   }
   
   /// Get the size of a single value, in bytes                               
   ///   @return the number of bytes a single value contains                  
   template<CT::Map THIS> LANGULUS(INLINED)
   constexpr size_t BlockMap::GetValueStride() const noexcept {
      return GetVals<THIS>().GetStride();
   }

   #if LANGULUS(TESTING)
      /// Get raw key memory pointer, used only in testing                    
      ///   @return the pointer                                               
      LANGULUS(INLINED)
      constexpr const void* BlockMap::GetRawKeysMemory() const noexcept {
         return mKeys.mRaw;
      }

      /// Get raw value memory pointer, used only in testing                  
      ///   @return the pointer                                               
      LANGULUS(INLINED)
      constexpr const void* BlockMap::GetRawValsMemory() const noexcept {
         return mValues.mRaw;
      }
   #endif

   /// Get the key type                                                       
   ///   @return the key type definition                                      
   template<CT::Map THIS> LANGULUS(INLINED)
   DMeta BlockMap::GetKeyType() const noexcept {
      return GetKeys<THIS>().GetType();
   }

   /// Get the value type                                                     
   ///   @return the value type definition                                    
   template<CT::Map THIS> LANGULUS(INLINED)
   DMeta BlockMap::GetValueType() const noexcept {
      return GetVals<THIS>().GetType();
   }

   /// Get the info array (const)                                             
   ///   @return a pointer to the first element inside the info array         
   LANGULUS(INLINED)
   auto BlockMap::GetInfo() const noexcept -> const InfoType* {
      return mInfo;
   }

   /// Get the info array                                                     
   ///   @return a pointer to the first element inside the info array         
   LANGULUS(INLINED)
   auto BlockMap::GetInfo() noexcept -> InfoType* {
      return mInfo;
   }

   /// Get the end of the info array                                          
   ///   @return a pointer to the first element inside the info array         
   LANGULUS(INLINED)
   auto BlockMap::GetInfoEnd() const noexcept -> const InfoType* {
      return mInfo + GetReserved();
   }

   /// Get the key container                                                  
   ///   @attention for internal use only, elements might not be initialized  
   template<CT::Map THIS> LANGULUS(INLINED)
   auto& BlockMap::GetKeys() const noexcept {
      return reinterpret_cast<const Block<typename THIS::Key>&>(mKeys);
   }

   /// Get the templated key container                                        
   ///   @attention for internal use only, elements might not be initialized  
   template<CT::Map THIS> LANGULUS(INLINED)
   auto& BlockMap::GetKeys() noexcept {
      return reinterpret_cast<Block<typename THIS::Key>&>(mKeys);
   }

   /// Get the templated values container                                     
   ///   @attention for internal use only, elements might not be initialized  
   template<CT::Map THIS> LANGULUS(INLINED)
   auto BlockMap::GetVals() const noexcept {
      auto temp = reinterpret_cast<const Block<typename THIS::Value>&>(mValues);
      temp.mCount = mKeys.mCount;
      temp.mReserved = mKeys.mReserved;
      return temp;
   }

   /// Get the number of inserted pairs                                       
   ///   @return the number of inserted pairs                                 
   LANGULUS(INLINED)
   constexpr size_t BlockMap::GetCount() const noexcept {
      return mKeys.mCount;
   }

   /// Get the number of deep key containers                                  
   ///   @return the number of deep key containers                            
   template<CT::Map THIS> LANGULUS(INLINED)
   size_t BlockMap::GetKeyCountDeep() const noexcept {
      return GetCountDeep(GetKeys<THIS>());
   }

   /// Get the number of deep key containers                                  
   ///   @return the number of deep key containers                            
   template<CT::Map THIS> LANGULUS(INLINED)
   size_t BlockMap::GetKeyCountElementsDeep() const noexcept {
      return GetCountElementsDeep(GetKeys<THIS>());
   }

   /// Get the number of deep value containers                                
   ///   @return the number of deep value containers                          
   template<CT::Map THIS> LANGULUS(INLINED)
   size_t BlockMap::GetValueCountDeep() const noexcept {
      return GetCountDeep(GetVals<THIS>());
   }

   /// Get the number of deep value containers                                
   ///   @return the number of deep value containers                          
   template<CT::Map THIS> LANGULUS(INLINED)
   size_t BlockMap::GetValueCountElementsDeep() const noexcept {
      return GetCountElementsDeep(GetVals<THIS>());
   }

   /// Get the key block's state                                              
   ///   @return the key block state                                          
   LANGULUS(INLINED)
   constexpr DataState BlockMap::GetKeyState() const noexcept {
      return mKeys.GetState();
   }
   
   /// Get the value block's state                                            
   ///   @return the value block state                                        
   LANGULUS(INLINED)
   constexpr DataState BlockMap::GetValueState() const noexcept {
      return mValues.GetState();
   }
   
   /// Is key data compressed?                                                
   ///   @return the true if compressed                                       
   LANGULUS(INLINED)
   constexpr bool BlockMap::IsKeyCompressed() const noexcept {
      return mKeys.IsCompressed();
   }
   
   /// Is value data compressed?                                              
   ///   @return the true if compressed                                       
   LANGULUS(INLINED)
   constexpr bool BlockMap::IsValueCompressed() const noexcept {
      return mValues.IsCompressed();
   }
   
   /// Is key data constant?                                                  
   ///   @return the true if constant                                         
   LANGULUS(INLINED)
   constexpr bool BlockMap::IsKeyConstant() const noexcept {
      return mKeys.IsConstant();
   }
   
   /// Is value data constant?                                                
   ///   @return the true if constant                                         
   LANGULUS(INLINED)
   constexpr bool BlockMap::IsValueConstant() const noexcept {
      return mValues.IsConstant();
   }
   
   /// Is key data encrypted?                                                 
   ///   @return the true if encrypted                                        
   LANGULUS(INLINED)
   constexpr bool BlockMap::IsKeyEncrypted() const noexcept {
      return mKeys.IsEncrypted();
   }
   
   /// Is value data encrypted?                                               
   ///   @return the true if encrypted                                        
   LANGULUS(INLINED)
   constexpr bool BlockMap::IsValueEncrypted() const noexcept {
      return mValues.IsEncrypted();
   }

   /// Inner function, for counting nested containers in key or value blocks  
   ///   @param what - the block to scan                                      
   ///   @return the number of found blocks                                   
   size_t BlockMap::GetCountDeep(const CT::Block auto& what) const noexcept {
      if (IsEmpty() or not what.IsDeep())
         return 1;

      size_t counter = 1;
      auto data = what.template GetRawAs<Block>();
      auto info = GetInfo();
      const auto infoEnd = GetInfoEnd();
      while (info != infoEnd) {
         if (*info)
            counter += (data + (info - GetInfo()))->GetCountDeep();
         ++info;
      }
      return counter;
   }

   /// Inner function, for counting nested elements in key or value blocks    
   ///   @param what - the block to scan                                      
   ///   @return the number of found blocks                                   
   size_t BlockMap::GetCountElementsDeep(const CT::Block auto& what) const noexcept {
      if (IsEmpty() or what.IsUntyped())
         return 0;

      if (not what.IsDeep())
         return GetCount();

      size_t counter = 0;
      auto data = what.template GetRawAs<Block>();
      auto info = GetInfo();
      const auto infoEnd = GetInfoEnd();
      while (info != infoEnd) {
         if (*info)
            counter += (data + (info - GetInfo()))->GetCountElementsDeep();
         ++info;
      }

      return counter;
   }

   /// Get the number of allocated pairs                                      
   ///   @return the number of allocated pairs                                
   LANGULUS(INLINED)
   constexpr size_t BlockMap::GetReserved() const noexcept {
      return mKeys.mReserved;
   }

   /// Check if there are any pairs in this map                               
   ///   @return true if there's at least one pair available                  
   LANGULUS(INLINED)
   constexpr bool BlockMap::IsEmpty() const noexcept {
      return mKeys.IsEmpty();
   }

   /// Check if block contains either created elements, or relevant state     
   ///   @return true if block either contains state, or has inserted stuff   
   LANGULUS(INLINED)
   constexpr bool BlockMap::IsValid() const noexcept {
      return not IsEmpty() or mKeys.GetUnconstrainedState()
                           or mValues.GetUnconstrainedState();
   }

   /// Check if block contains no elements and no relevant state              
   ///   @return true if this is an empty stateless container                 
   LANGULUS(INLINED)
   constexpr bool BlockMap::IsInvalid() const noexcept {
      return not IsValid();
   }

   /// Check if the map has been allocated                                    
   ///   @return true if the map uses dynamic memory                          
   LANGULUS(INLINED)
   constexpr bool BlockMap::IsAllocated() const noexcept {
      return mKeys.IsAllocated();
   }
   
   /// Check if keys marked missing                                           
   ///   @return true if the keys are marked missing                          
   LANGULUS(INLINED)
   bool BlockMap::IsKeyMissing() const noexcept {
      return mKeys.IsMissing();
   }
   
   /// Check if values are marked missing                                     
   ///   @return true if the values are marked missing                        
   LANGULUS(INLINED)
   bool BlockMap::IsValueMissing() const noexcept {
      return mValues.IsMissing();
   }
   
   /// Check if the map contains at least one missing entry (nested)          
   ///   @return true if the map has missing entries                          
   template<CT::Map THIS> LANGULUS(INLINED)
   bool BlockMap::IsKeyMissingDeep() const {
      if (IsKeyMissing())
         return true;

      bool missing = false;
      ForEachKeyDeep<false, false, THIS>([&](const Block<>& key) {
         return not (missing = key.IsMissing());
      });
      return missing;
   }
   
   /// Check if the map contains at least one missing entry (nested)          
   ///   @return true if the map has missing entries                          
   template<CT::Map THIS> LANGULUS(INLINED)
   bool BlockMap::IsValueMissingDeep() const {
      if (IsValueMissing())
         return true;

      bool missing = false;
      ForEachValueDeep<false, false, THIS>([&](const Block<>& val) {
         return not (missing = val.IsMissing());
      });
      return missing;
   }
   
   /// Check if keys are executable                                           
   ///   @return true if the keys are executable                              
   template<CT::Map THIS> LANGULUS(INLINED)
   bool BlockMap::IsKeyExecutable() const noexcept {
      return GetKeys<THIS>().IsExecutable();
   }
   
   /// Check if values are executable                                         
   ///   @return true if the values are executable                            
   template<CT::Map THIS> LANGULUS(INLINED)
   bool BlockMap::IsValueExecutable() const noexcept {
      return GetVals<THIS>().IsExecutable();
   }
   
   /// Check if the map contains at least one executable key (nested)         
   ///   @return true if the map has executable entries                       
   template<CT::Map THIS> LANGULUS(INLINED)
   bool BlockMap::IsKeyExecutableDeep() const {
      if (IsKeyExecutable<THIS>())
         return true;

      bool exec = false;
      ForEachKeyDeep<false, true, THIS>([&](const Block<>& key) {
         return not (exec = key.IsExecutable());
      });
      return exec;
   }
   
   /// Check if the map contains at least one executable value (nested)       
   ///   @return true if the map has executable entries                       
   template<CT::Map THIS> LANGULUS(INLINED)
   bool BlockMap::IsValueExecutableDeep() const {
      if (IsValueExecutable<THIS>())
         return true;

      bool exec = false;
      ForEachValueDeep<false, true, THIS>([&](const Block<>& val) {
         return not (exec = val.IsExecutable());
      });
      return exec;
   }

   /// Explicit bool cast operator, for use in if statements                  
   ///   @return true if block contains at least one valid element            
   LANGULUS(INLINED)
   constexpr BlockMap::operator bool() const noexcept {
      return not IsEmpty();
   }

   template<CT::Map THIS>
   void BlockMap::Dump() const {
      const auto tab = Logger::InfoTab("BlockMap::Dump");
      auto info = GetInfo();
      const auto infoEnd = GetInfoEnd();
      while (info != infoEnd) {
         const auto index = info - GetInfo();
         if (*info) {
            if constexpr (Logger::Formattable<typename THIS::Key, typename THIS::Value>) {
               Logger::Info('[', index, "] ",
                  GetKeyRef<THIS>(index), " -> ",
                  GetValRef<THIS>(index)
               );
            }
            else if constexpr (CT::TypeErased<THIS>) {
               Logger::Info('[', index, "] ?? -> ??"/*,
                  Logger::Hex(&GetKeyRef<THIS>(index)), " -> ",
                  Logger::Hex(&GetValRef<THIS>(index))*/
               );
            }
         }
         ++info;
      }
   }
}
