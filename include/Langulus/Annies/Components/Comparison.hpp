///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Container.hpp"
#include <Langulus/IntentOf.hpp>
#include "Langulus/CT/Serializer.hpp"
#include <Langulus/CT/Character.hpp>
#include <Langulus/CT/Comparable.hpp>
#include <Langulus/CT/Index.hpp>
#include <Langulus/CT/Text.hpp>
#include <Langulus/CT/Unfold.hpp>
#include <Langulus/CT/Deep.hpp>
#include <Langulus/RTTI/MetaData.hpp>


#if 0 or LANGULUS_META_VERBOSITY_MASTER_SWITCH()
   #include <Langulus/Logger/EnableVerbose.hpp>
#else
   #include <Langulus/Logger/NoVerbose.hpp>
#endif


namespace Langulus::Annies
{
   struct Text;
}

namespace Langulus::Annies::Component
{
   ///                                                                        
   /// Implements comparison for containers. This includes functions for      
   /// searching and pattern-matching.                                        
   ///   @tparam CONVERT whether to serialize as self before comparing range. 
   ///      Useful for byte and text containers. Use 'false' to compare       
   ///      without serialization.                                            
   ///   @tparam HASH whether to compare hashes before elements. This is      
   ///      mostly useful when hash is cachable, otherwise kind of pointless. 
   ///   @tparam ID, SHARED heaps/stacks we're comparing                      
   template<bool CONVERT, bool HASH, Cid ID, Cid...SHARED>
   struct Comparison {
      using CTTI_Component = Yup;
      using CTTI_ReflectAs = void;
      using Id = Values<ID, SHARED...>;
      
      static constexpr int  ComponentPrecedence = 3000;
      static constexpr bool Shared = (sizeof...(SHARED) > 0);
      template<Cid SID>
      static constexpr bool Relevant = Id::template Contains<SID>;

   private:
      LglsComMerging(friend);

      template<CT::Container C>
      using Count = typename Deref<C>::CountType;

      template<CT::Container C>
      using Ordering = Tif<CT::TypeErased<C>, Compared, ::std::partial_ordering>;

