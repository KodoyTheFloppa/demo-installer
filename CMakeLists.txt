cmake_minimum_required(VERSION 3.5)
project(demo-installer)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)
find_package(Qt5Widgets REQUIRED)

qt5_add_resources(RESOURCE_SRCS resources.qrc)

add_executable(demo-installer
    main.cpp
    ${RESOURCE_SRCS}
)

target_link_libraries(demo-installer Qt5::Widgets)