/* hidlib.cpp */

/* The MIT License
 *
 * Copyright (c) 2009 Scott Tsai
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// Use /dev/input/uinput under Linux to simulate mouse and keyboard input, see:
// http://www.einfochips.com/download/dash_jan_tip.pdf
// http://www.geocities.com/gnumohan/Linux/uinput.txt
// http://lists.freedesktop.org/pipermail/xorg/2008-November/040446.html

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>

#include <fcntl.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <libgen.h>

#include <linux/input.h>
#include <linux/uinput.h>

#include "hidlib.h"

// uinput_open() -> fd
int uinput_open(void)
{
  char program_name[] = "fakehid";
    
  int fd, r, i;
  struct uinput_user_dev uidev;

  if ((fd = open(UINPUT_DEVICE_PATH, O_WRONLY | O_NDELAY)) < 0) {
      //fprintf(stderr, "%s: openining \"%s\" failed: %s\n", program_name, UINPUT_DEVICE_PATH, strerror(errno));
      exit(3);
  }

  bzero(&uidev, sizeof(uidev));
  strncpy(uidev.name, "simulated input device", 20);
  
  // see: /sys/class/input/input*/id/version
  uidev.id.version = 4;
  uidev.id.bustype = BUS_USB;

  r = ioctl(fd, UI_SET_EVBIT, EV_KEY);
  assert(r == 0);
  r = ioctl(fd, UI_SET_EVBIT, EV_REP);
  assert(r == 0);
  r = ioctl(fd, UI_SET_EVBIT, EV_REL);
  assert(r == 0);
  r = ioctl(fd, UI_SET_RELBIT, REL_X);
  assert(r == 0);
  r = ioctl(fd, UI_SET_RELBIT, REL_Y);
  assert(r == 0);

  r = ioctl(fd, UI_SET_KEYBIT, BTN_MOUSE);
  assert(r == 0);
  r = ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);
  assert(r == 0);

  
  for (i = 0; i < 256; i++) {
    r = ioctl(fd, UI_SET_KEYBIT, i);
    assert(r == 0);
  }

  r = ioctl(fd, UI_SET_KEYBIT, BTN_MOUSE);
  assert(r == 0);

  // create input device in input subsystem
  r = write(fd, &uidev, sizeof(uidev));
  //fprintf(stderr, "%s: write returned: %d\n", program_name, r);

  r = (ioctl(fd, UI_DEV_CREATE));
  //fprintf(stderr, "%s: ioctl UI_DEV_CREATE returned: %d\n", program_name, r);
  assert(r == 0);

  wait_until_uinput_device_is_connected_to_xserver();
  
  return fd;
}

void uinput_close(int fd)
{
  int r;
  r = ioctl(fd, UI_DEV_DESTROY);
  assert(r == 0);
  close(fd);
}


/* assume that the mouse pointer is currently at 0,0 top-left */
void move_mouse_absolute(int uifd, int x, int y)
{
  struct input_event event;

  // X axis mouse movement
  bzero(&event, sizeof(event));
  gettimeofday(&event.time, NULL);
  event.type = EV_REL;
  event.code = REL_X;
  event.value = x;
  write(uifd, &event, sizeof(event));

  bzero(&event, sizeof(event));
  gettimeofday(&event.time, NULL);
  event.type = EV_SYN;
  event.code = SYN_REPORT;
  event.value = 0;
  write(uifd, &event, sizeof(event));

  // Y axis mouse movement
  bzero(&event, sizeof(event));
  gettimeofday(&event.time, NULL);
  event.type = EV_REL;
  event.code = REL_Y;
  event.value = y;
  write(uifd, &event, sizeof(event));

  bzero(&event, sizeof(event));
  gettimeofday(&event.time, NULL);
  event.type = EV_SYN;
  event.code = SYN_REPORT;
  event.value = 0;
  write(uifd, &event, sizeof(event));

  return;
}


