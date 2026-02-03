cmake_minimum_required(VERSION 4.1)
project(rayEngine C)
set(CMAKE_C_STANDARD 99)

file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.c)

add_executable(rayEngine ${SOURCES}
        include/lua_yxengine_wrappers.h)
target_include_directories(rayEngine PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/game
        ${CMAKE_SOURCE_DIR}/include/generic
        ${CMAKE_SOURCE_DIR}/include/lua_header
        ${CMAKE_SOURCE_DIR}/3rdlib/raylib-5.5/include
        ${CMAKE_SOURCE_DIR}/3rdlib/raygui-4.0/src
)

if (WIN32)
    target_link_libraries(rayEngine PRIVATE
            ${CMAKE_SOURCE_DIR}/3rdlib/raylib-5.5/lib/raylibdll.lib
    )
    # 3. 运行时拷贝 dll（关键！）
    add_custom_command(TARGET rayEngine POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/3rdlib/raylib-5.5/lib/raylib.dll
            $<TARGET_FILE_DIR:rayEngine>
    )
    include_directories(${RAYLIB_DIR}/include)
    link_directories(${RAYLIB_DIR}/lib)
    link_directories(${CMAKE_SOURCE_DIR}/3rdlib/raylib-5.5/lib)
elseif (UNIX)
    #set(CMAKE_PREFIX_PATH /usr/local/lib)
    find_package(raylib REQUIRED)
    include_directories(${RAYLIB_INCLUDE_DIRS})
    target_link_libraries(rayEngine PRIVATE
            /usr/local/lib/libraylib.a
            m
            pthread
            dl
            X11
    )
endif()
