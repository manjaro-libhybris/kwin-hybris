# SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
#
# SPDX-License-Identifier: BSD-3-Clause

set(minimizeallplugin_SOURCES
    minimizealldbusservice.cpp
    plugin.cpp
)

qt_add_dbus_adaptor(minimizeallplugin_SOURCES org.kde.KWin.Script.MinimizeAll1.xml minimizealldbusservice.h KWin::MinimizeAllDbusService)

add_library(minimizeallplugin MODULE ${minimizeallplugin_SOURCES})
target_link_libraries(minimizeallplugin PRIVATE
    Qt5::Core
    Qt5::DBus
    Qt5::Qml
)

install(TARGETS minimizeallplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/kwin/private/minimizeall)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/kwin/private/minimizeall)
