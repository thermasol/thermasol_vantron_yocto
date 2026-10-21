# Copyright (C) 2021,Vantron - All Rights Reserved
FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

# One splash binary per product, instead of one global image.
#
# The base recipe (poky/meta/recipes-core/psplash/psplash_git.bb) compiles
# the PNG into the psplash executable itself, but it supports building
# several: each entry in SPLASH_IMAGES produces its own package,
# psplash-<outsuffix>, shipping ${bindir}/psplash-<outsuffix> and
# registering it as an update-alternatives provider of /usr/bin/psplash.
# So the image picks its splash by installing the matching package - see
# the RDEPENDS in each app recipe, which is what ties artwork to product.
#
# Filenames here are the product, not the artwork: signatouch-ir.png is
# the Almost Heaven Saunas wordmark that shipped as "yocto.png" before
# this split - re-rendered white on black (the original was a black
# wordmark on transparency, invisible on the black background below; it
# only ever showed on upstream's grey). The kernel boot logo for the IR
# product (recipes-kernel/linux, logo_almostheaven_clut224.ppm) is made
# from this file; keep them in step. signatouch.png is the ThermaSol wordmark (tsol_logo_305x27.png
# flattened onto black and padded to 308x27 - psplash_fb_draw_image wraps a
# row when dx*bytes_per_pixel reaches the 4-byte-aligned rowstride, so any
# RGB image whose width is not a multiple of 4 is drawn as a diagonal smear;
# 305 wide came out sheared on the panel) - the same image the kernel draws as its
# boot logo (recipes-kernel/linux/files/logo_linux_clut224.ppm), so that
# psplash taking over the framebuffer at ~3 s changes nothing on screen.
# Keep the two in step if the artwork changes.
#
# "default" is what every other image in this layer (core, weston,
# qt5-minimal, factory) gets by way of psplash's own RDEPENDS, and it is
# back to the stock Yocto placeholder. Before this split those images were
# picking up the Almost Heaven logo, which was never intentional - it was
# just the one global SPLASH_IMAGES entry.
SPLASH_IMAGES = "file://signatouch.png;outsuffix=signatouch \
                 file://signatouch-ir.png;outsuffix=signatouch-ir \
                 file://yocto.png;outsuffix=default \
                "

# psplash-<product> RDEPENDS on psplash, and psplash RDEPENDS on
# psplash-default, so psplash-default is always installed alongside the
# product package and both register /usr/bin/psplash as an alternative.
# The base recipe gives every one of them ALTERNATIVE_PRIORITY 100, and a
# tie is resolved arbitrarily - which would make the splash a coin flip.
# Outranking the default is what makes the product artwork deterministic.
ALTERNATIVE_PRIORITY_psplash-signatouch = "200"
ALTERNATIVE_PRIORITY_psplash-signatouch-ir = "200"

# Match the kernel boot logo so the handoff is seamless:
#
#  - "fullscreen" (kept from the default PACKAGECONFIG) is misnamed: it does
#    not scale anything, it centres the image in the whole framebuffer
#    instead of in the top 5/6 - which is exactly where fbcon puts a
#    logo-pos:center logo.
#  - the progress bar would appear under the logo at the moment psplash
#    starts, then vanish when the app takes the display ~4 s later. Off.
#  - psplash-colors.h: black background instead of upstream's light grey.
#    The header has no #ifndef guards, so it is replaced, not overridden.
PACKAGECONFIG:remove = "progress-bar"

SRC_URI += "file://psplash-colors.h"

do_configure:prepend() {
    cp ${WORKDIR}/psplash-colors.h ${S}/psplash-colors.h
}
