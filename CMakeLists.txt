cmake_minimum_required(VERSION 3.16)
project(lesson_parser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Test)

add_library(lesson_model STATIC
    model/lesson.cpp
    model/parser.cpp
    model/lesson_model.cpp
    model/lesson.cpp
    model/parser.cpp
    model/lesson_model.cpp
    model/command_parser.cpp
)

target_include_directories(lesson_model PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/model
)

target_link_libraries(lesson_model
    Qt6::Core
)

add_executable(lesson_tests
    tests/test_lesson_model.cpp
)

target_include_directories(lesson_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/model
)

target_link_libraries(lesson_tests
    lesson_model
    Qt6::Core
    Qt6::Test
)

add_executable(lesson_app
    main.cpp
    view/mainwindow.cpp
    view/mainwindow.h
    model/parser_exceptions.h
    model/lesson.h
    model/lesson_model.h
    model/parser.h
    model/command_parser.h
    model/command_parser.cpp
)

target_include_directories(lesson_app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/model
    ${CMAKE_CURRENT_SOURCE_DIR}/view
)

target_link_libraries(lesson_app
    lesson_model
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

enable_testing()
add_test(NAME LessonModelTests COMMAND lesson_tests)
