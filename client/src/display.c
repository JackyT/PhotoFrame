//-------------------------------------------------------------------
//播放特效函数
//-------------------------------------------------------------------
#include <stdio.h> 
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include "common.h"

//static fb_info jpeg_inf;
static float xscale = 1; 
static float yscale = 1;

void up_mid_down(char *filename,fb_info fb_inf)
{
	//printf("hello world");
	unsigned char *s, *d;
	int i=0;
	int j=0;
	int x=0;
	int y=0;
	fb_info jpeg_inf;
	read_JPEG_file(filename, &jpeg_inf);

	float xscale = (float)fb_inf.w / (jpeg_inf.w); 
	float yscale = (float)fb_inf.h / (jpeg_inf.h);
	for (j = 0; j < fb_inf.h / 2; j++) // =
	{
		for (i = 0; i < fb_inf.w; i++) 
		{
			x = (float) i / (float)xscale;
			y = (float) j / (float)yscale;
			s = jpeg_inf.fbmem + (x + y*jpeg_inf.w)*jpeg_inf.bpp;
			d = fb_inf.fbmem + (i + j*fb_inf.w)*fb_inf.bpp/8;
			d[0] = s[2];
			d[1] = s[1];
			d[2] = s[0];
			x = (float) i / (float)xscale;
			y = (float) (fb_inf.h-1-j) / (float)yscale;
			s = jpeg_inf.fbmem + (x + y*jpeg_inf.w)*jpeg_inf.bpp;
			d = fb_inf.fbmem + (i + (fb_inf.h-1-j)* fb_inf.w)*fb_inf.bpp/8;
			d[0] = s[2];
			d[1] = s[1];
			d[2] = s[0];
		}
		usleep(10000);
	}
	free(jpeg_inf.fbmem);
	sleep(1);

}
void left_fix_right(char *filename,fb_info fb_inf)
{
	unsigned char *s ,*d;
	int i=0,j=0,r,l,n;
	fb_info jpeg_inf;
	read_JPEG_file(filename, &jpeg_inf);
	n = 24;
	float xscale = (float) (fb_inf.w) / jpeg_inf.w;
	float yscale = (float) (fb_inf.h) / jpeg_inf.h;
	for(r = 0; r <= 1029; )
	{
		if (r == 1029)
			r = 1023;
		for(i = 0; i <= r; i++)
		{
			for (l = 0; l < n / 2; l++) 
			{
				for (j = l*2*fb_inf.h/n; j < l*2*fb_inf.h/n+32; j++) 
				{
					int x = (float) (fb_inf.w-1-(r-i)) / (float) xscale;
					int y = (float) j / (float) yscale;
					s = jpeg_inf.fbmem+(x+y*jpeg_inf.w)*jpeg_inf.bpp;
					d = fb_inf.fbmem+(i+j*fb_inf.w)*fb_inf.bpp/8;
					d[2] = s[0];
					d[1] = s[1];
					d[0] = s[2];
					x = (float) (r-i) / (float) xscale;
					y = (float) (j+32) / (float) yscale;
					s = jpeg_inf.fbmem + (x+y*jpeg_inf.w)*jpeg_inf.bpp;
					d = fb_inf.fbmem+(fb_inf.w-1-i+(j+32)*fb_inf.w)*fb_inf.bpp/8;
					d[2] = s[0];
					d[1] = s[1];
					d[0] = s[2];
				}
			}
		}
		r += 7;
	}
	free(jpeg_inf.fbmem);
	usleep(10000);

}
static void creat_rand(int num[], int size)
{
	srand(time(NULL));
	int tmp;
	int i, j;
	for (i = 0; i < size; i++) 
	{
		num[i] = i;
	}
	for (i = 0; i < size; i++) 
	{
		j = rand() % size;
		tmp = num[i];
		num[i] = num[j];
		num[j] = tmp;
	}

}
void Box_radom(char *filename,fb_info fb_inf)
{
	unsigned char *s,*d;
	int i=0,j=0,x=0,y=0;
	fb_info jpeg_inf;
	read_JPEG_file(filename, &jpeg_inf);
	float xscale = (float)fb_inf.w / (jpeg_inf.w); 
	float yscale = (float)fb_inf.h / (jpeg_inf.h);
	int block[32*32];
	creat_rand(block, 32*32);

	for (i = 0; i < 32*32; i++) 
	{
		for(j = 0; j < 32*24; j++)
		{
			x = (float) (block[i] % 32*32 + j % 32) / (float)xscale;
			y = (float) (block[i] / 32*24 + j / 32) / (float)yscale;
			s = jpeg_inf.fbmem + (x + y*jpeg_inf.w)*jpeg_inf.bpp;
			d = fb_inf.fbmem + (block[i]%32*32+j%32 + (block[i]/32*24+j/32)*fb_inf.w)*fb_inf.bpp/8;
			d[0] = s[2];
			d[1] = s[1];
			d[2] = s[0];
		}
		usleep(1000);
	}
	free(jpeg_inf.fbmem);
	sleep(1);
}
void dissolve(char * filename,fb_info fb_inf)
{

	int i=0,j=0;
	int color;
	unsigned char *s,*d;
	fb_info jpeg_inf;
	color = 80;
	read_JPEG_file(filename, &jpeg_inf);
	while(1)
	{
		srand(time(NULL));
		float xscale = (float)fb_inf.w / (jpeg_inf.w); 
		float yscale = (float)fb_inf.h / (jpeg_inf.h);
		for (j = 0; j < fb_inf.h; j++) 
		{
			for (i = 0; i <fb_inf.w; i++) 
			{
				int x = (float) i / (float)xscale;
				int y = (float) j / (float)yscale;

				s = jpeg_inf.fbmem + (x + y*jpeg_inf.w)*jpeg_inf.bpp;
				d = fb_inf.fbmem + (i + j * fb_inf.w)*fb_inf.bpp/8;
				if (s[0] < color && s[1] < color && s[2] < color)
				{
					d[0] = s[2];
					d[1] = s[1];
					d[2] = s[0];
				}
			}
		}
		color += 1;
		if(color > 256)
			break;
	}       
	free(jpeg_inf.fbmem);

}
void right_mid_left(char*filename,fb_info fb_inf)
{

	int i=0,j=0;
	int l=0;
	unsigned char *s,*d;
	fb_info jpeg_inf;
	read_JPEG_file(filename, &jpeg_inf);
	float xscale = (float)fb_inf.w / (jpeg_inf.w); 
	float yscale = (float)fb_inf.h / (jpeg_inf.h);
	for(l = 0; l < fb_inf.h; )
	{
		for (j = 0; j <= l; j++) 
		{
			for (i = 0; i < l*512/768+2; i++)
			{   
				int x = (float) (512-(l*512/768+2-i)) / (float)xscale;
				int y = (float) (767-(l-j)) / (float)yscale;
				s = jpeg_inf.fbmem + (x + y*jpeg_inf.w)*jpeg_inf.bpp;
				d = fb_inf.fbmem + (i + j *fb_inf.w)*fb_inf.bpp/8;
				d[0] = s[2];
				d[1] = s[1];
				d[2] = s[0];
				x = (float) (512+(l*512/768-i)) / (float)xscale;
				y = (float) (767-(l-j)) / (float)yscale;
				s = jpeg_inf.fbmem + (x + y*jpeg_inf.w)*jpeg_inf.bpp;
				d = fb_inf.fbmem + (1023-i + j *fb_inf.w)*fb_inf.bpp/8;
				d[0] = s[2];
				d[1] = s[1];
				d[2] = s[0];
			}
		}
		l += 2;
	}
	free(jpeg_inf.fbmem);
}

