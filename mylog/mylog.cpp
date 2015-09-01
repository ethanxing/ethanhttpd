#include "mylog.h"
#include <time.h>
#include <memory>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>

const unsigned int LIMIT_SIZE =  (10 * 1024 * 1024); //10M
const int LOG_BUF_SIZE = 2048;
enum SPLIT_TYPE{
	SPLIT_BY_TIME = 0,
	SPLIT_BY_SIZE,
	SPLIT_NUM
};
struct log_file_info{
	int mday;
	char fullname[1024];
	int split_type;
	unsigned int limit_size;
	FILE *fp;
	log_file_info(){
		split_type = SPLIT_BY_TIME;
		limit_size = LIMIT_SIZE;
		fp = NULL;
	}
	~log_file_info(){
		if(fp != NULL){
			fclose(fp);
		}	
	}
};
struct log_info_unit{
	struct log_file_info *p_logf;
	struct log_file_info *p_logf_wf;

	log_info_unit(){
		p_logf = p_logf_wf = NULL;
	}
	~log_info_unit(){
		if(p_logf_wf != NULL){
			delete p_logf_wf;
		}
		if(p_logf != NULL){
			delete p_logf;
		}
	}
	
};

//static std::shared_ptr <log_info_unit> plog_info_unit(new log_info_unit);
static log_info_unit  *plog_info_unit = new log_info_unit();

int check_log_time(log_file_info *p_logf);
int check_log_size(log_file_info *p_logf);
int spilt_log_file(log_file_info *p_logf);

int log_open_file(log_file_info *p_logf)
{
	p_logf->fp = fopen(p_logf->fullname, "a");
	if(p_logf->fp == NULL){
		WARNING_OUT("open log file failed!");
		return -1;
	}
	return 0;
	
}

int log_init(const char* log_path, const char* log_name, int log_split_type, unsigned int limit_size)
{
	if(log_path == NULL || log_name == NULL){
		WARNING_LOG("log_path:[%s], log_name:[%s] is NULL", log_path, log_name);
		return -1;
	}
	plog_info_unit->p_logf = new log_file_info();
	plog_info_unit->p_logf_wf = new log_file_info();
	
	plog_info_unit->p_logf->split_type =  log_split_type;
	plog_info_unit->p_logf_wf->split_type =  log_split_type;

	if(limit_size > LIMIT_SIZE){
		plog_info_unit->p_logf->limit_size = limit_size;
		plog_info_unit->p_logf_wf->limit_size = limit_size;
	}
	if(log_path[strlen(log_path)-1] != '/'){
		snprintf(plog_info_unit->p_logf->fullname, 1024, "%s/%s", log_path, log_name);
		snprintf(plog_info_unit->p_logf_wf->fullname, 1024, "%s/%s.wf", log_path, log_name);
	}else{
		snprintf(plog_info_unit->p_logf->fullname, 1024, "%s%s", log_path, log_name);
		snprintf(plog_info_unit->p_logf_wf->fullname, 1024, "%s%s.wf", log_path, log_name);
	}
	int ret = access(log_path, F_OK);
	if(ret != 0){
		mkdir(log_path, 0700);
	}
	if(log_open_file(plog_info_unit->p_logf) < 0){
		return -1;
	}
	if(log_open_file(plog_info_unit->p_logf_wf) < 0){
		return -1;
	}
	return 0;
	
}

