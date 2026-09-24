///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include <Langulus/CT/Contiguous.hpp>
#include <Langulus/CT/Akin.hpp>
#include <Langulus/CT/Typed.hpp>
#include <Langulus/Utils/Tuple.hpp>
#include <Langulus/Utils/Types.hpp>
#include <Langulus/Utils/Pot.hpp>
#include <Langulus/Utils/Values.hpp>
#include <Langulus/HashOf.hpp>


namespace Langulus::CTTI
{
   /// Type traits for CT checks                                              
   template<class T> struct State;
   template<class T> struct Component;
   template<class T> struct Container;
   template<class T> struct Map;
   template<class T> struct Set;
   template<class T> struct Tag;
   template<class T> struct Pair;
   template<class T> struct Handle;
   template<class T> struct Iterator;
}

LANGULUS_CTTI_CONCEPT_DECVQ(State);
LANGULUS_CTTI_CONCEPT_DECVQ(Component);
LANGULUS_CTTI_CONCEPT_DECVQE(Container);
LANGULUS_CTTI_CONCEPT_DECVQ(Map);
LANGULUS_CTTI_CONCEPT_DECVQ(Set);
LANGULUS_CTTI_CONCEPT_DECVQ(Tag);
LANGULUS_CTTI_CONCEPT_DECVQ(Pair);
LANGULUS_CTTI_CONCEPT_DECVQ(Handle);
LANGULUS_CTTI_CONCEPT_DECVQ(Iterator);

namespace Langulus
{
   struct Describe;
}

namespace Langulus::Annies
{
   /// A component ID                                                         
   using Cid = unsigned;

   namespace Component
   {
      /// Used to disable components at compile-time                          
      template<class>
      struct DisabledComponent {
         using CTTI_Component = Yup;
         static constexpr bool SkipThisComponent = true;
      };
   }
   
   /// I don't like how long ::std::conditional_t is to write.                
   /// Also, std::conditional_t must instantiate both paths, which is a big   
   /// design flaw.                                                           
   template<bool CONDITION, class YES, class NO>
   using Tif = typename ::std::conditional_t<CONDITION,
         ::std::type_identity<YES>,
         ::std::type_identity<NO>
      >::type;

   /// Enables COMPONENT only if CONDITION is met.                            
   /// Useful for making container definitions more flexible.                 
   template<bool CONDITION, CT::Component COMPONENT>
   using EnableComponentIf = Tif<CONDITION, COMPONENT, Component::DisabledComponent<COMPONENT>>;
}

namespace Langulus::Annies::Component
{
   enum OwnershipStyle {
      NoOwnership = 0,
      OnCreateAndDestroy = 1,
      OnAssign = 2
   };

   enum IndexingStyle {
      NotIndexed = 0,
      IndexedLinearly = 1,
      IndexedTable = 2
   };

   constexpr unsigned WeakOwnership   = OnAssign;
   constexpr unsigned StrongOwnership = OnCreateAndDestroy | OnAssign;

   /// Get the dimensions of a container                                      
   /// This is usually defined in the count component, and depicts the        
   /// 'horizontal' size of a container. A pair, for example, has two         
   /// dimensions, despite having a count of 1. A fair analogy would be a     
   /// matrix: where a 4x4 matrix will have count of 4 in 4 dimensions.       
   template<class T>
   using Dimensions = typename ShedDeref<T>::Dimensions;

   template<class C1, class...CN>
   concept DimensionMatch = (Same<Dimensions<C1>, Dimensions<CN>> and ...);

   /// A simple request for allocating memory, which includes heap            
   /// byte size, number of reserved elements, and header/footer offsets.     
   struct Request {
      pot_t  mTotalBytes;
      size_t mHeaderBytes;
      size_t mReserved;
   };

   template<CT::Component...CN>
   constexpr size_t CountEnabled = decltype(Discard(Types<CN...>{},
      []<class C> static {
         return requires { C::SkipThisComponent; }; })
      )::Count;
}

namespace Langulus::CT
{
   /// Check if listed types are containers with any kind of Ownership        
   /// component                                                              
   template<class...T>
   concept Owned = Container<T...>
       and ((ShedDeref<T>::Owned != 0) and ...);

