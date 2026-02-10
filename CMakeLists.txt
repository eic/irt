cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

project(IRT
  VERSION 2.1.0
  LANGUAGES CXX
)

cmake_policy(SET CMP0079 NEW) # allow `target_link_libraries` from any dir

#----------------------------------------------------------------------------
# options
option(EVALUATION  "Build evaluation programs"                          OFF)
option(DELPHES     "Delphes card production"                            ON)
option(IRT_ROOT_IO "Generate dictionary for ROOT I/O of libIRT objects" ON)
option(JSON_EXPORT "Be able to export calibrations in JSON format"      OFF)

#----------------------------------------------------------------------------
# dependencies

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(ROOT 6 REQUIRED COMPONENTS Core EG Hist MathCore RIO Tree)
include(${ROOT_USE_FILE})

#----------------------------------------------------------------------------
# flags

include(GNUInstallDirs) # standard GNU installation
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

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-vla-cxx-extension")
    
#----------------------------------------------------------------------------
# IRT library

# headers
include_directories(
  ${PROJECT_SOURCE_DIR}/include 
  ${ROOT_INCLUDE_DIRS}

  # For now assume that newly installed edm4eic::(CherenkovPID, ...) event structures are available 
  # in the same place where IRT is supposed to be installed;
  ${CMAKE_INSTALL_PREFIX}/include
)
file(GLOB HEADERS ${PROJECT_SOURCE_DIR}/include/*.h)
list(FILTER HEADERS EXCLUDE REGEX "LinkDef\\.h$")

#if(DEFINED EICRECON)
#   set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DEICRECON")
#endif()

# sources
set(IRT_SRC
  ${PROJECT_SOURCE_DIR}/source/CherenkovDetector.cc
  ${PROJECT_SOURCE_DIR}/source/CherenkovDetectorCollection.cc
  
  ${PROJECT_SOURCE_DIR}/source/ParametricSurface.cc
  ${PROJECT_SOURCE_DIR}/source/SphericalSurface.cc
  ${PROJECT_SOURCE_DIR}/source/ToricSurface.cc
  ${PROJECT_SOURCE_DIR}/source/CylindricalSurface.cc
  ${PROJECT_SOURCE_DIR}/source/ConicalSurface.cc
  ${PROJECT_SOURCE_DIR}/source/FlatSurface.cc
  ${PROJECT_SOURCE_DIR}/source/IRT.cc
  ${PROJECT_SOURCE_DIR}/source/ChargedParticle.cc
  ${PROJECT_SOURCE_DIR}/source/CherenkovRadiator.cc

  ${PROJECT_SOURCE_DIR}/source/DigitizedHit.cc
  ${PROJECT_SOURCE_DIR}/source/GeantImport.cc
  #${PROJECT_SOURCE_DIR}/source/Configuration.cc
  ${PROJECT_SOURCE_DIR}/source/Calibration.cc
  ${PROJECT_SOURCE_DIR}/source/Digitization.cc
  ${PROJECT_SOURCE_DIR}/source/ReconstructionFactory.cc
)
if(IRT_ROOT_IO)
  set(IRT_DICT G__${CMAKE_PROJECT_NAME})
  list(APPEND IRT_SRC ${IRT_DICT}.cxx)
endif()

# library target
add_library(${CMAKE_PROJECT_NAME} SHARED ${IRT_SRC} )
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wall -Wno-misleading-indentation)
if(NOT IRT_ROOT_IO)
  # disable calling of `ClassDef` etc. if not generating ROOT dictionary
  message(STATUS "NOTE: disabling ROOT dictionary generation")
  target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC DISABLE_ROOT_IO)
endif()
IF(JSON_EXPORT)
  message(STATUS "NOTE: JSON_EXPORT enabled")
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DJSON_EXPORT")
  find_package(nlohmann_json 3.12.0 REQUIRED)
  target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
ENDIF()
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<INSTALL_INTERFACE:include>
)

# ROOT I/O dictionary generation
if(IRT_ROOT_IO)
  set(IRT_ROOTMAP
    ${CMAKE_CURRENT_BINARY_DIR}/lib${CMAKE_PROJECT_NAME}_rdict.pcm
    ${CMAKE_CURRENT_BINARY_DIR}/lib${CMAKE_PROJECT_NAME}.rootmap
  )
  root_generate_dictionary(${IRT_DICT} ${HEADERS} LINKDEF include/irtLinkDef.h)
endif()

# linking
target_link_libraries(${CMAKE_PROJECT_NAME} ROOT::Core ROOT::EG ROOT::Hist ROOT::RIO ROOT::MathCore ROOT::Tree)

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
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_PROJECT_NAME}
)
install(TARGETS ${CMAKE_PROJECT_NAME}
  EXPORT ${CMAKE_PROJECT_NAME}Targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_PROJECT_NAME}
)
if(IRT_ROOT_IO)
  install(FILES
    ${IRT_ROOTMAP}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )
endif()

#----------------------------------------------------------------------------
# package config

include(CMakePackageConfigHelpers)

install(EXPORT ${CMAKE_PROJECT_NAME}Targets
  FILE ${CMAKE_PROJECT_NAME}Targets.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}
)

set(TARGETS_INSTALL_PATH ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}/${CMAKE_PROJECT_NAME}Targets.cmake)

configure_package_config_file(
  cmake/${CMAKE_PROJECT_NAME}Config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}
  PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR TARGETS_INSTALL_PATH
)

write_basic_package_version_file(
  "${CMAKE_PROJECT_NAME}ConfigVersion.cmake"
  VERSION ${VERSION}
  COMPATIBILITY SameMajorVersion
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}ConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}
)

