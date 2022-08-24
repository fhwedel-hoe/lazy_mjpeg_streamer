# lazy_mjpeg_streamer

Low-latency MJPEG-over-HTTP server.

This server differs from ffserver, [mjpg-streamer](https://github.com/jacksonliam/mjpg-streamer) and [streameye](https://github.com/ccrisan/streameye) as this server only captures and compresses frames as long as there are actually viewers. If there are no viewers, the camera is switched off. This helps lowering overall power consumption and development of heat for a longer camera life.

Depends on boost::asio and libjpeg-turbo (aka. libturbojpeg).

Can also compress with WebP instead of JPEG.

Tested on Ubuntu 22.04 amd64 Linux.

Available sources:

* Cameras supported by the [IDS Imaging SDK](https://de.ids-imaging.com/download-ueye-lin64.html).
  Tested with a GigE uEye camera.

* Cameras supported by the Intel [librealsense 1](https://github.com/IntelRealSense/librealsense/tree/v1.12.1).
  Tested with a R200 camera.

* Cameras supported by the Basler [pylon 5 SDK](https://www.baslerweb.com/en/sales-support/downloads/software-downloads/).
  Tested with a Dart daA1600-60uc camera.
