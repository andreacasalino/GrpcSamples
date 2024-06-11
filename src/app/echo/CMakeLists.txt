add_executable(EchoServer EchoServer.cpp)
target_link_libraries(EchoServer PUBLIC proto)

add_executable(EchoClient EchoClient.cpp)
target_link_libraries(EchoClient PUBLIC proto)
