///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "TestManyCommon.hpp"


///                                                                           
///   Refer intents                                                           
///                                                                           
TEMPLATE_TEST_CASE("Testing refer-makable types", "[intents]",
   TMany<AggregateType>,
   TMany<ImplicitlyConstructible>,
   TMany<Destructible>,
   TMany<NonIntentConstructible>,
   TMany<AllIntentConstructible>,
   TMany<AllIntentConstructibleAndAssignable>,
   TMany<PartiallyIntentConstructible>,
   TMany<DescriptorConstructible>,
   TMany<Complex>, TMany<ContainsComplex>,
   TMany<bool>, TMany<uint32_t>, TMany<float>, TMany<char>, TMany<wchar_t>, TMany<char8_t>, TMany<Langulus::Byte>,
   TMany<AMeta>, TMany<TMeta>, TMany<CMeta>, TMany<DMeta>, TMany<VMeta>,
   TMany<ForcefullyPod>,
   TMany<NonDestructible>,
   TMany<PrivatelyConstructible>
) {
   using T = TestType;
   static_assert(CT::ReferMakable<T>);
   static_assert(CT::ReferMakable<T*>);
   static_assert(CT::IntentMakable<Referred, T>);
   static_assert(CT::IntentMakable<Referred, T*>);
   static_assert(CT::IntentMakableAlt<Referred<T>>);
   static_assert(CT::IntentMakableAlt<Referred<T*>>);

   auto meta1 = MetaDataOf<T>();
   REQUIRE(meta1);
   REQUIRE(meta1->mReferConstructor);

   auto meta2 = MetaDataOf<T*>();
   REQUIRE(meta2);
   REQUIRE(meta2->mReferConstructor);
}

/*TEMPLATE_TEST_CASE("Testing non-refer-makable types", "[intents]",
   //TMany<IncompleteType>,
   TMany<NonDestructible>,
   TMany<PrivatelyConstructible>
) {
   using T = TestType;
   static_assert(not CT::ReferMakable<T>);
   static_assert(    CT::ReferMakable<T*>);
   static_assert(not CT::IntentMakable<Referred, T>);
   static_assert(    CT::IntentMakable<Referred, T*>);
   static_assert(not CT::IntentMakableAlt<Referred<T>>);
   static_assert(    CT::IntentMakableAlt<Referred<T*>>);

   auto meta = MetaDataOf<Conditional<CT::Complete<T>, T, T*>>();
   REQUIRE(meta);
   REQUIRE_FALSE(meta->mReferConstructor);
}*/

TEMPLATE_TEST_CASE("Testing refer-assignable types", "[intents]",
   TMany<AggregateType>,
   TMany<ImplicitlyConstructible>,
   TMany<NonDestructible>,
   TMany<Destructible>,
   TMany<NonIntentConstructible>,
   TMany<AllIntentConstructible>,
   TMany<AllIntentConstructibleAndAssignable>,
   TMany<PartiallyIntentConstructible>,
   TMany<DescriptorConstructible>,
   TMany<bool>, TMany<uint32_t>, TMany<float>, TMany<char>, TMany<wchar_t>, TMany<char8_t>, TMany<Langulus::Byte>,
   TMany<AMeta>, TMany<TMeta>, TMany<CMeta>, TMany<DMeta>, TMany<VMeta>,
   TMany<ForcefullyPod>,
   TMany<Complex>,
   TMany<ContainsComplex>,
   TMany<PrivatelyConstructible>
) {
   using T = TestType;
   static_assert(    CT::ReferAssignable<T>);
   static_assert(not CT::ReferAssignable<const T>);
   static_assert(    CT::ReferAssignable<T*>);
   static_assert(    CT::ReferAssignable<const T*>);
   static_assert(    CT::IntentAssignable<Referred, T>);
   static_assert(not CT::IntentAssignable<Referred, const T>);
   static_assert(    CT::IntentAssignable<Referred, T*>);
   static_assert(    CT::IntentAssignable<Referred, const T*>);
   static_assert(    CT::IntentAssignableAlt<Referred<T>>);
   static_assert(not CT::IntentAssignableAlt<Referred<const T>>);
   static_assert(    CT::IntentAssignableAlt<Referred<T*>>);
   static_assert(    CT::IntentAssignableAlt<Referred<const T*>>);

   auto meta1 = MetaDataOf<T>();
   REQUIRE(meta1);
   REQUIRE(meta1->mReferAssigner);

   auto meta2 = MetaDataOf<T*>();
   REQUIRE(meta2);
   REQUIRE(meta2->mReferAssigner);

   auto meta3 = MetaDataOf<const T>();
   REQUIRE(meta3);
   REQUIRE(meta3->mReferAssigner);
}

