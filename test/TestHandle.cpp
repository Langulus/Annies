///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include <Langulus/Annies/Many.hpp>
#include "Common.hpp"


/// Create a dense or sparse container by providing simple arguments          
template<class T, class...FROM>
TMany<T> CreateManagedElements(FROM&&...from) {
   static_assert(CT::MakableFrom<Decay<T>, Decay<FROM>...>);
#if LANGULUS_FEATURE(MANAGED_MEMORY)
   TMany<Decay<T>> base {TypedCast(from)...};
   if constexpr (CT::Similar<T, Decay<T>>)
      return base;
   else {
      if constexpr (CT::Sparse<T> and CT::Dense<Deptr<T>>) {
         TMany<T> sparse;
         for (auto& item : base)
            sparse << &item;
         return sparse;
      }
      else static_assert(false, "TODO sparser T");
   }
#else
   if constexpr (CT::Dense<T>)
      return {Forward<FROM>(from)...};
   else if constexpr (CT::Sparse<T> and CT::Dense<Deptr<T>>)
      return {new Decay<T> {Forward<FROM>(from)}...};
   else
      static_assert(false, "TODO sparser T");
#endif
}

/// Create a dense or sparse local handle by providing simple arguments       
template<class T, class FROM>
HandleLocal<T> CreateHandle(FROM&& from) {
   static_assert(CT::MakableFrom<Decay<T>, Decay<FROM>>);

   if constexpr (CT::Similar<T, Decay<T>>)
      return {Forward<FROM>(from)};
   else {
      if constexpr (CT::Sparse<T> and CT::Dense<Deptr<T>>)
         return {new Decay<T> {Forward<FROM>(from)}};
      else
         static_assert(false, "TODO sparser T");
   }
}

