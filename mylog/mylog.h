#ifndef _MYLOG_H_
#define _MYLOG_H_
#include <stdio.h>

enum my_log_type{
	MY_LOG_FATAL,
	MY_LOG_WARNING,
	MY_LOG_NOTICE,
	MY_LOG_TRACE,
	MY_LOG_DEBUG,
	MY_LOG_NUM
};
int log_init(const char* log_path = "./log", const char* log_name = "ehttpd.log", int log_split_type = 1, unsigned int limit_size = 0);

int spilt_log_file();

int log_write(const int log_level, const char* fmt, ...);

int log_destroy();

#define WARNING_OUT(fmt, arg...) do{ \
		fprintf(stderr, fmt "\n", ##arg); \
}while(0)

#define FATAL_LOG(fmt, arg...) do{ \
		log_write( MY_LOG_FATAL, "[%s:%d]" fmt, __FILE__, __LINE__, ##arg); \
}while(0);

#define WARNING_LOG(fmt, arg...)  do { \
	   	log_write( MY_LOG_WARNING, "[%s:%d]" fmt, __FILE__, __LINE__, ##arg);  \
}while(0);

#define TRACE_LOG(fmt, arg...) do{ \
		log_write( MY_LOG_TRACE, "[%s:%d]" fmt, __FILE__, __LINE__, ##arg); \
}while(0);

#define NOTICE_LOG(fmt, arg...) do{ \
		log_write( MY_LOG_NOTICE, "[%s:%d]" fmt, __FILE__, __LINE__, ##arg); \
}while(0);

#define DEBUG_LOG(fmt, arg...) do{ \
		log_write( MY_LOG_DEBUG, "[%s:%d]" fmt, __FILE__, __LINE__, ##arg); \
}while(0);

#endif