/*TEMPLATE_TEST_CASE("Testing non-refer-assignable types", "[intents]",
   //TMany<IncompleteType>,
   TMany<Complex>,
   TMany<ContainsComplex>,
   TMany<PrivatelyConstructible>
) {
   using T = TestType;
   static_assert(not CT::ReferAssignable<T>);
   static_assert(not CT::ReferAssignable<const T>);
   static_assert(    CT::ReferAssignable<T*>);
   static_assert(    CT::ReferAssignable<const T*>);
   static_assert(not CT::IntentAssignable<Referred, T>);
   static_assert(not CT::IntentAssignable<Referred, const T>);
   static_assert(    CT::IntentAssignable<Referred, T*>);
   static_assert(    CT::IntentAssignable<Referred, const T*>);
   static_assert(not CT::IntentAssignableAlt<Referred<T>>);
   static_assert(not CT::IntentAssignableAlt<Referred<const T>>);
   static_assert(    CT::IntentAssignableAlt<Referred<T*>>);
   static_assert(    CT::IntentAssignableAlt<Referred<const T*>>);

   auto meta = MetaDataOf<Conditional<CT::Complete<T>, T, T*>>();
   REQUIRE(meta);
   REQUIRE_FALSE(meta->mReferAssigner);
}*/


///                                                                           
///   Move intents                                                            
///                                                                           
TEMPLATE_TEST_CASE("Testing move-makable types", "[intents]",
   TMany<AggregateType>,
   TMany<ImplicitlyConstructible>,
   TMany<Destructible>,
   TMany<NonIntentConstructible>,
   TMany<AllIntentConstructible>,
   TMany<AllIntentConstructibleAndAssignable>,
   TMany<PartiallyIntentConstructible>,
   TMany<DescriptorConstructible>,
   TMany<Complex>,
   TMany<ContainsComplex>,
   TMany<bool>, TMany<uint32_t>, TMany<float>, TMany<char>, TMany<wchar_t>, TMany<char8_t>, TMany<Langulus::Byte>,
   TMany<AMeta>, TMany<TMeta>, TMany<CMeta>, TMany<DMeta>, TMany<VMeta>,
   TMany<ForcefullyPod>,
   TMany<NonDestructible>,
   TMany<PrivatelyConstructible>
) {
   using T = TestType;
   static_assert(CT::MoveMakable<T>);
   static_assert(CT::MoveMakable<T*>);
   static_assert(CT::IntentMakable<Moved, T>);
   static_assert(CT::IntentMakable<Moved, T*>);
   static_assert(CT::IntentMakableAlt<Moved<T>>);
   static_assert(CT::IntentMakableAlt<Moved<T*>>);

   auto meta1 = MetaDataOf<T>();
   REQUIRE(meta1);
   REQUIRE(meta1->mMoveConstructor);

   auto meta2 = MetaDataOf<T*>();
   REQUIRE(meta2);
   REQUIRE(meta2->mMoveConstructor);
}

/*TEMPLATE_TEST_CASE("Testing non-move-makable types", "[intents]",
   //TMany<IncompleteType>,
   TMany<NonDestructible>,
   TMany<PrivatelyConstructible>
) {
   using T = TestType;
   static_assert(not CT::MoveMakable<T>);
   static_assert(    CT::MoveMakable<T*>);
   static_assert(not CT::IntentMakable<Moved, T>);
   static_assert(    CT::IntentMakable<Moved, T*>);
   static_assert(not CT::IntentMakableAlt<Moved<T>>);
   static_assert(    CT::IntentMakableAlt<Moved<T*>>);

   auto meta = MetaDataOf<Conditional<CT::Complete<T>, T, T*>>();
   REQUIRE(meta);
   REQUIRE_FALSE(meta->mMoveConstructor);
}*/

TEMPLATE_TEST_CASE("Testing move-assignable types", "[intents]",
   TMany<AggregateType>,
   TMany<ImplicitlyConstructible>,
   TMany<Destructible>,
   TMany<NonIntentConstructible>,
   TMany<AllIntentConstructible>,
   TMany<AllIntentConstructibleAndAssignable>,
   TMany<PartiallyIntentConstructible>,
   TMany<DescriptorConstructible>,
   TMany<bool>, TMany<uint32_t>, TMany<float>, TMany<char>, TMany<wchar_t>, TMany<char8_t>, TMany<Langulus::Byte>,
   TMany<AMeta>, TMany<TMeta>, TMany<CMeta>, TMany<DMeta>, TMany<VMeta>,
   TMany<ForcefullyPod>,
   TMany<NonDestructible>,
   TMany<Complex>,
   TMany<ContainsComplex>,
   TMany<PrivatelyConstructible>
) {
   using T = TestType;
   static_assert(    CT::MoveAssignable<T>);
   static_assert(not CT::MoveAssignable<const T>);
   static_assert(    CT::MoveAssignable<T*>);
   static_assert(    CT::MoveAssignable<const T*>);
   static_assert(    CT::IntentAssignable<Moved, T>);
   static_assert(not CT::IntentAssignable<Moved, const T>);
   static_assert(    CT::IntentAssignable<Moved, T*>);
   static_assert(    CT::IntentAssignable<Moved, const T*>);
   static_assert(    CT::IntentAssignableAlt<Moved<T>>);
   static_assert(not CT::IntentAssignableAlt<Moved<const T>>);
   static_assert(    CT::IntentAssignableAlt<Moved<T*>>);
   static_assert(    CT::IntentAssignableAlt<Moved<const T*>>);

   auto meta1 = MetaDataOf<T>();
   REQUIRE(meta1);
   REQUIRE(meta1->mMoveAssigner);

   auto meta2 = MetaDataOf<T*>();
   REQUIRE(meta2);
   REQUIRE(meta2->mMoveAssigner);

   auto meta3 = MetaDataOf<const T>();
   REQUIRE(meta3);
   REQUIRE(meta3->mMoveAssigner);
}

