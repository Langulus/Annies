///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../set/TestSetCommon.hpp"
#include "../handle/TestHandlePairCommon.hpp"
#include <Langulus/Pair.hpp>
#include <Langulus/TPair.hpp>


#if LANGULUS(BENCHMARK)
   /// MARK: Benchmarking                                                     
   /// Perform a persistent benchmark across build and verify performance     
   #define BenchmarkPair(func, tolerance, my_init, my) { \
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
   #define BenchmarkPairStd(func, tolerance_highscore, tolerance, my_init, my, theirs_init, theirs) { \
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
      const auto token_std = ::std::string("Test/") + func + "(" + NameOf<E1>() + "," + NameOf<E2>() + ") |std::pair|"; \
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
   #define BenchmarkPair(func, tolerance, my_init, my)
   #define BenchmarkPairStd(func, tolerance_highscore, tolerance, my_init, my, theirs_init, theirs)
#endif


/// MARK: TestType                                                            
template<class K, class V, CT::Container C> requires CT::NoIntent<C>
void Pair_Helper_TestType(C const& pack) {
   if constexpr (CT::Void<K>) {
      REQUIRE(    pack.template IsKeySame<int>());
      REQUIRE(    pack.template IsKeyExact<int>());
      REQUIRE(    pack.template IsKey<int>());
      REQUIRE(not pack.IsKeySparse());
      REQUIRE(not pack.IsKeyDeep());
      REQUIRE(    pack.GetKeyType() == MetaDataOf<int>());
   }
   else {
      REQUIRE(    pack.template IsKeySame<K>());
      REQUIRE(    pack.template IsKeyExact<K>());
      REQUIRE(    pack.template IsKey<K>());
      REQUIRE(    pack.IsKeySparse() == CT::Sparse<K>);
      REQUIRE(    pack.IsKeyDeep() == CT::Deep<K>);
      REQUIRE(    pack.GetKeyType() == MetaDataOf<K>());      
   }
   
   if constexpr (CT::Void<V>) {
      REQUIRE(    pack.template IsValSame<int>());
      REQUIRE(    pack.template IsValExact<int>());
      REQUIRE(    pack.template IsVal<int>());
      REQUIRE(not pack.IsValSparse());
      REQUIRE(not pack.IsValDeep());
      REQUIRE(    pack.GetValType() == MetaDataOf<int>());
   }
   else {
      REQUIRE(    pack.template IsValSame<V>());
      REQUIRE(    pack.template IsValExact<V>());
      REQUIRE(    pack.template IsVal<V>());
      REQUIRE(    pack.IsValSparse() == CT::Sparse<V>);
      REQUIRE(    pack.IsValDeep() == CT::Deep<V>);
      REQUIRE(    pack.GetValType() == MetaDataOf<V>());      
   }
   
   REQUIRE(pack.IsKeyTyped());
   REQUIRE(pack.IsValTyped());
}

/// MARK: TestSame                                                            
template<class LHS, class RHS> requires (CT::Container<LHS, RHS> and CT::NoIntent<LHS, RHS>)
void Pair_Helper_TestSame(const LHS& lhs, const RHS& rhs, bool match_constness = true) {
   REQUIRE(lhs.GetCount() == rhs.GetCount());

   if (not lhs.IsEmpty())
      REQUIRE(lhs.GetRaw() == rhs.GetRaw());

   REQUIRE(lhs.IsKeyExact(rhs.GetKeyType()));
   REQUIRE(lhs.IsValExact(rhs.GetValType()));
   REQUIRE(lhs == rhs);
   REQUIRE(lhs.IsKeyDeep() == rhs.IsKeyDeep());
   REQUIRE(lhs.IsValDeep() == rhs.IsValDeep());

   if (match_constness)
      REQUIRE(lhs.IsConstant() == rhs.IsConstant());

   if constexpr (requires { lhs.GetUnconstrainedState(); rhs.GetUnconstrainedState(); })
      REQUIRE(lhs.GetUnconstrainedState() == rhs.GetUnconstrainedState());
   else if constexpr (requires { lhs.GetUnconstrainedState(); })
      REQUIRE(lhs.IsDefaultState());
   else if constexpr (requires { rhs.GetUnconstrainedState(); })
      REQUIRE(rhs.IsDefaultState());
}

