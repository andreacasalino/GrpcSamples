add_executable(SimpleStreamServer SimpleStreamServer.cpp)
target_link_libraries(SimpleStreamServer PUBLIC 
carpet
proto
)

add_executable(SimpleStreamClient SimpleStreamClient.cpp)
target_link_libraries(SimpleStreamClient PUBLIC
carpet
proto
)
