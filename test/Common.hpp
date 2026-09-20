///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           

/// INTENTIONALLY NOT GUARDED                                                 
/// Include this file once in each cpp file, after all other headers          
#include <Langulus/Annies/Many.hpp>
#include <Langulus/Testing.hpp>

using namespace Annies;


template<class L, class R>
struct TypePair {
   using LHS = L;
   using RHS = R;
};

/// Used to configure a map test                                              
///   @param C - the map type we're testing                                   
///   @param K - the tested key type                                          
///   @param V - the tested value type                                        
///   @param MANAGED - true to contain memory allocated by our manager        
template<class C, class K, class V, bool MANAGED = false>
struct MapTest {
   using Container = C;
   using Key = K;
   using Value = V;
   static constexpr bool Managed = MANAGED;
};

/// Used to configure a set test                                              
///   @param C - the map type we're testing                                   
///   @param K - the tested key type                                          
///   @param MANAGED - true to contain memory allocated by our manager        
template<class C, class K, bool MANAGED = false>
struct SetTest {
   using Container = C;
   using Key = K;
   static constexpr bool Managed = MANAGED;
};

/// Type for testing hashing consistency between two containers               
///   @tparam K - the left container                                          
///   @tparam V - the right container                                         
template<class K, class V>
struct HashTest {
   using Key = K;
   using Value = V;
};


/// Create a dense element, on the stack                                      
///   @tparam T - type of element we're creating                              
///   @param e - the data we'll use to initialize an instance of T            
///   @return the new instance of T                                           
/*template<CT::Dense T, bool = false>
T CreateElement(const auto& e) {
   T element;
   if constexpr (CT::Same<T, decltype(e)>)
      element = e;
   else if constexpr (not CT::Same<T, Block<>>)
      element = Decay<T> {e};
   else {
      element = Block<> {};
      element.Insert(e);
   }

   return element;
}*/

/// Create a sparse element, on the heap                                      
///   @tparam T - type of element we're creating                              
///   @tparam MANAGED - whether we'll have authority over the pointer or not  
///   @param e - the data we'll use to initialize an instance of T            
///   @return pointer to the new instance of T                                
/*template<CT::Sparse T, bool MANAGED = false>
T CreateElement(const auto& e) {
   void* element;

   if constexpr (not MANAGED) {
      // Create a pointer that is guaranteed to not be owned by the     
      // memory manager. Notice we don't use 'new' operator here,       
      // because it is weakly linked, and can be overriden to use our   
      // memory manager.                                                
      if constexpr (not CT::Same<T, Block<>>) {
         element = malloc(sizeof(Decay<T>));
         new (element) Decay<T> {e};
      }
      else {
         element = malloc(sizeof(Block<>));
         new (element) Block<> {};
         static_cast<Block<>*>(element)->Insert(e);
      }
   }
   else {
      // Create a pointer owned by the memory manager                   
      auto& container = BANK.Emplace(IndexBack);

      if constexpr (not CT::Same<T, Block<>>) {
         container << Decay<T> {e};
         element = container.GetRaw();
      }
      else {
         container << e;
         element = &container;
      }
   }

   return static_cast<T>(element);
}

template<bool MANAGED = false>
void DestroyElement(auto e) {
   using E = decltype(e);
   if constexpr (CT::Sparse<E>) {
      if constexpr (CT::Referencable<Deptr<E>>)
         e->Reference(-1);

      if constexpr (CT::Destroyable<Decay<E>>)
         e->~Decay<E>();

      if constexpr (not MANAGED)
         free(e);
   }
}*/

/// Create a test pair                                                        
///   @tparam P - the pair type                                               
///   @tparam K - the pair key type                                           
///   @tparam V - the pair value type                                         
///   @tparam MANAGED - whether or not we have auhtority over the data        
///   @param key - the key initialization data                                
///   @param value - the value initialization data                            
///   @return the pair                                                        
template<class P, class K, class V, bool MANAGED = false>
P CreatePair(const auto& key, const auto& value) {
   return P {
      CreateElement<K, MANAGED>(key),
      CreateElement<V, MANAGED>(value)
   };
}

