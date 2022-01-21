/*
    SPDX-FileCopyrightText: 2022 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "x11utils.h"
#include "main.h"

namespace KWin::X11
{

QPoint mapToNative(const QPoint &point)
{
    return QPoint {
        int(point.x() * kwinApp()->x11Scale()),
        int(point.y() * kwinApp()->x11Scale()),
    };
}

NETPoint mapToNative(const NETPoint &point)
{
    return NETPoint(mapToNative(point.toPoint()));
}

QPoint mapFromNative(const QPoint &point)
{
    return QPoint {
        int(point.x() / kwinApp()->x11Scale()),
        int(point.y() / kwinApp()->x11Scale()),
    };
}

NETPoint mapFromNative(const NETPoint &point)
{
    return NETPoint(mapFromNative(point.toPoint()));
}

QSize mapToNative(const QSize &size)
{
    return QSize {
        int(size.width() * kwinApp()->x11Scale()),
        int(size.height() * kwinApp()->x11Scale()),
    };
}

NETSize mapToNative(const NETSize &size)
{
    return NETSize(mapToNative(size.toSize()));
}

QSize mapFromNative(const QSize &size)
{
    return QSize {
        int(size.width() / kwinApp()->x11Scale()),
        int(size.height() / kwinApp()->x11Scale()),
    };
}

NETSize mapFromNative(const NETSize &size)
{
    return NETSize(mapFromNative(size.toSize()));
}

QRect mapToNative(const QRect &rect)
{
    return QRect(mapToNative(rect.topLeft()), mapToNative(rect.size()));
}

NETRect mapToNative(const NETRect &rect)
{
    return NETRect(mapToNative(rect.toRect()));
}

QRect mapFromNative(const QRect &rect)
{
    return QRect(mapFromNative(rect.topLeft()), mapFromNative(rect.size()));
}

NETRect mapFromNative(const NETRect &rect)
{
    return NETRect(mapFromNative(rect.toRect()));
}

} // namespace KWin::X11
