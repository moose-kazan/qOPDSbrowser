cmake_minimum_required(VERSION 3.5)

project(qOPDSbrowser VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network Xml LinguistTools)
find_package(Qt6 REQUIRED COMPONENTS Widgets Network Xml LinguistTools)

set(TS_FILES qOPDSbrowser_ru_RU.ts)

set(PROJECT_SOURCES
        src/main.cpp
	src/mainwindow.cpp
	src/mainwindow.h
	src/mainwindow.ui
	src/dialogbookmarkadd.cpp
	src/dialogbookmarkadd.h
	src/dialogbookmarkadd.ui
	src/dialogbookmarkedit.h
	src/dialogbookmarkedit.cpp
	src/dialogbookmarkedit.ui
	src/opdslist.cpp
	src/opdslist.h
	src/feedparser.h
	src/feedparser.cpp
	src/settings.h
	src/settings.cpp
	src/dialogsettings.h
	src/dialogsettings.cpp
	src/dialogsettings.ui
	src/downloadhistory.h
	src/downloadhistory.cpp
	${TS_FILES}
)

qt_add_executable(qOPDSbrowser
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)
qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
qt6_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

target_link_libraries(qOPDSbrowser PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(qOPDSbrowser PRIVATE Qt${QT_VERSION_MAJOR}::Network)
target_link_libraries(qOPDSbrowser PRIVATE Qt${QT_VERSION_MAJOR}::Xml)

set_target_properties(qOPDSbrowser PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS qOPDSbrowser
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(qOPDSbrowser)
endif()
