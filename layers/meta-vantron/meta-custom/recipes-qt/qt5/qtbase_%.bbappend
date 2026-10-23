# qtbase_%.bbappend matches both meta-qt5's and meta-qt6's qtbase_git.bb
# (see ../qt6/qtbase_%.bbappend, which gates its content the same way), so
# everything here is applied only when the base recipe is meta-qt5's.
FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

# eglfs: QEglFSWindow::destroy() deletes the raster compositing context of
# the root window but leaves qt_gl_global_share_context() pointing at it.
# The next top-level window shares with freed memory and the app segfaults
# in QEglFSIntegration::createPlatformOpenGLContext. Qt fixed it in 6.x
# (f3db9abebf7e) and never backported it to 5.15. SignaSteam's first-boot
# flow (orientation dialog -> setup wizard -> MainWindow, three successive
# top-level windows) crashes on the wizard's Finish without it.
#
# eglfs_kms: the KMS mode (and with it the primary plane's framebuffer) is
# set once per screen. Destroying the window that owns the gbm_surface
# removes its framebuffers and the kernel unbinds the plane; the next
# window's page flips then all fail with EBUSY and the panel stays black
# (QTBUG-122663, fixed in Qt 6.5). Same first-boot flow, next window along.
QTBASE_EGLFS_PATCHES = " \
    file://0001-eglfs-Unset-shared-raster-compositing-context-before.patch \
    file://0002-eglfs-kms-Fix-having-more-than-one-window-over-the-screen-s-lifetime.patch \
"
SRC_URI += "${@d.getVar('QTBASE_EGLFS_PATCHES') if 'meta-qt5' in d.getVar('FILE') else ''}"
