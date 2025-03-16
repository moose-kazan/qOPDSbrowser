cmake_minimum_required(VERSION 3.5)

project(qOPDSbrowser VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network Xml LinguistTools)
find_package(Qt6 REQUIRED COMPONENTS Widgets Network Xml LinguistTools)

set (ICON_FILES
    src/icons/icon16.png
    src/icons/icon24.png
    src/icons/icon32.png
    src/icons/icon32-find.png
    src/icons/icon32-next.png
    src/icons/icon32-prev.png
    src/icons/icon32-reload.png
    src/icons/icon48.png
    src/icons/icon64.png
    src/icons/icon96.png
    src/icons/icon128.png
    src/icons/icon256.png
    src/icons/icon512.png
)

set(TS_FILES src/languages/qOPDSbrowser_ru.ts src/languages/qOPDSbrowser_en.ts)

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
	src/feedparserviewmodel.h
	src/feedparserviewmodel.cpp
	src/filetypes.h
	src/filetypes.cpp
	src/settings.h
	src/settings.cpp
	src/dialogsettings.h
	src/dialogsettings.cpp
	src/dialogsettings.ui
	src/downloadhistory.h
	src/downloadhistory.cpp
	src/urlvalidator.h
	src/urlvalidator.cpp
	qOPDSbrowser.desktop
	src/qOPDSbrowser.qrc
	${ICON_FILES}
	${TS_FILES}
)

qt_add_executable(qOPDSbrowser
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    src/urlvalidator.h src/urlvalidator.cpp
)

qt6_standard_project_setup()

qt_add_translations(
    qOPDSbrowser
    TS_FILES ${TS_FILES}
    RESOURCE_PREFIX "i18n"
)

target_link_libraries(qOPDSbrowser PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(qOPDSbrowser PRIVATE Qt${QT_VERSION_MAJOR}::Network)
target_link_libraries(qOPDSbrowser PRIVATE Qt${QT_VERSION_MAJOR}::Xml)

set_target_properties(qOPDSbrowser PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER com.ylsoftware.qopdsbrowser
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS qOPDSbrowser
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES qOPDSbrowser.desktop DESTINATION share/applications)
install(FILES src/icons/icon512.png DESTINATION share/icons RENAME qOPDSbrowser.png)
install(FILES qOPDSbrowser.conf DESTINATION /etc/xdg/YLSoftware/)
#install(FILES qOPDSbrowser.conf DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/xdg/YLSoftware/)

foreach(SIZE 16 24 32 48 64 96 128 256 512)
    install(FILES src/icons/icon${SIZE}.png DESTINATION share/icons/hicolor/${SIZE}x${SIZE}/apps RENAME qOPDSbrowser.png)
endforeach()

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(qOPDSbrowser)
endif()
