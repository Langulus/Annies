///                                                                           
/// Langulus::Annies                                                           
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include <Langulus/Anyness/Text.hpp>
#include <Langulus/Anyness/TOwn.hpp>
#include <Langulus/Anyness/TRef.hpp>
#include <Langulus/Anyness/TMap.hpp>
#include <Langulus/Anyness/Tag.hpp>
#include "Common.hpp"


struct Resolvable {
   Resolvable(DMeta d) : mMeta {d} {}
   DMeta mMeta {};
};

class Unit;
using UnitMap = TMapUnsorted<DMeta, TMany<Unit*>>;
using TagsMap = TMapUnsorted<TMeta, TMany<Tag>>;

struct Thing final : Resolvable {
   using CTTI_Abstract = No;
   using CTTI_Producer = Thing;
   using CTTI_Bases    = Resolvable;

   Thing();

   TOwn<Thing*>  mOwned;
   TRef<Thing*>  mOwner;
   TMany<Thing*> mChildren;
   UnitMap       mUnits;
   TagsMap       mTags;
};

Thing::Thing() : Resolvable {MetaOf<Thing>()} {}

SCENARIO("Testing incomplete type hierarchy", "[incomplete]") {
   static_assert(CT::Complete<Resolvable>);
   static_assert(CT::Complete<TOwn<Thing*>>);
   static_assert(CT::Complete<TRef<Thing*>>);
   static_assert(CT::Complete<TMany<Thing*>>);
   static_assert(CT::Complete<UnitMap>);
   static_assert(CT::Complete<TagsMap>);
   static_assert(CT::Complete<Thing>);

   GIVEN("A thing instance") {
      Thing thing;
   }

   // Destroy BANK before static data - otherwise problems happen if    
   // not using managed reflection                                      
   BANK.Reset();

   REQUIRE_FALSE(Allocator::CollectGarbage());
}