   public:
      /// MARK: CompareEqual                                                  
      /// Compare two containers for equality.                                
      /// This has much greater performance when hashed.                      
      ///   @attention compares all shared dimensions at once                 
      ///   @param lhs left container                                         
      ///   @param rhs right container                                        
      ///   @return true if the two containers are identical                  
      template<Cid SID = ID, CT::Container LHS, CT::Container RHS> requires Relevant<SID>
      constexpr bool CompareEqual(this const LHS& lhs, const RHS& rhs) {
         if consteval {
            // Heap should be empty at compile-time                     
            //TODO what about stacks??
            return true;
         }
         else {
            LglsVerboseScoped("Comparing ",
               Logger::White, lhs.GetCount(), "x of ", lhs.GetName(),
               Logger::Reset, " with ",
               Logger::White, rhs.GetCount(), "x of ", rhs.GetName()
            );

            IF_NOT_LANGULUS_FORCE_TYPE_ERASURE(if constexpr (CT::TypeErased<LHS> or CT::TypeErased<RHS>) {)
               //                                                       
               // Both containers are type-erased - all we can do is    
               // call the reflected comparison functions               
               const auto lhs_count = lhs.GetCount();
               const auto rhs_count = rhs.GetCount();
               if (lhs_count != rhs_count) {
                  LglsVerbose(Logger::Red, "Different count (type-erased): ",
                     lhs_count, " != ", rhs_count);
                  return false;
               }

               if (not lhs_count)
                  return true;   // Both empty                          

               const RTTI::DMeta LT = lhs.template GetType<SID>();
               const RTTI::DMeta RT = rhs.template GetType<SID>();
               if (not LT.IsSame(RT)) { //TODO but what if differently typed pointers to the same virtual objects?
                  LglsVerbose(Logger::Red, "Types differ (type-erased): ",
                     LT, " != ", RT);
                  return false;
               }

               const auto comparer = LT.GetComparerEqual();
               if (not comparer) {
                  LglsVerbose(Logger::Red, "Type not comparable (type-erased): ", LT);
                  return false;
               }
               
               const auto raw1 = lhs.GetRaw();
               const auto raw2 = rhs.GetRaw();
               if (raw1 == raw2)
                  return true;   // Both point to same memory           

               if constexpr (requires { lhs.CompareHashes(rhs); }) {
                  if (LT.GetHasher() and not lhs.CompareHashes(rhs)) {
                     // Early failure if valid hashes differ - no point 
                     // in comparing anything at all                    
                     LglsVerbose(Logger::Red, "Different hashes (type-erased): ",
                        Logger::Hex(lhs.GetHash()), " != ", Logger::Hex(rhs.GetHash()));
                     return false;
                  }
               }

               if constexpr (CT::Contiguous<LHS, RHS>) {
                  if (LT.IsPOD()) {
                     // Batch-compare memory if POD or sparse           
                     const bool same = (0 == ::std::memcmp(raw1, raw2, lhs.GetBytesize()));
                     if (not same) {
                        LglsVerbose(Logger::Red,
                           "Different POD memory after memcmp (type-erased)");
                        LglsVerbose(Logger::Red,
                           "Most likely padding bytes filled with junk - pack your struct: ", LT);
                     }
                     return same;
                  }
               }

               // Use comparison operator between all elements          
               bool result = true;
               auto t2 = rhs.GetHandle();
               lhs.template Apply<false>([&](auto&& t1) -> bool {
                  if constexpr (CT::Supported<decltype(t1)>) {
                     if constexpr (not CT::Contiguous<RHS>) {
                        // Make sure hash table spot is valid           
                        const auto idx = t1 - lhs.GetHandle();
                        if (not rhs.GetHashTable()[idx]) {
                           LglsVerbose(Logger::Red,
                              "Element #", idx, " has no hash table equivalent (typed)");
                           return (result = false);
                        }
                     }

                     if (not comparer (t1.GetRaw(), t2.GetRaw())) {
                        // Make sure all elements match                 
                        LglsVerbose(Logger::Red,
                           "Element #", t1 - lhs.GetHandle(), " differs (typed)");
                        return (result = false);
                     }
                  }
                  else if constexpr (not CT::Contiguous<RHS>) {
                     // Spots on tables must both match                 
                     const auto idx = t2 - rhs.GetHandle();
                     if (rhs.GetHashTable()[idx]) {
                        LglsVerbose(Logger::Red,
                           "Element #", idx, " has no hash table equivalent (typed)");
                        return (result = false);
                     }
                  }

                  ++t2;
                  return true;
               });

               return result;
            #if not LANGULUS(FORCE_TYPE_ERASURE)
            } else {
               //                                                       
               // Both containers are statically-typed - leverage it by 
               // using static comparisons                              
               using LT = TypeOf<LHS>;
               using RT = TypeOf<RHS>;

               const auto lhs_count = lhs.GetCount();
               const auto rhs_count = rhs.GetCount();
               if (lhs_count != rhs_count) {
                  // Early failure if count differs, no point in        
                  // comparing anything at all                          
                  LglsVerbose(Logger::Red, "Different count (typed): ",
                     lhs.GetCount(), " != ", rhs.GetCount());
                  return false;
               }

               if (not lhs_count)
                  return true;   // Both empty                          

               if constexpr (not Same<LT, RT>) { //TODO but what if differently typed pointers to the same virtual objects?
                  // Types are different                                
                  LglsVerbose(Logger::Red, "Types differ (typed): ",
                     NameOf<LT>(), " != ", NameOf<RT>());
                  return false;
               }
               else {
                  if constexpr (CT::ComparableEqual<LT, LT>) {
                     const auto raw1 = lhs.GetRaw();
                     const auto raw2 = rhs.GetRaw();
                     if (raw1 == raw2)
                        return true;   // Both point to same memory     

                     if constexpr (HASH and CT::Hashable<LT, RT>) {
                        if (not lhs.CompareHashes(rhs)) {
                           // Early failure if valid hashes differ - no 
                           // point in comparing anything at all        
                           LglsVerbose(Logger::Red, "Different hashes (typed): ",
                              Logger::Hex(lhs.GetHash()), " != ", Logger::Hex(rhs.GetHash()));
                           return false;
                        }
                     }

                     if constexpr (CT::POD<LT> and CT::Contiguous<LHS, RHS>) {
                        // Batch compare POD data, including pointers   
                        const bool same = (0 == ::std::memcmp(raw1, raw2, lhs.GetBytesize()));
                        if (not same) {
                           LglsVerbose(Logger::Red,
                              "Different POD memory after memcmp (typed)");
                           LglsVerbose(Logger::Red,
                              "Most likely padding bytes filled with junk - pack your struct: ", NameOf<LT>());
                        }
                        return same;
                     }

                     // Use comparison operator between all elements    
                     bool result = true;
                     auto t2 = rhs.GetHandle();
                     lhs.template Apply<false>([&](auto&& t1) -> bool {
                        if constexpr (CT::Supported<decltype(t1)>) {
                           if constexpr (not CT::Contiguous<RHS>) {
                              // Make sure hash table spot is valid     
                              const auto idx = t1 - lhs.GetHandle();
                              if (not rhs.GetHashTable()[idx]) {
                                 LglsVerbose(Logger::Red,
                                    "Element #", idx, " has no hash table equivalent (typed)");
                                 return (result = false);
                              }
                           }

                           if (*t1.GetRaw() != *t2.GetRaw()) {
                              // Make sure all elements match           
                              LglsVerbose(Logger::Red,
                                 "Element #", t1 - lhs.GetHandle(), " differs (typed)");
                              return (result = false);
                           }
                        }
                        else if constexpr (not CT::Contiguous<RHS>) {
                           // Spots on tables must both match           
                           const auto idx = t2 - rhs.GetHandle();
                           if (rhs.GetHashTable()[idx]) {
                              LglsVerbose(Logger::Red,
                                 "Element #", idx, " has no hash table equivalent (typed)");
                              return (result = false);
                           }
                        }

                        if_available(++t2);
                        return true;
                     });

                     return result;
                  }
                  else {
                     LglsVerbose(Logger::Red, "Type not comparable (typed): ", NameOf<LT>());
                     return false;
                  }
               }
            }
            #endif
         }
      }
      
