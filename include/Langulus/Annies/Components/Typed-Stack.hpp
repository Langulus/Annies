///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Component.hpp"
#include "../States/Typed.hpp"
#include "Langulus/IntentOf.hpp"
#include "Langulus/Typenav.hpp"
#include <Langulus/MetaOf.hpp>
#include <Langulus/CT/Akin.hpp>
#include <Langulus/CT/Deep.hpp>


namespace Langulus::Annies
{
   using DMeta = RTTI::DMeta;
}

namespace Langulus::Annies::Component
{
   /// Refers back to this particular component instance through the deduced  
   /// 'this'. Just for convenience. It is #undef-ed at the end of this file. 
   #define ThisCom self.TypedStack<META, TYPE, CONSTRAIN, ID>

   ///                                                                        
   /// Defines the contained type as a member variable, allowing the use of   
   /// type-erasure. You can optionally constrain the type at runtime.        
   ///   @attention when constrained, the value on the stack is used only     
   ///      for padding so that containers are binary-compatible, but not     
   ///      really red or written to.                                         
   ///   @tparam META the type of the meta                                    
   ///   @tparam TYPE optionally static type, use void for type-erasure       
   ///   @tparam CONSTRAIN override type-constraint                           
   ///   @tparam ID data provider that gets typed                             
   template<class META, class TYPE, bool CONSTRAIN, Cid ID>
   struct TypedStack : State::Typed<StateValueIf(CONSTRAIN or not ::std::is_void_v<TYPE>), ID> {
      using CTTI_Component = Yup;
      using CTTI_Typed     = TYPE;
      using CTTI_ReflectAs = void;
      using StackRequest   = META;
      using Id             = Values<ID>;

      static constexpr int  ComponentPrecedence = -3000;
      static constexpr bool TypeErased = CT::Void<TYPE> or LANGULUS(FORCE_TYPE_ERASURE);
      /// @attention valid only if not TypeErased                             
      static constexpr bool Sparse = not TypeErased and CT::Sparse<TYPE>;
      /// @attention valid only if not TypeErased                             
      static constexpr bool Dense  = not TypeErased and CT::Dense<TYPE>;

      /// MARK: Public                                                        
      /// Get the contained type - not possible at compile-time yet           
      ///   @tparam SID - type selector                                       
      template<Cid SID = ID>// requires (SID == ID)
      constexpr META GetType(this auto const& self) noexcept {
         if consteval { return META {}; }
         else {
            if constexpr (TypeErased)
               return ThisCom::GetTypeInner();
            else
               return MetaDataOf<TYPE>();
         }
      }

      /// Get the size of a single element in bytes                           
      ///   @tparam SID - type selector                                       
      template<Cid SID = ID>// requires (SID == ID)
      constexpr size_t GetStride(this auto const& self) noexcept {
         if constexpr (TypeErased)
            return ThisCom::GetTypeInner().GetSize();
         else
            return sizeof(TYPE);
      }

      /// Get the alignment of a single element in bytes                      
      ///   @tparam SID - type selector                                       
      template<Cid SID = ID>// requires (SID == ID)
      constexpr pot_t GetAlignment(this auto const& self) noexcept {
         if constexpr (TypeErased)
            return ThisCom::GetTypeInner().GetAlignment();
         else
            return pot_t(alignof(TYPE));
      }

      /// Get the reflected type name                                         
      ///   @tparam SID - type selector                                       
      template<Cid SID = ID>// requires (SID == ID)
      constexpr auto GetName(this auto const& self) noexcept {
         if constexpr (TypeErased)
            return ThisCom::GetTypeInner().GetName();
         else
            return NameOf<TYPE>();
      }

      /// Check if block has a data type                                      
      ///   @tparam SID - type selector                                       
      ///   @return true if data contained in this pack is specified          
      template<Cid SID = ID>// requires (SID == ID)
      constexpr bool IsTyped(this auto const& self) noexcept {
         if constexpr (TypeErased)
            return static_cast<bool>(ThisCom::GetTypeInner());
         else
            return true;
      }

      /// Check if type is akin to the provided type (can run at compile-time 
      /// if container is statically-typed)                                   
      ///   @attention ignores all sparsity and cv-qualifiers                 
      ///   @tparam T the type to compare against                             
      ///   @return true if origin types match                                
      template<CT::NotVoid T, Cid SID = ID>// requires (SID == ID)
      constexpr bool Is(this auto const& self) noexcept {
         if constexpr (TypeErased)
            return ThisCom::GetTypeInner().Is(MetaDataOf<T>());
         else
            return Akin<TYPE, T>;
      }

