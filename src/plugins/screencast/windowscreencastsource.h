/*
    SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "screencastsource.h"

#include <QPointer>

namespace KWin
{

class Toplevel;

class WindowScreenCastSource : public ScreenCastSource
{
    Q_OBJECT

public:
    explicit WindowScreenCastSource(Toplevel *window, QObject *parent = nullptr);

    bool hasAlphaChannel() const override;
    QSize textureSize() const override;

    void render(GLFramebuffer *target) override;
    void render(QImage *image) override;
    std::chrono::nanoseconds clock() const override;

private:
    QPointer<Toplevel> m_window;
};

} // namespace KWin
