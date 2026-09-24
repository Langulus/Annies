///                                                                           
/// Langulus::Annies                                                          
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Component.hpp"
#include "Components/State-Stack.hpp"
#include <Langulus/IntentOf.hpp>
#include <Langulus/Utils/Sequence.hpp>
#include <Langulus/HashOf.hpp>
#include <Langulus/CT/Bool.hpp>
#include <concepts>

/// Make the rest of the code aware, that Langulus::Annies has been included 
#define LANGULUS_LIBRARY_ANYNESS() 1
#define LANGULUS_ANYNESS_VERBOSITY_MASTER_SWITCH() 0

#define if_available(...) if constexpr (requires { __VA_ARGS__; }) { __VA_ARGS__; }

//WORKAROUND G++ (up to version 16 when a last checked) has a notorious bug when       
//WORKAROUND parsing requires { with a relative base specifier inside }. I've pondered 
//WORKAROUND this for too long, and this is the only workaround that was actually able 
//WORKAROUND to do the trick.                                                          
//WORKAROUND   @important: in order for this to work, you might need your              
//WORKAROUND      'deduced this' functions spell out their 'this' argument explicitly  
#define if_available_gcc(...) \
   if constexpr (requires { &__VA_ARGS__; }) self.__VA_ARGS__

namespace Langulus::Annies
{
   /// MARK: Predeclarations                                                  
   namespace Component
   {
      template<CT::Component...C> requires ValidComponentOrder<C...>
      struct LANGULUS_EBCO Container;
   }

   struct Handle;
   struct HandleMut;
   struct HandleDisowned;
   struct HandleDisownedMut;

   #if not LANGULUS(FORCE_TYPE_ERASURE)
      template<class> struct THandle;
      template<class> struct THandleEmergent;
      template<class> struct THandleDisowned;
   #endif
   
   template<CT::Handle, CT::Handle>
   struct THandlePair;

   struct Any;
   struct Bytes;
   struct Construct;
   struct Many;
   struct Neat;
   struct Pair;
   struct Path;
   struct Text;
   struct Tag;

   template<CT::NotVoid> struct TOwn;
   template<class>       struct TRef;
   template<CT::NotVoid> struct TAny;
   template<CT::NotVoid> struct THive;
   template<CT::NotVoid> struct TMany;

   template<CT::NotVoid, StateValue SORT = StateValue::Variable>
   struct TSet;

   template<class, class, StateValue SORT = StateValue::Variable>
   struct TMap;

   template<CT::NotVoid, CT::NotVoid>
   struct TPair;

   namespace Inner
   {
      template<StateValue SORT = StateValue::Variable>
      struct Map;

      template<StateValue SORT = StateValue::Variable>
      struct Set;

      struct DisambiguatorTag {
         using CTTI_Void          = void;
         using CTTI_ReflectAs     = void;
         using CTTI_Disambiguator = Yup;
      };

      /// Tag for calling container constructors that initalize the           
      /// internal stack tuple. Extensively used by handles and iterators.    
      struct Stackwise : DisambiguatorTag {};

      /// Tag for calling container constructors that emplace elements.       
      /// Often used to disambiguate and state clear intent.                  
      struct Piecewise : DisambiguatorTag {};

      /// Tag for calling container constructors that absorb container.       
      /// Often used to disambiguate and state clear intent.                  
      struct Absorb : DisambiguatorTag {};

      /// Tag for calling container constructors that slice containers.       
      /// Often used to disambiguate and state clear intent.                  
      template<Cid>
      struct Slice : DisambiguatorTag {};

      /// MARK: DecideHandleType                                              
      /// Inner function that picks the best possible handle type, depending  
      /// on a container's constness and type-erasedness, as well as member   
      /// types HandleType and HandleMutType. Guarantees to always result in  
      /// a handle. No-op if C is already a handle.                           
      ///   @attention multidimensional containers should result in multi-    
      ///      dimensional handles                                            
      template<CT::Container C> 
      consteval auto DecideHandleType() {
         static_assert(not CT::Sheddable<C>, "Strip sheddables first");
         static_assert(not CT::Reference<C>, "Strip references first");

         if constexpr (CT::Handle<C>) {
            // No-op                                                    
            return Types<C> {};
         }
         else if constexpr (requires {typename C::HandleType; typename C::HandleMutType; }) {
            // Always prioritize custom handle types if defined         
            using mutbl = typename C::HandleMutType;
            using immut = typename C::HandleType;
            static_assert(C::Dimensions::Count == mutbl::Dimensions::Count, 
               "Custom HandleMutType doesn't represent container dimensions properly!"
            );
            static_assert(C::Dimensions::Count == immut::Dimensions::Count, 
               "Custom HandleType doesn't represent container dimensions properly!"
            );
            return Types<Tmut<C, mutbl, immut>> {};
         }
         else IF_NOT_LANGULUS_FORCE_TYPE_ERASURE(if constexpr (CT::TypeErased<C>)) {
            // Type-erased handle                                       
            static_assert(C::Dimensions::Count == 1, 
               "Multidimensional containers should result in multidimensional handles! "
               "Define HandleMutType and HandleType!"
            );

            if constexpr (CT::Owned<C>)
               return Types<Tmut<C, HandleMut,         Handle>> {};
            else
               return Types<Tmut<C, HandleDisownedMut, HandleDisowned>> {};
         }
         #if not LANGULUS(FORCE_TYPE_ERASURE)
         else {
            // Statically-typed handle                                  
            static_assert(C::Dimensions::Count == 1, 
               "Multidimensional containers should result in multidimensional handles! "
               "Define HandleMutType and HandleType!"
            );

            using T     = TypeOf<C>;
            using Inner = Tmut<C, T&, ConstAll<T&>>;
            if constexpr (CT::Owned<C>)
               return Types<THandle        <Inner>> {};
            else
               return Types<THandleDisowned<Inner>> {};
         }
         #endif
      }

