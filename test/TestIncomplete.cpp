///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include <Langulus/Annies/Text.hpp>
#include <Langulus/Annies/Trait.hpp>
#include <Langulus/Annies/Own.hpp>
#include <Langulus/Annies/Ref.hpp>
#include <Langulus/Annies/TMap.hpp>
#include "Common.hpp"


struct Resolvable {
   Resolvable(DMeta d) : mMeta {d} {}
   DMeta mMeta {};
};

class Unit;

using UnitMap = TUnorderedMap<DMeta, TMany<Unit*>>;
using TraitMap = TUnorderedMap<TMeta, TMany<Trait>>;

struct Thing final : Resolvable {
   LANGULUS(ABSTRACT) false;
   LANGULUS(PRODUCER) Thing;
   LANGULUS_BASES(Resolvable);

   Thing();

   Own<Thing*> mOwned;
   Ref<Thing> mOwner;
   TMany<Thing*> mChildren;
   UnitMap mUnits;
   TraitMap mTraits;
};

Thing::Thing() : Resolvable {MetaOf<Thing>()} {}

SCENARIO("Testing incomplete type hierarchy", "[incomplete]") {
   static_assert(CT::Complete<Resolvable>);
   static_assert(CT::Complete<Own<Thing*>>);
   static_assert(CT::Complete<Ref<Thing>>);
   static_assert(CT::Complete<TMany<Thing*>>);
   static_assert(CT::Complete<UnitMap>);
   static_assert(CT::Complete<TraitMap>);
   static_assert(CT::Complete<Thing>);

   GIVEN("A thing instance") {
      Thing thing;
   }

   // Destroy BANK before static data - otherwise problems happen if    
   // not using managed reflection                                      
   BANK.Reset();

   REQUIRE_FALSE(Allocator::CollectGarbage());
}