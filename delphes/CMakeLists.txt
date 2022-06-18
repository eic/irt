
cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

project(DELPHES)

set(DELPHES_LIB DELPHES)

SET( DELPHES_SRC
  source/DelphesConfig.cc
  source/DelphesConfigRICH.cc
  source/DelphesConfigTOF.cc
  source/DelphesConfigDIRC.cc
  source/DrcPidFast.cxx

  G__${DELPHES_LIB}.cxx
)

include_directories( 
  ${PROJECT_SOURCE_DIR}/include 
)
  
SET(DELPHES_ROOTMAP ${CMAKE_CURRENT_BINARY_DIR}/lib${DELPHES_LIB}_rdict.pcm ${CMAKE_CURRENT_BINARY_DIR}/lib${DELPHES_LIB}.rootmap )

ROOT_GENERATE_DICTIONARY(G__${DELPHES_LIB} DelphesConfigRICH.h DelphesConfigTOF.h DelphesConfigDIRC.h LINKDEF include/delphesLinkDef.h)

ADD_LIBRARY( ${DELPHES_LIB} SHARED ${DELPHES_SRC})
TARGET_LINK_LIBRARIES(${DELPHES_LIB} ${ROOT_LIBRARIES} EG )

#----------------------------------------------------------------------------

INSTALL(FILES   ${DELPHES_ROOTMAP} DESTINATION lib)
install(TARGETS ${DELPHES_LIB}     DESTINATION lib)