      /// Check if type origin is the same as another (always at runtime)     
      ///   @attention ignores sparsity and cv-qualifiers                     
      ///   @param type the type to check for                                 
      ///   @return true if this container's type is akin to 'type'           
      template<Cid SID = ID>// requires (SID == ID)
      bool Is(this auto const& self, META type) noexcept {
         if constexpr (TypeErased)
            return ThisCom::GetTypeInner().Is(type);
         else
            return ThisCom::GetType().Is(type);
      }

      /// Check if type origin is the same as another container's type.       
      ///   @attention ignores sparsity and cv-qualifiers                     
      ///   @param other the type to check for                                
      template<Cid SID = ID, CT::Container C>// requires (SID == ID)
      constexpr void AssertTypesAreAkin(this auto const& self, C const& other) {
         if constexpr (TypeErased or CT::TypeErased<C>) {
            auto t1 = ThisCom::GetTypeInner();
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

      /// Check if type origin is the same as another container's type        
      ///   @attention ignores sparsity and cv-qualifiers                     
      ///   @param other the type to check for                                
      ///   @return true if this container's type is akin to other's          
      template<Cid SID = ID, CT::Container C>// requires (SID == ID)
      constexpr bool Is(this auto const& self, C const& other) noexcept {
         if constexpr (TypeErased or CT::TypeErased<C>)
            return ThisCom::GetTypeInner().Is(other.template GetType<SID>());
         else
            return Akin<TYPE, TypeOf<C, SID>>;
      }

      /// Check if unqualified type is the same as provided one               
      ///   @attention ignores only cv-qualifiers (across all indirections)   
      ///   @tparam T the type to compare against                             
      ///   @return true if contained type is same as T                       
      template<CT::NotVoid T, Cid SID = ID>// requires (SID == ID)
      constexpr bool IsSame(this auto const& self) noexcept {
         if constexpr (TypeErased)
            return ThisCom::GetTypeInner().IsSame(MetaDataOf<T>());
         else
            return Same<TYPE, T>;
      }

      /// Check if unqualified type is the same as another                    
      ///   @attention ignores only cv-qualifiers                             
      ///   @param type the type to check for                                 
      ///   @return true if this block contains similar data                  
      template<Cid SID = ID>// requires (SID == ID)
      bool IsSame(this auto const& self, META type) noexcept {
         if constexpr (TypeErased)
            return ThisCom::GetTypeInner().IsSame(type);
         else
            return ThisCom::GetType().IsSame(type);
      }

      /// Check if unqualified type is the same as another container's type   
      ///   @attention ignores only cv-qualifiers                             
      ///   @param other the container to check for                           
      template<Cid SID = ID, CT::Container C>// requires (SID == ID)
      constexpr void AssertTypesAreSame(this auto const& self, C const& other) {
         if constexpr (TypeErased or CT::TypeErased<C>) {
            auto t1 = ThisCom::GetTypeInner();
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

      /// Check if unqualified type is the same as another container's type   
      ///   @attention ignores only cv-qualifiers                             
      ///   @param other the container to check for                           
      ///   @return true if this container has similar data                   
      template<Cid SID = ID, CT::Container C>// requires (SID == ID)
      constexpr bool IsSame(this auto const& self, C const& other) noexcept {
         if constexpr (TypeErased or CT::TypeErased<C>)
            return ThisCom::GetTypeInner().IsSame(other.template GetType<SID>());
         else
            return Same<TYPE, TypeOf<C, SID>>;
      }

      /// Check if this type is exactly T (references are ignored)            
      ///   @tparam T the type to compare against                             
      ///   @return true if data type matches T                               
      template<CT::NotVoid T, Cid SID = ID>// requires (SID == ID)
      constexpr bool IsExact(this auto const& self) noexcept {
         if constexpr (TypeErased)
            return ThisCom::GetTypeInner().IsExact(MetaDataOf<T>());
         else
            return Exact<TYPE, T>;
      }

      /// Check if this type is exactly another                               
      ///   @param type the type to match                                     
      ///   @return true if data type matches type exactly                    
      template<Cid SID = ID>// requires (SID == ID)
      bool IsExact(this auto const& self, META type) noexcept {
         if constexpr (TypeErased)
            return ThisCom::GetTypeInner().IsExact(type);
         else
            return ThisCom::GetType().IsExact(type);
      }

      /// Check if this type is exactly another container's type              
      ///   @param other the block to match                                   
      ///   @return true if data type matches type exactly                    
      template<Cid SID = ID, CT::Container C>// requires (SID == ID)
      constexpr void AssertTypesAreExact(this auto const& self, C const& other) {
         if constexpr (TypeErased or CT::TypeErased<C>) {
            auto t1 = ThisCom::GetTypeInner();
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
      ///   @param other the block to match                                   
      ///   @return true if data type matches type exactly                    
      template<Cid SID = ID, CT::Container C>// requires (SID == ID)
      constexpr bool IsExact(this auto const& self, C const& other) noexcept {
         if constexpr (TypeErased or CT::TypeErased<C>)
            return ThisCom::GetTypeInner().IsExact(other.template GetType<ID>());
         else
            return Exact<TYPE, TypeOf<C, SID>>;
      }
      
      /// Check if container contains pointers                                
      ///   @return true if the block contains pointers                       
      template<Cid SID = ID>// requires (SID == ID)
      constexpr bool IsSparse(this auto const& self) noexcept {
         if constexpr (TypeErased)
            return ThisCom::GetTypeInner().IsSparse();
         else
            return CT::Sparse<TYPE>;
      }
      
      /// Get the number of indirections                                      
      /// int**** will result in 4; int* will result in 1, int results in 0.  
      template<Cid SID = ID>// requires (SID == ID)
      constexpr size_t GetIndirections(this auto const& self) noexcept {
         if constexpr (TypeErased)
            return ThisCom::GetTypeInner().GetIndirections();
         else
            return IndirectsOf<TYPE>;
      }
      
      /// Check if block is constant                                          
      ///   @attention disowned containers are always constant                
      ///   @return true if the contents are constant                         
      template<Cid SID = ID>// requires (SID == ID)
      constexpr bool IsConstant(this auto const& self) noexcept {
         if constexpr (TypeErased)
            return self.IsDisowned() or ThisCom::GetTypeInner().IsConstant();
         else
            return CT::Constant<TYPE> or self.IsDisowned();
      }

      /// Check if container is made of other containers                      
      ///   @return true if the container is deep                             
      template<Cid SID = ID>// requires (SID == ID)
      constexpr bool IsDeep(this auto const& self) noexcept {
         if constexpr (TypeErased)
            return ThisCom::GetTypeInner().IsDeep();
         else
            return CT::Deep<TYPE>;
      }

      /// Check if container contains executable items                        
      ///   @attention this is a deep check!                                  
      ///   @return true if the container has at least one executable element 
      template<Cid SID = ID, CT::Container C>// requires (SID == ID)
      constexpr bool IsExecutable(this C const& self) noexcept {
         if (self.template IsEmpty<ID>())
            return false;

         if constexpr (TypeErased) {
            // Type-erased                                              
            const auto T = ThisCom::GetTypeInner();
            if (T.IsExecutable())
               return true;
            else if (T.IsDeep()) {
               bool result = false;
               self.Apply([&result](auto const& item) noexcept {
                  if (item.template Get<typename C::DeepType const, SID>()->template IsExecutable<ID>()) {
                     result = true;
                     return false;
                  }
                  else return true;
               });
               return result;

               /*if constexpr (CT::ContainsMany<C>) {
                  bool result = false;
                  self.ForEach([&result](typename C::DeepType const& inner) noexcept {
                     if (inner.template IsExecutable<ID>()) {
                        result = true;
                        return Loop::Break;
                     }
                     return Loop::Continue;
                  });
                  return result;
               }
               else return self.template As<typename C::DeepType const>().template IsExecutable<ID>();*/
            }
            else return false;
         }
         else {
            // Statically-typed                                         
            if constexpr (CT::Executable<TYPE>)
               return true;
            else if constexpr (CT::Deep<TYPE>) {
               bool result = false;
               self.Apply([&result](auto const& item) noexcept {
                  if (item.template Get<Decay<TYPE>, SID>()->template IsExecutable<ID>()) {
                     result = true;
                     return false;
                  }
                  else return true;
               });
               return result;
            }
            else return false;
         }
      }

      /// Get the size of the type times the contained elements               
      ///   @return the size of all elements in bytes                         
      template<Cid SID = ID>// requires (SID == ID)
      constexpr size_t GetBytesize(this auto const& self) noexcept {
         return ThisCom::GetStride() * self.template GetCount<ID>();
      }
      
      /// Check if contained data can be interpreted as a given type by doing 
      /// only pointer arithmetics                                            
      ///   @attention direction matters, if block is dense                   
      ///   @param type the type check if current type interprets to          
      ///   @param count allows casts to arrays (Vec4f casts to float[4])     
      ///   @param binary_compatible do we require for the type to be         
      ///      binary compatible with this container's type (have same size)  
      ///   @param advanced an advanced check will also involve dynamic_cast, 
      ///      essentially making it bidirectional                            
      ///   @return true if able to interpret current type to 'type'          
      bool CastsTo(
         this auto const& self,
         META type, size_t count = 1,
         bool binary_compatible = false,
         bool advanced = false
      ) {
         const META t = ThisCom::GetType();
         return t and t.CastsTo(type, count, binary_compatible, advanced);
      }
   
      template<CT::NotVoid TO>
      bool CastsTo(
         this auto const& self,
         size_t count = 1,
         bool binary_compatible = false,
         bool advanced = false
      ) {
         return self.CastsTo(MetaDataOf<TO>(), count, binary_compatible, advanced);
      }

      /// Dereference the first element inside the container                  
      constexpr auto& operator * (this auto&& self) assumptious
      requires (not TypeErased and requires { *self.template GetRawAs<TYPE>(); }) {
         LglsAssumeDev(not self.IsEmpty(), "Container is empty");
         return *self.template GetRawAs<TYPE>();
      }

      /// Access the first element inside the container                       
      constexpr auto* operator -> (this auto&& self) assumptious
      requires (not TypeErased and requires { self.template GetRawAs<TYPE>(); }) {
         LglsAssumeDev(not self.IsEmpty(), "Container is empty");
         return self.template GetRawAs<TYPE>();
      }
      
      /// Set the contained data type if possible.                            
      /// This is still used if statically typed - checks if types are        
      /// compatible in constructors and assigners.                           
      ///   @tparam T the new type                                            
      template<CT::NotVoid T, Cid SID = ID, CT::Container C>// requires (SID == ID)
      void SetType(this C& self) {
         static_assert(CT::NotSheddable<T>, "Strip all sheddables first");
         static_assert(CT::NotReference<T>, "Strip all references first");
         static_assert(TypeErased or Exact<T, TYPE>, "Type mismatch");
         if constexpr (TypeErased)
            ThisCom::SetType(MetaDataOf<T>());
      }

      /// Set the contained data type if possible.                            
      /// This is still used if statically typed - checks if types are        
      /// compatible in constructors and assigners.                           
      /// This particular override doesn't benefit from compile-time checks.  
      ///   @param type the new type                                          
      template<Cid SID = ID, CT::Container C>// requires (SID == ID)
      void SetType(this C& self, META type) {
         if constexpr (TypeErased) {
            // This container is type-erased                            
            auto& t = ThisCom::GetTypeInner();
            if (t == type)
               return;
         
            if (not t) {
               t = type;
               return;
            }

            LglsAssert(not ThisCom::IsTypeConstrained(),
               "Attempting to mutate type-locked container"
               " of type ", t, " to type ", type
            );

            /*if (t.CastsTo(type)) {
               // Type is compatible, but only sparse data can mutate   
               // freely. Dense containers can't mutate because their   
               // destructors might be wrong later                      
               LglsAssert(t.IsSparse(), "Can't mutate ", t,
                  " to incompatible type ", type);
            }
            else {*/
               // Type is not compatible, but container is not typed, so
               // if it has no constructed elements we can still mutate 
               LglsAssert(self.template IsEmpty<ID>(), "Can't mutate ", t,
                  " to incompatible type ", type);
            /*}*/
            
            t = type;
         }
         else {
            // This container is statically typed                       
            auto local = MetaDataOf<TYPE>();
            LglsAssert(local.IsExact(type), "Type mismatch", ": ", local,
               " is not exactly ", type);
         }
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
               ThisCom::SetType(T.GetDecvq());
            else
               ThisCom::SetType(T);
         }
         else {
            using T = Deref<TypeOf<Deint<I>, SID>>;
            if constexpr (CT::Copied<I> or CT::Cloned<I> or not CT::HeapAllocated<I>)
               ThisCom::template SetType<Decvq<T>>();
            else
               ThisCom::template SetType<T>();
         }
      }

      /// Deduce type of the container from provided argument                 
      ///   @param a The argument. Accepts intents, handles, arrays etc.      
      template<class A>
      constexpr void DeduceType(this auto& self, A const& a) {
         static_assert(not Same<A, Describe>,
            "Can't deduce type from a describe intent. "
            "You have to set it up manually.");

         if constexpr (CT::Handle<A>)
            ThisCom::template AbsorbType<0>(Copy(a));
         else
            ThisCom::template SetType<Decvq<DeextAll<Deref<Deint<A>>>>>();
      }

   protected:
      /// MARK: Protected                                                     
      LglsComRemoval(friend);
      LglsComHeapMovable(friend);
      LglsComIndexedCommon(friend);
      LglsComEmplacement(friend);

      /// Reset the type of the container, unless it's type-constrained.      
      /// If this container isn't type-erased, this call is a no-op.          
      ///   @attention allocation remains the same, and might not correspond  
      ///      to the next type which is set                                  
      template<Cid SID = ID>// requires (SID == ID)
      constexpr void ResetType(this auto& self) noexcept {
         if constexpr (TypeErased) {
            if constexpr (requires { self.template IsTypeConstrained<SID>(); }) {
               if (not self.template IsTypeConstrained<SID>())
                  ThisCom::SetTypeInner({});
            }
            else ThisCom::SetTypeInner({});
         }
      }
      
      /// Resets all types, in case container is not Multitype                
      constexpr void ResetAllTypes(this auto& self) noexcept {
         if constexpr (TypeErased)
            ThisCom::ResetType();
      }
      
      /// Get the contained type (inner)                                      
      template<Cid SID = ID>// requires (SID == ID)
      constexpr auto& GetTypeInner(this auto&& self) noexcept {
         auto& member = self.template AccessStack<TypedStack>();
         if constexpr (not TypeErased) {
            // Statically typed containers generally don't use the      
            // stack member - it's there only for binary compatiblity.  
            // Set the member only if really, REALLY need by reference. 
            DecvqAllCast(member) = MetaDataOf<TYPE>();
         }
         return (member);
      }

      /// Set the contained type (inner)                                      
      ///   @attention noop if type-erased                                    
      template<Cid SID = ID>// requires (SID == ID)
      constexpr void SetTypeInner(this auto& self, const META& type) noexcept {
         if constexpr (TypeErased)
            ThisCom::GetTypeInner() = type;
      }

      /// Transfer from any kind of container, respecting intents.            
      /// Do it for a particular dimension.                                   
      ///   @param intent The intent and container to transfer from.          
      template<Cid D, class SELF, CT::Intent I> requires CT::Container<I>
      void SliceFrom(this SELF& self, I&& intent) {
         static_assert(CT::Disowned<I>);

         ThisCom::template AbsorbType<D>(LglsFwd(intent));

         if constexpr (TypeErased) { //TODO type constraints are either pointless, or should happen only if source is !Copied and !Cloned and !HeapAllocated
            // While we are interfacing external memory, we have to     
            // keep the type-constrained state, otherwise we risk       
            // interpreting static memory the wrong way.                
            if constexpr (not CONSTRAIN) {
               if constexpr (not CT::TypeErased<I>)
                  // From statically-typed to dynamically-typed         
                  ThisCom::EnableTypeConstrained();
               else if (intent->template IsTypeConstrained<D>())
                  // From dynamically-typed to dynamically-typed        
                  ThisCom::EnableTypeConstrained();
            }
         }
      }

      /// Transfer from any kind of container, respecting intents             
      ///   @param intent the intent and container to transfer from           
      template<class SELF, CT::Intent I> requires CT::Container<I>
      void ConstructFrom(this SELF& self, I&& intent) {
         ThisCom::AbsorbType(LglsFwd(intent));

         if constexpr (TypeErased) { //TODO type constraints are either pointless, or should happen only if source is !Copied and !Cloned and !HeapAllocated
            // While we are interfacing external memory, we have to     
            // keep the type-constrained state, otherwise we risk       
            // interpreting static memory the wrong way.                
            if constexpr (not CONSTRAIN) {
               if constexpr (not CT::TypeErased<I>)
                  // From statically-typed to dynamically-typed         
                  ThisCom::EnableTypeConstrained();
               else if (intent->template IsTypeConstrained<ID>())
                  // From dynamically-typed to dynamically-typed        
                  ThisCom::EnableTypeConstrained();
            }
         }

         if constexpr (CT::Moved<I> and CT::TypeErased<I>)
            intent->template SetTypeInner<ID>(META{});
      }
   };

   #undef ThisCom
}
