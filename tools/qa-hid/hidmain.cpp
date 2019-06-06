/**
 * himain.cpp
 *
 * Copyright (C) 2019 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU GPL v2
 *
 * Command line frontend to the fakehid functions.
 *
 */


#include <docopt/docopt.h>

#include <linux/input.h>
#include <linux/uinput.h>
#include <linux/input-event-codes.h>

#include <stdio.h>
#include <unistd.h>

#include <cstdint>
#include <iostream>
#include <map>
#include <string>

#include "qa-hid/hidlib.h"


static const char USAGE[] =
  R"(fakehid is a tool that mimics different input types.

Usage:
    fakehid key-press <key>
    fakehid key-release <key>
    fakehid key-stroke <key> [--delay <time_ms>]
    fakehid mouse-move <abs_x> <abs_y> [--scale <factor>]
    fakehid mouse-press <button>
    fakehid mouse-release <button>
    fakehid mouse-click <button> [--delay <time_ms>]
    fakehid touch-press <touch_id> <abs_x> <abs_y>
    fakehid touch-release <touch_id> <abs_x> <abs_y>
    fakehid touch-click <touch_id> <abs_x> <abs_y> [--delay <time_ms>]
    fakehid -h | --help

Commands:
    key-press
        TODO
    key-release
        TODO
    key-stroke
        TODO
    mouse-move
        TODO
    mouse-press
        TODO
    mouse-release
        TODO
    mouse-click
        Perform a mouse press and release at an absolute position on the screen
        and with a certain delay in between.
    touch-press
        TODO
    touch-release
        TODO
    touch-click
        TODO

Arguments:
    <key>
        () TODO
    <abs_x>
        (int) Absolute X position on the screen.
    <abs_y>
        (int) TODO
    <button>
        BTN_LEFT, BTN_RIGHT
    <time_ms>
        (int) TODO
    <touch_id>
        () TODO

Options:
    -d, --delay
        TODO
    -h, --help   
        Show this message.

Examples:
    fakehid mouse-click 300 500 --delay 50)";


std::map <std::string, int> special_keys = {
    { std::string("KEY_ENTER"), KEY_ENTER },
    { std::string("KEY_ESC"), KEY_ESC },
    { std::string("KEY_TAB"), KEY_TAB }
};



int main(int argc, char **argv)
{
    int uifd;
    int rc = 0;

    // Parsing command line arguments with docopt.
    std::map<std::string, docopt::value> args =
        docopt::docopt(USAGE, {argv + 1, argv + argc});

    if (args["key-press"].asBool()) {
        std::string key = args["<key>"].asString();
        std::cout << "sending key-press: " << key.c_str() << std::endl;
        uifd = uinput_open();

        std::map<std::string, int>::iterator it;
        it = special_keys.find(key);

        if (it != special_keys.end()) {
            std::cout << "Sending special key-press " << it->first
                      << " : " << it->second << std::endl;
            single_key_press(uifd, it->second);
        } else {
            std::cout << "Sending key-press: " << key.c_str()[0] << std::endl;
            single_key_press(uifd, ascii_to_linux_key(key.c_str()[0]));
        }
    } else if (args["key-stroke"].asBool()) {
        std::string key = args["<key>"].asString();
        int32_t delay = static_cast<int32_t>(args["<time_ms>"].asLong());

        std::cout << "sending key-stroke: " << key.c_str()
                  << " with delay: " << delay << std::endl;
        uifd = uinput_open();
        single_key_press(uifd, ascii_to_linux_key(key.c_str()[0]));
        std::cout << "delay" << std::endl;
        usleep(100 * delay);
        single_key_release(uifd, ascii_to_linux_key(key.c_str()[0]));
    } else if (args["mouse-click"].asBool()) {
        std::cout << "sending mouse-click: "
                  << args["<button>"].asString().c_str() << std::endl;
        uifd = uinput_open();

        if (args["<button>"].asString() == "BTN_LEFT") {
            mouse_left_click(uifd);
        }
    } else if (args["mouse-move"].asBool()) {

        std::cout << "absolute mouse position requested" << std::endl;
        long x = args["<abs_x>"].asLong();
        long y = args["<abs_y>"].asLong();

        // FIXME: detect that the option is not passed
        double factor = atof(args["<factor>"].asString().c_str());

        uifd = uinput_open();

        // TODO: Ideally we want to query the screen size programmatically, outside X1
        std::cout << "moving mouse to top left screen corner" << std::endl;
        move_mouse_top_left(uifd, 9999, 9999);
        std::cout << "moving mouse to absolute coordinates x=" << x << " y=" << y << std::endl;

        if (factor) {
            // translate coordinates to a given scale factor
            x = x * factor;
            y = y * factor;
            std::cout << "scale=" << factor << " translated x=" << x << " translated y=" << y << std::endl;
        }
        move_mouse_absolute(uifd, x, y);
    }

    uinput_close(uifd);
    exit(rc);
}
