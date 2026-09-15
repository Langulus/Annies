# Annies
Modern (C++20) cross-platform set of type-erased containers, for standalone or [Langulus](https://github.com/Langulus/Langulus) use

[![Langulus::Annies CI](https://github.com/Langulus/Annies/actions/workflows/ci.yml/badge.svg)](https://github.com/Langulus/Annies/actions/workflows/ci.yml)

## What is it?
The main design goal behind the library, is to simplify containment of data as much as possible without sacrificing a lot of performance.
An `Any` can contain sparse/dense data, manage ownership and references, and ensure type safety at runtime. 
Type-erased containers can be safely reinterpreted to their statically optimized templated equivalents, if the contained type is known at compile time. 
Additionally, all containers utilize RTTI, managed memory, encryption (WIP), and compression (WIP). Here are some examples:

Simple initialization:
```c++
Any data = 42;                  // creates a flat container similar to TMany<int> {42}
if (data == 42)   { /*true*/  }
if (data == 5.0f) { /*false*/ }
```

Complex initialization:
```c++
Thing object;
Any data {42, 24, true, 5.0f, &object};
// ^ creates a deep container similar to: 
// TMany<Any> {
//    TMany<int> {42}, 
//    TMany<int> {24}, 
//    TMany<bool> {true}, 
//    TMany<float> {5.0f}, 
//    TMany<Thing*> {&object}
// }

// To avoid TMany<int> suboptimal duplication, simply group the integers
// When all elements of a list are exactly the same type, they're optimally packed
Any data { Any {42, 24}, true, 5.0f, &object};
// ^ creates a deep container similar to: 
// TMany<Any> {
//    TMany<int> {42, 24},
//    TMany<bool> {true}, 
//    TMany<float> {5.0f}, 
//    TMany<Thing*> {&object}
// }
```

Interface static data safely:
```c++
static Thing object;
Any data = &object;
data.~Any();                    // will never exercise ownership over unowned pointer
```

Iterate all elements, regardless of hierarchy:
```c++
data.ForEachDeep(
   [](const int& i) { /**/ },   // Will be executed for 42 and 24
                                // If no integers exist, will move to next function
   [](const Thing& t) { /**/ }, // Will be executed for the pushed object pointer
                                // Notice how we can retrieve it by reference instead
                                // If no Thing(s) exist, will move to next function
   [](const float& i) { /**/ }  // Will be executed for 5.0f
);
```

Iterate all elements, preserving hierarchy:
```c++
data.ForEach(
   [](const Any& group) {
      group.ForEach(
         [](const int* i) { /*doesn't matter if we iterate by dense or sparse value*/ },
         [](const Thing* t) { /**/ },
         [](const float& i) { /**/ }
      )
   }
);
```

You can safely reinterpret a container to a statically optimized equivalent:
```c++
Any integers {1, 2, 3};
if (integers.Is<int>()) {
   auto& optimized = reinterpret_cast<TMany<int>&>(integers);
   // do stuff with the optimized container, for better performance
}
```

Integrated with std::range, even when type-erased:
```c++
Any integers {1, 2, 3};
for (auto it : integers) {  // it is a type-erased iterator, that turns to Annies::Block when dereferenced
   if(it == 1)    { /*true for first element, false for the rest*/ }
   if(it == 1.0f) { /*always false*/ }
}
```

Index in various ways, each with advantages and disadvantages:
```c++
TMany<int> integers {1, 2, 3};
if (integers[0u] == 1) { /*unsigned index is fastest, but most unsafe*/ }
if (integers[-1] == 3) { /*signed indices allow for counting backwards*/ }
if (integers[IndexMiddle] == 2) { /*special indices are most safe and convenient, but slowest*/ }
if (integers[IndexMax] == 3) { /*some of them are context-dependent, IndexMax returns the biggest value*/ }
```

## Mission statement
>*"Within C++, there is a much smaller and cleaner language struggling to get out"*
>
> [Bjarne Stroustrup](https://www.stroustrup.com/index.html)
- Ultimately targeting C++23, and nothing less
- Maintain binary compatibility between compatible containers
- Maximize ease of use
- Prioritize performance rather than memory footprint
- Keep the code concise, well styled, readable, and documented, without compromise

## Development status
For the most part, the library is complete, with the exception of a couple of optional features, and containers:
1. Ordered maps and sets remain to be finished (50%)
2. Containers such as linked lists are not even conceived yet (you can use sparse Any/TMany containers as an alternative at this point)
3. Thread safety patterns not decided yet, will probably use standard stuff
4. The encryption feature is not implemented yet, library is not decided yet, may do it myself (optional feature)
5. The compression feature is not implemented yet, it will use [zlib](https://github.com/madler/zlib), naturally (optional feature)
6. [utfcpp](https://github.com/nemtrif/utfcpp) is planned for the `Text` container at some point (optional feature)
7. Some kind of JSON interoperability is planned in the far future, but it is not required at this point

## Past/Future considerations
The set of features and interfaces are likely to change radically at this point, as many features are not yet completely fleshed-out, and others will be added on demand, when [Langulus](https://github.com/Langulus/Langulus) is developed.
The plan is to never support older C++ standards. When C++23 comes out, it will be immediately incorporated, especially if it provides opportunity for fully constexpr RTTI. Currently, the library is designed for nothing less than C++20. Any attempt for porting to older standard is to produce a considerable bloat to the code, and might even require external tools, if prior to C++11. This is against our mission

## Building the library
1. Clone the main branch
   - navigate to a folder of your choice and:
   - `git clone https://github.com/Langulus/Annies.git`
2. Use CMake to configure the project...:
   - by directly opening the cloned folder in MSVC 2022 and running the integrated CMake (preferred)
   - by using CMake (at least version 3.22) directly (see the [CI file](https://github.com/Langulus/Annies/blob/main/.github/workflows/ci.yml) for hints on that)
3. If in-tree as part of another project, you are responsible for providing the dependencies. Otherwise, when configuring as a top-level project, wait for external libraries to be automatically and seamlessly downloaded at configure time (happens only once, unless you delete the `external` folder) and configured by CMake:
   - [Langulus::Core](https://github.com/Langulus/Core) is used for common definitions all across Langulus
   - [Langulus::Logger](https://github.com/Langulus/Logger) is used for logging on debug builds
      - [fmt](https://github.com/fmtlib/fmt) will be downloaded, too
   - [Langulus::RTTI](https://github.com/Langulus/RTTI) is used for reflecting any contained types - it is highly recommended to readup on it, so that you can fully take advantage of many Annies features, such as marking types explicitly as POD, for example
   - [Langulus::SIMD](https://github.com/Langulus/SIMD) is used for statically optimized containers of vectorizable types (WIP)
      - [SIMDe](https://github.com/simd-everywhere/simde) will be downloaded, too
4. Setup CMake options for features (and reconfigure if you have to)
   - enable `LANGULUS_SHARED_LIBRARIES` option, if you want a shared library build (disabled by default)
   - enable `LANGULUS_SAFE_MODE` for additional runtime assertions and sanity checks (disabled by default)
   - enable `LANGULUS_PARANOIA` to make sure that any released memory gets wiped to zero (disabled by default)
   - enable `LANGULUS_DEBUGGING` to explicitly enable some verbose logging (this is usually automatically deduced from popular IDEs)
   - enable `LANGULUS_FEATURE_MANAGED_MEMORY` if you want memory to be pooled and recycled when freed. You can safely push any kind of pointer, as long as it was allocated by the memory manager, or by the overridden new/delete feature. You can specify pooling strategies by reflection to fine tune your software. Allows you to basically distinguish between your data, and data on the stack/heap that is not yours (enabled by default)
   - enable `LANGULUS_FEATURE_MANAGED_REFLECTION`, so that reflections will be kept in a centralized location, when reflected, which speeds up type comparisons, and allows you to dynamically modify the reflection at runtime (enabled by default)
   - enable `LANGULUS_FEATURE_MEMORY_STATISTICS` for keeping track of managed memory (disabled by default, works only if managed memory feature is enabled, too)
   - enable `LANGULUS_FEATURE_NEWDELETE` overrides new/delete operators for anything statically linked to this library, or provides LANGULUS_MONOPOLIZE_MEMORY() macro for you to use to override them, if dynamically linked (disabled by default, works only if managed memory feature is enabled, too)
   - enable `LANGULUS_FEATURE_UNICODE` - WIP
   - enable `LANGULUS_FEATURE_COMPRESSION` - WIP
   - enable `LANGULUS_FEATURE_ENCRYPTION` - WIP
   - you can set `LANGULUS_ALIGNMENT` to a power-of-two number - it will affect available SIMD optimizations, as well as minimal allocation sizes
5. Build using your favourite C++20 compliant compiler version
6. Use by linking with Langulus.Annies CMake target (or library output), and including <LangulusAnnies.hpp>

## Motivation
**Annies** started out as an educational project. 
Few years later, as [Langulus](https://github.com/Langulus/Langulus) strayed away from standard containers, it became a simple drop-in replacement for `std::any`. With the years, it evolved to its own thing, with memory manager, RTTI, new semantics, and style.
[Langulus](https://github.com/Langulus/Langulus) required a fast and flexible way to share data between independent modules, utilizing runtime polymorphism, and type-erasure to minimize interdependences.

## Container catalogue
You can check examples and feature details by following the links (WIP)

### Block
 - **Block** - an intermediate container without ownership, that serves as base to all others
   - Binary compatible with: `Any`, `TMany`, `Bytes`, `Text`, `Path`
   - Status: ~90% complete, ~75% tested
   - Features:
     + Const-preserving - if you insert a constant pointer, that pointer remains constant throughout the container's lifetime
     + Static-preserving (when managed memory is enabled) - if you interface memory that is not produced by the memory manager, that data is never destroyed or resized. That way you can safely interface static data, or data on the stack, but also data in other containers. This technically turns any container into a view, much like `std::span` or `std::string_view`
     + Density-preserving - if you insert a pointer, a pointer will be contained throughout the container's lifetime. Each inserted pointer becomes a shared pointer, and will be tracked by the memory manager, if owned by it
     + Shallow-copy semantics - by default, copy-construction and copy-assignment of a Block based containers always performs a shallow-copy. That is, no actual data gets copied, only the references to the original data gets copied (and referenced, if the container has the Ownership feature)
     + Disown-copy semantics - disown-construction and disown-assignment allows for a shallow-copy without referencing, essentially subverting container ownership. These semantics have a narrow use, and are primarily used internally as an optimization
     + Move semantics - standard move-construction and move-assignment via std::move and std::forward
     + Abandon-move semantics - abandon-construction and abandon-assignment is the same as a standard move, but doesn't fully reset the source container, saving in on a couple of instructions. When using this, you should guarantee, that the instance you're moving will no longer be used by its scope for anything else, but destruction. These semantics have a narrow use, and are primarily used internally as an optimization
     + Clone-copy semantics - the clone-construction and clone-assignment is more like conventional C++ container copy/assign - it duplicates all contained data
     + Indexing - use a flexible set of index types to access specific elements inside containers - use unsigned integer for greatest performance; use signed integers and negative indices for indexing in reverse; or even use reserved indices, like IndexMax, to find the biggest element (if contained type is sortable)
     + Insert - insert an element at a statically optimized, or dynamic index (Indexing); insert a range of elements; insert using move, copy, disown, abandon, or clone semantics
     + Emplace - emplace a single element at a statically optimized, or dynamic index (Indexing), by using perfect forwarding
     + Concat - safely concatenate with another Block if possible, at a statically optimized, or dynamic index (Indexing)
     + Merge - insert an element, or a range of elements, if the elements are not yet found inside the container (if contained type is comparable)
     + Find - search for a specific element's index inside the container, starting at a specific offset, and given a specific direction (if contained type is comparable)
     + Hash - get a hash of all hashable contents (if contained types are hashable)
     + Compare - compare Blocks, element by element (if contained types are comparable)
     + Cast - safely access elements as different types, using only pointer arithmetics and RTTI
     + ForEach - use a visitor pattern by providing any set of lambdas with different argument types; iterate the container deeply or shallowly in the desired direction, and perform a lambda for each argument-compatible element
     + std::range integration - seamlessly integrates with ranged-for loops and std algorithms
     + Encrypt (WIP) - encrypt/decrypt the memory block with a set of keys
     + Compress (WIP) - compress/decompress the memory block with zlib
     + Diff (WIP) - generate a difference container between two inputs
     + Small value optimization (WIP) - avoid heap allocation for small data
 - **Any** - analogous to `std::any`, but can contain an array of elements, similar to a type-erased `std::vector`
   - Binary compatible with: `Block`, `TMany`, `Bytes`, `Text`, `Path`
   - Status: ~90% complete, ~75% tested
   - Features:
     + Ownership
     + All the aforementioned features of a Block
 - **TMany** - templated equivalent to `Any`, analogous to `std::vector<T>`
   - Binary compatible with: `Block`, `Any`, `Bytes`, `Text`, `Path`
   - Status: ~90% complete, ~75% tested
   - Features:
     + All `Any` features, but statically optimized for T
 - **Bytes** - raw byte container, with various raw byte manipulation services
   - Binary compatible with: `Block`, `Any`, `TMany<Byte>`
   - Status: ~90% complete, ~75% tested
   - Features:
     + All `TMany<Byte>` features, but statically optimized
     + Specialized interface for raw byte sequence manipulation
 - **Text** - count-terminated text container, analogous to `std::string`, with various string manipulation services
   - Binary compatible with: `Block`, `Any`, `TMany<Letter>`, `TMany<Byte>`, `Bytes`, `Path`
   - Status: ~90% complete, ~75% tested
 - **Path** - a specialized `Text` container, with various file-system path manipulation services
   - Binary compatible with: `Block`, `Any`, `TMany<Letter>`, `TMany<Byte>`, `Bytes`, `Text`
   - Status: ~30% complete, not tested

***
### BlockMap
 - **BlockMap** - an intermediate container without ownership, that serves as base to all map types
   - Binary compatible with: `UnorderedMap`, `TUnorderedMap`, `OrderedMap`, `TOrderedMap`
   - Status: ~90% complete, ~75% tested
   - Features:
     + Uses a variation of the [Robin Hood algorithm](https://programming.guide/robin-hood-hashing.html)
     + Same as `Block` features, but indexing happens only through keys and iterators; insertion at indices is disabled
     + Additional ForEachKey and ForEachValue patterns
 - **UnorderedMap** - type-erased equivalent to `std::unordered_map` based on hashing
   - Binary compatible with: `BlockMap`, `TUnorderedMap`, `OrderedMap`, `TOrderedMap`
   - Status: ~90% complete, ~75% tested
   - Features:
     + Ownership
     + All features of the aforementioned `BlockMap`
 - **TUnorderedMap** - templated binary-compatible equivalent to `UnorderedMap`, practically the same as `std::unordered_map<Key, Value>`
   - Binary compatible with: `BlockMap`, `TUnorderedMap`, `OrderedMap`, `TOrderedMap`
   - Status: ~90% complete, ~75% tested
   - Features:
     + All features of the aforementioned `UnorderedMap`, but statically optimized for `Key` and `Value`
 - **OrderedMap** - type-erased equivalent to `std::ordered_map` based on sorting
   - Binary compatible with: `BlockMap`, `UnorderedMap`, `TUnorderedMap`, `TOrderedMap`
   - Status: ~50% complete, not tested
   - Features:
     + Ordered
     + Ownership
     + All features of the aforementioned `BlockMap`
 - **TOrderedMap** - templated binary-compatible equivalent to `OrderedMap`, practically the same as `std::ordered_map<Key, Value>`
   - Binary compatible with: `BlockMap`, `UnorderedMap`, `TUnorderedMap`, `OrderedMap`
   - Status: ~50% complete, not tested
   - Features:
     + All features of the aforementioned `OrderedMap`, but statically optimized for `Key` and `Value`
     
***
### BlockSet
 - **BlockSet** - an intermediate container without ownership, that serves as base to all set types
   - Binary compatible with: `UnorderedSet`, `TUnorderedSet`, `OrderedSet`, `TOrderedSet`
   - Status: ~90% complete, not tested
   - Features:
     + Uses a variation of the [Robin Hood algorithm](https://programming.guide/robin-hood-hashing.html)
     + Same as `Block` features, but indexing happens only through iterators; insertion at indices is disabled
 - **UnorderedSet** - type-erased equivalent to `std::unordered_set` based on hashing
   - Binary compatible with: `BlockSet`, `TUnorderedSet`, `OrderedSet`, `TOrderedSet`
   - Status: ~90% complete, not tested
   - Features:
     + Ownership
     + All features of the aforementioned `BlockSet`
 - **TUnorderedSet** - templated binary-compatible equivalent to `UnorderedSet`, practically the same as `std::unordered_set<T>`
   - Binary compatible with: `BlockSet`, `TUnorderedSet`, `OrderedSet`, `TOrderedSet`
   - Status: ~90% complete, not tested
   - Features:
     + All features of the aforementioned `UnorderedSet`, but statically optimized for `T`
 - **OrderedSet** - type-erased equivalent to `std::ordered_set` based on sorting
   - Binary compatible with: `BlockSet`, `UnorderedSet`, `TUnorderedSet`, `TOrderedSet`
   - Status: ~50% complete, not tested
   - Features:
     + Ordered
     + Ownership
     + All features of the aforementioned `BlockSet`
 - **TOrderedSet** - templated binary-compatible equivalent to `OrderedSet`, practically the same as `std::ordered_set<T>`
   - Binary compatible with: `BlockSet`, `UnorderedSet`, `TUnorderedSet`, `OrderedSet`
   - Status: ~50% complete, not tested
   - Features:
     + All features of the aforementioned `OrderedSet`, but statically optimized for `T`
     
***
### Other  
- **Pair** - analogous to a type-erased `std::pair`, its `mKey` and `mValue` members are binary compatible with all `Block`-compatible types
   - Binary compatible with: `TPair`
   - Status: ~100% complete, ~75% tested
   - Features:
     + All the aforementioned features of an `Any`, but indirectly via access to `mKey` and `mValue` members
 - **TPair** - templated equivalent to `Pair`, analogous to `std::pair<Key, Value>`
   - Binary compatible with: `Pair`
   - Status: ~100% complete, ~75% tested
   - Features:
     + All the aforementioned features of `Pair`, but statically optimized for `Key` and `Value`
 - **TPointer** - pretty much the same as `std::shared_ptr`, but works with the memory manager
   - Binary compatible with: nothing
   - Status: ~100% complete, ~75% tested
   - Features:
     + Ownership
