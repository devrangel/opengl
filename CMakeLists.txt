cmake_minimum_required(VERSION 3.16)

project(AssimpMC)

set(TARGET "AssimpMC")

set(CMAKE_CXX_STANDARD 11)

add_executable(AssimpMC ${CMAKE_SOURCE_DIR}/src/main.cpp
                        ${CMAKE_SOURCE_DIR}/src/utils/shader.cpp
                        ${CMAKE_SOURCE_DIR}/include/glad/glad.c)

target_compile_options(${TARGET} PRIVATE -Wall)

target_include_directories(${TARGET} PRIVATE ${CMAKE_SOURCE_DIR}/include)

set(GLFW "${CMAKE_SOURCE_DIR}/lib/libglfw3.a")
set(ASSIMP "${CMAKE_SOURCE_DIR}/lib/libassimp.a")
set(ZLIB "${CMAKE_SOURCE_DIR}/lib/libzlibstatic.a")

#add_library(libassimp SHARED IMPORTED)
#set_target_properties(libassimp PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/libassimp.so)

target_link_libraries(${TARGET} PRIVATE ${GLFW}
                                        ${ASSIMP}
                                        ${ZLIB}
                                        X11
                                        GL
                                        dl
                                        pthread)