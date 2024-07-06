function(MakeApp APP_NAME)    
    add_executable(${APP_NAME} ${APP_NAME}.cpp ${CMAKE_CURRENT_SOURCE_DIR}/../Common.h)
    target_link_libraries(${APP_NAME} PUBLIC
        carpet
        messages
    )
endfunction()