/// MARK: Default                                                             
template<class K, class V, CT::Container C> requires CT::NoIntent<C>
void Pair_CheckState_Default(C const& pack, bool ismap = false, bool typed = false) {
   if constexpr (CT::Typed<C>) {
      static_assert(Exact<TypeOf<C, 0>, K>);
      static_assert(Exact<TypeOf<C, 1>, V>);
      static_assert(Exact<typename C::Key, K>);
      static_assert(Exact<typename C::Val, V>);
      Pair_Helper_TestType<K, V>(pack);
   }
   else if (not typed) {
      REQUIRE_FALSE(pack.IsKeyTyped());
      REQUIRE      (pack.GetKeyType() == nullptr);
      REQUIRE_FALSE(pack.IsKeySparse());
      REQUIRE_FALSE(pack.IsKeyDeep());
      
      REQUIRE_FALSE(pack.IsValTyped());
      REQUIRE      (pack.GetValType() == nullptr);
      REQUIRE_FALSE(pack.IsValSparse());
      REQUIRE_FALSE(pack.IsValDeep());
   }
   else Pair_Helper_TestType<K, V>(pack);

   REQUIRE      (pack.IsDefaultState());
   REQUIRE      (pack.IsKeyTypeConstrained() == CT::Typed<C>);
   REQUIRE      (pack.IsValTypeConstrained() == CT::Typed<C>);
   REQUIRE      (pack.IsKeyConstant() == ismap);
   REQUIRE_FALSE(pack.IsValConstant());
   REQUIRE_FALSE(pack.IsValid());
   REQUIRE_FALSE(pack.GetAllocation());
   REQUIRE      (pack.IsEmpty());
   REQUIRE      (pack.GetCount() == 0);
   REQUIRE      (pack.GetReserved() == 0);
   REQUIRE      (pack.GetUses() == 0);
   REQUIRE_FALSE(pack);
   REQUIRE      (not pack);
   REQUIRE      (pack == C{});

   //TODO Many_CheckState_Default<K>(map.GetKeys());
   //TODO Many_CheckState_Default<V>(map.GetVals());

   REQUIRE_FALSE(pack.IsEncrypted());
}

/// MARK: OwnedEmpty                                                          
template<class K, class V, CT::Container C> requires CT::NoIntent<C>
void Pair_CheckState_OwnedEmpty(C const& pack, bool ismap = false) {
   Pair_Helper_TestType<K, V>(pack);

   REQUIRE      (pack.IsKeyTypeConstrained() == CT::Typed<C>);
   REQUIRE      (pack.IsValTypeConstrained() == CT::Typed<C>);
   REQUIRE      (pack.IsKeyConstant() == (CT::Constant<K> or ismap));
   REQUIRE      (pack.IsValConstant() == CT::Constant<V>);
   REQUIRE_FALSE(pack.IsValid());
   REQUIRE      (pack.GetAllocation());
   REQUIRE      (pack.IsEmpty());
   REQUIRE      (pack.GetCount() == 0);
   REQUIRE      (pack.GetReserved() > 0);
   REQUIRE      (pack.GetUses() == 1);
   REQUIRE_FALSE(pack);
   REQUIRE      (not pack);
   REQUIRE      (pack == C{});

   //TODO Many_CheckState_OwnedEmpty<K>(map.GetKeys());
   //TODO Many_CheckState_OwnedEmpty<V>(map.GetVals());
}

/// MARK: OwnedFull                                                           
template<class K, class V, CT::Container C> requires CT::NoIntent<C>
void Pair_CheckState_OwnedFull(C const& pack, bool ismap = false) {
   Pair_Helper_TestType<K, V>(pack);

   REQUIRE      (pack.IsKeyTypeConstrained() == CT::Typed<C>);
   REQUIRE      (pack.IsValTypeConstrained() == CT::Typed<C>);
   REQUIRE      (pack.IsKeyConstant() == (CT::Constant<K> or ismap));
   REQUIRE      (pack.IsValConstant() == CT::Constant<V>);
   REQUIRE      (pack.IsValid());
   REQUIRE      (pack.GetAllocation());
   REQUIRE_FALSE(pack.IsEmpty());
   REQUIRE      (pack.GetCount() > 0);
   REQUIRE      (pack.GetReserved() > 0);
   REQUIRE      (pack.GetUses() > 0);
   REQUIRE      (pack.GetRaw());
   REQUIRE      (pack);
   REQUIRE_FALSE(not pack);
   REQUIRE      (pack != C{});

   //TODO Many_CheckState_OwnedFull<K>(map.GetKeys());
   //TODO Many_CheckState_OwnedFull<V>(map.GetVals());
}

