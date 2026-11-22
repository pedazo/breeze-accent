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

## Build without installing

Reference environment: openSUSE Tumbleweed `20261120`, Breeze, KWin, and
KDecoration3 `6.8.3-1.1`, Qt `6.12.0`, Frameworks `6.32.0`, Wayland.
You need a C++ compiler, CMake, Ninja, and the Qt 6, KDE Frameworks, and
KDecoration3 development packages. See the spec's [`BuildRequires`](packaging/breeze-accent.spec)
for the complete list. Compatibility with other snapshots or Plasma versions
has not been validated.

```sh
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DCMAKE_INSTALL_PREFIX=/usr \
  -DBUILD_TESTING=ON
cmake --build build
QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure
DESTDIR="$PWD/stage" cmake --install build
```

`DESTDIR` places the modules in `stage/`; it does not copy them to `/usr`.
Build the binary RPM, source RPM, and debug RPMs without installing anything:

```sh
bash packaging/build-rpm.sh
```

The script writes to `dist/` by default; it accepts an output directory as
its first argument. Generated RPMs in `dist/` are local build artifacts and
are not included or published in the repository. Run the script to create
them. The local package is unsigned.

## Install, update, and remove

Inspect the local RPM's contents and digests with `rpm -qpl` and `rpm -K`.
In the reference `x86_64` environment, install it with:

```sh
sudo zypper install ./dist/breeze-accent-0.1.0-0.x86_64.rpm
```

Select **Breeze Accent** in the selector described above. To update, install
the new binary RPM using the same command and its corresponding filename.
If you update the library while KWin has it loaded, log out and back in to
ensure the new version is loaded.

To remove it, first select **Breeze** in the selector and apply:

```sh
sudo zypper remove breeze-accent
```

The project configuration at `${XDG_CONFIG_HOME:-$HOME/.config}/breezeaccentrc`
is not deleted when the package is removed. These steps were not performed
on the host desktop during testing.

## Limitations and maintenance

Applications that draw their own decoration (CSD), fullscreen windows, and
windows whose decoration is removed by KWin are outside the plugin's scope.
The outline does not change tiling geometry; portions extending beyond a
screen may be clipped by KWin.

See [architecture and maintenance](docs/architecture.md) and
[test results and untested cases](docs/testing.md) before using it with a
different version.

## Source and licenses

This project incorporates code from [KDE Breeze](https://invent.kde.org/plasma/breeze).
Preserved copyright notices and SPDX identifiers are in the source files.
The corresponding license texts are in [upstream/LICENSES](upstream/LICENSES).
