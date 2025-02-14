# for things that are also used by kwin libraries
configure_file(libkwineffects/kwinconfig.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/libkwineffects/kwinconfig.h)
configure_file(config-kwin.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kwin.h)

set(kwin_effects_dbus_xml ${CMAKE_CURRENT_SOURCE_DIR}/org.kde.kwin.Effects.xml)
qt_add_dbus_interface(effects_interface_SRCS ${kwin_effects_dbus_xml} kwineffects_interface)
add_library(KWinEffectsInterface STATIC ${effects_interface_SRCS})
target_link_libraries(KWinEffectsInterface Qt::DBus)

add_subdirectory(effects)
add_subdirectory(scripts)
add_subdirectory(tabbox)
add_subdirectory(scripting)
add_subdirectory(helpers)
add_subdirectory(qml)

add_subdirectory(libkwineffects)
if (KWIN_BUILD_KCMS)
    add_subdirectory(kcmkwin)
endif()

add_library(kwin SHARED)
set_target_properties(kwin PROPERTIES
   VERSION ${PROJECT_VERSION}
   SOVERSION ${PROJECT_VERSION_MAJOR}
)

target_sources(kwin PRIVATE
    3rdparty/xcursor.c
    abstract_client.cpp
    abstract_output.cpp
    abstract_wayland_output.cpp
    activation.cpp
    appmenu.cpp
    atoms.cpp
    client_machine.cpp
    composite.cpp
    cursor.cpp
    dbusinterface.cpp
    debug_console.cpp
    decorationitem.cpp
    decorations/decoratedclient.cpp
    decorations/decorationbridge.cpp
    decorations/decorationpalette.cpp
    decorations/decorations_logging.cpp
    decorations/settings.cpp
    deleted.cpp
    dmabuftexture.cpp
    dpmsinputeventfilter.cpp
    effectloader.cpp
    effects.cpp
    events.cpp
    focuschain.cpp
    ftrace.cpp
    gestures.cpp
    globalshortcuts.cpp
    group.cpp
    idle_inhibition.cpp
    input.cpp
    input_event.cpp
    input_event_spy.cpp
    inputbackend.cpp
    inputdevice.cpp
    inputmethod.cpp
    inputpanelv1client.cpp
    inputpanelv1integration.cpp
    internal_client.cpp
    item.cpp
    keyboard_input.cpp
    keyboard_layout.cpp
    keyboard_layout_switching.cpp
    keyboard_repeat.cpp
    killwindow.cpp
    layers.cpp
    layershellv1client.cpp
    layershellv1integration.cpp
    linux_dmabuf.cpp
    main.cpp
    modifier_only_shortcuts.cpp
    moving_client_x11_filter.cpp
    netinfo.cpp
    onscreennotification.cpp
    options.cpp
    osd.cpp
    outline.cpp
    overlaywindow.cpp
    placement.cpp
    platform.cpp
    plugin.cpp
    pluginmanager.cpp
    pointer_input.cpp
    popup_input_filter.cpp
    renderbackend.cpp
    renderjournal.cpp
    renderloop.cpp
    rootinfo_filter.cpp
    rulebooksettings.cpp
    rules.cpp
    scene.cpp
    screenedge.cpp
    screenlockerwatcher.cpp
    screens.cpp
    scripting/v2/clientmodel.cpp
    scripting/v3/clientmodel.cpp
    scripting/v3/virtualdesktopmodel.cpp
    scripting/dbuscall.cpp
    scripting/desktopbackgrounditem.cpp
    scripting/screenedgeitem.cpp
    scripting/scriptedeffect.cpp
    scripting/scripting.cpp
    scripting/scripting_logging.cpp
    scripting/scriptingutils.cpp
    scripting/thumbnailitem.cpp
    scripting/workspace_wrapper.cpp
    session.cpp
    session_consolekit.cpp
    session_logind.cpp
    session_noop.cpp
    shadow.cpp
    shadowitem.cpp
    sm.cpp
    surfaceitem.cpp
    surfaceitem_internal.cpp
    surfaceitem_wayland.cpp
    surfaceitem_x11.cpp
    syncalarmx11filter.cpp
    tablet_input.cpp
    toplevel.cpp
    hide_cursor_spy.cpp
    touch_input.cpp
    udev.cpp
    unmanaged.cpp
    useractions.cpp
    virtualdesktops.cpp
    virtualdesktopsdbustypes.cpp
    virtualkeyboard_dbus.cpp
    was_user_interaction_x11_filter.cpp
    wayland_server.cpp
    waylandclient.cpp
    waylandoutput.cpp
    waylandoutputconfig.cpp
    waylandoutputdevicev2.cpp
    waylandshellintegration.cpp
    window_property_notify_x11_filter.cpp
    windowitem.cpp
    workspace.cpp
    x11client.cpp
    x11eventfilter.cpp
    x11syncmanager.cpp
    xcursortheme.cpp
    xdgactivationv1.cpp
    xdgshellclient.cpp
    xdgshellintegration.cpp
    xkb.cpp
    xwaylandclient.cpp
    xwl/xwayland_interface.cpp
)