void move_mouse_one_pixel(int uifd)
{
  // move pointer left by 1 pixel
  struct input_event event;

  bzero(&event, sizeof(event));
  gettimeofday(&event.time, NULL);
  event.type = EV_REL;
  event.code = REL_X;
  event.value = -1;
  write(uifd, &event, sizeof(event));

  bzero(&event, sizeof(event));
  gettimeofday(&event.time, NULL);
  event.type = EV_SYN;
  event.code = SYN_REPORT;
  event.value = 0;
  write(uifd, &event, sizeof(event));
}


void move_mouse_upper_left(int uifd)
{
  int i;
  struct input_event event;
  for (i = 0; i < N_MOUSE_EVENTS; i++) {
    // move pointer up and left by 5 pixels
    bzero(&event, sizeof(event));
    gettimeofday(&event.time, NULL);
    event.type = EV_REL;
    event.code = REL_X;
    event.value = -5;
    write(uifd, &event, sizeof(event));

    bzero(&event, sizeof(event));
    gettimeofday(&event.time, NULL);
    event.type = EV_REL;
    event.code = REL_Y;
    event.value = -5;
    write(uifd, &event, sizeof(event));

    bzero(&event, sizeof(event));
    gettimeofday(&event.time, NULL);
    event.type = EV_SYN;
    event.code = SYN_REPORT;
    event.value = 0;

    write(uifd, &event, sizeof(event));

    // delay
    usleep(16 * 1000);
  }
}

void move_mouse_lower_right_left(int uifd)
{
  int i;
  struct input_event event;
  for (i = 0; i < N_MOUSE_EVENTS; i++) {
    // move pointer up and left by 5 pixels
    bzero(&event, sizeof(event));
    gettimeofday(&event.time, NULL);
    event.type = EV_REL;
    event.code = REL_X;
    event.value = 5;
    write(uifd, &event, sizeof(event));

    bzero(&event, sizeof(event));
    gettimeofday(&event.time, NULL);
    event.type = EV_REL;
    event.code = REL_Y;
    event.value = 5;
    write(uifd, &event, sizeof(event));

    bzero(&event, sizeof(event));
    gettimeofday(&event.time, NULL);
    event.type = EV_SYN;
    event.code = SYN_REPORT;
    event.value = 0;
    write(uifd, &event, sizeof(event));

    // delay
    usleep(16 * 1000);
  }
}

void move_mouse_top_left(int uifd, int screen_width, int screen_height)
{
  int i;
  struct input_event event;

  for (i = 0; i < screen_width / 10; i++) {

    bzero(&event, sizeof(event));
    gettimeofday(&event.time, NULL);
    event.type = EV_REL;
    event.code = REL_X;
    event.value = -10;
    write(uifd, &event, sizeof(event));

    bzero(&event, sizeof(event));
    gettimeofday(&event.time, NULL);
    event.type = EV_REL;
    event.code = REL_Y;
    event.value = -10;
    write(uifd, &event, sizeof(event));

    bzero(&event, sizeof(event));
    gettimeofday(&event.time, NULL);
    event.type = EV_SYN;
    event.code = SYN_REPORT;
    event.value = 0;
    write(uifd, &event, sizeof(event));

    // delay
    usleep(3 * 1000);
  }
}

void mouse_left_click(int uifd)
{
  struct input_event event;

  bzero(&event, sizeof(event));
  gettimeofday(&event.time, NULL);
  event.type = EV_KEY;
  event.code = BTN_LEFT;
  event.value = 1;
  write(uifd, &event, sizeof(event));

  /*
  bzero(&event, sizeof(event));
  gettimeofday(&event.time, NULL);
  event.type = EV_KEY;
  event.code = BTN_LEFT;
  event.value = 0;
  write(uifd, &event, sizeof(event));
  */

  //bzero(&event, sizeof(event));
  //gettimeofday(&event.time, NULL);
  event.type = EV_SYN;
  event.code = SYN_REPORT;
  event.value = 0;
  write(uifd, &event, sizeof(event));


  bzero(&event, sizeof(event));
  gettimeofday(&event.time, NULL);
  event.type = EV_KEY;
  event.code = BTN_LEFT;
  event.value = 0;
  write(uifd, &event, sizeof(event));

  event.type = EV_SYN;
  event.code = SYN_REPORT;
  event.value = 0;
  write(uifd, &event, sizeof(event));
}

