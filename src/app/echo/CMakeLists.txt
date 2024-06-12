add_executable(EchoServer EchoServer.cpp)
target_link_libraries(EchoServer PUBLIC 
carpet
proto
)

add_executable(EchoAsyncServer EchoAsyncServer.cpp)
target_link_libraries(EchoAsyncServer PUBLIC
carpet
proto
)

add_executable(EchoClient EchoClient.cpp)
target_link_libraries(EchoClient PUBLIC
carpet
proto
)
