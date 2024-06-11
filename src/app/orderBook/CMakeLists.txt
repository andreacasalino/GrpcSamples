add_subdirectory(src)

function(MAKE_APP NAME)
    add_executable(${NAME} ${NAME}.cpp) 
    target_link_libraries(${NAME} PUBLIC
        orderBook
    )
endfunction()

# MAKE_APP(OrderBookServer)
