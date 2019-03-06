/*
 *  rpi-screenshot.h
 *
 *  Wrapper code to obtain a bitmap image from the RaspberryPI screen.
 *
 */

void *get_rpi_screenshot(bool verbose,  int *pwidth, int *pheight);
