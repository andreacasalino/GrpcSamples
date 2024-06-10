INCLUDE(FindProtobuf)
INCLUDE_DIRECTORIES(${PROTOBUF_INCLUDE_DIR})
file(GLOB SOURCES_PROTO ${CMAKE_CURRENT_SOURCE_DIR}/*.proto)
get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)
protobuf_generate(TARGET ${SOURCES_PROTO} LANGUAGE cpp)
protobuf_generate(TARGET ${SOURCES_PROTO} LANGUAGE grpc GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}")
# PROTOBUF_GENERATE_CPP(PROTO_SRC PROTO_HEADER ${SOURCES_PROTO})

SET(LIB_NAME messages)
ADD_LIBRARY(${LIB_NAME} STATIC ${PROTO_HEADER} ${PROTO_SRC})
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/)
target_link_libraries(${LIB_NAME} PUBLIC 
${PROTOBUF_LIBRARY}
gRPC::grpc
gRPC::grpc++
)
