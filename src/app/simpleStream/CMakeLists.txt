add_executable(SimpleStreamServer SimpleStreamServer.cpp)
target_link_libraries(SimpleStreamServer PUBLIC 
carpet
proto
)

# add_executable(EchoClient EchoClient.cpp)
# target_link_libraries(EchoClient PUBLIC
# carpet
# proto
# )
