# Replaces upstream's TsolArcSlider.pro for the Yocto build. Upstream builds
# a Qt Designer plugin (QT += designer, links against
# QtUiPlugin/QDesignerCustomWidgetInterface) so the widget can be previewed
# inside Qt Creator - that machinery has no purpose on the target device.
# Only the ArcSlider widget itself (arcslider.cpp/h) is needed at runtime, so
# this builds just that as a plain shared library, keeping the same output
# name (libarcsliderplugin.so) that SignaTouch_IR.pro already links against.

TARGET      = arcsliderplugin
TEMPLATE    = lib
CONFIG      += plugin
QT          += core gui widgets svg

SOURCES     = arcslider.cpp
HEADERS     = arcslider.h
RESOURCES   = sliderresources.qrc
