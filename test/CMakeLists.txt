add_langulus_test(LangulusAnniesTest-Text
   SOURCES	   text/TestText-Empty.cpp
               text/TestText-Absorb.cpp
               text/TestText-Piecewise.cpp

   LIBRARIES   LangulusAnnies
)

add_langulus_test(LangulusAnniesTest-Bytes
   SOURCES	   bytes/TestBytes-Empty.cpp
               bytes/TestBytes-Absorb.cpp
               bytes/TestBytes-Piecewise.cpp

   LIBRARIES   LangulusAnnies
)

add_langulus_test(LangulusAnniesTest-TRef-TOwn
   SOURCES	   TestTRef.cpp
               TestTOwn.cpp

   LIBRARIES   LangulusAnnies
)

add_langulus_test(LangulusAnniesTest-Any
   SOURCES	   any/TestAny-Empty.cpp
               any/TestAny-Absorb.cpp
               any/TestAny-Piecewise.cpp
               any/TestAny-Convert.cpp

   LIBRARIES   LangulusAnnies
)

add_langulus_test(LangulusAnniesTest-Many
   SOURCES	   many/TestMany-Empty.cpp
               many/TestMany-Absorb.cpp
               many/TestMany-Piecewise.cpp
               many/TestMany-Convert.cpp
               many/TestMany-CornerCases.cpp

   LIBRARIES   LangulusAnnies
)

add_langulus_test(LangulusAnniesTest-Set
   SOURCES	   set/TestSet-Empty.cpp
               set/TestSet-Absorb.cpp
               set/TestSet-Piecewise.cpp

   LIBRARIES   LangulusAnnies
)

add_langulus_test(LangulusAnniesTest-Pair
   SOURCES	   pair/TestPair-Empty.cpp
               pair/TestPair-Absorb.cpp
               pair/TestPair-Piecewise.cpp

   LIBRARIES   LangulusAnnies
)

add_langulus_test(LangulusAnniesTest-Map
   SOURCES	   map/TestMap-Empty.cpp
               map/TestMap-Absorb.cpp
               map/TestMap-Piecewise.cpp

   LIBRARIES   LangulusAnnies
)