/*
 *  rpi-screenshot.h
 *
 *  Wrapper code to obtain a bitmap image from the RaspberryPI screen.
 *
 */

#ifndef RPI_SCREENSHOT

void *get_rpi_screenshot(bool verbose,  int *pwidth, int *pheight);

#endif
