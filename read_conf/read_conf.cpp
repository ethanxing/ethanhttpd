#include "read_conf.h"
#include <string.h>
#include <stdio.h>
conf_data *init_conf(int limitnum)
{
	conf_data *pconf = new conf_data(limitnum);
	int ret = pconf->init_conf_data();
	if(ret < 0){
		WARNING_LOG("init_conf_data failed!");
		return NULL;
	}
	return pconf;
}
int read_conf(const char *work_path, const char* confname, struct conf_data *pconf)
{
	if(work_path == NULL || confname == NULL || pconf == NULL){
		WARNING_LOG("Illegal params!");
		return NULL;
	}
	char fullname[PATHSIZE];
	if(work_path[strlen(work_path)-1] == '/'){
		snprintf(fullname, PATHSIZE, "%s%s", work_path, confname);
	}else{
		snprintf(fullname, PATHSIZE, "%s/%s", work_path, confname);
	}

	int ret = read_conf_unit(fullname, pconf, 0);
	if(ret < 0){
		WARNING_LOG("read_conf failed!");
		return -1;
	}
	return ret;
}
int read_conf_unit(const char* fullname, struct conf_data *pconf, int start_index)
{
	if(fullname == NULL || pconf == NULL || start_index < 0){
		WARNING_LOG("Illegal params!");
		return -1;
	}
	FILE *fp = fopen(fullname, "r");
	if(fp == NULL){
		WARNING_LOG("open file [%s] failed!", fullname);
		return -1;
	}

	NOTICE_LOG("read config [%s]", fullname);
	char buf[BUFSIZE];
	char temp_key[WORDSIZE];
	char temp_value[WORDSIZE];

	while(fgets(buf, BUFSIZE, fp) != NULL){
		char *head = buf;
		int len = strlen(buf);
		
		while(*head == ' ' || *head == '\t'){
			++head;
		}
		if(*head == '#' || *head == '\n' || *head == '\r'){
			continue;
		}
		char *tail = strchr(buf, ':');
		*tail = '\0';
		if(tail == head){
			continue;
		}
		char *tail_tmp = tail-1;
		while(*tail_tmp == ' ' || *tail_tmp == '\t'){
			*tail_tmp = '\0';
			--tail_tmp;
		}
		snprintf(temp_key, WORDSIZE, "%s", head);

		if(tail - head + 1 != len){
			head = tail+1;
		}
		tail = buf + len - 1;
		while(*head == ' ' || *head == '\t'){
			++head;
		}
		if(*head == '\n' || *head == '\r'){
			continue;
		}
		if(tail == head){
			continue;
		}
		while(*tail == ' ' || *tail == '\t' || *tail == '\r' || *tail == '\n'){
			*tail = '\0';
			--tail;
		}
		if(head > tail){
			continue;
		}

		snprintf(temp_value, WORDSIZE, "%s", head);

		if(start_index >= pconf->limit_num){
			WARNING_LOG("Out of conf limit size!");
			return -1;
		}
		snprintf(pconf->conf_item_arr[start_index].key, WORDSIZE, "%s", temp_key);
		snprintf(pconf->conf_item_arr[start_index].value, WORDSIZE, "%s", temp_value);
		++start_index;
	}
	pconf->real_num = start_index;
	return 0;
}
