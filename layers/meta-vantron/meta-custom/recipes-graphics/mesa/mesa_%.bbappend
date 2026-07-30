# AM62L has no dedicated 3D GPU — display is TI's TIDSS controller only, with
# rendering done via software (llvmpipe). Mesa's "kmsro" Gallium driver is
# what provides the KMS/GBM-side "tidss" DRI driver for exactly this kind of
# display-only SoC, but it isn't in the default PACKAGECONFIG, so no
# tidss_dri.so ever got built — Qt5 EGLFS silently fell back to swrast
# entirely instead of at least using tidss for KMS/scanout.
PACKAGECONFIG:append = " kmsro"