      /// MARK: DecidePickType                                                
      /// Inner function that picks the best possible handle or reference     
      /// type, depending on a container's constness and type-erasedness, as  
      /// well as member types HandleType and HandleMutType. Unlike           
      /// DecideHandleType, this one will prefer to use references whenever   
      /// possible.                                                           
      template<CT::Container C> 
      consteval auto DecidePickType() {
         static_assert(not CT::Sheddable<C>, "Strip sheddables first");
         static_assert(not CT::Reference<C>, "Strip references first");

         if constexpr (requires {typename C::Pick; typename C::PickMut; }) {
            // Always prioritize custom pick types if defined           
            return Types<Tmut<C, typename C::PickMut, typename C::Pick>> {};
         }
         else IF_NOT_LANGULUS_FORCE_TYPE_ERASURE(if constexpr (CT::TypeErased<C>)) {
            // Type-erased containers always result in handle picks     
            return DecideHandleType<C>();
         }
         #if not LANGULUS(FORCE_TYPE_ERASURE)
         else {
            // Statically-typed container - always prefer a reference,  
            // unless we're referencing an owned pointer                
            using T = TypeOf<C>;
            if constexpr (CT::Owned<C> and CT::Sparse<T> and CT::Mutable<C>)
               return DecideHandleType<C>();
            else
               return Types<Tmut<C, T&, ConstAll<T&>>> {};
         }
         #endif
      }

      /// MARK: CoalesceComponents                                            
      template<CT::Typelist OLD, CT::Component NEW1, CT::Component...NEWN>
      consteval auto CoalesceComponentsInner() {
         auto lower_precedence = Extract(OLD{}, []<class O> static {
            if constexpr (O::ComponentPrecedence < NEW1::ComponentPrecedence)
               return Types<O>{};
            else if constexpr (O::ComponentPrecedence > NEW1::ComponentPrecedence)
               return NoTypes{};
            else {
               static_assert(not Same<O, NEW1>,
                  "Duplicated component");
               static_assert(O::Id::template Intersect<typename NEW1::Id>::Empty,
                  "Overlapping ids");
               return Types<O>{};
            }
         });
         auto higher_precedence = Extract(OLD{}, []<class O> static {
            if constexpr (O::ComponentPrecedence > NEW1::ComponentPrecedence)
               return Types<O>{};
            else
               return NoTypes{};
         });

         auto result = lower_precedence + Types<NEW1>{} + higher_precedence;
         if constexpr (sizeof...(NEWN) == 0)
            return result;
         else
            return CoalesceComponentsInner<decltype(result), NEWN...>();
      }

      /// Inserts new components at the proper place, considering precedence  
      /// and Ids. Produces a new Container<OLD + NEW>.                       
      template<class OLD, CT::Component...NEW>
      consteval auto CoalesceComponents() {
         return Expand(CoalesceComponentsInner<OLD, NEW...>(), []<class...C> {
            return ::std::type_identity<Component::Container<C...>> {};
         });
      }
   }

   constexpr Inner::Stackwise Stackwise {};
   constexpr Inner::Piecewise Piecewise {};
   constexpr Inner::Absorb    Absorb {};
   template<Cid ID>
   constexpr auto Slice = Inner::Slice<ID>{};
   
   template<CT::Container C>
   using DecideHandle = typename decltype(Inner::DecideHandleType<Deref<C>>())::First;

   template<CT::Container C>
   using DecidePick = typename decltype(Inner::DecidePickType<Deref<C>>())::First;

   template<class...T>
   concept Disambiguate = ((not requires { typename Decay<T>::CTTI_Disambiguator; }) and ...);

   
   namespace Component
   {
   /// MARK: Container                                                        
   ///                                                                        
   /// A container definition using composition                               
   ///   @tparam COMPONENTS list of components that define the container      
   ///      behavior. Order is verified based on ComponentPrecedence members  
   ///      for various reasons, the main ones being initialization order and 
   ///      build-time optimization: too many superficially different template
   ///      specializations will bloat code generation significantly and slow 
   ///      builds down a lot...                                              
   template<CT::Component...COMPONENTS> requires ValidComponentOrder<COMPONENTS...>
   struct LANGULUS_EBCO Container : DecideStateComponent<COMPONENTS...>, COMPONENTS... {
      using CTTI_Container = Yup;
      using StateComponent = DecideStateComponent<COMPONENTS...>;
      using ComponentList  = Types<StateComponent, COMPONENTS...>;
      using Base           = Container;

      /// Generate a new container type with additional components            
      template<CT::Component...MORE>
      using Include = typename decltype(Annies::Inner::CoalesceComponents<Types<COMPONENTS...>, MORE...>())::type;

      /// Get the number of heap providers (all dimensions)                   
      /// Needs to be public, because it's used in concept checks.            
      template<CT::Typelist L = ComponentList>
      static consteval size_t CountHeapProviders() {
         size_t count = 0;
         ForEach(L{}, [&count]<class C> {
            if constexpr (requires { typename C::Subcomponents; })
               count += CountHeapProviders<typename C::Subcomponents>();
            else if constexpr (requires { typename C::HeapProvider; })
               ++count;
         });
         return count;
      }

