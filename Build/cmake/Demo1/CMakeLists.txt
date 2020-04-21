set(NOW_PROJECT_NAME Demo1)

if(CMAKE_CL_64)
set(NOW_x86_x64 "x64")
else(CMAKE_CL_64)
set(NOW_x86_x64 "x86")
endif(CMAKE_CL_64)

IF(UNIX)
	IF(RELEASE_MODE)
	set(CMAKE_BUILD_TYPE "Release")
	MESSAGE(STATUS "This is Release Mode")
	ELSE(RELEASE_MODE)
	set(CMAKE_BUILD_TYPE "Debug")
	MESSAGE(STATUS "This is Debug Mode")
	ENDIF(RELEASE_MODE)
set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} "/usr/local")
#ARM版优化指令
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC -optimize-sibling-calls -fsigned-char")
ENDIF(UNIX)

include_directories(
../../../Core/MagicThread
)

set(SOURCE_FILES
${PROJECT_SOURCE_DIR}../../../Core/Demo1/main.cpp)

IF(WIN32)
	link_directories(
		${PROJECT_SOURCE_DIR}../../../lib/${CMAKE_SYSTEM_NAME}/${NOW_x86_x64})
	link_libraries(
		debug MagicThread_d.lib optimized MagicThread.lib)
ENDIF(WIN32)
IF(UNIX)
	#find_library(MAGIC_THREAD_LIBRARY MagicThread_d HINTS /home/CXX/MagicThread/bin/Linux/x86/)
	link_directories(
		${PROJECT_SOURCE_DIR}/../../bin/${CMAKE_SYSTEM_NAME}/${NOW_x86_x64})
	#link_libraries(
	#	debug -lMagicThread_d.so optimized -lMagicThread.so)
ENDIF(UNIX)

IF(WIN32)
	add_definitions(-DUNICODE -D_UNICODE)
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE")
ENDIF(WIN32)

ADD_EXECUTABLE (${NOW_PROJECT_NAME} ${SOURCE_FILES})
IF(UNIX)
	target_link_libraries(${NOW_PROJECT_NAME} debug -lMagicThread_d optimized -lMagicThread)
ENDIF(UNIX)

#RUNTIME	.exe、.dll	可执行档
SET_TARGET_PROPERTIES(${NOW_PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../../../bin/${CMAKE_SYSTEM_NAME}/${NOW_x86_x64} OUTPUT_NAME_DEBUG ${NOW_PROJECT_NAME}_d)
#LIBRARY		.so
#SET_TARGET_PROPERTIES(${NOW_PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ../../../lib)
#ARCHIVE	.a、.lib包含 dll 的连结介面	.a
SET_TARGET_PROPERTIES(${NOW_PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ../../../lib/${CMAKE_SYSTEM_NAME}/${NOW_x86_x64} OUTPUT_NAME_DEBUG ${NOW_PROJECT_NAME}_d)

add_dependencies(${NOW_PROJECT_NAME} MagicThread)
