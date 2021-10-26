
cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

project(EVALUATION)

SET(READER_EXE reader)
SET(EVALUATION_EXE evaluation)

# Well, it is assumed that includes are available in /usr/local/include/dd4pod;
SET( READER_SRC
  ${PROJECT_SOURCE_DIR}/source/reader.cc

  G__evaluation.cxx
)

# FIXME: split the CINT library part off;
SET( EVALUATION_SRC
  ${PROJECT_SOURCE_DIR}/source/evaluation.cc

  G__evaluation.cxx
)

SET(EVALUATION_ROOTMAP ${CMAKE_CURRENT_BINARY_DIR}/libevaluation_rdict.pcm ${CMAKE_CURRENT_BINARY_DIR}/libevaluation.rootmap )

ROOT_GENERATE_DICTIONARY(G__evaluation dd4pod/PhotoMultiplierHitData.h dd4pod/Geant4ParticleData.h LINKDEF include/evaluationLinkDef.h)

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
ADD_EXECUTABLE( ${READER_EXE} ${READER_SRC} )
TARGET_LINK_LIBRARIES(${READER_EXE} ${ROOT_LIBRARIES} DD4pod IRT)

#ADD_EXECUTABLE( ${EVALUATION_EXE} ${EVALUATION_SRC} )
#TARGET_LINK_LIBRARIES(${EVALUATION_EXE} ${ROOT_LIBRARIES} DD4pod IRT)

#----------------------------------------------------------------------------

INSTALL(FILES   ${EVALUATION_ROOTMAP} DESTINATION bin)
install(TARGETS ${READER_EXE}         DESTINATION bin)
#install(TARGETS ${EVALUATION_EXE}     DESTINATION bin)