   /// Check if listed containers are referenced upon construction/assignment 
   /// and then automatically dereferenced on destruction                     
   template<class...T>
   concept OwnedStrong = Container<T...>
       and (((ShedDeref<T>::Owned & Annies::Component::StrongOwnership) == Annies::Component::StrongOwnership) and ...);
   
   /// Check if listed types are containers with any kind of OwnershipDeep    
   /// component                                                              
   template<class...T>
   concept OwnedDeep = Container<T...>
       and ((ShedDeref<T>::OwnedDeep != 0) and ...);

   /// Check if listed containers are referenced upon construction/assignment 
   /// and then automatically dereferenced on destruction                     
   template<class...T>
   concept OwnedDeepStrong = Container<T...>
       and (((ShedDeref<T>::OwnedDeep & Annies::Component::StrongOwnership) == Annies::Component::StrongOwnership) and ...);

   /// Check if two containers/elements have the same dimensions              
   template<class LHS, class RHS>
   concept CompatibleDimensions =
      (Container<LHS, RHS> and Same<Annies::Component::Dimensions<LHS>, Annies::Component::Dimensions<RHS>>)
        or (Container<LHS> and not Container<RHS> and Same<Annies::Component::Dimensions<LHS>, Values<Annies::Cid(0)>>)
        or (Container<RHS> and not Container<LHS> and Same<Annies::Component::Dimensions<RHS>, Values<Annies::Cid(0)>>);

   /// Check if listed types are containers with any kind of heap memory      
   template<class...T>
   concept HeapAllocated = Container<T...>
       and ((ShedDeref<T>::CountHeapProviders() > 0) and ...);
   
   /// Check if listed types are containers with more than one heap provider, 
   /// such as maps.                                                          
   template<class...T>
   concept Multiheap = Container<T...>
       and ((ShedDeref<T>::CountHeapProviders() > 1) and ...);
   
   /// Check if listed types are containers with variable count               
   ///   @attention this includes containers with Com::CountStatic, but have  
   ///      nullifiable heap pointer                                          
   template<class...T>
   concept HasVariableCount = HeapAllocated<T...>
       and (ShedDeref<T>::HeapCanBeNull and ...);
   
   /// Check if listed types are containers that can have multiple elements   
   template<class...T>
   concept ContainsMany = Container<T...>
       and (ShedDeref<T>::ContainsMany and ...);
   
   /// Check if listed types are containers that can be reallocated           
   template<class...T>
   concept Reallocatable = Container<T...>
       and (ShedDeref<T>::Reallocatable and ...);
   
   /// Check if listed types are containers that can have single element      
   template<class...T>
   concept ContainsOne = Container<T...>
       and ((not ShedDeref<T>::ContainsMany) and ...);
   
   /// Check if listed types are type-erased containers                       
   template<class...T>
   concept TypeErased = Container<T...>
      and ((ShedDeref<T>::TypeErased) and ...);
   
   /// Check if listed types are tag-erased containers                        
   template<class...T>
   concept TagErased = Container<T...>
      and ((ShedDeref<T>::TagErased) and ...);
   
   /// Check if listed types are verb-erased containers                       
   template<class...T>
   concept VerbErased = Container<T...>
      and ((ShedDeref<T>::VerbErased) and ...);

   /// Check if listed types are containers, and are indexed                  
   template<class...T>
   concept Indexed = Container<T...>
       and ((ShedDeref<T>::Indexed != Annies::Component::NotIndexed) and ...);

   /// Check if listed types are containers, and are linearly indexed         
   template<class...T>
   concept IndexedLinearly = Container<T...>
      and ((ShedDeref<T>::Indexed == Annies::Component::IndexedLinearly) and ...);

   /// Check if listed types are containers, and are hash table indexed       
   template<class...T>
   concept IndexedTable = Container<T...>
      and ((ShedDeref<T>::Indexed == Annies::Component::IndexedTable) and ...);

   /// Check if listed types are containers, and are linearly indexed         
   template<class T>
   concept HeapEntry = requires {
      {T::Id} -> Same<unsigned>;
      requires Sparse<typename T::T>;
   };
}

namespace Langulus::Annies
{
   /// Each state can be either dynamically set at runtime, be statically     
   /// enabled, or statically disabled. Only the 'Variable' value reserves    
   /// a single bit inside the state component representation.                
   enum class StateValue {
      Variable = 0,
      Enabled  = 1,
      Disabled = 2
   };

