/*
    SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "plugin.h"
#include "minimizealldbusservice.h"

#include <QQmlEngine>

namespace KWin
{

void MinimizeAllPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<MinimizeAllDbusService>(uri, 1, 0, "MinimizeAllDbusService");
}

} // namespace KWin
