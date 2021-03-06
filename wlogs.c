#include <stdio.h>
#include <getopt.h> //getopt_long()
#include <pthread.h> //pthread_create()
#include <sys/types.h> 
#include <dirent.h>  // DIR
#include <string.h> //memset()
#include <fcntl.h> // open()
#include <unistd.h> //write(),close(),uslee(), getpid()
#include <stdlib.h> //exit()
#include "wlogs.h"

//mmap 사용
#include <fcntl.h>    //open(), close()
#include <sys/mman.h> //mmap()
#include <sys/stat.h> //stat 구조체 
#include <time.h> //ctime()

//netlink 사용 
#include <asm/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <syslog.h>

#define NETLINK_USER 	31
// mutex 변수 선언
pthread_mutex_t kernel_log_mutex;
pthread_mutex_t app_log_mutex;
// pthread 조건 변수 선언
pthread_cond_t kernel_log_cond;

struct option origin_opts[] = {  
	{ "start", 0, 0, 's'},  //0 isnt need to 인수, 1 is need to 인수
	{ "stop ", 0, 0, 't'},  // 3번 칸은 지정한 값을 받을 주소, 예) {"opt55",0,&var55}
	{ "help", 0, 0, 'h'},   
	{ "debug", 0, 0, 'd'},
	{ 0, 0, 0, 0}
};

struct log_struct {
	char name;
	int desc;
	int time;
};

void kernel_log_handler(void *ptr);
void log_handler(DIR *dir);
void help(char *name);
void check_process(int opt);
void check_argument(int argc, char * argv[], int opt, struct option *opts, int optind);
void collect_log();

int main(int argc, char *argv[])
{
	// 스레드 변수 선언
	pthread_t kernel_log_thread;
	// 입력 받을 구조체 포인터 변수 선언
	struct option *opts = origin_opts;
	int opt;
	int command =0;

	printf("argv[0]=%s\n",argv[0]);

	if (argc < 2) {
		help(PROGRAM_NAME);
		exit(1);
	}


	while(1){ //opt=unicode value
		int option_index = 0;
		int option_optind = optind ? optind : 1;
		opt = getopt_long(argc, argv, "sthd:", opts, &option_index);
		//printf("optind=%d, opt=%d\n", optind, opt); //optind는 인자값 없으면 1, 있으면 2
		check_argument(argc, argv, opt, opts, optind);
		switch(opt){
			case 's':  // 데몬 시작 및 PID파일 생성
				check_process(opt);
				break;

			case 't':  // 데몬 종료 및 PID파일 제거
				check_process(opt);
				break;

			case 'd':  // 디버깅 기능 추가
				check_process(opt);
				break;
			case '?':  // help
			case 'h':  // help
			default :  // help
				help(PROGRAM_NAME);
				exit(1);
		}
		break;
	}


	// mutex 초기화
	pthread_mutex_init(&kernel_log_mutex,NULL);
	pthread_mutex_init(&app_log_mutex,NULL);
	// pthread 조건 변수 초기화
	//pthread_cond_init(&kernel_log_cond,NULL);

	if ( pthread_create(&kernel_log_thread, NULL, (void *)kernel_log_handler, NULL) < 0){
		printf("pthread_create error\n");
	}

	// pthread 조건 변수 신호 보냄 -> kernel_log_handler()의 cond_wait 깨움
	//pthread_cond_signal(&kernel_log_cond);

	/*
	pthread_cond_init()  공유 데이터에 대한 특정 조건에 따라 스레드의 실행을 중지하거나 실행시키는 역할
						   cond 변수를 cond_attr로 지정된 변수 속성을 사용하여 초기화 하거나, NULL인
	                       경우는 기본 속성으로 초기화 한다. */

	// 함수 call한 후 끝날때까지 기다린다.         
	//usleep(1000000);

	pthread_join(kernel_log_thread, NULL); 
	// mutex 파괴
	pthread_mutex_destroy(&kernel_log_mutex);
	pthread_mutex_destroy(&app_log_mutex);
	return 0;
}

