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
# the Almost Heaven Saunas logo that shipped as "yocto.png" before this
# split. signatouch.png is STILL THE YOCTO PLACEHOLDER - drop the real
# artwork over it, no other change needed.
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
