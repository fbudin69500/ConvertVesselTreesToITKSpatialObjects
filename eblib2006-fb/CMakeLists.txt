# This is the root FPTK CMakeLists file.
CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

PROJECT(eblib2006-fb)

# If CMake is the version we're expecting, don't show the
# CMAKE_BACKWARDS_COMPATIBILITY option. If the version is higher than the
# minimal version required, then show the backward compatibility option.
IF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" MATCHES "^2\\.4$")
  MARK_AS_ADVANCED(FORCE CMAKE_BACKWARDS_COMPATIBILITY)
ELSE("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" MATCHES "^2\\.4$")
  MARK_AS_ADVANCED(CLEAR CMAKE_BACKWARDS_COMPATIBILITY)
ENDIF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" MATCHES "^2\\.4$")


FIND_PACKAGE(FLTK REQUIRED)

SET(MetaImageLib2007-fb_DIR "${MetaImageLib2007-fb_BINARY_DIR}" CACHE PATH "MetaImageLib2007-fb dir")

IF(NOT MetaImageLib2007-fb_DIR)
  MESSAGE(SEND_ERROR "MetaImageLib2007-fb path is required")
ELSE(NOT MetaImageLib2007-fb_DIR)
  INCLUDE(${MetaImageLib2007-fb_DIR}/MetaImageLib2007-fb-Config.cmake)
  INCLUDE(${MetaImageLib2007-fb_DIR}/UseMetaImageLib2007-fb.cmake)
ENDIF(NOT MetaImageLib2007-fb_DIR)

INCLUDE_DIRECTORIES(
  ${FLTK_INCLUDE_DIR}
  )
#INCLUDE_DIRECTORIES(${MetaImageLib2007_BINARY_DIR})
SET(eblib2006-fb_SRCS
#ebDEFINE.h
#ebANGIOV.h
#ebANGIOV.cpp
ebARLIST.h
ebARLIST.cpp
#ebARRAYT.h
#ebARRAYT.cpp
ebFILEHEADERIO.h
ebFILEHEADERIO.cpp
ebFVEC.h
ebFVEC.cpp
#ebGEOM.h
#ebGEOM.cpp
ebINCLUDES.h
#ebINTCURVE.h
#ebINTCURVE.cpp
#eblib.h
ebLIST.h
ebLIST.cpp
#ebMATH.h
#ebMATH.cpp
#ebMESH.h
#ebMESH.cpp
#ebMESH_s.h
#ebMESH_s.cpp
#ebMESHCOL.h
#ebMESHCOL.cpp
#ebMESHCOL_s.h
#ebMESHCOL_s.cpp
#ebMESHRENDER.h
#ebMESHRENDER.cpp
#ebMESHRENDER_s.h
#ebMESHRENDER_s.cpp
ebOBJECTIO.h
ebOBJECTIO.cpp
#ebOBJECTIO_s.h
#ebOBJECTIO_s.cpp
#ebOBJECTVR.h
#ebOBJECTVR.cpp
#ebOMAT.h
#ebOMAT.cpp
#ebRESCALET.h
#ebRESCALET.cpp
#ebSLICEWIN.h
#ebSLICEWIN.cpp
#ebSTAT.h
#ebSTAT.cpp
#ebSURFACE.h
#ebSURFACE.cpp
#ebSURFACECOL.h
#ebSURFACECOL.cpp
ebTBOX.h
ebTBOX.cpp
#ebTBOX_s.h
#ebTBOX_s.cpp
#ebVEC.h
#ebVEC.cpp
ebVESS.h
ebVESS.cpp
#ebVESS_s.h
#ebVESS_s.cpp
ebVESSCOL.h
ebVESSCOL.cpp
#ebVESSCOL_s.h
#ebVESSCOL_s.cpp
ebVESSCOLUTILS.h
ebVESSCOLUTILS.cpp
#ebVESSCOLUTILS_s.h
#ebVESSCOLUTILS_s.cpp
#ebVESSVR.h
#ebVESSVR.cpp
#ebZBUFF.h
)

#IF (WIN32)
#    ADD_DEFINITIONS (/D _CRT_SECURE_NO_WARNINGS)
#ENDIF (WIN32)



LINK_LIBRARIES(
  ${FLTK_LIBRARY}
)

ADD_LIBRARY(eblib2006-fb ${eblib2006-fb_SRCS})

SET(eblib2006-fb_DIR "${eblib2006-fb_BINARY_DIR}" CACHE INTERNAL "eblib2006-fb dir to be used by subprojects")


SET(eblib2006-fb_LIBRARIES eblib2006-fb)
SET(eblib2006-fb_LIBRARY_DIR ${eblib2006-fb_BINARY_DIR})
SET(eblib2006-fb_INCLUDE_DIR ${eblib2006-fb_SOURCE_DIR})

#-----------------Copy the configuration files-------------------
CONFIGURE_FILE(${eblib2006-fb_SOURCE_DIR}/CMake/eblib2006-fb-Config.cmake.in
               ${eblib2006-fb_BINARY_DIR}/eblib2006-fb-Config.cmake @ONLY)
CONFIGURE_FILE(${eblib2006-fb_SOURCE_DIR}/CMake/Useeblib2006-fb.cmake.in
                ${eblib2006-fb_BINARY_DIR}/Useeblib2006-fb.cmake  COPYONLY IMMEDIATE)