   protected:
      LglsComIterationOperators(friend);
      LglsComTypedStack(friend);
      LglsComVerbedStack(friend);
      LglsComTaggedStack(friend);
      LglsComStack(friend);
      LglsComHeapReference(friend);
      LglsComHeapMovable(friend);
      LglsComOwnershipEmergent(friend);
      LglsComOwnershipStack(friend);
      LglsComCountHeap(friend);
      LglsComCountStack(friend);
      LglsComCountStatic(friend);
      LglsComReserveStack(friend);
      LglsComOwnershipDeepHeap(friend);
      LglsComOwnershipDeepReference(friend);
      LglsComOwnershipDeepEmergent(friend);
      LglsComHashStack(friend);
      LglsComHashHeap(friend);
      LglsComHashEmergent(friend);
      LglsComDictionaryHeap(friend);
      LglsComComparison(friend);
      LglsComEmplacement(friend);
      LglsComAssignment(friend);
      LglsComInsertion(friend);
      LglsComMerging(friend);
      LglsComConversion(friend);
      LglsComReserveEmergent(friend);
      LglsComIndexedHashHeap(friend);
      LglsComIndexedHashStack(friend);
      LglsComStateStack(friend);
      LglsComIndexedCommon(friend);
      LglsComIndexedCommonHashed(friend);
      LglsComIndexedLinear(friend);
      LglsComRemoval(friend);
      LglsComIterationOperators(friend);
      LglsComChargedStack(friend);

      // Here lies the stack. It is an optimized tuple that is filled   
      // with StackRequest(s) from components.                          
      compact_tuple_from_typelist<decltype(Inner::DefineStack(ComponentList{}))> mStack;
      
      /// Default constructor doesn't initialize anything.                    
      /// Your container needs to call ConstructDefault manually.             
      constexpr Container() noexcept = default;

      /// A tag-dispatch constructor that forwards arguments to mStack.       
      /// Used in some niche container cases, like TOwn.                      
      explicit constexpr Container(Inner::Stackwise, auto&&...arguments)
         : mStack({LglsFwd(arguments)}...) {}

      /// Default destructor does nothing. Each container has to implement    
      /// it, most likely by calling this->Destroy(). This is needed, because 
      /// the destructor relies on properly deducing 'this'.                  
      constexpr ~Container() noexcept = default;
      
      /// MARK: CountHeapRequests                                             
      /// Get the total number of heap requests (all dimensions)              
      template<CT::Typelist L = ComponentList>
      static consteval size_t CountHeapRequests() {
         size_t count = 0;
         ForEach(L{}, [&count]<class C> {
            if constexpr (requires { typename C::Subcomponents; })
               count += CountHeapRequests<typename C::Subcomponents>();
            else if constexpr (requires { typename C::HeapRequest; }) {
               if constexpr (CT::NotVoid<typename C::HeapRequest>)
                  ++count;
            }
         });
         return count;
      }

      /// MARK: CountHeapFooterRequests                                       
      /// Get the number of heap requests in the footer for chosen heap ID    
      template<Cid SID, CT::Typelist L = ComponentList>
      static consteval size_t CountHeapFooterRequests() {
         size_t count = 0;
         ForEach(L{}, [&count]<class C> {
            if constexpr (requires { typename C::Subcomponents; })
               count += CountHeapFooterRequests<SID, typename C::Subcomponents>();
            else if constexpr (requires { typename C::HeapRequest; }) {
               if constexpr (C::Id::template Contains<SID>
               and IsFooterRequest<typename C::HeapRequest>)
                  ++count;
            }
         });
         return count;
      }

      /// MARK: GetStackOffset                                                
      /// Go through all components until PICK is reached, and accumulate     
      /// the offset up to that point, to get the index in the stack tuple.   
      template<class PICK, CT::Typelist L>
      static constexpr auto GetStackOffsetInner(size_t& offset) {
         return ForEachConstOr(L{}, [&offset]<class C> {
            if constexpr (requires { typename C::Subcomponents; })
               return GetStackOffsetInner<PICK, typename C::Subcomponents>(offset);
            else if constexpr (CT::DerivedFrom<C, PICK>)
               return true;
            else if constexpr (requires { typename C::StackRequest; }) {
               if constexpr (CT::NotVoid<typename C::StackRequest>)
                  ++offset;
               return No {};
            }
            else return No {};
         });
      }

      template<class PICK>
      static consteval size_t GetStackOffset() {
         static_assert(requires { typename PICK::StackRequest; },
            "Component data is not on the stack");

         size_t offset = 0;
         GetStackOffsetInner<PICK, ComponentList>(offset);
         return offset;
      }
      
      /// MARK: FindProvider                                                  
      /// Get the type of heap/stack provider for a given ID                  
      ///   @tparam ID provider ID                                            
      ///   @return return type of the provider                               
      template<Cid SID, CT::Typelist L = ComponentList>
      static consteval auto FindProvider() {
         return ForEachConstOr(L{}, []<class C> {
            if constexpr (requires { typename C::Subcomponents; })
               return FindProvider<SID, typename C::Subcomponents>();
            else if constexpr (requires { C::StackProvider; }) {
               if constexpr (C::StackProvider == SID)
                  return Types<C> {};
               else
                  return No {};
            }
            else if constexpr (requires { typename C::HeapProvider; }) {
               if constexpr (C::HeapProvider::template Contains<SID>)
                  return Types<C> {};
               else
                  return No {};
            }
            else return No {};
         });
      }
      
      /// Get the set of providers for a list of IDs                          
      ///   @param ids provider IDs                                           
      ///   @return return the set of the providers, no duplicates            
      template<CT::Typelist L = ComponentList, CT::Typelist CARRY = Types<>, Cid ID1, Cid...IDN>
      static consteval auto FindProviders(Values<ID1, IDN...>&&) {
         constexpr auto p1 = FindProvider<ID1, L>();
         if constexpr (CARRY::template Contains<typename decltype(p1)::First>) {
            if constexpr (sizeof...(IDN))
               return FindProviders<L, CARRY>(Values<IDN...>{});
            else
               return CARRY {};
         }
         else {
            if constexpr (sizeof...(IDN))
               return FindProviders<L, decltype(CARRY{} + p1)>(Values<IDN...>{});
            else
               return CARRY{} + p1;
         }
      }