/*TEMPLATE_TEST_CASE("Testing non-move-assignable types", "[intents]",
   //TMany<IncompleteType>,
   TMany<NonDestructible>,
   TMany<Complex>,
   TMany<ContainsComplex>,
   TMany<PrivatelyConstructible>
) {
   using T = TestType;
   static_assert(not CT::MoveAssignable<T>);
   static_assert(not CT::MoveAssignable<const T>);
   static_assert(    CT::MoveAssignable<T*>);
   static_assert(    CT::MoveAssignable<const T*>);
   static_assert(not CT::IntentAssignable<Moved, T>);
   static_assert(not CT::IntentAssignable<Moved, const T>);
   static_assert(    CT::IntentAssignable<Moved, T*>);
   static_assert(    CT::IntentAssignable<Moved, const T*>);
   static_assert(not CT::IntentAssignableAlt<Moved<T>>);
   static_assert(not CT::IntentAssignableAlt<Moved<const T>>);
   static_assert(    CT::IntentAssignableAlt<Moved<T*>>);
   static_assert(    CT::IntentAssignableAlt<Moved<const T*>>);

   auto meta = MetaDataOf<Conditional<CT::Complete<T>, T, T*>>();
   REQUIRE(meta);
   REQUIRE_FALSE(meta->mMoveAssigner);
}*/


///                                                                           
///   Copy semantics                                                          
///                                                                           
TEMPLATE_TEST_CASE("Testing copy-makable types", "[intents]",
   TMany<ImplicitlyConstructible>,
   TMany<AggregateType>,
   TMany<AllIntentConstructible>,
   TMany<AllIntentConstructibleImplicit>,
   TMany<AllIntentConstructibleAndAssignable>,
   TMany<PartiallyIntentConstructible>,
   TMany<bool>, TMany<uint32_t>, TMany<float>, TMany<char>, TMany<wchar_t>, TMany<char8_t>, TMany<Langulus::Byte>,
   TMany<AMeta>, TMany<TMeta>, TMany<CMeta>, TMany<DMeta>, TMany<VMeta>,
   TMany<ForcefullyPod>,
   TMany<Destructible>,
   TMany<Complex>,
   TMany<ContainsComplex>,
   TMany<NonIntentConstructible>,
   TMany<DescriptorConstructible>,
   TMany<AggregateTypeComplex>
) {
   using T = TestType;
   static_assert(CT::CopyMakable<T>);
   static_assert(CT::CopyMakable<T*>);
   static_assert(CT::IntentMakable<Copied, T>);
   static_assert(CT::IntentMakable<Copied, T*>);
   static_assert(CT::IntentMakableAlt<Copied<T>>);
   static_assert(CT::IntentMakableAlt<Copied<T*>>);

   auto meta1 = MetaDataOf<T>();
   REQUIRE(meta1);
   REQUIRE(meta1->mCopyConstructor);

   auto meta2 = MetaDataOf<T*>();
   REQUIRE(meta2);
   REQUIRE(meta2->mCopyConstructor);
}

TEMPLATE_TEST_CASE("Testing non-copy-makable types", "[intents]",
   //TMany<IncompleteType>,
   TMany<NonDestructible>,
   TMany<PrivatelyConstructible>
) {
   using T = TestType;
   static_assert(not CT::CopyMakable<T>);
   static_assert(    CT::CopyMakable<T*>);
   static_assert(not CT::IntentMakable<Copied, T>);
   static_assert(    CT::IntentMakable<Copied, T*>);
   static_assert(not CT::IntentMakableAlt<Copied<T>>);
   static_assert(    CT::IntentMakableAlt<Copied<T*>>);

   auto meta = MetaDataOf<Conditional<CT::Complete<T>, T, T*>>();
   REQUIRE(meta);
   //REQUIRE_FALSE(meta->mCopyConstructor);
   REQUIRE(meta->mCopyConstructor); 
   // Since the introduction of LANGULUS(ACT_AS), these are still copy-constructible, because
   // they're implicitly inserted as a plain Many type. You would still not be able to construct 
   // such a container at compile-time, so it's completely fine
}

