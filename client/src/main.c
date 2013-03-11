#include <stdio.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>
#include <time.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "font.h"
#include "display.h"
#include "mouse.h"
#include "common.h"
#include "dir_tra.h"

#define DISPLAY_FUNS 17
#define BUFFER_SIZE 1024

#define SCREEN_SIZE 3145729 

//----------------------------------------------------------------------------------------------------------
//global value
//----------------------------------------------------------------------------------------------------------
static fb_info fb_inf;
volatile int mouse_global_flag = 0;
volatile int rool_flag = 0;
volatile int welcome_flag = 0;

int screen_size = 0;
char screen_save[SCREEN_SIZE];

pthread_mutex_t mutex_lock = PTHREAD_MUTEX_INITIALIZER;

//字符串处理
char *str_proc(char *buf)
{
	int i = 0;
	if(buf[i] != ' ')
		return NULL;
	while(buf[i] != '\0')
	{
		if(buf[i] == ' ')
			i++;
		else 
			return &buf[i];
	}
	return NULL;
}
#if 0
int display_nature(char *filename,fb_info fb_inf)
{
	//u_char *decode_jpeg (const char *filename, fb_info *jpeg_inf)
	u_char *jpeg_retbuf;
	fb_info jpeg_inf;
	jpeg_retbuf = decode_jpeg(filename,&jpeg_inf);
	if(jpeg_retbuf == NULL){
		fprintf(stderr,"decode jpeg failed");	
		return -1;
	}

	//u8_t * scale24(u8_t *buf24, fb_info new_inf, fb_info jpeg_inf)
	u8_t *scale_buf = scale24(jpeg_retbuf,fb_inf,jpeg_inf);
	if(scale_buf == NULL){
		fprintf(stderr,"scale24 call failed.\n");
		return -2; 
	}

	//u32_t * rgb24to32(u8_t *buf24, fb_info jpeg_inf)
	u32_t *screen_buf = rgb24to32(scale_buf,jpeg_inf);
	if(screen_buf == NULL){
		fprintf(stderr,"trans from 24 to 32 failed.\n");	
		return -3;
	}
	u32_t display_size = fb_inf.w * fb_inf.h * 4 ;
	memcpy(fb_inf.fbmem,screen_buf,display_size-1);
	free(jpeg_retbuf);
	jpeg_retbuf = NULL;
	free(scale_buf);
	scale_buf = NULL;
	free(screen_buf);
	screen_buf = NULL;
	return 0;
}
#endif
//鼠标线程
void *mouse_ops(void *arg)
{

	mouse_t mevent; 
	memset(&mevent,0,sizeof(mevent));
	fb_info mous_inf = fb_inf;;

	int mfd = open_mice(NULL);
	int pre_px,pre_py;
	sleep(1);
	while(1)
	{
		pre_px = mevent.px;
		pre_py = mevent.py;
		read_mice(mfd, &mevent, mous_inf);
		if(mevent.px != pre_px || mevent.py != pre_py){

			pthread_mutex_lock(&mutex_lock);
			memcpy(fb_inf.fbmem,screen_save,screen_size);		
			draw_mouse(mous_inf,mevent);
			pthread_mutex_unlock(&mutex_lock);

			usleep(20000);
		}
		//recover_mouse(mous_inf,mevent);
		if(welcome_flag == 1){
			if(mevent.button == 1)	{
				if(mevent.px > 200 && mevent.px < 336 && mevent.py > 176 && mevent.py < 200){
					pthread_mutex_lock(&mutex_lock);
					display_string("自动播放",200,200,fb_inf,0x777777); 
					pthread_mutex_unlock(&mutex_lock);
					rool_flag = 1;
					mevent.button = 0;
					mouse_global_flag = 0;
					usleep(100000);
				}else if(mevent.px > 406 && mevent.px < 530 && mevent.py > 373 && mevent.py < 400){

					pthread_mutex_lock(&mutex_lock);
					display_string("手动播放",400,400,fb_inf,0x777777); 
					pthread_mutex_unlock(&mutex_lock);
					rool_flag = 1;
					mevent.button = 0;
					mouse_global_flag = 1;

				}else if(mevent.px > 606 && mevent.px < 730 && mevent.py > 569 && mevent.py < 595){
					pthread_mutex_lock(&mutex_lock);
					display_string("音乐播放",600,600,fb_inf,0x777777); 
					pthread_mutex_unlock(&mutex_lock);
					rool_flag = 1;
					mevent.button = 0;
					mouse_global_flag = 2;
				}else if(mevent.px > 950 && mevent.px < 1000 && mevent.py > 725 && mevent.py < 753){
					pthread_mutex_lock(&mutex_lock);
					display_string("退出",950,750,fb_inf,0x777777); 
					pthread_mutex_unlock(&mutex_lock);
					rool_flag = 1;
					mevent.button = 0;
					mouse_global_flag = 3;
				}

			}
		}else{
			if(mevent.button == 1)	{
				if(mevent.px > 950 && mevent.px < 1000 && mevent.py > 725 && mevent.py < 753){
					init_ft("./res/fonts/stsong.ttf",30);
					display_string("返回",950,750,fb_inf,0x777777); 
					mevent.button = 0;
					mouse_global_flag = 2;
				}else{
					rool_flag = 1;
					mevent.button = 0;
					mouse_global_flag = 1;
				}
			}
			else if(mevent.button == 2){
				mouse_global_flag = -1;
				mevent.button = 0;
			}else if(mevent.button == 4){
				mouse_global_flag = 0;
				mevent.button = 0;

			}
		}
		usleep(10000);
	}
	return (void *)NULL;
}