/// MARK: DisownedFull                                                        
template<class K, class V, CT::Container C> requires CT::NoIntent<C>
void Pair_CheckState_DisownedFull(C const& pack) {
   Pair_Helper_TestType<K, V>(pack);

   REQUIRE      (pack.IsKeyTypeConstrained() == CT::Typed<C>);
   REQUIRE      (pack.IsValTypeConstrained() == CT::Typed<C>);
   REQUIRE      (pack.IsKeyConstant());
   REQUIRE      (pack.IsValConstant());
   REQUIRE      (pack.IsValid());
   REQUIRE      (pack.GetAllocation());
   REQUIRE_FALSE(pack.IsEmpty());
   REQUIRE      (pack.GetCount() > 0);
   REQUIRE      (pack.GetReserved() > 0);
   REQUIRE      (pack.GetUses() > 0);
   REQUIRE      (pack.GetRaw());
   REQUIRE      (pack);
   REQUIRE_FALSE(not pack);
   REQUIRE      (pack != C{});

   //TODO Many_CheckState_DisownedFull<K>(map.GetKeys());
   //TODO Many_CheckState_DisownedFull<V>(map.GetVals());
}

/// MARK: Abandoned                                                           
template<class K, class V, CT::Container C> requires CT::NoIntent<C>
void Pair_CheckState_Abandoned(C const& pack) {
   REQUIRE(pack.IsDisowned());

   //TODO Many_CheckState_Abandoned<K>(map.GetKeys());
   //TODO Many_CheckState_Abandoned<V>(map.GetVals());
}

