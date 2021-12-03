# SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
#
# SPDX-License-Identifier: BSD-3-Clause

add_subdirectory(kcm)

set(grid_SOURCES
    main.cpp
    grideffect.cpp
)

kconfig_add_kcfg_files(grid_SOURCES
    gridconfig.kcfgc
)

kwin4_add_effect_module(kwin4_effect_grid ${grid_SOURCES})

install(DIRECTORY qml DESTINATION ${KDE_INSTALL_DATADIR}/kwin/effects/grid)
