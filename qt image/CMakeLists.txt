cmake_minimum_required(VERSION 3.21)

project(LibraryGui VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(LibraryGui
    main.cpp
    mainwindow.cpp
    libraryshell.cpp
    mainwindow.h
    mainwindow.ui
)

set_target_properties(LibraryGui PROPERTIES WIN32_EXECUTABLE TRUE)

target_link_libraries(LibraryGui PRIVATE Qt6::Widgets)