      /// MARK: Compare                                                       
      /// Three-way compare two containers                                    
      ///   @attention compares all shared dimensions at once                 
      ///   @attention this doesn't benefit from hashing and will three-way   
      ///      compare all elements until short-circuited                     
      ///   @return the ordering result                                       
      template<CT::Container LHS, CT::Container RHS>
      constexpr auto Compare(this const LHS& lhs, const RHS& rhs) /*-> Ordering<LHS>*/ {
         LglsVerboseScoped("Comparing ",
            Logger::White, lhs.GetCount(), "x of ", lhs.GetName(),
            Logger::Reset, " with ",
            Logger::White, rhs.GetCount(), "x of ", rhs.GetName()
         );

         IF_NOT_LANGULUS_FORCE_TYPE_ERASURE(if constexpr (CT::TypeErased<LHS> or CT::TypeErased<RHS>) {)
            //                                                          
            // Both container are type-erased - all we can do is call   
            // the reflected comparison functions                       
            const auto lhs_count = lhs.GetCount();
            const auto rhs_count = rhs.GetCount();
            if (lhs_count != rhs_count) {
               LglsVerbose(Logger::Red, "Different count (type-erased): ",
                  lhs_count, " != ", rhs_count);
               return Compared::Unordered;
            }

            if (not lhs_count)
               return Compared::Equal;    // Both empty                 

            const RTTI::DMeta LT = lhs.GetType();
            const RTTI::DMeta RT = rhs.GetType();
            if (not LT.IsSame(RT)) { //TODO but what if differently typed pointers to the same virtual objects?
               LglsVerbose(Logger::Red, "Types differ (type-erased): ",
                  LT, " != ", RT);
               return Compared::Unordered;
            }
            
            const auto comparer = LT.GetComparer();
            if (comparer) {
               auto t1 = lhs.template GetRawAs<uint8_t>();
               auto t2 = rhs.template GetRawAs<uint8_t>();
               if (t1 == t2) {
                  // Both point to the same memory and have same        
                  // count. Notice that this is valid optimization only 
                  // when the types are comparable. If you have weird   
                  // types that can be different despite occupying same 
                  // memory, you'll have to delete their comparison     
                  // operator.                                          
                  return Compared::Equal;
               }

               // Call compare operator for each element pair           
               [[maybe_unused]] const auto t1_start = t1;
               const auto size = LT.GetSize();
               const auto t1end = t1 + lhs_count * size;
               while (t1 < t1end) {
                  const Compared last_compare = comparer(t1, t2);
                  if (last_compare != Compared::Equal) {
                     LglsVerbose(Logger::Red,
                        "Element #", (t1 - t1_start) / size, " differs (type-erased)");
                     return last_compare;
                  }

                  t1 += size;
                  t2 += size;
               }
               return Compared::Equal;
            }

            LglsVerbose(Logger::Red, "Type not comparable (type-erased): ", LT);
            return Compared::Unordered;
         #if not LANGULUS(FORCE_TYPE_ERASURE)
         } else {
            //                                                          
            // Both blocks are statically-typed - leverage it by using  
            // static comparisons                                       
            const auto lhs_count = lhs.GetCount();
            const auto rhs_count = rhs.GetCount();
            if (lhs_count != rhs_count) {
               // Early failure if count differs, no point in        
               // comparing anything at all                          
               LglsVerbose(Logger::Red, "Different count (typed): ",
                  lhs_count, " != ", rhs_count);
               return ::std::partial_ordering::unordered;
            }

            if (not lhs_count) {
               // Both empty                                         
               return ::std::partial_ordering::equivalent;
            }

            using LT = TypeOf<LHS>;
            using RT = TypeOf<RHS>;
            if constexpr (not Same<LT, RT>) { //TODO but what if differently typed pointers to the same virtual objects?
               // Types are different                                   
               LglsVerbose(Logger::Red, "Types differ (typed): ",
                  NameOf<LT>(), " != ", NameOf<RT>());
               return ::std::partial_ordering::unordered;
            }
            else {
               if constexpr (CT::Comparable<LT, LT>) {
                  auto t1 = lhs.GetRaw();
                  auto t2 = rhs.GetRaw();
                  if (t1 == t2) {
                     // Both point to the same memory and have same     
                     // count. Notice that this is valid only when      
                     // the types are comparable. If you have weird     
                     // types that can be different despite occupying   
                     // same memory, you'll have to delete their        
                     // comparison operator.                            
                     return ::std::partial_ordering::equivalent;
                  }

                  // Use comparison operator between all elements       
                  const auto t1end = t1 + lhs_count;
                  auto last_compare = ::std::partial_ordering::unordered;
                  while (t1 < t1end and ((last_compare = ToPartialOrdering(*t1 <=> *t2)) == ::std::partial_ordering::equivalent)) {
                     ++t1;
                     ++t2;
                  }

                  if (t1 != t1end) {
                     LglsVerbose(Logger::Red,
                        "Element #", t1 - lhs.GetRaw(), " differs (typed)");
                  }
                  return last_compare;
               }
               else {
                  LglsVerbose(Logger::Red,
                     "Type not comparable (typed): ", NameOf<LT>());
                  return ::std::partial_ordering::unordered;
               }
            }
         }
         #endif
      }

