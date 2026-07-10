#DEPENDS += " virtual/libgles2 virtual/egl"
#do_configure:prepend() {
#    rm -f ${STAGING_LIBDIR}/libEGL.so.1
#    ln -sf ${STAGING_LIBDIR}/libmali.so.1.9.0 ${STAGING_LIBDIR}/libEGL.so.1
#}