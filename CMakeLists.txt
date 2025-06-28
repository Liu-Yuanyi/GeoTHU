cmake_minimum_required(VERSION 3.16)

project(test_project VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(test_project
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        geometricobject.h
        point.h
        canvas.h
        point.h point.cpp
        canvas.h canvas.cpp

        objecttype.h
        line.h line.cpp
        geometricobject.cpp
        circle.h circle.cpp
        operation.h operation.cpp
        resources.qrc
        lineoo.h lineoo.cpp
        lineo.h lineo.cpp
        tools.h tools.cpp
        intersectioncreator.h intersectioncreator.cpp
        calculator.h
        icon.qrc
        app.rc
        Log.md
        README.md
        saveloadhelper.h saveloadhelper.cpp
        measurement.h measurement.cpp
        customizedoperation.h customizedoperation.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET test_project APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(test_project SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(test_project
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(test_project PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.test_project)
endif()
set_target_properties(test_project PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS test_project
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

target_compile_definitions(test_project PUBLIC QT_USE_QREAL_OPAQUE)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(test_project)
endif()