void Random(char*filename,fb_info fb_inf)
{
	int line[fb_inf.h];
	creat_rand(line, fb_inf.h);
	int i=0,j=0;
	unsigned char *s,*d; 
	fb_info jpeg_inf;
	read_JPEG_file(filename, &jpeg_inf);
	float xscale = (float) (fb_inf.w) / jpeg_inf.w;
	float yscale = (float) (fb_inf.h) / jpeg_inf.h;

	for (j = 0; j < fb_inf.h; j++) 
	{
		for (i = 0; i < fb_inf.w; i++) 
		{
			int x = (float) i / (float) xscale;
			int y = (float) line[j] / (float) yscale;

			s = jpeg_inf.fbmem + (x + y * jpeg_inf.w) * jpeg_inf.bpp ;
			d = fb_inf.fbmem + (i + line[j] * fb_inf.w) * fb_inf.bpp/8;
			d[2] = s[0];
			d[1] = s[1];
			d[0] = s[2];
		}
		usleep(1000);
	}
	free(jpeg_inf.fbmem);
	sleep(1);
}
#if 1 
static void pictureshow(char *filename, float dx, float dy, float R,fb_info fb_inf)
	//将图片显示在一为圆的区域
{
	int i, j, x, y;
	unsigned char *s, *d;
	fb_info jpeg_inf;
	read_JPEG_file(filename, &jpeg_inf);

	xscale = (float)fb_inf.w/jpeg_inf.w;
	yscale = (float)fb_inf.h/jpeg_inf.h;

	for(j = 0; j < fb_inf.h; j++)
		for(i = 0; i < fb_inf.w; i++)
		{
			x = i/xscale;
			y = j/yscale;
			s = jpeg_inf.fbmem + ((x + y*jpeg_inf.w)*jpeg_inf.bpp);
			d = fb_inf.fbmem + ((i + j * fb_inf.w)* fb_inf.bpp/8);
			if((i-dx)*(i - dx) + (j - dy)*(j - dy) < R*R  )
			{
				d[0] = s[2];
				d[1] = s[1];
				d[2] = s[0];
			}
		}
	usleep(1000);

	free(jpeg_inf.fbmem);
}
static void pictureshow2(char *picturename, float dx, float dy, float R,fb_info fb_inf)
	//将图片显示在一为圆的区域外
{
	int i, j, x, y;
	unsigned char *s, *d;
	fb_info jpeg_inf;
	read_JPEG_file(picturename, &jpeg_inf);

	xscale = (float)fb_inf.w/jpeg_inf.w;
	yscale = (float)fb_inf.h/jpeg_inf.h;

	for(j = 0; j < fb_inf.h; j++)
		for(i = 0; i < fb_inf.w; i++)
		{
			x = i/xscale;
			y = j/yscale;
			s = jpeg_inf.fbmem + ((x + y*jpeg_inf.w)*jpeg_inf.bpp);
			d = fb_inf.fbmem + ((i + j * fb_inf.w)* fb_inf.bpp/8);
			if((i-dx)*(i - dx) + (j - dy)*(j - dy) > R*R  )
			{
				d[0] = s[2];
				d[1] = s[1];
				d[2] = s[0];
			}
		}

	free(jpeg_inf.fbmem);
}
void crilepicture_small(char *filename,fb_info fb_inf)
{
	float x, y, R;
	x = fb_inf.w/2;
	y = fb_inf.h/2;
	R = 700;

	while(R >= 0)
	{

		pictureshow2(filename, x, y, R,fb_inf);
		R = R - 5;
	}
}
void crilepicture_big(char *filename,fb_info fb_inf)
{
	float x, y, R;
	x = fb_inf.w/2;
	y = fb_inf.h/2;
	R = 0;

	while(R*R < (1024/2)*(1024/2) + (768/2)*(768/2))
	{
		pictureshow(filename, x, y, R,fb_inf);
		R = R + 5;
	}
}
#endif
void bai_ye_chuang(char *filename,fb_info fb_inf)
{
	fb_info jpeg_inf;
	unsigned char *s, *d;
	int i=0;
	int j=0;
	int x=0;
	int y=0;
	int n,l;
	n=16;
	read_JPEG_file(filename, &jpeg_inf);
	float xscale = (float)fb_inf.w / jpeg_inf.w; 
	float yscale = (float)fb_inf.h / jpeg_inf.h;
	for (i = 0; i < fb_inf.w / n; i++) 
	{
		for (j = 0; j < fb_inf.h; j++) 
		{
			y = (float)j / (float)yscale;
			for (l = 0; l < n; l++)
			{
				x = (float) (i + l*fb_inf.w / n) / (float)xscale;
				s = jpeg_inf.fbmem + (x + y*jpeg_inf.w)*jpeg_inf.bpp;
				d = fb_inf.fbmem+(i+l*fb_inf.w/n+j*fb_inf.w)*fb_inf.bpp/8;
				d[0] = s[2];
				d[1] = s[1];
				d[2] = s[0];
			}
		}
		usleep(6000);
	}
	sleep(1);
	free(jpeg_inf.fbmem);
}
void up_down(char *filename,fb_info fb_inf)
{
	unsigned char *s, *d;
	int i=0;
	int j=0;
	int x=0;
	int y=0;
	int n,l;
	fb_info jpeg_inf;
	n=32;
	read_JPEG_file(filename, &jpeg_inf);
	float xscale = (float)fb_inf.w / jpeg_inf.w; 
	float yscale = (float)fb_inf.h / jpeg_inf.h;
	for (j = 0; j < fb_inf.h/n; j++) 
	{
		for (i = 0; i <fb_inf.w; i++) 
		{
			x = (float) i / (float)xscale;
			for (l = 0; l < n; l++)
			{
				y = (float) (j + l*fb_inf.h/n) / (float)yscale;
				s = jpeg_inf.fbmem + (x + y*jpeg_inf.w)*jpeg_inf.bpp;
				d = fb_inf.fbmem +(i+(j+l*fb_inf.h/n)*fb_inf.w)*fb_inf.bpp/8;
				d[0] = s[2];
				d[1] = s[1];
				d[2] = s[0];
			}
		}
		usleep(100000);
	}
	sleep(1);              
	free(jpeg_inf.fbmem);
}

