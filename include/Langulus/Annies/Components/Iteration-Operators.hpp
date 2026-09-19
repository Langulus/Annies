///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Component.hpp"


namespace Langulus::Annies::Component
{
   /// Refers back to this particular component instance through the deduced  
   /// 'this'. Just for convenience. It is #undef-ed at the end of this file. 
   #define ThisCom self.IterationOperators<ID, SHARED...>

   ///                                                                        
   /// Adds +, -, +=, -=, ++ prefix/suffix, -- prefix/suffix operators.       
   /// Adds - operator for difference between two containers.                 
   /// These operators are fundamentally unsafe so the API is protected.      
   /// Used mainly by handles. For discontiguous container, these handles may 
   /// point to uninitialized values, and needs to be checked against a hash  
   /// table entry. Works on all dimensions simultaneously.                   
   ///   @tparam ID heap provider we're iterating                             
   ///   @tparam SHARED other heap providers that get iterated together       
   template<Cid ID, Cid...SHARED>
   struct IterationOperators {
      using CTTI_Component = Yup;
      using CTTI_ReflectAs = void;
      using Id = Values<ID, SHARED...>;

      static constexpr int ComponentPrecedence = 3000;

      /// Offset first element to the right by the desired amount             
      ///   @attention this doesn't check any boundaries, use carefully       
      ///   @attention this operates on all dimensions simultaneously         
      ///   @param offset the number of elements to offset                    
      ///   @return a shallow modified copy of this container                 
      template<CT::Container C>
      constexpr C operator + (this C const& self, size_t offset) assumptious {
         C copy = self;
         return copy.IterationOperators<ID, SHARED...>::operator += (offset);
      }

      /// Offset first element to the right by the desired amount             
      ///   @attention this doesn't check any boundaries, use carefully       
      ///   @attention this operates on all dimensions simultaneously         
      ///   @param offset the number of elements to offset                    
      ///   @return reference to this, after being modified                   
      template<CT::Container C>
      constexpr C& operator += (this C& self, size_t offset) assumptious {
         Id::ForEach([&]<Cid D> {
            auto data = self.template GetRaw<D>();
            LglsAssumeDevAndOptimize(data, "Invalid heap for dimension #", D);

            if constexpr (CT::TypeErased<C>) {
               auto T = self.template GetType<D>();
               data = static_cast<uint8_t*>(data) + T.GetSize() * offset;
               if constexpr (requires { self.template GetEntriesInner<D>(); }) {
                  auto& entries = self.template GetEntriesInner<D>();
                  if (entries)
                     entries += T.GetIndirections() * offset;
               }
            }
            else {
               using T = TypeOf<C, D>;
               data += offset;
               if constexpr (CT::Sparse<T>) {
                  if constexpr (requires { self.template GetEntriesInner<D>(); }) {
                     auto& entries = self.template GetEntriesInner<D>();
                     if (entries)
                        entries += IndirectsOf<T> * offset;
                  }
               }
            }

            self.template SetHeapInner<D>(data);
         });

         return self;
      }

      /// Prefix increment operator                                           
      ///   @attention this doesn't check any boundaries, use carefully       
      ///   @attention this operates on all dimensions simultaneously         
      ///   @return reference to this, after being modified                   
      template<CT::Container C>
      constexpr C& operator ++ (this C& self) assumptious {
         return ThisCom::operator += (1);
      }

      /// Suffix increment operator                                           
      ///   @attention this doesn't check any boundaries, use carefully       
      ///   @attention this operates on all dimensions simultaneously         
      ///   @return a copy of the state, before modifying it                  
      template<CT::Container C>
      constexpr C operator ++ (this C& self, int) assumptious {
         C backup = self;
         ThisCom::operator += (1);
         return backup;
      }
      
