# Architecture and limitations

## Base and separation

The code is based on [KDE Breeze `v6.8.3`](https://invent.kde.org/plasma/breeze/-/tree/v6.8.3),
commit `fae34ee82f190146a6320afd3eab6da9e50230dd`. `upstream/` contains the
subset required to build the decoration and the license files; it does not
contain the application style, cursors, color schemes, or wallpapers.

The KDecoration3 module `org.kde.breezeaccent`, its KCM `kcm_breezeaccent`, and
the `breezeaccentrc` configuration do not replace Breeze's counterparts. The
project registers neither a KWin effect nor a persistent service.

The KCM adds an accent tab and retains Breeze's options on a separate tab,
using its own values in `breezeaccentrc`. Accent mode reads
`[General] AccentColor` from `kdeglobals`; if no explicit accent is set, it
uses the global color scheme's selection color, not the window-specific
scheme. The plugin reloads when it receives a KDE palette-change notification
or a notification from its own KCM.

## Geometry and repainting

`upstream/kdecoration/breezedecoration.cpp::recalculateBorders()` keeps the
decoration margins, resize area, corner radii, and outline separate. For
ordinary windows, `BorderOutline` passes the color, thickness, and radii to
KWin. KWin draws the outline outside the window; it does not reserve client
space or change the resize handle area. Thickness is rounded to the next
window-scale grid step, with a minimum of one physical pixel.

- **Fully maximized:** The outer outline is disabled. If the indicator option
  is enabled, a strip is drawn inside the title bar's top edge, if the title
  bar remains visible. It does not draw all four sides or overlap the
  application's surface.
- **Tiling:** Breeze's radii and margins, based on the adjacent edges reported
  by KWin, are retained. The compositor clips parts that extend beyond the
  output. `BorderOutline` cannot suppress a single side or negotiate spacing
  between adjacent tiled windows; outlines can overlap if shown on both active
  and inactive windows.
- **SSD/CSD:** The decoration runs only when KWin provides the frame. An
  application can choose its own frame, including on Wayland with negotiated
  decoration support. The plugin does not modify CSD, draw over application
  content, or restore frames suppressed by KWin in fullscreen or by its
  borderless-window policy.

## Updating the base

For each update, compare the `upstream/` subset with the new official tag.
Reapply the local identity, option handling, and outline without replacing
Breeze's newer geometry decisions. Check the KDecoration3 ABI, KCM metadata,
and installation tree; repeat the tests in KWin Wayland before publishing an
RPM. The base is not downloaded during the build.

`src/` contains the integrating KCM and accent policy. Changes to the imported
subset are concentrated in plugin identity, the configuration file,
notifications, the outline, and the window menu icon. `QIcon` is used without
the temporary `KIconThemes` palette, so some symbolic icons may use a
different color from the original Breeze version.
