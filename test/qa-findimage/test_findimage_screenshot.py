#
# test_findimage_screenshot.py
#
# This module makes sure that findimage is able to compare 2 screenshots
#

from qafindimage import *

def test_find_the_same_screenshot():
    is_asset_on_image(dashboard_screenshot, dashboard_screenshot)