TEMPLATE_TEST_CASE("Testing copy-assignable types", "[intents]",
   TMany<ImplicitlyConstructible>,
   TMany<AggregateType>,
   TMany<AllIntentConstructibleImplicit>,
   TMany<AllIntentConstructibleAndAssignable>,
   TMany<bool>, TMany<uint32_t>, TMany<float>, TMany<char>, TMany<wchar_t>, TMany<char8_t>, TMany<Langulus::Byte>,
   TMany<AMeta>, TMany<TMeta>, TMany<CMeta>, TMany<DMeta>, TMany<VMeta>,
   TMany<ForcefullyPod>,
   TMany<Destructible>,
   TMany<Complex>,
   TMany<ContainsComplex>,
   TMany<NonIntentConstructible>,
   TMany<DescriptorConstructible>,
   TMany<AllIntentConstructible>,
   TMany<PartiallyIntentConstructible>,
   TMany<AggregateTypeComplex>
) {
   using T = TestType;
   static_assert(    CT::CopyAssignable<T>);
   static_assert(not CT::CopyAssignable<const T>);
   static_assert(    CT::CopyAssignable<T*>);
   static_assert(    CT::CopyAssignable<const T*>);
   static_assert(    CT::IntentAssignable<Copied, T>);
   static_assert(not CT::IntentAssignable<Copied, const T>);
   static_assert(    CT::IntentAssignable<Copied, T*>);
   static_assert(    CT::IntentAssignable<Copied, const T*>);
   static_assert(    CT::IntentAssignableAlt<Copied<T>>);
   static_assert(not CT::IntentAssignableAlt<Copied<const T>>);
   static_assert(    CT::IntentAssignableAlt<Copied<T*>>);
   static_assert(    CT::IntentAssignableAlt<Copied<const T*>>);

   auto meta1 = MetaDataOf<T>();
   REQUIRE(meta1);
   REQUIRE(meta1->mCopyAssigner);

   auto meta2 = MetaDataOf<T*>();
   REQUIRE(meta2);
   REQUIRE(meta2->mCopyAssigner);

   auto meta3 = MetaDataOf<const T>();
   REQUIRE(meta3);
   REQUIRE(meta3->mCopyAssigner);
}

TEMPLATE_TEST_CASE("Testing non-copy-assignable types", "[intents]",
   //TMany<IncompleteType>,
   TMany<NonDestructible>,
   TMany<PrivatelyConstructible>
) {
   using T = TestType;
   static_assert(not CT::CopyAssignable<T>);
   static_assert(not CT::CopyAssignable<const T>);
   static_assert(    CT::CopyAssignable<T*>);
   static_assert(    CT::CopyAssignable<const T*>);
   static_assert(not CT::IntentAssignable<Copied, T>);
   static_assert(not CT::IntentAssignable<Copied, const T>);
   static_assert(    CT::IntentAssignable<Copied, T*>);
   static_assert(    CT::IntentAssignable<Copied, const T*>);
   static_assert(not CT::IntentAssignableAlt<Copied<T>>);
   static_assert(not CT::IntentAssignableAlt<Copied<const T>>);
   static_assert(    CT::IntentAssignableAlt<Copied<T*>>);
   static_assert(    CT::IntentAssignableAlt<Copied<const T*>>);

   auto meta = MetaDataOf<Conditional<CT::Complete<T>, T, T*>>();
   REQUIRE(meta);
   //REQUIRE_FALSE(meta->mCopyAssigner);
   REQUIRE(meta->mCopyAssigner);
   // Since the introduction of LANGULUS(ACT_AS), these are still copy-assignable, because
   // they're implicitly inserted as a plain Many type. You would still not be able to assign 
   // such a container at compile-time, so it's completely fine
}


///                                                                           
///   Clone semantics                                                         
///                                                                           
TEMPLATE_TEST_CASE("Testing clone-makable types", "[intents]",
   TMany<ImplicitlyConstructible>,
   TMany<AllIntentConstructible>,
   TMany<AllIntentConstructibleAndAssignable>,
   TMany<PartiallyIntentConstructible>,
   TMany<bool>, TMany<uint32_t>, TMany<float>, TMany<char>, TMany<wchar_t>, TMany<char8_t>, TMany<Langulus::Byte>,
   TMany<AMeta>, TMany<TMeta>, TMany<CMeta>, TMany<DMeta>, TMany<VMeta>,
   TMany<ForcefullyPod>,
   TMany<AggregateType>
) {
   using T = TestType;
   static_assert(CT::CloneMakable<T>);
   static_assert(CT::CloneMakable<T*>);
   static_assert(CT::IntentMakable<Cloned, T>);
   static_assert(CT::IntentMakable<Cloned, T*>);
   static_assert(CT::IntentMakableAlt<Cloned<T>>);
   static_assert(CT::IntentMakableAlt<Cloned<T*>>);

   auto meta1 = MetaDataOf<T>();
   REQUIRE(meta1);
   REQUIRE(meta1->mCloneConstructor);

   auto meta2 = MetaDataOf<T*>();
   REQUIRE(meta2);
   REQUIRE(meta2->mCloneConstructor);
}

TEMPLATE_TEST_CASE("Testing non-clone-makable types", "[intents]",
   //TMany<IncompleteType>,
   TMany<NonDestructible>,
   TMany<Destructible>,
   TMany<PrivatelyConstructible>,
   TMany<NonIntentConstructible>,
   TMany<DescriptorConstructible>,
   TMany<Complex>,
   TMany<ContainsComplex>,
   TMany<AggregateTypeComplex>
) {
   using T = TestType;
   static_assert(not CT::DeepMakable<TypeOf<T>, Cloned<TestType>>);
   static_assert(not CT::CloneMakable<T>);
   static_assert(not CT::CloneMakable<T*>);
   static_assert(not CT::IntentMakable<Cloned, T>);
   static_assert(not CT::IntentMakable<Cloned, T*>);
   static_assert(not CT::IntentMakableAlt<Cloned<T>>);
   static_assert(not CT::IntentMakableAlt<Cloned<T*>>);

   auto meta = MetaDataOf<Conditional<CT::Complete<T>, T, T*>>();
   REQUIRE(meta);
   //REQUIRE_FALSE(meta->mCloneConstructor);
   REQUIRE(meta->mCloneConstructor);
   // Since the introduction of LANGULUS(ACT_AS), these are still clone-constructible, because
   // they're implicitly inserted as a plain Many type. You would still not be able to construct 
   // such a container at compile-time, so it's completely fine
}

