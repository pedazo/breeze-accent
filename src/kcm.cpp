// SPDX-License-Identifier: MIT
#include "accentconfig.h"
#include "breezeconfigwidget.h"
#include <KCModule>
#include <KColorButton>
#include <KPluginFactory>
#include <QCheckBox>
#include <QComboBox>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QFormLayout>
#include <QSpinBox>
#include <QTabWidget>
#include <QVBoxLayout>

class AccentKcm : public KCModule
{
    Q_OBJECT
public:
    AccentKcm(QObject *parent, const KPluginMetaData &metadata, const QVariantList &args)
        : KCModule(parent, metadata)
        , breeze(new Breeze::ConfigWidget(this, metadata, args))
    {
        auto layout = new QVBoxLayout(widget());
        auto tabs = new QTabWidget;
        layout->addWidget(tabs);
        auto accentPage = new QWidget;
        auto form = new QFormLayout(accentPage);
        active = new QComboBox;
        inactive = new QComboBox;
        for (auto box : {active, inactive}) {
            box->addItems({tr("Plasma accent"), tr("Custom color"), tr("Breeze"), tr("None")});
        }
        ac = new KColorButton;
        ic = new KColorButton;
        width = new QSpinBox;
        width->setRange(1, 6);
        width->setSuffix(tr(" px"));
        maximized = new QCheckBox(tr("Show a top indicator on maximized windows"));
        form->addRow(tr("Active outline:"), active);
        form->addRow(tr("Active custom color:"), ac);
        form->addRow(tr("Inactive outline:"), inactive);
        form->addRow(tr("Inactive custom color:"), ic);
        form->addRow(tr("Width:"), width);
        form->addRow(maximized);
        tabs->addTab(accentPage, tr("Accent"));
        tabs->addTab(breeze->widget(), tr("Breeze options"));
        connect(active, &QComboBox::currentIndexChanged, this, [this] { setNeedsSave(true); });
        connect(inactive, &QComboBox::currentIndexChanged, this, [this] { setNeedsSave(true); });
        connect(ac, &KColorButton::changed, this, [this] { setNeedsSave(true); });
        connect(ic, &KColorButton::changed, this, [this] { setNeedsSave(true); });
        connect(width, &QSpinBox::valueChanged, this, [this] { setNeedsSave(true); });
        connect(maximized, &QCheckBox::toggled, this, [this] { setNeedsSave(true); });
        connect(breeze, &Breeze::ConfigWidget::needsSaveChanged, this, [this] {
            if (breeze->needsSave()) setNeedsSave(true);
        });
    }
    void apply(const AccentConfig &s)
    {
        active->setCurrentIndex(s.activeMode);
        inactive->setCurrentIndex(s.inactiveMode);
        ac->setColor(s.activeColor);
        ic->setColor(s.inactiveColor);
        width->setValue(s.width);
        maximized->setChecked(s.maximized);
    }
    void load() override
    {
        apply(AccentConfig::read());
        breeze->load();
        setNeedsSave(false);
    }
    void defaults() override
    {
        apply(AccentConfig{});
        breeze->defaults();
        setNeedsSave(true);
    }
    void save() override
    {
        breeze->save();
        auto config = KSharedConfig::openConfig(QStringLiteral("breezeaccentrc"));
        KConfigGroup group(config, "Accent");
        group.writeEntry("ActiveMode", active->currentIndex());
        group.writeEntry("InactiveMode", inactive->currentIndex());
        group.writeEntry("ActiveColor", ac->color());
        group.writeEntry("InactiveColor", ic->color());
        group.writeEntry("Width", width->value());
        group.writeEntry("ShowMaximized", maximized->isChecked());
        config->sync();
        QDBusConnection::sessionBus().send(QDBusMessage::createSignal(
            QStringLiteral("/BreezeAccent"), QStringLiteral("org.kde.BreezeAccent"), QStringLiteral("changed")));
        setNeedsSave(false);
    }
private:
    Breeze::ConfigWidget *breeze;
    QComboBox *active, *inactive;
    KColorButton *ac, *ic;
    QSpinBox *width;
    QCheckBox *maximized;
};
K_PLUGIN_CLASS_WITH_JSON(AccentKcm, "kcm.json")
#include "kcm.moc"