/// Destroy a test pair created via CreatePair                                
///   @tparam MANAGED - was it created by the memory manager?                 
///   @param pair - the pair to destroy                                       
template<bool MANAGED = false>
void DestroyPair(auto& pair) {
   using P = Deref<decltype(pair)>;

   if constexpr (not MANAGED) {
      if constexpr (requires { pair.GetKey(); }) {
         if constexpr (CT::Typed<P>) {
            // It's a statically typed langulus pair                    
            using K = typename P::Key;
            using V = typename P::Value;

            if constexpr (CT::Sparse<K>) {
               if constexpr (CT::Referencable<Deptr<K>>)
                  REQUIRE(pair.GetKey()->Reference(-1) == 0);
               if constexpr (CT::Destroyable<Decay<K>>)
                  pair.GetKey()->~Decay<K>();
               free(pair.GetKey());
            }
            else if constexpr (requires (K k) { k.Reset(); })
               DecvqCast(pair.GetKey()).Reset();

            if constexpr (CT::Sparse<V>) {
               if constexpr (CT::Referencable<Deptr<V>>)
                  REQUIRE(pair.GetValue()->Reference(-1) == 0);
               if constexpr (CT::Destroyable<Decay<V>>)
                  pair.GetValue()->~Decay<V>();
               free(pair.GetValue());
            }
            else if constexpr (requires (V v) { v.Reset(); })
               DecvqCast(pair.GetValue()).Reset();
         }
         else {
            if (pair.GetKey().IsSparse()) {
               if (pair.GetKey().GetType()->mReference)
                  REQUIRE(pair.GetKey().GetType()->mReference(*pair.GetKey().template GetRaw<void*>(), -1) == 0);
               if (pair.GetKey().GetType()->mDestructor)
                  pair.GetKey().GetType()->mDestructor(*pair.GetKey().template GetRaw<void*>());
               free(*pair.GetKey().template GetRaw<void*>());
            }
            
            DecvqCast(pair.GetKey()).Reset();

            if (pair.GetValue().IsSparse()) {
               if (pair.GetValue().GetType()->mReference)
                  REQUIRE(pair.GetValue().GetType()->mReference(*pair.GetValue().template GetRaw<void*>(), -1) == 0);
               if (pair.GetValue().GetType()->mDestructor)
                  pair.GetValue().GetType()->mDestructor(*pair.GetValue().template GetRaw<void*>());
               free(*pair.GetValue().template GetRaw<void*>());
            }
            
            DecvqCast(pair.GetValue()).Reset();
         }
      }
      else if constexpr (requires { pair.first; }) {
         // It's an std::pair                                           
         using K = decltype(pair.first);
         using V = decltype(pair.second);

         if constexpr (CT::Sparse<K>) {
            if constexpr (CT::Referencable<Deptr<K>>)
               REQUIRE(pair.first->Reference(-1) == 0);
            if constexpr (CT::Destroyable<Decay<K>>)
               pair.first->~Decay<K>();
            free(pair.first);
         }
         else if constexpr (requires (K k) { k.Reset(); })
            DecvqCast(pair.first).Reset();

         if constexpr (CT::Sparse<V>) {
            if constexpr (CT::Referencable<Deptr<V>>)
               REQUIRE(pair.second->Reference(-1) == 0);
            if constexpr (CT::Destroyable<Decay<V>>)
               pair.second->~Decay<V>();
            free(pair.second);
         }
         else if constexpr (requires (V v) { v.Reset(); })
            DecvqCast(pair.second).Reset();
      }
      else static_assert(false, "What kind of pair is this? Are you making stuff up?");
   }
   else BANK.Reset();
}


namespace Langulus::Flow
{
   struct Verb {};
   class Construct {};
   class Constructconst {};
   class constConstructconst {};
   class constconst {};
}

/// An empty trivial type                                                     
class ImplicitlyConstructible {};

/// A simple aggregate type                                                   
struct AggregateType {
   int m1, m2, m3, m4;
   bool m5;
};

/// Explicitly deleted destructor                                             
class NonDestructible {
   ~NonDestructible() = delete;
};

/// Has an explicit destructor                                                
class Destructible {
public:
   char* someptr {};

   ~Destructible() {
      if (someptr)
         delete someptr;
   }
};

/// Default-constructible, but only privately                                 
class PrivatelyConstructible {
   LANGULUS(POD) false;
private:
   PrivatelyConstructible() = default;
   PrivatelyConstructible(const PrivatelyConstructible&) = default;
   PrivatelyConstructible(PrivatelyConstructible&&) = default;
};

/// Has no explicit intent constructors and assigners                         
/// Has only implicit refer & move constructors and assigners                 
class NonIntentConstructible {
   LANGULUS(POD) false;
public:
   NonIntentConstructible(CT::NoIntent auto&&) {}
};

