
cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

cmake_policy(SET CMP0079 NEW) # allow `target_link_libraries` from any dir

project(IRT)

#----------------------------------------------------------------------------

find_package(ROOT 6.0.00 REQUIRED COMPONENTS RIO Hist Tree ROOTDataFrame)
include(${ROOT_USE_FILE})

#----------------------------------------------------------------------------

include(CheckCXXCompilerFlag)

SET(CMAKE_CXX_FLAGS "-std=c++0x")

check_cxx_compiler_flag(-std=c++17 HAVE_FLAG_STD_CXX17)
if(HAVE_FLAG_STD_CXX17)
  # Have -std=c++17, use it;	
  SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
else()
  check_cxx_compiler_flag(-std=c++11 HAVE_FLAG_STD_CXX11)
  if(HAVE_FLAG_STD_CXX11)
    # Have -std=c++17, use it;	
    SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  endif()
endif()

#----------------------------------------------------------------------------
#
include_directories( 
  ${PROJECT_SOURCE_DIR}/include 

  ${ROOT_INCLUDE_DIRS}
)

file(GLOB HEADERS ${PROJECT_SOURCE_DIR}/include/*.h )

SET(IRT_LIB irt)

SET( IRT_SRC
   ${PROJECT_SOURCE_DIR}/source/ParametricSurface.cc
   ${PROJECT_SOURCE_DIR}/source/IRT.cc
   ${PROJECT_SOURCE_DIR}/source/ChargedParticle.cc

   G__irt.cxx
)

SET(IRT_ROOTMAP ${CMAKE_CURRENT_BINARY_DIR}/lib${IRT_LIB}_rdict.pcm ${CMAKE_CURRENT_BINARY_DIR}/lib${IRT_LIB}.rootmap )

ROOT_GENERATE_DICTIONARY(G__irt ${HEADERS} LINKDEF irtLinkDef.h)

ADD_LIBRARY( ${IRT_LIB} SHARED ${IRT_SRC} )
TARGET_LINK_LIBRARIES(${IRT_LIB} ${ROOT_LIBRARIES} )
TARGET_COMPILE_OPTIONS(${IRT_LIB} PRIVATE -Wall -Wno-misleading-indentation)

#----------------------------------------------------------------------------

SET(ignoreMe "${READER}")
if(DEFINED READER)
  add_subdirectory(reader)
endif()

SET(ignoreMe "${ATHENA}")
if(DEFINED ATHENA)
  add_compile_definitions(IRTGEO)
  add_subdirectory(athena)
  target_link_libraries(athena PUBLIC -L${IRT}/lib irt)
endif()

#----------------------------------------------------------------------------

install(TARGETS ${IRT_LIB}     DESTINATION lib)
INSTALL(FILES   ${IRT_ROOTMAP} DESTINATION lib)
INSTALL(FILES   ${HEADERS}     DESTINATION include/irt)