      /// MARK: DefineHeapHeader                                              
      /// Go through all relevant components for the dimension 'SID' and      
      /// accumulate their header heap requests into a byte amount.           
      ///   @return The size of the heap header for the dimension, in bytes.  
      ///      The header bytes are located relative to:                      
      ///         GetAllocation<SID>()->GetBlockStart()                       
      ///   @attention the resulting bytesize needs to be aligned to the      
      ///      alignment of the first type of the provider!                   
      template<Cid SID, CT::Typelist L = ComponentList>
      static consteval size_t DefineHeapHeader() {
         using PROVIDER = typename decltype(FindProvider<SID>())::First;
         size_t bytesize = 0;
         ForEach(L{}, [&bytesize]<class C> {
            if constexpr (requires { typename C::Subcomponents; })
               bytesize += DefineHeapHeader<SID, typename C::Subcomponents>();
            else if constexpr (requires { typename C::HeapRequest; }) {
               if constexpr (C::Id::template Contains<SID>) {
                  using R = typename C::HeapRequest;
                  if constexpr (IsRequestModifier<R>) {
                     if constexpr (R::AllocatedPerDimension and IsHeaderRequest<R>) {
                        // Multiply only by the number of dimensions    
                        // that are shared with the relevant provider.  
                        using INTERSECT = typename C::Id::template Intersect<typename PROVIDER::Id>;
                        bytesize += sizeof(TypeOf<R>) * INTERSECT::Count;
                     }
                  }
                  else bytesize += sizeof(R);
               }
            }
         });
         return bytesize;
      }      
      
      /// Go through all components until PICK is reached, and accumulate     
      /// the offset up to that point, to get the byte offset in the header   
      /// for the particular dimension 'SID'.                                 
      ///   @return The header offset, where PICK's data resides. The offset  
      ///      is relative to GetAllocation<SID>()->GetBlockStart()           
      template<class PICK, Cid SID, CT::Typelist L>
      static consteval auto GetHeapHeaderOffsetInner(size_t& offset) {
         return ForEachConstOr(L{}, [&offset]<class C> {
            if constexpr (requires { typename C::Subcomponents; })
               return GetHeapHeaderOffsetInner<PICK, SID, typename C::Subcomponents>(offset);
            else if constexpr (CT::DerivedFrom<C, PICK>) {
               // Target component reached, but there might be          
               // dimensional offset to consider.                       
               using R = typename C::HeapRequest;
               if constexpr (IsRequestModifier<R>) {
                  if constexpr(R::AllocatedPerDimension) {
                     using PROVIDER = typename decltype(FindProvider<SID>())::First;
                     using INTERSECT = C::Id::template Intersect<typename PROVIDER::Id>;
                     INTERSECT::ForEachConstOr([&offset]<Cid D> {
                        if constexpr (D == SID)
                           return true;
                        else {
                           offset += sizeof(TypeOf<R>);
                           return No {};
                        }
                     });
                  }
               }
               return true;
            }
            else if constexpr (requires { typename C::HeapRequest; }) {
               if constexpr (C::Id::template Contains<SID>) {
                  using R = typename C::HeapRequest;
                  if constexpr (IsRequestModifier<R>) {
                     if constexpr (R::AllocatedPerDimension and IsHeaderRequest<R>) {
                        // Multiply only by the number of dimensions    
                        // that are shared with the relevant provider.  
                        using PROVIDER = typename decltype(FindProvider<SID>())::First;
                        using INTERSECT = C::Id::template Intersect<typename PROVIDER::Id>;
                        offset += sizeof(TypeOf<R>) * INTERSECT::Count;
                     }
                  }
                  else offset += sizeof(R);
               }
               return No {};
            }
            else return No {};
         });
      }

      template<class PICK, Cid SID>
      static consteval size_t GetHeapHeaderOffset() {
         static_assert(requires { typename PICK::HeapRequest; },
            "Component data is not on the heap");
         static_assert(PICK::Id::template Contains<SID>,
            "The PICK must share the provided ID");

         using PICK_R = typename PICK::HeapRequest;
         static_assert(IsHeaderRequest<PICK_R>, "Not a header request, "
            "use GetHeapFooterOffset/GetHeapFooterOffsetGlobal instead"
         );
         
         size_t offset = 0;
         GetHeapHeaderOffsetInner<PICK, SID, ComponentList>(offset);
         return offset;
      }

      /// MARK: DefineHeapFooter                                              
      /// Go through all components relevant to the provided dimension SID    
      /// that have PerDimension modifier, and accumulate their heap          
      /// requests into a byte amount.                                        
      ///   @param count Footer can depend on a new reserved amount           
      ///   @param indirects Footer can depend on number of indirections      
      ///   @return The size of the heap footer for chosen dimension in bytes.
      ///      The footer starts at GetRawReserveEnd<SID>().                  
      template<Cid SID, CT::Typelist L = ComponentList>
      static constexpr size_t DefineHeapFooter(
         [[maybe_unused]] const size_t count,
         [[maybe_unused]] const size_t indirects
      ) noexcept {
         size_t bytesize = 0;
         ForEach(L{}, [&bytesize, &indirects, &count]<class C> {
            if constexpr (requires { typename C::Subcomponents; })
               bytesize += DefineHeapFooter<SID, typename C::Subcomponents>(count, indirects);
            else if constexpr (requires { typename C::HeapRequest; }) {
               using R = typename C::HeapRequest;
               if constexpr (IsLocalFooterRequest<R> and C::Id::template Contains<SID>) {
                  //if constexpr (R::AllocatedPerDimension) {
                     bytesize += sizeof(TypeOf<R>)
                        * (R::AllocatedPerElement     ? count      : 1)
                        * (R::AllocatedPerIndirection ? indirects  : 1);
                  //}
               }
            }
         });
         return bytesize;
      }