target_link_libraries(kwin
    kwineffects
    kwinglutils

    Qt::Concurrent
    Qt::DBus
    Qt::Quick

    KF5::ConfigCore
    KF5::ConfigWidgets
    KF5::CoreAddons
    KF5::GlobalAccel
    KF5::GlobalAccelPrivate
    KF5::I18n
    KF5::Notifications
    KF5::Package
    KF5::Plasma
    KF5::QuickAddons
    KF5::WindowSystem

    KDecoration2::KDecoration
    KDecoration2::KDecoration2Private

    PW::KScreenLocker
    Plasma::KWaylandServer

    XCB::COMPOSITE
    XCB::CURSOR
    XCB::DAMAGE
    XCB::GLX
    XCB::ICCCM
    XCB::KEYSYMS
    XCB::RANDR
    XCB::RENDER
    XCB::SHAPE
    XCB::SHM
    XCB::SYNC
    XCB::XCB
    XCB::XFIXES

    UDev::UDev
    XKB::XKB
    Qt5::XkbCommonSupportPrivate
    epoxy::epoxy

    Threads::Threads
)

kconfig_add_kcfg_files(kwin
    settings.kcfgc
    rulesettings.kcfgc
    rulebooksettingsbase.kcfgc
)

ki18n_wrap_ui(kwin
    debug_console.ui
    shortcutdialog.ui
)

set(kwin_dbus_SRCS)
qt_add_dbus_adaptor(kwin_dbus_SRCS scripting/org.kde.kwin.Script.xml scripting/scripting.h KWin::AbstractScript)
qt_add_dbus_adaptor(kwin_dbus_SRCS org.kde.KWin.xml dbusinterface.h KWin::DBusInterface)
qt_add_dbus_adaptor(kwin_dbus_SRCS org.kde.kwin.Compositing.xml dbusinterface.h KWin::CompositorDBusInterface)
qt_add_dbus_adaptor(kwin_dbus_SRCS ${kwin_effects_dbus_xml} effects.h KWin::EffectsHandlerImpl)
qt_add_dbus_adaptor(kwin_dbus_SRCS org.kde.KWin.VirtualDesktopManager.xml dbusinterface.h KWin::VirtualDesktopManagerDBusInterface)
qt_add_dbus_adaptor(kwin_dbus_SRCS org.kde.KWin.Session.xml sm.h KWin::SessionManager)
qt_add_dbus_adaptor(kwin_dbus_SRCS org.kde.KWin.Plugins.xml dbusinterface.h KWin::PluginManagerDBusInterface)

qt_add_dbus_interface(kwin_dbus_SRCS ${KSCREENLOCKER_DBUS_INTERFACES_DIR}/kf5_org.freedesktop.ScreenSaver.xml screenlocker_interface)
qt_add_dbus_interface(kwin_dbus_SRCS ${KSCREENLOCKER_DBUS_INTERFACES_DIR}/org.kde.screensaver.xml kscreenlocker_interface)
qt_add_dbus_interface(kwin_dbus_SRCS org.kde.kappmenu.xml appmenu_interface)

target_sources(kwin PRIVATE
    ${kwin_dbus_SRCS}
)

