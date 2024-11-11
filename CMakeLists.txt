cmake_minimum_required(VERSION 3.16)

project(CourseWorkDB VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt
#find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql)
#find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Find the Qt packages, including the Sql module
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql)  # Include Sql here
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)  # Include Sql here



# Manually specify PostgreSQL paths (adjust these to your system)
include_directories("C:/Users/Can/.vcpkg-clion/vcpkg/packages/libpq_x64-windows/include")
link_directories("C:/Users/Can/.vcpkg-clion/vcpkg/packages/libpq_x64-windows/lib")

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(CourseWorkDB
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        mainapplication.h mainapplication.cpp mainapplication.ui
        modeldb.h modeldb.cpp



    )
else()
    if(ANDROID)
        add_library(CourseWorkDB SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(CourseWorkDB
            ${PROJECT_SOURCES}
        )
    endif()
endif()




# Link Qt libraries
#target_link_libraries(CourseWorkDB PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(CourseWorkDB PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql)
# Manually link PostgreSQL library
target_link_libraries(CourseWorkDB PRIVATE "C:/Users/Can/.vcpkg-clion/vcpkg/packages/libpq_x64-windows/lib/libpq.lib")

# Additional settings for macOS and Windows
set_target_properties(CourseWorkDB PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS CourseWorkDB
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(CourseWorkDB)
endif()
