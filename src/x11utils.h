/*
    SPDX-FileCopyrightText: 2022 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <netwm_def.h>

namespace KWin::X11
{

QPoint mapToNative(const QPoint &point);
NETPoint mapToNative(const NETPoint &point);

QPoint mapFromNative(const QPoint &point);
NETPoint mapFromNative(const NETPoint &point);

QSize mapToNative(const QSize &size);
NETSize mapToNative(const NETSize &size);

QSize mapFromNative(const QSize &size);
NETSize mapFromNative(const NETSize &size);

QRect mapToNative(const QRect &rect);
NETRect mapToNative(const NETRect &rect);

QRect mapFromNative(const QRect &rect);
NETRect mapFromNative(const NETRect &rect);

} // namespace KWin::X11