///                                                                           
TEMPLATE_TEST_CASE("Handles from sequential containers", "[handle]",
   RT*, RT, int, int*
) {
   static Allocator::State memoryState;

   using T = TestType;
   using HE = Handle<T>;
   using HL = HandleLocal<T>;

   static_assert(not CT::Defaultable<HE>);
   static_assert(not CT::Defaultable<HL>);

   static constexpr bool SPARSE = CT::Sparse<T>;
   static constexpr bool REFERENCABLE = CT::Referencable<Deptr<T>>;


   GIVEN("A statically typed sequential container") {
      TMany<T> data = CreateManagedElements<T>(665, 666, 667);

      REQUIRE(data.GetCount() == 3);
      REQUIRE(DenseCast(data[0]) == 665);
      REQUIRE(DenseCast(data[1]) == 666);
      REQUIRE(DenseCast(data[2]) == 667);

      Handle<T> h0 = data.GetHandle(0);
      Handle<T> h1 = data.GetHandle(1);
      Handle<T> h2 = data.GetHandle(2);

   #if LANGULUS_FEATURE(MANAGED_MEMORY)
      REQUIRE(h0.GetEntry());
      REQUIRE(h0.GetEntry() == h1.GetEntry());
      REQUIRE(h0.GetEntry() == h2.GetEntry());
      REQUIRE(h0.GetEntry() == h2.GetEntry());

      if constexpr (SPARSE)
         REQUIRE(h0.GetEntry()->GetUses() == 3);
      else
         REQUIRE(h0.GetEntry()->GetUses() == 1);
   #endif

      if constexpr (REFERENCABLE) {
         REQUIRE(DenseCast(h0.Get()).GetReferences() == 1);
         REQUIRE(DenseCast(h1.Get()).GetReferences() == 1);
         REQUIRE(DenseCast(h2.Get()).GetReferences() == 1);

         REQUIRE(DenseCast(h0.Get()).destroyed == false);
         REQUIRE(DenseCast(h1.Get()).destroyed == false);
         REQUIRE(DenseCast(h2.Get()).destroyed == false);
      }

      const T h0p = h0.Get();
      const Allocation* const h0e = h0.GetEntry();

      WHEN("An element is taken out of the container and assigned into another") {
         TMany<T> next = CreateManagedElements<T>(0);
         Handle<T> n = next.GetHandle(0);
         const Allocation* const n0e = n.GetEntry();
         IF_LANGULUS_MANAGED_MEMORY(REQUIRE(n0e->GetUses() == 1));

         n.AssignWithIntent(Move(h0));

         if constexpr (REFERENCABLE) {
            if constexpr (SPARSE) {
               REQUIRE(h0.Get() == nullptr);
               REQUIRE(h0.GetEntry() == nullptr);

               REQUIRE(n.Get() == h0p);
               REQUIRE(n.GetEntry() == h0e);
               REQUIRE(n.Get()->GetReferences() == 1);
               IF_LANGULUS_MANAGED_MEMORY(REQUIRE(n.GetEntry()->GetUses() == 3));
               REQUIRE(n.Get()->data == h0p->data);
               REQUIRE(n.Get()->destroyed == false);
               REQUIRE(n.Get()->moved_in == false);
               REQUIRE(n.Get()->moved_out == false);
            }
            else {
               REQUIRE(h0.Get().GetReferences() == 1);
               REQUIRE(h0.Get().destroyed == false);
               REQUIRE(h0.Get().moved_in == false);
               REQUIRE(h0.Get().moved_out == true);

               REQUIRE(n.Get().GetReferences() == 1);
               REQUIRE(n.Get().data == h0p.data);
               REQUIRE(n.GetEntry() == n0e);
               REQUIRE(n.Get().destroyed == false);
               REQUIRE(n.Get().moved_in == true);
               REQUIRE(n.Get().moved_out == false);
            }
         }
         else {
            if constexpr (SPARSE) {
               REQUIRE(h0.Get() == nullptr);
               REQUIRE(h0.GetEntry() == nullptr);

               REQUIRE(n.Get() == h0p);
               REQUIRE(n.GetEntry() == h0e);
               IF_LANGULUS_MANAGED_MEMORY(REQUIRE(n.GetEntry()->GetUses() == 3));
            }
            else {
               REQUIRE(n.Get() == h0p);
               REQUIRE(n.GetEntry() == n0e);
               REQUIRE(h0.GetEntry() == h0e);
            }
         }
      }
      
      WHEN("An element is taken out of the container and swapped with another") {
         TMany<T> next = CreateManagedElements<T>(0);
         Handle<T> n = next.GetHandle(0);
         T const n0p = n.Get();
         const Allocation* const n0e = n.GetEntry();
         IF_LANGULUS_MANAGED_MEMORY(REQUIRE(n0e->GetUses() == 1));

         n.Swap(h0);

         if constexpr (REFERENCABLE) {
            if constexpr (SPARSE) {
               REQUIRE(h0.Get() == n0p);
               REQUIRE(h0.GetEntry() == n0e);
               REQUIRE(h0.Get()->GetReferences() == 1);
               IF_LANGULUS_MANAGED_MEMORY(REQUIRE(h0.GetEntry()->GetUses() == 1));
               REQUIRE(h0.Get()->data == n0p->data);
               REQUIRE(h0.Get()->destroyed == false);
               REQUIRE(h0.Get()->moved_in == false);
               REQUIRE(h0.Get()->moved_out == false);

               REQUIRE(n.Get() == h0p);
               REQUIRE(n.GetEntry() == h0e);
               REQUIRE(n.Get()->GetReferences() == 1);
               IF_LANGULUS_MANAGED_MEMORY(REQUIRE(n.GetEntry()->GetUses() == 3));
               REQUIRE(n.Get()->data == h0p->data);
               REQUIRE(n.Get()->destroyed == false);
               REQUIRE(n.Get()->moved_in == false);
               REQUIRE(n.Get()->moved_out == false);
            }
            else {
               REQUIRE(h0.Get().data == n0p.data);
               REQUIRE(h0.GetEntry() == h0e);
               REQUIRE(h0.Get().GetReferences() == 1);
               REQUIRE(h0.Get().destroyed == false);
               REQUIRE(h0.Get().moved_in == true);
               REQUIRE(h0.Get().moved_out == false);

               REQUIRE(n.Get().data == h0p.data);
               REQUIRE(n.GetEntry() == n0e);
               REQUIRE(n.Get().GetReferences() == 1);
               REQUIRE(n.Get().destroyed == false);
               REQUIRE(n.Get().moved_in == true);
               REQUIRE(n.Get().moved_out == false);
            }
         }
         else {
            if constexpr (SPARSE) {
               REQUIRE(h0.Get() == n0p);
               REQUIRE(h0.GetEntry() == n0e);
               IF_LANGULUS_MANAGED_MEMORY(REQUIRE(h0.GetEntry()->GetUses() == 1));

               REQUIRE(n.Get() == h0p);
               REQUIRE(n.GetEntry() == h0e);
               IF_LANGULUS_MANAGED_MEMORY(REQUIRE(n.GetEntry()->GetUses() == 3));
            }
            else {
               REQUIRE(h0.Get() == n0p);
               REQUIRE(h0.GetEntry() == h0e);

               REQUIRE(n.Get() == h0p);
               REQUIRE(n.GetEntry() == n0e);
            }
         }

         if constexpr (CT::Referencable<T>)
            REQUIRE(const_cast<T&>(n0p).Reference(-1) == 0);
      }

      WHEN("An element is taken out of the container and swapped with managed local") {
         TMany<T> next = CreateManagedElements<T>(0);
         HandleLocal<T> n = next[0];
         T const n0p = n.Get();
         const Allocation* const n0e = n.GetEntry();

         if constexpr (SPARSE)
            IF_LANGULUS_MANAGED_MEMORY(REQUIRE(n0e->GetUses() == 2));
         else
            REQUIRE(n0e == nullptr);

         n.Swap(h0);

         if constexpr (REFERENCABLE) {
            if constexpr (SPARSE) {
               REQUIRE(h0.Get() == n0p);
               REQUIRE(h0.GetEntry() == n0e);
               #if LANGULUS_FEATURE(MANAGED_MEMORY)
                  REQUIRE(h0.Get()->GetReferences() == 2);
               #else
                  REQUIRE(h0.Get()->GetReferences() == 1);
               #endif
               IF_LANGULUS_MANAGED_MEMORY(REQUIRE(h0.GetEntry()->GetUses() == 2));
               REQUIRE(h0.Get()->data == n0p->data);
               REQUIRE(h0.Get()->destroyed == false);
               REQUIRE(h0.Get()->moved_in == false);
               REQUIRE(h0.Get()->moved_out == false);

               REQUIRE(n.Get() == h0p);
               REQUIRE(n.GetEntry() == h0e);
               REQUIRE(n.Get()->GetReferences() == 1);
               IF_LANGULUS_MANAGED_MEMORY(REQUIRE(n.GetEntry()->GetUses() == 3));
               REQUIRE(n.Get()->data == h0p->data);
               REQUIRE(n.Get()->destroyed == false);
               REQUIRE(n.Get()->moved_in == false);
               REQUIRE(n.Get()->moved_out == false);
            }
            else {
               REQUIRE(h0.Get().data == n0p.data);
               REQUIRE(h0.GetEntry() == h0e);
               REQUIRE(h0.Get().GetReferences() == 1);
               REQUIRE(h0.Get().destroyed == false);
               REQUIRE(h0.Get().moved_in == true);
               REQUIRE(h0.Get().moved_out == false);

               REQUIRE(n.Get().data == h0p.data);
               REQUIRE(n.GetEntry() == n0e);
               REQUIRE(n.Get().GetReferences() == 1);
               REQUIRE(n.Get().destroyed == false);
               REQUIRE(n.Get().moved_in == true);
               REQUIRE(n.Get().moved_out == false);
            }
         }
         else {
            if constexpr (SPARSE) {
               REQUIRE(h0.Get() == n0p);
               REQUIRE(h0.GetEntry() == n0e);
               IF_LANGULUS_MANAGED_MEMORY(REQUIRE(h0.GetEntry()->GetUses() == 2));

               REQUIRE(n.Get() == h0p);
               REQUIRE(n.GetEntry() == h0e);
               IF_LANGULUS_MANAGED_MEMORY(REQUIRE(n.GetEntry()->GetUses() == 3));
            }
            else {
               REQUIRE(h0.Get() == n0p);
               REQUIRE(h0.GetEntry() == h0e);

               REQUIRE(n.Get() == h0p);
               REQUIRE(n.GetEntry() == n0e);
            }
         }

         if constexpr (CT::Referencable<T>)
            REQUIRE(const_cast<T&>(n0p).Reference(-1) == 0);
      }

      WHEN("An element is taken out of the container and swapped with a unmanaged local") {
         HandleLocal<T> n = CreateHandle<T>(42);
         T const n0p = n.Get();
         const Allocation* const n0e = n.GetEntry();
         REQUIRE(n0e == nullptr);

         n.Swap(h0);

         if constexpr (REFERENCABLE) {
            if constexpr (SPARSE) {
               REQUIRE(h0.Get() == n0p);
               REQUIRE(h0.GetEntry() == n0e);
               REQUIRE(h0.Get()->GetReferences() == 1);
               REQUIRE(h0.Get()->data == n0p->data);
               REQUIRE(h0.Get()->destroyed == false);
               REQUIRE(h0.Get()->moved_in == false);
               REQUIRE(h0.Get()->moved_out == false);

               REQUIRE(n.Get() == h0p);
               REQUIRE(n.GetEntry() == h0e);
               REQUIRE(n.Get()->GetReferences() == 1);
               IF_LANGULUS_MANAGED_MEMORY(REQUIRE(n.GetEntry()->GetUses() == 3));
               REQUIRE(n.Get()->data == h0p->data);
               REQUIRE(n.Get()->destroyed == false);
               REQUIRE(n.Get()->moved_in == false);
               REQUIRE(n.Get()->moved_out == false);
            }
            else {
               REQUIRE(h0.Get().data == n0p.data);
               REQUIRE(h0.GetEntry() == h0e);
               REQUIRE(h0.Get().GetReferences() == 1);
               REQUIRE(h0.Get().destroyed == false);
               REQUIRE(h0.Get().moved_in == true);
               REQUIRE(h0.Get().moved_out == false);

               REQUIRE(n.Get().data == h0p.data);
               REQUIRE(n.GetEntry() == n0e);
               REQUIRE(n.Get().GetReferences() == 1);
               REQUIRE(n.Get().destroyed == false);
               REQUIRE(n.Get().moved_in == true);
               REQUIRE(n.Get().moved_out == false);
            }
         }
         else {
            if constexpr (SPARSE) {
               REQUIRE(h0.Get() == n0p);
               REQUIRE(h0.GetEntry() == n0e);

               REQUIRE(n.Get() == h0p);
               REQUIRE(n.GetEntry() == h0e);
               IF_LANGULUS_MANAGED_MEMORY(REQUIRE(n.GetEntry()->GetUses() == 3));
            }
            else {
               REQUIRE(h0.Get() == n0p);
               REQUIRE(h0.GetEntry() == h0e);

               REQUIRE(n.Get() == h0p);
               REQUIRE(n.GetEntry() == n0e);
            }
         }

         if constexpr (SPARSE) {
            if constexpr (REFERENCABLE)
               REQUIRE(h0.Get()->Reference(-1) == 0);
            delete h0.Get();
         }

         if constexpr (CT::Referencable<T>)
            REQUIRE(const_cast<T&>(n0p).Reference(-1) == 0);
      }

      WHEN("An element is taken out of the container and moved into a local handle") {
         HandleLocal<T> local = Move(h0);

         if constexpr (REFERENCABLE) {
            if constexpr (SPARSE) {
               REQUIRE(h0.Get() == nullptr);
               REQUIRE(h0.GetEntry() == nullptr);

               REQUIRE(local.Get() == h0p);
               REQUIRE(local.GetEntry() == h0e);
               REQUIRE(local.Get()->GetReferences() == 1);
               IF_LANGULUS_MANAGED_MEMORY(REQUIRE(local.GetEntry()->GetUses() == 3));
               REQUIRE(local.Get()->data == h0p->data);
               REQUIRE(local.Get()->destroyed == false);
               REQUIRE(local.Get()->moved_in == false);
               REQUIRE(local.Get()->moved_out == false);
            }
            else {
               REQUIRE(h0.Get().GetReferences() == 1);
               REQUIRE(h0.Get().destroyed == false);
               REQUIRE(h0.Get().moved_in == false);
               REQUIRE(h0.Get().moved_out == true);

               REQUIRE(local.Get().GetReferences() == 1);
               REQUIRE(local.Get().data == h0p.data);
               REQUIRE(local.GetEntry() == nullptr);
               REQUIRE(local.Get().destroyed == false);
               REQUIRE(local.Get().moved_in == true);
               REQUIRE(local.Get().moved_out == false);
            }
         }
         else {
            if constexpr (SPARSE) {
               REQUIRE(h0.Get() == nullptr);
               REQUIRE(h0.GetEntry() == nullptr);

               REQUIRE(local.Get() == h0p);
               REQUIRE(local.GetEntry() == h0e);
               IF_LANGULUS_MANAGED_MEMORY(REQUIRE(local.GetEntry()->GetUses() == 3));
            }
            else {
               REQUIRE(local.Get() == h0p);
               REQUIRE(local.GetEntry() == nullptr);
            }
         }
      }

      if constexpr (CT::Referencable<T>)
         REQUIRE(const_cast<T&>(h0p).Reference(-1) == 0);
   }

   REQUIRE(memoryState.Assert());

   // Destroy BANK before static data - otherwise problems happen if    
   // not using managed reflection                                      
   BANK.Reset();

   REQUIRE_FALSE(Allocator::CollectGarbage());
}


