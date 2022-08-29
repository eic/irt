cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

project(IRT
  VERSION 1.0
  LANGUAGES CXX
)

cmake_policy(SET CMP0079 NEW) # allow `target_link_libraries` from any dir

#----------------------------------------------------------------------------
# options
option(EVALUATION "Build evaluation programs" OFF)
option(DELPHES    "Delphes card production"   ON)

#----------------------------------------------------------------------------
# dependencies

find_package(ROOT 6.0.00 REQUIRED COMPONENTS RIO Hist Tree ROOTDataFrame)
include(${ROOT_USE_FILE})

#----------------------------------------------------------------------------
# flags

include(CheckCXXCompilerFlag)

set(CMAKE_CXX_FLAGS "-std=c++0x")

check_cxx_compiler_flag(-std=c++17 HAVE_FLAG_STD_CXX17)
if(HAVE_FLAG_STD_CXX17)
  # Have -std=c++17, use it;	
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
else()
  check_cxx_compiler_flag(-std=c++11 HAVE_FLAG_STD_CXX11)
  if(HAVE_FLAG_STD_CXX11)
    # Have -std=c++11, use it;	
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  endif()
endif()

#----------------------------------------------------------------------------
# IRT library

set(IRT_LIB IRT)

include_directories( 
  ${PROJECT_SOURCE_DIR}/include 
  ${ROOT_INCLUDE_DIRS}

  # For now assume that newly installed edm4eic::(CherenkovPID, ...) event structures are available 
  # in the same place where IRT is supposed to be installed;
  ${CMAKE_INSTALL_PREFIX}/include 
)
file(GLOB HEADERS ${PROJECT_SOURCE_DIR}/include/*.h )

set(IRT_SRC
  ${PROJECT_SOURCE_DIR}/source/SphericalSurface.cc
  ${PROJECT_SOURCE_DIR}/source/FlatSurface.cc
  ${PROJECT_SOURCE_DIR}/source/IRT.cc
  ${PROJECT_SOURCE_DIR}/source/ChargedParticle.cc
  G__IRT.cxx
)

set(IRT_ROOTMAP ${CMAKE_CURRENT_BINARY_DIR}/lib${IRT_LIB}_rdict.pcm ${CMAKE_CURRENT_BINARY_DIR}/lib${IRT_LIB}.rootmap )
root_generate_dictionary(G__IRT ${HEADERS} LINKDEF irtLinkDef.h)

add_library(${IRT_LIB} SHARED ${IRT_SRC} )
target_link_libraries(${IRT_LIB} ${ROOT_LIBRARIES} )
target_compile_options(${IRT_LIB} PRIVATE -Wall -Wno-misleading-indentation)
target_include_directories(${IRT_LIB} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<INSTALL_INTERFACE:include>
)

#----------------------------------------------------------------------------
# optional targets

if(EVALUATION)
  add_subdirectory(evaluation)
endif()

if(DELPHES)
  add_subdirectory(delphes)
endif()

#----------------------------------------------------------------------------
# installation

install(FILES
  ${HEADERS}
  DESTINATION include/IRT
)
install(TARGETS ${IRT_LIB}
  EXPORT ${PROJECT_NAME}Targets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include/IRT
)
install(FILES
  ${IRT_ROOTMAP}
  DESTINATION lib
)

#----------------------------------------------------------------------------
# package config

include(CMakePackageConfigHelpers)
install(EXPORT ${PROJECT_NAME}Targets
  FILE ${PROJECT_NAME}Targets.cmake
  DESTINATION lib/IRT
)

set(TARGETS_INSTALL_PATH lib/IRT/IRTTargets.cmake)

configure_package_config_file(
  cmake/IRTConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/IRTConfig.cmake
  INSTALL_DESTINATION lib/IRT
  PATH_VARS TARGETS_INSTALL_PATH
)

write_basic_package_version_file(
  "IRTConfigVersion.cmake"
  VERSION ${VERSION}
  COMPATIBILITY SameMajorVersion
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/IRTConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/IRTConfigVersion.cmake
  DESTINATION lib/IRT
)