TEMPLATE_TEST_CASE("Testing clone-assignable types", "[intents]",
   TMany<ImplicitlyConstructible>,
   TMany<AllIntentConstructibleImplicit>,
   TMany<AllIntentConstructibleAndAssignable>,
   TMany<bool>, TMany<uint32_t>, TMany<float>, TMany<char>, TMany<wchar_t>, TMany<char8_t>, TMany<Langulus::Byte>,
   TMany<AMeta>, TMany<TMeta>, TMany<CMeta>, TMany<DMeta>, TMany<VMeta>,
   TMany<ForcefullyPod>,
   TMany<AggregateType>,
   TMany<AllIntentConstructible>,       // see @attention notice in the test below
   TMany<PartiallyIntentConstructible>  // see @attention notice in the test below
) {
   using T = TestType;
   static_assert(    CT::CloneAssignable<T>);
   static_assert(not CT::CloneAssignable<const T>);
   static_assert(    CT::CloneAssignable<T*>);
   static_assert(not CT::CloneAssignable<const T*>);
   static_assert(    CT::IntentAssignable<Cloned, T>);
   static_assert(not CT::IntentAssignable<Cloned, const T>);
   static_assert(    CT::IntentAssignable<Cloned, T*>);
   static_assert(not CT::IntentAssignable<Cloned, const T*>);
   static_assert(    CT::IntentAssignableAlt<Cloned<T>>);
   static_assert(not CT::IntentAssignableAlt<Cloned<const T>>);
   static_assert(    CT::IntentAssignableAlt<Cloned<T*>>);
   static_assert(not CT::IntentAssignableAlt<Cloned<const T*>>);

   auto meta1 = MetaDataOf<T>();
   REQUIRE(meta1);
   REQUIRE(meta1->mCloneAssigner);

   auto meta2 = MetaDataOf<T*>();
   REQUIRE(meta2);
   REQUIRE(meta2->mCloneAssigner);

   auto meta3 = MetaDataOf<const T>();
   REQUIRE(meta3);
   REQUIRE(meta3->mCloneAssigner);
}

TEMPLATE_TEST_CASE("Testing non-clone-assignable types", "[intents]",
   //TMany<IncompleteType>,
   TMany<NonDestructible>,
   TMany<Destructible>,
   TMany<PrivatelyConstructible>,
   TMany<NonIntentConstructible>,
   TMany<Complex>,
   TMany<ContainsComplex>,
   //TMany<AllIntentConstructible>,        // see @attention notice below    
   //TMany<PartiallyIntentConstructible>,  // see @attention notice below    
   TMany<DescriptorConstructible>,
   TMany<AggregateTypeComplex>
) {
   using T = TestType;
   static_assert(not CT::DeepAssignable<TypeOf<T>, Cloned<TestType>>);
   // @attention since TMany semantic constructor isn't explicit, types 
   // that have explicit semantic constructors (and thus no implicit    
   // semantic assigners), will still be clone-assignable when wrapped  
   // in a TMany - new elements will simply be re-constructed in the    
   // container. This might not be intended, but is too hard to work    
   // around currently.                                                 
   static_assert(not CT::CloneAssignable<T>);
   static_assert(not CT::CloneAssignable<const T>);
   static_assert(not CT::CloneAssignable<T*>);
   static_assert(not CT::CloneAssignable<const T*>);
   static_assert(not CT::IntentAssignable<Cloned, T>);
   static_assert(not CT::IntentAssignable<Cloned, const T>);
   static_assert(not CT::IntentAssignable<Cloned, T*>);
   static_assert(not CT::IntentAssignable<Cloned, const T*>);
   static_assert(not CT::IntentAssignableAlt<Cloned<T>>);
   static_assert(not CT::IntentAssignableAlt<Cloned<const T>>);
   static_assert(not CT::IntentAssignableAlt<Cloned<T*>>);
   static_assert(not CT::IntentAssignableAlt<Cloned<const T*>>);

   auto meta = MetaDataOf<Conditional<CT::Complete<T>, T, T*>>();
   REQUIRE(meta);
   //REQUIRE_FALSE(meta->mCloneAssigner);
   REQUIRE(meta->mCloneAssigner);
   // Since the introduction of LANGULUS(ACT_AS), these are still clone-assignable, because
   // they're implicitly inserted as a plain Many type. You would still not be able to assign 
   // such a container at compile-time, so it's completely fine
}


