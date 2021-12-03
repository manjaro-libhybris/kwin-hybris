/*
    SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick 2.12
import org.kde.kwin 3.0 as KWinComponents
import org.kde.kwin.private.effects 1.0
import org.kde.plasma.components 3.0 as PlasmaComponents
import org.kde.plasma.core 2.0 as PlasmaCore

Rectangle {
    required property QtObject effect
    required property QtObject targetScreen

    color: "black"

    function start() {
        root.animationEnabled = true;
        root.organized = true;
    }

    function stop() {
        root.organized = false;
    }

    KWinComponents.VirtualDesktopModel { id: desktopModel }
    KWinComponents.ClientModel { id: stackModel }

    Item {
        id: root
        anchors.fill: parent

        property bool animationEnabled: false
        property int animationDuration: PlasmaCore.Units.longDuration
        property int currentIndex: KWinComponents.Workspace.currentDesktop - 1
        property bool organized: false

        property alias columns: grid.columns
        property alias rows: grid.rows
        property real rowSpacing: grid.rowSpacing
        property real columnSpacing: grid.columnSpacing
        property alias model: tiles.model

        readonly property real layoutScale: Math.min(width / (width * columns + columnSpacing * (columns - 1)), height / (height * rows + rowSpacing * (rows - 1)))
        readonly property real cellWidth: organized ? layoutScale * width : width
        readonly property real cellHeight: organized ? layoutScale * height : height

        readonly property int currentRow: Math.floor(currentIndex / columns)
        readonly property int currentColumn: currentIndex % columns

        Grid {
            id: grid
            rows: KWinComponents.Workspace.desktopGridHeight
            columns: KWinComponents.Workspace.desktopGridWidth
            columnSpacing: PlasmaCore.Units.largeSpacing
            rowSpacing: PlasmaCore.Units.largeSpacing
            width: root.cellWidth * columns + columnSpacing * (columns - 1)
            height: root.cellHeight * rows + rowSpacing * (rows - 1)
            x: root.organized ? 0.5 * (root.width - width) : -columnX(root.currentColumn)
            y: root.organized ? 0.5 * (root.height - height) : -rowY(root.currentRow)

            function columnX(column) {
                return (root.cellWidth + columnSpacing) * column;
            }

            function rowY(row) {
                return (root.cellHeight + rowSpacing) * row;
            }

            Behavior on x {
                XAnimator { duration: root.animationDuration; easing.type: Easing.OutCubic }
            }
            Behavior on y {
                YAnimator { duration: root.animationDuration; easing.type: Easing.OutCubic }
            }

            Repeater {
                id: tiles
                model: desktopModel

                Item {
                    id: tile
                    required property QtObject desktop

                    width: root.cellWidth
                    height: root.cellHeight
                    z: KWinComponents.Workspace.currentVirtualDesktop == desktop ? 1 : 0
                    transformOrigin: Item.TopLeft
                    state: root.organized ? "organized" : "initial"

                    // We animate the scale instead of width and height to avoid relayouting.
                    transitions: [
                        Transition {
                            from: "initial"; to: "organized"
                            PropertyAction {
                                target: tile
                                property: "scale"
                                value: 1 / root.layoutScale
                            }
                            ScaleAnimator {
                                target: tile
                                easing.type: Easing.OutCubic
                                from: 1 / root.layoutScale
                                to: 1
                                duration: root.animationDuration
                            }
                        },
                        Transition {
                            from: "organized"; to: "initial"
                            PropertyAction {
                                target: tile
                                property: "scale"
                                value: root.layoutScale
                            }
                            ScaleAnimator {
                                target: tile
                                easing.type: Easing.OutCubic
                                from: root.layoutScale
                                to: 1
                                duration: root.animationDuration
                            }
                        }
                    ]

                    KWinComponents.DesktopBackgroundItem {
                        anchors.fill: parent
                        outputName: targetScreen.name
                        desktop: tile.desktop
                    }

                    WindowHeap {
                        anchors.fill: parent
                        animationDuration: root.animationDuration
                        animationEnabled: root.animationEnabled
                        organized: root.organized || KWinComponents.Workspace.currentVirtualDesktop != tile.desktop
                        model: KWinComponents.ClientFilterModel {
                            activity: KWinComponents.Workspace.currentActivity
                            desktop: tile.desktop
                            screenName: targetScreen.name
                            clientModel: stackModel
                            windowType: ~KWinComponents.ClientFilterModel.Dock &
                                    ~KWinComponents.ClientFilterModel.Desktop &
                                    ~KWinComponents.ClientFilterModel.Notification;
                        }
                        onActivated: effect.deactivate();
                    }
                }
            }

            move: Transition {
                XAnimator { duration: root.animationDuration; easing.type: Easing.OutCubic }
                YAnimator { duration: root.animationDuration; easing.type: Easing.OutCubic }
            }
        }
    }

    Repeater {
        model: KWinComponents.ClientFilterModel {
            desktop: KWinComponents.Workspace.currentVirtualDesktop
            screenName: targetScreen.name
            clientModel: stackModel
            windowType: KWinComponents.ClientFilterModel.Dock
        }

        KWinComponents.WindowThumbnailItem {
            wId: model.client.internalId
            x: model.client.x - targetScreen.geometry.x
            y: model.client.y - targetScreen.geometry.y
            visible: opacity > 0
            opacity: (model.client.hidden || root.organized) ? 0 : 1

            Behavior on opacity {
                NumberAnimation { duration: root.animationDuration; easing.type: Easing.OutCubic }
            }
        }
    }

    Row {
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.bottomMargin: PlasmaCore.Units.largeSpacing
        anchors.rightMargin: PlasmaCore.Units.largeSpacing

        PlasmaComponents.Button {
            icon.name: "list-remove"
            enabled: KWinComponents.Workspace.desktops > 1
            implicitWidth: PlasmaCore.Units.iconSizes.medium
            implicitHeight: implicitWidth
            onClicked: desktopModel.remove(desktopModel.rowCount() - 1)
        }
        PlasmaComponents.Button {
            icon.name: "list-add"
            enabled: KWinComponents.Workspace.desktops < 20
            implicitWidth: PlasmaCore.Units.iconSizes.medium
            implicitHeight: implicitWidth
            onClicked: desktopModel.create(desktopModel.rowCount())
        }
    }

    Component.onCompleted: Qt.callLater(start);
}
