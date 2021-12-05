/*
    SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QObject>

namespace KWin
{

class MinimizeAllDbusService : public QObject
{
    Q_OBJECT

public:
    explicit MinimizeAllDbusService(QObject *parent = nullptr);
    ~MinimizeAllDbusService() override;

public Q_SLOTS:
    void toggle();

Q_SIGNALS:
    void toggled();
};

} // namespace KWin
