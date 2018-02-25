enable_testing()

macro(create_test TARGET_NAME SOURCE)
    add_executable(${TARGET_NAME} ${SOURCE})

    if(WIN32)
      target_compile_options(${TARGET_NAME} PRIVATE /std:c++latest)
    else()
      target_compile_options(${TARGET_NAME} PRIVATE -stdlib=libc++ -std=c++1z -Wall -Wextra -Wfatal-errors -g)
    endif()

    target_include_directories(${TARGET_NAME} PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
      )

    # Special case for benchmark tests
    add_test(${TARGET_NAME} ${TARGET_NAME})
endmacro()

create_test(pool_test   PoolAllocatorTest.cpp)
create_test(signal_test SignalTest.cpp)
create_test(event_test  EventTest.cpp)
create_test(em_test     EntityManagerTest.cpp)
create_test(list_test   CircularListTest.cpp)
create_test(sm_test     SystemTest.cpp)
