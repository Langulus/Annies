///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Component.hpp"
#include "Langulus/IntentOf.hpp"
LglsDisableWarningPush
LglsDisableWarning_UnusedLocalTypedef


namespace Langulus::Annies::Component
{
   template<class...> struct Multitype;

   template<CT::Component...TN> requires (CountEnabled<TN...> == 0)
   struct Multitype<TN...> {
      using CTTI_Component = Yup;
      static constexpr bool SkipThisComponent = true;
   };

   ///                                                                        
   /// Combines multiple type components into a unified interface to combat   
   /// C++ base method ambiguities, and to add a bit more convenience.        
   ///   @tparam TC0, TC1, TCN... all the type components to unify            
   template<CT::Component...TN> requires (CountEnabled<TN...> >= 2)
   struct LANGULUS_EBCO Multitype<TN...> : TN... {
      using CTTI_Component = Yup;
      using CTTI_ReflectAs = void;
      using Subcomponents  = decltype(Discard(Types<TN...>{},  []<class C> static { return requires { C::SkipThisComponent; }; }));
      using Id             = decltype(Extract(Subcomponents{}, []<class C> static { return typename C::Id{}; }));
      using CTTI_Typed     = decltype(Extract(Subcomponents{}, []<class C> static { return Types<TypeOf<C>>{}; }));

      using Key = CTTI_Typed::First;
      using Val = CTTI_Typed::Second;

      static_assert(ForEachIndexedAnd(Subcomponents{}, []<class C, size_t I> {
         return C::Id::Count == 1 and C::Id::First == I; }),
         "Each enabled subcomponent needs to be dedicated to their single dimension, "
         "and all subcomponents need to be sequential"
      );

      static constexpr int ComponentPrecedence = -3000;
      static_assert(ForEachAnd(Subcomponents{}, []<class C> { return C::ComponentPrecedence == -3000; }),
         "All precedences should match");

      static constexpr bool TypeErased = ForEachOr(Subcomponents{}, []<class C> { return C::TypeErased; });
      static_assert(ForEachAnd(Subcomponents{}, []<class C> { return C::TypeErased == TypeErased; }),
         "Currently all types must either be type-erased or not");

