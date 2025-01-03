# Copyright 2018 The Min-DevKit Authors. All rights reserved.
# Use of this source code is governed by the MIT License found in the License.md file.

cmake_minimum_required(VERSION 3.30)

set(C74_MIN_API_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../min-api)
set(CMAKE_OSX_ARCHITECTURES x86_64;arm64)
include(${C74_MIN_API_DIR}/script/min-pretarget.cmake)
set(BUILD_SHARED_LIBS FALSE)
add_subdirectory(hidapi)
#find_package(hidapi REQUIRED)

#############################################################
# MAX EXTERNAL
#############################################################


include_directories(
	"${C74_INCLUDES}"
)




set( SOURCE_FILES
	${PROJECT_NAME}.cpp
	Manta.cpp
		MantaMulti.cpp
		MantaUSB.cpp
)


add_library( 
	${PROJECT_NAME} 
	MODULE
	${SOURCE_FILES}
)


target_link_libraries(${PROJECT_NAME} PRIVATE hidapi::hidapi)


include(${C74_MIN_API_DIR}/script/min-posttarget.cmake)


#############################################################
# UNIT TEST
#############################################################

include(${C74_MIN_API_DIR}/test/min-object-unittest.cmake)
target_link_libraries(${TEST_NAME}  PRIVATE hidapi::hidapi)
