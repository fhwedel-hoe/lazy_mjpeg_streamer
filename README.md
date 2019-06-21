# lazy_mjpeg_streamer
MJPEG-over-HTTP server.

This server differs from ffserver, [mjpg-streamer](https://github.com/jacksonliam/mjpg-streamer) and [streameye](https://github.com/ccrisan/streameye) as this server only captures and compresses frames as long as there are actually viewers. If there are no viewers, the camera is switched off.

Depends on libjpeg-turbo and boost::asio.

Available sources:

* Cameras supported by the [IDS Imaging SDK](https://de.ids-imaging.com/download-ueye-lin64.html).
  Tested with a GigE uEye camera on Ubuntu 18.04 amd64 Linux.

* Cameras supported by the Intel [librealsense 1](https://github.com/IntelRealSense/librealsense/tree/v1.12.1).
  Tested with a R200 camera on Ubuntu 18.04 amd64 Linux.
