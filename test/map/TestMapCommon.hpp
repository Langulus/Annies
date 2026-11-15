///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../pair/TestPairCommon.hpp"
#include "../handle/TestHandlePairCommon.hpp"
#include <Langulus/Map.hpp>
#include <Langulus/TMap.hpp>
#include <unordered_map>


#if LANGULUS(BENCHMARK)
   /// Perform a persistent benchmark across build and verify performance     
   #define BenchmarkMap(func, tolerance, my_init, my) { \
      const auto token = ::std::string("Test/") + func + "(" + NameOf<E1>() + "," + NameOf<E2>() + ") |" + NameOf<T>() + "|"; \
      volatile int i = 0; \
      for (; i < BenchmarkWarmupCycles; i += 1) { \
         my_init; \
         my; \
      } \
      for (; i < BenchmarkWarmupCycles + BenchmarkMeasureCycles; i += 1) { \
         my_init; \
         { \
            CTRACK_NAME_PERSIST(token.c_str()); \
            my; \
         } \
      } \
      auto results = ctrack::result_get_detail_table(); \
      results.check_highscore(tolerance); \
   }

   /// Perform two persistent benchmarks across builds - one for Map and      
   /// one for std::unordered_map. Make sure they don't deviate in a bad way. 
   #define BenchmarkMapStd(func, tolerance_highscore, tolerance, my_init, my, theirs_init, theirs) { \
      const auto token = ::std::string("Test/") + func + "(" + NameOf<E1>() + "," + NameOf<E2>() + ") |" + NameOf<T>() + "|"; \
      volatile int i = 0; \
      for (; i < BenchmarkWarmupCycles; i += 1) { \
         my_init; \
         my; \
      } \
      for (; i < BenchmarkWarmupCycles + BenchmarkMeasureCycles; i += 1) { \
         my_init; \
         { \
            CTRACK_NAME_PERSIST(token.c_str()); \
            my; \
         } \
      } \
      i = 0; \
      const auto token_std = ::std::string("Test/") + func + "(" + NameOf<E1>() + "," + NameOf<E2>() + ") |std::unordered_map|"; \
      for (; i < BenchmarkWarmupCycles; i += 1) { \
         theirs_init; \
         theirs; \
      } \
      for (; i < BenchmarkWarmupCycles + BenchmarkMeasureCycles; i += 1) { \
         theirs_init; \
         { \
            CTRACK_NAME(token_std.c_str()); \
            theirs; \
         } \
      } \
      auto results = ctrack::result_get_detail_table(); \
      results.check_highscore(tolerance_highscore); \
      REQUIRE(results.check_same(token.c_str(), token_std.c_str(), tolerance)); \
   }
#else
   #define BenchmarkMap(func, tolerance, my_init, my)
   #define BenchmarkMapStd(func, tolerance_highscore, tolerance, my_init, my, theirs_init, theirs)
#endif


template<class K, class V, CT::Container C> requires CT::NoIntent<C>
void Map_Helper_TestType(C const& pack) {
   Pair_Helper_TestType<K, V>(pack);
}

template<class LHS, class RHS> requires (CT::Container<LHS, RHS> and CT::NoIntent<LHS, RHS>)
void Map_Helper_TestSame(const LHS& lhs, const RHS& rhs, bool match_constness = true) {
   Pair_Helper_TestSame(lhs, rhs, match_constness);
}

///                                                                           
/// Possible state test implementations                                       
template<class K, class V, CT::Container C> requires CT::NoIntent<C>
void Map_CheckState_Default(C const& pack, bool typed = false) {
   Pair_CheckState_Default<K, V>(pack, true, typed);

   REQUIRE_FALSE(pack.IsCompressed());
   REQUIRE_FALSE(pack.IsSorted());
}

template<class K, class V, CT::Container C> requires CT::NoIntent<C>
void Map_CheckState_OwnedEmpty(C const& pack) {
   Pair_CheckState_OwnedEmpty<K, V>(pack, true);
}

template<class K, class V, CT::Container C> requires CT::NoIntent<C>
void Map_CheckState_OwnedFull(C const& pack) {
   Pair_CheckState_OwnedFull<K, V>(pack, true);
}