   constexpr auto StateValueIf(bool condition) -> StateValue {
      return condition ? StateValue::Enabled : StateValue::Variable;
   }

   /// A helper structure for pairing heap components with type components    
   ///   @tparam ID the type component ID                                     
   ///   @tparam POINTER_TYPE associated heap pointer type - mainly a         
   ///      customization point for packed pointer use.                       
   template<Cid ID = 0, CT::Sparse POINTER_TYPE = void*>
   struct HeapEntry {
      using CTTI_ReflectAs = void;
      static constexpr Cid Id = ID;
      using T = POINTER_TYPE;
   };

   /// Unique IDs for state components.                                       
   /// These are used to interface with the outside world, but the true       
   /// values of the states are determined on per-container basis, depending  
   /// on the enabled components.                                             
   enum class State {
      Invalid     = 0,
      Compressed  = 1,
      Encrypted   = 2,
      Future      = 4,
      Past        = 8,
      Or          = 16,
      Sorted      = 32,
      Tracked     = 64,
      Typed       = 128,
      Tagged      = 256,
      Verbed      = 512,
      Disowned    = 1024,
      Pinned      = 2048
   };

   namespace Component
   {
      /// Check if a type definition depicts a modifier                       
      template<class T>
      concept IsRequestModifier = requires { T::AllocatedPerDimension;
                                             T::AllocatedPerElement;
                                             T::AllocatedPerIndirection; };

      /// Header requests can't be per-element or per-indirection             
      template<class T>
      concept IsHeaderRequest = not IsRequestModifier<T> or (IsRequestModifier<T>
          and not T::AllocatedPerElement
          and not T::AllocatedPerIndirection);

      /// Checks for either IsLocalFooterRequest or IsGlobalFooterRequest     
      template<class T>
      concept IsFooterRequest = IsRequestModifier<T>
          and (T::AllocatedPerElement or T::AllocatedPerIndirection);

      /// Local footers are placed immediately after data, so they are easily 
      /// transferrable by a single pointer when absorbing containers.        
      template<class T>
      concept IsLocalFooterRequest = IsFooterRequest<T>
          and T::AllocatedPerDimension;

      /// Global footers can't be per-dimension and are placed at the literal 
      /// end of the allocated memory, after tha last dimension's data and    
      /// local footer.                                                       
      template<class T>
      concept IsGlobalFooterRequest = IsFooterRequest<T>
          and not T::AllocatedPerDimension;

      /// Helper for propagating modifiers. Undefined after no longer used.   
      #define propagate_modifier(a) \
         static constexpr bool a = [] { \
            if constexpr (IsRequestModifier<T>) return T::a; \
            else return false; \
         }()

      /// Makes requests take up more space, based on dimension count.        
      ///   @attention Requests with this modifier are positioned after       
      ///      elements __of the specific dimension__ in order to avoid moving
      ///      elements when heap is resized. Data remains relative to the    
      ///      heap origin for the particular dimension.                      
      ///   @attention If there are no PerElement or PerIndirection modifiers,
      ///      the data will be kept in the header, as number if dimensions   
      ///      is known at compile-time.                                      
      template<class T>
      struct PerDimension {
         using CTTI_ReflectAs = void;
         using CTTI_Typed     = Tif<IsRequestModifier<T>, TypeOf<T>, T>;

         static constexpr bool AllocatedPerDimension = true;
         propagate_modifier(AllocatedPerElement);
         propagate_modifier(AllocatedPerIndirection);
      };

      /// Makes requests take up more space, based on element count.          
      ///   @attention Requests with this modifier are positioned after       
      ///      elements __of the specific dimension__ in order to avoid moving
      ///      elements when heap is resized. Data remains relative to the    
      ///      heap origin for the particular dimension.                      
      template<class T>
      struct PerElement {
         using CTTI_ReflectAs = void;
         using CTTI_Typed     = Tif<IsRequestModifier<T>, TypeOf<T>, T>;

         static_assert(not [] {
            if constexpr (IsRequestModifier<T>) return T::AllocatedPerDimension;
            else return false; } (),
            "The proper order of these modifiers is: "
            "PerDimension<PerElement<PerIndirection>>"
         );

