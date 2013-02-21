#ifndef MOUSE_H_
#define MOUSE_H_
#include <stdio.h>
#include <unistd.h>
#include "common.h"
typedef struct {
	int px, py;
	int tx, ty;
	int dx, dy;
	int button;
}mouse_t;
int open_mice(char * path);
int read_mice(int mfd, mouse_t * event, fb_info fb_inf);
int draw_mouse(fb_info fb_inf, mouse_t mouse);
int save_mouse(fb_info fb_inf, mouse_t mouse);
int recover_mouse(fb_info fb_inf, mouse_t mouse);
#endif
