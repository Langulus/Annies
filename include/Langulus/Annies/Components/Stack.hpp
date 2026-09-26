///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Container.hpp"


namespace Langulus::Annies::Component
{
   /// Refers back to this particular component instance through the deduced  
   /// 'this'. Just for convenience. It is #undef-ed at the end of this file. 
   #define ThisCom self.Stack<T, ID>

   ///                                                                        
   /// Adds a variable to a container. Supports references.                   
   /// Increases the container's bytesize.                                    
   ///   @tparam T type of the variable (supports references)                 
   ///   @tparam ID the stack provider ID for use with other components.      
   ///      All stack/heap providers must have unique sequential IDs.         
   template<CT::NotVoid T, Cid ID>
   struct Stack {
      using CTTI_Component = Yup;
      using CTTI_ReflectAs = void;
      using StackRequest   = T;
      using Id             = Values<ID>;
      using StackProvider  = Id;

      static constexpr int ComponentPrecedence = -2000;
      
      /// Get a direct access to the stack memory                             
      template<Cid SID = ID>// requires (SID == ID)
      constexpr auto GetRaw(this auto&& self) noexcept {
         return &ThisCom::GetStackInner();
      }

      /// Get a direct access to the stack memory as a different type         
      template<class AS, Cid SID = ID, CT::Container C>// requires (SID == ID)
      constexpr auto GetRawAs(this C&& self) noexcept {
         using AScvq = LglsMutIf(C, AS*);
         return static_cast<AScvq>(ThisCom::GetRaw());
      }

      /// Get a direct access to the stack memory's end                       
      template<Cid SID = ID>// requires (SID == ID)
      constexpr auto GetRawEnd(this auto&& self) noexcept {
         return ThisCom::GetRaw() + 1;
      }

      /// Get a direct access to the stack memory's end                       
      template<Cid SID = ID>// requires (SID == ID)
      constexpr auto GetRawReserveEnd(this auto&& self) noexcept {
         return ThisCom::GetRawEnd();
      }
      
      /// Get pointer to the first element for the given dimension.           
      /// This is a lower-level routine that does only sparseness checking.   
      /// No conversion or copying occurs, only pointer arithmetic.           
      ///   @attention no type-safety                                         
      ///   @attention assumes the container is typed                         
      ///   @attention assumes the container has valid memory                 
      ///   @tparam AS the type of data we're accessing - use void to use the 
      ///      type of the container, if statically typed                     
      ///   @tparam SID can be used to access specific dimension              
      ///   @return pointer to the first element of the desired dimension     
      template<class AS = void, Cid SID = ID, CT::Container C>// requires (SID == ID)
      auto* Get(this C&& self) assumptious {
         static_assert(not CT::Handle<AS>,    "AS can't be a handle");
         static_assert(not CT::Reference<AS>, "Strip references first");

         using TC   = LglsMutIf(C, Deref<T>);
         using TCP  = LglsMutIf(C, TC*);
         using TH   = Tif<CT::Void<AS>, TC, AS>;
         using THP  = LglsMutIf(C, TH*);
         auto* heap = DecvqAllCast(ThisCom::GetRaw());

         // Casting to a desired static type                            
         if constexpr (IndirectsOf<TC> == IndirectsOf<TH>) {
            // No difference in indirections                            
            return const_cast<THP>(static_cast<DecvqAll<THP>>(heap));
         }
         else if constexpr (IndirectsOf<TC> > IndirectsOf<TH>) {
            // We need to dereference. Can be done without a            
            // reinterpret_cast, and thus be constexpr-friendly.        
            // Supports packed pointers as well.                        
            return static_cast<THP>(DenseCast<IndirectsOf<TC> - IndirectsOf<TH>>(heap));
         }
         else {
            // We are allowed to add one additional indirection         
            static_assert(IndirectsOf<TCP> == IndirectsOf<TH>,
               "Too many indirections");
            static_assert(CT::Sparse<TH>,
               "Casting to a dense shouldn't happen here");
            return static_cast<LglsMutIf(C, TH)>(heap);
         }
      }
      