void kernel_log_handler(void *ptr)
{
	DIR *dir;
	while(1){
		/* 
			커널 로그 수집
		*/
		// 동작중인 스레드를 중지시킨다.
		// 다른 스레드로부터 signal이나broadcast를 받았을 경우 mutex_lock을 걸고 cond_signal or
		// cond_broadcast를 이용하여 스레드를 깨우고 mutex_unlock로 가지고 있는 mutex를 풀어준다.
		//pthread_cond_wait(&kernel_log_cond, &kernel_log_mutex);
		printf("[%s] opened directory\n",__FUNCTION__);
		dir = opendir(KERN_LOG_DATA);
		if (dir == NULL){
			printf("[%s:%d] could not open directory\n",__FUNCTION__,__LINE__);
			continue;
		}
		log_handler(dir);
		closedir(dir);
		printf("[%s] closed directory\n",__FUNCTION__);
		usleep(1000000);  // 1초 = 1000000
	}
}

void log_handler(DIR *dir)
{
#if 0
	FILE *src_fp, *dst_fp;
	struct dirent *entry;
	char src_file[64];
	char dst_file[64];
	char *buff;
	long Size;
#else
	int sock;
	struct sockaddr_nl src_addr, dst_addr;  // netlink struct
	struct nlmsghdr *nlh;    // netlink msg header
	struct msghdr msg;
	struct iovec iov;
	struct _syslog_msg *syslogmsg;

	int len;
	int i;

	memset(&msg,0x00,sizeof(msg));
	memset(&iov,0x00,sizeof(iov));
	syslogmsg = malloc(sizeof(struct _syslog_msg));
	memset(syslogmsg, 0x00, sizeof(struct _syslog_msg));
	
#endif

	/*
		커널 로그를 수집해서 파일로 떨군다.
	*/
#if 0  // 동적메모리할당과 fopen(), fread(), fwirte()사용하여 기능 구현
	while ((entry = readdir(dir)) != NULL){
		if (!strncmp(entry->d_name, ".", 1) || !strncmp(entry->d_name,"..",2)){
			usleep(500000);
			continue;
		}
		pthread_mutex_lock(&log_ses_mutex);
		// while( (log_count = fread(buff, 1, 1024, src_fp)) > 0 )
		// {
		// 	printf("log_count = %d\n",log_count);
		// 	sprintf(dst_file,"%s/%s", WRITE_LOG_DATA, entry->d_name);
		// 	dst_fp = fopen(dst_file,"wt");
		// 	fwrite( buff, 1, sizeof(buff), dst_fp);
		// }

		/* READ */
		sprintf(src_file, "%s/%s", KERN_LOG_DATA, entry->d_name); //klog
		if ((src_fp = fopen(src_file, "r")) == NULL)
		{
			printf("%s open error\n",src_file);
			return;
		}
		fseek(src_fp, 0, SEEK_END);   //src_fp가 가르키는 파일의 끝점에서, 0바이트 이전으로 이동
		
		Size= ftell(src_fp);          //해당 파일형 포인터가 가리키는 파일의 파일내부포인터의 위치 반환
										//시작점으로부터 몇 바이트 위치에 있는지 long형으로 반환  
		printf("Size=%d\n",Size);
		rewind(src_fp);               //파일 내부푸인터를 시작점으로 옮김 (초기화)
		buff = (char*)malloc(sizeof(char)*Size);
		fread(buff,1,Size,src_fp);
		fclose(src_fp);
		/* WRITE */
		sprintf(dst_file,"%s/%s", WRITE_LOG_DATA, entry->d_name);
		if ((dst_fp = fopen(dst_file,"wt")) == NULL)
		{
			printf("%s open error\n",dst_file);
			return;
		}
		fwrite(buff, 1, Size, dst_fp);
		fclose(dst_fp);;
		unlink(src_file);
		free(buff);
		pthread_mutex_unlock(&log_ses_mutex);
		usleep(1000000);
	}
	return;

#else  // netlink 사용
	if ((sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_USER)) == -1)   // domain, type, protocol
	{
		perror("socket error :");
		exit(1);
	}
	memset(&src_addr, 0, sizeof(src_addr));
	src_addr.nl_family=AF_NETLINK;
	src_addr.nl_groups = 0;
	if(bind(sock, (struct sockaddr *)&src_addr, sizeof(src_addr)))
	{
		perror("bind error");
		close(sock);
		exit(1);
	}
	memset(&dst_addr, 0, sizeof(dst_addr));
	dst_addr.nl_family = AF_NETLINK;
	dst_addr.nl_pid = 0;
	dst_addr.nl_groups = 0;

	nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
	memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
	nlh -> nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
	nlh -> nlmsg_pid = getpid();
	nlh -> nlmsg_flags = 0;

	
	strcpy(NLMSG_DATA(nlh), "The message sended from user space to kernel space");
	/*
		iov_base : 이름
		iov_len : 길이 
	*/
	iov.iov_base = (void *)nlh;
	iov.iov_len = nlh->nlmsg_len;

	msg.msg_name = (void *)&dst_addr;
	msg.msg_namelen = sizeof(dst_addr);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	printf("Sending message to kernel\n");
	sendmsg(sock,&msg,0);
	printf("Waiting for message from kernel\n");
	
	len = recvmsg(sock, &msg, 0);
	//printf("len = %d\n", len);      // 커널로부터 받은 메시지의 길이 
	memcpy(syslogmsg, NLMSG_DATA(nlh),SYSLOG_MSG_SIZE);
	printf("Recevied message = %s\n", syslogmsg->syslog_msg);
	//printf("Received message payload : %s\n", (char *)NLMSG_DATA(nlh));
	collect_log();	
	close(sock);
	return;
