cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

set(PROJECT glfw)

set(PROJECT_PATH ${THIRD_PARTY_PATH}/${PROJECT})

set(LIB "")
if(WIN32)
  set(LIB libglfw3_x86.lib)
elseif(UNIX AND NOT APPLE)
  set(LIB libglfw3.so)
elseif(APPLE)
  set(LIB libglfw3_osx.a)
endif()

set(THIRD_PARTY_INCLUDE_DIRS ${THIRD_PARTY_INCLUDE_DIRS} 
	${PROJECT_PATH}/include 
	PARENT_SCOPE)
set(THIRD_PARTY_LIBS ${THIRD_PARTY_LIBS} 
	${PROJECT_PATH}/lib/${LIB}
	PARENT_SCOPE)