/// MARK: Accessors                                                           
template<CT::Container T, CT::Intent I1, CT::Intent I2> requires CT::NoIntent<T>
void Pair_VerifyAccessorInterface(T const& pack, I1&&, I2&&) {
   using E1 = typename Decay<Deint<I1>>::CTTI_Typed;
   using E2 = typename Decay<Deint<I2>>::CTTI_Typed;

   // The Get method always adds a pointer, because it interfaces the   
   // heap directly                                                     
   static_assert(requires {
      {pack.template Get<Decay<E1>      , 0>()} -> ::std::same_as<const Decay<E1>*>;
      {pack.template Get<Decay<E1> const, 0>()} -> ::std::same_as<const Decay<E1>*>;
      {pack.template Get<Decay<E2>      , 1>()} -> ::std::same_as<const Decay<E2>*>;
      {pack.template Get<Decay<E2> const, 1>()} -> ::std::same_as<const Decay<E2>*>;
   });
   static_assert(requires {
      {pack.template Get<E1      , 0>()} -> ::std::same_as<ConstAll<E1> const*>;
      {pack.template Get<E1 const, 0>()} -> ::std::same_as<ConstAll<E1> const*>;
      {pack.template Get<E2      , 1>()} -> ::std::same_as<ConstAll<E2> const*>;
      {pack.template Get<E2 const, 1>()} -> ::std::same_as<ConstAll<E2> const*>;
   });

   // AsAt dereferences that pointer and/or wraps inside handles or     
   // containers.                                                       
   using innerT1 = Tif<(CT::Sparse<E1> and not CT::CustomPointer<E1>), ConstAll<E1>, ConstAll<E1> const&>;
   static_assert(requires {
      {pack.template As<E1, 0>()} -> ::std::same_as<innerT1>;
   });

   using innerT2 = Tif<(CT::Sparse<E2> and not CT::CustomPointer<E2>), ConstAll<E2>, ConstAll<E2> const&>;
   static_assert(requires {
      {pack.template As<E2, 1>()} -> ::std::same_as<innerT2>;
   });

   if constexpr (CT::Dense<E1> and CT::Typed<T>) {
      // One additional indirection is always acceptable                
      // A static container will static_assert if too many indirects    
      static_assert(requires {
         {pack.template Get<Decay<E1>      *, 0>()} -> ::std::same_as<Decay<E1> const*>;
         {pack.template Get<Decay<E1> const*, 0>()} -> ::std::same_as<Decay<E1> const*>;
      });
      static_assert(requires {
         {pack.template As<Decay<E1>*, 0>()} -> ::std::same_as<Decay<E1> const*>;
      });
   }
   else if constexpr (not CT::CustomPointer<E1>) {
      // One additional indirection is always acceptible                
      // Type-erased containers will throw an exception at runtime, if  
      // too many indirects were requested                              
      static_assert(requires {
         {pack.template Get<Decay<E1>      *, 0>()} -> ::std::same_as<Decay<E1> const* const*>;
         {pack.template Get<Decay<E1> const*, 0>()} -> ::std::same_as<Decay<E1> const* const*>;
      });
      static_assert(requires {
         {pack.template As<Decay<E1>*, 0>()} -> ::std::same_as<Decay<E1> const*>;
      });

      if constexpr (IndirectsOf<E1> >= 2 or CT::TypeErased<T>) {
         static_assert(requires {
            {pack.template Get<Decay<E1>      **, 0>()} -> ::std::same_as<Decay<E1> const* const* const*>;
            {pack.template Get<Decay<E1> const**, 0>()} -> ::std::same_as<Decay<E1> const* const* const*>;
         });

         static_assert(requires {
            {pack.template As<Decay<E1>**, 0>()} -> ::std::same_as<Decay<E1> const* const*>;
         });
      }
      else {
         static_assert(requires {
            {pack.template Get<Decay<E1>      **, 0>()} -> ::std::same_as<Decay<E1> const* const*>;
            {pack.template Get<Decay<E1> const**, 0>()} -> ::std::same_as<Decay<E1> const* const*>;
         });

         static_assert(requires {
            {pack.template As<Decay<E1>**, 0>()} -> ::std::same_as<Decay<E1> const* const*>;
         });
      }
   }

   if constexpr (CT::Dense<E2> and CT::Typed<T>) {
      // One additional indirection is always acceptable                
      // A static container will static_assert if too many indirects    
      static_assert(requires {
         {pack.template Get<Decay<E2>      *, 1>()} -> ::std::same_as<Decay<E2> const*>;
         {pack.template Get<Decay<E2> const*, 1>()} -> ::std::same_as<Decay<E2> const*>;
      });
      static_assert(requires {
         {pack.template As<Decay<E2>*, 1>()} -> ::std::same_as<Decay<E2> const*>;
      });
   }
   else if constexpr (not CT::CustomPointer<E2>) {
      // One additional indirection is always acceptible                
      // Type-erased containers will throw an exception at runtime, if  
      // too many indirects were requested                              
      static_assert(requires {
         {pack.template Get<Decay<E2>      *, 1>()} -> ::std::same_as<Decay<E2> const* const*>;
         {pack.template Get<Decay<E2> const*, 1>()} -> ::std::same_as<Decay<E2> const* const*>;
      });
      static_assert(requires {
         {pack.template As<Decay<E2>*, 1>()} -> ::std::same_as<Decay<E2> const*>;
      });

      if constexpr (IndirectsOf<E2> >= 2 or CT::TypeErased<T>) {
         static_assert(requires {
            {pack.template Get<Decay<E2>      **, 1>()} -> ::std::same_as<Decay<E2> const* const* const*>;
            {pack.template Get<Decay<E2> const**, 1>()} -> ::std::same_as<Decay<E2> const* const* const*>;
         });

         static_assert(requires {
            {pack.template As<Decay<E2>**, 1>()} -> ::std::same_as<Decay<E2> const* const*>;
         });
      }
      else {
         static_assert(requires {
            {pack.template Get<Decay<E2>      **, 1>()} -> ::std::same_as<Decay<E2> const* const*>;
            {pack.template Get<Decay<E2> const**, 1>()} -> ::std::same_as<Decay<E2> const* const*>;
         });

         static_assert(requires {
            {pack.template As<Decay<E2>**, 1>()} -> ::std::same_as<Decay<E2> const* const*>;
         });
      }
   }
}

