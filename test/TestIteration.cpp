///                                                                           
/// Langulus::Annies                                                         
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include <Langulus/Annies/Text.hpp>
#include "Common.hpp"


SCENARIO("Iterating containers", "[iteration]") {
   GIVEN("Templated Any with some POD items") {
      TMany<int> dense;
      dense << int(1) << int(2) << int(3) << int(4) << int(5);
      TMany<int*> sparse;
      sparse << new int(6) << new int(7) << new int(8) << new int(9) << new int(10);

      WHEN("Dense-iterating a dense pack (shallow)") {
         int it = 0;
         dense.ForEach([&](int& i) {
            REQUIRE(i == it + 1);
            ++it;
            return true;
         });
         REQUIRE(size_t(it) == dense.GetCount());

         it = 0;
         dense.ForEach<true>([&](int& i) {
            REQUIRE(i == 5 - it);
            ++it;
            return true;
         });
         REQUIRE(size_t(it) == dense.GetCount());

         it = 0;
         dense.ForEach([&](const int& i) {
            REQUIRE(i == it + 1);
            ++it;
            return true;
         });
         REQUIRE(size_t(it) == dense.GetCount());
      }

      WHEN("Dense-iterating a sparse pack (shallow)") {
         int it = 0;
         sparse.ForEach([&](int& i) {
            REQUIRE(i == it + 6);
            ++it;
            return true;
         });
         REQUIRE(size_t(it) == sparse.GetCount());

         it = 0;
         sparse.ForEach<true>([&](int& i) {
            REQUIRE(i == 10 - it);
            ++it;
            return true;
         });
         REQUIRE(size_t(it) == sparse.GetCount());

         it = 0;
         sparse.ForEach([&](const int& i) {
            REQUIRE(i == it + 6);
            ++it;
            return true;
         });
         REQUIRE(size_t(it) == sparse.GetCount());
      }

      /*WHEN("Sparse-iterating a dense pack (shallow)") {
         int it = 0;
         dense.ForEach([&](int* i) {
            REQUIRE(*i == it + 1);
            ++it;
            return true;
         });
         REQUIRE(size_t(it) == dense.GetCount());

         it = 0;
         dense.ForEach<true>([&](int* i) {
            REQUIRE(*i == 5 - it);
            ++it;
            return true;
         });
         REQUIRE(size_t(it) == dense.GetCount());

         it = 0;
         dense.ForEach([&](const int* i) {
            REQUIRE(*i == it + 1);
            ++it;
            return true;
         });
         REQUIRE(size_t(it) == dense.GetCount());
      }

      WHEN("Sparse-iterating a sparse pack (shallow)") {
         int it = 0;
         sparse.ForEach([&](int* i) {
            REQUIRE(*i == it + 6);
            ++it;
            return true;
         });
         REQUIRE(size_t(it) == sparse.GetCount());

         it = 0;
         sparse.ForEach<true>([&](int* i) {
            REQUIRE(*i == 10 - it);
            ++it;
            return true;
         });
         REQUIRE(size_t(it) == sparse.GetCount());

         it = 0;
         sparse.ForEach([&](const int* i) {
            REQUIRE(*i == it + 6);
            ++it;
            return true;
         });
         REQUIRE(size_t(it) == sparse.GetCount());
      }*/
   }

   // this test fails on msvc 64bit - causes heap corruption
   /*GIVEN("Universal maps with some pairs") {
      Map dense;
      dense
         << TPair<Text, int>("one", 1)
         << TPair<Text, int>("two", 2)
         << TPair<Text, int>("three", 3)
         << TPair<Text, int>("four", 4);

      Map sparse;
      sparse
         << TPair<Text*, int*>(new Text("five"), new int(5))
         << TPair<Text*, int*>(new Text("six"), new int(6))
         << TPair<Text*, int*>(new Text("seven"), new int(7))
         << TPair<Text*, int*>(new Text("eight"), new int(8));

      WHEN("Dense-iterating a dense map (shallow)") {
         int it = 0;
         dense.GetValues().ForEach([&](int& i) {
            REQUIRE(i == it + 1);
            ++it;
            return true;
         });
         REQUIRE(size_t(it) == dense.GetCount());

         it = 0;
         dense.GetValues().ForEachRev([&](int& i) {
            REQUIRE(i == 4 - it);
            ++it;
            return true;
         });
         REQUIRE(size_t(it) == dense.GetCount());

         it = 0;
         dense.GetValues().ForEach([&](const int& i) {
            REQUIRE(i == it + 1);
            REQUIRE(CT::Constant<decltype(i)>);
            ++it;
            return true;
         });
         REQUIRE(size_t(it) == dense.GetCount());

         it = 0;
         dense.ForEachPair([&](Text& key, int& value) {
            REQUIRE(value == it + 1);
            switch (it) {
            case 0: REQUIRE(key == "one"); break;
            case 1: REQUIRE(key == "two"); break;
            case 2: REQUIRE(key == "three"); break;
            case 3: REQUIRE(key == "four"); break;
            default:
               REQUIRE(false);
            }
            ++it;
            return true;
         });
         REQUIRE(size_t(it) == dense.GetCount());

         it = 0;
         dense.ForEachPairRev([&](Text& key, int& value) {
            REQUIRE(value == 4 - it);
            switch (it) {
            case 0: REQUIRE(key == "four"); break;
            case 1: REQUIRE(key == "three"); break;
            case 2: REQUIRE(key == "two"); break;
            case 3: REQUIRE(key == "one"); break;
            default:
               REQUIRE(false);
            }
            ++it;
            return true;
         });
         REQUIRE(size_t(it) == dense.GetCount());
      }

      WHEN("Dense-iterating a sparse map (shallow)") {
         int it = 0;
         sparse.GetValues().ForEach([&](int& i) {
            REQUIRE(i == it + 5);
            ++it;
            return true;
         });
         REQUIRE(size_t(it) == sparse.GetCount());

         it = 0;
         sparse.GetValues().ForEachRev([&](int& i) {
            REQUIRE(i == 8 - it);
            ++it;
            return true;
         });
         REQUIRE(size_t(it) == sparse.GetCount());

         it = 0;
         sparse.GetValues().ForEach([&](const int& i) {
            REQUIRE(i == it + 5);
            REQUIRE(CT::Constant<decltype(i)>);
            ++it;
            return true;
         });
         REQUIRE(size_t(it) == sparse.GetCount());

         it = 0;
         sparse.ForEachPair([&](Text& key, int& value) {
            REQUIRE(value == it + 5);
            switch (it) {
            case 0: REQUIRE(key == "five"); break;
            case 1: REQUIRE(key == "six"); break;
            case 2: REQUIRE(key == "seven"); break;
            case 3: REQUIRE(key == "eight"); break;
            default:
               REQUIRE(false);
            }
            ++it;
            return true;
         });
         REQUIRE(size_t(it) == sparse.GetCount());

         it = 0;
         sparse.ForEachPairRev([&](Text& key, int& value) {
            REQUIRE(value == 8 - it);
            switch (it) {
            case 0: REQUIRE(key == "eight"); break;
            case 1: REQUIRE(key == "seven"); break;
            case 2: REQUIRE(key == "six"); break;
            case 3: REQUIRE(key == "five"); break;
            default:
               REQUIRE(false);
            }
            ++it;
            return true;
         });
         REQUIRE(size_t(it) == sparse.GetCount());
      }

      WHEN("Sparse-iterating a dense map (shallow)") {
         int it = 0;
         dense.GetValues().ForEach([&](int* i) {
            REQUIRE(*i == it + 1);
            ++it;
            return true;
         });
         REQUIRE(size_t(it) == dense.GetCount());

         it = 0;
         dense.GetValues().ForEachRev([&](int* i) {
            REQUIRE(*i == 4 - it);
            ++it;
            return true;
         });
         REQUIRE(size_t(it) == dense.GetCount());

         it = 0;
         dense.GetValues().ForEach([&](const int* i) {
            REQUIRE(*i == it + 1);
            ++it;
            return true;
         });
         REQUIRE(size_t(it) == dense.GetCount());

         it = 0;
         dense.ForEachPair([&](Text& key, int& value) {
            REQUIRE(value == it + 1);
            switch (it) {
            case 0: REQUIRE(key == "one"); break;
            case 1: REQUIRE(key == "two"); break;
            case 2: REQUIRE(key == "three"); break;
            case 3: REQUIRE(key == "four"); break;
            default:
               REQUIRE(false);
            }
            ++it;
            return true;
         });
         REQUIRE(size_t(it) == dense.GetCount());

         it = 0;
         dense.ForEachPairRev([&](Text& key, int& value) {
            REQUIRE(value == 4 - it);
            switch (it) {
            case 0: REQUIRE(key == "four"); break;
            case 1: REQUIRE(key == "three"); break;
            case 2: REQUIRE(key == "two"); break;
            case 3: REQUIRE(key == "one"); break;
            default:
               REQUIRE(false);
            }
            ++it;
            return true;
         });
         REQUIRE(size_t(it) == dense.GetCount());
      }

      WHEN("Sparse-iterating a sparse map (shallow)") {
         int it = 0;
         sparse.GetValues().ForEach([&](int* i) {
            REQUIRE(*i == it + 5);
            ++it;
            return true;
         });
         REQUIRE(size_t(it) == sparse.GetCount());

         it = 0;
         sparse.GetValues().ForEachRev([&](int* i) {
            REQUIRE(*i == 8 - it);
            ++it;
            return true;
         });
         REQUIRE(size_t(it) == sparse.GetCount());

         it = 0;
         sparse.GetValues().ForEach([&](const int* i) {
            REQUIRE(*i == it + 5);
            ++it;
            return true;
         });
         REQUIRE(size_t(it) == sparse.GetCount());

         it = 0;
         sparse.ForEachPair([&](Text* key, int* value) {
            REQUIRE(*value == it + 5);
            switch (it) {
            case 0: REQUIRE(*key == "five"); break;
            case 1: REQUIRE(*key == "six"); break;
            case 2: REQUIRE(*key == "seven"); break;
            case 3: REQUIRE(*key == "eight"); break;
            default:
               REQUIRE(false);
            }
            ++it;
            return true;
         });
         REQUIRE(size_t(it) == sparse.GetCount());

         it = 0;
         sparse.ForEachPairRev([&](Text* key, int* value) {
            REQUIRE(*value == 8 - it);
            switch (it) {
            case 0: REQUIRE(*key == "eight"); break;
            case 1: REQUIRE(*key == "seven"); break;
            case 2: REQUIRE(*key == "six"); break;
            case 3: REQUIRE(*key == "five"); break;
            default:
               REQUIRE(false);
            }
            ++it;
            return true;
         });
         REQUIRE(size_t(it) == sparse.GetCount());
      }
   }*/

   GIVEN("Any") {
      constexpr float df = 5.55f;
      constexpr float sf = 6.55f;
      Many dense_any = df;
      Many sparse_any = new float(sf);

      WHEN("Dense-iterating a dense any (shallow)") {
         size_t it {};
         dense_any.ForEach([&](float& i) {
            REQUIRE(i == df + float(it));
            ++it;
            return true;
         });
         REQUIRE(it == dense_any.GetCount());

         it = 0;
         dense_any.ForEach<true>([&](float& i) {
            REQUIRE(i == df + float(it));
            ++it;
            return true;
         });
         REQUIRE(it == dense_any.GetCount());

         it = 0;
         dense_any.ForEach([&](const float& i) {
            REQUIRE(i == df + float(it));
            ++it;
            return true;
         });
         REQUIRE(it == dense_any.GetCount());
      }

      WHEN("Dense-iterating a sparse any (shallow)") {
         size_t it {};
         sparse_any.ForEach([&](float& i) {
            REQUIRE(i == sf + float(it));
            ++it;
            return true;
         });
         REQUIRE(it == sparse_any.GetCount());

         it = 0;
         sparse_any.ForEach<true>([&](float& i) {
            REQUIRE(i == sf + float(it));
            ++it;
            return true;
         });
         REQUIRE(it == sparse_any.GetCount());

         it = 0;
         sparse_any.ForEach([&](const float& i) {
            REQUIRE(i == sf + float(it));
            ++it;
            return true;
         });
         REQUIRE(it == sparse_any.GetCount());
      }

      /*WHEN("Sparse-iterating a dense pack (shallow)") {
         size_t it {};
         dense_any.ForEach([&](float* i) {
            REQUIRE(*i == df + float(it));
            ++it;
            return true;
         });
         REQUIRE(it == dense_any.GetCount());

         it = 0;
         dense_any.ForEach<true>([&](float* i) {
            REQUIRE(*i == df + float(it));
            ++it;
            return true;
         });
         REQUIRE(it == dense_any.GetCount());

         it = 0;
         dense_any.ForEach([&](const float* i) {
            REQUIRE(*i == df + float(it));
            ++it;
            return true;
         });
         REQUIRE(it == dense_any.GetCount());
      }

      WHEN("Sparse-iterating a sparse pack (shallow)") {
         size_t it {};
         sparse_any.ForEach([&](float* i) {
            REQUIRE(*i == sf + float(it));
            ++it;
            return true;
         });
         REQUIRE(it == sparse_any.GetCount());

         it = 0;
         sparse_any.ForEach<true>([&](float* i) {
            REQUIRE(*i == sf + float(it));
            ++it;
            return true;
         });
         REQUIRE(it == sparse_any.GetCount());

         it = 0;
         sparse_any.ForEach([&](const float* i) {
            REQUIRE(*i == sf + float(it));
            ++it;
            return true;
         });
         REQUIRE(it == sparse_any.GetCount());
      }*/
   }

   GIVEN("A universal Any with some deep items") {
      Many pack;
      Many subpack1;
      Many subpack2;
      Many subpack3;
      subpack1 << int(1) << int(2) << int(3) << int(4) << int(5);
      subpack2 << int(6) << int(7) << int(8) << int(9) << int(10);
      subpack3 << subpack1 << subpack2;
      pack << subpack1 << subpack2 << subpack3;

      REQUIRE(pack.GetUses() == 1);
      REQUIRE(subpack1.GetUses() == 3);
      REQUIRE(subpack2.GetUses() == 3);
      REQUIRE(subpack3.GetUses() == 2);

      WHEN("Flat-iterated with the intent to remove specific subpacks") {
         pack.ForEach([&](Many& subcontent) {
            if (subcontent.Is<int>())
               return Loop::Discard;
            return Loop::Continue;
         });

         //TODO REQUIRE(pack == subpack2); // this is the result when the discard optimization pipeline is implemented

         Many resultingPack;
         resultingPack << subpack3;
         REQUIRE(pack.GetUses() == 1);
         REQUIRE(pack == resultingPack);
         REQUIRE(subpack1.GetUses() == 2);
         REQUIRE(subpack2.GetUses() == 2);
         REQUIRE(subpack3.GetUses() == 3);
      }

      WHEN("Deep-iterated with the intent to remove specific subpacks") {
         pack.ForEachDeep([&](Many& subcontent) {
            if (subcontent == subpack1)
               return Loop::Discard;
            return Loop::Continue;
         });

         Many resultingPack;
         resultingPack << subpack2;
         resultingPack << subpack2;
         REQUIRE(pack.GetUses() == 1);
         REQUIRE(pack == resultingPack);
         REQUIRE(subpack1.GetUses() == 2);
         REQUIRE(subpack2.GetUses() == 6);  //TODO 6 due to branch out? make sure this is correct
         REQUIRE(subpack3.GetUses() == 1);
      }

      WHEN("Deep-iterated with the intent to remove specific subpacks (without skipping intermediate groups)") {
         pack.template ForEachDeep<false, false>([&](Many& subcontent) {
            if (subcontent == subpack1)
               return Loop::Discard;
            return Loop::Continue;
         });

         Many resultingPack;
         resultingPack << subpack2;
         resultingPack << subpack2;
         REQUIRE(pack.GetUses() == 1);
         REQUIRE(pack == resultingPack);
         REQUIRE(subpack1.GetUses() == 2);
         REQUIRE(subpack2.GetUses() == 6);  //TODO 6 due to branch out? make sure this is correct
         REQUIRE(subpack3.GetUses() == 1);
      }
   }

   // Destroy BANK before static data - otherwise problems happen if    
   // not using managed reflection                                      
   BANK.Reset();

   REQUIRE_FALSE(Allocator::CollectGarbage());
}
