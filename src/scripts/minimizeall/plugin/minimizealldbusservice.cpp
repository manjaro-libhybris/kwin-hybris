/*
    SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "minimizealldbusservice.h"
#include "minimizeall1adaptor.h"

#include <QDBusConnection>

namespace KWin
{

MinimizeAllDbusService::MinimizeAllDbusService(QObject *parent)
    : QObject(parent)
{
    new MinimizeAll1Adaptor(this);

    QDBusConnection::sessionBus().registerObject(QStringLiteral("/org/kde/KWin/Script/MinimizeAll1"), this);
    QDBusConnection::sessionBus().registerService(QStringLiteral("org.kde.KWin.Script.MinimizeAll1"));
}

MinimizeAllDbusService::~MinimizeAllDbusService()
{
    QDBusConnection::sessionBus().unregisterService(QStringLiteral("org.kde.KWin.Script.MinimizeAll1"));
}

void MinimizeAllDbusService::toggle()
{
    Q_EMIT toggled();
}

} // namespace KWin
