cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

# CMP0074: find_package() uses <PackageName>_ROOT variables
cmake_policy(SET CMP0074 NEW)

PROJECT(athena)

set(CMAKE_CXX_STANDARD 17)
find_package( DD4hep REQUIRED COMPONENTS DDCore DDG4)

SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-int-to-pointer-cast")

#-----------------------------------------------------------------------------------

include_directories( 
  ${IRT}/include/irt 
)

#-----------------------------------------------------------------------------------

set(a_lib_name athena)

dd4hep_configure_output()

dd4hep_add_plugin(${a_lib_name} SOURCES 
  src/ERich_geo.cpp
  )

target_link_libraries(${a_lib_name}
  PUBLIC DD4hep::DDCore -L${IRT}/lib irt
  )

#-----------------------------------------------------------------------------------
# Install the detector description files.
install(DIRECTORY compact/
    DESTINATION share/${PROJECT_NAME}/compact
    FILES_MATCHING PATTERN "*.xml"
    )
install(FILES athena.xml
    DESTINATION share/${PROJECT_NAME}/
    )
