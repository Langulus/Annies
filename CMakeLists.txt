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
   fetch_langulus_module(Fractalloc GIT_TAG wip)
endif()

# Build and install Annies library												
add_langulus_library(LangulusAnnies
   SOURCES  $<TARGET_OBJECTS:LangulusLogger>
            $<TARGET_OBJECTS:LangulusRTTI>
            $<TARGET_OBJECTS:LangulusFractalloc>
            $<TARGET_OBJECTS:LangulusPercist>
            source/Bytes.cpp
            source/Many.cpp
            source/Tag.cpp
            source/Recipe.cpp
            source/Neat.cpp
)

target_include_directories(LangulusAnnies
   PUBLIC   include
)

target_link_libraries(LangulusAnnies
   PUBLIC   LangulusFractalloc
)

target_compile_definitions(LangulusAnnies
   PRIVATE  LANGULUS_EXPORT_ALL
)

if(LANGULUS_OPTION_TESTING)
   enable_testing()
   add_subdirectory(test)
endif()