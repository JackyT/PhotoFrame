#ifndef _DISPLAY_H_

#define _DISPLAY_H_

#include "common.h"
typedef void (*fun)(char *,fb_info);

void disp_jpeg1(char * filename, fb_info fb_inf);
void disp_jpeg2(char * filename, fb_info fb_inf);
void disp_jpeg3(char * filename, fb_info fb_inf); //BUG
void fang_picture_l(char *filename,fb_info fb_inf);
void fang_picture_h(char *filename,fb_info fb_inf);
void down_in(char *filename,fb_info fb_inf);
void right_in(char *filename,fb_info fb_inf);
void pictureshow(char *filename,fb_info fb_inf);
void bai_ye_chuang(char *filename,fb_info fb_inf);
void up_down(char *filename,fb_info fb_inf);
void left_right(char *filename,fb_info fb_inf);
void rand_picture(char *filename,fb_info fb_inf);

void crilepicture_big(char *filename,fb_info fb_inf);
void crilepicture_small(char *filename,fb_info fb_inf);

void up_mid_down(char *filename,fb_info fb_inf);
void left_fix_right(char *filename,fb_info fb_inf);
void Random(char*filename,fb_info fb_inf);
void Box_radom(char *filename,fb_info fb_inf);
void dissolve(char * filename,fb_info fb_inf);
void right_mid_left(char*filename,fb_info fb_inf);
#endif
