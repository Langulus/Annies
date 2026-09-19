///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../BlockSet.hpp"


namespace Langulus::Annies
{
   
   /// Templated tables are always typed                                      
   ///   @return false                                                        
   template<CT::Set THIS> LANGULUS(INLINED)
   constexpr bool BlockSet::IsTyped() const noexcept {
      return GetValues<THIS>().IsTyped();
   }
   
   /// Templated tables are always typed                                      
   ///   @return false                                                        
   template<CT::Set THIS> LANGULUS(INLINED)
   constexpr bool BlockSet::IsUntyped() const noexcept {
      return GetValues<THIS>().IsUntyped();
   }
   
   /// Templated tables are always type-constrained                           
   ///   @return true                                                         
   template<CT::Set THIS> LANGULUS(INLINED)
   constexpr bool BlockSet::IsTypeConstrained() const noexcept {
      return GetValues<THIS>().IsTypeConstrained();
   }

   /// Check if key type is deep                                              
   template<CT::Set THIS> LANGULUS(INLINED)
   constexpr bool BlockSet::IsDeep() const noexcept {
      return GetValues<THIS>().IsDeep();
   }

   /// Check if the key type is a pointer                                     
   template<CT::Set THIS> LANGULUS(INLINED)
   constexpr bool BlockSet::IsSparse() const noexcept {
      return GetValues<THIS>().IsSparse();
   }

   /// Check if the key type is not a pointer                                 
   template<CT::Set THIS> LANGULUS(INLINED)
   constexpr bool BlockSet::IsDense() const noexcept {
      return GetValues<THIS>().IsDense();
   }

   /// Get the size of a single key, in bytes                                 
   ///   @return the number of bytes a single key contains                    
   template<CT::Set THIS> LANGULUS(INLINED)
   constexpr size_t BlockSet::GetStride() const noexcept {
      return GetValues<THIS>().GetStride();
   }

   /// Get the state of the memory                                            
   ///   @return the state of the value memory                                
   LANGULUS(INLINED)
   constexpr DataState BlockSet::GetState() const noexcept {
      return mKeys.mState;
   }

   /// Get the type of the set                                                
   template<CT::Set THIS> LANGULUS(INLINED)
   DMeta BlockSet::GetType() const noexcept {
      return GetValues<THIS>().GetType();
   }

   /// Get the info array (const)                                             
   ///   @return a pointer to the first element inside the info array         
   LANGULUS(INLINED)
   const BlockSet::InfoType* BlockSet::GetInfo() const noexcept {
      return mInfo;
   }

   /// Get the info array                                                     
   ///   @return a pointer to the first element inside the info array         
   LANGULUS(INLINED)
   BlockSet::InfoType* BlockSet::GetInfo() noexcept {
      return mInfo;
   }

   /// Get the end of the info array                                          
   ///   @return a pointer to the first element inside the info array         
   LANGULUS(INLINED)
   const BlockSet::InfoType* BlockSet::GetInfoEnd() const noexcept {
      return mInfo + GetReserved();
   }

   /// Get the templated values container                                     
   ///   @attention for internal use only, elements might not be initialized  
   template<CT::Set THIS> LANGULUS(INLINED)
   auto& BlockSet::GetValues() const noexcept {
      return reinterpret_cast<const Block<TypeOf<THIS>>&>(mKeys);
   }

   /// Get the templated values container                                     
   ///   @attention for internal use only, elements might not be initialized  
   template<CT::Set THIS> LANGULUS(INLINED)
   auto& BlockSet::GetValues() noexcept {
      return reinterpret_cast<Block<TypeOf<THIS>>&>(mKeys);
   }

   /// Get the number of inserted pairs                                       
   ///   @return the number of inserted pairs                                 
   LANGULUS(INLINED)
   constexpr size_t BlockSet::GetCount() const noexcept {
      return mKeys.GetCount();
   }

   /// Get the number of allocated pairs                                      
   ///   @return the number of allocated pairs                                
   LANGULUS(INLINED)
   constexpr size_t BlockSet::GetReserved() const noexcept {
      return mKeys.GetReserved();
   }

   /// Check if there are any pairs in this map                               
   ///   @return true if there's at least one pair available                  
   LANGULUS(INLINED)
   constexpr bool BlockSet::IsEmpty() const noexcept {
      return mKeys.IsEmpty();
   }
   
   /// Check if set contains either created elements, or relevant state       
   ///   @return true if block either contains state, or has inserted stuff   
   LANGULUS(INLINED)
   constexpr bool BlockSet::IsValid() const noexcept {
      return mKeys.IsValid();
   }
   
