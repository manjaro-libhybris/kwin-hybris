/*
    KWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2019 Roman Gilg <subdiff@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#ifndef KWIN_X11_OUTPUT_H
#define KWIN_X11_OUTPUT_H

#include "abstract_output.h"
#include <kwin_export.h>

#include <QObject>
#include <QRect>

#include <xcb/randr.h>

namespace KWin
{

/**
 * X11 output representation
 */
class KWIN_EXPORT X11Output : public AbstractOutput
{
    Q_OBJECT

public:
    explicit X11Output(const QString &name, QObject *parent = nullptr);

    QString name() const override;

    RenderLoop *renderLoop() const override;
    void setRenderLoop(RenderLoop *loop);

    int xineramaNumber() const;
    void setXineramaNumber(int number);

    QRect geometry() const override;
    void setGeometry(QRect set);

    int refreshRate() const override;
    void setRefreshRate(int set);

    int gammaRampSize() const override;
    bool setGammaRamp(const GammaRamp &gamma) override;

    QSize physicalSize() const override;
    void setPhysicalSize(const QSize &size);

    QSize pixelSize() const override;
    bool usesSoftwareCursor() const override;

private:
    void setCrtc(xcb_randr_crtc_t crtc);
    void setGammaRampSize(int size);

    RenderLoop *m_loop = nullptr;
    xcb_randr_crtc_t m_crtc = XCB_NONE;
    QString m_name;
    QRect m_geometry;
    QSize m_physicalSize;
    int m_gammaRampSize;
    int m_refreshRate;
    int m_xineramaNumber = 0;

    friend class X11StandalonePlatform;
};

}

#endif