static void randinit(int  a[768])
{
	int n;
	int i = 0;
	int j = 0;

	srand(time(NULL));
	for(i = 0; i< 768; i++)
	{
		while(1)
		{
			n = rand()%768;
			for(j = 0; j < i; j++)
				if(n == a[j])
					break;
			if(j == i) 
				break;
		}
		a[i] = n;
	}
}
void rand_picture(char *filename,fb_info fb_inf)
{
	int i, j, x, y;
	unsigned char *s, *d;
	int a[768];
	
	fb_info jpeg_inf;

	randinit(a);
	srand(time(NULL));

	//init_fb(&fb_inf);
	read_JPEG_file(filename, &jpeg_inf);

	xscale = (float)fb_inf.w/jpeg_inf.w;
	yscale = (float)fb_inf.h/jpeg_inf.h;

	for(j = 0; j < fb_inf.h; j++)
	{
		for(i = 0; i < fb_inf.w; i++)
		{
			x = i/xscale;
			y = a[j]/yscale;
			s = jpeg_inf.fbmem + ((x + y*jpeg_inf.w)*jpeg_inf.bpp);
			d = fb_inf.fbmem + ((i + a[j] * fb_inf.w)* fb_inf.bpp/8);
			d[0] = s[2];
			d[1] = s[1];
			d[2] = s[0];
		}
		usleep(2500);
	}
	free(jpeg_inf.fbmem);
}
void left_right(char *filename,fb_info fb_inf)
{	unsigned char *s, *d;
	int i=0;
	int j=0;
	int x=0;
	int y=0;
	fb_info jpeg_inf;

	read_JPEG_file(filename, &jpeg_inf);
	float xscale = (float)fb_inf.w / jpeg_inf.w; 
	float yscale = (float)fb_inf.h / jpeg_inf.h;
	for (i = 0; i < fb_inf.w / 2; i++) 
	{
		for (j = 0; j < fb_inf.h; j++) 
		{
			x = (float) i / (float)xscale;
			y = (float) j / (float)yscale;
			s = jpeg_inf.fbmem + (x + y*jpeg_inf.w)*jpeg_inf.bpp;
			d = fb_inf.fbmem + (i + j*fb_inf.w)*fb_inf.bpp/8;
			d[0] = s[2];
			d[1] = s[1];
			d[2] = s[0];
			x = (float) (fb_inf.w-1-i) / (float)xscale;
			y = (float) j / (float)yscale;
			s = jpeg_inf.fbmem + (x + y*jpeg_inf.w)*jpeg_inf.bpp;
			d = fb_inf.fbmem + (fb_inf.w-1-i + j* fb_inf.w)*fb_inf.bpp/8;
			d[0] = s[2];
			d[1] = s[1];
			d[2] = s[0];
		}
		usleep(1000);
	}
	sleep(1);
	free(jpeg_inf.fbmem);
}
void disp_jpeg1(char * filename, fb_info fb_inf)
{
	int i, j, r;
	int x,y;
	u8_t * s, *d;
	u32_t color;
	float xscale, yscale;
	fb_info jpeg_inf;
	read_JPEG_file (filename, &jpeg_inf);
	xscale = (float)fb_inf.w / (float)jpeg_inf.w;
	yscale = (float)fb_inf.h / (float)jpeg_inf.h;
	for(r = 0; r < fb_inf.h/2; r++)
	{
		for (j = 0; j < fb_inf.h; j++) 
		{
			for (i = 0; i < fb_inf.w; i++) 
			{
				if((i-512)*(i-512) + (j-384)*(j-384) <= r * r)
				{
					x = (float)i / xscale;
					y = (float)j / yscale;
					s = jpeg_inf.fbmem + (x + y*jpeg_inf.w)*3;
					d = fb_inf.fbmem + (i + j * fb_inf.w)*4;
					d[0] = s[2];
					d[1] = s[1];
					d[2] = s[0];
				}

			}
		}
	}
	r = fb_inf.h/2;
	for(; r >= 0; r--)
	{
		for (j = 0; j < fb_inf.h; j++) 
		{
			for (i = 0; i < fb_inf.w; i++) 
			{
				if((i-512)*(i-512) + (j-384)*(j-384) <= r*r)
				{
					x = (float)i / xscale;
					y = (float)j / yscale;
					s = jpeg_inf.fbmem + (x + y*jpeg_inf.w)*3;
					d = fb_inf.fbmem + (i + j * fb_inf.w)*4;
					d[0] = s[2];
					d[1] = s[1];
					d[2] = s[0];
				}

			}
		}

		for (i = 0; i < fb_inf.w; i++) 
		{
			if(((i-512)*(i-512) + (j-384)*(j-384) >= r*r) && 
			   ((i - 512)*(i-512) + (j-384)*(j-384)<= 
				(fb_inf.h/2)*(fb_inf.h/2)))
			{
				color = 0x00;
				fb_pixel(fb_inf, i, j, color);
			}

		}

	}
	free(jpeg_inf.fbmem);
}
void disp_jpeg2(char * filename, fb_info fb_inf)
{
	int i, j, tmp;
	int x, y;
	u8_t * s, *d;
	float xscale, yscale;
	fb_info jpeg_inf;
	int line[fb_inf.h];
	for (i = 0; i < fb_inf.h; i++)
	{
		line[i] = i;
	}
	for (i = 0; i < fb_inf.h; i++)
	{
		j = rand() % fb_inf.h;
		tmp = line[i]; line[i] = line[j];
		line[j] = tmp;
	}
	read_JPEG_file (filename, &jpeg_inf);
	xscale = (float)fb_inf.w / (float)jpeg_inf.w;
	yscale = (float)fb_inf.h / (float)jpeg_inf.h;

	for (j = 0; j < fb_inf.h; j++)
	{
		usleep(1000);
		for (i = 0; i < fb_inf.w; i++)
		{
			x = (float)i / xscale;
			y = (float)line[j] / yscale;
			s = jpeg_inf.fbmem + (x + y*jpeg_inf.w)*3;
			d = fb_inf.fbmem + (i + line[j] * fb_inf.w)*4;
			d[0] = s[2];
			d[1] = s[1];
			d[2] = s[0];

		}
	}
	free(jpeg_inf.fbmem);
}

