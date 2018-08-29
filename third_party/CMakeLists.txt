set(THIRD_PARTY_PATH ${CMAKE_SOURCE_DIR}/third_party)

add_subdirectory(glad)
add_subdirectory(imgui)
add_subdirectory(glm)
add_subdirectory(spdlog)
add_subdirectory(glfw)

set(THIRD_PARTY_INCLUDE_DIRS 
	${THIRD_PARTY_INCLUDE_DIRS} 
	PARENT_SCOPE)
set(THIRD_PARTY_LIBS
	${THIRD_PARTY_LIBS} 
	PARENT_SCOPE)