      /// Go through all components with PerDimension modifier until PICK is  
      /// reached, and accumulate the offset up to that point, to get the byte
      /// offset in the heap for the particular dimension 'SID'.              
      ///   @param count Footer can depend on a new reserved amount           
      ///   @param indirects Footer can depend on number of indirections      
      ///   @return The heap byte offset, where PICK's data resides. Relative 
      ///      to GetRawReserveEnd<SID>().                                    
      template<class PICK, Cid SID, CT::Typelist L>
      static constexpr auto GetHeapFooterOffsetInner(
         [[maybe_unused]] const size_t count,
         [[maybe_unused]] const size_t indirects,
         size_t& offset
      ) noexcept {
         return ForEachConstOr(L{}, [&offset, &indirects, &count]<class C> {
            if constexpr (requires { typename C::Subcomponents; })
               return GetHeapFooterOffsetInner<PICK, SID, typename C::Subcomponents>(count, indirects, offset);
            else if constexpr (CT::DerivedFrom<C, PICK>)
               return true;
            else if constexpr (requires { typename C::HeapRequest; }) {
               using R = typename C::HeapRequest;
               if constexpr (IsLocalFooterRequest<R> and C::Id::template Contains<SID>) {
                  //if constexpr (R::AllocatedPerDimension) {
                     offset += sizeof(TypeOf<R>)
                        * (R::AllocatedPerElement     ? count     : 1)
                        * (R::AllocatedPerIndirection ? indirects : 1);
                  //}
               }
               return No {};
            }
            else return No {};
         });
      }

      template<class PICK, Cid SID>
      static constexpr size_t GetHeapFooterOffset(
         [[maybe_unused]] const size_t count,
         [[maybe_unused]] const size_t indirects
      ) noexcept {
         static_assert(requires { typename PICK::HeapRequest; },
            "Component data is not on the heap");
         static_assert(PICK::Id::template Contains<SID>,
            "The PICK must share the provided ID");

         using PICK_R = typename PICK::HeapRequest;
         static_assert(IsLocalFooterRequest<PICK_R>,
            "Not a local footer request, "
            "use GetHeapHeaderOffset/GetHeapFooterOffsetGlobal instead"
         );
         //static_assert(PICK_R::AllocatedPerDimension,
         //   "Not a PerDimension request, use GetHeapFooterOffsetGlobal instead");

         size_t offset = 0;
         GetHeapFooterOffsetInner<PICK, SID, ComponentList>(count, indirects, offset);
         return offset;
      }

      /// MARK: DefineHeapFooterGlobal                                        
      /// Go through all components relevant to the provider associated with  
      /// dimension SID that have no PerDimension modifier, and accumulate    
      /// their heap requests into a byte amount.                             
      ///   @param count Footer can depend on a new reserved amount           
      ///   @return The size of the global heap footer for the provider       
      ///      associated with the dimension 'SID'. Given 'D' is the last     
      ///      dimension for that provider, the offset is relative to:        
      ///      GetRawReserveEnd<D>() + DefineHeapFooter<D>                    
      template<Cid SID, CT::Typelist L = ComponentList>
      static constexpr size_t DefineHeapFooterGlobal(
         [[maybe_unused]] const size_t count
      ) noexcept {
         size_t bytesize = 0;
         ForEach(L{}, [&bytesize, &count]<class C> {
            if constexpr (requires { typename C::Subcomponents; })
               bytesize += DefineHeapFooterGlobal<SID, typename C::Subcomponents>(count);
            else if constexpr (requires { typename C::HeapRequest; }) {
               using R = typename C::HeapRequest;
               if constexpr (IsGlobalFooterRequest<R> /*IsFooterRequest<R>*/) {
                  //if constexpr (not R::AllocatedPerDimension) {
                     static_assert(not R::AllocatedPerIndirection,
                        "Can't have a PerIndirection modifier without PerDimension modifier, "
                        "because indirections are individual to each dimension"
                     );
                     using PROVIDER  = typename decltype(FindProvider<SID>())::First;
                     using INTERSECT = C::Id::template Intersect<typename PROVIDER::Id>;
                     if constexpr (not INTERSECT::Empty)
                        bytesize += sizeof(TypeOf<R>) * (R::AllocatedPerElement ? count : 1);
                  //}
               }
            }
         });
         return bytesize;
      }

      /// Go through all components relevant to the provider associated with  
      /// dimension SID that have no PerDimension modifier, and accumulate    
      ///   @param count Footer can depend on a new reserved amount           
      ///   @return The size of the global heap footer for the provider       
      ///      associated with the dimension 'SID'. Given 'D' is the last     
      ///      dimension for that provider, the offset is relative to:        
      ///      GetRawReserveEnd<D>() + DefineHeapFooter<D>                    
      template<class PICK, Cid SID, CT::Typelist L>
      static constexpr auto GetHeapFooterOffsetGlobalInner(
         [[maybe_unused]] const size_t count, size_t& offset
      ) noexcept {
         return ForEachConstOr(L{}, [&offset, &count]<class C> {
            if constexpr (requires { typename C::Subcomponents; })
               return GetHeapFooterOffsetGlobalInner<PICK, SID, typename C::Subcomponents>(count, offset);
            else if constexpr (CT::DerivedFrom<C, PICK>)
               return true;
            else if constexpr (requires { typename C::HeapRequest; }) {
               using R = typename C::HeapRequest;
               if constexpr (IsGlobalFooterRequest<R> /*IsFooterRequest<R>*/) {
                  //if constexpr (not R::AllocatedPerDimension) {
                     /*static_assert(not R::AllocatedPerIndirection,
                        "Can't have a PerIndirection modifier without PerDimension modifier, "
                        "because indirections are individual to each dimension"
                     );*/
                     using PROVIDER  = typename decltype(FindProvider<SID>())::First;
                     using INTERSECT = C::Id::template Intersect<typename PROVIDER::Id>;
                     if constexpr (not INTERSECT::Empty)
                        offset += sizeof(TypeOf<R>) * (R::AllocatedPerElement ? count : 1);
                  //}
               }
               return No {};
            }
            else return No {};
         });
      }

