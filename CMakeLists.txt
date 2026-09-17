cmake_minimum_required(VERSION 3.30)
project(LangulusAnnies
   VERSION         1.0.0
   DESCRIPTION     "Langulus container library - compose your own containers"
   HOMEPAGE_URL    https://langulus.com
)

# Check if this project is built as standalone, or a part of something else		
if (PROJECT_IS_TOP_LEVEL OR NOT LANGULUS)
   set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
   set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
   set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
   include(LangulusUtilities.cmake)

   fetch_langulus_module(RTTI GIT_TAG wip)
   if(LANGULUS_OPTION_MANAGED_MEMORY)
      fetch_langulus_module(Fractalloc GIT_TAG wip)
   endif()
   fetch_langulus_module(SIMD GIT_TAG wip)
endif()

file(GLOB_RECURSE
   LANGULUS_ANNIES_SOURCES 
   LIST_DIRECTORIES FALSE CONFIGURE_DEPENDS
   source/*.cpp
)

# Build and install Annies library												
add_langulus_library(LangulusAnnies
   SOURCES  $<TARGET_OBJECTS:LangulusLogger>
            $<TARGET_OBJECTS:LangulusRTTI>
            $<$<BOOL:${LANGULUS_OPTION_MANAGED_MEMORY}>:$<TARGET_OBJECTS:LangulusFractalloc>>
            $<$<BOOL:${LANGULUS_OPTION_PROFILING}>:$<TARGET_OBJECTS:LangulusPercist>>
            ${LANGULUS_ANNIES_SOURCES}
)

target_include_directories(LangulusAnnies
   PUBLIC   $<TARGET_PROPERTY:LangulusLogger,INTERFACE_INCLUDE_DIRECTORIES>
            $<TARGET_PROPERTY:LangulusRTTI,INTERFACE_INCLUDE_DIRECTORIES>
            $<$<BOOL:${LANGULUS_OPTION_MANAGED_MEMORY}>:$<TARGET_PROPERTY:LangulusFractalloc,INTERFACE_INCLUDE_DIRECTORIES>>
            $<$<BOOL:${LANGULUS_OPTION_PROFILING}>:$<TARGET_PROPERTY:LangulusPercist,INTERFACE_INCLUDE_DIRECTORIES>>
            include
)

target_link_libraries(LangulusAnnies
   PUBLIC   LangulusCore
            LangulusSIMD
            $<$<BOOL:${LANGULUS_OPTION_LOGGING}>:fmt>
)

target_compile_definitions(LangulusAnnies
   PRIVATE  LANGULUS_EXPORT_ALL
)

if(LANGULUS_OPTION_TESTING)
   enable_testing()
   add_subdirectory(test)
endif()