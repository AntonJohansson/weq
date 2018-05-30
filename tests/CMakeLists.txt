enable_testing()

if(WIN32)
  set(EXTERNAL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../external/x64)
  #set(EXTERNAL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../external/x32)
  set(library_path ${EXTERNAL_PATH}/lib)
  link_directories(${EXTERNAL_PATH}/lib)
elseif(APPLE)
  set(CMAKE_CXX_COMPILER "g++-7")
  set(EXTERNAL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../external/osx)
  set(library_path ${EXTERNAL_PATH}/lib)
  link_directories(${EXTERNAL_PATH}/lib)
  find_library(cocoa Cocoa REQUIRED)
  find_library(iokit IOKit REQUIRED)
  find_library(corevideo CoreVideo REQUIRED)
  set(EXTRA_LIBS "stdc++fs" ${cocoa} ${iokit} ${corevideo})

  add_definitions(-DGLM_ENABLE_EXPERIMENTAL)
endif()


macro(create_test TARGET_NAME SOURCE)
  add_executable(${TARGET_NAME} ${SOURCE}
    ../src/weq/ecs/Event.cpp
    ../src/weq/ecs/System.cpp
    ../src/weq/ecs/Component.cpp
    )

    if(WIN32)
      target_compile_options(${TARGET_NAME} PRIVATE /std:c++latest)
    else()
      target_compile_options(${TARGET_NAME} PRIVATE -std=c++1z -Wall -Wextra -Wfatal-errors -g)
    endif()

    target_include_directories(${TARGET_NAME} PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
      $<BUILD_INTERFACE:${EXTERNAL_PATH}/include>
      )

    # Special case for benchmark tests
    add_test(${TARGET_NAME} ${TARGET_NAME})
endmacro()

create_test(pool_test   PoolAllocatorTest.cpp )
create_test(signal_test SignalTest.cpp        )
create_test(event_test  EventTest.cpp         )
create_test(em_test     EntityManagerTest.cpp )
create_test(list_test   CircularListTest.cpp  )
create_test(sm_test     SystemTest.cpp        )
#create_test(sch_test    SchedulerTest.cpp     )