      /// MARK: CompareOneEqual                                               
      /// Equality-compare with the first contained element                   
      ///   @attention compares only the main dimension                       
      ///   @param rhs the value to compare against                           
      ///   @return true if elements are the same                             
      template<CT::Container C, CT::NoIntent RT>
      constexpr bool CompareOneEqual(this C const& self, const RT& rhs) {
         if consteval {
            // Heap should be empty at compile-time                     
            //TODO what about stack-based containers?
            return false;
         }
         else {
            if (self.GetCount() != 1)
               return false;

            if constexpr (CT::TypeErased<C>) {
               if (not self.IsTyped())
                  return false;
            }

            return self.template CompareOneEqualInner<Id::First>(rhs);
         }
      }

      /// MARK: CompareOne                                                    
      /// Three-way compare with the first contained element                  
      ///   @attention compares only the main dimension                       
      ///   @attention this doesn't benefit from hashing                      
      ///   @param rhs the value to compare against                           
      ///   @return true if elements are the same                             
      template<CT::Container C, CT::NoIntent RT>
      constexpr auto CompareOne(this C const& self, const RT& rhs) -> Ordering<C> {
         if constexpr (CT::TypeErased<C>) {
            if (self.GetCount() != 1)
               return Compared::Unordered;

            if (not self.IsTyped())
               return Compared::Unordered;

            return self.template CompareOneInner<Id::First>(rhs);
         }
         else {
            if (self.GetCount() != 1)
               return ::std::partial_ordering::unordered;
            
            return self.template CompareOneInner<Id::First>(rhs);
         }
      }

      /// MARK: CompareOneEqualEx                                             
      /// Equality-compare with the first contained element                   
      ///   @attention compares only the main dimension                       
      ///   @param rhs the value to compare against                           
      ///   @return true if elements are the same                             
      template<CT::Container C, CT::NoIntent RT> requires CT::ContainsOne<RT>
      constexpr bool CompareOneEqualEx(this C const& self, const RT& rhs) {
         if consteval {
            // Heap should be empty at compile-time                     
            //TODO what about stack-based containers?
            return false;
         }
         else {
            using RELEVANT = typename Id::template Intersect<typename RT::Dimensions>;
            if (self.template GetCount<RELEVANT::First>() != 1)
               return false;

            auto rhs_handle = rhs.GetHandle();
            return RELEVANT::ForEachAnd([&self, &rhs_handle]<Cid D> {
               if constexpr (CT::TypeErased<C> or CT::TypeErased<RT>) {
                  auto type = self.template GetType<D>();
                  return type.IsSame(rhs_handle.template GetType<D>())
                     and type.GetComparerEqual()
                     and self.template CompareOneEqualInner<D>(rhs_handle);
               }
               else return self.template CompareOneEqualInner<D>(rhs_handle);
            });
         }
      }

      /// MARK: CompareOneEx                                                  
      /// Three-way compare with the first contained element                  
      ///   @attention compares only the main dimension                       
      ///   @attention this doesn't benefit from hashing                      
      ///   @param rhs the value to compare against                           
      ///   @return true if elements are the same                             
      template<CT::Container C, CT::NoIntent RT> requires CT::ContainsOne<RT>
      constexpr auto CompareOneEx(this C const& self, const RT& rhs) {
         using RELEVANT = typename Id::template Intersect<typename RT::Dimensions>;

         IF_NOT_LANGULUS_FORCE_TYPE_ERASURE(if constexpr (CT::TypeErased<C, RT>) {)
            if (self.template GetCount<RELEVANT::First>() != 1)
               return Compared::Unordered;

            auto result = Compared::Unordered;
            auto rhs_handle = rhs.GetHandle();
            RELEVANT::ForEachAnd([&self, &rhs_handle, &result]<Cid D> {
               auto type = self.template GetType<D>();
               if (not type.IsSame(rhs_handle.template GetType<D>())
               or  not type.GetComparer())
                  return false; // Short circuit                        
               
               // Continue comparing until a dimension differs          
               result = self.template CompareOneInner<D>(rhs_handle);
               return result == Compared::Equal or result == Compared::Equivalent;
            });
            return result;
         #if not LANGULUS(FORCE_TYPE_ERASURE)
         } else {
            if (self.template GetCount<RELEVANT::First>() != 1)
               return ::std::partial_ordering::unordered;
            
            auto result = ::std::partial_ordering::unordered;
            auto rhs_handle = rhs.GetHandle();
            RELEVANT::ForEachAnd([&self, &rhs_handle, &result]<Cid D> {
               // Continue comparing until a dimension differs          
               result = self.template CompareOneInner<D>(rhs_handle);
               return result == ::std::partial_ordering::equivalent;
            });
            return result;
         }
         #endif
      }

      /// MARK: CompareHashes                                                 
      /// Compare hashes of two containers.                                   
      /// Most useful when hashes are cached, as it will otherwise force      
      /// HashRecompute every time this comparison happens.                   
      ///   @attention compares all shared dimensions at once                 
      ///   @return true if hashes are the same                               
      template<CT::Container LHS, CT::Container RHS>
      constexpr bool CompareHashes(this LHS const& lhs, RHS const& rhs)
      requires (HASH and requires { lhs.GetHash(); rhs.GetHash(); }) {
         return lhs.GetHash() == rhs.GetHash();
      }
      