///                                                                           
///   Disown intents                                                          
///                                                                           
TEMPLATE_TEST_CASE("Testing disown-makable types", "[intents]",
   TMany<ImplicitlyConstructible>,
   TMany<AllIntentConstructible>,
   TMany<AllIntentConstructibleAndAssignable>,
   TMany<PartiallyIntentConstructible>,
   TMany<bool>, TMany<uint32_t>, TMany<float>, TMany<char>, TMany<wchar_t>, TMany<char8_t>, TMany<Langulus::Byte>,
   TMany<AMeta>, TMany<TMeta>, TMany<CMeta>, TMany<DMeta>, TMany<VMeta>,
   TMany<ForcefullyPod>,
   TMany<AggregateType>,
   TMany<NonDestructible>,
   TMany<Destructible>,
   TMany<PrivatelyConstructible>,
   TMany<NonIntentConstructible>,
   TMany<DescriptorConstructible>,
   TMany<Complex>,
   TMany<ContainsComplex>,
   TMany<AggregateTypeComplex>
) {
   using T = TestType;
   static_assert(CT::DisownMakable<T>);
   static_assert(CT::DisownMakable<T*>);
   static_assert(CT::IntentMakable<Disowned, T>);
   static_assert(CT::IntentMakable<Disowned, T*>);
   static_assert(CT::IntentMakableAlt<Disowned<T>>);
   static_assert(CT::IntentMakableAlt<Disowned<T*>>);

   auto meta1 = MetaDataOf<T>();
   REQUIRE(meta1);
   REQUIRE(meta1->mDisownConstructor);

   auto meta2 = MetaDataOf<T*>();
   REQUIRE(meta2);
   REQUIRE(meta2->mDisownConstructor);
}

/*TEMPLATE_TEST_CASE("Testing non-disown-makable types", "[intents]",
   //TMany<IncompleteType>,
   TMany<PrivatelyConstructible>,
   TMany<NonSemanticConstructible>,
   TMany<DescriptorConstructible>,
   TMany<Complex>,
   TMany<ContainsComplex>,
   TMany<AggregateTypeComplex>
) {
   using T = TestType;
   static_assert(not CT::DisownMakable<T>);
   static_assert(    CT::DisownMakable<T*>);
   static_assert(not CT::IntentMakable<Disowned, T>);
   static_assert(    CT::IntentMakable<Disowned, T*>);
   static_assert(not CT::IntentMakableAlt<Disowned<T>>);
   static_assert(    CT::IntentMakableAlt<Disowned<T*>>);

   auto meta = MetaDataOf<Conditional<CT::Complete<T>, T, T*>>();
   REQUIRE(meta);
   REQUIRE_FALSE(meta->mDisownConstructor);
}*/

TEMPLATE_TEST_CASE("Testing disown-assignable types", "[intents]",
   TMany<ImplicitlyConstructible>,
   TMany<AllIntentConstructibleImplicit>,
   TMany<AllIntentConstructibleAndAssignable>,
   TMany<bool>, TMany<uint32_t>, TMany<float>, TMany<char>, TMany<wchar_t>, TMany<char8_t>, TMany<Langulus::Byte>,
   TMany<AMeta>, TMany<TMeta>, TMany<CMeta>, TMany<DMeta>, TMany<VMeta>,
   TMany<ForcefullyPod>,
   TMany<AggregateType>,
   TMany<NonDestructible>,
   TMany<Destructible>,
   TMany<PrivatelyConstructible>,
   TMany<NonIntentConstructible>,
   TMany<DescriptorConstructible>,
   TMany<Complex>,
   TMany<ContainsComplex>,
   TMany<AllIntentConstructible>,
   TMany<PartiallyIntentConstructible>
) {
   using T = TestType;
   static_assert(    CT::DisownAssignable<T>);
   static_assert(not CT::DisownAssignable<const T>);
   static_assert(    CT::DisownAssignable<T*>);
   static_assert(    CT::DisownAssignable<const T*>);
   static_assert(    CT::IntentAssignable<Disowned, T>);
   static_assert(not CT::IntentAssignable<Disowned, const T>);
   static_assert(    CT::IntentAssignable<Disowned, T*>);
   static_assert(    CT::IntentAssignable<Disowned, const T*>);
   static_assert(    CT::IntentAssignableAlt<Disowned<T>>);
   static_assert(not CT::IntentAssignableAlt<Disowned<const T>>);
   static_assert(    CT::IntentAssignableAlt<Disowned<T*>>);
   static_assert(    CT::IntentAssignableAlt<Disowned<const T*>>);

   auto meta1 = MetaDataOf<T>();
   REQUIRE(meta1);
   REQUIRE(meta1->mDisownAssigner);

   auto meta2 = MetaDataOf<T*>();
   REQUIRE(meta2);
   REQUIRE(meta2->mDisownAssigner);

   auto meta3 = MetaDataOf<const T>();
   REQUIRE(meta3);
   REQUIRE(meta3->mDisownAssigner);
}

