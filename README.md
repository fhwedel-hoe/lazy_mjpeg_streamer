# lazy_mjpeg_streamer

Low-latency MJPEG-over-HTTP server.

This server differs from ffserver, [mjpg-streamer](https://github.com/jacksonliam/mjpg-streamer) and [streameye](https://github.com/ccrisan/streameye) as this server only captures and compresses frames as long as there are actually viewers. If there are no viewers, the camera is switched off. This helps lowering overall power consumption. Additionally, the camera is not in need to be cooled permanently, possibly increasing camera life.

In case of multiple viewers, the server performs the data duplication. There may be many viewers even if the camera itself supports only one viewer at a time.

Depends on boost::asio and libjpeg-turbo (aka. libturbojpeg).

Can also compress with WebP instead of JPEG.

Tested on Ubuntu 24.04 amd64 Linux.

Available sources:

* Cameras supported by the [IDS Software Suite 4.96.1](https://de.ids-imaging.com/download-details/AB02574.html?os=linux&version=&bus=64).
  Tested with a GigE uEye UI-6230SE-C-HQ Rev.2 camera.

* Cameras supported by the Intel [librealsense 1](https://github.com/IntelRealSense/librealsense/tree/v1.12.1).
  Tested with a R200 camera.

* Cameras supported by the Basler [pylon 8 SDK](https://www.baslerweb.com/en/downloads/).
  Tested with a USB3 Dart daA1600-60uc camera.
