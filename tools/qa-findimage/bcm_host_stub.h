/**
 * bcm_host_stub.h
 *
 * Copyright (C) 2019 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * Stub replacement for bcm_host.h
 *
 */


#ifndef TOOLS_QA_FINDIMAGE_BCM_HOST_STUB_H_
#define TOOLS_QA_FINDIMAGE_BCM_HOST_STUB_H_


#include <cstdint>


typedef int DISPMANX_DISPLAY_HANDLE_T;
typedef int DISPMANX_RESOURCE_HANDLE_T;
typedef struct {
    int width;
    int height;
} DISPMANX_MODEINFO_T;
typedef int VC_IMAGE_TYPE_T;
typedef struct {
    int x;
    int y;
    int width;
    int height;
} VC_RECT_T;


void bcm_host_init() {}
void bcm_host_deinit() {}
DISPMANX_RESOURCE_HANDLE_T vc_dispmanx_resource_create(VC_IMAGE_TYPE_T, int,
                                                       int, uint32_t*)
{
    return 0;
}

DISPMANX_DISPLAY_HANDLE_T vc_dispmanx_display_open(uint32_t)
{
    return 0;
}

int vc_dispmanx_display_close(DISPMANX_DISPLAY_HANDLE_T)
{
    return 0;
}

int vc_dispmanx_display_get_info(DISPMANX_DISPLAY_HANDLE_T,
                                 DISPMANX_MODEINFO_T*)
{
    return 0;
}

int vc_dispmanx_resource_delete(DISPMANX_DISPLAY_HANDLE_T)
{
    return 0;
}

int vc_dispmanx_snapshot(DISPMANX_DISPLAY_HANDLE_T, int, int)
{
    return 0;
}

int vc_dispmanx_rect_set(VC_RECT_T*, int, int, int, int)
{
    return 0;
}

int vc_dispmanx_resource_read_data(DISPMANX_RESOURCE_HANDLE_T, VC_RECT_T*,
                                   void*, int)
{
    return 0;
}


VC_IMAGE_TYPE_T VC_IMAGE_RGB888 = 0;
int DISPMANX_NO_ROTATE = 0;


#endif  // TOOLS_QA_FINDIMAGE_BCM_HOST_STUB_H_
