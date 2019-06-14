# RPI QA Tools

A collection of tools and methods to run interactive tests on the RaspberryPI,
by simulating Keyboard, Mouse and Touch events programmatically.

## qa-hid

A program tool that simulates keyboard, mouse and touch events. On Linux, tt accomplishes this by accessing
the device `/proc/uinput` which is controlled by the uinput kernel module.

 * https://www.kernel.org/doc/html/v4.12/input/uinput.html

## qa-findimage

A program tool that is capable of locating the screen coordinates of graphic elements on the Raspberry PI screen.
This program uses the well-known opencv library to compare images, combined with real-time screenshots from the GPU.

 * http://docs.opencv.org/2.4/doc/tutorials/imgproc/histograms/template_matching/template_matching.html

