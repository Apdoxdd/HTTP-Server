cmake_minimum_required(VERSION 3.10)

project(httpServer)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Bundled OpenSSL
set(OPENSSL_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dependency")
set(OPENSSL_USE_STATIC_LIBS FALSE)

find_package(OpenSSL REQUIRED)

set(SOURCES
        src/main.cpp
            src/httpRequest.cpp
                src/httpServer.cpp
                    src/methods.cpp
                        src/threadPool.cpp
                            src/logger.cpp
                        )

                        add_executable(httpServer ${SOURCES})

                        target_include_directories(httpServer PRIVATE
                                "${CMAKE_CURRENT_SOURCE_DIR}/include"
                            )

                            target_link_libraries(httpServer PRIVATE
                                    ws2_32
                                        mswsock
                                            OpenSSL::SSL
                                                OpenSSL::Crypto
                                            )

                                            # Copy OpenSSL DLLs beside the executable
                                            add_custom_command(TARGET httpServer POST_BUILD
                                                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                                                            "${CMAKE_CURRENT_SOURCE_DIR}/dependency/bin/libssl-3-x64.dll"
                                                                    "$<TARGET_FILE_DIR:httpServer>"

                                                                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                                                                                "${CMAKE_CURRENT_SOURCE_DIR}/dependency/bin/libcrypto-3-x64.dll"
                                                                                        "$<TARGET_FILE_DIR:httpServer>"
                                                                                    )