         static constexpr bool AllocatedPerDimension = false;
         static constexpr bool AllocatedPerElement   = true;
         propagate_modifier(AllocatedPerIndirection);
      };

      /// Makes requests take up more space, based on type indirections.      
      ///   @attention Requests with this modifier are positioned after       
      ///      elements __of the specific dimension__ in order to avoid moving
      ///      elements when heap is resized. Data remains relative to the    
      ///      heap origin for the particular dimension.                      
      template<class T>
      struct PerIndirection {
         using CTTI_ReflectAs = void;
         using CTTI_Typed     = Tif<IsRequestModifier<T>, TypeOf<T>, T>;

         static_assert(not [] {
            if constexpr (IsRequestModifier<T>) return T::AllocatedPerDimension
                                                    or T::AllocatedPerElement;
            else return false; } (),
            "The proper order of these modifiers is: "
            "PerDimension<PerElement<PerIndirection>>"
         );

         static constexpr bool AllocatedPerDimension   = false;
         static constexpr bool AllocatedPerElement     = false;
         static constexpr bool AllocatedPerIndirection = true;
      };

      #undef propagate_modifier



      ///                                                                     
      ///   COMPONENT CATALOGUE                                               
      ///                                                                     
      
      /// Type providers                                                      
      template<class META, class TYPE = void, bool CONSTRAIN = not ::std::is_void_v<TYPE>, Cid = 0> struct TypedStack;
      #define LglsComTypedStack(modifier) \
         template<class, class, bool, Cid> modifier struct TypedStack

      template<class META, CT::NotVoid TYPE, Cid = 0> struct TypedStatic;
      #define LglsComTypedStatic(modifier) \
         template<class, CT::NotVoid, Cid> modifier struct TypedStatic

      /// Tag providers                                                       
      template<class META, class TYPE = void, bool CONSTRAIN = not ::std::is_void_v<TYPE>, Cid = 0> struct TaggedStack;
      #define LglsComTaggedStack(modifier) \
         template<class, class, bool, Cid> modifier struct TaggedStack

      template<class META, CT::NotVoid TYPE, Cid = 0> struct TaggedStatic;
      #define LglsComTaggedStatic(modifier) \
         template<class, CT::NotVoid, Cid> modifier struct TaggedStatic

      /// Verb providers                                                      
      template<class META, class TYPE = void, bool CONSTRAIN = not ::std::is_void_v<TYPE>, Cid = 0> struct VerbedStack;
      #define LglsComVerbedStack(modifier) \
         template<class, class, bool, Cid> modifier struct VerbedStack

      template<class META, CT::NotVoid TYPE, Cid = 0> struct VerbedStatic;
      #define LglsComVerbedStatic(modifier) \
         template<class, CT::NotVoid, Cid> modifier struct VerbedStatic

      /// Data providers                                                      
      template<CT::HeapEntry = HeapEntry<0, void*>, CT::HeapEntry...> struct HeapReference;
      #define LglsComHeapReference(modifier) \
         template<CT::HeapEntry, CT::HeapEntry...> modifier struct HeapReference

      template<unsigned = 0, unsigned = 0, CT::HeapEntry = HeapEntry<0, void*>, CT::HeapEntry...> struct HeapImmovable;
      #define LglsComHeapImmovable(modifier) \
         template<unsigned, unsigned, CT::HeapEntry, CT::HeapEntry...> modifier struct HeapImmovable

      template<unsigned = 0, unsigned = 0, CT::HeapEntry = HeapEntry<0, void*>, CT::HeapEntry...> struct HeapMovable;
      #define LglsComHeapMovable(modifier) \
         template<unsigned, unsigned, CT::HeapEntry, CT::HeapEntry...> modifier struct HeapMovable

      template<CT::NotVoid, Cid = 0> struct Stack;
      #define LglsComStack(modifier) \
         template<CT::NotVoid, Cid> modifier struct Stack

      /// Count, reserve, dimensions                                          
      template<class T = size_t, Cid = 0, Cid...> struct CountHeap;
      #define LglsComCountHeap(modifier) \
         template<class, Cid, Cid...> modifier struct CountHeap

      template<class T = size_t, Cid = 0, Cid...> struct CountStack;
      #define LglsComCountStack(modifier) \
         template<class, Cid, Cid...> modifier struct CountStack

