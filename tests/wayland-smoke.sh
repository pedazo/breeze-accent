#!/usr/bin/env bash
# Pixel regression test for KWin Wayland 6.8.3; does not use the host session.
set -euo pipefail

if [[ ${1:-} != --inside ]]; then
    plugins=$(realpath -- "${1:?Usage: bash tests/wayland-smoke.sh /path/to/stage/usr/lib64/qt6/plugins}")
    test -r "$plugins/org.kde.kdecoration3/org.kde.breezeaccent.so"
    for program in dbus-run-session dbus-send kate kwin_wayland python3 qdbus6 spectacle; do
        command -v "$program" >/dev/null
    done
    python3 -c 'from PIL import Image'
    tmp_base=${TMPDIR:-/tmp}
    if [[ -z ${TMPDIR:-} && -w /tmp/opencode ]]; then
        tmp_base=/tmp/opencode
    fi
    lab=$(mktemp -d "$tmp_base/breeze-accent-wayland.XXXXXXXX")
    echo "Wayland smoke logs and screenshots: $lab"
    mkdir -m 700 "$lab"/{home,config,data,cache,runtime}
    exec env -u DISPLAY -u WAYLAND_DISPLAY -u WAYLAND_SOCKET -u DBUS_SESSION_BUS_ADDRESS \
        HOME="$lab/home" XDG_CONFIG_HOME="$lab/config" XDG_DATA_HOME="$lab/data" \
        XDG_CACHE_HOME="$lab/cache" XDG_RUNTIME_DIR="$lab/runtime" \
        QT_QPA_PLATFORM=wayland QT_PLUGIN_PATH="$plugins" \
        dbus-run-session -- bash "$(realpath -- "$0")" --inside "$lab"
fi

lab=$2
export WAYLAND_DISPLAY=breeze-accent-test
cat > "$XDG_CONFIG_HOME/kwinrc" <<'EOF'
[org.kde.kdecoration2]
library=org.kde.breezeaccent
[Windows]
BorderlessMaximizedWindows=false
EOF
printf '[General]\nAccentColor=208,72,112\n' > "$XDG_CONFIG_HOME/kdeglobals"
kwin_wayland --virtual --no-lockscreen --no-kactivities --no-global-shortcuts \
    --socket "$WAYLAND_DISPLAY" --width 1200 --height 800 >"$lab/kwin.log" 2>&1 &
kwin=$!
kate=
cleanup() {
    if [[ -n $kate ]]; then kill "$kate" 2>/dev/null || :; fi
    kill "$kwin" 2>/dev/null || :
    wait "$kwin" 2>/dev/null || :
}
trap cleanup EXIT
for ((i=0; i<80; i++)); do
    [[ ! -S "$XDG_RUNTIME_DIR/$WAYLAND_DISPLAY" ]] || break
    kill -0 "$kwin"
    sleep 0.25
done
test -S "$XDG_RUNTIME_DIR/$WAYLAND_DISPLAY"
WAYLAND_DEBUG=client kate --new >"$lab/kate.log" 2>&1 &
kate=$!
for ((i=0; i<80; i++)); do
    if grep -Eq 'zxdg_toplevel_decoration_v1#[0-9]+\.configure\(2\)' "$lab/kate.log"; then break; fi
    kill -0 "$kate"
    sleep 0.25
done
grep -E 'zxdg_toplevel_decoration_v1#[0-9]+\.configure\(2\)' "$lab/kate.log" > "$lab/ssd.log"
qdbus6 org.kde.KWin /KWin supportInformation > "$lab/kwin-support.log"
grep -Fx 'Plugin: org.kde.breezeaccent' "$lab/kwin-support.log"

capture() {
    sleep 1
    spectacle -b -n -o "$lab/$1.png" >"$lab/$1-spectacle.log" 2>&1
    test -s "$lab/$1.png"
}
script() {
    local name=$1 js=$2 id
    printf '%s\n' "$js" > "$lab/$name.js"
    id=$(qdbus6 org.kde.KWin /Scripting org.kde.kwin.Scripting.loadScript "$lab/$name.js" "$name")
    [[ $id =~ ^[0-9]+$ ]]
    qdbus6 org.kde.KWin "/Scripting/Script$id" run >/dev/null
}

capture normal
printf '[General]\nAccentColor=49,188,104\n' > "$XDG_CONFIG_HOME/kdeglobals"
dbus-send --session --type=signal /KGlobalSettings org.kde.KGlobalSettings.notifyChange int32:0 int32:0
capture accent-changed

script maximize 'workspace.slotWindowMaximize();'
capture maximized-default
printf '[Accent]\nActiveMode=1\nActiveColor=23,164,235\nWidth=6\nShowMaximized=true\n' > "$XDG_CONFIG_HOME/breezeaccentrc"
dbus-send --session --type=signal /BreezeAccent org.kde.BreezeAccent.changed
capture maximized-indicator

script restore 'workspace.slotWindowMaximize();'
capture custom
script tile 'workspace.slotWindowQuickTileLeft();'
capture left-tile

python3 - "$lab" <<'PY'
from collections import Counter
from pathlib import Path
import sys
from PIL import Image

lab = Path(sys.argv[1])
pink = (208, 72, 112)
green = (49, 188, 104)
blue = (23, 164, 235)

def check(name, expected, *, top=False, absent=(), middle=None):
    img = Image.open(lab / f"{name}.png").convert("RGB")
    pixels = Counter(getattr(img, "get_flattened_data", img.getdata)())
    print(name, {color: pixels[color] for color in (pink, green, blue)})
    assert expected is None or pixels[expected] > 200, name
    assert not any(pixels[color] for color in absent), name
    if top:
        for y in range(6):
            count = sum(img.getpixel((x, y)) == expected for x in range(img.width))
            assert count > img.width * 0.9, (name, y, count)
        assert img.getpixel((img.width // 2, 6)) != expected, name
    if middle is not None:
        x_positions = [x for x in range(img.width) if img.getpixel((x, img.height // 2)) == expected]
        assert len(x_positions) == middle, (name, x_positions)
    return img

check("normal", pink, middle=4)
check("accent-changed", green, absent=(pink,), middle=4)
check("maximized-default", None, absent=(pink, green, blue))
check("maximized-indicator", blue, top=True, absent=(pink, green))
check("custom", blue, middle=12)
tile = check("left-tile", blue, middle=6)
assert all(tile.getpixel((x, tile.height // 2)) == blue for x in range(tile.width // 2, tile.width // 2 + 6))
PY
