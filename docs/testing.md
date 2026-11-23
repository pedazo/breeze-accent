# Testing and verification

Environment: openSUSE Tumbleweed `20261120` (`x86_64`), KWin, Breeze, and
KDecoration3 `6.8.3-1.1`, Qt `6.12.0`, KF6 `6.32.0`. The compositor test uses
a virtual KWin with a private D-Bus session and XDG directories; it does not install
or select the plugin on the host desktop.

## Repeating the build tests

Follow the [build and staging instructions](../README.md#build-without-installing).
The modules should appear only in the plugin subdirectories
`org.kde.kdecoration3/org.kde.breezeaccent.so` and
`org.kde.kdecoration3.kcm/kcm_breezeaccent.so`. The Qt 6 plugin root varies
by distribution; append the system Qt 6 `qtpaths` plugin path to the staging
root as shown in the README.

To prepare another test session, start `kwin_wayland --virtual` with its own
D-Bus bus, `XDG_CONFIG_HOME`, and `XDG_RUNTIME_DIR`. In the isolated
`kwinrc`, the key still uses the historical group name even though the
plugins are KDecoration3:

```ini
[org.kde.kdecoration2]
library=org.kde.breezeaccent
```

Add the Qt 6 root from the staging tree to `QT_PLUGIN_PATH` and connect test
applications only to that session's socket. In a Wayland trace,
`zxdg_toplevel_decoration_v1.configure(2)` confirms SSD negotiation; opening
a window or taking a screenshot alone does not.

## Results on 6.8.3

- CMake, compilation, and `DESTDIR` staging succeeded. CTest: **3/3**
  (`accent_test`, `kcm_smoke`, and the ECM AppStream check). Unit tests cover
  initial values, thickness limits, the global accent, and indicator geometry.
- `rpmbuild -ba` generated an `x86_64` binary RPM, a source RPM, and debug
  packages. `%check` passed 3/3 and `rpm -K` verified the digests. The binary
  contains only the two project modules, documentation, and licenses; it has
  no install scripts. The RPM was not installed, and Breeze, KWin, and
  KDecoration3 packages were unchanged (`rpm -V` reported no differences).
- The Wayland smoke test loaded the plugin extracted from the final RPM.
  Virtual KWin identified `org.kde.breezeaccent` in `supportInformation`.
  Kate negotiated SSD. Screenshots confirmed that the outline changed when
  the global accent was updated and that the project configuration applied
  without restarting KWin. Pixel checks confirmed **no outline by default
  when maximized** and an interior strip 6 px high when the option was
  enabled. They also confirmed a custom outline after restoring and in
  quick-left tiling, with sides adjacent to the output clipped and the 6 px
  strip visible toward the center of the screen.
- In an additional matrix, KDialog and Kate (Qt) negotiated SSD and showed
  the configured accent. `gtk3-demo` (GTK/Wayland) showed its CSD frame
  without the accent when no rule was set. With the
  [Force → Only shadow rule](../README.md#electron-steam-and-other-apps-with-custom-title-bars),
  a separate isolated Wayland test confirmed that `gtk3-demo` gained the
  accent while retaining its own title bar. VS Code (Electron) started with
  Ozone Wayland requested and showed its own title bar without the accent;
  the trace did not independently confirm its Wayland transport. The
  shadow-only rule was not validated with VS Code, Steam, or ChatGPT.

To repeat the pixel test, you need Kate, Spectacle, `qdbus6`, D-Bus, and
Python with Pillow, in addition to KWin and the staged modules:

```sh
bash tests/wayland-smoke.sh "$PWD/stage$(qtpaths6 --plugin-dir)"
```

The script saves screenshots and traces in the temporary directory it reports.
Portal warnings appeared on the isolated bus during testing, but did not
prevent screenshots. The following were not tested: selection from System
Settings in a full session, global installation, fractional scaling in a
compositor, partial maximization, multiple monitors, or every tiling and
fullscreen variant.
