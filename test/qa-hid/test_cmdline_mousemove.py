#
# pytest module for the qa-hid tool
#

import os

def call_hid(options):
    # FIXME: We want to get rid of sudo
    return os.WEXITSTATUS(os.system('sudo ../tools/qa-hid/qa-hid {}'.format(options)))

def pipe_hid(options):
    return (os.popen('{} {}'.format(hid, options)).read())


def test_mouse_move():
    options = 'mouse-move 100 100'
    assert (call_hid(options) == 0)

def test_mouse_move_scale():
    assert (call_hid('mouse-move 100 100 --scale 0.5') == 0)

def test_mouse_move_relative():
    assert (call_hid('mouse-move 100 100 --relative') == 0)

def test_mouse_scale_relative():
    assert (call_hid('mouse-move 100 100 --scale 0.5 --relative') == 0)
