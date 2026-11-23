# Breeze Accent

An independent window decoration for KDE Plasma 6.8, based on the official
Breeze 6.8.3 code. It adds an accent outline to windows decorated by KWin,
without replacing Breeze, changing the Qt style, or installing an effect.

In **System Settings → Colors & Themes → Window Decorations**, select
**Breeze Accent**, apply it, and open the decoration's configuration button.
By default, the active window uses the Plasma accent (2 px) and the inactive
window uses the Breeze appearance. You can choose custom colors, restore the
Breeze outline, or disable the outline for either state. The thickness of
Accent and Custom modes is adjustable from 1 to 6 logical pixels. Fully
maximized windows have no outline by default. If enabled, the corresponding
option shows only a top strip, and only when KWin retains the title bar.

## Install from source

The CMake build is not tied to a package manager. Build on the target system
against its own KDE and Qt packages. Breeze Accent targets Plasma/KWin 6.8
with a compatible KDecoration3 ABI; it has been tested on openSUSE Tumbleweed
with Plasma 6.8.3 on Wayland. Other distributions may work if they ship a
compatible stack, but have not been validated. Do not install the openSUSE
RPM on another distribution merely because it also uses RPMs.

Install these prerequisites using your distribution's package manager:

- A C++20 compiler, CMake 3.20 or newer, Ninja, and Gettext tools.
- Qt **6.11+** development files: Core, Gui, Widgets, and D-Bus (plus Qt Test
  if building the tests).
- Extra CMake Modules and KDE Frameworks **6.30+** development files:
  CoreAddons, Config, ConfigWidgets, GuiAddons, I18n, KCMUtils, and ColorScheme.
- KDecoration3 **6.8.3+** development files compatible with your running KWin.

Package names vary by distribution. If your repositories don't provide a
compatible stack, use a newer distribution release rather than mixing
system Qt or KDE libraries from unrelated repositories.

### Build without installing

Use the Qt 6 `qtpaths` executable supplied by your distribution. The example
uses `qtpaths6`; if yours is named `qtpaths` or `qtpaths-qt6`, check that it
reports Qt 6 with `--qt-version` and substitute its name.

```sh
qtpaths6 --qt-version
qt_prefix=$(qtpaths6 --install-prefix)
qt_plugins=$(qtpaths6 --plugin-dir)

cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DCMAKE_INSTALL_PREFIX="$qt_prefix" \
  -DKDE_INSTALL_PLUGINDIR="$qt_plugins" \
  -DBUILD_TESTING=ON
cmake --build build
QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure
DESTDIR="$PWD/stage" cmake --install build
find "$PWD/stage$qt_plugins" -name '*breezeaccent.so'
```

`DESTDIR` stages the two modules without touching the system. The plugin
directory should match the system Qt 6 installation used by KWin; do not
assume `lib64` or `/usr/local`. If Qt Test is unavailable, configure with
`-DBUILD_TESTING=OFF` and skip CTest.

### Install, update, or remove

Prefer packaging the staged files with your distribution's tooling if you
want package-managed upgrades and removal. Otherwise, after reviewing staging:

```sh
sudo cmake --install build
```

Select **Breeze Accent** in **System Settings → Colors & Themes → Window
Decorations**. After an update, rebuild against the current system libraries,
reinstall using the same method, then log out and back in to reload the plugin.

To remove it, first select **Breeze** and apply. Use your package manager for
a packaged install. For a manual install, check `build/install_manifest.txt`
and remove only `org.kde.kdecoration3/org.kde.breezeaccent.so` and
`org.kde.kdecoration3.kcm/kcm_breezeaccent.so` from the Qt 6 plugin directory.
Do not remove the directories themselves or any files belonging to Breeze.
Removing the plugin does not delete
`${XDG_CONFIG_HOME:-$HOME/.config}/breezeaccentrc`.

### Optional: openSUSE Tumbleweed RPM

On openSUSE Tumbleweed, the [RPM spec](packaging/breeze-accent.spec) lists
the `BuildRequires`. Build and inspect the local, unsigned package:

```sh
bash packaging/build-rpm.sh
rpm -qpl dist/breeze-accent-0.1.0-0.x86_64.rpm
rpm -K dist/breeze-accent-0.1.0-0.x86_64.rpm
```

The script writes binary, source, and debug RPMs to `dist/` by default.
These are local artifacts, not published downloads. Install or upgrade with
`sudo zypper install ./dist/<binary-rpm>` using the actual filename. Switch
back to Breeze before running `sudo zypper remove breeze-accent`.

## Electron, Steam, and other apps with custom title bars

Apps such as Electron apps, Steam, and ChatGPT desktop or browser app windows
may draw their own title bar. If the accent is missing, make KWin draw just
the **shadow** for that window. This lets Breeze Accent draw the outline
without asking KWin to add a second title bar.

1. Open the affected app, then go to **System Settings → Window Management →
   Window Rules** and create a rule.
2. Use **Detect Window Properties** and select the app window. Match its
   **Window class (application)** precisely rather than relying on the title,
   which can change. For a browser app, check that the match won't affect all
   windows of that browser.
3. Choose **Add Property…**, then under **Appearance & Fixes** select
   **Window manager draws titlebar, frame, and shadows**.
4. Set the policy to **Force** and the value to **Only shadow**. Apply the rule
   and reopen the affected window if needed.

Create a separate rule for each app that needs it. If the property is
missing, your KWin version may not offer it. Wayland and XWayland apps can
behave differently, so verify the result per app; remove the rule if it adds a
second title bar or matches windows you didn't intend to change.

## Limitations and maintenance

The plugin cannot draw over decorations provided by the app itself (CSD).
It needs KWin to supply a frame or the shadow-only decoration described
above. Fullscreen windows and windows for which KWin supplies neither are
outside its scope.
The outline does not change tiling geometry; portions extending beyond a
screen may be clipped by KWin.

See [architecture and maintenance](docs/architecture.md) and
[test results and untested cases](docs/testing.md) before using it with a
different version.

## Source and licenses

This project incorporates code from [KDE Breeze](https://invent.kde.org/plasma/breeze).
Preserved copyright notices and SPDX identifiers are in the source files.
The corresponding license texts are in [upstream/LICENSES](upstream/LICENSES).
