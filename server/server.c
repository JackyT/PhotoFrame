#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>

#define BUFFER_SIZE 10240

char *str_proc(char *p)
{
	int i = 0;
	char *ptr;
	if(p[i] != ' ')
		return NULL;
	while(p[i] != '\0')
	{
		if(p[i] == ' ')	
			i++;
		else{
			ptr = &p[i];
			return ptr;
		}
	}

	return NULL;
}
int search_file(char *desk[],char *file,int num)
{
	int i = 0;
	for(;i < num;i++){
		if(strncmp(desk[i],file,strlen(file)) == 0)	
			return 0;
	}
	return -1;
}
void tranves_file_desk(char *desk[],int num) 
{

	int i = 0;
	for(;i < num;i++){
		fprintf(stdout,"%s\n",desk[i]);	
	}
}
int tranves_file_desk_to_socket(char *desk[],int num,int fd) 
{

	int total = 0;
	int i = 0;
	for(;i < num;i++){
		write(fd,desk[i],strlen(desk[i]));
		write(fd,"\n",sizeof("\n"));
		total += strlen(desk[i]) + 2;
	}
	return total;
}

void destroy_file_desk(char *desk[],int num)
{
	int i = 0;
	for(;i < num;i++)
		free(desk[i]);
}
int is_all_digit(char *filename)
{
	int i = 0;
	while(filename[i] != '\0')
	{
		if(isdigit(filename[i]) != 1)	
			return 0;
		i++;
	}
	return 1;
}
int read_jpg_dir(char *path,char *file_desk[],int *pic_num)
{	

	DIR *dir = opendir(path);
	if(dir == NULL){
		fprintf(stderr,"open dir failed,%s\n",strerror(errno));	
		return -1;	
	}

	struct dirent *files;
	*pic_num = 0;
	while((files = readdir(dir)) != NULL)
	{
		if(is_all_digit(files->d_name) == 1)	
			continue;
		if(strstr(files->d_name,".jpg") == NULL)
			continue;

		//fprintf(stdout,"%s\n",files->d_name);
		file_desk[*pic_num] = malloc(sizeof(char) * strlen(files->d_name) + 1);

		if(file_desk[*pic_num] == NULL){
			fprintf(stderr,"malloc for file_desk[%d] failed,%s\n",*pic_num,strerror(errno));	
			return -2;
		}
		memcpy(file_desk[*pic_num],files->d_name,strlen(files->d_name));
		file_desk[*pic_num][strlen(files->d_name)] = '\0';
		//fprintf(stdout,"file_desk[%d] %s\n",*pic_num,file_desk[*pic_num]);
		(*pic_num)++;
	}
	closedir(dir);
	return 0;
}

