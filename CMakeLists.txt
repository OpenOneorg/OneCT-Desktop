cmake_minimum_required(VERSION 3.16)
project(OneCtQtClient LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Qt6 COMPONENTS Widgets Network REQUIRED)
add_executable(OneCtQtClient
    src/main.cpp
    src/MainWindow.h
    src/MainWindow.cpp
    src/ApiClient.h
    src/ApiClient.cpp
    src/SettingsDialog.h
    src/SettingsDialog.cpp
    src/PostWidget.h
    src/PostWidget.cpp
)
target_include_directories(OneCtQtClient PRIVATE src)
target_link_libraries(OneCtQtClient PRIVATE Qt6::Widgets Qt6::Network)
