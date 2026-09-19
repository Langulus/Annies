///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
//#include "../Container.hpp"
#include "../Component.hpp"
#include <Langulus/MetaOf.hpp>
#include <Langulus/CT/Deep.hpp>


namespace Langulus::Annies
{
   using DMeta = RTTI::DMeta;
   //using TMeta = RTTI::TMeta;
}

namespace Langulus::Annies::Component
{
   ///                                                                        
   /// Defines the contained type at compile-time.                            
   /// Doesn't allow for type-erasure and doesn't take up space.              
   ///   @tparam META the type of the definition                              
   ///   @tparam TYPE static type, can't be void                              
   ///   @tparam ID which heap/stack is typed?                                
   template<class META, CT::NotVoid TYPE, Cid ID>
   struct TypedStatic {
      using CTTI_Component = Yup;
      using CTTI_Typed     = TYPE;
      using CTTI_ReflectAs = void;
      using Id             = Values<ID>;

      static constexpr int  ComponentPrecedence = -3000;
      static constexpr bool TypeErased = false;
      static constexpr bool Sparse     = CT::Sparse<TYPE>;
      static constexpr bool Dense      = CT::Dense<TYPE>;

      /// MARK: Public                                                        
      /// Get the reflected type definition                                   
      template<Cid SID = ID>// requires (SID == ID)
      META GetType() const noexcept {
         return MetaOf<TYPE>();
      }

      /// Get the size of a single element of TYPE in bytes                   
      template<Cid SID = ID>// requires (SID == ID)
      constexpr size_t GetStride() const noexcept {
         return sizeof(TYPE);
      }

      /// Get the alignment of a single element of TYPE in bytes              
      template<Cid SID = ID>// requires (SID == ID)
      constexpr pot_t GetAlignment() const noexcept {
         return pot_t(alignof(TYPE));
      }

      /// Get the reflected type name                                         
      template<Cid SID = ID>// requires (SID == ID)
      constexpr auto GetName() const noexcept {
         return NameOf<TYPE>();
      }

      /// Statically typed containers are always typed                        
      template<Cid SID = ID>// requires (SID == ID)
      constexpr bool IsTyped() const noexcept {
         return true;
      }

      /// Check if type origin is akin to one of the provided types.          
      /// Always happens at compile-time.                                     
      ///   @attention ignores sparsity and cv-qualifiers                     
      ///   @tparam T the type to compare against                             
      ///   @return true if origin types are the same                         
      template<CT::NotVoid T, Cid SID = ID>// requires (SID == ID)
      constexpr bool Is() const noexcept {
         return Akin<TYPE, T>;
      }

      /// Check if type origin is the same as another                         
      ///   @attention ignores sparsity and cv-qualifiers                     
      ///   @param type the type to check for                                 
      ///   @return true if this container has similar data                   
      template<Cid SID = ID>// requires (SID == ID)
      bool Is(META type) const noexcept {
         return GetType().Is(type);
      }
      
      /// Check if type origin is the same as another container's type.       
      /// This can potentially happen at compile-time.                        
      ///   @attention ignores sparsity and cv-qualifiers                     
      ///   @param other the type to check for                                
      template<Cid SID = ID, CT::Container C>// requires (SID == ID)
      constexpr void AssertTypesAreAkin(C const& other) const {
         if constexpr (CT::TypeErased<C>) {
            auto t1 = GetType();
            auto t2 = other.template GetType<SID>();
            if (t1 and t2) {
               LglsAssert(t1.Is(t2), "Type mismatch", ": ",
                  t1, " is not akin to ", t2, " (dimension #", SID, ")");
            }
         }
         else {
            (void) other;
            static_assert(Akin<TYPE, TypeOf<C, SID>>, "Type mismatch");
         }
      }

      /// Check if type origin is the same as another container's type.       
      /// This can potentially happen at compile-time.                        
      ///   @attention ignores sparsity and cv-qualifiers                     
      ///   @param other the type to check for                                
      ///   @return true if this container has similar data                   
      template<Cid SID = ID, CT::Container C>// requires (SID == ID)
      constexpr bool Is(C const& other) const noexcept {
         if constexpr (CT::TypeErased<C>)
            return GetType().Is(other.GetType());
         else
            return Akin<TYPE, TypeOf<C>>;
      }

      /// Check if unqualified type is the same as T.                         
      /// Always happens at compile-time.                                     
      ///   @attention ignores only cv-qualifiers                             
      ///   @tparam T the type to compare against                             
      ///   @return true if data type is same as T                            
      template<CT::NotVoid T, Cid SID = ID>// requires (SID == ID)
      constexpr bool IsSame() const noexcept {
         return Same<TYPE, T>;
      }