add_subdirectory(backends)
add_subdirectory(scenes)
add_subdirectory(utils)

if (KWIN_BUILD_CMS)
    target_sources(kwin PRIVATE
        colordevice.cpp
        colormanager.cpp
    )
    target_link_libraries(kwin lcms2::lcms2)
endif()

if (KWIN_BUILD_ACTIVITIES)
    target_sources(kwin PRIVATE activities.cpp)
    target_link_libraries(kwin KF5::Activities)
endif()

if (KWIN_BUILD_TABBOX)
    target_sources(kwin PRIVATE
        tabbox/clientmodel.cpp
        tabbox/desktopchain.cpp
        tabbox/desktopmodel.cpp
        tabbox/switcheritem.cpp
        tabbox/tabbox.cpp
        tabbox/tabbox_logging.cpp
        tabbox/tabboxconfig.cpp
        tabbox/tabboxhandler.cpp
        tabbox/x11_filter.cpp
    )
    target_link_libraries(kwin Qt::GuiPrivate)
endif()

qt5_generate_dbus_interface(virtualkeyboard_dbus.h org.kde.kwin.VirtualKeyboard.xml OPTIONS -A)
qt5_generate_dbus_interface(tabletmodemanager.h org.kde.KWin.TabletModeManager.xml OPTIONS -A)

generate_export_header(kwin EXPORT_FILE_NAME kwin_export.h)

add_executable(kwin_x11 main_x11.cpp)
target_link_libraries(kwin_x11
    kwin4_effect_builtins
    kwin
    KF5::Crash
    Qt::X11Extras
)

install(TARGETS kwin ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)
install(TARGETS kwin_x11 ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

add_subdirectory(xwl)

add_executable(kwin_wayland
    main_wayland.cpp
    tabletmodemanager.cpp
)
target_link_libraries(kwin_wayland
    kwin4_effect_builtins
    kwin
    KF5::Crash
    KWinXwaylandServerModule
)
if (HAVE_LIBCAP)
    target_link_libraries(kwin_wayland ${Libcap_LIBRARIES})
endif()

install(TARGETS kwin_wayland ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
if (HAVE_LIBCAP)
    install(
    CODE "execute_process(
            COMMAND
                ${SETCAP_EXECUTABLE}
                CAP_SYS_NICE=+ep
                \$ENV{DESTDIR}${CMAKE_INSTALL_FULL_BINDIR}/kwin_wayland)"
    )
endif()

add_subdirectory(platformsupport)
add_subdirectory(plugins)

target_link_libraries(kwin_wayland
    KWinQpaPlugin
    KF5GlobalAccelKWinPlugin
    KF5WindowSystemKWinPlugin
    KF5IdleTimeKWinPlugin
)

add_custom_target(
    KWinDBusInterfaces
    ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kwin.VirtualKeyboard.xml
            ${CMAKE_CURRENT_BINARY_DIR}/org.kde.KWin.TabletModeManager.xml
)

if (PipeWire_FOUND)
    target_link_libraries(kwin_wayland KWinScreencastPlugin)
endif()

install(FILES kwin.kcfg DESTINATION ${KDE_INSTALL_KCFGDIR} RENAME ${KWIN_NAME}.kcfg)
install(FILES kwin.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFY5RCDIR} RENAME ${KWIN_NAME}.notifyrc)
install(
    FILES
        org.kde.KWin.VirtualDesktopManager.xml
        org.kde.KWin.xml
        org.kde.kwin.Compositing.xml
        org.kde.kwin.Effects.xml
        org.kde.KWin.Plugins.xml
        ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kwin.VirtualKeyboard.xml
        ${CMAKE_CURRENT_BINARY_DIR}/org.kde.KWin.TabletModeManager.xml
    DESTINATION
        ${KDE_INSTALL_DBUSINTERFACEDIR}
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kwin_export.h DESTINATION ${KDE_INSTALL_INCLUDEDIR} COMPONENT Devel)

# Install the KWin/Script service type
install(FILES scripting/kwinscript.desktop DESTINATION ${KDE_INSTALL_KSERVICETYPES5DIR})
