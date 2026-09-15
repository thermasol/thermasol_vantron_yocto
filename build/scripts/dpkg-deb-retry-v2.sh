#!/bin/sh
# Wrapper around dpkg-deb to work around host tar/pseudo/dpkg compatibility
# bugs on this Ubuntu 24.04 + GNU tar 1.35 host:
#
# 1. GNU tar's *at()-family syscalls intermittently confuse pseudo's
#    fd-tracking on deep directory trees ("got *at() syscall for unknown
#    directory"), failing dpkg-deb's internal tar invocation. Retrying
#    recovers in practice since the race isn't always hit.
# 2. Separately, dpkg-deb has been observed to exit 0 while silently emitting
#    a truncated archive missing the data.tar member entirely. Worse, on at
#    least one observed case (mesa-src) the truncated attempt and the
#    eventual successful attempt were named DIFFERENTLY (one without the
#    package's epoch prefix, one with) — so a single precomputed expected
#    path isn't reliable for validation. Instead, glob-match every
#    "<pkg>_*_<arch>.deb" file dpkg-deb could have produced in outdir, delete
#    any broken droppings after every attempt, and require exactly one valid
#    survivor with both control.tar and data.tar members.
#
# Falls back to a pure-Python .deb builder if dpkg-deb can't produce a valid
# archive after retrying.

root="$2"
outdir="$3"
control="$root/DEBIAN/control"
pkg=$(awk -F': ' '/^Package:/{print $2; exit}' "$control" | tr -d '\r')
arch=$(awk -F': ' '/^Architecture:/{print $2; exit}' "$control" | tr -d '\r')

is_valid_deb() {
    [ -f "$1" ] || return 1
    members=$(ar t "$1" 2>/dev/null)
    echo "$members" | grep -q "^control\.tar" || return 1
    echo "$members" | grep -q "^data\.tar" || return 1
    return 0
}

# Remove any droppings from a failed attempt (whatever name dpkg-deb used),
# keeping the pattern narrow to this exact package+arch to avoid touching
# other packages' output sitting in the same shared outdir.
cleanup_broken() {
    for f in "$outdir/${pkg}_"*"_${arch}.deb"; do
        [ -e "$f" ] || continue
        is_valid_deb "$f" || rm -f "$f"
    done
}

find_valid_deb() {
    for f in "$outdir/${pkg}_"*"_${arch}.deb"; do
        [ -e "$f" ] || continue
        is_valid_deb "$f" && { echo "$f"; return 0; }
    done
    return 1
}

attempts=30
i=0
while [ "$i" -lt "$attempts" ]; do
    dpkg-deb "$@" >/dev/null 2>&1
    if find_valid_deb >/dev/null; then
        cleanup_broken
        exit 0
    fi
    cleanup_broken
    i=$((i + 1))
    sleep 0.2
done

# dpkg-deb can't produce a valid archive for this package after retrying
# (either the tar/pseudo race deterministically hits, or the silent
# truncation bug). Fall back to a pure-Python .deb builder that doesn't hit
# either issue.
python3 "$(dirname "$0")/pydeb-build.py" "$@" >/dev/null 2>&1
if find_valid_deb >/dev/null; then
    cleanup_broken
    exit 0
fi
cleanup_broken
exit 1
