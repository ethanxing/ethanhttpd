#include "read_conf.h"
#include <iostream>
#include "mylog.h"
#include <stdlib.h>
using namespace std;
int main()
{
	log_init();
	conf_data *pconf = init_conf(1);

	NOTICE_LOG("Start to read conf file!");

	read_conf("./conf", "ethanhttpd.conf", pconf);
	
	int num = pconf->real_num;
	for(int i = 0; i < num; ++i){
		cout << pconf->conf_item_arr[i].key << ":" << pconf->conf_item_arr[i].value << endl;
	}

	free(pconf);

	return 0;
}
