#include <fcntl.h>
#include "mouse.h" 
#include "common.h"
#define C_WIDTH 10
#define C_HEIGHT 17
#define T_ 0x7777F
#define BORD 0x0
#define X_ 0xFFFF
static u32_t cursor_pixel[C_WIDTH * C_HEIGHT] = {
	BORD, T_, T_, T_, T_, T_, T_, T_, T_, T_,
	BORD, BORD, T_, T_, T_, T_, T_, T_, T_, T_,
	BORD, X_, BORD, T_, T_, T_, T_, T_, T_, T_,
	BORD, X_, X_, BORD, T_, T_, T_, T_, T_, T_,
	BORD, X_, X_, X_, BORD, T_, T_, T_, T_, T_,
	BORD, X_, X_, X_, X_, BORD, T_, T_, T_, T_,
	BORD, X_, X_, X_, X_, X_, BORD, T_, T_, T_,
	BORD, X_, X_, X_, X_, X_, X_, BORD, T_, T_,
	BORD, X_, X_, X_, X_, X_, X_, X_, BORD, T_,
	BORD, X_, X_, X_, X_, X_, X_, X_, X_, BORD,
	BORD, X_, X_, X_, X_, X_, BORD, BORD, BORD,BORD,
	BORD, X_, X_, BORD, X_, X_, BORD, T_, T_, T_,
	BORD, X_, BORD, T_, BORD, X_, X_, BORD,T_, T_,
	BORD, BORD, T_, T_, BORD, X_, X_, BORD, T_, T_,
	T_, T_, T_, T_, T_, BORD, X_, X_, BORD, T_,
	T_, T_, T_, T_, T_, BORD, X_, X_, BORD, T_,
	T_, T_, T_, T_, T_, T_, BORD, BORD, T_, T_
};
u32_t save_cursor[C_WIDTH * C_HEIGHT];

int open_mice(char * path)
{
	int fd;
	if(path == NULL)
		path = "/dev/input/mice";
	fd = open(path, O_RDWR | O_NONBLOCK);
	return fd;
}

int read_mice(int mfd, mouse_t * event, fb_info fb_inf)
{
	int ret;
	unsigned char buf[8];
	ret = read(mfd, buf, 8);
	if(ret < 0)
		return -1;
	event->button = buf[0] & 0x07;
	event->dx = -256 * ((buf[0] & 0x10)?1:0) + buf[1];
	event->dy = -256 * ((buf[0] & 0x20)?1:0) + buf[2];
	event->px = event->tx;
	event->py = event->ty;

	event->tx += event->dx;
	event->ty -= event->dy;
	if(event->tx < 0) event->tx = 0;
	if(event->tx >= fb_inf.w) event->tx = fb_inf.w - 1;
	if(event->ty < 0) event->ty = 0;
	if(event->ty >= fb_inf.h) event->ty = fb_inf.h - 1;
	return ret;
}
int save_mouse(fb_info fb_inf, mouse_t mouse)
{
	int i, j, x, y; 
	u8_t * p ;
	for (j = 0; j < C_HEIGHT; j++) 
	{
		for (i = 0; i < C_WIDTH; i++) 
		{
			x = mouse.tx + i;
			y = mouse.ty + j;
			if(x >= fb_inf.w || y >= fb_inf.h)
				continue;
			p = fb_inf.fbmem + (x + y * fb_inf.w)*4;
			save_cursor[i+j * C_WIDTH] = *(u32_t *)p;
		}
	}
	return 0;
}
int draw_mouse(fb_info fb_inf, mouse_t mouse)
{
	int i, j, x, y;
	u32_t color;
	save_mouse(fb_inf, mouse);
	for (j = 0; j < C_HEIGHT; j++) 
	{
		for (i = 0; i < C_WIDTH; i++) 
		{
			x = mouse.tx + i;
			y = mouse.ty + j;
			if(x >= fb_inf.w || y >= fb_inf.h)
				continue;
			color = cursor_pixel[i + j * C_WIDTH];
			if(color != T_)
				fb_pixel(fb_inf, x, y, color);
		}
	}
	return 0;
}
int recover_mouse(fb_info fb_inf, mouse_t mouse)
{
	int i, j, x, y; u32_t color;
	for (j = 0; j < C_HEIGHT; j++) 
	{
		for (i = 0; i < C_WIDTH; i++) 
		{
			x = mouse.px + i;
			y = mouse.py + j;
			if(x >= fb_inf.w || y >= fb_inf.h)
				continue;
			color = save_cursor[i + j * C_WIDTH];
			fb_pixel(fb_inf, x, y, color);
		}
	}
	return 0;
}






