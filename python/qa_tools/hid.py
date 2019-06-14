#
# hid.py
#
# Copyright (C) 2019 Kano Computing Ltd.
# License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPLv2
#
# Access to qa-hid keyboard simulator functions
#

import os

QA_HID = 'sudo /usr/bin/qa-hid'


def _call_hid_pipe(options):
    os.popen('{} {}'.format(QA_HID, options))

def _call_hid(options):
    return os.WEXITSTATUS(os.system('{} {}'.format(QA_HID, options)))



def key_press(key):
    options = 'key-press {}'.format(key)
    return (_call_hid(options) == 0)

def key_stroke(key, delay=0):
    options = 'key-stroke {} --delay {}'.format(key, delay)
    return (_call_hid(options) == 0)

def mouse_move(abs_x, abs_y):
    options = 'mouse-move {} {}'.format(abs_x, abs_y)
    return (_call_hid(options) == 0)

def mouse_move(abs_x, abs_y, scale=0.5):
    options = 'mouse-move {} {} --scale {}'.format(abs_x, abs_y, scale)
    return (_call_hid(options) == 0)

def mouse_left_click():
    return (_call_hid('mouse-click BTN_LEFT') == 0)
