/*
    KWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2013 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "screenlockerwatcher.h"
#include "wayland_server.h"

#include <QFutureWatcher>
#include <QtConcurrentRun>
// dbus generated
#include "kscreenlocker_interface.h"
#include "screenlocker_interface.h"

namespace KWin
{
KWIN_SINGLETON_FACTORY(ScreenLockerWatcher)

static const QString SCREEN_LOCKER_SERVICE_NAME = QStringLiteral("org.freedesktop.ScreenSaver");

ScreenLockerWatcher::ScreenLockerWatcher(QObject *parent)
    : QObject(parent)
    , m_serviceWatcher(new QDBusServiceWatcher(this))
    , m_locked(false)
{
    if (waylandServer() && waylandServer()->hasScreenLockerIntegration()) {
        connect(waylandServer(), &WaylandServer::initialized, this, &ScreenLockerWatcher::initialize);
    } else {
        initialize();
    }
}

ScreenLockerWatcher::~ScreenLockerWatcher()
{
}

void ScreenLockerWatcher::initialize()
{
    connect(m_serviceWatcher, &QDBusServiceWatcher::serviceOwnerChanged, this, &ScreenLockerWatcher::serviceOwnerChanged);
    m_serviceWatcher->setWatchMode(QDBusServiceWatcher::WatchForOwnerChange);
    m_serviceWatcher->addWatchedService(SCREEN_LOCKER_SERVICE_NAME);
    // check whether service is registered
    QFutureWatcher<QDBusReply<bool>> *watcher = new QFutureWatcher<QDBusReply<bool>>(this);
    connect(watcher, &QFutureWatcher<QDBusReply<bool>>::finished, this, &ScreenLockerWatcher::serviceRegisteredQueried);
    connect(watcher, &QFutureWatcher<QDBusReply<bool>>::canceled, watcher, &QFutureWatcher<QDBusReply<bool>>::deleteLater);
    watcher->setFuture(
        QtConcurrent::run(QDBusConnection::sessionBus().interface(), &QDBusConnectionInterface::isServiceRegistered, SCREEN_LOCKER_SERVICE_NAME));
}

void ScreenLockerWatcher::serviceOwnerChanged(const QString &serviceName, const QString &oldOwner, const QString &newOwner)
{
    Q_UNUSED(oldOwner)
    if (serviceName != SCREEN_LOCKER_SERVICE_NAME) {
        return;
    }
    delete m_interface;
    m_interface = nullptr;
    delete m_kdeInterface;
    m_kdeInterface = nullptr;

    m_locked = false;
    if (!newOwner.isEmpty()) {
        m_interface = new OrgFreedesktopScreenSaverInterface(newOwner, QStringLiteral("/ScreenSaver"), QDBusConnection::sessionBus(), this);
        m_kdeInterface = new OrgKdeScreensaverInterface(newOwner, QStringLiteral("/ScreenSaver"), QDBusConnection::sessionBus(), this);
        connect(m_interface, &OrgFreedesktopScreenSaverInterface::ActiveChanged, this, &ScreenLockerWatcher::setLocked);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(m_interface->GetActive(), this);
        connect(watcher, &QDBusPendingCallWatcher::finished, this, &ScreenLockerWatcher::activeQueried);
        connect(m_kdeInterface, &OrgKdeScreensaverInterface::AboutToLock, this, &ScreenLockerWatcher::aboutToLock);
    }
}

void ScreenLockerWatcher::serviceRegisteredQueried()
{
    QFutureWatcher<QDBusReply<bool>> *watcher = dynamic_cast<QFutureWatcher<QDBusReply<bool>> *>(sender());
    if (!watcher) {
        return;
    }
    const QDBusReply<bool> &reply = watcher->result();
    if (reply.isValid() && reply.value()) {
        QFutureWatcher<QDBusReply<QString>> *ownerWatcher = new QFutureWatcher<QDBusReply<QString>>(this);
        connect(ownerWatcher, &QFutureWatcher<QDBusReply<QString>>::finished, this, &ScreenLockerWatcher::serviceOwnerQueried);
        connect(ownerWatcher, &QFutureWatcher<QDBusReply<QString>>::canceled, ownerWatcher, &QFutureWatcher<QDBusReply<QString>>::deleteLater);
        ownerWatcher->setFuture(
            QtConcurrent::run(QDBusConnection::sessionBus().interface(), &QDBusConnectionInterface::serviceOwner, SCREEN_LOCKER_SERVICE_NAME));
    }
    watcher->deleteLater();
}

void ScreenLockerWatcher::serviceOwnerQueried()
{
    QFutureWatcher<QDBusReply<QString>> *watcher = dynamic_cast<QFutureWatcher<QDBusReply<QString>> *>(sender());
    if (!watcher) {
        return;
    }
    const QDBusReply<QString> reply = watcher->result();
    if (reply.isValid()) {
        serviceOwnerChanged(SCREEN_LOCKER_SERVICE_NAME, QString(), reply.value());
    }

    watcher->deleteLater();
}

void ScreenLockerWatcher::activeQueried(QDBusPendingCallWatcher *watcher)
{
    QDBusPendingReply<bool> reply = *watcher;
    if (!reply.isError()) {
        setLocked(reply.value());
    }
    watcher->deleteLater();
}

void ScreenLockerWatcher::setLocked(bool activated)
{
    if (m_locked == activated) {
        return;
    }
    m_locked = activated;
    Q_EMIT locked(m_locked);
}

}
