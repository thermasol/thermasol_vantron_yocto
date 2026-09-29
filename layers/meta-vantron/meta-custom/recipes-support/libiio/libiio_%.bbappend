# Local (sysfs) backend only. The default PACKAGECONFIG enables the usb,
# network and serial backends, which drag in libusb1, libxml2 and
# libserialport. We only ship libiio for iio_info / iio_attr as an on-board
# sanity check of the room-temperature ADC, so none of those are needed.
PACKAGECONFIG = ""
