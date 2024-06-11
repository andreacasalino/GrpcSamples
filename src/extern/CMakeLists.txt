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

if(BUILD_TESTS)
    if (WIN32)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    endif (WIN32)
    FETCH_LIB(googletest v1.13.0 https://github.com/google/googletest.git)
endif()
