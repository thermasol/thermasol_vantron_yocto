#!/bin/sh
# Script to configure Qt5 EGLFS display rotation based on SignaSteam settings
# This script reads the orientation from SignaSteam.conf and generates:
# 1. EGLFS KMS configuration file (for reference, though rotation may not work in hardware)
# 2. Environment file with QT_QPA_EGLFS_ROTATION for software rotation

set -e

CONFIG_FILE="/opt/ThermaSol/SignaSteam.conf"
EGLFS_CONFIG_DIR="/etc/qt5"
EGLFS_CONFIG_FILE="${EGLFS_CONFIG_DIR}/eglfs_kms.json"
ROTATION_ENV_FILE="/etc/qt5/rotation.env"

# Ensure config directory exists
mkdir -p "${EGLFS_CONFIG_DIR}"

# Default to landscape if config file doesn't exist
ORIENTATION="landscape"
ROTATION=0

# Read orientation from SignaSteam config if it exists
if [ -f "${CONFIG_FILE}" ]; then
    ORIENTATION=$(grep "^orientationSelected=" "${CONFIG_FILE}" 2>/dev/null | cut -d'=' -f2)
fi

# Map orientation to rotation angle
case "${ORIENTATION}" in
    landscape)
        ROTATION=0
        ;;
    portrait)
        ROTATION=90
        ;;
    *)
        echo "Warning: Unknown orientation '${ORIENTATION}', defaulting to landscape"
        ROTATION=0
        ;;
esac

echo "Setting display rotation to ${ROTATION} degrees (orientation: ${ORIENTATION})"

# Generate EGLFS KMS configuration (for reference)
cat > "${EGLFS_CONFIG_FILE}" <<EOF
{
  "device": "/dev/dri/card0",
  "outputs": [
    {
      "name": "DSI1",
      "mode": "480x854",
      "rotation": ${ROTATION},
      "format": "xrgb8888"
    }
  ]
}
EOF

# Generate environment file for software rotation
cat > "${ROTATION_ENV_FILE}" <<EOF
QT_QPA_EGLFS_ROTATION=${ROTATION}
EOF

echo "EGLFS KMS configuration updated: ${EGLFS_CONFIG_FILE}"
echo "Rotation environment file updated: ${ROTATION_ENV_FILE}"

# The panel backlight probes at a very low default level (confirmed
# on-device: brightness=6 out of max_brightness=255, no saved
# systemd-backlight state involved) - the screen looks off even though
# it's rendering. Force it to full brightness on every boot instead of
# trusting the driver's probe-time default. Adjust the divisor below if
# full brightness is too bright for the panel.
for bl in /sys/class/backlight/*/; do
    [ -f "${bl}max_brightness" ] || continue
    max=$(cat "${bl}max_brightness")
    echo 0 > "${bl}bl_power" 2>/dev/null || true
    echo "${max}" > "${bl}brightness" 2>/dev/null || true
    echo "Set backlight $(basename "${bl%/}") to brightness ${max}"
done

exit 0
