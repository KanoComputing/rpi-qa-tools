#!/usr/bin/python
#
# Python pytest module for the qa-findimage, using exact images
#

from qafindimage import *

def test_find_minecraft():
    assert (find_exact_asset ('hack-minecraft.png', 444, 367, 103, 102))

def test_find_kano_code():
    assert (find_exact_asset ('kano-code.png', 448, 223, 96, 98))

def test_find_make_art():
    assert (find_exact_asset ('make-art.png', 589, 222, 103, 101))

def test_find_make_pong():
    assert (find_exact_asset ('make-pong.png', 734, 366, 101, 101))

def test_find_make_snake():
    assert (find_exact_asset ('make-snake.png', 445, 512, 101, 102))

def test_find_projects():
    assert (find_exact_asset ('projects.png', 734, 221, 102, 103))

def test_find_scratch():
    assert (find_exact_asset ('scratch.png', 590, 367, 101, 102))

def test_find_song_maker():
    assert (find_exact_asset ('song-maker.png', 733, 512, 102, 98))

def test_find_terminal_quest():
    assert (find_exact_asset ('terminal-quest.png', 590, 511, 101, 101))