void wait_until_uinput_device_is_connected_to_xserver(void)
{
  // should be a NOP if X is configured to accept input from /dev/input/uinput by default
  // I'll cheat and delay for sometime to make it work on Desktop Linux
  usleep(1000 * 1000);
}

// use 'evtest' or see:
// http://www2.apebox.org/wordpress/wp-content/uploads/2007/09/cakemote.py
// to see how combination keys are handled
void single_key_send(int fd, int code, int value)
{
  struct input_event event;
  bzero(&event, sizeof(event));
  gettimeofday(&event.time, NULL);
  event.type = EV_KEY;
  event.code = code;
  event.value = value;
  write(fd, &event, sizeof(event));

  bzero(&event, sizeof(event));
  gettimeofday(&event.time, NULL);
  event.type = EV_SYN;
  event.code = SYN_REPORT;
  write(fd, &event, sizeof(event));
}

void single_key_press(int fd, int code)
{
  // see: linux-2.6/Documentation/input/input.txt
  // "0 for EV_KEY for release, 1 for keypress and 2 for autorepeat."
  single_key_send(fd, code, 1);
}

void single_key_release(int fd, int code)
{
  single_key_send(fd, code, 0);
}

void multi_key_press(int fd, int code)
{
  struct input_event event;
  bzero(&event, sizeof(event));
  gettimeofday(&event.time, NULL);
  event.type = EV_KEY;
  event.code = code;
  event.value = 1;
  write(fd, &event, sizeof(event));
}

void multi_key_press_end(int fd)
{
  struct input_event event;
  bzero(&event, sizeof(event));
  gettimeofday(&event.time, NULL);
  event.type = EV_SYN;
  event.code = SYN_REPORT;
  write(fd, &event, sizeof(event));
}

int ascii_to_linux_key(char c)
{
  const int linux_keys[] = {
    KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G,
    KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P,
    KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
  };
  assert('a' <= c && c <= 'z');
  return linux_keys[c - 'a'];
}

void type_msg(int fd, const char *m)
{
  char c;
  int t;
  for ( ; *m != '\0'; m++) {
    c = *m;
    if (isupper(c)) {
      t = ascii_to_linux_key(tolower(c));
      multi_key_press(fd, KEY_LEFTSHIFT);
      multi_key_press(fd, t);
      multi_key_press_end(fd);
      single_key_release(fd, KEY_LEFTSHIFT);
      single_key_release(fd, t);
    } else if (islower(c)) {
      t = ascii_to_linux_key(c);
      single_key_press(fd, t);
      single_key_release(fd, t);
    } else if (c == ' ') {
      t = KEY_SPACE;
      single_key_press(fd, t);
      single_key_release(fd, t);
    } else if (c == '!') {
      t = KEY_1;
      multi_key_press(fd, KEY_LEFTSHIFT);
      multi_key_press(fd, t);
      multi_key_press_end(fd);
      single_key_release(fd, KEY_LEFTSHIFT);
      single_key_release(fd, t);
    } else {
      t = KEY_X;
      single_key_press(fd, t);
      single_key_release(fd, t);
    }
    usleep(100 * 1000);
  }
}

void type_dull_boy(int uifd)
{
  int i;
  const char msg[] = "No work and no play makes Scott a dull boy!";

  wait_until_uinput_device_is_connected_to_xserver();
  
  for (i = 0; i < N_DULL_BOY; i++)
      type_msg(uifd, msg);
}