template<class K, class V, CT::Container C> requires CT::NoIntent<C>
void Map_CheckState_DisownedFull(C const& pack) {
   Pair_CheckState_DisownedFull<K, V>(pack);
}

template<class K, class V, CT::Container C> requires CT::NoIntent<C>
void Map_CheckState_Abandoned(C const& pack) {
   Pair_CheckState_Abandoned<K, V>(pack);

   //TODO Many_CheckState_Abandoned<K>(map.GetKeys());
   //TODO Many_CheckState_Abandoned<V>(map.GetVals());
}

template<CT::Container T, CT::Intent I1, CT::Intent I2> requires CT::NoIntent<T>
void Map_VerifyAccessorInterface(T const& map, I1&&, I2&&) {
   using E1 = typename Decay<Deint<I1>>::CTTI_Typed;
   using E2 = typename Decay<Deint<I2>>::CTTI_Typed;

   // The Get method always adds a pointer, because it interfaces the   
   // heap directly                                                     
   static_assert(requires {
      {map.template GetAt<Decay<E1>      , 0>(0)} -> ::std::same_as<const Decay<E1>*>;
      {map.template GetAt<Decay<E1> const, 0>(0)} -> ::std::same_as<const Decay<E1>*>;
      {map.template GetAt<Decay<E2>      , 1>(0)} -> ::std::same_as<const Decay<E2>*>;
      {map.template GetAt<Decay<E2> const, 1>(0)} -> ::std::same_as<const Decay<E2>*>;
   });
   static_assert(requires {
      {map.template GetAt<E1      , 0>(0)} -> ::std::same_as<ConstAll<E1> const*>;
      {map.template GetAt<E1 const, 0>(0)} -> ::std::same_as<ConstAll<E1> const*>;
      {map.template GetAt<E2      , 1>(0)} -> ::std::same_as<ConstAll<E2> const*>;
      {map.template GetAt<E2 const, 1>(0)} -> ::std::same_as<ConstAll<E2> const*>;
   });

   // AsAt dereferences that pointer and/or wraps inside handles or     
   // containers.                                                       
   using innerT1 = Tif<(CT::Sparse<E1> and not CT::CustomPointer<E1>), ConstAll<E1>, ConstAll<E1> const&>;
   static_assert(requires {
      {map.template AsAt<E1, 0>(0)} -> ::std::same_as<innerT1>;
   });

   using innerT2 = Tif<(CT::Sparse<E2> and not CT::CustomPointer<E2>), ConstAll<E2>, ConstAll<E2> const&>;
   static_assert(requires {
      {map.template AsAt<E2, 1>(0)} -> ::std::same_as<innerT2>;
   });

   if constexpr (CT::Dense<E1> and CT::Typed<T>) {
      // One additional indirection is always acceptable                
      // A static container will static_assert if too many indirects    
      static_assert(requires {
         {map.template GetAt<Decay<E1>      *, 0>(0)} -> ::std::same_as<Decay<E1> const*>;
         {map.template GetAt<Decay<E1> const*, 0>(0)} -> ::std::same_as<Decay<E1> const*>;
      });
      static_assert(requires {
         {map.template AsAt<Decay<E1>*, 0>(0)} -> ::std::same_as<Decay<E1> const*>;
      });
   }
   else if constexpr (not CT::CustomPointer<E1>) {
      // One additional indirection is always acceptible                
      // Type-erased containers will throw an exception at runtime, if  
      // too many indirects were requested                              
      static_assert(requires {
         {map.template GetAt<Decay<E1>      *, 0>(0)} -> ::std::same_as<Decay<E1> const* const*>;
         {map.template GetAt<Decay<E1> const*, 0>(0)} -> ::std::same_as<Decay<E1> const* const*>;
      });
      static_assert(requires {
         {map.template AsAt<Decay<E1>*, 0>(0)} -> ::std::same_as<Decay<E1> const*>;
      });

      if constexpr (IndirectsOf<E1> >= 2 or CT::TypeErased<T>) {
         static_assert(requires {
            {map.template GetAt<Decay<E1>      **, 0>(0)} -> ::std::same_as<Decay<E1> const* const* const*>;
            {map.template GetAt<Decay<E1> const**, 0>(0)} -> ::std::same_as<Decay<E1> const* const* const*>;
         });

         static_assert(requires {
            {map.template AsAt<Decay<E1>**, 0>(0)} -> ::std::same_as<Decay<E1> const* const*>;
         });
      }
      else {
         static_assert(requires {
            {map.template GetAt<Decay<E1>      **, 0>(0)} -> ::std::same_as<Decay<E1> const* const*>;
            {map.template GetAt<Decay<E1> const**, 0>(0)} -> ::std::same_as<Decay<E1> const* const*>;
         });

         static_assert(requires {
            {map.template AsAt<Decay<E1>**, 0>(0)} -> ::std::same_as<Decay<E1> const* const*>;
         });
      }
   }

   if constexpr (CT::Dense<E2> and CT::Typed<T>) {
      // One additional indirection is always acceptable                
      // A static container will static_assert if too many indirects    
      static_assert(requires {
         {map.template GetAt<Decay<E2>      *, 1>(0)} -> ::std::same_as<Decay<E2> const*>;
         {map.template GetAt<Decay<E2> const*, 1>(0)} -> ::std::same_as<Decay<E2> const*>;
      });
      static_assert(requires {
         {map.template AsAt<Decay<E2>*, 1>(0)} -> ::std::same_as<Decay<E2> const*>;
      });
   }
   else if constexpr (not CT::CustomPointer<E2>) {
      // One additional indirection is always acceptible                
      // Type-erased containers will throw an exception at runtime, if  
      // too many indirects were requested                              
      static_assert(requires {
         {map.template GetAt<Decay<E2>      *, 1>(0)} -> ::std::same_as<Decay<E2> const* const*>;
         {map.template GetAt<Decay<E2> const*, 1>(0)} -> ::std::same_as<Decay<E2> const* const*>;
      });
      static_assert(requires {
         {map.template AsAt<Decay<E2>*, 1>(0)} -> ::std::same_as<Decay<E2> const*>;
      });

      if constexpr (IndirectsOf<E2> >= 2 or CT::TypeErased<T>) {
         static_assert(requires {
            {map.template GetAt<Decay<E2>      **, 1>(0)} -> ::std::same_as<Decay<E2> const* const* const*>;
            {map.template GetAt<Decay<E2> const**, 1>(0)} -> ::std::same_as<Decay<E2> const* const* const*>;
         });

         static_assert(requires {
            {map.template AsAt<Decay<E2>**, 1>(0)} -> ::std::same_as<Decay<E2> const* const*>;
         });
      }
      else {
         static_assert(requires {
            {map.template GetAt<Decay<E2>      **, 1>(0)} -> ::std::same_as<Decay<E2> const* const*>;
            {map.template GetAt<Decay<E2> const**, 1>(0)} -> ::std::same_as<Decay<E2> const* const*>;
         });

         static_assert(requires {
            {map.template AsAt<Decay<E2>**, 1>(0)} -> ::std::same_as<Decay<E2> const* const*>;
         });
      }
   }
}

