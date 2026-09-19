///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Component.hpp"
#include <Langulus/IntentOf.hpp>
#include <Langulus/CT/Convertible.hpp>
#include <Langulus/CT/Serializer.hpp>


namespace Langulus::Annies::Component
{
   ///                                                                        
   /// Adds operators for concatenation (+ and +=)                            
   /// May convert the argument (if CONVERT is specified in Com::Insertion).  
   /// Will never deepen the container in order to insert.                    
   ///   @tparam ID, SHARED operators that share the same insertion behavior. 
   ///   @attention this relies on Com::Insertion being present               
   template<Cid ID, Cid...SHARED>
   struct InsertionOperatorsConcat {
      using CTTI_Component = Yup;
      using CTTI_ReflectAs = void;
      using Id             = Values<ID, SHARED...>;

      static constexpr int  ComponentPrecedence = 3000;
      static constexpr bool InsertionOperatorsConcatEnabled = true;

      /// MARK: += element                                                    
      /// Insert `rhs` at the back. Supports bounded arrays.                  
      template<CT::ContainsMany LHS, CT::NotContainer RHS>
      LHS& operator += (this LHS& lhs, RHS&& rhs) {
         using ITEM = DeextAll<Deint<RHS>>;

         if constexpr (CT::Array<RHS>) {
            if constexpr (requires { LHS {LglsFwd(rhs)}; }) {
               // Some containers have bounded array constructors       
               lhs.Concat(LHS {LglsFwd(rhs)});
            }
            else {
               // Otherwise just insert/concatenate each array item     
               if constexpr (Same<TypeOf<LHS>, ITEM>) {
                  using I = IntentOf(rhs);
                  for (auto& i : DeintCast(rhs))
                     lhs.Insert(I::Nest(i));
               }
               else if constexpr (CT::Serializer<LHS>) {
                  static_assert(LHS::AttemptConvertOnInsert,
                     "Can't be concatenated - incompatible arguments");

                  for (auto& i : DeintCast(rhs))
                     Langulus::Serialize(i, lhs);
               }
               else {
                  static_assert(LHS::AttemptConvertOnInsert,
                     "Can't be concatenated - incompatible arguments");
                  static_assert(CT::Inner::FindMorphism<ITEM, LHS>() >= 0 /*CT::Convertible<ITEM, LHS>*/,
                     "Can't be concatenated - not convertible to LHS");

                  for (auto& i : DeintCast(rhs))
                     lhs.Concat(Convert<LHS>(i));
               }
            }
         }
         else if constexpr (Same<TypeOf<LHS>, ITEM>) {
            lhs.Insert(LglsFwd(rhs));
         }
         else if constexpr (CT::Serializer<LHS>) {
            static_assert(LHS::AttemptConvertOnInsert,
               "Can't be concatenated - incompatible arguments");

            Langulus::Serialize(DeintCast(rhs), lhs);
         }
         else {
            static_assert(LHS::AttemptConvertOnInsert,
               "Can't be concatenated - incompatible arguments");
            static_assert(CT::Inner::FindMorphism<ITEM, LHS>() >= 0 /*CT::Convertible<ITEM, LHS>*/,
               "Can't be concatenated - not convertible to LHS");

            lhs.Concat(Convert<LHS>(DeintCast(rhs)));
         }
         return lhs;
      }

      /// MARK: += self                                                       
      /// Concatenate another container at the back, reusing this one.        
      /// Supports bounded arrays.                                            
      template<CT::ContainsMany LHS, CT::Container RHS>
      LHS& operator += (this LHS& lhs, RHS&& rhs) {
         using ITEM = DeextAll<Deint<RHS>>;

         if constexpr (CT::Array<RHS>) {
            using I = IntentOf(rhs);
            for (auto& i : DeintCast(rhs)) {
               if constexpr (Same<ITEM, LHS>)
                  lhs.Concat(I::Nest(i));
               else
                  lhs.Insert(I::Nest(i));
            }
         }
         else if constexpr (Same<ITEM, LHS>)
            lhs.Concat(LglsFwd(rhs));
         else
            lhs.Insert(LglsFwd(rhs));
         return lhs;
      }
   };

   template<class T>
   concept HasInsertionOperatorsConcatEnabled = CT::Container<T> and Decay<T>::InsertionOperatorsConcatEnabled;
}

