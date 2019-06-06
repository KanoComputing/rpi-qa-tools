
#ifndef FAKELIB
  #define FAKELIB

enum {
  N_DULL_BOY = 3,
};

enum {
  N_MOUSE_EVENTS = 100,
};

#define UINPUT_DEVICE_PATH "/dev/uinput"

void type_dull_boy(int uifd);
void type_msg(int fd, const char *m);
int ascii_to_linux_key(char c);
void multi_key_press_end(int fd);
void multi_key_press(int fd, int code);
void single_key_release(int fd, int code);
void single_key_press(int fd, int code);
void single_key_send(int fd, int code, int value);
void wait_until_uinput_device_is_connected_to_xserver(void);
void mouse_left_click(int uifd);
void move_mouse_top_left(int uifd, int screen_width, int screen_height);
void move_mouse_absolute(int uifd, int x, int y);
void move_mouse_lower_right_left(int uifd);
void move_mouse_upper_left(int uifd);
void move_mouse_one_pixel(int uifd);
void uinput_close(int fd);
int uinput_open(void);

#endif