/// Has explicit copy, move, refer, clone, abandon, disown constructors       
/// Has implicit refer & move constructors, too                               
/// Has no explicit intent assigners, only implicit refer & move              
class PartiallyIntentConstructible {
public:
   template<template<class> class S, class T>
   PartiallyIntentConstructible(S<T>&&) requires CT::Intent<S<T>> {}
};

/// Has all intent constructors + implicit refer & move ones                  
/// Has no explicit intent assigners, only implicit refer & move ones         
/// Making constructor explicit makes sure, that no implicit intent assign    
/// happens                                                                   
class AllIntentConstructible {
public:
   LANGULUS(POD) false;
   explicit AllIntentConstructible(CT::Intent auto&&) {}
};

/// Has all intent constructors + implicit refer & move ones                  
/// Has no explicit intent assigners, only implicit refer & move ones         
/// Making constructor implicit also allows for intent assignments            
class AllIntentConstructibleImplicit {
public:
   LANGULUS(POD) false;
   AllIntentConstructibleImplicit(CT::Intent auto&&) {}
};

/// Has all intnet constructors and assigners + implicit refer & move ones    
class AllIntentConstructibleAndAssignable {
public:
   LANGULUS(POD) false;
   AllIntentConstructibleAndAssignable(CT::Intent auto&&) {}
   AllIntentConstructibleAndAssignable& operator = (CT::Intent auto&&) { return *this; }
};

/// Has explicit descriptor constructor, and implicit refer & move ones       
/// Has no explicit intent assigners, only implicit refer & move              
class DescriptorConstructible {
public:
   DescriptorConstructible(Describe) {}
};


enum class Pi {
   Number = 314
};

struct IncompleteType;

namespace One::Two::Three {
   struct TypeDeepIntoNamespaces;

   template<class T>
   struct TemplatedTypeDeepIntoNamespaces {
      enum VeryDeeplyTemplatedEnum { YesYouGotThatRight };

      template<class MORE>
      struct Nested;
   };

   template<class T>
   struct VeryComplexTemplate;
}

namespace Verbs
{

   ///                                                                        
   /// A testing verb, similar to the ones used in Langulus::Flow             
   ///                                                                        
   struct Create : public Flow::Verb {
      LANGULUS(POSITIVE_VERB) "Create";
      LANGULUS(NEGATIVE_VERB) "Destroy";
      LANGULUS(POSITIVE_OPERATOR) " + ";
      LANGULUS(NEGATIVE_OPERATOR) " - ";
      LANGULUS(PRECEDENCE) 5;
      LANGULUS(INFO)
         "Used for allocating new elements. "
         "If the type you're creating has	a producer, "
         "you need to execute the verb in a matching producer, "
         "or that producer will be created automatically for you, if possible";

      /// Check if the verb is available in a type, and with given arguments  
      ///   @return true if verb is available in T with arguments A...        
      /*template<CT::NotVoid T, CT::NotVoid... A>
      static constexpr bool AvailableFor() noexcept {
         if constexpr (sizeof...(A) == 0)
            return requires (T & t, Verb & v) { t.Create(v); };
         else
            return requires (T & t, Verb & v, A... a) { t.Create(v, a...); };
      }*/

      /// Get the verb functor for the given type and arguments               
      ///   @return the function, or nullptr if not available                 
      /*template<CT::NotVoid T, CT::NotVoid... A>
      static constexpr auto Of() noexcept {
         if constexpr (!Create::AvailableFor<T, A...>()) {
            return nullptr;
         }
         else if constexpr (CT::Constant<T>) {
            return [](const void* context, Flow::Verb& verb, A... args) {
               auto typedContext = static_cast<const T*>(context);
               typedContext->Create(verb, args...);
            };
         }
         else {
            return [](void* context, Flow::Verb& verb, A... args) {
               auto typedContext = static_cast<T*>(context);
               typedContext->Create(verb, args...);
            };
         }
      }*/

      /*template<CT::NotVoid T>
      static bool ExecuteIn(T&, Verb&);*/

      static bool ExecuteDefault(const Annies::Block<>&, Verb&) {
         return true;
      }

      static bool ExecuteDefault(Annies::Block<>&, Verb&) {
         return false;
      }

      static bool ExecuteStateless(Verb&) {
         return false;
      }
   };

}

struct ImplicitlyReflectedData {
   LANGULUS(POD) true;
   LANGULUS(FILES) "ASE";

   enum Named {One, Two, Three};
   LANGULUS_NAMED_VALUES(One, Two, Three);
   LANGULUS(TYPED) Named;

