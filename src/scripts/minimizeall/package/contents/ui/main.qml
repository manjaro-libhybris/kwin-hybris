/*
    SPDX-FileCopyrightText: 2014 Thomas Lübking <thomas.luebking@gmail.com>
    SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick 2.12
import org.kde.kwin 3.0 as KWinComponents

QtObject {
    id: script

    property var screenEdges: []

    property var pointerScreenEdges: Repeater {
        model: script.screenEdges
        KWinComponents.ScreenEdgeItem {
            mode: KWinComponents.ScreenEdgeItem.Pointer
            edge: modelData
            onActivated: script.toggle();
        }
    }

    function toggle() {
        const allClients = workspace.clients;
        const relevantClients = [];

        for (let i = 0; i < allClients.length; ++i) {
            const client = allClients[i];
            if (client.minimizable &&
                    (client.onAllDesktops || client.desktop === workspace.currentDesktop) &&
                    (!client.activities.length || client.activities.indexOf(workspace.currentActivity.toString()) > -1)) {
                relevantClients.push(client);
            }
        }

        // Try to preserve last active window by sorting windows.
        relevantClients.sort((a, b) => {
            if (a.active) {
                return 1;
            } else if (b.active) {
                return -1;
            }
            return a.stackingOrder - b.stackingOrder;
        });

        const minimize = relevantClients.some(client => !client.minimized);
        for (const client of relevantClients) {
            const wasMinimizedByScript = client.minimizedByScript;
            delete client.minimizedByScript;

            if (minimize) {
                if (client.minimized) {
                    continue;
                }
                client.minimized = true;
                client.minimizedByScript = true;
            } else {
                if (!wasMinimizedByScript) {
                    continue;
                }
                client.minimized = false;
            }
        }
    }

    function reconfigure() {
        script.screenEdges = KWin.readConfig("BorderActivate", "").toString().split(",")
            .map(value => parseInt(value))
            .filter(value => isFinite(value));
    }

    Component.onCompleted: {
        options.configChanged.connect(reconfigure);
        reconfigure();
        KWin.registerShortcut("MinimizeAll", "MinimizeAll", "Meta+Shift+D", toggle);
    }
}
