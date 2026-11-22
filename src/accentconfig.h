// SPDX-License-Identifier: MIT
#pragma once
#include <KColorScheme>
#include <KConfigGroup>
#include <KSharedConfig>
#include <QColor>
#include <QRectF>
#include <KDecoration3/ScaleHelpers>
#include <algorithm>
struct AccentConfig {
    int activeMode = 0; // Plasma accent, custom, Breeze, none
    int inactiveMode = 2;
    QColor activeColor = QColor(61, 174, 233);
    QColor inactiveColor = QColor(61, 174, 233);
    int width = 2;
    bool maximized = false;
    static AccentConfig read() {
        auto config = KSharedConfig::openConfig(QStringLiteral("breezeaccentrc"));
        config->reparseConfiguration();
        KConfigGroup group(config, "Accent");
        AccentConfig s;
        s.activeMode = std::clamp(group.readEntry("ActiveMode", 0), 0, 3);
        s.inactiveMode = std::clamp(group.readEntry("InactiveMode", 2), 0, 3);
        s.activeColor = group.readEntry("ActiveColor", s.activeColor);
        s.inactiveColor = group.readEntry("InactiveColor", s.inactiveColor);
        s.width = std::clamp(group.readEntry("Width", 2), 1, 6);
        s.maximized = group.readEntry("ShowMaximized", false);
        return s;
    }
};

inline qreal outlineThickness(qreal width, qreal scale)
{
    return std::max(KDecoration3::pixelSize(scale), KDecoration3::snapToPixelGrid(width, scale));
}

// A maximized indicator belongs entirely to the titlebar, never the client.
inline QRectF maximizedIndicatorRect(bool maximized, bool enabled, bool titlebarVisible,
                                     qreal width, qreal titlebarHeight, qreal thickness)
{
    if (!maximized || !enabled || !titlebarVisible || width <= 0 || titlebarHeight <= 0 || thickness <= 0) {
        return {};
    }
    return QRectF(0, 0, width, std::min(titlebarHeight, thickness));
}

inline QColor readPlasmaAccent(const KSharedConfig::Ptr &config)
{
    const QColor accent = KConfigGroup(config, "General").readEntry("AccentColor", QColor(Qt::transparent));
    if (accent.isValid() && accent.alpha() != 0) {
        return accent;
    }
    return KColorScheme::createApplicationPalette(config).color(QPalette::Active, QPalette::Highlight);
}
