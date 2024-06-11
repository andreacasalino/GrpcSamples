file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

set(LIB_NAME carpet)
add_library(${LIB_NAME} STATIC ${SOURCES})

target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
install(TARGETS ${LIB_NAME})
target_link_libraries(${LIB_NAME} PUBLIC
${CMAKE_THREAD_LIBS_INIT}
)

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