      /// Check if unqualified type is the same as another                    
      ///   @attention ignores only cv-qualifiers                             
      ///   @param type the type to check for                                 
      ///   @return true if this block contains similar data                  
      template<Cid SID = ID>// requires (SID == ID)
      bool IsSame(META type) const noexcept {
         return GetType().IsSame(type);
      }

      /// Check if unqualified type is the same as another container's type.  
      /// This can potentially happen at compile-time.                        
      ///   @attention ignores only cv-qualifiers                             
      ///   @param other the container to check for                           
      template<Cid SID = ID, CT::Container C>// requires (SID == ID)
      constexpr void AssertTypesAreSame(C const& other) const {
         if constexpr (CT::TypeErased<C>) {
            auto t1 = GetType();
            auto t2 = other.template GetType<SID>();
            if (t1 and t2) {
               LglsAssert(t1.IsSame(t2), "Type mismatch", ": ",
                  t1, " is not similar to ", t2, " (dimension #", SID, ")");
            }
         }
         else {
            (void) other;
            static_assert(Same<TYPE, TypeOf<C, SID>>, "Type mismatch");
         }
      }

      /// Check if unqualified type is the same as another container's type.  
      /// This can potentially happen at compile-time.                        
      ///   @attention ignores only cv-qualifiers                             
      ///   @param other the container to check for                           
      ///   @return true if this container has similar data                   
      template<Cid SID = ID, CT::Container C>// requires (SID == ID)
      constexpr bool IsSame(C const& other) const noexcept {
         if constexpr (CT::TypeErased<C>)
            return GetType().IsSame(other.GetType());
         else
            return Same<TYPE, TypeOf<C>>;
      }

      /// Check if this type is exactly T (ignored references).               
      /// Always happens at compile-time.                                     
      ///   @tparam T the type to compare against                             
      ///   @return true if data type matches at least one type               
      template<CT::NotVoid T, Cid SID = ID>// requires (SID == ID)
      constexpr bool IsExact() const noexcept {
         return Exact<TYPE, T>;
      }

      /// Check if this type is exactly another                               
      ///   @param type the type to match                                     
      ///   @return true if data type matches type exactly                    
      template<Cid SID = ID>// requires (SID == ID)
      bool IsExact(META type) const noexcept {
         return GetType().IsExact(type);
      }

      /// Check if this type is exactly another container's type              
      /// This can potentially happen at compile-time                         
      ///   @param other the block to match                                   
      template<Cid SID = ID, CT::Container C>// requires (SID == ID)
      constexpr void AssertTypesAreExact(C const& other) const {
         if constexpr (CT::TypeErased<C>) {
            auto t1 = GetType();
            auto t2 = other.template GetType<SID>();
            if (t1 and t2) {
               LglsAssert(t1.IsExact(t2), "Type mismatch", ": ",
                  t1, " is not exactly ", t2, " (dimension #", SID, ")");
            }
         }
         else {
            (void) other;
            static_assert(Exact<TYPE, TypeOf<C, SID>>, "Type mismatch");
         }
      }

      /// Check if this type is exactly another container's type              
      /// This can potentially happen at compile-time                         
      ///   @param other the block to match                                   
      ///   @return true if data type matches type exactly                    
      template<Cid SID = ID, CT::Container C>// requires (SID == ID)
      constexpr bool IsExact(C const& other) const noexcept {
         if constexpr (CT::TypeErased<C>)
            return GetType().IsExact(other.GetType());
         else
            return Exact<TYPE, TypeOf<C>>;
      }

      /// Check if container contains pointers                                
      ///   @return true if the block contains pointers                       
      template<Cid SID = ID>// requires (SID == ID)
      constexpr bool IsSparse() const noexcept {
         return CT::Sparse<TYPE>;
      }

      /// Get the number of indirections                                      
      /// int**** will result in 4; int* will result in 1, int results in 0.  
      template<Cid SID = ID>// requires (SID == ID)
      constexpr size_t GetIndirections() const noexcept {
         return IndirectsOf<TYPE>;
      }

      /// Check if contained data is constant                                 
      ///   @attention disowned containers are always constant                
      ///   @return true if the contents are constant                         
      template<Cid SID = ID>// requires (SID == ID)
      constexpr bool IsConstant(this auto const& self) noexcept {
         return CT::Constant<TYPE> or self.IsDisowned();
      }

      /// Check if container is made of other containers                      
      ///   @return true if the container is deep                             
      template<Cid SID = ID>// requires (SID == ID)
      constexpr bool IsDeep() const noexcept {
         return CT::Deep<TYPE>;
      }
      
