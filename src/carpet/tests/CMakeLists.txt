file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
set(TEST_EXE_NAME carpetTests)
add_executable(${TEST_EXE_NAME} ${SOURCES})
target_link_libraries(${TEST_EXE_NAME} PUBLIC
    gtest
    gtest_main           
    carpet
)
install(TARGETS ${TEST_EXE_NAME})