   /// Check if set contains no elements and no relevant state                
   ///   @return true if this is an empty stateless container                 
   LANGULUS(INLINED)
   constexpr bool BlockSet::IsInvalid() const noexcept {
      return mKeys.IsInvalid();
   }

   /// Check if the set has been allocated                                    
   ///   @return true if the set uses dynamic memory                          
   LANGULUS(INLINED)
   constexpr bool BlockSet::IsAllocated() const noexcept {
      return mKeys.IsAllocated();
   }

   /// Check if the set is marked as constant                                 
   ///   @return true if the set is marked as constant                        
   LANGULUS(INLINED)
   bool BlockSet::IsConstant() const noexcept {
      return mKeys.IsConstant();
   }
   
   /// Check if the set is marked as compressed                               
   ///   @return true if the set is marked as compressed                      
   LANGULUS(INLINED)
   bool BlockSet::IsCompressed() const noexcept {
      return mKeys.IsCompressed();
   }
   /// Check if the set is marked as encrypted                                
   ///   @return true if the set is marked as encrypted                       
   LANGULUS(INLINED)
   bool BlockSet::IsEncrypted() const noexcept {
      return mKeys.IsEncrypted();
   }
   
   /// Check if the set is marked missing                                     
   ///   @return true if the set is marked as missing                         
   LANGULUS(INLINED)
   bool BlockSet::IsMissing() const noexcept {
      return mKeys.IsMissing();
   }
   
   /// Check if the set contains at least one missing entry (nested)          
   ///   @return true if the set has missing entries                          
   template<CT::Set THIS> LANGULUS(INLINED)
   bool BlockSet::IsMissingDeep() const {
      bool missing = false;
      ForEachDeep([&](const Block<>& value) {
         missing = value.IsMissing();
         return not missing;
      });

      return missing;
   }
   
   /// Check if a type can be inserted to this block                          
   ///   @param other - check if a given type is insertable to this block     
   ///   @return true if able to insert an instance of the type to this block 
   template<CT::Set THIS> LANGULUS(INLINED)
   constexpr bool BlockSet::IsInsertable(DMeta other) const noexcept {
      return GetValues<THIS>().IsInsertable(other);
   }
   
   /// Check if a static type can be inserted                                 
   ///   @tparam T - the type to check                                        
   ///   @return true if able to insert an instance of the type to this block 
   template<CT::NotVoid T, CT::Set THIS> LANGULUS(INLINED)
   constexpr bool BlockSet::IsInsertable() const noexcept {
      return GetValues<THIS>().template IsInsertable<T>();
   }

   /// Check if the memory for the table is owned by us                       
   ///   @return the allocation pointer                                       
   LANGULUS(INLINED)
   constexpr auto BlockSet::GetAllocation() const noexcept -> const Allocation* {
      return mKeys.mEntry;
   }

   /// Get the number of references for the allocated memory                  
   ///   @attention always returns zero if we don't have authority            
   ///   @return the number of references                                     
   LANGULUS(INLINED)
   constexpr size_t BlockSet::GetUses() const noexcept {
      return mKeys.GetUses();
   }

#if LANGULUS(TESTING)
   /// Get raw key memory pointer, used only in testing                       
   ///   @return the pointer                                                  
   LANGULUS(INLINED)
   constexpr const void* BlockSet::GetRawMemory() const noexcept {
      return mKeys.mRaw;
   }

   /// Get allocation, used only in testing                                   
   ///   @return the allocation entry                                         
   LANGULUS(INLINED)
   const Allocation* BlockSet::GetEntry() const noexcept {
      return mKeys.mEntry;
   }
#endif
      
   /// Explicit bool cast operator, for use in if statements                  
   ///   @return true if block contains at least one valid element            
   LANGULUS(INLINED)
   constexpr BlockSet::operator bool() const noexcept {
      return not IsEmpty();
   }

#if LANGULUS(DEBUG)
   template<CT::Set THIS>
   void BlockSet::Dump() const {
      const auto tab = Logger::Section("BlockSet::Dump:");
      auto info = GetInfo();
      const auto infoEnd = GetInfoEnd();
      while (info != infoEnd) {
         const auto index = info - GetInfo();
         if (*info)
            Logger::Info('[', index, "] -", (*info-1), " -> ", GetRaw<THIS>(index).GetHash());
         else
            Logger::Info('[', index, "] empty");

         ++info;
      }
   }
#endif

} // namespace Langulus::Annies
