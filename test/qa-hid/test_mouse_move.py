#
#  Pytest to test the Python interface module for qa-hid
#

from qahid import *

def test_key_press():
    assert (key_press('a') == True)

def test_key_stroke():
    assert (key_stroke('b') == True)

def test_key_stroke_delay():
    assert (key_stroke('c', delay=100) == True)

def test_mouse_move():
    assert (mouse_move(10, 10) == True)

def test_mouse_move_scale():
    assert (mouse_move(10, 10, scale=0.5) == True)

def test_mouse_left_click():
    assert (mouse_left_click() == True)
