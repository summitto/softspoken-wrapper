cmake_minimum_required(VERSION 3.16)

if (POLICY CMP0048)
cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)

cmake_policy(SET CMP0091 NEW)
cmake_policy(SET CMP0042 NEW)

project(emp-ot-wrapper VERSION 0.0.1)
set (CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "" FORCE)

set(PARALLEL_FETCH 4)

set(LIBOTE_BUILD_DIR ${PROJECT_BINARY_DIR}/libOTe)

set(ENABLE_SOFTSPOKEN_OT ON)
set(ENABLE_MRR ON) # MRR is base OT, there should be at least one base OT enabled
set(ENABLE_SILENTOT ON) # was needed for SoftSpoken OT for some reason?

set(ENABLE_SPAN_LITE ON) # seems to be actually required, couldn't compile without it. Used for aligned memory vectors
set(FETCH_SPAN_LITE ON) 

set(ENABLE_BOOST ON) # actually required for client/server
set(FETCH_BOOST OFF)

set(ENABLE_BITPOLYMUL ON)
set(FETCH_BITPOLYMUL ON)

if (APPLE)
  # libbitpolymul is optional, supports only x86
  set(ENABLE_BITPOLYMUL OFF)
  set(FETCH_BITPOLYMUL OFF)
  # these 2 cause libdivide to use x86 intrinsics, which are not available on Mac M1
  set(ENABLE_SSE OFF)
  set(ENABLE_AVX OFF)

  
  add_definitions(-D_LIBCPP_ENABLE_CXX17_REMOVED_UNARY_BINARY_FUNCTION)
endif()

set(FETCH_OPENSSL ON) # not really used now afaik, but can be used later for TLS encrypting socket communication
set(ENABLE_OPENSSL ON)

set(FETCH_SODIUM ON)
set(ENABLE_SODIUM ON)

set(FETCH_LIBDIVIDE ON)
set(ENABLE_LIBDIVIDE ON)

set(COPROTO_ENABLE_BOOST ON) # force coproto to use Boost ASIO, ENABLE_BOOST flag should be enough but it wasn't
set(FETCH_COPROTO ON)
set(ENABLE_COPROTO true)

set(ENABLE_PIC ON CACHE BOOL "" FORCE) # building a Node.js addon doesn't work without PIC since all of the dependencies build themselves statically for some reason

file(MAKE_DIRECTORY libOTe/out)

# build libOTe dependency
add_subdirectory(libOTe)

set(LIB_COMMON src/logger.cpp src/ot-manager.cpp)

# main wrapper library, which is linked to Go and Node.js separate libs
add_library(ot-wrapper STATIC ${LIB_COMMON})

target_include_directories(ot-wrapper PUBLIC libOTe/libOTe libOTe/cryptoTools/cryptoTools)
target_link_libraries(ot-wrapper libOTe)

option(ENABLE_GO_WRAPPER "Enable building Golang wrapper" ON)

# Node.js Addon target if used with CMake.js

if (NOT CMAKE_JS_VERSION)
  message(WARNING "Node.js addon will not be built because the configuration command isn't run with cmake-js, see README.md and package.json")
endif()

if (CMAKE_JS_VERSION)
  message(STATUS "Building Node.js addon")

  # Include Node-API wrappers
  execute_process(COMMAND node -p "require('node-addon-api').include"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE NODE_ADDON_API_DIR
  )
  string(REGEX REPLACE "[\r\n\"]" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})

  add_library(ot-wrapper-node SHARED ${CMAKE_JS_SRC} ${LIB_COMMON} node/ot.cc node/async-ot-manager.cc)
  add_definitions(-DNAPI_VERSION=8)

  # Essential include files to build a node addon,
  # You should add this line in every CMake.js based project
  target_include_directories(ot-wrapper-node PRIVATE
    ${CMAKE_JS_INC}
    ${NODE_ADDON_API_DIR}
  )
  target_include_directories(ot-wrapper-node PUBLIC src/)

  # Essential library files to link to a node addon
  # You should add this line in every CMake.js based project
  target_link_libraries(ot-wrapper-node ${CMAKE_JS_LIB} ot-wrapper)

  # Gives our library file a .node extension without any "lib" prefix
  set_target_properties(ot-wrapper-node PROPERTIES PREFIX "" SUFFIX ".node")
endif()

# Golang OT wrapper, can be built using regular CMake or from cmake-js
if (${ENABLE_GO_WRAPPER})
  message(STATUS "Building Golang wrapper")

  find_package(SWIG 4.1 COMPONENTS go REQUIRED)

  include(UseSWIG)

  set(SWIG_INTERFACE go/ot-wrapper.i)
  set_property(SOURCE ${SWIG_INTERFACE} PROPERTY CPLUSPLUS ON) # enable C++ mode for Swig
  set_property(SOURCE ${SWIG_INTERFACE} PROPERTY OUTPUT_DIR ${CMAKE_SOURCE_DIR}/pkg/) # generated file output dir
  set_property(SOURCE ${SWIG_INTERFACE} PROPERTY OUTFILE_DIR swig-gen) # separate directory for C++ swig wrappers, recommended to be separate from the rest of the build
  set_property(SOURCE ${SWIG_INTERFACE} PROPERTY SWIG_MODULE_NAME pkg) # generated go file package name
  # include directories to pass to C++ compiler so that the generated C++ wrapper could find the sources we're wrapping, i.e. the OTManager class and it's Go adapter in go directory
  set_property(SOURCE ${SWIG_INTERFACE} PROPERTY GENERATED_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/src/ ${CMAKE_CURRENT_SOURCE_DIR}/go/)
  set_property(SOURCE ${SWIG_INTERFACE} PROPERTY UseSWIG_MODULE_VERSION 2)

  swig_add_library(libOtGo TYPE SHARED LANGUAGE go SOURCES go/ot-wrapper.i) # this step will generate Go wrapper in pkg directory
  swig_link_libraries(libOtGo ot-wrapper)

  # after the shared lib is built, copy it to pkg directory
  add_custom_command(
    TARGET libOtGo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/libOtGo${CMAKE_SHARED_LIBRARY_SUFFIX}
            ${CMAKE_SOURCE_DIR}/pkg/libOtGo${CMAKE_SHARED_LIBRARY_SUFFIX}
  )
endif()
