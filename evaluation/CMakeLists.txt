
cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

project(EVALUATION)

set(IRTEVAL_LIB IRTEVAL)

SET(READER_EXE reader)
SET(EVALUATION_EXE evaluation)

SET( IRTEVAL_SRC
  G__IRTEVAL.cxx
)

# Well, it is assumed that includes are available in /usr/local/include/dd4pod;
SET( READER_SRC
  ${PROJECT_SOURCE_DIR}/source/reader.cc
)

SET( EVALUATION_SRC
  ${PROJECT_SOURCE_DIR}/source/evaluation.cc
)

SET(EVALUATION_ROOTMAP ${CMAKE_CURRENT_BINARY_DIR}/lib${IRTEVAL_LIB}_rdict.pcm ${CMAKE_CURRENT_BINARY_DIR}/lib${IRTEVAL_LIB}.rootmap )

ROOT_GENERATE_DICTIONARY(G__${IRTEVAL_LIB} eicd/CherenkovPdgHypothesis.h eicd/CherenkovThetaAngleMeasurement.h eicd/CherenkovParticleIDData.h dd4pod/PhotoMultiplierHitData.h dd4pod/Geant4ParticleData.h LINKDEF include/evaluationLinkDef.h)

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
ADD_EXECUTABLE( ${READER_EXE} ${READER_SRC} )
TARGET_LINK_LIBRARIES(${READER_EXE} ${ROOT_LIBRARIES} DD4pod IRT IRTEVAL)

ADD_EXECUTABLE( ${EVALUATION_EXE} ${EVALUATION_SRC} )
TARGET_LINK_LIBRARIES(${EVALUATION_EXE} ${ROOT_LIBRARIES} DD4pod IRT IRTEVAL)

ADD_LIBRARY( ${IRTEVAL_LIB} SHARED ${IRTEVAL_SRC})
TARGET_LINK_LIBRARIES(${IRTEVAL_LIB} ${ROOT_LIBRARIES} DD4pod )

#----------------------------------------------------------------------------

INSTALL(FILES   ${EVALUATION_ROOTMAP} DESTINATION lib)
install(TARGETS ${READER_EXE}         DESTINATION bin)
install(TARGETS ${EVALUATION_EXE}     DESTINATION bin)
install(TARGETS ${IRTEVAL_LIB}        DESTINATION lib)