/// MARK: ContainsOne                                                         
template<CT::Container T, CT::Intent IK, CT::Intent IV> requires CT::NoIntent<T>
void Pair_CheckState_ContainsOne(T const& pack, IK&& key_with_intent, IV&& val_with_intent, int uses = 1) {
   Pair_VerifyAccessorInterface(pack, LglsFwd(key_with_intent), LglsFwd(val_with_intent));

   auto& e1 = key_with_intent.what;
   auto& e2 = val_with_intent.what;
   using E1 = typename Decay<Deint<IK>>::CTTI_Typed;
   using E2 = typename Decay<Deint<IV>>::CTTI_Typed;

   if constexpr (CT::DeepDense<E1>) {
      REQUIRE(pack.template KeyAs<E1 >().template IsSame<int>());
      REQUIRE(pack.template KeyAs<E1*>()->template IsSame<int>());
   }

   if constexpr (CT::DeepDense<E2>) {
      REQUIRE(pack.template ValAs<E2 >().template IsSame<int>());
      REQUIRE(pack.template ValAs<E2*>()->template IsSame<int>());
   }

   REQUIRE(pack.GetCount() == 1);
   REQUIRE(pack.GetUses() == uses);
   REQUIRE(pack.GetReserved() >= (uses ? 1 : 0));

   if constexpr (not CT::CustomPointer<E1>)
      REQUIRE(pack.template KeyAs<Decay<E1>>() == DenseCast(*e1));
   if constexpr (not CT::CustomPointer<E2>)
      REQUIRE(pack.template ValAs<Decay<E2>>() == DenseCast(*e2));

   if constexpr (CT::Cloned<IK> and CT::Sparse<E1>) {
      REQUIRE(pack.template KeyAs<E1>() != *e1);
      REQUIRE((*pack.template KeyAs<E1*>()) != *e1);
   }
   else {
      REQUIRE(pack.template KeyAs<E1>() == *e1);
      REQUIRE((*pack.template KeyAs<E1*>()) == *e1);
   }

   if constexpr (CT::Cloned<IV> and CT::Sparse<E2>) {
      REQUIRE(pack.template ValAs<E2>() != *e2);
      REQUIRE((*pack.template ValAs<E2*>()) != *e2);
   }
   else {
      REQUIRE(pack.template ValAs<E2>() == *e2);
      REQUIRE((*pack.template ValAs<E2*>()) == *e2);
   }

   if constexpr (CT::OwnedDeep<T>) {
      if constexpr (CT::Dense<E1>)
         REQUIRE(pack.GetKeyEntries() == nullptr);
      else if (uses) {
         REQUIRE(pack.GetKeyEntries() != nullptr);

         if constexpr (not CT::Disowned<IK>) {
            for (size_t i = 0; i < IndirectsOf<E1>; ++i) {
               if constexpr (CT::Cloned<IK>)
                  REQUIRE(pack.GetKeyEntries()[i] != e1.entries[i + 1]);
               else
                  REQUIRE(pack.GetKeyEntries()[i] == e1.entries[i + 1]);
            }
         }
         else {
            for (size_t i = 0; i < IndirectsOf<E1>; ++i)
               REQUIRE(pack.GetKeyEntries()[i] == nullptr);
         }
      }

      if constexpr (CT::Dense<E2>)
         REQUIRE(pack.GetValEntries() == nullptr);
      else if (uses) {
         REQUIRE(pack.GetValEntries() != nullptr);

         if constexpr (not CT::Disowned<IV>) {
            for (size_t i = 0; i < IndirectsOf<E2>; ++i) {
               if constexpr (CT::Cloned<IV>)
                  REQUIRE(pack.GetValEntries()[i] != e2.entries[i + 1]);
               else
                  REQUIRE(pack.GetValEntries()[i] == e2.entries[i + 1]);
            }
         }
         else {
            for (size_t i = 0; i < IndirectsOf<E2>; ++i)
               REQUIRE(pack.GetValEntries()[i] == nullptr);
         }
      }
   }
}

/// MARK: CompareOne                                                          
template<CT::Container T, class E1, class E2> requires CT::NoIntent<T>
void Pair_Helper_CompareOne(const T&, const E1&, const E2&) {
   //TODO Many_Helper_CompareOne(map, e);
}
