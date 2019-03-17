/**
 * rpi-screenshot.h
 *
 * Copyright (C) 2019 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * Wrapper code to obtain a bitmap image from the RaspberryPI screen.
 *
 */


#ifndef TOOLS_QA_FINDIMAGE_RPI_SCREENSHOT_H_
#define TOOLS_QA_FINDIMAGE_RPI_SCREENSHOT_H_

#include <memory>
std::shared_ptr<void> get_rpi_screenshot(bool verbose, int *pwidth,
                                         int *pheight);

#endif  // TOOLS_QA_FINDIMAGE_RPI_SCREENSHOT_H_