      template<CT::Container C1, CT::Container C2>
      auto Matches(this const C1&, const C2&) noexcept -> Count<C1>;

      template<CT::Container C1, CT::Container C2> requires CT::Character<TypeOf<C1>, TypeOf<C2>>
      bool CompareLoose(this const C1&, const C2&) noexcept;
      template<CT::Container C1, CT::Container C2> requires CT::Character<TypeOf<C1>, TypeOf<C2>>
      auto MatchesLoose(this const C1&, const C2&) noexcept -> Count<C1>;
      
      /// MARK: Find                                                          
      /// Get a handle to a matching item                                     
      ///   @attention compares only the chosen dimension                     
      ///   @param item the item to search for                                
      ///   @param cookie resume search from a given index                    
      ///   @return handle of the found item                                  
      template<bool REVERSE = false, Cid SID = ID, CT::ContainsMany C, CT::NoIntent T>
      requires (CT::Contiguous<C> or not REVERSE)
      auto Find(this C&& self, T const& item, size_t cookie = 0) assumptious
      -> DecideHandle<C> {
         if (self.template IsEmpty<SID>())
            return {};

         if constexpr (CT::TypeErased<C>) {
            auto type = self.template GetType<SID>();
            if (not type.GetComparerEqual())
               return {};

            if constexpr (CT::Handle<T>) {
               if (not type.IsSame(item.template GetType<SID>()))
                  return {};
            }
            else {
               if (not type.IsSame(MetaDataOf<T>()))
                  return {};
            }
         }
         
         if constexpr (CT::IndexedTable<C>) {
            (void) cookie;
            return self.template TableSearch<SID>(item);
         }
         else return self.template FindInner<REVERSE, SID>(item, cookie);
      }

      /// MARK: FindReverse                                                   
      /// Get a handle to a matching item in reverse                          
      ///   @attention compares only the chosen dimension                     
      ///   @param item the item to search for                                
      ///   @param cookie resume search from a given index                    
      ///   @return handle of the found item                                  
      template<Cid SID = ID, CT::ContainsMany C, CT::NoIntent T> requires CT::Contiguous<C>
      auto FindReverse(this C&& self, T const& item, size_t cookie = 0) assumptious
      -> DecideHandle<C> {
         return self.template Find<true, SID, C, T>(item, cookie);
      }
      
      /// MARK: FindEx                                                        
      /// Get a handle to a matching multidimensional pattern                 
      ///   @attention compares all shared dimensions                         
      ///   @param tuple the multidimensional item to search for              
      ///   @param cookie resume search from a given index                    
      ///   @return the handle of the found item                              
      template<bool REVERSE = false, CT::ContainsMany C, CT::NoIntent T>
      requires (CT::ContainsOne<T> and (CT::Contiguous<C> or not REVERSE))
      auto FindEx(this C&& self, T const& item, size_t cookie = 0) assumptious
      -> DecideHandle<C> {
         using RELEVANT = typename Id::template Intersect<typename T::Dimensions>;
         if (self.template IsEmpty<RELEVANT::First>())
            return {};

         if constexpr (CT::TypeErased<C>) {
            if (not RELEVANT::ForEachAnd([&self,&item]<Cid D> {
               auto type = self.template GetType<D>();
               return type.IsSame(item.template GetType<D>())
                  and type.GetComparerEqual();
            })) return {};
         }

         // Find the first relevant dimension                           
         auto first_rhs = item.GetHandle();

         DecideHandle<C> first_lhs;
         if constexpr (CT::IndexedTable<C>) {
            (void) cookie;
            first_lhs = self.template TableSearch<RELEVANT::First>(first_rhs);
         }
         else first_lhs = self.template FindInner<REVERSE, RELEVANT::First>(first_rhs, cookie);

         if constexpr (RELEVANT::Count == 1)
            return first_lhs;
         else if (first_lhs) {
            // Compare the rest of the dimensions with the first handle 
            if (RELEVANT::Expand([&first_lhs, &first_rhs]<Cid, Cid...DN> {
               return (first_lhs.template CompareOneEqualInner<DN>(first_rhs) and ...);
            }))
               return first_lhs;
            else
               return {};
         }
         else return {};
      }

      /// MARK: FindExReverse                                                 
      /// Get a handle to a matching multidimensional pattern in reverse      
      ///   @attention compares all shared dimensions                         
      ///   @param tuple the multidimensional item to search for              
      ///   @param cookie resume search from a given index                    
      ///   @return the handle of the found item                              
      template<CT::ContainsMany C, CT::NoIntent T>
      requires (CT::ContainsOne<T> and CT::Contiguous<C>)
      auto FindExReverse(this C&& self, T const& item, size_t cookie = 0) assumptious
      -> DecideHandle<C> {
         return self.template FindEx<true, C, T>(item, cookie);
      }