void disp_jpeg3(char *filename, fb_info fb_inf)
{
	int i, j, x, y;
	fb_info jpeg_inf;
	unsigned char *s, *d;
	float xscale, yscale;

	read_JPEG_file ( filename,  &jpeg_inf);
	xscale=(float)fb_inf.w / (float)jpeg_inf.w;
	yscale=(float)fb_inf.h / (float)jpeg_inf.h;

	for(j=0; j<fb_inf.h; j++)
	{
		usleep(500);
		for(i=0; i<fb_inf.w; i++)
		{
			if(i>j){
				x=(float)i/xscale;
				y=(float)j/yscale;
				s=jpeg_inf.fbmem+(x+y*jpeg_inf.w)*jpeg_inf.bpp/8;
				d=fb_inf.fbmem+(i+j*fb_inf.w)*fb_inf.bpp/8;
				d[0]=s[2]; d[1]=s[1]; d[2]=s[0];}
		}
	}

	for(j=fb_inf.h-1; j>=0; j--)
	{
		usleep(666);
		for(i=0; i<fb_inf.w; i++)
		{
			if(i<=j){
				x=(float)i/xscale;
				y=(float)j/yscale;
				s=jpeg_inf.fbmem+(x+y*jpeg_inf.w)*jpeg_inf.bpp/8;
				d=fb_inf.fbmem+(i+j*fb_inf.w)*fb_inf.bpp/8;
				d[0]=s[2]; d[1]=s[1]; d[2]=s[0];
			}
		}
	}
	free(jpeg_inf.fbmem);
}

