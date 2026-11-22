#!/usr/bin/env bash
# Build binary and source RPMs without installing either.
set -euo pipefail

repo=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)
spec="$repo/packaging/breeze-accent.spec"
out=$(realpath -m -- "${1:-$repo/dist}")
version=$(rpmspec -q --srpm --qf '%{version}\n' "$spec")
name="breeze-accent-$version"
tmp_base=${TMPDIR:-/tmp}
if [[ -z ${TMPDIR:-} && -w /tmp/opencode ]]; then
    tmp_base=/tmp/opencode
fi
tmp=$(mktemp -d "$tmp_base/breeze-accent-rpm.XXXXXXXX")
trap 'rm -rf -- "$tmp"' EXIT

mkdir -p "$tmp"/{BUILD,BUILDROOT,RPMS,SOURCES,SPECS,SRPMS} "$out"
cd "$repo"

# Archive only project sources, never build trees, Git metadata or home config.
roots=(CMakeLists.txt README.md docs packaging)
for dir in LICENSES cmake kdecoration libbreezecommon upstream breeze src tests; do
    [[ ! -d "$dir" ]] || roots+=("$dir")
done
tar --sort=name --owner=0 --group=0 --numeric-owner \
    --mtime="@${SOURCE_DATE_EPOCH:-1794009600}" \
    --transform="s|^|$name/|" -cf - -- "${roots[@]}" |
    gzip -n > "$tmp/SOURCES/$name.tar.gz"

rpmbuild -ba --define "_topdir $tmp" "$spec"
find "$tmp/RPMS" "$tmp/SRPMS" -type f -name '*.rpm' -exec cp -v -- '{}' "$out/" \;