      #define if_inherits(...) requires (ForEachOr(Subcomponents{}, [&]<class C> { \
         return requires { self.C::__VA_ARGS__; }; }))

      /// Get the contained type                                              
      ///   @tparam SID - type selector                                       
      template<Cid SID = 0>
      constexpr auto GetType(this auto const& self) noexcept {
         using C = typename Subcomponents::template At<SID>;
         return self.C::GetType();
      }
      constexpr auto GetKeyType(this auto const& self) noexcept {
         using C = typename Subcomponents::First;
         return self.C::GetType();
      }
      constexpr auto GetValType(this auto const& self) noexcept {
         using C = typename Subcomponents::Second;
         return self.C::GetType();
      }

      /// Get the size of a single element in bytes                           
      ///   @tparam SID - type selector                                       
      template<Cid SID = 0>
      constexpr size_t GetStride(this auto const& self) noexcept {
         using C = typename Subcomponents::template At<SID>;
         return self.C::GetStride();
      }
      constexpr size_t GetKeyStride(this auto const& self) noexcept {
         using C = typename Subcomponents::First;
         return self.C::GetStride();
      }
      constexpr size_t GetValStride(this auto const& self) noexcept {
         using C = typename Subcomponents::Second;
         return self.C::GetStride();
      }

      /// Get the alignment of a single element in bytes                      
      ///   @tparam SID - type selector                                       
      template<Cid SID = 0>
      constexpr pot_t GetAlignment(this auto const& self) noexcept {
         using C = typename Subcomponents::template At<SID>;
         return self.C::GetAlignment();
      }
      constexpr pot_t GetKeyAlignment(this auto const& self) noexcept {
         using C = typename Subcomponents::First;
         return self.C::GetAlignment();
      }
      constexpr pot_t GetValAlignment(this auto const& self) noexcept {
         using C = typename Subcomponents::Second;
         return self.C::GetAlignment();
      }

      /// Get the reflected type name                                         
      ///   @tparam SID - type selector                                       
      template<Cid SID = 0>
      constexpr auto GetName(this auto const& self) noexcept {
         using C = typename Subcomponents::template At<SID>;
         return self.C::GetName();
      }
      constexpr auto GetKeyName(this auto const& self) noexcept {
         using C = typename Subcomponents::First;
         return self.C::GetName();
      }
      constexpr auto GetValName(this auto const& self) noexcept {
         using C = typename Subcomponents::Second;
         return self.C::GetName();
      }

      /// Check if block has a data type                                      
      ///   @tparam SID - type selector                                       
      ///   @return true if data contained in this pack is specified          
      template<Cid SID = 0>
      constexpr bool IsTyped(this auto const& self) noexcept {
         using C = typename Subcomponents::template At<SID>;
         return self.C::IsTyped();
      }
      constexpr bool IsKeyTyped(this auto const& self) noexcept {
         using C = typename Subcomponents::First;
         return self.C::IsTyped();
      }
      constexpr bool IsValTyped(this auto const& self) noexcept {
         using C = typename Subcomponents::Second;
         return self.C::IsTyped();
      }

      /// Check if type is akin to the provided type (can run at compile-time 
      /// if container is statically-typed)                                   
      ///   @attention ignores all sparsity and cv-qualifiers                 
      ///   @tparam T the type to compare against                             
      ///   @return true if origin types match                                
      template<CT::NotVoid T, Cid SID = 0>
      constexpr bool Is(this auto const& self) noexcept {
         using C = typename Subcomponents::template At<SID>;
         return self.C::template Is<T>();
      }
      template<CT::NotVoid T>
      constexpr bool IsKey(this auto const& self) noexcept {
         using C = typename Subcomponents::First;
         return self.C::template Is<T>();
      }
      template<CT::NotVoid T>
      constexpr bool IsVal(this auto const& self) noexcept {
         using C = typename Subcomponents::Second;
         return self.C::template Is<T>();
      }

      /// Check if type origin is the same as another (always at runtime)     
      ///   @attention ignores sparsity and cv-qualifiers                     
      ///   @param type the type to check for                                 
      ///   @return true if this container's type is akin to 'type'           
      template<Cid SID = 0>
      constexpr bool Is(this auto const& self, auto const& type) noexcept {
         using C = typename Subcomponents::template At<SID>;
         return self.C::Is(type);
      }
      constexpr bool IsKey(this auto const& self, auto const& type) noexcept {
         using C = typename Subcomponents::First;
         return self.C::Is(type);
      }
      constexpr bool IsVal(this auto const& self, auto const& type) noexcept {
         using C = typename Subcomponents::Second;
         return self.C::Is(type);
      }

      /// Assert if any of the types aren't Akin                              
      ///   @attention ignores sparsity and cv-qualifiers                     
      ///   @param other the container to compare with                        
      template<CT::Container LHS, CT::Container RHS> requires CT::NoIntent<RHS>
      void AssertTypesAreAkin(this LHS const& self, RHS const& other) {
         Id::ForEach([&]<Cid D> {
            if constexpr (CT::TypeErased<RHS> or CT::TypeErased<LHS>) {
               auto t1 = self.template GetType<D>();
               auto t2 = other.template GetType<D>();
               if (t1 and t2) {
                  LglsAssert(t1.Is(t2), "Type mismatch", ": ",
                     t1, " is not akin to ", t2, " (dimension #", D, ")");
               }
            }
            else {
               (void) self;
               (void) other;
               static_assert(Akin<TypeOf<LHS, D>, TypeOf<RHS, D>>, "Type mismatch");
            }
         });
      }

      /// Check if type origin is the same as another container's type        
      ///   @attention ignores sparsity and cv-qualifiers                     
      ///   @param other the container to compare with                        
      ///   @return true if all container's type are Akin                     
      template<CT::Container LHS, CT::Container RHS> requires CT::NoIntent<RHS>
      constexpr bool Is(this LHS const& self, RHS const& other) noexcept {
         return Id::ForEachAnd([&]<Cid D> noexcept {
            if constexpr (CT::TypeErased<RHS> or CT::TypeErased<LHS>) {
               auto t1 = self.template GetType<D>();
               auto t2 = other.template GetType<D>();
               return t1 and t2 and t1.Is(t2);
            }
            else {
               (void) self;
               (void) other;
               return Akin<TypeOf<LHS, D>, TypeOf<RHS, D>>;
            }
         });
      }
      constexpr bool IsKey(this auto const& self, CT::Container auto const& other) noexcept {
         using C = typename Subcomponents::First;
         return self.C::Is(other);
      }
      constexpr bool IsVal(this auto const& self, CT::Container auto const& other) noexcept {
         using C = typename Subcomponents::Second;
         return self.C::Is(other);
      }

      /// Check if unqualified type is the same as provided one               
      ///   @attention ignores only cv-qualifiers (across all indirections)   
      ///   @tparam T the type to compare against                             
      ///   @return true if contained type is same as T                       
      template<CT::NotVoid T, Cid SID = 0>
      constexpr bool IsSame(this auto const& self) noexcept {
         using C = typename Subcomponents::template At<SID>;
         return self.C::template IsSame<T>();
      }
      template<CT::NotVoid T>
      constexpr bool IsKeySame(this auto const& self) noexcept {
         using C = typename Subcomponents::First;
         return self.C::template IsSame<T>();
      }
      template<CT::NotVoid T>
      constexpr bool IsValSame(this auto const& self) noexcept {
         using C = typename Subcomponents::Second;
         return self.C::template IsSame<T>();
      }

      /// Check if unqualified type is the same as another                    
      ///   @attention ignores only cv-qualifiers                             
      ///   @param type the type to check for                                 
      ///   @return true if this block contains similar data                  
      template<Cid SID = 0>
      constexpr bool IsSame(this auto const& self, auto const& type) noexcept {
         using C = typename Subcomponents::template At<SID>;
         return self.C::IsSame(type);
      }
      constexpr bool IsKeySame(this auto const& self, auto const& type) noexcept {
         using C = typename Subcomponents::First;
         return self.C::IsSame(type);
      }
      constexpr bool IsValSame(this auto const& self, auto const& type) noexcept {
         using C = typename Subcomponents::Second;
         return self.C::IsSame(type);
      }
      
      /// Assert if any of the types aren't Same                              
      ///   @attention ignores cv-qualifiers only                             
      ///   @param other the container to compare with                        
      template<CT::Container LHS, CT::Container RHS> requires CT::NoIntent<RHS>
      void AssertTypesAreSame(this LHS const& self, RHS const& other) {
         Id::ForEach([&]<Cid D> {
            if constexpr (CT::TypeErased<RHS> or CT::TypeErased<LHS>) {
               auto t1 = self.template GetType<D>();
               auto t2 = other.template GetType<D>();
               if (t1 and t2) {
                  LglsAssert(t1.IsSame(t2), "Type mismatch", ": ",
                     t1, " is not similar to ", t2, " (dimension #", D, ")");
               }
            }
            else {
               (void) self;
               (void) other;
               static_assert(Same<TypeOf<LHS, D>, TypeOf<RHS, D>>, "Type mismatch");
            }
         });
      }

      /// Check if types are similar to another containers'                   
      ///   @attention ignores cv-qualifiers only                             
      ///   @param other the container to compare with                        
      ///   @return true if all container's type are Same                     
      template<CT::Container LHS, CT::Container RHS> requires CT::NoIntent<RHS>
      constexpr bool IsSame(this LHS const& self, RHS const& other) noexcept {
         return Id::ForEachAnd([&]<Cid D> noexcept {
            if constexpr (CT::TypeErased<RHS> or CT::TypeErased<LHS>) {
               auto t1 = self.template GetType<D>();
               auto t2 = other.template GetType<D>();
               return t1 and t2 and t1.IsSame(t2);
            }
            else {
               (void) self;
               (void) other;
               return Same<TypeOf<LHS, D>, TypeOf<RHS, D>>;
            }
         });
      }
      constexpr bool IsKeySame(this auto const& self, CT::Container auto const& other) noexcept {
         using C = typename Subcomponents::First;
         return self.C::IsSame(other);
      }
      constexpr bool IsValSame(this auto const& self, CT::Container auto const& other) noexcept {
         using C = typename Subcomponents::Second;
         return self.C::IsSame(other);
      }

      /// Check if this type is exactly T (references are ignored)            
      ///   @tparam T the type to compare against                             
      ///   @return true if data type matches T                               
      template<CT::NotVoid T, Cid SID = 0>
      constexpr bool IsExact(this auto const& self) noexcept {
         using C = typename Subcomponents::template At<SID>;
         return self.C::template IsExact<T>();
      }
      template<CT::NotVoid T>
      constexpr bool IsKeyExact(this auto const& self) noexcept {
         using C = typename Subcomponents::First;
         return self.C::template IsExact<T>();
      }
      template<CT::NotVoid T>
      constexpr bool IsValExact(this auto const& self) noexcept {
         using C = typename Subcomponents::Second;
         return self.C::template IsExact<T>();
      }

      /// Check if this type is exactly another                               
      ///   @param type the type to match                                     
      ///   @return true if data type matches type exactly                    
      template<Cid SID = 0>
      constexpr bool IsExact(this auto const& self, auto&& type) noexcept {
         using C = typename Subcomponents::template At<SID>;
         return self.C::IsExact(type);
      }
      constexpr bool IsKeyExact(this auto const& self, auto const& type) noexcept {
         using C = typename Subcomponents::First;
         return self.C::IsExact(type);
      }
      constexpr bool IsValExact(this auto const& self, auto const& type) noexcept {
         using C = typename Subcomponents::Second;
         return self.C::IsExact(type);
      }
      
      /// Assert if any of the types aren't Exact                             
      ///   @param other the container to compare with                        
      template<CT::Container LHS, CT::Container RHS> requires CT::NoIntent<RHS>
      void AssertTypesAreExact(this LHS const& self, RHS const& other) {
         Id::ForEach([&]<Cid D> {
            if constexpr (CT::TypeErased<RHS> or CT::TypeErased<LHS>) {
               auto t1 = self.template GetType<D>();
               auto t2 = other.template GetType<D>();
               if (t1 and t2) {
                  LglsAssert(t1.IsExact(t2), "Type mismatch", ": ",
                     t1, " is not exactly ", t2, " (dimension #", D, ")");
               }
            }
            else {
               (void) self;
               (void) other;
               static_assert(Exact<TypeOf<LHS, D>, TypeOf<RHS, D>>, "Type mismatch");
            }
         });
      }

      /// Check if types are exactly the same as another containers'          
      ///   @param other the container to compare with                        
      ///   @return true if all container's type are Exact                    
      template<CT::Container LHS, CT::Container RHS> requires CT::NoIntent<RHS>
      constexpr bool IsExact(this LHS const& self, RHS const& other) noexcept {
         return Id::ForEachAnd([&]<Cid D> noexcept {
            if constexpr (CT::TypeErased<RHS> or CT::TypeErased<LHS>) {
               auto t1 = self.template GetType<D>();
               auto t2 = other.template GetType<D>();
               return t1 and t2 and t1.IsExact(t2);
            }
            else {
               (void) self;
               (void) other;
               return Exact<TypeOf<LHS, D>, TypeOf<RHS, D>>;
            }
         });
      }
      constexpr bool IsKeyExact(this auto const& self, CT::Container auto const& other) noexcept {
         using C = typename Subcomponents::First;
         return self.C::IsExact(other);
      }
      constexpr bool IsValExact(this auto const& self, CT::Container auto const& other) noexcept {
         using C = typename Subcomponents::Second;
         return self.C::IsExact(other);
      }

      /// Check if container contains pointers                                
      ///   @return true if the block contains pointers                       
      template<Cid SID = 0>
      constexpr bool IsSparse(this auto const& self) noexcept {
         using C = typename Subcomponents::template At<SID>;
         return self.C::IsSparse();
      }
      constexpr bool IsKeySparse(this auto const& self) noexcept {
         using C = typename Subcomponents::First;
         return self.C::IsSparse();
      }
      constexpr bool IsValSparse(this auto const& self) noexcept {
         using C = typename Subcomponents::Second;
         return self.C::IsSparse();
      }

      /// Get the number of indirections                                      
      /// int**** will result in 4; int* will result in 1, int results in 0.  
      template<Cid SID = 0>
      constexpr size_t GetIndirections(this auto const& self) noexcept {
         using C = typename Subcomponents::template At<SID>;
         return self.C::GetIndirections();
      }
      constexpr size_t GetKeyIndirections(this auto const& self) noexcept {
         using C = typename Subcomponents::First;
         return self.C::GetIndirections();
      }
      constexpr size_t GetValIndirections(this auto const& self) noexcept {
         using C = typename Subcomponents::Second;
         return self.C::GetIndirections();
      }

      /// Check if block is constant                                          
      ///   @attention disowned containers are always constant                
      ///   @return true if the contents are constant                         
      template<Cid SID = 0>
      constexpr bool IsConstant(this auto const& self) noexcept {
         using C = typename Subcomponents::template At<SID>;
         return self.C::IsConstant();
      }
      constexpr bool IsKeyConstant(this auto const& self) noexcept {
         using C = typename Subcomponents::First;
         return self.C::IsConstant();
      }
      constexpr bool IsValConstant(this auto const& self) noexcept {
         using C = typename Subcomponents::Second;
         return self.C::IsConstant();
      }

      /// Check if container is made of other containers                      
      ///   @return true if the container is deep                             
      template<Cid SID = 0>
      constexpr bool IsDeep(this auto const& self) noexcept {
         using C = typename Subcomponents::template At<SID>;
         return self.C::IsDeep();
      }
      constexpr bool IsKeyDeep(this auto const& self) noexcept {
         using C = typename Subcomponents::First;
         return self.C::IsDeep();
      }
      constexpr bool IsValDeep(this auto const& self) noexcept {
         using C = typename Subcomponents::Second;
         return self.C::IsDeep();
      }

      /// Check if container contains executable items                        
      ///   @return true if the container has at least one executable element 
      template<Cid SID = 0>
      constexpr bool IsExecutable(this auto const& self) noexcept {
         using C = typename Subcomponents::template At<SID>;
         return self.C::IsExecutable();
      }
      constexpr bool IsKeyExecutable(this auto const& self) noexcept {
         using C = typename Subcomponents::First;
         return self.C::IsExecutable();
      }
      constexpr bool IsValExecutable(this auto const& self) noexcept {
         using C = typename Subcomponents::Second;
         return self.C::IsExecutable();
      }

      /// Get the size of the type times the contained elements               
      ///   @return the size of all elements in bytes                         
      template<Cid SID = 0>
      constexpr size_t GetBytesize(this auto const& self) noexcept {
         using C = typename Subcomponents::template At<SID>;
         return self.C::GetBytesize();
      }
      constexpr size_t GetKeyBytesize(this auto const& self) noexcept {
         using C = typename Subcomponents::First;
         return self.C::GetBytesize();
      }
      constexpr size_t GetValBytesize(this auto const& self) noexcept {
         using C = typename Subcomponents::Second;
         return self.C::GetBytesize();
      }

      /// Set the contained data type if possible.                            
      /// This is still used if statically typed - checks if types are        
      /// compatible in constructors and assigners.                           
      ///   @tparam T the new type                                            
      template<CT::NotVoid T, Cid SID = 0>
      void SetType(this auto& self) {
         using C = typename Subcomponents::template At<SID>;
         self.C::template SetType<T>();
      }
      template<CT::NotVoid T>
      constexpr void SetKeyType(this auto const& self) {
         using C = typename Subcomponents::First;
         self.C::template SetType<T>();
      }
      template<CT::NotVoid T>
      constexpr void SetValType(this auto const& self) {
         using C = typename Subcomponents::Second;
         self.C::template SetType<T>();
      }

      /// Set the contained data type if possible.                            
      /// This is still used if statically typed - checks if types are        
      /// compatible in constructors and assigners.                           
      /// This particular override doesn't benefit from compile-time checks.  
      ///   @param type the new type                                          
      template<Cid SID = 0>
      void SetType(this auto& self, auto const& type) {
         using C = typename Subcomponents::template At<SID>;
         self.C::SetType(type);
      }
      constexpr void SetKeyType(this auto const& self, auto const& type) {
         using C = typename Subcomponents::First;
         self.C::SetType(type);
      }
      constexpr void SetValType(this auto const& self, auto const& type) {
         using C = typename Subcomponents::Second;
         self.C::SetType(type);
      }

      /// Check if type is mutable when the container is empty                
      template<Cid SID = 0>
      constexpr bool IsTypeConstrained(this auto const& self) noexcept {
         using C = typename Subcomponents::template At<SID>;
         return self.C::IsTypeConstrained();
      }
      constexpr bool IsKeyTypeConstrained(this auto const& self) noexcept {
         using C = typename Subcomponents::First;
         return self.C::IsTypeConstrained();
      }
      constexpr bool IsValTypeConstrained(this auto const& self) noexcept {
         using C = typename Subcomponents::Second;
         return self.C::IsTypeConstrained();
      }

      /// Set all contained data types by copying them from another container 
      /// This is still used if statically typed - checks if types are        
      /// compatible in constructors and assigners.                           
      ///   @attention intents like Clone and Copy will strip constness       
      ///   @param other the container to copy types from                     
      template<CT::Container I, class SELF> requires CT::Intent<I>
      void AbsorbType(this SELF& self, I const& other) {
         static_assert(CT::Handle<I>,
            "Multidimensional types should be set using a handle");

         ForEach(Subcomponents{}, [&]<class C> {
            //WORKAROUND GNU 14.2.0 refuses to recognize C as a base    
            //WORKAROUND Clang 21 refuses to unfold when Expand used    
            //WORKAROUND This workaround is the only thing that         
            //WORKAROUND pacifies both...                               
            //self.C::AbsorbType(other);
            auto absorb = &C::template AbsorbType<C::Id::First, I, SELF>;
            absorb(self, other);
         });
      }

      /// Deduce all types of the container from provided arguments           
      ///   @param a The arguments - one for each dimension.                  
      constexpr void DeduceType(this auto& self, auto const&...a) {
         Expand(Subcomponents{}, [&]<class...C> {
            (self.C::DeduceType(a), ...);
         });
      }

   protected:
      LglsComRemoval(friend);
      LglsComHeapMovable(friend);
      LglsComIndexedCommon(friend);
      LglsComEmplacement(friend);

      /// Reset the type of the container, unless it's type-constrained.      
      /// If this container isn't type-erased, this call is a no-op.          
      ///   @attention allocation remains the same, and might not correspond  
      ///      to the next type which is set                                  
      template<Cid SID = 0>
      constexpr void ResetType(this auto& self) noexcept {
         using C = typename Subcomponents::template At<SID>;
         self.C::ResetType();
      }
      
      /// Reset all types                                                     
      constexpr void ResetAllTypes(this auto& self) noexcept {
         ForEach(Subcomponents{}, [&]<class C> noexcept {
            if_available(self.C::ResetType());
         });
      }
      
      /// Get the contained type (inner)                                      
      template<Cid SID = 0>
      constexpr auto& GetTypeInner(this auto&& self) noexcept {
         using C = typename Subcomponents::template At<SID>;
         return self.C::GetTypeInner();
      }

      /// Set the contained type (inner)                                      
      template<Cid SID = 0>
      constexpr void SetTypeInner(this auto& self, auto&& type) noexcept {
         using C = typename Subcomponents::template At<SID>;
         self.C::SetTypeInner(type);
      }

      /// Transfer from any kind of container, respecting intents             
      ///   @param intent the intent and container to transfer from           
      template<class SELF, CT::Intent I> requires CT::Container<I>
      void ConstructFrom(this SELF& self, I&& intent) if_inherits(ConstructFrom(LglsFwd(intent))) {
         ForEach(Subcomponents{}, [&]<class C> {
            if_available(self.C::ConstructFrom(LglsFwd(intent)));
         });
      }

      #undef if_inherits
   };
}

LglsDisableWarningPop