      template<auto COUNT = 0u, Cid = 0, Cid...> struct CountStatic;
      #define LglsComCountStatic(modifier) \
         template<auto, Cid, Cid...> modifier struct CountStatic

      template<class T = size_t, Cid = 0, Cid...> struct ReserveEmergent;
      #define LglsComReserveEmergent(modifier) \
         template<class, Cid, Cid...> modifier struct ReserveEmergent

      template<class T = size_t, Cid = 0, Cid...> struct ReserveStack;
      #define LglsComReserveStack(modifier) \
         template<class, Cid, Cid...> modifier struct ReserveStack

      template<auto COUNT = 0u, Cid = 0, Cid...> struct ReserveStatic;
      #define LglsComReserveStatic(modifier) \
         template<auto, Cid, Cid...> modifier struct ReserveStatic
      
      /// Ownership                                                           
      template<unsigned = StrongOwnership, Cid = 0, Cid...> struct OwnershipEmergent;
      #define LglsComOwnershipEmergent(modifier) \
         template<unsigned, Cid, Cid...> modifier struct OwnershipEmergent

      template<unsigned = StrongOwnership, Cid = 0, Cid...> struct OwnershipStack;
      #define LglsComOwnershipStack(modifier) \
         template<unsigned, Cid, Cid...> modifier struct OwnershipStack

      template<unsigned = StrongOwnership, bool REF_INDIVIDUAL = true, Cid = 0, Cid...> struct OwnershipDeepReference;
      #define LglsComOwnershipDeepReference(modifier) \
         template<unsigned, bool, Cid, Cid...> modifier struct OwnershipDeepReference

      template<unsigned = StrongOwnership, bool REF_INDIVIDUAL = true, Cid = 0, Cid...> struct OwnershipDeepEmergent;
      #define LglsComOwnershipDeepEmergent(modifier) \
         template<unsigned, bool, Cid, Cid...> modifier struct OwnershipDeepEmergent

      template<unsigned = StrongOwnership, bool REF_INDIVIDUAL = true, Cid = 0, Cid...> struct OwnershipDeepHeap;
      #define LglsComOwnershipDeepHeap(modifier) \
         template<unsigned, bool, Cid, Cid...> modifier struct OwnershipDeepHeap

      /// Hashing                                                             
      template<Cid = 0, class H = Hash, Cid...> struct HashEmergent;
      #define LglsComHashEmergent(modifier) \
         template<Cid, class, Cid...> modifier struct HashEmergent

      template<Cid = 0, class H = Hash, Cid...> struct HashHeap;
      #define LglsComHashHeap(modifier) \
         template<Cid, class, Cid...> modifier struct HashHeap

      template<Cid = 0, class H = Hash, Cid...> struct HashStack;
      #define LglsComHashStack(modifier) \
         template<Cid, class, Cid...> modifier struct HashStack

      /// Indexing                                                            
      template<Cid = 0, Cid...> struct IndexedCommon;
      #define LglsComIndexedCommon(modifier) \
         template<Cid, Cid...> modifier struct IndexedCommon

      template<Cid = 0, Cid...> struct IndexedLinear;
      #define LglsComIndexedLinear(modifier) \
         template<Cid, Cid...> modifier struct IndexedLinear

      template<Cid = 0, class H = Hash, Cid...> struct IndexedCommonHashed;
      #define LglsComIndexedCommonHashed(modifier) \
         template<Cid, class, Cid...> modifier struct IndexedCommonHashed

      template<Cid = 0, class H = Hash, Cid...> struct IndexedHashHeap;
      #define LglsComIndexedHashHeap(modifier) \
         template<Cid, class, Cid...> modifier struct IndexedHashHeap

      template<Cid = 0, class H = Hash, Cid...> struct IndexedHashStack;
      #define LglsComIndexedHashStack(modifier) \
         template<Cid, class, Cid...> modifier struct IndexedHashStack

      /// Iteration                                                           
      template<Cid = 0, Cid...> struct IterationForEach;
      #define LglsComIterationForEach(modifier) \
         template<Cid, Cid...> modifier struct IterationForEach

      template<Cid = 0, Cid...> struct IterationRange;
      #define LglsComIterationRange(modifier) \
         template<Cid, Cid...> modifier struct IterationRange

