///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "../any/TestAnyCommon.hpp"


TEST_CASE_TEMPLATE("Test empty Handle/THandle", T
   , Text,   int,   Any,   RT
   , Text*,  int*,  Any*,  RT*
   , Text**, int**, Any**, RT**
   #if LANGULUS_FEATURE(MANAGED_MEMORY)
      , pptr16, pptr16*, pptr16**
   #endif
) {
   static MemoryState memoryState;

   static constexpr bool SPARSE = CT::Sparse<T>;
   static constexpr bool REFFED = CT::Referenced<Decay<T>>;

   using HUM  = HandleMut;   
   using HUC  = Handle;   
   using HUdM = HandleDisownedMut;   
   using HUdC = HandleDisowned;   
   using HEM  = THandle<T&>;
   using HEC  = THandle<T const&>;
   using HEmM = THandleEmergent<T&>;
   using HEmC = THandleEmergent<T const&>;
   using HEdM = THandleDisowned<T&>;
   using HEdC = THandleDisowned<T const&>;
   using HLM  = THandle<T>;
   using HLC  = THandle<T const>;
   
   static_assert(CT::Defaultable<
      HUM,  HUC,
      HUdM, HUdC,
      HEM,  HEC,
      HEmM, HEmC,
      HEdM, HEdC,
      HLM,  HLC
   >);

   static_assert(not requires { THandleEmergent<T> {}; });
   static_assert(not requires { THandleDisowned<T> {}; });


   GIVEN("Default-constructed handles") {
      HUM  default_constructed0;
      HUC  default_constructed1;
      HUdM default_constructed2;
      HUdC default_constructed3;
      HEM  default_constructed4;
      HEC  default_constructed5;
      HEmM default_constructed6;
      HEmC default_constructed7;
      HEdM default_constructed8;
      HEdC default_constructed9;
      HLM  default_constructed10;
      HLC  default_constructed11;

      Any_CheckState_Default<T>(default_constructed0);
      Any_CheckState_Default<T>(default_constructed1);
      Any_CheckState_Default<T>(default_constructed2);
      Any_CheckState_Default<T>(default_constructed3);
      Any_CheckState_Default<T>(default_constructed4);
      Any_CheckState_Default<T>(default_constructed5);
      Any_CheckState_Default<T>(default_constructed6);
      Any_CheckState_Default<T>(default_constructed7);
      Any_CheckState_Default<T>(default_constructed8);
      Any_CheckState_Default<T>(default_constructed9);
      Any_CheckState_Default<T>(default_constructed10);
      Any_CheckState_Default<T>(default_constructed11);
   }

   GIVEN("A sequential container (managed entries)") {
      const ScopedElementManaged<T> e1 {665};
      const ScopedElementManaged<T> e2 {666};
      const ScopedElementManaged<T> e3 {667};

      Any      data1 {*e1};
      TAny<T>  data2 {*e1};
      Many     data3 {*e1, *e2, *e3};
      TMany<T> data4 {*e1, *e2, *e3};
      /*Bytes    data5 {*e1};
      Text     data6 {*e1};
      TOwn<T>  data7 {*e1};
      TRef<T>  data8 {*e1};*/

      Any      const data1c {*e1};
      TAny<T>  const data2c {*e1};
      Many     const data3c {*e1, *e2, *e3};
      TMany<T> const data4c {*e1, *e2, *e3};
      /*Bytes    const data5c {*e1};
      Text     const data6c {*e1};
      TOwn<T>  const data7c {*e1};
      TRef<T>  const data8c {*e1};*/

      THandle<T&> h0 = data4.GetHandle();
      /*THandle<T&> h1 = data4.GetHandle() + 1;
      THandle<T&> h2 = data4.GetHandle() + 2;*/

      const T* h0p = h0.Get();
      REQUIRE(h0p == data4.GetRaw());

      if constexpr (CT::Sparse<T>) {
         AllocationPtr const* h0e = h0.GetEntries();
         REQUIRE(h0e == data4.GetEntries());
      }
      else static_assert(not requires { h0.GetEntries(); });
   }

   constexpr size_t refs1   = SPARSE ? 10 : 1;
   constexpr size_t refs1_1 = SPARSE ? 11 : 1;
   constexpr size_t refs2   = SPARSE ?  2 : 1;

   GIVEN("A stack-based swapper") {
      const ScopedElementManaged<T> e1 {1};
      const ScopedElementManaged<T> e2 {2};
      const ScopedElementManaged<T> e3 {3};
      const ScopedElementManaged<T> e4 {4};
      const ScopedElementManaged<T> e5 {5};
      const ScopedElementManaged<T> e6 {6};
      const ScopedElementManaged<T> e7 {7};
      const ScopedElementManaged<T> e8 {8};
      const ScopedElementManaged<T> e9 {9};
      const ScopedElementManaged<T> e10 {10};

      TMany<T> factory1 {*e1, *e2, *e3, *e4, *e5, *e6, *e7, *e8, *e9, *e10};
      REQUIRE(factory1.GetAllocation()->GetUses() == 1);

      WHEN("Swap through all elements and insert at the end") {
         const ScopedElementManaged<T> e100 {100};
         TMany<T> factory2 {*e100};

         REQUIRE(factory2.GetAllocation()->GetUses() == 1);

         // Create a handle to an element inside factory2               
         // The entry will be searched for in the memory manager        
         // Since we're using a local handle, the element will be reffed
         THandle<T> swapper {factory2[0]};
         REQUIRE(DenseCast(swapper.Get()) == *e100);

         if constexpr (SPARSE)
            REQUIRE(swapper.GetEntry()->GetUses() == refs2);
         if constexpr (REFFED)
            REQUIRE(DenseCast(swapper.Get()).GetReferences() == 2);

         {
            auto h = factory1.GetHandle();
            REQUIRE(DenseCast(h.Get()) == *e1);

            if constexpr (SPARSE)
               REQUIRE(h.GetEntries()[0]->GetUses() == refs1);
            if constexpr (REFFED)
               REQUIRE(DenseCast(h.Get()).GetReferences() == 1);

            // factory1[0] == 1                                         
            // swapped with swapper (referring to factory2[0] == 100)   
            h.SwapContents(swapper);

            // Swapper now only thing that refers to factory1[0]        
            REQUIRE(DenseCast(swapper.Get()) == *e1);
            REQUIRE(DenseCast(h.Get()) == *e100);

            if constexpr (SPARSE)
               REQUIRE(swapper.GetEntries()[0]->GetUses() == refs1);
            if constexpr (REFFED)
               REQUIRE(DenseCast(swapper.Get()).GetReferences() == 1);

            // Embedded handle is a second ref of factory2              
            if constexpr (SPARSE)
               REQUIRE(h.GetEntries()[0]->GetUses() == refs2);
            if constexpr (REFFED)
               REQUIRE(DenseCast(h.Get()).GetReferences() == 2);
         }

         {
            auto h = factory1.GetHandle() + 1;
            if constexpr (SPARSE)
               REQUIRE(h.GetEntries()[0]->GetUses() == refs1);
            h.SwapContents(swapper);
         }

         {
            auto h = factory1.GetHandle() + 2;
            if constexpr (SPARSE)
               REQUIRE(h.GetEntries[0]()->GetUses() == refs1);
            h.SwapContents(swapper);
         }

         {
            auto h = factory1.GetHandle() + 3;
            if constexpr (SPARSE)
               REQUIRE(h.GetEntries()[0]->GetUses() == refs1);
            h.SwapContents(swapper);
         }

         {
            auto h = factory1.GetHandle() + 4;
            if constexpr (SPARSE)
               REQUIRE(h.GetEntries()[0]->GetUses() == refs1);
            h.SwapContents(swapper);
         }

         {
            auto h = factory1.GetHandle() + 5;
            if constexpr (SPARSE)
               REQUIRE(h.GetEntries()[0]->GetUses() == refs1);
            h.SwapContents(swapper);
         }

         {
            auto h = factory1.GetHandle() + 6;
            if constexpr (SPARSE)
               REQUIRE(h.GetEntries()[0]->GetUses() == refs1);
            h.SwapContents(swapper);
         }

         {
            auto h = factory1.GetHandle() + 7;
            if constexpr (SPARSE)
               REQUIRE(h.GetEntries()[0]->GetUses() == refs1);
            h.SwapContents(swapper);
         }

         {
            auto h = factory1.GetHandle() + 8;
            if constexpr (SPARSE)
               REQUIRE(h.GetEntries()[0]->GetUses() == refs1);
            h.SwapContents(swapper);
         }

         {
            auto h = factory1.GetHandle() + 9;
            if constexpr (SPARSE)
               REQUIRE(h.GetEntries()[0]->GetUses() == refs1);
            h.SwapContents(swapper);
         }

         #if LANGULUS(SAFE)
            // Handles are designed for low level use and are unsafe    
            // but fast by design.                                      
            REQUIRE_THROWS(factory1.GetHandle() + 10);
         #endif

         // The swapper should contain the last element in factory1     
         REQUIRE(DenseCast(swapper.Get()) == *e10);

         if constexpr (SPARSE)
            REQUIRE(swapper.GetEntry()->GetUses() == refs1);
         if constexpr (REFFED)
            REQUIRE(DenseCast(swapper.Get()).GetReferences() == 1);

         // First element in factory1 should be the first from factory2 
         auto h0 = factory1.GetHandle();
         REQUIRE(DenseCast(h0.Get()) == *e100);

         if constexpr (SPARSE)
            REQUIRE(h0.GetEntry()->GetUses() == refs2);
         if constexpr (REFFED)
            REQUIRE(DenseCast(h0.Get()).GetReferences() == 2);

         THEN("Appending the leftover by Abandon") {
            factory1 << Abandon(swapper);

            REQUIRE(swapper.GetEntries() == nullptr);
            auto last = factory1.GetHandle() + (factory1.GetCount() - 1);
            REQUIRE(DenseCast(last.Get()) == *e10);
            
            if constexpr (SPARSE)
               REQUIRE(last.GetEntry()->GetUses() == refs1);
            if constexpr (REFFED)
               REQUIRE(DenseCast(last.Get()).GetReferences() == 1);

            for (int i = 1; i <= 10; ++i) {
               auto hi = factory1.GetHandle() + i;
               REQUIRE(DenseCast(hi.Get()) == i);

               if constexpr (SPARSE)
                  REQUIRE(hi.GetEntry()->GetUses() == refs1);
               if constexpr (REFFED)
                  REQUIRE(DenseCast(hi.Get()).GetReferences() == 1);
            }
         }

         THEN("Appending the leftover by Refer") {
            factory1 << Refer(swapper);

            if constexpr (SPARSE)
               REQUIRE(swapper.GetEntry());
            auto last = factory1.GetHandle() + (factory1.GetCount() - 1);
            REQUIRE(DenseCast(last.Get()) == *e10);

            if constexpr (SPARSE)
               REQUIRE(last.GetEntry()->GetUses() == refs1_1);
            if constexpr (REFFED)
               REQUIRE(DenseCast(last.Get()).GetReferences() == 2);

            for (int i = 1; i <= 9; ++i) {
               auto hi = factory1.GetHandle() + i;
               REQUIRE(DenseCast(hi.Get()) == i);

               if constexpr (SPARSE)
                  REQUIRE(hi.GetEntry()->GetUses() == refs1_1);
               if constexpr (REFFED)
                  REQUIRE(DenseCast(hi.Get()).GetReferences() == 1);
            }
         }

         THEN("Appending the leftover by Move") {
            factory1 << Move(swapper);

            REQUIRE(swapper.GetEntry() == nullptr);
            auto last = factory1.GetHandle() + (factory1.GetCount() - 1);
            REQUIRE(DenseCast(last.Get()) == *e10);

            if constexpr (SPARSE)
               REQUIRE(last.GetEntry()->GetUses() == refs1);
            if constexpr (REFFED)
               REQUIRE(DenseCast(last.Get()).GetReferences() == 1);

            for (int i = 1; i <= 10; ++i) {
               auto hi = factory1.GetHandle() + i;
               REQUIRE(DenseCast(hi.Get()) == i);

               if constexpr (SPARSE)
                  REQUIRE(hi.GetEntry()->GetUses() == refs1);
               if constexpr (REFFED)
                  REQUIRE(DenseCast(hi.Get()).GetReferences() == 1);
            }
         }
      }

      REQUIRE(factory1.GetAllocation()->GetUses() == 1);

      auto start = factory1.GetHandle(0);
      REQUIRE(DenseCast(start.Get()) == 100);

      if constexpr (SPARSE)
         REQUIRE(start.GetEntry()->GetUses() == 1);
      if constexpr (REFFED)
         REQUIRE(DenseCast(start.Get()).GetReferences() == 1);

      for (size_t i = 1; i < factory1.GetCount(); ++i) {
         auto h = factory1.GetHandle() + i;
         REQUIRE(DenseCast(h.Get()) == static_cast<int>(i));

         if constexpr (SPARSE)
            REQUIRE(h.GetEntry()->GetUses() == refs1);
         if constexpr (REFFED)
            REQUIRE(DenseCast(h.Get()).GetReferences() == 1);
      }
   }

   REQUIRE(memoryState.Assert());
   REQUIRE_FALSE(Allocator::CollectGarbage());
}