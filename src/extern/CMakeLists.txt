function(FETCH_LIB LIB_NAME TAG URL)
    include(FetchContent)
    message(STATUS "===> fetching: ${LIB_NAME}")
    FetchContent_Declare(
        ${LIB_NAME}
        GIT_REPOSITORY ${URL}
        GIT_TAG        ${TAG}
    )
    FetchContent_MakeAvailable(${LIB_NAME})
endfunction()

# find_package(Threads REQUIRED)

# set(FETCHCONTENT_QUIET OFF)
FETCH_LIB(gRPC v1.64.1 https://github.com/grpc/grpc)

find_package(gRPC REQUIRED)
# target_link_libraries(myproto
#     PUBLIC
#         protobuf::libprotobuf
#         gRPC::grpc
#         gRPC::grpc++
# )

# if(BUILD_TESTS)
#     if (WIN32)
#     set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
#     endif (WIN32)
#     FETCH_LIB(googletest v1.13.0 https://github.com/google/googletest.git)
# endif()

# set(BUILD_MinimalCppSocket_SAMPLES OFF CACHE BOOL "" FORCE)
# FETCH_LIB(minimal-socket v.3.0 https://github.com/andreacasalino/Minimal-Socket)
