/*
    SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "grideffect.h"
#include "gridconfig.h"

#include <KGlobalAccel>
#include <KLocalizedString>

#include <QAction>
#include <QDebug>
#include <QQuickItem>
#include <QTimer>

namespace KWin
{

GridEffect::GridEffect()
    : m_shutdownTimer(new QTimer(this))
{
    m_shutdownTimer->setSingleShot(true);
    connect(m_shutdownTimer, &QTimer::timeout, this, &GridEffect::realDeactivate);

    const QKeySequence defaultToggleShortcut = Qt::CTRL + Qt::META + Qt::Key_G;
    m_toggleAction = new QAction(this);
    connect(m_toggleAction, &QAction::triggered, this, &GridEffect::toggle);
    m_toggleAction->setObjectName(QStringLiteral("Grid"));
    m_toggleAction->setText(i18n("Toggle Grid"));
    KGlobalAccel::self()->setDefaultShortcut(m_toggleAction, {defaultToggleShortcut});
    KGlobalAccel::self()->setShortcut(m_toggleAction, {defaultToggleShortcut});
    m_toggleShortcut = KGlobalAccel::self()->shortcut(m_toggleAction);
    effects->registerGlobalShortcut({defaultToggleShortcut}, m_toggleAction);

    connect(effects, &EffectsHandler::screenAboutToLock, this, &GridEffect::realDeactivate);

    initConfig<GridConfig>();
    reconfigure(ReconfigureAll);

    setSource(QUrl::fromLocalFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("kwin/effects/grid/qml/main.qml"))));
}

GridEffect::~GridEffect()
{
}

QVariantMap GridEffect::initialProperties(EffectScreen *screen)
{
    return QVariantMap {
        { QStringLiteral("effect"), QVariant::fromValue(this) },
        { QStringLiteral("targetScreen"), QVariant::fromValue(screen) },
    };
}

void GridEffect::reconfigure(ReconfigureFlags)
{
    GridConfig::self()->read();
    setAnimationDuration(animationTime(200));

    for (const ElectricBorder &border : qAsConst(m_borderActivate)) {
        effects->unreserveElectricBorder(border, this);
    }

    for (const ElectricBorder &border : qAsConst(m_touchBorderActivate)) {
        effects->unregisterTouchBorder(border, m_toggleAction);
    }

    m_borderActivate.clear();
    m_touchBorderActivate.clear();

    const QList<int> activateBorders = GridConfig::borderActivate();
    for (const int &border : activateBorders) {
        m_borderActivate.append(ElectricBorder(border));
        effects->reserveElectricBorder(ElectricBorder(border), this);
    }

    const QList<int> touchActivateBorders = GridConfig::touchBorderActivate();
    for (const int &border : touchActivateBorders) {
        m_touchBorderActivate.append(ElectricBorder(border));
        effects->registerTouchBorder(ElectricBorder(border), m_toggleAction);
    }
}

int GridEffect::animationDuration() const
{
    return m_animationDuration;
}

void GridEffect::setAnimationDuration(int duration)
{
    if (m_animationDuration != duration) {
        m_animationDuration = duration;
        Q_EMIT animationDurationChanged();
    }
}

int GridEffect::requestedEffectChainPosition() const
{
    return 70;
}

bool GridEffect::borderActivated(ElectricBorder border)
{
    if (m_borderActivate.contains(border)) {
        toggle();
        return true;
    }
    return false;
}

void GridEffect::toggle()
{
    if (!isRunning()) {
        activate();
    } else {
        deactivate();
    }
}

void GridEffect::activate()
{
    setRunning(true);
}

void GridEffect::deactivate()
{
    const auto screenViews = views();
    for (QuickSceneView *view : screenViews) {
        QMetaObject::invokeMethod(view->rootItem(), "stop");
    }
    m_shutdownTimer->start(animationDuration());
}

void GridEffect::realDeactivate()
{
    setRunning(false);
}

void GridEffect::quickDeactivate()
{
    m_shutdownTimer->start(0);
}

void GridEffect::grabbedKeyboardEvent(QKeyEvent *keyEvent)
{
    if (m_toggleShortcut.contains(keyEvent->key() + keyEvent->modifiers())) {
        if (keyEvent->type() == QEvent::KeyPress) {
            toggle();
        }
        return;
    }
    QuickSceneEffect::grabbedKeyboardEvent(keyEvent);
}

} // namespace KWin