      template<class PICK, Cid SID>
      static constexpr size_t GetHeapFooterOffsetGlobal(
         [[maybe_unused]] const size_t count
      ) noexcept {
         static_assert(requires { typename PICK::HeapRequest; },
            "Component data is not on the heap");

         using PICK_R = typename PICK::HeapRequest;
         static_assert(IsGlobalFooterRequest<PICK_R> /*IsFooterRequest<PICK_R>*/,
            "Not a global footer request, "
            "use GetHeapHeaderOffset/GetHeapFooterOffset instead"
         );
         /*static_assert(not PICK_R::AllocatedPerDimension,
            "PerDimension request, use GetHeapFooterOffset instead");
         static_assert(not PICK_R::AllocatedPerIndirection,
            "Can't have a PerIndirection modifier without PerDimension modifier, "
            "because indirections are individual to each dimension"
         );*/

         size_t offset = 0;
         GetHeapFooterOffsetGlobalInner<PICK, SID, ComponentList>(count, offset);
         return offset;
      }

      /// MARK: AccessStack                                                   
      /// Access a variable on the stack associated with a component          
      ///   @attention always returns a reference to valid memory             
      template<class PICK, class SELF>
      constexpr auto& AccessStack(this SELF&& self) noexcept {
         constexpr size_t IDX = GetStackOffset<PICK>();
         auto& result = ::Langulus::get<IDX>(self.mStack).value;
         using RC = LglsMutIf(SELF, decltype(result));
         return const_cast<RC>(result);
      }

      /// MARK: AccessHeap                                                    
      /// Access a variable on the heap of a specified dimension, associated  
      /// with a component.                                                   
      ///   @attention always returns a pointer which may be null if container
      ///      is disowned, or hasn't been heap-allocated yet. This is the    
      ///      price you pay for keeping stuff on the heap - repeated safety  
      ///      checks. Another drawback is when cached variables, like hashes,
      ///      are cached only when containers aren't disowned, thus fallback 
      ///      to emergent behavior may occur, like hashes being recomputed   
      ///      on every demand.                                               
      ///   @attention if ownership is provided, this call assumes that type  
      ///      information and reserved count have been initialized for the   
      ///      relevant dimension 'SID'                                       
      template<CT::Component PICK, Cid SID, CT::Container SELF>
      constexpr auto* AccessHeap(this SELF&& self) assumptious {
         static_assert(requires { typename PICK::HeapRequest; },
            "Component data is not on the heap");
         static_assert(PICK::Id::template Contains<SID>,
            "The PICK must share the provided ID");

         LglsAssumeDev(self.template GetAllocationInner<SID>(),
            "Heap requests are available only when container has ownership. "
            "Make sure you access them only then, and fallback to emergent "
            "behavior otherwise, if that's possible."
         );

         using R = typename PICK::HeapRequest;
         if constexpr (IsFooterRequest<R>) {
            // Access footer heap                                       
            if constexpr (R::AllocatedPerDimension) {
               // Local footer, positioned after each dimension data    
               const size_t reserved = self.template GetReserved<SID>();
               const size_t indirects = self.template GetIndirections<SID>();
               const size_t stride = self.template GetStride<SID>();
               const size_t offset = GetHeapFooterOffset<PICK, SID>(reserved, indirects);
               const auto heap = self.template GetRawAs<uint8_t, SID>();
               using RC = LglsMutIf(SELF, TypeOf<R>*);
               return reinterpret_cast<RC>(heap + reserved * stride + offset);
            }
            else {
               // Global footer, positioned after the last dimension    
               using PROVIDER = typename decltype(FindProvider<SID>())::First;
               constexpr Cid LAST_ID = PROVIDER::Id::Last;
               const size_t reserved = self.template GetReserved<LAST_ID>();
               const size_t indirects = self.template GetIndirections<LAST_ID>();
               const size_t stride = self.template GetStride<LAST_ID>();
               const size_t offset_local = DefineHeapFooter<LAST_ID>(reserved, indirects);
               const size_t offset_global = GetHeapFooterOffsetGlobal<PICK, SID>(reserved);
               const auto heap = self.template GetRawAs<uint8_t, LAST_ID>();
               using RC = LglsMutIf(SELF, TypeOf<R>*);
               return reinterpret_cast<RC>(heap + reserved * stride + offset_local + offset_global);
            }
         }
         else {
            // Access header heap                                       
            constexpr size_t offset = GetHeapHeaderOffset<PICK, SID>();
            const auto al   = self.template GetAllocationInner<SID>();
            const auto heap = al->GetBlockStart();
            using RC = LglsMutIf(SELF, R*);
            return reinterpret_cast<RC>(heap + offset);
         }
      }

      /// Often used to clear global heap footers upon allocation             
      template<class SELF>
      constexpr void ConstructHeapRequestGlobal(this SELF&& self) noexcept {
         ForEach(ComponentList{}, [&]<class C>{
            if_available_gcc(C::template ConstructHeapRequestGlobal<SELF>)();
         });
      }

      /// Often used to clear local heap footers upon allocation              
      ///   @attention works in one dimension at a time!                      
      template<Cid SID, class SELF>
      constexpr void ConstructHeapRequestPerDimension(this SELF&& self) noexcept {
         ForEach(ComponentList{}, [&]<class C>{
            if_available_gcc(C::template ConstructHeapRequestPerDimension<SID, SELF>)();
         });
      }

      /// Default-construct all components                                    
      template<class SELF>
      constexpr void ConstructDefault(this SELF&& self) noexcept {
         ForEach(ComponentList{}, [&]<class C>{
            if_available_gcc(C::template ConstructDefault<SELF>)();
         });
      }

