CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
CMAKE_POLICY(VERSION 2.6)

PROJECT(ConvertVesselsTreesToMask)

  FIND_PACKAGE(ITK REQUIRED)
   IF(USE_ITK_FILE)
     INCLUDE(${USE_ITK_FILE})
   ENDIF(USE_ITK_FILE)
  SET(ITK_LIBRARIES
    ITKCommon
    ITKIO
    ITKNumerics
    ITKBasicFilters
    ITKAlgorithms
    )

SET(eblib2006-fb_DIR "${eblib2006-fb_BINARY_DIR}" CACHE PATH "eblib2006-fb dir")


IF(NOT eblib2006-fb_DIR)
  MESSAGE(SEND_ERROR "eblib2006-fb path is required")
ELSE(NOT eblib2006-fb_DIR)
  INCLUDE(${eblib2006-fb_DIR}/eblib2006-fb-Config.cmake)
  INCLUDE(${eblib2006-fb_DIR}/Useeblib2006-fb.cmake)
ENDIF(NOT eblib2006-fb_DIR)	
	
SET(ConvertVesselsTreesToMask_SRCS
ConvertVesselsTreesToMask.cpp
VesselsClass.cpp
VesselsClass.h
  )
   
LINK_LIBRARIES(
  ${ITK_LIBRARIES}
 )
   
 
ADD_EXECUTABLE(ConvertVesselsTreesToMask ${ConvertVesselsTreesToMask_SRCS})
