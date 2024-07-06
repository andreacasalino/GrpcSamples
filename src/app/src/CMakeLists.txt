file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
set(LIB_NAME app_base)
add_library(${LIB_NAME} ${SOURCES})
target_link_libraries(${LIB_NAME} PUBLIC messages)