      /// Check if container contains executable items                        
      ///   @return true if the container has at least one executable element 
      template<Cid SID = ID, CT::Container C>// requires (SID == ID)
      constexpr bool IsExecutable(this C const& self) noexcept {
         if (self.template IsEmpty<SID>())
            return false;

         if constexpr (CT::Executable<TYPE>)
            return true;
         else if constexpr (CT::Deep<TYPE>) {
            // Dig deeper                                               
            for (TYPE const& inner : self) {
               if (inner.template IsExecutable<SID>())
                  return true;
            }
            return false;
         }
         else return false;
      }
      
      /// Always returns true                                                 
      template<Cid SID = ID>// requires (SID == ID)
      constexpr bool IsTypeConstrained() const noexcept {
         return true;
      }

      /// Does nothing                                                        
      template<Cid SID = ID>// requires (SID == ID)
      constexpr void EnableTypeConstrained() const noexcept { }

      /// Can't disable type-constraint in a statically-typed container       
      template<Cid SID = ID>// requires (SID == ID)
      constexpr void DisableTypeConstrained() const noexcept {
         static_assert(false,
            "Can't disable type-constraint in a statically-typed container"
         );
      }

      /// Get the size of the type times the contained elements               
      ///   @return the size of all elements in bytes                         
      template<Cid SID = ID, CT::Container C>// requires (SID == ID)
      constexpr size_t GetBytesize(this C const& self) noexcept {
         return sizeof(TYPE) * self.template GetCount<SID>();
      }

      /// Dereference the first element inside the container                  
      constexpr auto& operator * (this auto&& self) assumptious
      requires requires { *self.template GetRawAs<TYPE>(); } {
         LglsAssumeDev(not self.IsEmpty(), "Container is empty");
         return *self.template GetRawAs<TYPE>();
      }

      /// Access the first element inside the container                       
      constexpr auto* operator -> (this auto&& self) assumptious
      requires requires { self.template GetRawAs<TYPE>(); } {
         LglsAssumeDev(not self.IsEmpty(), "Container is empty");
         return self.template GetRawAs<TYPE>();
      }
      
      /// This is still used if statically-typed - checks if types are        
      /// compatible in constructors and assigners                            
      ///   @tparam T the new type                                            
      template<CT::NotVoid T, Cid SID = ID>// requires (SID == ID)
      constexpr void SetType() {
         static_assert(CT::NotSheddable<T>, "Strip all sheddables first");
         static_assert(CT::NotReference<T>, "Strip all references first");
         static_assert(Exact<T, TYPE>, "Type mismatch");
      }

      /// This is still used if statically-typed - checks if types are        
      /// compatible when arguments are type-erased. This particular override 
      /// doesn't benefit from compile-time checks.                           
      ///   @param type the new type                                          
      template<Cid SID = ID>// requires (SID == ID)
      void SetType(META type) {
         LglsAssert(GetType().IsExact(type), "Type mismatch");
      }
      
      /// Set all contained data types by copying them from another container 
      /// This is still used if statically typed - checks if types are        
      /// compatible in constructors and assigners.                           
      ///   @attention intents like Clone and Copy will strip constness       
      ///   @param other the container to copy types from                     
      template<Cid SID = ID, CT::Container I, class SELF> requires CT::Intent<I>
      void AbsorbType(this SELF& self, I const& other) {
         if constexpr (TypeErased or CT::TypeErased<I>) {
            auto T = DeintCast(other).template GetType<SID>();
            if constexpr (CT::Copied<I> or CT::Cloned<I> or not CT::HeapAllocated<I>)
               self.template SetType<ID>(T.GetDecvq());
            else
               self.template SetType<ID>(T);
         }
         else {
            using T = Deref<TypeOf<Deint<I>, SID>>;
            if constexpr (CT::Copied<I> or CT::Cloned<I> or not CT::HeapAllocated<I>)
               self.template SetType<Decvq<T>, ID>();
            else
               self.template SetType<T, ID>();
         }
      }

      /// Deduce type of the container from provided argument. Statically     
      /// typed container can't change their type, so this acts like a static 
      /// assertion, if the argument is compatible with the contained type.   
      ///   @param a The argument. Accepts intents, handles, arrays etc.      
      template<class A>
      constexpr void DeduceType(A const& a) noexcept {
         if constexpr (CT::Handle<A>)
            AbsorbType<0>(Copy(a));
         else
            SetType<Decvq<DeextAll<Deref<Deint<A>>>>>();
      }
   };
}
