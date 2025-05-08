cmake_minimum_required(VERSION 3.19)
project(MyGoBang2 LANGUAGES CXX)

# 指定使用 C++23 标准
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(MACOSX_BUNDLE_ICON_FILE MyGobang2.icns)
set(APP_ICON res/icons/MyGobang2.icns)

# 把图标（或其他资源文件）打包进 macOS .app 应用中，并放进 .app 结构中的 Contents/Resources 目录里。
set_source_files_properties(${APP_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Multimedia MultimediaWidgets Sql)

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
    databasemanager.h databasemanager.cpp
    usermanagerwindow.h usermanagerwindow.cpp
    timelogger.h timelogger.cpp
)

target_link_libraries(MyGoBang2
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt6::Multimedia
        Qt6::MultimediaWidgets
        Qt6::Sql
)

# 设置资源目录和目标目录


# 仅在Debug模式下执行
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(RESOURCE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/res")
    set(RESOURCE_DEST_DIR "${CMAKE_BINARY_DIR}/MyGoBang2.app/Contents/Resources")
    add_custom_command(
        OUTPUT "${RESOURCE_DEST_DIR}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${RESOURCE_DEST_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${RESOURCE_SOURCE_DIR}" "${RESOURCE_DEST_DIR}"
        COMMENT "Copying resource files to app bundle in Debug mode..."
    )

    add_custom_target(copy_resources ALL
        DEPENDS "${RESOURCE_DEST_DIR}"
    )
endif()


include(GNUInstallDirs)

install(TARGETS MyGoBang2
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    file(GLOB_RECURSE RES_FILES "${CMAKE_SOURCE_DIR}/res/*")

    add_custom_target(copy_res ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/MyGoBang2.app/Contents/Resources"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_SOURCE_DIR}/res"
                "${CMAKE_BINARY_DIR}/MyGoBang2.app/Contents/Resources"
        COMMENT "Copying all resource files to .app bundle..."
    )
endif()


qt_generate_deploy_app_script(
    TARGET MyGoBang2
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})


