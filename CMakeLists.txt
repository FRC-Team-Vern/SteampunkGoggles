cmake_minimum_required (VERSION 2.8)
project (SteampunkGoogles)
add_definitions(-std=c++1y -Wall -Wextra -Wno-switch)

if (${CMAKE_LIBRARY_ARCHITECTURE} STREQUAL "arm-linux-gnueabihf")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -mcpu=cortex-a15 -mfpu=neon-vfpv4")
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -mcpu=cortex-a15 -mfpu=neon-vfpv4")
	add_definitions(-DIS_JETSON=1)
endif ()

set (CUDA_USE_STATIC_CUDA_RUNTIME OFF)

find_package (OpenCV 3.1 REQUIRED)

# For the shared libraries
set (NTCORE_LINK_LIBS libntcore.so)
set (CSCORE_LINK_LIBS libcscore.so)
set (WPIUTIL_LINK_LIBS libwpiutil.so)
link_directories (~/NetworkTables-3.1.7-armhf/Linux/arm)
link_directories (~/cscore-1.0.2-armhf/Linux/arm)
link_directories (~/wpiutil-1.0.2-armhf/Linux/arm)

include_directories (
	~/allwpilib/wpilibc/athena/include
	~/allwpilib/wpilibc/shared/include
	~/allwpilib/hal/include
	~/NetworkTables-3.1.7-armhf/include
	~/cscore-1.0.2-armhf/include
	~/wpiutil-1.0.2-armhf/include
	${OpenCV_INCLUDE_DIRS})
	
set (INCLUDE_SRC src/CameraServerCustom.cpp)
set (INCLUDE_SRC ${INCLUDE_SRC} src/GreyScalePublish.cpp)

add_executable (streampunk_goggles src/main.cpp ${INCLUDE_SRC})

target_link_libraries (streampunk_goggles ${NTCORE_LINK_LIBS} ${CSCORE_LINK_LIBS} ${WPIUTIL_LINK_LIBS} ${OpenCV_LIBS})