      template<Cid = 0, Cid...> struct IterationOperators;
      #define LglsComIterationOperators(modifier) \
         template<Cid, Cid...> modifier struct IterationOperators

      /// States                                                              
      /*template<CT::State...> struct StateHeap;
      #define LglsComStateHeap(modifier) \
         template<CT::State...> modifier struct StateHeap*/

      template<CT::State...> struct StateStack;
      #define LglsComStateStack(modifier) \
         template<CT::State...> modifier struct StateStack

      /*template<CT::State...> struct StateStatic;
      #define LglsComStateStatic(modifier) \
         template<CT::State...> modifier struct StateStatic*/

      namespace State
      {
         template<StateValue = StateValue::Variable, Cid = 0, Cid...> struct Compressed;
         #define LglsStateCompressed(modifier) \
            template<StateValue, Cid, Cid...> modifier struct State::Compressed

         template<StateValue = StateValue::Variable, Cid = 0, Cid...> struct Encrypted;
         #define LglsStateEncrypted(modifier) \
            template<StateValue, Cid, Cid...> modifier struct State::Encrypted

         template<StateValue = StateValue::Variable, Cid = 0, Cid...> struct Future;
         #define LglsStateFuture(modifier) \
            template<StateValue, Cid, Cid...> modifier struct State::Future

         template<StateValue = StateValue::Variable, Cid = 0, Cid...> struct Or;
         #define LglsStateOr(modifier) \
            template<StateValue, Cid, Cid...> modifier struct State::Or

         template<StateValue = StateValue::Variable, Cid = 0, Cid...> struct Past;
         #define LglsStatePast(modifier) \
            template<StateValue, Cid, Cid...> modifier struct State::Past

         template<StateValue = StateValue::Variable, Cid = 0, Cid...> struct Sorted;
         #define LglsStateSorted(modifier) \
            template<StateValue, Cid, Cid...> modifier struct State::Sorted

         template<StateValue = StateValue::Variable, Cid = 0, Cid...> struct Tracked;
         #define LglsStateTracked(modifier) \
            template<StateValue, Cid, Cid...> modifier struct State::Tracked

         template<StateValue = StateValue::Variable, Cid = 0, Cid...> struct Tagged;
         #define LglsStateTagged(modifier) \
            template<StateValue, Cid, Cid...> modifier struct State::Tagged

         template<StateValue = StateValue::Variable, Cid = 0, Cid...> struct Verbed;
         #define LglsStateVerbed(modifier) \
            template<StateValue, Cid, Cid...> modifier struct State::Verbed

         template<StateValue = StateValue::Variable, Cid = 0, Cid...> struct Typed;
         #define LglsStateTyped(modifier) \
            template<StateValue, Cid, Cid...> modifier struct State::Typed

         template<StateValue = StateValue::Variable, Cid = 0, Cid...> struct Disowned;
         #define LglsStateDisowned(modifier) \
            template<StateValue, Cid, Cid...> modifier struct State::Disowned

         template<StateValue = StateValue::Variable, Cid = 0, Cid...> struct Pinned;
         #define LglsStatePinned(modifier) \
            template<StateValue, Cid, Cid...> modifier struct State::Pinned
      }

      namespace StateInner
      {
         /// Go through all components and accumulate their state requests    
         /// into a StateStack component                                      
         template<CT::Component C1, CT::Component...CN>
         consteval auto DefineStates() {
            if constexpr (requires { typename C1::Subcomponents; }) {
               constexpr auto first = Expand(typename C1::Subcomponents{}, []<class...InnerC1> {
                  return DefineStates<InnerC1...>();
               });
               return first + DefineStates<CN...>();
            }
            else if constexpr (requires { typename C1::StateRequest; }) {
               if constexpr (CT::NotVoid<typename C1::StateRequest>) {
                  static_assert(CT::State<typename C1::StateRequest>);
                  Types<typename C1::StateRequest> first;

                  if constexpr (sizeof...(CN))
                     return first + DefineStates<CN...>();
                  else
                     return first;
               }
               else {
                  if constexpr (sizeof...(CN))
                     return DefineStates<CN...>();
                  else
                     return NoTypes {};
               }
            }
            else {
               if constexpr (sizeof...(CN))
                  return DefineStates<CN...>();
               else
                  return NoTypes {};
            }
         }