/*TEMPLATE_TEST_CASE("Testing non-disown-assignable types", "[intents]",
   //TMany<IncompleteType>,
   TMany<NonDestructible>,
   TMany<Destructible>,
   TMany<PrivatelyConstructible>,
   TMany<NonIntentConstructible>,
   TMany<DescriptorConstructible>,
   TMany<Complex>,
   TMany<ContainsComplex>,
   TMany<AllIntentConstructible>,
   TMany<PartiallyIntentConstructible>
) {
   using T = TestType;
   static_assert(not CT::DisownAssignable<T>);
   static_assert(not CT::DisownAssignable<const T>);
   static_assert(    CT::DisownAssignable<T*>);
   static_assert(    CT::DisownAssignable<const T*>);
   static_assert(not CT::IntentAssignable<Disowned, T>);
   static_assert(not CT::IntentAssignable<Disowned, const T>);
   static_assert(    CT::IntentAssignable<Disowned, T*>);
   static_assert(    CT::IntentAssignable<Disowned, const T*>);
   static_assert(not CT::IntentAssignableAlt<Disowned<T>>);
   static_assert(not CT::IntentAssignableAlt<Disowned<const T>>);
   static_assert(    CT::IntentAssignableAlt<Disowned<T*>>);
   static_assert(    CT::IntentAssignableAlt<Disowned<const T*>>);

   auto meta = MetaDataOf<Conditional<CT::Complete<T>, T, T*>>();
   REQUIRE(meta);
   REQUIRE_FALSE(meta->mDisownAssigner);
}*/


///                                                                           
///   Abandon intents                                                         
///                                                                           
TEMPLATE_TEST_CASE("Testing abandon-makable types", "[intents]",
   TMany<ImplicitlyConstructible>,
   TMany<Destructible>,
   TMany<NonIntentConstructible>,
   TMany<AllIntentConstructible>,
   TMany<AllIntentConstructibleAndAssignable>,
   TMany<PartiallyIntentConstructible>,
   TMany<DescriptorConstructible>,
   TMany<Complex>,
   TMany<ContainsComplex>,
   TMany<bool>, TMany<uint32_t>, TMany<float>, TMany<char>, TMany<wchar_t>, TMany<char8_t>, TMany<Langulus::Byte>,
   TMany<AMeta>, TMany<TMeta>, TMany<CMeta>, TMany<DMeta>, TMany<VMeta>,
   TMany<AggregateType>,
   TMany<ForcefullyPod>,
   TMany<NonDestructible>,
   TMany<PrivatelyConstructible>
) {
   using T = TestType;
   static_assert(CT::AbandonMakable<T>);
   static_assert(CT::AbandonMakable<T*>);
   static_assert(CT::IntentMakable<Abandoned, T>);
   static_assert(CT::IntentMakable<Abandoned, T*>);
   static_assert(CT::IntentMakableAlt<Abandoned<T>>);
   static_assert(CT::IntentMakableAlt<Abandoned<T*>>);

   auto meta1 = MetaDataOf<T>();
   REQUIRE(meta1);
   REQUIRE(meta1->mAbandonConstructor);

   auto meta2 = MetaDataOf<T*>();
   REQUIRE(meta2);
   REQUIRE(meta2->mAbandonConstructor);
}

/*TEMPLATE_TEST_CASE("Testing non-abandon-makable types", "[intents]",
   //TMany<IncompleteType>,
   TMany<NonDestructible>,
   TMany<PrivatelyConstructible>
) {
   using T = TestType;
   static_assert(not CT::AbandonMakable<T>);
   static_assert(    CT::AbandonMakable<T*>);
   static_assert(not CT::IntentMakable<Abandoned, T>);
   static_assert(    CT::IntentMakable<Abandoned, T*>);
   static_assert(not CT::IntentMakableAlt<Abandoned<T>>);
   static_assert(    CT::IntentMakableAlt<Abandoned<T*>>);

   auto meta = MetaDataOf<Conditional<CT::Complete<T>, T, T*>>();
   REQUIRE(meta);
   REQUIRE_FALSE(meta->mAbandonConstructor);
}*/

TEMPLATE_TEST_CASE("Testing abandon-assignable types", "[intents]",
   TMany<ImplicitlyConstructible>,
   TMany<Destructible>,
   TMany<NonIntentConstructible>,
   TMany<AllIntentConstructible>,
   TMany<AllIntentConstructibleAndAssignable>,
   TMany<PartiallyIntentConstructible>,
   TMany<DescriptorConstructible>,
   TMany<bool>, TMany<uint32_t>, TMany<float>, TMany<char>, TMany<wchar_t>, TMany<char8_t>, TMany<Langulus::Byte>,
   TMany<AMeta>, TMany<TMeta>, TMany<CMeta>, TMany<DMeta>, TMany<VMeta>,
   TMany<AggregateType>,
   TMany<ForcefullyPod>,
   TMany<NonDestructible>,
   TMany<Complex>,
   TMany<ContainsComplex>,
   TMany<PrivatelyConstructible>
) {
   using T = TestType;
   static_assert(    CT::AbandonAssignable<T>);
   static_assert(not CT::AbandonAssignable<const T>);
   static_assert(    CT::AbandonAssignable<T*>);
   static_assert(    CT::AbandonAssignable<const T*>);
   static_assert(    CT::IntentAssignable<Abandoned, T>);
   static_assert(not CT::IntentAssignable<Abandoned, const T>);
   static_assert(    CT::IntentAssignable<Abandoned, T*>);
   static_assert(    CT::IntentAssignable<Abandoned, const T*>);
   static_assert(    CT::IntentAssignableAlt<Abandoned<T>>);
   static_assert(not CT::IntentAssignableAlt<Abandoned<const T>>);
   static_assert(    CT::IntentAssignableAlt<Abandoned<T*>>);
   static_assert(    CT::IntentAssignableAlt<Abandoned<const T*>>);

   auto meta1 = MetaDataOf<T>();
   REQUIRE(meta1);
   REQUIRE(meta1->mAbandonAssigner);

   auto meta2 = MetaDataOf<T*>();
   REQUIRE(meta2);
   REQUIRE(meta2->mAbandonAssigner);

   auto meta3 = MetaDataOf<const T>();
   REQUIRE(meta3);
   REQUIRE(meta3->mAbandonAssigner);
}