      /// Find a matching sequence of one or more matching elements           
      ///   @attention compares only the chosen dimension                     
      ///   @tparam REVERSE true to perform search in reverse                 
      ///   @param range sequence of items to search for                      
      ///   @param cookie resume search from a given index                    
      ///   @return handle of the found item                                  
      template<bool REVERSE = false, Cid SID = ID, CT::ContainsMany C, CT::ContainsMany T>
      requires (CT::NoIntent<T> and CT::IndexedLinearly<C, T>)
      auto FindRange(this C const& self, T const& range, size_t cookie = 0) assumptious 
      -> DecideHandle<C> {
         const size_t pool_size = self.GetCount() - cookie;
         const size_t range_size = range.GetCount();
         if (not range_size or range_size > pool_size)
            return {};

         using T1 = TypeOf<C, SID>;
         using T2 = TypeOf<T, SID>;
         if constexpr (not CT::TypeErased<C, T> and not CT::ComparableEqual<T1, T2>)
            // Early exit if statically incompatible                    
            return {};
         else {
            [[maybe_unused]] RTTI::DefinitionData::FCompareEqual comparer;
            [[maybe_unused]] const size_t bytesize = range.template GetBytesize<SID>();

            if constexpr (CT::TypeErased<C> or CT::TypeErased<T>) {
               // Runtime type check                                    
               auto type = self.template GetType<SID>();
               comparer = type.GetComparerEqual();
               if (not comparer or not type.IsSame(range.template GetType<SID>()))
                  return {};
            }
            
            const auto head = range.GetHandle();
            //const auto last = self.GetHandle() + pool_size;

            while(cookie <= pool_size - range_size /*h and h.GetRaw() < last.GetRaw()*/) {
               const auto h = self.template FindInner<REVERSE, SID>(head, cookie);
               if (not h)
                  return {};

               cookie = h - self.GetHandle();

               if constexpr (not CT::TypeErased<C> and not CT::TypeErased<T>) {
                  if constexpr (CT::BinaryCompatible<T1, T2> and CT::POD<T1, T2>) {
                     // We can use batch-compare                        
                     if (0 == memcmp(h.GetRaw(), range.GetRaw(), bytesize))
                        return h;               // Found                
                  }
                  else {
                     // Types are not batch-comparable, so compare them 
                     // one by one                                      
                     auto lhs = h;
                     auto rhs = head;
                     const auto rhsEnd = head + range_size;
                     while (rhs.GetRaw() != rhsEnd.GetRaw() and *lhs == *rhs) {
                        ++lhs;
                        ++rhs;
                     }

                     if (rhs == rhsEnd)
                        return h;               // Found                
                  }
               }
               else {
                  if (self.template IsPOD<SID>()) {
                     // We can use batch-compare                        
                     if (0 == memcmp(h.GetRaw(), range.GetRaw(), bytesize))
                        return h;               // Found                
                  }
                  else {
                     // Types are not batch-comparable, so compare them 
                     // one by one                                      
                     auto lhs = h;
                     auto rhs = head;
                     const auto rhsEnd = head + range_size;
                     while (rhs.GetRaw() != rhsEnd.GetRaw() and comparer(rhs.GetRaw(), rhsEnd.GetRaw())) {
                        ++lhs;
                        ++rhs;
                     }

                     if (rhs == rhsEnd)
                        return h;               // Found                
                  }
               }

               //h = self.template FindInner<REVERSE, SID>(head, cookie + 1);
               ++cookie;
            }

            // If reached, then range wasn't found                      
            return {};
         }
      }

      /// MARK: Contains                                                      
      /// Check if the container contains an element                          
      ///   @attention compares only the main dimension                       
      ///   @param A1 the item to search for                                  
      ///   @return true if item was found in the main dimension              
      template<CT::Container C, CT::NoIntent A1>
      bool Contains(this C const& self, A1 const& a1) {
         if constexpr (CT::ContainsMany<C>)
            return static_cast<bool>(self.Find(a1));
         else
            return self.CompareOneEqual(a1);
      }

      /// MARK: ContainsRange                                                 
      /// Check if the container contains a sequence of elements              
      ///   @attention compares only the main dimension                       
      ///   @param A1 the sequence of items to search for                     
      ///   @return true if item was found in the main dimension              
      template<CT::ContainsMany C, /*CT::ContainsMany*/class A1> requires CT::NoIntent<A1>
      bool ContainsRange(this C const& self, A1 const& a1) {
         if constexpr (CONVERT) {
            if constexpr (CT::Serializer<C>) {
               C converted;
               Langulus::Serialize(a1, converted);
               return static_cast<bool>(self.FindRange(converted));
            }
            else {
               const C converted = Langulus::Convert<C>(a1);
               return static_cast<bool>(self.FindRange(converted));
            }
         }
         else {
            static_assert(CT::Container<A1>, "Argument must be a container");
            return static_cast<bool>(self.FindRange(a1));
         }
      }

      /// MARK: ContainsEx                                                    
      /// Check if the container contains an element in each shared dimension 
      ///   @attention compares all shared dimensions                         
      ///   @param tuple the items that need to exist together                
      ///   @return true if all provided dimensions are found together        
      template<CT::Container C, CT::NoIntent A> requires CT::ContainsOne<A>
      bool ContainsEx(this C const& self, A const& tuple) {
         if constexpr (CT::ContainsMany<C>)
            return static_cast<bool>(self.FindEx(tuple));
         else
            return self.CompareOneEqualEx(tuple);
      }

