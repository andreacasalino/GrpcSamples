include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/common.cmake)

execute_process(
     COMMAND /usr/bin/python3 ${CMAKE_CURRENT_SOURCE_DIR}/gen.py 
     --build_folder "${CMAKE_CURRENT_BINARY_DIR}"
     --grpc_plugin "${_GRPC_CPP_PLUGIN_EXECUTABLE}" 
)

file(GLOB SOURCES_PROTO ${CMAKE_CURRENT_BINARY_DIR}/*.h ${CMAKE_CURRENT_BINARY_DIR}/*.cc)
SET(LIB_NAME messages)
ADD_LIBRARY(${LIB_NAME} STATIC ${SOURCES_PROTO})
set_target_properties(${LIB_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${LIB_NAME} PUBLIC
absl::check
${_REFLECTION}
${_GRPC_GRPCPP}
${_PROTOBUF_LIBPROTOBUF}
)
