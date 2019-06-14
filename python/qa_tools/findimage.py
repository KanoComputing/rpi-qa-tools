#
# findimage.py
#
# Copyright (C) 2019 Kano Computing Ltd.
# License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPLv2
#
# Access to the qa-findimage OpenCV program
#

import os
import json


def call_findimage(source_image, asset_image, ui=False):
    '''
    Returns a json object as a result of executing qa-findimage command_line
    '''

    ui_flag = 'ui' if ui else ''

    # stderr to /dev/null due to this generic issue on some png files
    # libpng warning: iCCP: known incorrect sRGB profile
    command_line = 'qa-findimage {} {} {} 2>/dev/null'.format(
        source_image, asset_image, ui_flag
    )
    return json.loads(os.popen(command_line).read())

def is_asset_on_screen(asset):
    '''
    Returns True if the asset file is located on the screen
    '''
    findimage_json = call_findimage('dispmanx', asset)
    return findimage_json['found']

def is_asset_on_image(image, asset):
    '''
    Returns True if the asset file is located on the screen
    '''
    findimage_json = call_findimage(image, asset)
    return findimage_json['found']

def get_json_from_screen(asset):
    return call_findimage('dispmanx', asset)

def get_json_from_image(image, asset):
    return call_findimage(image, asset)