int main (int argc, char **argv)
{
	if(argc > 1)//文件传输模式
	{
		//fprintf(stdout,"%s\n",argv[1]);	
		int server_fd;
		unsigned short remote_port;
		remote_port = 8000;

		struct	sockaddr_in remote_ipv4_address;
		memset(&remote_ipv4_address,0,sizeof(remote_ipv4_address));	
		remote_ipv4_address.sin_family = AF_INET;
		remote_ipv4_address.sin_port = htons(remote_port);
		inet_pton(AF_INET,argv[1],&remote_ipv4_address.sin_addr);

		ssize_t receive;
		ssize_t total = 0;
		ssize_t send;
		char buffer[BUFFER_SIZE];
		while(1)
		{
			//1.create socket fd
			if((server_fd = socket(PF_INET,SOCK_STREAM,0)) < 0){
				fprintf(stderr,"socket create failed,%s\n",strerror(errno));	
				exit(1);
			}
			fprintf(stdout,"Socket create successed,server fd %d\n",server_fd);

			//2.connect socket fd
			if(connect(server_fd,(struct sockaddr *)&remote_ipv4_address,sizeof(remote_ipv4_address)) < 0){
				fprintf(stderr,"connect to remote server %s : %d failed,%s\n",argv[1],remote_port,strerror(errno));	
				close(server_fd);
				exit(1);
			}
			fprintf(stdout,"Connected to %s:%d success.\n",argv[1],remote_port);

			//3.command transport
			receive = read(STDIN_FILENO,buffer,sizeof(buffer));
			if(strncmp(buffer,"ls",2) == 0){
				send = write(server_fd,buffer,receive);		
				if(send < 0){
					fprintf(stderr,"send command to server failed,%s\n",strerror(errno));	
					exit(2);
				}
				fprintf(stdout,"Send %d bytes to server successed.\n",send);
				while(1)
				{
					receive = read(server_fd,buffer,BUFFER_SIZE - 1);	
					if(receive < 0){
						if(errno == EINTR)	
							continue;
						fprintf(stderr,"receive data failed,%s\n",strerror(errno));
						exit(3);
					}else if(receive == 0){
						break;	
					}
					write(STDOUT_FILENO,buffer,receive);
					total += receive;
				}
				fprintf(stdout,"Received %d bytes.\n",total);
				total = 0;
			}else if(strncmp(buffer,"cp",2) == 0){
				total = 0;
				send = write(server_fd,buffer,receive);	
				if(send < 0){
					fprintf(stderr,"send command to server failed,%s\n",strerror(errno));	
					exit(2);
				}
				fprintf(stdout,"Send %d bytes to server successed.\n",send);
				char rbuffer[BUFFER_SIZE];
				receive = read(server_fd,rbuffer,BUFFER_SIZE - 1);
				if(strncmp(rbuffer,"Bad",3) == 0){
					write(STDOUT_FILENO,rbuffer,receive);	
					exit(3);
				}
				if(strncmp(rbuffer,"File",4) == 0){
					write(STDOUT_FILENO,rbuffer,receive);	
					exit(3);
				}
				char *ptr;
				buffer[send - 1] = '\0';
				fprintf(stdout,"%s\n",buffer);
				ptr = str_proc(buffer + 2);
				if(ptr == NULL){
					fprintf(stderr,"string process failed\n");	
					exit(3);
				}
				char path[4096];
				snprintf(path,sizeof(path),"./res/jpg/%s",ptr);
				fprintf(stdout,"path %s\n",path);
				int file_fd = open(path,O_RDWR | O_CREAT | O_TRUNC,0644);
				if(file_fd < 0){
					fprintf(stderr,"open file %s failed,%s\n",path,strerror(errno));	
					exit(3);
				}
				write(file_fd,rbuffer,receive);
				total += receive;
				while((receive = read(server_fd,rbuffer,BUFFER_SIZE - 1)) != 0)
				{
					write(file_fd,rbuffer,receive);	
					total += receive;
				}
				fprintf(stdout,"Download success.Received %d bytes.\n",total);
				close(file_fd);

			}else if(strncmp(buffer,"exit",4) == 0){
				break;	
			}
			//4.close socket fd
			close(server_fd);
		}
	}
	else{	//普通数码相框演示模式
		if(init_fb(&fb_inf) < 0){//初始化屏幕fd
			fprintf(stderr,"init fb failed,%s\n",strerror(errno));	
			exit(1);
		}

		screen_size = fb_inf.w * fb_inf.h * fb_inf.bpp / 8;
		fprintf(stdout,"%d\n",screen_size);
		int err_code;

		pthread_t mou_tid;
		//鼠标线程创建
		if((err_code = pthread_create(&mou_tid,NULL,mouse_ops,NULL)) != 0 )	{
			fprintf(stderr,"create pthread failed,%s\n",strerror(err_code));	
			exit(5);
		}

		char *file_desk[1024];


		int pic_num = 0;
		//遍历文件夹jpg文件
		if(read_jpg_dir("./res/jpg",file_desk,&pic_num) < 0){
			fprintf(stderr,"read_jpg_dir failed.\n");	
			int pic_num = 0;
			if(read_jpg_dir("./res/jpg",file_desk,&pic_num) < 0){
				fprintf(stderr,"read_jpg_dir failed.\n");	
				exit(4);
			}
			exit(4);
		}
		tranves_file_desk(file_desk,pic_num);
		fprintf(stdout,"sum %d\n",pic_num);
		err_code = init_ft("./res/fonts/fanxinshu.TTF",36);
		if(err_code != 0){
			fprintf(stderr,"init_ft failed\n");	
			exit(1);
		}

		//显示效果函数desk
		fun play_funs[DISPLAY_FUNS] = {disp_jpeg2,fang_picture_l,right_mid_left,fang_picture_h,down_in,right_in,bai_ye_chuang,up_down,left_right,rand_picture,crilepicture_big,crilepicture_small,up_mid_down,left_fix_right,Random,Box_radom,dissolve};
		unsigned int index = 0;
		unsigned int fun_ind = 0;
		int flag = 1;
		char pathname[1024];
welcome_menu:
		rool_flag = 0;
		welcome_flag = 1;
		//字体初始化
		err_code = init_ft("./res/fonts/fanxinshu.TTF",36);
		if(err_code != 0){
			fprintf(stderr,"init_ft failed\n");	
			exit(1);
		}

		pthread_mutex_lock(&mutex_lock);
		play_funs[0]("./res/welcome/welcome.jpg",fb_inf);
		display_string("自动播放",200,200,fb_inf,0x0930e); 
		display_string("手动播放",400,400,fb_inf,0xc9112d); 
		display_string("音乐播放",600,600,fb_inf,0xe68500); 
		display_string("退出",950,750,fb_inf,0x9f0521);
		memcpy(screen_save,fb_inf.fbmem,screen_size);
		pthread_mutex_unlock(&mutex_lock);
		while(!rool_flag)	
			sleep(1);

		welcome_flag = 0;

		while(flag == 1)
		{
			//fprintf(stdout,"%s,%d,flag %d\n",pathname,fun_ind,mouse_global_flag);
			switch(mouse_global_flag)
			{
			case 0:	//正常播放
				snprintf(pathname,sizeof(pathname),"./res/jpg/%s",file_desk[index]);

				pthread_mutex_lock(&mutex_lock);
				play_funs[fun_ind](pathname,fb_inf);
				init_ft("./res/fonts/fanxinshu.TTF",22);
				display_string(file_desk[index],10,20,fb_inf,0xaffff); 
				memcpy(screen_save,fb_inf.fbmem,screen_size);
				pthread_mutex_unlock(&mutex_lock);
				fun_ind++;
				fun_ind = fun_ind % DISPLAY_FUNS;
				index++;
				index = index % pic_num;

				sleep(1);

				break;
			case 1://播放下一张
				index++;
				index = index % pic_num;
				snprintf(pathname,sizeof(pathname),"./res/jpg/%s",file_desk[index]);

				pthread_mutex_lock(&mutex_lock);
				play_funs[15](pathname,fb_inf);
				init_ft("./res/fonts/stsong.ttf",30);
				display_string("返回",950,750,fb_inf,0x9f0521); 
				init_ft("./res/fonts/fanxinshu.TTF",22);
				display_string(file_desk[index],10,20,fb_inf,0xaffff);
				memcpy(screen_save,fb_inf.fbmem,screen_size);
				pthread_mutex_unlock(&mutex_lock);


				mouse_global_flag = 10;
				sleep(1);

				break;
			case -1://播放上一张
				index--;
				index = index % pic_num;
				snprintf(pathname,sizeof(pathname),"./res/jpg/%s",file_desk[index]);

				pthread_mutex_lock(&mutex_lock);
				play_funs[16](pathname,fb_inf);
				init_ft("./res/fonts/stsong.ttf",30);
				display_string("返回",950,750,fb_inf,0x9f0521); 
				init_ft("./res/fonts/fanxinshu.TTF",22);
				display_string(file_desk[index],10,20,fb_inf,0xaffff); 
				memcpy(screen_save,fb_inf.fbmem,screen_size);
				pthread_mutex_unlock(&mutex_lock);

				mouse_global_flag = 10;
				sleep(1);
				break;
			case 2://返回main menu
				goto welcome_menu;
				break;
			case 3://退出
				flag = 0;
				break;
			default:
				break;
			}
		}

		pthread_mutex_lock(&mutex_lock);
		play_funs[9]("./res/end/end.jpg",fb_inf);
		init_ft("./res/fonts/fanxinshu.TTF",90);
		display_string("谢谢观赏",360,300,fb_inf,0xb8264a);
		memcpy(screen_save,fb_inf.fbmem,screen_size);
		pthread_mutex_unlock(&mutex_lock);

		pthread_mutex_destroy(&mutex_lock);	
		pthread_cancel(mou_tid);
		pthread_join(mou_tid,NULL);	

		destroy_file_desk(file_desk,pic_num);
		if(munmap(fb_inf.fbmem,fb_inf.w * fb_inf.h * fb_inf.bpp / 8) < 0){
			fprintf(stderr,"mmunmap failed,%s\n",strerror(errno));	
			exit(5);
		}
	}

	return 0;
}





