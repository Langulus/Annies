include(FetchContent)

# Utility for fetching langulus libraries using FetchContent                  
function(fetch_langulus_module NAME GIT_TAG TAG)
	if (LANGULUS)
		message(FATAL_ERROR "[LANGULUS] You can't fetch Langulus${NAME}, because this build \
		indicates LANGULUS is being build along your project. The library you're \
		trying to fetch should already be available locally.")
	endif()

    if(NOT DEFINED LANGULUS_EXTERNAL_DIRECTORY)
        set(LANGULUS_EXTERNAL_DIRECTORY "${CMAKE_SOURCE_DIR}/external" CACHE PATH
            "Place where external dependencies will be downloaded")
        message(STATUS "[LANGULUS] LANGULUS_EXTERNAL_DIRECTORY not defined, using default: ${LANGULUS_EXTERNAL_DIRECTORY}")
    endif()

   # Completely avoid downloading or updating anything, once the appropriate  
   # folder exists                                                            
   string(TOUPPER Langulus${NAME} UPPERCASE_NAME)
   if (EXISTS "${LANGULUS_EXTERNAL_DIRECTORY}/Langulus${NAME}-src")
      set(FETCHCONTENT_SOURCE_DIR_${UPPERCASE_NAME} "${LANGULUS_EXTERNAL_DIRECTORY}/Langulus${NAME}-src" CACHE INTERNAL "" FORCE)
      message(STATUS "[LANGULUS] Reusing the cached external library Langulus${NAME}")
      message(STATUS "[LANGULUS] (delete ${LANGULUS_EXTERNAL_DIRECTORY}/Langulus${NAME}-src manually if you want to redownload)")
   else()
      unset(FETCHCONTENT_SOURCE_DIR_${UPPERCASE_NAME} CACHE)
      message(STATUS "[LANGULUS] Freshly downloading external library Langulus${NAME} from https://github.com/Langulus/${NAME}.git")
   endif()

   FetchContent_Declare(
      Langulus${NAME}
      GIT_REPOSITORY  https://github.com/Langulus/${NAME}.git
      GIT_TAG         ${TAG}
      SOURCE_DIR      "${LANGULUS_EXTERNAL_DIRECTORY}/Langulus${NAME}-src"
      SUBBUILD_DIR    "${CMAKE_BINARY_DIR}/external/Langulus${NAME}-subbuild"
      ${ARGN}
   )
   FetchContent_MakeAvailable(Langulus${NAME})
endfunction()

# Utility for fetching external libraries using FetchContent                  
function(fetch_external_module NAME GIT_REPOSITORY REPO GIT_TAG TAG)
    if(NOT DEFINED LANGULUS_EXTERNAL_DIRECTORY)
        set(LANGULUS_EXTERNAL_DIRECTORY "${CMAKE_SOURCE_DIR}/external" CACHE PATH
            "Place where external dependencies will be downloaded")
        message(STATUS "[LANGULUS] LANGULUS_EXTERNAL_DIRECTORY not defined, using default: ${LANGULUS_EXTERNAL_DIRECTORY}")
    endif()

   # Completely avoid downloading or updating anything, once the appropriate  
   # folder exists                                                            
   string(TOUPPER ${NAME} UPPERCASE_NAME)
   if (EXISTS "${LANGULUS_EXTERNAL_DIRECTORY}/${NAME}-src")
      set(FETCHCONTENT_SOURCE_DIR_${UPPERCASE_NAME} "${LANGULUS_EXTERNAL_DIRECTORY}/${NAME}-src" CACHE INTERNAL "" FORCE)
      message(STATUS "[LANGULUS] Reusing the cached external library ${NAME}")
      message(STATUS "[LANGULUS] (delete ${LANGULUS_EXTERNAL_DIRECTORY}/${NAME}-src manually if you want to redownload)")
   else()
      unset(FETCHCONTENT_SOURCE_DIR_${UPPERCASE_NAME} CACHE)
      message(STATUS "[LANGULUS] Freshly downloading external library ${NAME} from ${REPO} ...")
   endif()

   FetchContent_Declare(
      ${NAME}
      GIT_REPOSITORY  ${REPO}
      GIT_TAG         ${TAG}
      SOURCE_DIR      "${LANGULUS_EXTERNAL_DIRECTORY}/${NAME}-src"
      SUBBUILD_DIR    "${CMAKE_BINARY_DIR}/external/${NAME}-subbuild"
      ${ARGN}
      EXCLUDE_FROM_ALL
   )
   FetchContent_MakeAvailable(${NAME})

   string(TOLOWER ${NAME} LOWERCASE_NAME)
   set(${NAME}_SOURCE_DIR "${${LOWERCASE_NAME}_SOURCE_DIR}" CACHE INTERNAL "${NAME} source directory")
   set(${NAME}_BINARY_DIR "${${LOWERCASE_NAME}_BINARY_DIR}" CACHE INTERNAL "${NAME} binary directory")
endfunction()