namespace Langulus::Annies
{
   /// MARK: NonContainer + Container                                         
   template<CT::NotContainer LHS, CT::Container RHS>
   requires (CT::NoIntent<LHS, RHS> and Com::HasInsertionOperatorsConcatEnabled<RHS>)
   RHS operator + (LHS const& lhs, RHS const& rhs) {
      using ITEM = DeextAll<LHS>;
      RHS temp;

      if constexpr (CT::Array<LHS>) {
         if constexpr (requires { RHS {lhs}; }) {
            // Some containers have bounded array constructors          
            temp.Concat(RHS {lhs}, rhs);
         }
         else {
            // Otherwise just insert/concatenate each array item        
            for (auto& i : lhs) {
               if constexpr (Same<TypeOf<RHS>, ITEM>)
                  temp.Insert(i);
               else if constexpr (CT::Serializer<RHS>) {
                  static_assert(RHS::AttemptConvertOnInsert,
                     "Can't be concatenated - incompatible arguments");
                  Langulus::Serialize(i, temp);
               }
               else {
                  static_assert(RHS::AttemptConvertOnInsert,
                     "Can't be concatenated - incompatible arguments");
                  static_assert(CT::Inner::FindMorphism<ITEM, RHS>() >= 0 /*CT::Convertible<ITEM, RHS>*/,
                     "Can't be concatenated - not convertible to RHS");
                  temp.Concat(Convert<RHS>(i));
               }
            }
            temp.Concat(rhs);
         }
      }
      else if constexpr (Exact<ITEM, TypeOf<RHS>>) {
         temp.Insert(lhs);
         temp.Concat(rhs);
      }
      else if constexpr (CT::Serializer<RHS>) {
         static_assert(RHS::AttemptConvertOnInsert,
            "Can't be concatenated - incompatible arguments");
         Langulus::Serialize(lhs, temp);
         temp.Concat(rhs);
      }
      else {
         static_assert(RHS::AttemptConvertOnInsert,
            "Can't be concatenated - incompatible argument");
         static_assert(CT::Inner::FindMorphism<ITEM, RHS>() >= 0 /*CT::Convertible<ITEM, RHS>*/,
            "Can't be concatenated - not convertible to RHS");
         temp.Concat(Convert<RHS>(lhs), rhs);
      }
      return temp;
   }
   
   /// MARK: Container + NonContainer                                         
   template<CT::Container LHS, CT::NotContainer RHS>
   requires (CT::NoIntent<LHS, RHS> and Com::HasInsertionOperatorsConcatEnabled<LHS>)
   LHS operator + (LHS const& lhs, RHS const& rhs) {
      using ITEM = DeextAll<RHS>;
      LHS temp;

      if constexpr (CT::Array<RHS>) {
         if constexpr (requires { LHS {rhs}; }) {
            // Some containers have bounded array constructors          
            temp.Concat(lhs, LHS {rhs});
         }
         else {
            // Otherwise just insert/concatenate each array item        
            temp.Concat(lhs);
            for (auto& i : rhs) {
               if constexpr (Same<TypeOf<LHS>, ITEM>)
                  temp.Insert(i);
               else if constexpr (CT::Serializer<LHS>) {
                  static_assert(LHS::AttemptConvertOnInsert,
                     "Can't be concatenated - incompatible arguments");
                  Langulus::Serialize(i, temp);
               }
               else {
                  static_assert(LHS::AttemptConvertOnInsert,
                     "Can't be concatenated - incompatible arguments");
                  static_assert(CT::Inner::FindMorphism<ITEM, LHS>() >= 0 /*CT::Convertible<ITEM, LHS>*/,
                     "Can't be concatenated - not convertible to RHS");
                  temp.Concat(Convert<LHS>(i));
               }
            }
         }
      }
      else if constexpr (Exact<TypeOf<LHS>, ITEM>) {
         temp.Concat(lhs);
         temp.Insert(rhs);
      }
      else if constexpr (CT::Serializer<LHS>) {
         static_assert(LHS::AttemptConvertOnInsert,
            "Can't be concatenated - incompatible arguments");
         temp.Concat(lhs);
         Langulus::Serialize(rhs, temp);
      }
      else {
         static_assert(LHS::AttemptConvertOnInsert,
            "Can't be concatenated - incompatible arguments");
         static_assert(CT::Inner::FindMorphism<ITEM, LHS>() >= 0 /*CT::Convertible<ITEM, LHS>*/,
            "Can't be concatenated - not convertible to LHS");
         temp.Concat(lhs, Convert<LHS>(rhs));
      }
      return temp;
   }
   
   /// MARK: Container + Container                                            
   ///   @attention when both sides are serializers, LHS takes priority       
   template<CT::Container LHS, CT::Container RHS>
   requires (Com::HasInsertionOperatorsConcatEnabled<LHS>
          or Com::HasInsertionOperatorsConcatEnabled<RHS>)
   auto operator + (LHS const& lhs, RHS const& rhs) {
      if constexpr (Exact<LHS, RHS>) {
         LHS temp;
         temp.Concat(lhs, rhs);
         return temp;
      }
      else if constexpr (CT::Serializer<LHS>) {
         LHS temp;
         static_assert(LHS::AttemptConvertOnInsert,
            "Can't be concatenated - incompatible arguments");
         temp.Concat(lhs);
         Langulus::Serialize(rhs, temp);
         return temp;
      }
      else {
         RHS temp;
         static_assert(RHS::AttemptConvertOnInsert,
            "Can't be concatenated - incompatible argument");
         Langulus::Serialize(lhs, temp);
         temp.Concat(rhs);
         return temp;
      }
   }
}