void fang_picture_l(char *filename,fb_info fb_inf)
{
	float isw = 1024;
	float ish = 768;
	float px = 0;
	float py = 0;
	int i, j, x, y;
	int m = 0;
	int n = 0;
	unsigned char *s, *d;
	fb_info jpeg_inf;

	read_JPEG_file(filename, &jpeg_inf);
	xscale = isw/jpeg_inf.w;
	yscale = ish/jpeg_inf.h;

	for(j = 0, n = py; j < ish && n < fb_inf.h; j++,n++)
	{
		for(i = 0, m = px; i < isw && m < fb_inf.w; i++,m++)
		{
			x = i/xscale;
			y = j/yscale;
			s = jpeg_inf.fbmem + ((x + y*jpeg_inf.w)*jpeg_inf.bpp);
			d = fb_inf.fbmem + ((m + n * fb_inf.w)* fb_inf.bpp/8);
			if(((i - px) <= isw) && ((j - py) <= ish))
			{
				d[0] = s[2];
				d[1] = s[1];
				d[2] = s[0];
			}
		}
		usleep(2500);
	}
	free(jpeg_inf.fbmem);
}
void fang_picture_h(char *filename,fb_info fb_inf)
{
	int isw = 1024;
	int ish = 768;
	int px = 0;
	int py = 0;
	int i, j, x, y;
	int  m,n;
	unsigned char *s, *d;
	fb_info jpeg_inf;
	read_JPEG_file(filename, &jpeg_inf);

	xscale = (float)isw/jpeg_inf.w;
	yscale = (float)ish/jpeg_inf.h;

	for(i = 0, m = px; i < isw && m < fb_inf.w; i++,m++)
	{
		for(j = 0, n = py; j < ish && n < fb_inf.h; j++,n++)
		{
			x = i/xscale;
			y = j/yscale;
			s = jpeg_inf.fbmem + ((x + y*jpeg_inf.w)*jpeg_inf.bpp);
			d = fb_inf.fbmem + ((m + n * fb_inf.w)* fb_inf.bpp/8);
			if((i - px < isw) && (j - py < ish))
			{
				d[0] = s[2];
				d[1] = s[1];
				d[2] = s[0];
			}
		}
		usleep(2500);
	}
	free(jpeg_inf.fbmem);
}


