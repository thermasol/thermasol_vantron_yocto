#!/bin/sh
# Raise the panel backlight to the product level once the boot is past the
# point where the input stage can be tripped: the kernel brings the panel up
# dark (DT default-brightness-level 0) and the boost's cold inrush is ramped
# by the regulator driver, but it must still not land on the panel's
# display-on surge or on the first burst of userspace start-up. psplash has
# drawn by the time this unit runs; a short settle after that is enough.
LEVEL=${BACKLIGHT_LEVEL:-100}
sleep "${BACKLIGHT_SETTLE:-1.0}"
for bl in /sys/class/backlight/*/; do
    [ -f "${bl}brightness" ] || continue
    echo 0 > "${bl}bl_power" 2>/dev/null || true
    echo "${LEVEL}" > "${bl}brightness" 2>/dev/null || true
    echo "backlight-boot: $(basename "${bl%/}") set to ${LEVEL}" > /dev/kmsg 2>/dev/null || true
done
