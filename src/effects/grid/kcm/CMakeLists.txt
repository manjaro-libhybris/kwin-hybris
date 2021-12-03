# SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
#
# SPDX-License-Identifier: BSD-3-Clause

set(kwin_grid_config_SOURCES grideffectkcm.cpp)
ki18n_wrap_ui(kwin_grid_config_SOURCES grideffectkcm.ui)
kconfig_add_kcfg_files(kwin_grid_config_SOURCES ../gridconfig.kcfgc)

add_library(kwin_grid_config MODULE ${kwin_grid_config_SOURCES})
target_link_libraries(kwin_grid_config
    KF5::Completion
    KF5::ConfigWidgets
    KF5::GlobalAccel
    KF5::I18n
    KF5::XmlGui
    KWinEffectsInterface
)

kcoreaddons_desktop_to_json(kwin_grid_config grideffectkcm.desktop SERVICE_TYPES kcmodule.desktop)
install(TARGETS kwin_grid_config DESTINATION ${KDE_INSTALL_PLUGINDIR}/kwin/effects/configs)