void down_in(char *filename,fb_info fb_inf)
{
	int i, j, x, y;
	int h = 0;
	int w = 0;
	int m = 0;
	int n = 0;
	unsigned char *s, *d;
	fb_info jpeg_inf;
	read_JPEG_file(filename, &jpeg_inf);

	xscale = (float)fb_inf.w/jpeg_inf.w;
	yscale = (float)fb_inf.h/jpeg_inf.h;

	while(h  <  768 && w < 1024)
	{
		for(j = 0, m = 768 - h; j < 768 && m < 768; j++, m++)
		{
			for(i = 0, n = 1024 - w; i < 1024 && n < 1024; i++, n++)
			{
				x = i/xscale;
				y = j/yscale;
				s = jpeg_inf.fbmem + ((x + y*jpeg_inf.w)*jpeg_inf.bpp);
				d = fb_inf.fbmem + ((n +  m * fb_inf.w)* fb_inf.bpp/8);
				d[0] = s[2];
				d[1] = s[1];
				d[2] = s[0];
			}
		}
		h = h + 3;
		w = w + 4;
	}
	free(jpeg_inf.fbmem);
}

void right_in(char *filename,fb_info fb_inf)
{
	int i, j, x, y;
	int h = 0;
	int w = 0;
	int m = 0;
	int n = 0;
	unsigned char *s, *d;
	fb_info jpeg_inf;
	read_JPEG_file(filename, &jpeg_inf);

	xscale = (float)fb_inf.w/jpeg_inf.w;
	yscale = (float)fb_inf.h/jpeg_inf.h;
	while(h < fb_inf.h && w < fb_inf.w)
	{
		for(j = 767, m = h; j >= 0 && m >= 0; j--, m--)
		{
			for(i = 1023, n = w; i >= 0 && n >= 0; i--, n--)
			{
				//	fprintf(stdout,"ok\n");
				x = i/xscale;
				y = j/yscale;
				s = jpeg_inf.fbmem + ((x + y*jpeg_inf.w)*jpeg_inf.bpp);
				d = fb_inf.fbmem + ((n +  m * fb_inf.w)* fb_inf.bpp/8);
				d[0] = s[2];
				d[1] = s[1];
				d[2] = s[0];
			}
		}
		h = h + 3;
		w = w + 4;
	}
	free(jpeg_inf.fbmem);
}