      /// Get the element difference between two iterators                    
      ///   @attention very usafe - assumes rhs's type is same as self        
      ///   @param rhs the other iterator                                     
      ///   @return the difference in number of elements                      
      template<CT::Container C, CT::Container RHS>
      constexpr auto operator - (this C const& self, RHS const& rhs)
      noexcept(not CT::TypeErased<C> and not LANGULUS(SAFE)) -> ::std::ptrdiff_t {
         #if LANGULUS(SAFE)
            // Make sure the difference in all dimensions is the same   
            const auto first = (self.template GetRawAs<uint8_t, ID>()
                             -   rhs.template GetRawAs<uint8_t, ID>()) / self.template GetStride<ID>();

            Id::ForEach([&]<Cid D> {
               const auto diff = self.template GetRawAs<uint8_t, D>()
                               -  rhs.template GetRawAs<uint8_t, D>();
               LglsAssert((diff % self.template GetStride<D>()) == 0,
                  "Unaligned difference for dimension #", D);
               LglsAssert( diff / self.template GetStride<D>() == first,
                  "Dimension not in tandem (#", D, ")");
            });
         #endif

         if constexpr (CT::TypeErased<C>) {
            const auto diff = self.template GetRawAs<uint8_t, ID>()
                            -  rhs.template GetRawAs<uint8_t, ID>();
            return diff / self.template GetStride<ID>();
         }
         else {
            return self.template GetRaw<ID>()
                  - rhs.template GetRaw<ID>();
         }
      }
      
      /// Offset first element to the left by the desired amount              
      ///   @attention this doesn't check any boundaries, use carefully       
      ///   @attention this operates on all dimensions simultaneously         
      ///   @param offset the number of elements to offset                    
      ///   @return a shallow modified copy of this container                 
      template<CT::Container C>
      constexpr C operator - (this C const& self, size_t offset) assumptious {
         C copy = self;
         return copy.IterationOperators<ID, SHARED...>::operator -= (offset);
      }

      /// Offset first element to the left by the desired amount              
      ///   @attention this doesn't check any boundaries, use carefully       
      ///   @attention this operates on all dimensions simultaneously         
      ///   @param offset the number of elements to offset                    
      ///   @return reference to this, after being modified                   
      template<CT::Container C>
      constexpr C& operator -= (this C& self, size_t offset) assumptious {
         Id::ForEach([&]<Cid D> {
            auto data = self.template GetRaw<D>();
            LglsAssumeDevAndOptimize(data, "Invalid heap for dimension #", D);

            if constexpr (CT::TypeErased<C>) {
               auto T = self.template GetType<D>();
               data = static_cast<uint8_t*>(data) - T.GetSize() * offset;
               if constexpr (requires { self.template GetEntriesInner<D>(); }) {
                  auto& entries = self.template GetEntriesInner<D>();
                  if (entries)
                     entries -= T.GetIndirections() * offset;
               }
            }
            else {
               using T = TypeOf<C, D>;
               data -= offset;
               if constexpr (CT::Sparse<T>) {
                  if constexpr (requires { self.template GetEntriesInner<D>(); }) {
                     auto& entries = self.template GetEntriesInner<D>();
                     if (entries)
                        entries -= IndirectsOf<T> *offset;
                  }
               }
            }

            self.template SetHeapInner<D>(data);
         });

         return self;
      }

      /// Prefix decrement operator                                           
      ///   @attention this doesn't check any boundaries, use carefully       
      ///   @attention this operates on all dimensions simultaneously         
      ///   @return reference to this, after being modified                   
      template<CT::Container C>
      constexpr C& operator -- (this C& self) assumptious {
         return ThisCom::operator -= (1);
      }

      /// Suffix decrement operator                                           
      ///   @attention this doesn't check any boundaries, use carefully       
      ///   @attention this operates on all dimensions simultaneously         
      ///   @return a copy of the state, before modifying it                  
      template<CT::Container C>
      constexpr C operator -- (this C& self, int) assumptious {
         C backup = self;
         ThisCom::operator -= (1);
         return backup;
      }
   };

   #undef ThisCom
}