         template<CT::State...STATES>
         consteval auto DecideStateComponent(Types<STATES...>) -> StateStack<STATES...>;
      }

      template<CT::Component...COMPONENTS>
      using DecideStateComponent = decltype(
         StateInner::DecideStateComponent(
            decltype(StateInner::DefineStates<COMPONENTS...>()) {}
         )
      );

      /// Comparison, search, pattern matching                                
      template<bool CONVERT = false, bool HASH = true, Cid = 0, Cid...> struct Comparison;
      #define LglsComComparison(modifier) \
         template<bool, bool, Cid, Cid...> modifier struct Comparison

      /// Conversion                                                          
      template<Cid = 0, Cid...> struct Conversion;
      #define LglsComConversion(modifier) \
         template<Cid, Cid...> modifier struct Conversion

      /// Emplacement, insertion, merging, concatenation, removal             
      template<Cid = 0, Cid...> struct Emplacement;
      #define LglsComEmplacement(modifier) \
         template<Cid, Cid...> modifier struct Emplacement

      template<bool CONVERT = false, Cid = 0, Cid...> struct Assignment;
      #define LglsComAssignment(modifier) \
         template<bool, Cid, Cid...> modifier struct Assignment

      template<bool CONVERT = false, Cid = 0, Cid...> struct Insertion;
      #define LglsComInsertion(modifier) \
         template<bool, Cid, Cid...> modifier struct Insertion

      template<Cid = 0, Cid...> struct InsertionOperators;
      #define LglsComInsertionOperators(modifier) \
         template<Cid, Cid...> modifier struct InsertionOperators

      template<Cid = 0, Cid...> struct InsertionOperatorsConcat;
      #define LglsComInsertionOperatorsConcat(modifier) \
         template<Cid, Cid...> modifier struct InsertionOperatorsConcat

      template<bool CONVERT = false, Cid = 0, Cid...> struct Merging;
      #define LglsComMerging(modifier) \
         template<bool, Cid, Cid...> modifier struct Merging

      template<Cid = 0, Cid...> struct MergingOperators;
      #define LglsComMergingOperators(modifier) \
         template<Cid, Cid...> modifier struct MergingOperators

      template<Cid = 0, Cid...> struct Removal;
      #define LglsComRemoval(modifier) \
         template<Cid, Cid...> modifier struct Removal

      /// Other services                                                      
      template<Cid = 0> struct ChargedStack;
      #define LglsComChargedStack(modifier) \
            template<Cid> modifier struct ChargedStack

      template<Cid = 0> struct Extrapolation;
      #define LglsComExtrapolation(modifier) \
            template<Cid> modifier struct Extrapolation

      template<Cid = 0> struct Interpolation;
      #define LglsComInterpolation(modifier) \
         template<Cid> modifier struct Interpolation

      template<class T, Cid = 0, Cid...> struct DictionaryHeap;
      #define LglsComDictionaryHeap(modifier) \
         template<class, Cid, Cid...> modifier struct DictionaryHeap
   }
   
   namespace Com = Component;

   namespace Inner
   {
      /// Validate all used components in a container are properly ordered,   
      /// of standard layout, and containing proper sequential provider IDs.  
      ///   @tparam ACC accumulated stack/heap provider IDs                   
      ///   @tparam PRECEDENCE last valid precedence                          
      ///   @tparam INNER whether we're currently going through subcomponents 
      ///   @tparam C1, CN... components                                      
      template<int ACC, int PRECEDENCE, class C1, class...CN>
      consteval bool ValidateComponentOrderNested() {
         if constexpr (requires { typename C1::Subcomponents; }) {
            return Expand(typename C1::Subcomponents{}, []<class...InnerC1> {
               return ValidateComponentOrderNested<ACC, PRECEDENCE, InnerC1..., CN...>();
            });
         }
         else if constexpr (requires { C1::SkipThisComponent; }) {
            if constexpr (sizeof...(CN) > 0)
               return ValidateComponentOrderNested<ACC, PRECEDENCE, CN...>();
            else {
               static_assert(ACC > 0,
                  "Container must have at least one heap or stack provider");
               return true;
            }
         }
         else {
            static_assert(C1::ComponentPrecedence >= PRECEDENCE,
               "Wrong component order");

            if constexpr (requires { C1::StackProvider; }) {
               static_assert(C1::StackProvider == ACC,
                  "Invalid stack provider ID");
               static_assert(not requires { typename C1::HeapProvider; },
                  "Component can't be both a stack and a heap provider");

               if constexpr (sizeof...(CN) > 0)
                  return ValidateComponentOrderNested<ACC + 1, C1::ComponentPrecedence, CN...>();
               else
                  return true;
            }
            else if constexpr (requires { typename C1::HeapProvider; }) {
               static_assert(C1::HeapProvider::First == ACC,
                  "Invalid heap provider ID");
               static_assert(not requires { C1::StackProvider; },
                  "Component can't be both a stack and a heap provider");

               if constexpr (sizeof...(CN) > 0)
                  return ValidateComponentOrderNested<ACC + C1::HeapProvider::Count, C1::ComponentPrecedence, CN...>();
               else
                  return true;
            }
            else {
               if constexpr (sizeof...(CN) > 0)
                  return ValidateComponentOrderNested<ACC, C1::ComponentPrecedence, CN...>();
               else {
                  static_assert(ACC > 0,
                     "Container must have at least one heap or stack provider");
                  return true;
               }
            }
         }
      }

