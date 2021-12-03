/*
    SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "grideffectkcm.h"
#include "gridconfig.h"

#include <config-kwin.h>
#include <kwineffects_interface.h>

#include <KAboutData>
#include <KActionCollection>
#include <KGlobalAccel>
#include <KLocalizedString>
#include <KPluginFactory>

#include <QAction>

K_PLUGIN_FACTORY_WITH_JSON(GridEffectConfigFactory,
                           "grideffectkcm.json",
                           registerPlugin<KWin::GridEffectConfig>();)

namespace KWin
{

GridEffectConfig::GridEffectConfig(QWidget *parent, const QVariantList &args)
    : KCModule(parent, args)
{
    ui.setupUi(this);
    GridConfig::instance(KWIN_CONFIG);
    addConfig(GridConfig::self(), this);

    auto actionCollection = new KActionCollection(this, QStringLiteral("kwin"));

    actionCollection->setComponentDisplayName(i18n("KWin"));
    actionCollection->setConfigGroup(QStringLiteral("Grid"));
    actionCollection->setConfigGlobal(true);

    const QKeySequence defaultToggleShortcut = Qt::CTRL + Qt::META + Qt::Key_G;
    QAction *toggleAction = actionCollection->addAction(QStringLiteral("Grid"));
    toggleAction->setText(i18n("Toggle Grid"));
    toggleAction->setProperty("isConfigurationAction", true);
    KGlobalAccel::self()->setDefaultShortcut(toggleAction, {defaultToggleShortcut});
    KGlobalAccel::self()->setShortcut(toggleAction, {defaultToggleShortcut});

    ui.shortcutsEditor->addCollection(actionCollection);
    connect(ui.shortcutsEditor, &KShortcutsEditor::keyChange, this, &GridEffectConfig::markAsChanged);

    load();
}

GridEffectConfig::~GridEffectConfig()
{
    // If save() is called, undo() has no effect.
    ui.shortcutsEditor->undo();
}

void GridEffectConfig::save()
{
    KCModule::save();
    ui.shortcutsEditor->save();

    OrgKdeKwinEffectsInterface interface(QStringLiteral("org.kde.KWin"),
                                         QStringLiteral("/Effects"),
                                         QDBusConnection::sessionBus());
    interface.reconfigureEffect(QStringLiteral("grid"));
}

void GridEffectConfig::defaults()
{
    ui.shortcutsEditor->allDefault();
    KCModule::defaults();
}

} // namespace KWin

#include "grideffectkcm.moc"