      /// MARK: <=>                                                           
      ///   @attention compares all shared dimensions at once                 
      template<CT::Container LHS, CT::NoIntent RHS> requires CT::CompatibleDimensions<LHS, RHS>
      constexpr auto operator <=> (this LHS const& lhs, RHS const& rhs) assumptious {
         if constexpr (Same<LHS, RHS>)
            return lhs.Compare(rhs);
         else if constexpr (CT::DeepDense<RHS>) {
            LglsAssumeUser((CT::Typed<LHS> and Same<TypeOf<LHS>, TypeOf<RHS>>),
               "Ambiguous use of three-way comparison "
               "- you should use either Compare (if you want to compare "
               "containers) or CompareOne (if you want to compare the "
               "first item) in order to clearly state your intent. "
               "Compare will be used by default!"
            );
            return lhs.Compare(rhs);
         }
         else return lhs.CompareOne(rhs);
      }

      /// MARK: ==                                                            
      ///   @attention compares all shared dimensions at once                 
      template<CT::Container LHS, CT::NoIntent RHS> requires CT::CompatibleDimensions<LHS, RHS>
      constexpr bool operator == (this LHS const& lhs, RHS const& rhs) assumptious {
         if constexpr (Same<LHS, RHS>)
            return lhs.CompareEqual(rhs);
         else if constexpr (CT::DeepDense<RHS>) {
            LglsAssumeUser((CT::Typed<LHS> and Same<TypeOf<LHS>, TypeOf<RHS>>),
               "Ambiguous use of equality comparison "
               "- you should use either CompareEqual (if you want to compare "
               "containers) or CompareOneEqual (if you want to compare the "
               "first item) in order to clearly state your intent. "
               "CompareEqual will be used by default!"
            );
            return lhs.CompareEqual(rhs);
         }
         else return lhs.CompareOneEqual(rhs);
      }

   protected:
      /// MARK: Protected                                                     
      /// Find a single element's index inside container (inner)              
      ///   @tparam REVERSE true to perform search in reverse                 
      ///   @attention assumes container is not empty                         
      ///   @attention assumes that container is of the same comparable type  
      ///   @attention operates on a single dimension at a time               
      ///   @attention works in linearly indexed containers only              
      ///   @param item the item to search for                                
      ///   @param cookie resume search from a given index                    
      ///   @return handle of the found item                                  
      template<bool REVERSE = false, Cid SID = ID, CT::IndexedLinearly C, CT::NoIntent T>
      auto FindInner(this C&& self, T const& item, size_t cookie) assumptious
      -> DecideHandle<C> requires Relevant<SID> {
         LglsAssumeDev(not self.template IsEmpty<SID>(),
            "Container is assumed not emtpy");
         
         // Check type compatibility                                    
         [[maybe_unused]] RTTI::DefinitionData::FCompareEqual comparer = nullptr;
         if constexpr (CT::Handle<T>) {
            if constexpr (CT::TypeErased<C> or CT::TypeErased<T>) {
               const auto type = self.template GetType<SID>();
               LglsAssumeDev(type.IsSame(item.template GetType<SID>()),
                  "Type mismatch");
               comparer = type.GetComparerEqual();
               LglsAssumeDev(comparer, "Type-erased data not comparable");
            }
            else {
               static_assert(CT::Comparable<TypeOf<C, SID>, TypeOf<T, SID>>,
                  "Type not comparable");
            }
         }
         else {
            if constexpr (CT::TypeErased<C>) {
               const auto type = self.template GetType<SID>();
               LglsAssumeDev(type.IsSame(MetaDataOf<T>()),
                  "Type mismatch");
               comparer = type.GetComparerEqual();
               LglsAssumeDev(comparer, "Type-erased data not comparable");
            }
            else {
               static_assert(CT::Comparable<TypeOf<C, SID>, T>,
                  "Type not comparable");
            }
         }

         DecideHandle<C> result;
         self.template Apply<false>([&](auto&& test) -> bool {
            if constexpr (CT::Supported<decltype(test)>) {
               if constexpr (CT::Handle<T>) {
                  if constexpr (CT::TypeErased<C> or CT::TypeErased<T>) {
                     if (not comparer(test.template GetRaw<SID>(), item.template GetRaw<SID>()))
                        return true;   // Continue searching            
                  }
                  else {
                     if (*test.template GetRaw<SID>() != *item.template GetRaw<SID>())
                        return true;   // Continue searching            
                  }
               }
               else {
                  if constexpr (CT::TypeErased<C>) {
                     if (not comparer(test.template GetRaw<SID>(), &item))
                        return true;   // Continue searching            
                  }
                  else {
                     if (*test.template GetRaw<SID>() != item)
                        return true;   // Continue searching            
                  }
               }

               //                                                       
               // If reached, then match found                          
               new (&result) DecideHandle<C> {test};
               return false;
            }
            else return false;
            return true;
         }, cookie);

         //TODO allow hash table to warp back to the beginning
         return result;
      }

