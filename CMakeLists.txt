cmake_minimum_required(VERSION 3.19)
project(MyGoBang2 LANGUAGES CXX)

# 指定使用 C++23 标准
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(MACOSX_BUNDLE_ICON_FILE MyGobang2.icns)
set(APP_ICON icons/MyGobang2.icns)
set_source_files_properties(${APP_ICON} PROPERTIES
    MACOSX_PACKAGE_LOCATION "Resources")

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Multimedia MultimediaWidgets)

qt_standard_project_setup()

qt_add_executable(MyGoBang2
    WIN32 MACOSX_BUNDLE
    ${APP_ICON}
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    acstring.h alphabetaai.h gamemodel.h globals.h localevaluationai.h mainmenu.h mainwindow.h mctsai.h pairhash.h pattern.h point.h publictool.h zobristhash.h
    acstring.cpp alphabetaai.cpp gamemodel.cpp globals.cpp localevaluationai.cpp main.cpp mainmenu.cpp mainwindow.cpp mctsai.cpp pairhash.cpp pattern.cpp publictool.cpp zobristhash.cpp
    videodialog.h videodialog.cpp
)

target_link_libraries(MyGoBang2
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt6::Multimedia
        Qt6::MultimediaWidgets
)

include(GNUInstallDirs)

install(TARGETS MyGoBang2
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(DIRECTORY videos DESTINATION ${CMAKE_INSTALL_PREFIX}/MyGoBang2.app/Contents/Resources)
qt_generate_deploy_app_script(
    TARGET MyGoBang2
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})


