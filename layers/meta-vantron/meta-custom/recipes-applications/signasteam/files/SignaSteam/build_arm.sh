#!/bin/bash

# Build script for ARM target using Yocto SDK

# Source the Yocto SDK environment
source /opt/vtlinux/vt-sbc-am62l/6.6-scarthgap/environment-setup-aarch64-vtlinux-linux

# Clean previous build
echo "Cleaning previous build..."
make clean 2>/dev/null || true
rm -f Makefile *.o moc_* ui_* qrc_* SignaSteam

# Find qmake from the SDK
QMAKE_PATH=/opt/vtlinux/vt-sbc-am62l/6.6-scarthgap/sysroots/x86_64-vtlinuxsdk-linux/usr/bin/qmake

echo "Using qmake: $QMAKE_PATH"
echo "Target sysroot: $SDKTARGETSYSROOT"
echo "Cross compiler: $CC"

# Generate Makefile with ARM configuration
echo "Generating Makefile for ARM target..."
# Add GLES3 workaround include path (SDK only has GLES2)
$QMAKE_PATH SignaSteam.pro "CONFIG+=ARM" "INCLUDEPATH+=/home/etanner/SignaSteam/SignaSteam/gles3_workaround"

# Build the project
echo "Building for ARM..."
make -j$(nproc)

# Check if build was successful
if [ -f SignaSteam ]; then
    echo ""
    echo "=========================================="
    echo "Build successful!"
    echo "=========================================="
    file SignaSteam
    ls -lh SignaSteam
else
    echo ""
    echo "=========================================="
    echo "Build failed!"
    echo "=========================================="
    exit 1
fi
