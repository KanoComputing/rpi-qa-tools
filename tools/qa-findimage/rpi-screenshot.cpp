/**
 * rpi-screenshot.cpp
 *
 * Copyright (C) 2019 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * This module grabs a screenshot from the raspberry PI GPU and returns a plain
 * bitmap array.
 *
 * Conditional compilation is in place to allow for building on other platforms.
 *
 */


#include "qa-findimage/rpi-screenshot.h"

#ifdef RPI

#include <bcm_host.h>

#else

#warning "Non RPi build, stub the dispmanx functions"
#include "qa-findimage/bcm_host_stub.h"

#endif  // RPI


#include <cstdint>
#include <cstdio>
#include <memory>


#ifndef ALIGN_TO_16
#define ALIGN_TO_16(x)  ((x + 15) & ~15)
#endif  // ALIGN_TO_16

/*
 *  get_rpi_screenshot
 *
 *  Takes a screenshot from Dispmanx and returns the data array,
 *  along with its width and height. Format returned is 3 bytes per pixel,
 *  in RGB colorspace.
 *
 */
void *get_rpi_screenshot(bool verbose, int *pwidth, int *pheight)
{
    int result = 0;

    bcm_host_init();

    DISPMANX_DISPLAY_HANDLE_T displayHandle = vc_dispmanx_display_open(0);

    DISPMANX_MODEINFO_T modeInfo;
    result = vc_dispmanx_display_get_info(displayHandle, &modeInfo);

    if (result != 0) {
        printf("unable to get display information\n");
        bcm_host_deinit();
        return nullptr;
    }

    // RGB colorspace, 3 bytes per pixel
    VC_IMAGE_TYPE_T imageType = VC_IMAGE_RGB888;
    int bytesPerPixel = 3;

    int width = modeInfo.width;
    int height = modeInfo.height;
    int pitch = bytesPerPixel * ALIGN_TO_16(width);

    void *dmxImagePtr = malloc(pitch * height);

    if (dmxImagePtr == nullptr) {
        printf("unable to allocated image buffer\n");
        bcm_host_deinit();
        return nullptr;
    }

    uint32_t vcImagePtr = 0;
    DISPMANX_RESOURCE_HANDLE_T resourceHandle;
    resourceHandle = vc_dispmanx_resource_create(imageType,
                                                 width,
                                                 height,
                                                 &vcImagePtr);

    result = vc_dispmanx_snapshot(displayHandle,
                                  resourceHandle,
                                  DISPMANX_NO_ROTATE);

    VC_RECT_T rect;

    result = vc_dispmanx_rect_set(&rect, 0, 0, width, height);
    if (verbose) {
        printf("vc_dispmanx_rect_set() returned %d\n", result);
        printf("rect = { %d, %d, %d, %d }\n",
               rect.x,
               rect.y,
               rect.width,
               rect.height);
    }

    if (result != 0) {
        vc_dispmanx_resource_delete(resourceHandle);
        vc_dispmanx_display_close(displayHandle);

        printf("vc_dispmanx_rect_set() failed\n");
        bcm_host_deinit();
        return nullptr;
    }

    // transfer the bitmap array
    result = vc_dispmanx_resource_read_data(resourceHandle,
                                            &rect,
                                            dmxImagePtr,
                                            pitch);

    if (result != 0) {
        vc_dispmanx_resource_delete(resourceHandle);
        vc_dispmanx_display_close(displayHandle);

        printf("vc_dispmanx_resource_read_data() failed\n");
        bcm_host_deinit();
        return nullptr;
    }

    result = vc_dispmanx_resource_delete(resourceHandle);

    if (verbose) {
        printf("vc_dispmanx_resource_delete() returned %d\n", result);
    }

    result = vc_dispmanx_display_close(displayHandle);

    if (verbose) {
        printf("vc_dispmanx_display_close() returned %d\n", result);
    }

    // explain the image size
    if (pwidth) {
        *pwidth = width;
    }

    if (pheight) {
        *pheight = height;
    }

    return dmxImagePtr;
}