   Named v = One;

   inline bool operator == (const ImplicitlyReflectedData&) const noexcept = default;
};

class alignas(128) ImplicitlyReflectedDataWithTraits : public ImplicitlyReflectedData {
public:
   int member {664};
   RTTI::Tag<bool, Traits::Name> anotherMember {};
   int anotherMemberArray [12] {};
   int* sparseMember {};

   inline operator int() const noexcept {
      return member;
   }

   void Create(Flow::Verb&) const {
      //++member;
   }

   void Create(Flow::Verb&) {
      ++member;
   }

   ImplicitlyReflectedDataWithTraits() = default;
   explicit ImplicitlyReflectedDataWithTraits(Pi)
      : member {314} {}

   LANGULUS(NAME) "MyType";
   LANGULUS(INFO) "Info about MyType";
   LANGULUS(FILES) "txt, pdf";
   LANGULUS(VERSION_MAJOR) 2;
   LANGULUS(VERSION_MINOR) 1;
   LANGULUS(DEEP) true;
   LANGULUS(POD) true;
   LANGULUS(NULLIFIABLE) true;
   LANGULUS(POOL_TACTIC) RTTI::PoolTactic::Size;
   LANGULUS(CONCRETE) ImplicitlyReflectedData;
   LANGULUS(ACT_AS) void;
   LANGULUS(ALLOCATION_PAGE) 250;
   LANGULUS(ABSTRACT) true;
   LANGULUS_BASES(ImplicitlyReflectedData);
   LANGULUS_VERBS(Verbs::Create);
   LANGULUS_CONVERTS_TO(int);
   LANGULUS_CONVERTS_FROM(Pi);
   LANGULUS_NAMED_VALUES();

   using Self = ImplicitlyReflectedDataWithTraits;
   LANGULUS_MEMBERS(
      &Self::member,
      &Self::anotherMember,
      &Self::anotherMemberArray,
      &Self::sparseMember
   );
};

/// Doesn't have implicit copy/move, so it is abandon-makable by explicit move
/// but not abandon-assignable                                                
class alignas(128) Complex {
public:
   int member;
   bool anotherMember {};
   int anotherMemberArray [12] {};
   int* sparseMember {};

   LANGULUS(NAME) "ComplexType";
   LANGULUS(INFO) "Info about ComplexType";
   LANGULUS(VERSION_MAJOR) 2;
   LANGULUS(VERSION_MINOR) 1;
   LANGULUS(POOL_TACTIC) RTTI::PoolTactic::Size;
   LANGULUS(ALLOCATION_PAGE) 250;

   using Self = Complex;
   LANGULUS_MEMBERS(
      &Self::member,
      &Self::anotherMember,
      &Self::anotherMemberArray,
      &Self::sparseMember
   );

   Complex(const Complex& s)
      : member(s.member) {}
   Complex(Complex&& s)
      : member(s.member) {}
   Complex(int stuff)
      : member(stuff) {}

   ~Complex() {
      if (sparseMember)
         delete sparseMember;
   }
};

struct AnotherTypeWithSimilarilyNamedValues {
   enum Named {One = 501, Two, Three};
   LANGULUS_NAMED_VALUES(One, Two, Three);
   LANGULUS(NAME) "YetAnotherNamedType";

   int v = One;

   inline bool operator == (const AnotherTypeWithSimilarilyNamedValues&) const noexcept = default;
};

struct CheckingWhatGetsInherited : ImplicitlyReflectedDataWithTraits {
   LANGULUS(NAME) "CheckingWhatGetsInherited";

   using ImplicitlyReflectedDataWithTraits::ImplicitlyReflectedDataWithTraits;
};

class ContainsComplex {
   Complex mData;
};

/// A complex aggregate type                                                  
struct AggregateTypeComplex {
   int m1, m2, m3, m4;
   bool m5;
   Complex mData;
};

/// A complex aggregate type                                                  
struct AggregateThatCanBeConfusedWithDescriptorMakable {
   DescriptorConstructible mConfusable;
   int m1, m2, m3, m4;
};

class ForcefullyPod {
   LANGULUS(POD) true;
   Complex mData;
};

struct Type {};

struct TypeErasedContainer {
   LANGULUS(TYPED) void;
};

namespace N1 {
   struct Type {};
   struct Create {};
}

namespace N2 {
   struct Type {};
}

namespace N3 {
   struct type {};
}

enum class TypedEnum : int16_t {
   E1, E2, E3
};
