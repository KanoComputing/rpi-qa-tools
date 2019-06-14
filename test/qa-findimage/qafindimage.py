#
# Wrapper module to import the qa-findimage python interface modules
#

# Allow for calling the qa-findimage built through this repo
import os
os.environ['PATH'] = '../tools/qa-findimage:'.join(os.getenv('PATH'))

# The python interface module for qa-findimage
import sys
sys.path.append('../python/qa_tools')
from findimage import *

# Paths to data image files
dashboard_screenshot = 'qa-findimage/data/kano-dashboard.png'
asset_path_exact = 'qa-findimage/data/exact_size'
asset_path_scaled_down = 'qa-findimage/data/88x88'
asset_path_scaled_up = 'qa-findimage/data/188x188'

def find_asset(asset, expected_x, expected_y, expected_width, expected_height, accuracy=16):
    '''
    Calls qa-findimage through the official Python module, to locate the asset
    at the given coordinates with the specified size, all within plus/minus "accuracy" pixels.
    '''
    assert (is_asset_on_image(dashboard_screenshot, asset) == True)
    j = get_json_from_image (dashboard_screenshot, asset)
    assert (j['found'] == True)

    assert (expected_x in range (j['x'] - accuracy, j['x'] + accuracy))
    assert (expected_y in range (j['y'] - accuracy, j['y'] + accuracy))
    assert (expected_width in range (j['width'] - accuracy, j['width'] + accuracy))
    assert (expected_height in range (j['height'] - accuracy, j['height'] + accuracy))
    return True

def find_exact_asset(asset, x, y, width, height):
    find_asset (os.path.join(asset_path_exact, asset), x, y, width, height)
    return True

def find_scaled_down_asset(asset, x, y, width, height):
    find_asset (os.path.join(asset_path_scaled_down, asset), x, y, width, height)
    return True

def find_scaled_up_asset(asset, x, y, width, height):
    find_asset (os.path.join(asset_path_scaled_up, asset), x, y, width, height)
    return True
