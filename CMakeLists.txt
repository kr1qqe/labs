cmake_minimum_required(VERSION 3.16)
project(DominoProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Test)

set(APP_SOURCES
    mainwindow.cpp
)

set(APP_HEADERS
    mainwindow.h
)

set(APP_FORMS
    mainwindow.ui
)

add_executable(domino
    main.cpp
    ${APP_SOURCES}
    ${APP_HEADERS}
    ${APP_FORMS}
)

target_link_libraries(domino
    Qt6::Core
    Qt6::Widgets
)

add_executable(test_mainwindow
    test_mainwindow.cpp
    ${APP_SOURCES}
    ${APP_HEADERS}
    ${APP_FORMS}
)

target_link_libraries(test_mainwindow
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

target_include_directories(test_mainwindow PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

enable_testing()
add_test(NAME DominoTests COMMAND test_mainwindow)