      /// Equality-compare with the first contained element                   
      ///   @attention compares one dimension at a time                       
      ///   @attention assumes container is if type 'RT' and not empty        
      ///   @param rhs the value to compare against                           
      ///   @return true if elements are the same                             
      template<Cid SID, class C, CT::NoIntent RT>
      constexpr bool CompareOneEqualInner(this C const& self, const RT& rhs) {
         LglsAssumeDev(not self.template IsEmpty<SID>(),
            "Container is assumed not empty");

         if constexpr (CT::Handle<RT>) {
            LglsAssumeDev(self.template IsSame<SID>(rhs), "Type mismatch");

            if constexpr (CT::TypeErased<C, RT>) {
               const auto comparer = self.template GetType<SID>().GetComparerEqual();
               LglsAssumeDev(comparer, "Type not comparable (type-erased): ", self.template GetType<SID>());
               return comparer(self.template GetRaw<SID>(), rhs.template GetRaw<SID>());
            }
            else {
               using T = Tif<CT::TypeErased<C>, TypeOf<RT, SID>, TypeOf<C, SID>>;
               return *self.template Get<T, SID>() == *rhs.template Get<T, SID>();
            }
         }
         else IF_NOT_LANGULUS_FORCE_TYPE_ERASURE(if constexpr (CT::TypeErased<C>)) {
            //                                                          
            // THIS is type-erased, do runtime type checks              
            LglsAssumeDev(self.template IsTyped<SID>(),
               "Container is assumed typed");

            if constexpr (CT::ComparableEqual<RT, RT>) {
               // Non-deep element compare                              
               if (self.template IsSame<RT, SID>()) {
                  if constexpr (CT::Contiguous<C>)
                     return *self.template Get<RT, SID>() == rhs;
                  else
                     return *self.template GetAt<RT, SID>(0) == rhs;
               }
            
               if constexpr (CT::Text<RT>) {
                  if (self.template IsSame<Text, SID>()) {
                     // Implicitly make a text container                
                     if constexpr (CT::Contiguous<C>)
                        return *self.template Get<Text, SID>() == Text {Disown(rhs)};
                     else
                        return *self.template GetAt<Text, SID>(0) == Text {Disown(rhs)};
                  }
               }
            }
         }
         #if not LANGULUS(FORCE_TYPE_ERASURE)
         else {
            //                                                          
            // Both sides are statically typed                          
            using T = TypeOf<C, SID>;

            if constexpr (CT::ComparableEqual<T, RT>) {
               if constexpr (CT::Contiguous<C>)
                  return *self.template GetRaw<SID>() == rhs;
               else
                  return *self.template GetAt<T, SID>(0) == rhs;
            }
         }
         #endif

         return false;
      }
      
      /// Equality-compare with the first contained element                   
      ///   @attention compares one dimension at a time                       
      ///   @attention assumes container is if type 'RT' and not empty        
      ///   @param rhs the value to compare against                           
      ///   @return true if elements are the same                             
      template<Cid SID, class C, CT::NoIntent RT>
      constexpr auto CompareOneInner(this C const& self, const RT& rhs) -> Ordering<C> {
         LglsAssumeDev(not self.template IsEmpty<SID>(),
            "Container is assumed not empty");

         if constexpr (CT::Handle<RT>) {
            LglsAssumeDev(self.template IsSame<SID>(rhs), "Type mismatch");

            if constexpr (CT::TypeErased<C, RT>) {
               const auto comparer = self.template GetType<SID>().GetComparer();
               LglsAssumeDev(comparer, "Type not comparable (type-erased): ", self.template GetType<SID>());
               return comparer(self.template GetRaw<SID>(), rhs.template GetRaw<SID>());
            }
            else {
               using T = Tif<CT::TypeErased<C>, TypeOf<RT, SID>, TypeOf<C, SID>>;
               return ToPartialOrdering(*self.template Get<T, SID>() <=> *rhs.template Get<T, SID>());
            }
         }
         else IF_NOT_LANGULUS_FORCE_TYPE_ERASURE(if constexpr (CT::TypeErased<C>)) {
            //                                                          
            // THIS is type-erased, do runtime type checks              
            LglsAssumeDev(self.template IsTyped<SID>(),
               "Container is assumed typed");
            
            if constexpr (CT::Comparable<RT, RT>) {
               // Non-deep element compare                              
               if (self.template IsSame<RT, SID>()) {
                  if constexpr (CT::Contiguous<C>)
                     return FromOrdering(*self.template Get<RT, SID>() <=> rhs);
                  else
                     return FromOrdering(*self.template GetAt<RT, SID>(0) <=> rhs);
               }

               if constexpr (CT::Text<RT>) {
                  // Text types can be more loosely compared            
                  if (self.template IsSame<Text, SID>()) {
                     // Implicitly make a text container                
                     if constexpr (CT::Contiguous<C>)
                        return FromOrdering(*self.template Get<Text, SID>() <=> Text{Disown(rhs)});
                     else
                        return FromOrdering(*self.template GetAt<Text, SID>(0) <=> Text{Disown(rhs)});
                  }
               }
            }
            
            return Compared::Unordered;
         }
         #if not LANGULUS(FORCE_TYPE_ERASURE)
         else {
            //                                                          
            // Both sides are statically typed                          
            using T = TypeOf<C, SID>;

            if constexpr (CT::Comparable<T, RT>) {
               if constexpr (CT::Contiguous<C>)
                  return ToPartialOrdering(*self.template GetRaw<SID>() <=> rhs);
               else
                  return ToPartialOrdering(*self.template GetAt<T, SID>(0) <=> rhs);
            }
            else return ::std::partial_ordering::unordered;
         }
         #endif
      }
   };
}

#include <Langulus/Logger/DisableVerbose.hpp>