#endif
}
/*
	로그 수집 기능 
*/

void collect_log()
{
	/*
		flogs 로그 생성  -->  /var/log/wlogs
	*/
	char log_buff[1024] = "writing...";
	pthread_mutex_lock(&app_log_mutex);
	openlog(PROGRAM_NAME, LOG_CONS | LOG_NDELAY | LOG_PID, LOG_LOCAL0 );  /*  LOG_CONS : 로그를 보내는중 에러 발생하면, 콘솔로 즉시 내용 출력,
	            											LOG_NDELAY : 로그 기술자에 즉시 연결 
	            											LOG_LOG_PID : 각 메시지에 PID 포함 
	            											*/
	syslog(LOG_INFO, "%s -> %s\n",  __FUNCTION__, log_buff ); 
	closelog();
	pthread_mutex_unlock(&app_log_mutex);
}

/*
	추후 프로세스 관리 기능으로 추가 예정
	case 1. -s 옵션 주고 실행 중일 때, 실행중 아닐때,
	case 2. 실행 중 stop 기능 사용
*/
void check_process(int opt)
{
	int pid=getpid();
	if(opt==115){

	}else if(opt==116){
	//	kill(pid,9);
	}else if(opt==100){
		printf("디버그 기능 추가 예정\n");
		exit(1);
	}
}
/*
	ex) ./wlogs -d [start] 와 같이 [start]의 참/거짓 판별 
*/
void check_argument(int argc, char * argv[], int opt, struct option *opts, int optind)
{  
	if (opt == -1 || optind == 1){
		help(PROGRAM_NAME);
		exit(1);
	} else if (opt != 100 && argc > 2){
		help(PROGRAM_NAME);
		exit(1);
	}
}

/*
	도움말
*/
void help(char *name)
{
	printf("\n ----------------------usage--------------------- \n"
		" |\t-s, --start \t Starting to Daemon\t|\n"
		" |\t-t, --stop \t Stopping to Daemon\t|\n"
		" |\t-d, --debug \t Debugging to Daemon\t|\n"
		" |\t\t\t *must need to option*\t|\n"
		" |\t-h, --help \t Help\t\t\t|"
		"\n -----------------------end---------------------- \n"
		); 
}
