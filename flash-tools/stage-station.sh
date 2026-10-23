#!/bin/sh
# Stage a build on the Windows Flash Station folder: copy the images the
# station flashes, set the product's version in bin/images/version.txt and
# rewrite the product's image manifest (sha256 + size of every file the
# station's step list uses, the same thing its "Create/refresh manifest"
# button records).
#
#   stage-station.sh -s <station dir> -v <image version> [-p signatouch|signatouch-ir] [-d <deploy dir>]
#
# Copies, for product signatouch (default):
#   bin/loader/u-boot-flasher.img      <- vt-flasher/u-boot.img       (fastboot-capable loader, RAM only)
#   bin/images/boot0.img               <- <image>.boot0.img
#   bin/images/emmc-signatouch.simg    <- <image>.rootfs.wic.simg     (fastboot step 6)
#   bin/images/emmc-signatouch.wic     <- <image>.rootfs.wic          (DFU fallback, FlashMethod "dfu")
#   bin/images/u-boot.img              <- u-boot.img                  (DFU fallback step 7)
#   bin/images/emmc-signatouch-<ver>-release.wic   named copy, kept
# bin/loader/tiboot3.bin and tispl.bin are left alone (Vantron's stock
# USB-boot loader, used by steps 1-2 in both methods).
#
# This writes into the production station's folder: run it deliberately.
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
DEPLOY="$HERE/../build/deploy-ti/images/vt-sbc-am62l"
PRODUCT=signatouch
STATION=""
VERSION=""
while getopts "s:v:p:d:" o; do
    case $o in
        s) STATION=$OPTARG ;;
        v) VERSION=$OPTARG ;;
        p) PRODUCT=$OPTARG ;;
        d) DEPLOY=$OPTARG ;;
        *) exit 2 ;;
    esac
done
[ -n "$STATION" ] && [ -n "$VERSION" ] || { sed -n 2,20p "$0"; exit 2; }
[ -d "$STATION/bin/images" ] || { echo "not a station folder: $STATION" >&2; exit 1; }
case $PRODUCT in
    signatouch)    IMG=vtlinux-image-signatouch-vtlinux-vt-sbc-am62l;    KEY=signatouch;    DISPLAY="SignaTouch";    MANIFEST=image-manifest-signatouch.json ;;
    signatouch-ir) IMG=vtlinux-image-signatouch-ir-vtlinux-vt-sbc-am62l; KEY=signatouch_IR; DISPLAY="SignaTouch IR"; MANIFEST=image-manifest-signatouch-ir.json ;;
    *) echo "unknown product $PRODUCT" >&2; exit 2 ;;
esac
cpv() { cp "$1" "$2"; cmp "$1" "$2"; echo "  $(basename "$2")  <-  $(basename "$(readlink -f "$1")")"; }
echo "Staging $PRODUCT $VERSION from $DEPLOY to $STATION"
cpv "$DEPLOY/vt-flasher/u-boot.img"      "$STATION/bin/loader/u-boot-flasher.img"
cpv "$DEPLOY/$IMG.rootfs.boot0.img"             "$STATION/bin/images/boot0.img"
cpv "$DEPLOY/$IMG.rootfs.wic.simg"       "$STATION/bin/images/emmc-$PRODUCT.simg"
cpv "$DEPLOY/$IMG.rootfs.wic"            "$STATION/bin/images/emmc-$PRODUCT.wic"
cpv "$DEPLOY/u-boot.img"                 "$STATION/bin/images/u-boot.img"
cpv "$DEPLOY/$IMG.rootfs.wic"            "$STATION/bin/images/emmc-$PRODUCT-$VERSION-release.wic"

V="$STATION/bin/images/version.txt"
sed -i "s/^\[${KEY}_image_name\]=.*/[${KEY}_image_name]=\"emmc-$PRODUCT.simg\"/; s/^\[${KEY}_image_version\]=.*/[${KEY}_image_version]=\"$VERSION\"/" "$V"
grep "^\[${KEY}_" "$V"

cd "$STATION"
python3 -I - "$MANIFEST" "$DISPLAY" "$VERSION" "$PRODUCT" <<'PY'
import hashlib, json, os, sys, datetime
manifest, display, version, product = sys.argv[1:5]
files = ["bin/loader/tiboot3.bin", "bin/loader/tispl.bin", "bin/loader/u-boot-flasher.img",
         "bin/images/boot0.img", f"bin/images/emmc-{product}.simg"]
entries = {}
for f in files:
    h = hashlib.sha256()
    with open(f, "rb") as fh:
        for chunk in iter(lambda: fh.read(1 << 20), b""):
            h.update(chunk)
    entries[f.replace("/", "\\")] = {"sha256": h.hexdigest(), "size": os.path.getsize(f)}
doc = {"generated": datetime.datetime.now().strftime("%Y-%m-%dT%H:%M:%S"),
       "generatedBy": f"{os.environ.get('USER', 'wsl')} via stage-station.sh",
       "product": display, "imageVersion": version, "files": entries}
with open(manifest, "w", encoding="utf-8-sig") as out:
    json.dump(doc, out, indent=4)
    out.write("\n")
print(f"{manifest}: {len(entries)} files, imageVersion {version}")
PY
echo "Done. Start the station, run pre-flight: every file should report sha256 OK."
