cmake_minimum_required(VERSION 3.5)

project(tsctrl LANGUAGES C)

include_directories(${PROJECT_SOURCE_DIR}
                    ${PROJECT_SOURCE_DIR}/libs
)

add_executable(tsctrl
               libs/list.h
               libs/list.c
               libs/network.h
               libs/network.c
               libs/zkt-c3.h
               libs/zkt-c3.c
               libs/ticketapi.h
               libs/ticketapi.c
               main.c
               )

include(PVS-Studio.cmake)
pvs_studio_add_target(TARGET ${PROJECT_NAME}.analyze ALL
                      OUTPUT
                      FORMAT errorfile
                      ANALYZE ${PROJECT_NAME}
                      MODE GA:1,2
                      LOG ${PROJECT_NAME}.log
                      ARGS -e C:/Qt/
                      HIDE_HELP)
