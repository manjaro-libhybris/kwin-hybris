/*
    SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <KCModule>

#include "ui_grideffectkcm.h"

namespace KWin
{

class GridEffectConfig : public KCModule
{
    Q_OBJECT

public:
    explicit GridEffectConfig(QWidget *parent = nullptr, const QVariantList &args = QVariantList());
    ~GridEffectConfig() override;

public Q_SLOTS:
    void save() override;
    void defaults() override;

private:
    ::Ui::GridEffectConfig ui;
};

} // namespace KWin
