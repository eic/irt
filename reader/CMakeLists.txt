
cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

project(READER)

SET(EXE reader)

# Well, it is assumed that includes are available in /usr/local/include/dd4pod;
SET( READER_SRC
  ${PROJECT_SOURCE_DIR}/reader.cc

  G__reader.cxx
)

SET(READER_ROOTMAP ${CMAKE_CURRENT_BINARY_DIR}/libreader_rdict.pcm ${CMAKE_CURRENT_BINARY_DIR}/libreader.rootmap )

ROOT_GENERATE_DICTIONARY(G__reader dd4pod/PhotoMultiplierHitData.h dd4pod/Geant4ParticleData.h LINKDEF readerLinkDef.h)

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
ADD_EXECUTABLE( ${EXE} ${READER_SRC} )
TARGET_LINK_LIBRARIES(${EXE} ${ROOT_LIBRARIES} DD4pod irt)

#----------------------------------------------------------------------------

INSTALL(FILES   ${READER_ROOTMAP} DESTINATION bin)
install(TARGETS reader            DESTINATION bin)

