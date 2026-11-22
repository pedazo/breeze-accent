// SPDX-License-Identifier: MIT
#include "accentconfig.h"
#include <QTemporaryDir>
#include <QTest>

class AccentTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void defaultsAndClamp()
    {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        qputenv("XDG_CONFIG_HOME", dir.path().toUtf8());
        auto s = AccentConfig::read();
        QCOMPARE(s.activeMode, 0);
        QCOMPARE(s.inactiveMode, 2);
        QCOMPARE(s.width, 2);
        QVERIFY(!s.maximized);
        auto config = KSharedConfig::openConfig(QStringLiteral("breezeaccentrc"));
        KConfigGroup group(config, "Accent");
        group.writeEntry("Width", 99);
        group.writeEntry("ActiveMode", -9);
        group.writeEntry("ShowMaximized", true);
        config->sync();
        s = AccentConfig::read();
        QCOMPARE(s.width, 6);
        QCOMPARE(s.activeMode, 0);
        QVERIFY(s.maximized);
        group.writeEntry("Width", -1);
        config->sync();
        QCOMPARE(AccentConfig::read().width, 1);
    }
    void indicatorGeometry()
    {
        QCOMPARE(maximizedIndicatorRect(true, true, true, 100, 20, 6), QRectF(0, 0, 100, 6));
        QCOMPARE(maximizedIndicatorRect(true, true, true, 100, 2, 6), QRectF(0, 0, 100, 2));
        QVERIFY(maximizedIndicatorRect(false, true, true, 100, 20, 6).isEmpty());
        QVERIFY(maximizedIndicatorRect(true, false, true, 100, 20, 6).isEmpty());
        QVERIFY(maximizedIndicatorRect(true, true, false, 100, 20, 6).isEmpty());
        for (const qreal scale : {0.5, 1.0, 1.25, 1.5, 2.0}) {
            QVERIFY(outlineThickness(1, scale) >= KDecoration3::pixelSize(scale));
            const auto rect = maximizedIndicatorRect(true, true, true, 100, 20, outlineThickness(1, scale));
            QVERIFY(rect.height() >= KDecoration3::pixelSize(scale));
        }
    }
    void globalAccent()
    {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        auto config = KSharedConfig::openConfig(dir.filePath("colors"), KConfig::SimpleConfig);
        KConfigGroup general(config, "General");
        KConfigGroup selection(config, "Colors:Selection");
        selection.writeEntry("BackgroundNormal", QColor(12, 34, 56));
        general.writeEntry("AccentColor", QColor(120, 30, 40));
        QCOMPARE(readPlasmaAccent(config), QColor(120, 30, 40));
        general.writeEntry("AccentColor", QColor(Qt::transparent));
        QCOMPARE(readPlasmaAccent(config), QColor(12, 34, 56));
    }
};
QTEST_MAIN(AccentTest)
#include "accent_test.moc"
