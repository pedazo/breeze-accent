cmake_minimum_required(VERSION 3.20)
project(breezeaccent VERSION 0.1.0 LANGUAGES CXX)
# Vendored from KDE Breeze v6.8.3, commit fae34ee82f190146a6320afd3eab6da9e50230dd.
set(CMAKE_CXX_STANDARD 20)
find_package(ECM 6.30 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
include(KDEInstallDirs6)
include(KDECMakeSettings)
include(CTest)
find_package(Qt6 6.11 REQUIRED COMPONENTS Core Gui Widgets DBus)
find_package(KF6 6.30 REQUIRED COMPONENTS CoreAddons Config ConfigWidgets GuiAddons I18n KCMUtils ColorScheme)
find_package(KDecoration3 6.8.3 REQUIRED)
add_library(breezecommon6 OBJECT upstream/libbreezecommon/breezeboxshadowrenderer.cpp)
set_target_properties(breezecommon6 PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(breezecommon6 PUBLIC Qt6::Gui)
set(deco upstream/kdecoration/breezebutton.cpp upstream/kdecoration/breezedecoration.cpp upstream/kdecoration/breezesettingsprovider.cpp upstream/kdecoration/breezeexceptionlist.cpp)
kconfig_add_kcfg_files(deco upstream/kdecoration/breezesettings.kcfgc)
add_library(breezeaccent MODULE ${deco})
set_target_properties(breezeaccent PROPERTIES OUTPUT_NAME org.kde.breezeaccent)
target_include_directories(breezeaccent PRIVATE upstream/libbreezecommon ${CMAKE_CURRENT_BINARY_DIR} src)
target_link_libraries(breezeaccent PRIVATE breezecommon6 Qt6::DBus KF6::CoreAddons KF6::ConfigGui KF6::GuiAddons KF6::I18n KF6::ColorScheme KDecoration3::KDecoration)
install(TARGETS breezeaccent DESTINATION ${KDE_INSTALL_PLUGINDIR}/${KDECORATION_PLUGIN_DIR})
set(config_sources
 upstream/kdecoration/breezeexceptionlist.cpp
 upstream/kdecoration/config/breezeconfigwidget.cpp
 upstream/kdecoration/config/breezedetectwidget.cpp
 upstream/kdecoration/config/breezeexceptiondialog.cpp
 upstream/kdecoration/config/breezeexceptionlistwidget.cpp
 upstream/kdecoration/config/breezeexceptionmodel.cpp
 upstream/kdecoration/config/breezeitemmodel.cpp)
ki18n_wrap_ui(config_sources upstream/kdecoration/config/ui/breezeconfigurationui.ui upstream/kdecoration/config/ui/breezeexceptiondialog.ui upstream/kdecoration/config/ui/breezeexceptionlistwidget.ui)
kconfig_add_kcfg_files(config_sources upstream/kdecoration/breezesettings.kcfgc)
kcoreaddons_add_plugin(kcm_breezeaccent SOURCES src/kcm.cpp ${config_sources} INSTALL_NAMESPACE "${KDECORATION_KCM_PLUGIN_DIR}")
target_include_directories(kcm_breezeaccent PRIVATE src upstream/kdecoration upstream/kdecoration/config ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(kcm_breezeaccent PRIVATE Qt6::Widgets Qt6::DBus KF6::CoreAddons KF6::ConfigGui KF6::ConfigWidgets KF6::KCMUtils KF6::I18n KF6::ColorScheme KDecoration3::KDecoration)
target_compile_definitions(kcm_breezeaccent PRIVATE TRANSLATION_DOMAIN="breeze_kwin_deco")
if(BUILD_TESTING)
 find_package(Qt6 REQUIRED COMPONENTS Test)
 add_executable(accent_test tests/accent_test.cpp)
 target_include_directories(accent_test PRIVATE src)
 target_link_libraries(accent_test PRIVATE Qt6::Test KF6::ConfigGui KF6::ColorScheme KDecoration3::KDecoration)
 add_test(NAME accent_test COMMAND accent_test)
 set_tests_properties(accent_test PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
 find_program(KCMSHELL6_EXECUTABLE kcmshell6)
 if(KCMSHELL6_EXECUTABLE)
  file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/kcm-smoke-home")
  add_test(NAME kcm_smoke COMMAND ${CMAKE_COMMAND} -E env
   --unset=DISPLAY --unset=WAYLAND_DISPLAY
   "HOME=${CMAKE_CURRENT_BINARY_DIR}/kcm-smoke-home"
   "XDG_CONFIG_HOME=${CMAKE_CURRENT_BINARY_DIR}/kcm-smoke-home/config"
   "XDG_CONFIG_DIRS=${CMAKE_CURRENT_BINARY_DIR}/kcm-smoke-home/config-dirs"
   "XDG_DATA_HOME=${CMAKE_CURRENT_BINARY_DIR}/kcm-smoke-home/data"
   "XDG_CACHE_HOME=${CMAKE_CURRENT_BINARY_DIR}/kcm-smoke-home/cache"
   "XDG_CURRENT_DESKTOP="
   "DBUS_SESSION_BUS_ADDRESS=unix:path=${CMAKE_CURRENT_BINARY_DIR}/kcm-smoke-no-bus"
   "QT_QPA_PLATFORM=offscreen"
   "QT_PLUGIN_PATH=${CMAKE_BINARY_DIR}/bin"
   ${KCMSHELL6_EXECUTABLE} --smoke-test org.kde.kdecoration3.kcm/kcm_breezeaccent)
 endif()
endif()