      /// MARK: Absorb                                                        
      /// Call ConstructFrom in all components that implement it.             
      /// Fallback to ConstructDefault otherwise.                             
      template<CT::Container SELF, CT::Container FROM>
      constexpr void Absorb(this SELF& self, FROM&& from) {
         /*static_assert(CT::Handle<SELF> or CT::Contiguous<SELF> == CT::Contiguous<FROM>,
            "You can't absorb from containers with different contiguousness");*/ //TODO causes circular dependency due to ::std::contiguous_range and IterateRange

         decltype(auto) source = DeintCast(from);
         using I = IntentOf(from);

         if constexpr (not CT::Handle<SELF>) {
            if (source.IsEmpty()) {
               // If source is empty, we copy only the type and the     
               // unconstrained state. Everything else is defaulted.    
               ForEach(ComponentList{}, [&self,&from]<class C>{
                  if constexpr (requires { &C::template GetState<SELF>; }
                            or  requires { &C::template GetType<0, SELF>; }) {
                     if_available_gcc(C::template ConstructFrom<SELF, I>)(FWDIntent(from));
                  }
                  else if_available_gcc(C::template ConstructDefault<SELF>)();
               });
               return;
            }
         }

         ForEach(ComponentList{}, [&self,&from]<class C>{
                 if_available_gcc(C::template ConstructFrom<SELF, I>)(FWDIntent(from));
            else if_available_gcc(C::template ConstructDefault<SELF>)();
         });

         if constexpr (CT::Abandoned<I>) {
            // Make sure we disown an abandoned container at the end    
            // If container is not disownable, it is every component's  
            // responsibility to remain in a disowned state.            
            if_available(source.EnableDisowned());
         }
         else if constexpr (CT::Moved<I>) {
            // Make sure we reset state after all movement concludes    
            if_available(source.ResetState());
         }
      }

      /// MARK: SliceFrom                                                     
      /// Call SliceFrom in all components that implement it.                 
      /// Fallback to ConstructDefault otherwise.                             
      template<Cid ID, CT::Container SELF, CT::Container FROM>
      constexpr void SliceFrom(this SELF& self, FROM&& from) {
         using I = IntentOf(from);
         static_assert(CT::Handle<SELF>);
         static_assert(CT::Disowned<I>, "Slicing supports only disownment for now");
         ForEach(ComponentList{}, [&]<class C>{
                 if_available_gcc(C::template SliceFrom<ID, SELF, I>)(FWDIntent(from));
            else if_available_gcc(C::template ConstructDefault<SELF>)();
         });
      }

      /// MARK: Swap                                                          
      /// Swaps the immediate contents of two compatible containers           
      constexpr void Swap(Container& other) noexcept {
         mStack.swap(other.mStack);
      }

      /// MARK: Destroy                                                       
      /// Call Destroy in all components that implement it.                   
      /// You have to call it manually in your container's destructor, so     
      /// that 'deducing this' works appropriately.                           
      ///   @attention disowned containers are never destroyed                
      template<class SELF>
      constexpr void Destroy(this SELF& self) {
         if not consteval {
            if (self.IsDisowned())
               return;

            // Notice it executes in reverse                            
            ForEach(Reverse(ComponentList{}), [&]<class C> {
               if_available_gcc(C::template Destroy<SELF>)();
            });
         }
      }

      /// MARK: Free                                                          
      /// Call Free in all components that implement it.                      
      /// Always do it in reverse order!                                      
      ///   @attention disowned containers are never dereferenced             
      template<bool DEALLOCATE = true, class SELF>
      constexpr void Free(this SELF& self) {
         if not consteval {
            if (self.IsDisowned())
               return;

            // Notice it executes in reverse                            
            ForEach(Reverse(ComponentList{}), [&]<class C> {
               if_available_gcc(C::template Free<DEALLOCATE, SELF>)();
            });
         }
      }

      /// MARK: Keep                                                          
      /// Call Keep in all components that implement it.                      
      ///   @attention disowned containers are never referenced               
      template<class SELF>
      constexpr void Keep(this SELF& self) {
         if not consteval {
            if (self.IsDisowned())
               return;

            ForEach(ComponentList{}, [&]<class C> {
               if_available_gcc(C::template Keep<SELF>)();
            });
         }
      }

   public: // public because it is used from serialization routines
      /// MARK: GetHandle                                                     
      /// Get a handle to the first element(s). Very useful for internal use. 
      ///   @attention element might be uninitialized if C is discontiguous   
      ///   @attention handle should encompass all dimensions                 
      ///   @tparam AS the handle type, or void to decide automatically       
      ///   @return the handle to the first element                           
      template<class AS = void, CT::NotHandle C>
      decltype(auto) GetHandle(this C&& self) {
         static_assert(CT::Handle<AS> or CT::Void<AS>,
            "Must be either a handle or void (which will use DecideHandle");
         static_assert(not CT::Reference<AS>, "Strip references first");
         static_assert(CT::Dense<AS>,         "Must be dense");

         if constexpr (CT::Void<AS>)
            return DecideHandle<C> {self};
         else
            return AS {self};
      }

      /// No-op in case C is already a handle                                 
      template<class = void, CT::Handle C>
      constexpr C&& GetHandle(this C&& self) noexcept {
         return LglsFwd(self);
      }
      
      /// MARK: GetSlice                                                      
      /// Get a handle to the first element(s) chosen dimension               
      ///   @attention element might be uninitialized if C is discontiguous   
      ///   @tparam AS the handle type, or void to decide automatically       
      ///   @return the handle to the first element                           
      template<Cid SID, class AS = void, CT::NotHandle C>
      decltype(auto) GetSlice(this C&& self) {
         static_assert(CT::Handle<AS> or CT::Void<AS>,
            "Must be either a handle or void (which will use DecideSlice");
         static_assert(not CT::Reference<AS>, "Strip references first");
         static_assert(CT::Dense<AS>,         "Must be dense");

         if constexpr (CT::Void<AS>)
            return Deref<decltype(Fake<DecideHandle<C>>().template PickDimension<SID>())> {Slice<SID>, self};
         else
            return AS {Slice<SID>, self};
      }

