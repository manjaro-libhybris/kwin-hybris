/*
    SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "grideffect.h"

namespace KWin
{

KWIN_EFFECT_FACTORY_SUPPORTED(GridEffect,
                              "metadata.json.stripped",
                              return GridEffect::supported();)

} // namespace KWin

#include "main.moc"
