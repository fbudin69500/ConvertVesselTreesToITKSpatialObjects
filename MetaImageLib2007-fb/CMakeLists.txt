# This is the root FPTK CMakeLists file.
CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

PROJECT(MetaImageLib2007-fb)

# If CMake is the version we're expecting, don't show the
# CMAKE_BACKWARDS_COMPATIBILITY option. If the version is higher than the
# minimal version required, then show the backward compatibility option.
IF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" MATCHES "^2\\.4$")
  MARK_AS_ADVANCED(FORCE CMAKE_BACKWARDS_COMPATIBILITY)
ELSE("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" MATCHES "^2\\.4$")
  MARK_AS_ADVANCED(CLEAR CMAKE_BACKWARDS_COMPATIBILITY)
ENDIF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" MATCHES "^2\\.4$")

SET(MetaImageLib2007-fb_SRCS
BMPLib.h
BMPLib.cpp
MetaFileLib.h
MetaFileLib.cpp
MetaImageLib.h
MetaImageLib.cpp
MetaImageTypes.h
MetaUtils.h
MetaUtils.cpp
)

IF (WIN32)
    ADD_DEFINITIONS (/D _CRT_SECURE_NO_WARNINGS)
ENDIF (WIN32)

ADD_LIBRARY(MetaImageLib2007-fb ${MetaImageLib2007-fb_SRCS})

SET(MetaImageLib2007-fb_LIBRARIES MetaImageLib2007-fb)
SET(MetaImageLib2007-fb_LIBRARY_DIR ${MetaImageLib2007-fb_BINARY_DIR})
SET(MetaImageLib2007-fb_INCLUDE_DIR ${MetaImageLib2007-fb_SOURCE_DIR})

SET(MetaImageLib2007-fb_DIR "${MetaImageLib2007-fb_BINARY_DIR}" CACHE INTERNAL "MetaImageLib2007-fb dir to be used by subprojects")

#-----------------Copy the configuration files-------------------
CONFIGURE_FILE(${MetaImageLib2007-fb_SOURCE_DIR}/CMake/MetaImageLib2007-fb-Config.cmake.in
               ${MetaImageLib2007-fb_BINARY_DIR}/MetaImageLib2007-fb-Config.cmake @ONLY)
CONFIGURE_FILE(${MetaImageLib2007-fb_SOURCE_DIR}/CMake/UseMetaImageLib2007-fb.cmake.in
                ${MetaImageLib2007-fb_BINARY_DIR}/UseMetaImageLib2007-fb.cmake  COPYONLY IMMEDIATE)