      /// If C is already a handle, pick a slice from it                      
      template<Cid SID, class = void, CT::Handle C>
      decltype(auto) GetSlice(this C&& self) {
         return self.template PickDimension<SID>();
      }
      
      /// MARK: Apply                                                         
      /// Visit all element's handles and perform a function on them.         
      /// Handles both linear and non-linear containers gracefully.           
      ///   @tparam SKIP_EMPTY whether or not to skip empty elements inside   
      ///      maps/sets. If set to false, you have to check whether the      
      ///      current lambda argument is `if constexpr(CT::Supported)`       
      ///   @param lambda the function to perform. If the lambda returns bool,
      ///      you can end the loop early by returning false.                 
      ///   @param cookie the element/hash table spot to start off from       
      ///   @attention this will ignore any ordering                          
      ///   @attention assumes container isn't empty                          
      ///   @note when applying only to the keys/values of a map, you should  
      ///      first slice the map in the desired dimension, and then Apply   
      ///      for better overall performance.                                
      template<bool SKIP_EMPTY = true, CT::Container C>//TODO reverse?
      void Apply(this C& self, auto&& lambda, [[maybe_unused]] size_t cookie = 0) {
         LglsAssumeDev(not self.IsEmpty(), "Make sure container isn't empty");

         if constexpr (CT::Handle<C>)
            lambda(self);
         else if constexpr (CT::ContainsOne<C>)
            lambda(self.GetHandle());
         else {
            auto item = self.GetHandle() + cookie;

            if constexpr (CT::Contiguous<C>) {
               // Iterate a contiguous array of elements                
               LglsAssumeDev(cookie < self.GetCount(), "Limp cookie (contiguous)");
               auto const end = item + (self.GetCount() - cookie);
               while (item.GetRaw() != end.GetRaw()) {
                  if constexpr (CT::Bool<decltype(lambda(item))>) {
                     if (not lambda(item))
                        return;
                  }
                  else lambda(item);
                  ++item;
               }
            }
            else {
               // Iterate a hash table - some cells might be empty,     
               // thus container might not be a contiguous array        
               LglsAssumeDev(cookie < self.GetReserved(), "Limp cookie (discontiguous)");
               const auto tableBeg = self.GetHashTableInner() + cookie;
               const auto tableEnd = tableBeg + (self.GetReserved() - cookie);
               auto table = tableBeg;
               [[maybe_unused]] size_t scanned = 0;
               [[maybe_unused]] const size_t scannedMax = self.GetCount();
               while (table != tableEnd) {
                  if (*table) {
                     if constexpr (CT::Bool<decltype(lambda(item))>) {
                        if (not lambda(item))
                           return;
                     }
                     else lambda(item);

                     if constexpr (SKIP_EMPTY) {
                        ++scanned;
                        if (scanned == scannedMax)
                           return;
                     }
                  }
                  else if constexpr (not SKIP_EMPTY) {
                     if constexpr (CT::Bool<decltype(lambda(No{}))>) {
                        if (not lambda(No{}))
                           return;
                     }
                     else lambda(No{});
                  }

                  ++item;
                  ++table;
               }
            }
         }
      }

      /// MARK: AssignAbsorb                                                  
      /// Call AssignFrom in all components that implement it.                
      /// Fallback to AssignDefault otherwise.                                
      template<CT::Container LHS, CT::Container RHS>
      constexpr LHS& AssignAbsorb(this LHS& self, RHS&& rhs) {
         //TODO pinnables
         static_assert(CT::Contiguous<LHS> == CT::Contiguous<RHS>,
            "You can't assign-absorb from containers with different contiguousness");

         decltype(auto) from = DeintCast(rhs);
         // Make sure 'lhs' and 'rhs' are different instances,          
         // otherwise we lose rhs if we free lhs, and we have to        
         // free lhs in order to overwrite it with rhs.                 
         if (static_cast<const void*>(&self) == static_cast<const void*>(&from))
            return self;

         // Never modify containers if type-incompatible                
         self.AssertTypesAreSame(from);

         // Free old data and absorb the new container                  
         self.Free();
         self.ResetCount(); //TODO not redundant! but why? shouldn't Absorb set it either way?
         self.Absorb(LglsFwd(rhs));
         return self;
      }
   };
   }
}

namespace Langulus
{
   /// MARK: Loop control                                                     
   /// Loop controls from inside ForEach lambdas when iterating containers    
   struct LoopControl {
      enum Command : int {
         Break = 0,     // Break the loop                               
         Continue = 1,  // Continue the loop                            
         Repeat = 2,    // Repeat the current element                   
         Discard = 3,   // Remove the current element                   
         NextLoop = 4   // Skip to next function in the ForEach         
      } mControl;

      LoopControl() = delete;

      constexpr LoopControl(bool a) noexcept
         : mControl {static_cast<Command>(a)} {}
      constexpr LoopControl(Command a) noexcept
         : mControl {a} {}

      explicit constexpr operator bool() const noexcept {
         return mControl == Continue or mControl == Repeat;
      }

      constexpr bool operator == (const LoopControl&) const noexcept = default;
   };

   namespace Loop
   {
      /// Break the entire iteration as a whole                               
      constexpr LoopControl Break    = LoopControl::Break;
      /// Continue to next element or function                                
      constexpr LoopControl Continue = LoopControl::Continue;
      /// Repeat the current element                                          
      constexpr LoopControl Repeat   = LoopControl::Repeat;
      /// Remove the current element                                          
      constexpr LoopControl Discard  = LoopControl::Discard;
      /// End this iterating function and jump immediately to the next        
      constexpr LoopControl NextLoop = LoopControl::NextLoop;
   }
}