int main(int argc, char **argv)
{
	int listening_socket;
	unsigned short port;
	int backlog;
	char *file_desk[1024];
	//------------------------------------------------------------------
	// Parse command line arguments
	//------------------------------------------------------------------
	port = 8000; 
	backlog = 10;
	int pic_num;
	read_jpg_dir("./piconser",file_desk,&pic_num);
	tranves_file_desk(file_desk,pic_num);
	fprintf(stdout,"sum: %d\n",pic_num);

	//------------------------------------------------------------------
	// step 1, create socket
	//------------------------------------------------------------------
	//int socket(int domain, int type, int protocol);
	if ((listening_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		// failed
		fprintf(stderr, "Create new TCP socket failed: %s\n", strerror(errno));
		exit(1);
	}

	fprintf(stdout, "New TCP socket created, listening_socket = %d.\n", listening_socket);

	//------------------------------------------------------------------
	// Set SO_REUSEADDR & SO_REUSEPORT options
	//------------------------------------------------------------------
	int optval;

	optval = 1;

	//int setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen);
	setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

#ifdef SO_REUSEPORT
	optval = 1;
	setsockopt(listening_socket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
#endif

	//------------------------------------------------------------------
	// step 2, bind
	//------------------------------------------------------------------
	struct sockaddr_in local_ipv4_address;	// IPv4

#if 0
	struct sockaddr_in
	{
		sa_family_t sin_family;
		in_port_t sin_port;
		struct in_addr sin_addr;
		unsigned char sin_zero[8];
	};
#endif

	memset(&local_ipv4_address, 0, sizeof(local_ipv4_address));

	local_ipv4_address.sin_family = AF_INET;	// IPv4
	local_ipv4_address.sin_port = htons(port);

	//local_ipv4_address.sin_addr.s_addr = INADDR_ANY;
	// int inet_pton(int af, const char *src, void *dst);
	inet_pton(AF_INET, argv[1], &local_ipv4_address.sin_addr);

	//int bind(int sockfd, const struct sockaddr *my_addr, socklen_t addrlen);
	if (bind(listening_socket, (struct sockaddr *) &local_ipv4_address, sizeof(local_ipv4_address)) < 0)
	{
		fprintf(stderr, "Bind to %s:%d failed: %s\n", argv[1], port, strerror(errno));
		close(listening_socket);
		exit(1);
	}

	fprintf(stdout, "Bound to %s:%d successfully.\n", argv[1], port);

	//------------------------------------------------------------------
	// step 3, listen
	//------------------------------------------------------------------
	//int listen(int sockfd, int backlog);
	if (listen(listening_socket, backlog) < 0)
	{
		// failed
		fprintf(stderr, "Listen on %s:%d failed: %s\n", argv[1], port, strerror(errno));
		close(listening_socket);
		exit(1);
	}

	fprintf(stdout, "Listen on %s:%d successfully.\n", argv[1], port);
	fprintf(stdout, "Waiting for incomming connection ...\n");

	int new_connected_socket;
	struct sockaddr_in peer_ipv4_address;
	socklen_t peer_ipv4_address_length;
	char peer_ipv4_address_string[] = "ddd.ddd.ddd.ddd";
	int receive,send;
	int total = 0;
	char buffer[BUFFER_SIZE];
	for (;;)
	{
		// step 4, accept, create a new_connected_socket, original socket still listen

		// value-result parameter
		peer_ipv4_address_length = sizeof(peer_ipv4_address);

		//int accept(int sockfd, struct sockaddr *addr, socklen_t * addrlen);
		//int accept(int sockfd, struct sockaddr *addr, socklen_t * addrlen);
		if ((new_connected_socket = accept(listening_socket, (struct sockaddr *) &peer_ipv4_address, &peer_ipv4_address_length)) < 0)
		{
			// failed
			if (errno == EINTR)
			{
				// interrupted by signal
				continue;
			}

			// FIXME: how to do?
			fprintf(stderr, "accept() failed: %s\n", strerror(errno));
			break;
		}
		else
		{
			// success
			//const char *inet_ntop(int af, const void *src, char *dst, socklen_t cnt);
			inet_ntop(AF_INET, &peer_ipv4_address.sin_addr, peer_ipv4_address_string, sizeof(peer_ipv4_address_string));

			fprintf(stdout, "Accepted a new connection %d from %s:%d.\n", new_connected_socket, peer_ipv4_address_string, ntohs(peer_ipv4_address.sin_port));

			receive = read(new_connected_socket,buffer,sizeof(buffer));
			write(STDOUT_FILENO,buffer,receive);
			if(strncmp(buffer,"ls",2) == 0){

				//write(STDOUT_FILENO,buffer,receive);
				total = tranves_file_desk_to_socket(file_desk,pic_num,new_connected_socket);
				fprintf(stdout,"Send %d bytes successed.\n",total);	
				
			}else if(strncmp(buffer,"cp",2) == 0){
				total = 0;
				char *ptr;
				buffer[receive - 1] = '\0';
				ptr = str_proc(buffer+2);
				if(ptr == NULL){
					write(new_connected_socket,"Bad command\n",sizeof("Bad command\n"));	
					close(new_connected_socket);
					continue;
				}
				if(search_file(file_desk,ptr,pic_num) < 0){
					write(new_connected_socket,"File not exist\n",sizeof("File not exist\n"));	
					close(new_connected_socket);
					continue;	
				}
				char path[4096];
				snprintf(path,sizeof(path),"./piconser/%s",ptr);						
				int file_fd;
				file_fd = open(path,O_RDONLY);
				if(file_fd < 0){
					fprintf(stderr,"open %s failded,%s\n",ptr,strerror(errno));	
					write(new_connected_socket,"error: open file failed at server\n",sizeof("error: open file failed at server\n"));
				}
				while((send = read(file_fd,buffer,BUFFER_SIZE - 1)) != 0)
				{
					write(new_connected_socket,buffer,send);
					total += send;
				}
				fprintf(stdout,"send %d bytes successed.\n",total);
				close(file_fd);
				//fprintf(stdout,"%s\n",ptr);
			}

			// step 6, close new_connected_socket
			close(new_connected_socket);
		}
	}

	// final, close listening_socket
	close(listening_socket);

	return 0;
}
