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

find_package(Threads)
include(${CMAKE_CURRENT_SOURCE_DIR}/prepareGRPC.cmake)
