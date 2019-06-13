#
# pytest module for the qa-hid tool
#

import os

# TODO: implement a simulation mode so we do not need sudo
# and therefore can test the command line options on virtual test systems
hid = 'sudo ../tools/qa-hid/qa-hid'

def run_hid(options):
    return os.WEXITSTATUS(os.system('{} {}'.format(hid, options)))

def pipe_hid(options):
    return (os.popen('{} {}'.format(hid, options)).read())


def test_mouse_move():
    options = 'mouse-move 100 100'
    assert (run_hid(options) == 0)

def test_mouse_move_scale():
    assert (run_hid('mouse-move 100 100 --scale 0.5') == 0)

def test_mouse_move_relative():
    assert (run_hid('mouse-move 100 100 --relative') == 0)

def test_mouse_scale_relative():
    assert (run_hid('mouse-move 100 100 --scale 0.5 --relative') == 0)