      /// Validate all used components in a container are properly ordered,   
      /// of standard layout, and containing proper sequential provider IDs.  
      ///   @tparam CN... components                                          
      template<class...CN>
      consteval bool ValidateComponentOrder() {
         static_assert(CT::Component<CN...>,
            "All elements must be components");
         static_assert(((not requires { typename CN::StateList; }) and ...),
            "The state component will be added automatically - remove it, and just "
            "rely on StateRequest(s) in other components.");
         static_assert((::std::is_standard_layout_v<CN> and ...),
            "All components must have standard layouts");
         static_assert((sizeof(CN) * ...) == 1,
            "Use StackRequest instead of adding non-static members to components");
         static_assert(sizeof...(CN) > 0,
            "Composed container must posses at least one heap/stack provider");
         if constexpr (sizeof...(CN) > 0)
            return ValidateComponentOrderNested<0, -1000000, CN...>();
         else
            return false;
      }

      /// std::tuple default-initializes variables to zero, so I use this     
      /// wrapper to get back to the biblically accurate behavior             
      template<class T>
      struct StackVariable {
         T value;

         /// Default initialization shouldn't initialize anything, but also   
         /// completely fail if T is a reference type.                        
         constexpr StackVariable() noexcept requires CT::NotReference<T> {};

         /// Initializing a reference                                         
         constexpr StackVariable(T& v) noexcept requires CT::Reference<T>
            : value {v} {}

         /// Constructs directly if possible                                  
         constexpr StackVariable(auto&& v) noexcept
         requires (    requires { T{LglsFwd(v)}; } and CT::NotReference<T>)
            : value {LglsFwd(v)} {}

         /// Strips intents before constructing in case first attempt         
         /// fails. Useful for primitive types that don't support intents.    
         constexpr StackVariable(auto&& v) noexcept
         requires (not requires { T{LglsFwd(v)}; } and CT::NotReference<T>)
            : value {LglsFwd(DeintCast(v))} {}
      };
      
      /// Go through all components and accumulate their stack requests into  
      /// a type list                                                         
      template<class C1, class...CN>
      consteval auto DefineStack(Types<C1, CN...>&&) {
         if constexpr (requires { typename C1::Subcomponents; })
            return DefineStack(typename C1::Subcomponents{} + Types<CN...>{});
         else if constexpr (requires { typename C1::StackRequest; }) {
            using R = typename C1::StackRequest;
            if constexpr (CT::NotVoid<R>) {
               static_assert(not Com::IsRequestModifier<R>,
                  "Stack requests can't have modifiers");

               Types<StackVariable<R>> first;
               if constexpr (sizeof...(CN))
                  return first + DefineStack(Types<CN...>{});
               else
                  return first;
            }
            else {
               if constexpr (sizeof...(CN))
                  return DefineStack(Types<CN...>{});
               else
                  return NoTypes{};
            }
         }
         else {
            if constexpr (sizeof...(CN))
               return DefineStack(Types<CN...>{});
            else
               return NoTypes {};
         }
      }
   }

   template<class...CN>
   concept ValidComponentOrder = Inner::ValidateComponentOrder<CN...>();
}