add_executable(FooServer Server.cpp)
target_link_libraries(FooServer PUBLIC proto)

add_executable(FooClient Client.cpp)
target_link_libraries(FooClient PUBLIC proto)