template<CT::Container T, CT::Intent IK, CT::Intent IV> requires CT::NoIntent<T>
void Map_CheckState_ContainsOne(T const& map, IK&& key_with_intent, IV&& val_with_intent, int uses = 1) {
   Map_VerifyAccessorInterface(map, LglsFwd(key_with_intent), LglsFwd(val_with_intent));

   auto& e1 = key_with_intent.what;
   auto& e2 = val_with_intent.what;
   using E1 = typename Decay<Deint<IK>>::CTTI_Typed;
   using E2 = typename Decay<Deint<IV>>::CTTI_Typed;
   /*using P1 = TPair<ConstAll<E1>, E2>; //TODO test pair containers separately
   using P2 = TPair<ConstAll<E1&>, E2&>;
   using P3 = Anyness::Pair;
   using P4 = TPair<ConstAll<E1*>, E2*>;*/

   if constexpr (CT::DeepDense<E1>) {
      REQUIRE(map.template KeyAsAt<E1 >(0).template IsSame<int>());
      REQUIRE(map.template KeyAsAt<E1*>(0)->template IsSame<int>());
      /*REQUIRE(map.template AsAt<P1>(0).key.template IsSame<int>());
      REQUIRE(map.template AsAt<P2>(0).key.template IsSame<int>());
      REQUIRE(map.template AsAt<P3>(0).key.template IsSame<int>());
      REQUIRE(map.template AsAt<P4>(0).key->template IsSame<int>());*/
   }

   if constexpr (CT::DeepDense<E2>) {
      REQUIRE(map.template ValAsAt<E2 >(0).template IsSame<int>());
      REQUIRE(map.template ValAsAt<E2*>(0)->template IsSame<int>());
      /*REQUIRE(map.template AsAt<P1>(0).val.template IsSame<int>());
      REQUIRE(map.template AsAt<P2>(0).val.template IsSame<int>());
      REQUIRE(map.template AsAt<P3>(0).val.template IsSame<int>());
      REQUIRE(map.template AsAt<P4>(0).val->template IsSame<int>());*/
   }

   REQUIRE(map.GetCount() == 1);
   REQUIRE(map.GetUses() == uses);
   REQUIRE(map.GetReserved() >= (uses ? 1 : 0));

   if constexpr (not CT::CustomPointer<E1>)
      REQUIRE(map.template KeyAsAt<Decay<E1>>(0) == DenseCast(*e1));
   if constexpr (not CT::CustomPointer<E2>)
      REQUIRE(map.template ValAsAt<Decay<E2>>(0) == DenseCast(*e2));

   if constexpr (CT::Cloned<IK> and CT::Sparse<E1>) {
      REQUIRE(map.template KeyAsAt<E1>(0) != *e1);
      REQUIRE((*map.template KeyAsAt<E1*>(0)) != *e1);
   }
   else {
      REQUIRE(map.template KeyAsAt<E1>(0) == *e1);
      REQUIRE((*map.template KeyAsAt<E1*>(0)) == *e1);
   }

   if constexpr (CT::Cloned<IV> and CT::Sparse<E2>) {
      REQUIRE(map.template ValAsAt<E2>(0) != *e2);
      REQUIRE((*map.template ValAsAt<E2*>(0)) != *e2);
   }
   else {
      REQUIRE(map.template ValAsAt<E2>(0) == *e2);
      REQUIRE((*map.template ValAsAt<E2*>(0)) == *e2);
   }

   if constexpr (CT::OwnedDeep<T>) {
      if constexpr (CT::Dense<E1>)
         REQUIRE(map.GetKeyEntries() == nullptr);
      else if (uses) {
         REQUIRE(map.GetKeyEntries() != nullptr);

         if constexpr (not CT::Disowned<IK>) {
            for (size_t i = 0; i < IndirectsOf<E1>; ++i) {
               if constexpr (CT::Cloned<IK>) {
                  REQUIRE(map.GetKeyEntriesAt(0)[i] != nullptr);
                  REQUIRE(map.GetKeyEntriesAt(0)[i] != e1.entries[i + 1]);
               }
               else {
                  #if LANGULUS_FEATURE(MANAGED_MEMORY)
                     REQUIRE(map.GetKeyEntriesAt(0)[i] == e1.entries[i + 1]);
                  #else
                     REQUIRE(map.GetKeyEntriesAt(0)[i] == nullptr);
                  #endif
               }
            }
         }
         else {
            for (size_t i = 0; i < IndirectsOf<E1>; ++i)
               REQUIRE(map.GetKeyEntriesAt(0)[i] == nullptr);
         }
      }

      if constexpr (CT::Dense<E2>)
         REQUIRE(map.GetValEntries() == nullptr);
      else if (uses) {
         REQUIRE(map.GetValEntries() != nullptr);

         if constexpr (not CT::Disowned<IV>) {
            for (size_t i = 0; i < IndirectsOf<E2>; ++i) {
               if constexpr (CT::Cloned<IV>) {
                  REQUIRE(map.GetValEntriesAt(0)[i] != nullptr);
                  REQUIRE(map.GetValEntriesAt(0)[i] != e2.entries[i + 1]);
               }
               else {
                  #if LANGULUS_FEATURE(MANAGED_MEMORY)
                     REQUIRE(map.GetValEntriesAt(0)[i] == e2.entries[i + 1]);
                  #else
                     REQUIRE(map.GetValEntriesAt(0)[i] == nullptr);
                  #endif
               }
            }
         }
         else {
            for (size_t i = 0; i < IndirectsOf<E2>; ++i)
               REQUIRE(map.GetValEntriesAt(0)[i] == nullptr);
         }
      }
   }

   if constexpr (CT::TypeErased<T>) {
      REQUIRE_THROWS(map.template AsAt<float>(0));
      REQUIRE_THROWS(map.template AsAt<float*>(0));
      REQUIRE_THROWS(map.template AsAt<TPair<float, float>>(0));
      REQUIRE_THROWS(map.template AsAt<TPair<float const*, float const*>>(0));
      //REQUIRE_THROWS(map.template AsAt<TPair<float*, float*>>(0)); //TODO
      //REQUIRE_THROWS(map.template AsAt<TPair<E1, float>>(0)); //TODO
      //REQUIRE_THROWS(map.template AsAt<TPair<E1 const*, float const*>>(0)); //TODO
      //REQUIRE_THROWS(map.template AsAt<TPair<E1*, float*>>(0)); //TODO
      //REQUIRE_THROWS(map.template AsAt<TPair<float, E2>>(0)); //TODO
      //REQUIRE_THROWS(map.template AsAt<TPair<float const*, E2 const*>>(0)); //TODO
      //REQUIRE_THROWS(map.template AsAt<TPair<float*, E2*>>(0)); //TODO
   }

   //TODO test all kinds of ranged modifiers??
   for (auto& it : map) {
      if constexpr (CT::TypeErased<T>) {
         static_assert(not requires { it.GetKey(); });
         static_assert(not requires { it.GetVal(); });
         REQUIRE(it.GetKeyHandle().CompareOneEqual(*e1) != (CT::Cloned<IK> and CT::Sparse<E1>));
         REQUIRE(it.GetValHandle().CompareOneEqual(*e2) != (CT::Cloned<IV> and CT::Sparse<E2>));
      }
      else {
         static_assert(requires { it.GetKeyHandle(); });
         static_assert(requires { it.GetValHandle(); });
         REQUIRE((it.GetKey() != *e1) == (CT::Cloned<IK> and CT::Sparse<E1>));
         REQUIRE((it.GetVal() != *e2) == (CT::Cloned<IV> and CT::Sparse<E2>));
      }
   }
}