/*TEMPLATE_TEST_CASE("Testing non-abandon-assignable types", "[intents]",
   //TMany<IncompleteType>,
   TMany<NonDestructible>,
   TMany<Complex>,
   TMany<ContainsComplex>,
   TMany<PrivatelyConstructible>
) {
   using T = TestType;
   static_assert(not CT::AbandonAssignable<T>);
   static_assert(not CT::AbandonAssignable<const T>);
   static_assert(    CT::AbandonAssignable<T*>);
   static_assert(    CT::AbandonAssignable<const T*>);
   static_assert(not CT::IntentAssignable<Abandoned, T>);
   static_assert(not CT::IntentAssignable<Abandoned, const T>);
   static_assert(    CT::IntentAssignable<Abandoned, T*>);
   static_assert(    CT::IntentAssignable<Abandoned, const T*>);
   static_assert(not CT::IntentAssignableAlt<Abandoned<T>>);
   static_assert(not CT::IntentAssignableAlt<Abandoned<const T>>);
   static_assert(    CT::IntentAssignableAlt<Abandoned<T*>>);
   static_assert(    CT::IntentAssignableAlt<Abandoned<const T*>>);

   auto meta = MetaDataOf<Conditional<CT::Complete<T>, T, T*>>();
   REQUIRE(meta);
   REQUIRE_FALSE(meta->mAbandonAssigner);
}
*/

///                                                                           
///   Descriptor intents                                                      
///                                                                           
TEMPLATE_TEST_CASE("Testing descriptor-makable types", "[intents]",
   TMany<AllIntentConstructible>,
   TMany<AllIntentConstructibleAndAssignable>,
   TMany<DescriptorConstructible>
) {
   using T = TestType;
   static_assert(    CT::DeepMakable<TypeOf<T>, Describe>);
   static_assert(not CT::DeepAssignable<TypeOf<T>, Describe>);
   static_assert(    CT::DescriptorMakable<T>);
   static_assert(not CT::DescriptorMakable<T*>);
   static_assert(not CT::IntentMakableAlt<Describe>);

   auto meta1 = MetaDataOf<T>();
   REQUIRE(meta1);
   //REQUIRE(meta1->mDescriptorConstructor);
   REQUIRE_FALSE(meta1->mDescriptorConstructor);

   auto meta2 = MetaDataOf<T*>();
   REQUIRE(meta2);
   //REQUIRE(meta2->mDescriptorConstructor);
   REQUIRE_FALSE(meta2->mDescriptorConstructor);

   // Since the introduction of LANGULUS(ACT_AS), these are no longer describable, because
   // they're implicitly inserted as a plain Many type
}

TEMPLATE_TEST_CASE("Testing non-descriptor-makable types", "[intents]",
   //TMany<IncompleteType>,
   TMany<ImplicitlyConstructible>,
   TMany<NonDestructible>,
   TMany<Destructible>,
   TMany<PrivatelyConstructible>,
   TMany<NonIntentConstructible>,
   TMany<PartiallyIntentConstructible>,
   TMany<Complex>,
   TMany<ContainsComplex>,
   TMany<bool>, TMany<uint32_t>, TMany<float>, TMany<char>, TMany<wchar_t>, TMany<char8_t>, TMany<Langulus::Byte>,
   TMany<AMeta>, TMany<TMeta>, TMany<CMeta>, TMany<DMeta>, TMany<VMeta>,
   TMany<AggregateType>,
   TMany<ForcefullyPod>,
   TMany<AggregateThatCanBeConfusedWithDescriptorMakable>
) {
   using T = TestType;
   static_assert(not CT::UnfoldMakableFrom<TypeOf<T>, Describe>);
   static_assert(not CT::DeepMakable<TypeOf<T>, Describe>);
   static_assert(not CT::DeepAssignable<TypeOf<T>, Describe>);
   static_assert(not CT::DescriptorMakable<T>);
   static_assert(not CT::DescriptorMakable<T*>);
   static_assert(not CT::IntentMakableAlt<Describe>);

   auto meta = MetaDataOf<Conditional<CT::Complete<T>, T, T*>>();
   REQUIRE(meta);
   REQUIRE_FALSE(meta->mDescriptorConstructor);
}