///                                                                           
TEMPLATE_TEST_CASE("Managed handle swapping", "[handle]", RT*, RT, int, int*) {
   using T = TestType;
   static Allocator::State memoryState;

   constexpr bool sparse = CT::Sparse<T> and LANGULUS_FEATURE(MANAGED_MEMORY);
   constexpr bool referenced = sparse and CT::Referencable<Deptr<T>>;
   constexpr Count refs1 = CT::Sparse<T> and LANGULUS_FEATURE(MANAGED_MEMORY) ? 10 : 1;
   constexpr Count refs1_1 = CT::Sparse<T> and LANGULUS_FEATURE(MANAGED_MEMORY) ? 11 : 1;
   constexpr Count refs2 = CT::Sparse<T> and LANGULUS_FEATURE(MANAGED_MEMORY) ? 2 : 1;

   GIVEN("A stack-based swapper") {
      TMany<T> factory1 = CreateManagedElements<T>(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
      REQUIRE(factory1.GetAllocation()->GetUses() == 1);

      WHEN("Swap through all elements and insert at the end") {
         TMany<T> factory2 = CreateManagedElements<T>(100);
         REQUIRE(factory2.GetAllocation()->GetUses() == 1);

         // Create a handle to an element inside factory2               
         // The entry will be searched for in the memory manager        
         // Since we're using a local handle, the element will be reffed
         HandleLocal<T> swapper {factory2[0]};

         if constexpr (sparse)
            REQUIRE(swapper.GetEntry()->GetUses() == refs2);
         if constexpr (referenced)
            REQUIRE(DenseCast(swapper.Get()).GetReferences() == 2);

         {
            auto h = factory1.GetHandle(0);
            if constexpr (LANGULUS_FEATURE(MANAGED_MEMORY))
               REQUIRE(h.GetEntry()->GetUses() == refs1);
            REQUIRE(DenseCast(h.Get()) == 1);
            if constexpr (referenced)
               REQUIRE(DenseCast(h.Get()).GetReferences() == 1);

            // factory1[0] == 1                                         
            // swapped with swapper (referring to factory2[0] == 100)   
            h.Swap(swapper);

            // Swapper now only thing that refers to factory1[0]        
            REQUIRE(DenseCast(swapper.Get()) == 1);
            if constexpr (sparse)
               REQUIRE(swapper.GetEntry()->GetUses() == refs1);
            if constexpr (referenced)
               REQUIRE(DenseCast(swapper.Get()).GetReferences() == 1);

            // Embedded handle is a second ref of factory2              
            if constexpr (LANGULUS_FEATURE(MANAGED_MEMORY))
               REQUIRE(h.GetEntry()->GetUses() == refs2);
            REQUIRE(DenseCast(h.Get()) == 100);
            if constexpr (referenced)
               REQUIRE(DenseCast(h.Get()).GetReferences() == 2);
         }

         {
            auto h = factory1.GetHandle(1);
            if constexpr (LANGULUS_FEATURE(MANAGED_MEMORY))
               REQUIRE(h.GetEntry()->GetUses() == refs1);
            h.Swap(swapper);
         }

         {
            auto h = factory1.GetHandle(2);
            if constexpr (LANGULUS_FEATURE(MANAGED_MEMORY))
               REQUIRE(h.GetEntry()->GetUses() == refs1);
            h.Swap(swapper);
         }

         {
            auto h = factory1.GetHandle(3);
            if constexpr (LANGULUS_FEATURE(MANAGED_MEMORY))
               REQUIRE(h.GetEntry()->GetUses() == refs1);
            h.Swap(swapper);
         }

         {
            auto h = factory1.GetHandle(4);
            if constexpr (LANGULUS_FEATURE(MANAGED_MEMORY))
               REQUIRE(h.GetEntry()->GetUses() == refs1);
            h.Swap(swapper);
         }

         {
            auto h = factory1.GetHandle(5);
            if constexpr (LANGULUS_FEATURE(MANAGED_MEMORY))
               REQUIRE(h.GetEntry()->GetUses() == refs1);
            h.Swap(swapper);
         }

         {
            auto h = factory1.GetHandle(6);
            if constexpr (LANGULUS_FEATURE(MANAGED_MEMORY))
               REQUIRE(h.GetEntry()->GetUses() == refs1);
            h.Swap(swapper);
         }

         {
            auto h = factory1.GetHandle(7);
            if constexpr (LANGULUS_FEATURE(MANAGED_MEMORY))
               REQUIRE(h.GetEntry()->GetUses() == refs1);
            h.Swap(swapper);
         }

         {
            auto h = factory1.GetHandle(8);
            if constexpr (LANGULUS_FEATURE(MANAGED_MEMORY))
               REQUIRE(h.GetEntry()->GetUses() == refs1);
            h.Swap(swapper);
         }

         {
            auto h = factory1.GetHandle(9);
            if constexpr (LANGULUS_FEATURE(MANAGED_MEMORY))
               REQUIRE(h.GetEntry()->GetUses() == refs1);
            h.Swap(swapper);
         }

         // The swapper should contain the last element in factory1     
         REQUIRE(DenseCast(swapper.Get()) == 10);
         if constexpr (sparse)
            REQUIRE(swapper.GetEntry()->GetUses() == refs1);
         if constexpr (referenced)
            REQUIRE(DenseCast(swapper.Get()).GetReferences() == 1);

         // First element in factory1 should be the first from factory2 
         auto h0 = factory1.GetHandle(0);
         REQUIRE(DenseCast(h0.Get()) == 100);
         if constexpr (LANGULUS_FEATURE(MANAGED_MEMORY))
            REQUIRE(h0.GetEntry()->GetUses() == refs2);
         if constexpr (referenced)
            REQUIRE(DenseCast(h0.Get()).GetReferences() == 2);

         THEN("Appending the leftover by Abandon") {
            factory1 << Abandon(swapper);

            REQUIRE(swapper.GetEntry() == nullptr);
            auto last = factory1.GetHandle(factory1.GetCount() - 1);
            REQUIRE(DenseCast(last.Get()) == 10);
            if constexpr (LANGULUS_FEATURE(MANAGED_MEMORY))
               REQUIRE(last.GetEntry()->GetUses() == refs1);
            if constexpr (referenced)
               REQUIRE(DenseCast(last.Get()).GetReferences() == 1);

            for (int i = 1; i <= 10; ++i) {
               auto hi = factory1.GetHandle(i);
               REQUIRE(DenseCast(hi.Get()) == i);
               if constexpr (LANGULUS_FEATURE(MANAGED_MEMORY))
                  REQUIRE(hi.GetEntry()->GetUses() == refs1);
               if constexpr (referenced)
                  REQUIRE(DenseCast(hi.Get()).GetReferences() == 1);
            }
         }

         THEN("Appending the leftover by Refer") {
            factory1 << Refer(swapper);

            if constexpr (sparse)
               REQUIRE(swapper.GetEntry());
            auto last = factory1.GetHandle(factory1.GetCount() - 1);
            REQUIRE(DenseCast(last.Get()) == 10);
            if constexpr (LANGULUS_FEATURE(MANAGED_MEMORY))
               REQUIRE(last.GetEntry()->GetUses() == refs1_1);
            if constexpr (referenced)
               REQUIRE(DenseCast(last.Get()).GetReferences() == 2);

            for (int i = 1; i <= 9; ++i) {
               auto hi = factory1.GetHandle(i);
               REQUIRE(DenseCast(hi.Get()) == i);
               if constexpr (LANGULUS_FEATURE(MANAGED_MEMORY))
                  REQUIRE(hi.GetEntry()->GetUses() == refs1_1);
               if constexpr (referenced)
                  REQUIRE(DenseCast(hi.Get()).GetReferences() == 1);
            }
         }

         THEN("Appending the leftover by Move") {
            factory1 << Move(swapper);

            REQUIRE(swapper.GetEntry() == nullptr);
            auto last = factory1.GetHandle(factory1.GetCount() - 1);
            REQUIRE(DenseCast(last.Get()) == 10);
            if constexpr (LANGULUS_FEATURE(MANAGED_MEMORY))
               REQUIRE(last.GetEntry()->GetUses() == refs1);
            if constexpr (referenced)
               REQUIRE(DenseCast(last.Get()).GetReferences() == 1);

            for (int i = 1; i <= 10; ++i) {
               auto hi = factory1.GetHandle(i);
               REQUIRE(DenseCast(hi.Get()) == i);
               if constexpr (LANGULUS_FEATURE(MANAGED_MEMORY))
                  REQUIRE(hi.GetEntry()->GetUses() == refs1);
               if constexpr (referenced)
                  REQUIRE(DenseCast(hi.Get()).GetReferences() == 1);
            }
         }
      }

      REQUIRE(factory1.GetAllocation()->GetUses() == 1);

      auto start = factory1.GetHandle(0);
      if constexpr (LANGULUS_FEATURE(MANAGED_MEMORY))
         REQUIRE(start.GetEntry()->GetUses() == 1);
      REQUIRE(DenseCast(start.Get()) == 100);
      if constexpr (referenced)
         REQUIRE(DenseCast(start.Get()).GetReferences() == 1);

      for (Count i = 1; i < factory1.GetCount(); ++i) {
         auto h = factory1.GetHandle(i);
         if constexpr (LANGULUS_FEATURE(MANAGED_MEMORY))
            REQUIRE(h.GetEntry()->GetUses() == refs1);
         REQUIRE(DenseCast(h.Get()) == static_cast<int>(i));
         if constexpr (referenced)
            REQUIRE(DenseCast(h.Get()).GetReferences() == 1);
      }

      REQUIRE(memoryState.Assert());
   }

   // Destroy BANK before static data - otherwise problems happen if    
   // not using managed reflection                                      
   BANK.Reset();

   REQUIRE_FALSE(Allocator::CollectGarbage());
}