// 커널 로그 데이터 경로 선언
#define KERN_LOG_DATA "/home/cglee/study/wlogs/sample_log/log/klog"
#define WRITE_LOG_DATA "/home/cglee/study/wlogs/sample_log/log/clog"

//#define KERN_LOG_DATA "/home/wins/log/klog"
//#define WRITE_LOG_DATA "/home/wins/log/clog"
// buff size 선언
#define BUFF_SIZE 1024
// PROGAME_NAME 선언
#define PROGRAM_NAME "wlogs"

// payload size 선언
#define MAX_PAYLOAD 1024

typedef struct _syslog_header {
	int priority;
	int version;
	int timestamp;
	int hostname;
}Syslog_header;

typedef struct _syslog_data {
	char com;
	char logid;
	char level;
	int sip;
	char sport;
	int dip;
	char dport;
	char proto;
}__attribute__((packed)) Syslog_data;

typedef struct _syslog_msg{
	char msg[1024];
}Syslog_msg;