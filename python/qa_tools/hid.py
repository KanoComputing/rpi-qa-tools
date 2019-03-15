#
# hid.py
#
# Copyright (C) 2019 Kano Computing Ltd.
# License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPLv2
#
# Access to qa-hid keyboard simulator functions
#

import os

qa_hid='sudo /usr/bin/qa-hid'


def call_hid(command_line):
    os.popen(command_line)



def key_press(key):
    command_line='{} key-press {}'.format(qa_hid, key)
    call_hid(command_line)

def key_stroke(key, delay=0):
    command_line='{} key-stroke {} --delay {}'.format(qa_hid, key, delay)
    call_hid(command_line)



    
def mouse_move(abs_x, abs_y):
    command_line='{} mouse-move {} {}'.format(qa_hid, abs_x, abs_y)
    call_hid(command_line)

def mouse_left_click():
    command_line='{} mouse-click BTN_LEFT'.format(qa_hid)
    call_hid(command_line)