int log_write(const int log_level, const char* fmt, ...)
{
	char log_buf[LOG_BUF_SIZE];
	int data_pos = 0;
	log_file_info *target_logf = NULL;
	switch(log_level){
		case MY_LOG_FATAL:
			target_logf = plog_info_unit->p_logf_wf;
			data_pos = snprintf(log_buf, LOG_BUF_SIZE, "FATAL: ");
			break;
		case MY_LOG_WARNING:                                                                                                             
			target_logf = plog_info_unit->p_logf_wf;
			data_pos = snprintf(log_buf, LOG_BUF_SIZE, "WARNING: ");
			break;
		case MY_LOG_NOTICE:
			target_logf = plog_info_unit->p_logf;
			data_pos = snprintf(log_buf, LOG_BUF_SIZE, "NOTICE: ");
			break;
		case MY_LOG_TRACE:
			target_logf = plog_info_unit->p_logf;
			data_pos = snprintf(log_buf, LOG_BUF_SIZE, "TRACE: ");
			break;
		case MY_LOG_DEBUG:
			target_logf = plog_info_unit->p_logf;
			data_pos = snprintf(log_buf, LOG_BUF_SIZE, "DEBUG: ");
			break;
		default:
			break;

	}
	time_t cur_time = 0;
	struct tm cur_tm;
	time(&cur_time);
	localtime_r(&cur_time, &cur_tm);

	data_pos += snprintf(log_buf+data_pos, LOG_BUF_SIZE-data_pos, "%02d-%02d %02d:%02d:%02d ", cur_tm.tm_mon+1, cur_tm.tm_mday, cur_tm.tm_hour, cur_tm.tm_min, cur_tm.tm_sec);

	va_list args;
	va_start(args, fmt);
	vsnprintf(log_buf+data_pos, LOG_BUF_SIZE-data_pos, fmt, args);
	va_end(args);

	if(target_logf == NULL){
		WARNING_LOG("No where to write log!");
		return -1;
	}
	//split_log_file(target_logf);
	if( spilt_log_file(target_logf) < 0 )
	{
		WARNING_LOG("chek and split log failed!");
	}
	fprintf( target_logf->fp, "%s\n", log_buf);
	fflush(target_logf->fp);

	return 0;
}

int check_log_time(log_file_info *p_logf)
{
	struct stat st;
	int ret = stat(p_logf->fullname, &st);
	if(ret == -1){
		return -1;
	}
	struct tm old_tm, cur_tm;
	localtime_r(&st.st_mtime, &old_tm);
	p_logf->mday = old_tm.tm_mday;

	time_t cur_time = 0;
	time(&cur_time);
	localtime_r(&cur_time, &cur_tm);
	if(cur_tm.tm_mday == p_logf->mday){
		return 0;
	}
	char tmpfilename[1024];
	int len = snprintf(tmpfilename, 1024, "%s.%04d%02d%02d%02d%02d%02d", 
					p_logf->fullname, 1900+cur_tm.tm_year, 1+cur_tm.tm_mon, cur_tm.tm_mday, cur_tm.tm_hour, cur_tm.tm_min, cur_tm.tm_sec);
	int logindex = 0;
	while(access(tmpfilename, F_OK) == 0){
		len += snprintf(tmpfilename+len, 1024-len, ".%d", logindex++);
	}
	
	fflush(p_logf->fp);

	rename(p_logf->fullname, tmpfilename);
	fclose(p_logf->fp);

	if(log_open_file(p_logf) < 0){
		return -1;
	}
	p_logf->mday = cur_tm.tm_mday;
	return 0;
}

int check_log_size(log_file_info *p_logf)
{
	struct stat st;
	int ret = stat(p_logf->fullname, &st);
	if(ret == -1){
		return -1;
	}
	if(st.st_size < p_logf->limit_size){
		return 0;
	}
	time_t cur_time = 0;
	struct tm cur_tm;
	time(&cur_time);
	localtime_r(&cur_time, &cur_tm);
	char tmpfilename[1024];
	int len = snprintf(tmpfilename, 1024, "%s.%04d%02d%02d%02d%02d%02d", 
					p_logf->fullname, 1900+cur_tm.tm_year, cur_tm.tm_mon, 1+cur_tm.tm_mday, cur_tm.tm_hour, cur_tm.tm_min, cur_tm.tm_sec);
	int logindex = 0;
	while(access(tmpfilename, F_OK) == 0){
		len += snprintf(tmpfilename+len, 1024-len, ".%d", logindex++);
	}
	
	fflush(p_logf->fp);
	rename(p_logf->fullname, tmpfilename);
	fclose(p_logf->fp);

	if(log_open_file(p_logf) < 0){
		return -1;
	}
	return 0;

}

int spilt_log_file(log_file_info *p_logf)
{
	int split_type = p_logf->split_type;
	int ret = 0;
	switch(split_type){
		case SPLIT_BY_TIME:
			ret = check_log_time(p_logf);
		case SPLIT_BY_SIZE:
			ret = check_log_size(p_logf);
			break;
		default:
			break;
	}
	return ret;

}

int log_destroy()
{
	if(plog_info_unit != NULL){
		delete plog_info_unit;
		plog_info_unit = NULL;	
	}
	return 0;
}

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

