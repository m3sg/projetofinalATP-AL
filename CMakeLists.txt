cmake_minimum_required(VERSION 3.20)
project(projetofinalATPAL C)

set(CMAKE_C_STANDARD 99)

add_executable(projetofinalATPAL
        main.c
        algebra.h
        matriz.c
        parser.c
        sistema.c
        transformacao.c
        autovaloresAutovetores.c
        menu.c
)

if (NOT MSVC)
    target_link_libraries(projetofinalATPAL m)
endif()