template<CT::Container T, CT::Intent I> requires CT::NoIntent<T>
void Map_CheckState_ContainsN(size_t n, const T& map, I&& e_scoped_with_intent, int uses = 1) {
   auto& e = e_scoped_with_intent.what;
   //using E = typename Decay<Deint<I>>::CTTI_Typed;

   REQUIRE(map.GetCount() == n);
   REQUIRE(map.GetUses() == uses);
   REQUIRE(map.GetReserved() >= n);

   for (auto& it : map)
      REQUIRE(it == e);

   //TODO other kinds of iterations
}

template<CT::Container T, CT::Intent I> requires (CT::NoIntent<T> and CT::Array<I>)
void Map_CheckState_ContainsArray(const T& map, I&& e_scoped_array_with_intent) {
   auto  e = e_scoped_array_with_intent.what;
   //using E = typename Decay<Deint<I>>::CTTI_Typed;
   constexpr size_t n = ExtentOf<decltype(e_scoped_array_with_intent.what)>;

   REQUIRE(map.GetCount() == n);
   REQUIRE(map.GetUses() == 1);
   REQUIRE(map.GetReserved() >= n);

   //TODO
}

template<CT::Container T, class E> requires CT::NoIntent<T>
void Map_Helper_CompareOne(const T&, const E&) {
   //TODO Many_Helper_CompareOne(map, e);
}