      /// Get first element as a handle, or any desired wrapping type.        
      /// Conversion or copying may occur, depending on type.                 
      ///   @attention will throw if incompatible type is provided            
      ///   @tparam AS the type we're wrapping in                             
      ///   @tparam SID can be used to access specific dimension              
      ///   @return the element, as a reference if possible                   
      template<CT::NotVoid AS, Cid SID = ID, CT::Container C>// requires (SID == ID)
      decltype(auto) As(this C&& self) {
         static_assert(not CT::Reference<AS>, "Strip references first");

         if constexpr (CT::Handle<AS>) {
            static_assert(not CT::Pair<AS>,
               "Stacks can't be represented with multidimensional handle");

            if constexpr (CT::TypeErased<AS>) {
               // Type-erased handle                                    
               if constexpr (requires { self.template GetEntries<SID>(); }) {
                  return AS {
                     ThisCom::Get(),
                     self.template GetEntries<SID>(),
                     self.template GetType<SID>()
                  };
               }
               else return AS {
                  ThisCom::Get(),
                  self.template GetType<SID>()
               };
            }
            else {
               // Statically typed handle                               
               using HT = Deref<TypeOf<AS>>;
               static_assert(Same<T, HT>, "Type mismatch");

               if constexpr (requires { self.template GetEntries<SID>(); }) {
                  return AS {
                     ThisCom::Get(),
                     self.template GetEntries<SID>()
                  };
               }
               else return AS {ThisCom::Get()};
            }
         }
         else {
            // Access directly or wrapped in a container                
            if constexpr (Akin<T, AS>) {
               // Access directly                                       
               if constexpr (CT::Dense<AS> or CT::CustomPointer<AS>)
                  return *ThisCom::template Get<AS, SID>();
               else
                  return ThisCom::template Get<Deptr<AS>, SID>();
            }
            else if constexpr (CT::DeepDense<AS>) {
               // Wrap in a container                                   
               using H = DecideHandle<C>;

               if constexpr (CT::Pair<H> and not CT::Pair<AS>) {
                  //TODO magic numbers here, use H::PickDimension?
                  if constexpr (SID == 0)
                     return Decvq<AS> {Absorb, ThisCom::template As<typename H::KeyHandle, 0>()};
                  else if constexpr (SID == 1)
                     return Decvq<AS> {Absorb, ThisCom::template As<typename H::ValHandle, 1>()};
                  else
                     static_assert(false, "Unsupported SID");
               }
               else return Decvq<AS> {Absorb, ThisCom::template As<H, SID>()};
            }
            else static_assert(false, "Type mismatch");
         }
      }

      /// A safe way to get the first sparse entry after being resolved to    
      /// the most concrete type. Available only if container has DeepType.   
      ///   @return the most concrete representation of the first item        
      ///   @note defined in Handle.hpp because it requires HandleDisowned    
      template<Cid SID = ID, CT::Container C>// requires (SID == ID)
      auto GetResolved(this C&& self) -> HandleDisowned; /*{
         if (self.IsEmpty())
            return {};
         
         if (not self.IsSparse())
            return {Slice<SID>, self};

         if constexpr (CT::Resolvable<T>)
            return DenseCast(ThisCom::Get()).GetResolved();
         else
            return {Stackwise, self.template GetType<SID>().GetOrigin(), &DenseCast(ThisCom::Get())};
      }*/

      /// Get the first contained element, removing 'count' indirections.     
      /// Available only if container has DeepType defined.                   
      ///   @attention throws if type is incomplete and origin was reached    
      ///   @tparam AS specify the type we wrap the result in.                
      ///      Using 'void' will choose C::DeepType.                          
      ///   @param self deduced this                                          
      ///   @param count how many levels of indirection to remove?            
      ///   @return the dense first element                                   
      ///   @note defined in Handle.hpp because it requires HandleDisowned    
      template<Cid SID = ID, CT::Container C>// requires (SID == ID)
      auto GetDense(this C&& self, size_t count = -1) -> HandleDisowned; /*{
         if (self.IsEmpty())
            return {};

         if (not self.IsSparse() or count <= 0)
            return {Slice<SID>, self};

         // Check if origin type is complete before attempting anything 
         if (count >= IndirectsOf<T>) {
            LglsAssert(CT::Complete<Decay<T>>,
               "Trying to interface incomplete data `", self.GetType(),
               "` as dense"
            );
         }

         void* src = DecvqAllCast(&ThisCom::GetStackInner());
         auto type = self.GetType();
         while (count and type.IsSparse()) {
            auto nextType = type.GetDeptr();
            
            if (nextType.IsSparse()) {
               // Pointer T -> Pointer nextT                            
               type.GetDereffer()(src, &src);
            }
            else {
               // Pointer T -> Dense nextT                              
               return {Stackwise, nextType, UnpackPointer(type, nextType, src)};
            }

            type = nextType;
            --count;
         }
         
         LglsError("Should never be reached");
         return {};
      }*/

   protected:
      /// Get a direct access to the stack memory                             
      template<Cid SID = ID>// requires (SID == ID)
      constexpr void* GetRawVoid(this auto&& self) noexcept {
         return const_cast<void*>(static_cast<const void*>(&ThisCom::GetStackInner()));
      }

      /// Get the heap pointer (inner)                                        
      constexpr auto& GetStackInner(this auto&& self) noexcept {
         return self.template AccessStack<Stack>();
      }

      /// Set the heap pointer (inner)                                        
      constexpr void SetStackInner(this auto& self, T&& data) noexcept {
         ThisCom::GetStackInner() = LglsFwd(data);
      }

      /// Default-initialize the variable                                     
      constexpr void ConstructDefault(this auto& self) noexcept requires CT::NotReference<T> {
         ThisCom::SetStackInner({});
      }
   };

   #undef ThisCom
}
