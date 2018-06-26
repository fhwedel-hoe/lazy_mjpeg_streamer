# ueye_mjpeg_streamer
MJPEG-over-HTTP server for cameras supported by the IDS Imaging SDK.

This server differs from ffserver, [mjpeg-streamer](https://github.com/jacksonliam/mjpg-streamer) and [streameye](https://github.com/ccrisan/streameye).
This server only captures and compresses frames as long as there are actually viewers.

Based on the examples provided by the SDK available at https://de.ids-imaging.com/download-ueye-lin64.html.

Tested with a GigE uEye camera on 64bit Linux.

Depends on libjpeg-turbo and boost::asio.
