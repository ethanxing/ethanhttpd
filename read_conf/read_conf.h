#ifndef _READ_CONF_H_
#define _READ_CONF_H_
#include "mylog.h"
#include <stdio.h>

const int PATHSIZE = 1024;
const int WORDSIZE = 1024;
const int LIMITNUM = 1024;
const int BUFSIZE = 2048;

struct conf_item{
	char key[WORDSIZE];
	char value[WORDSIZE];
};
struct conf_data{
	int limit_num;
	int real_num;
	conf_item *conf_item_arr;
	conf_data(int num){
		limit_num = num < LIMITNUM ? LIMITNUM : num;
		real_num = 0;
	}
	int init_conf_data(){
		conf_item_arr = new conf_item[limit_num];
		if(conf_item_arr == NULL){
			WARNING_LOG("No enough memory!");
			return -1;
		}
		return 0;
	}
	~conf_data(){
		if(conf_item_arr != NULL){
			delete[] conf_item_arr;
			conf_item_arr = NULL;
		}
	}

};

conf_data *init_conf(int limit_num);

int read_conf(const char *work_path, const char* confname, struct conf_data *pconf);

int read_conf_unit(const char* fullname, struct conf_data *pconf, int start_index);

#endif
