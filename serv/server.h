
#ifndef _SERVER_H_
#define _SERVER_H_
#include "mylog.h"
#include "read_conf.h"

struct epd_config{
	int max_work_num;
	char doc_root[1024];
	char server_port[8];
	char server_name[1024];
};

class  Serv{
public:
	int listenfd;
	struct epd_config *pepd_conf;
	Serv()
	{
		pepd_conf = NULL;
		listenfd  = -1;
	}
	~Serv()
	{
		delete pepd_conf;
		pepd_conf = NULL;
	}
	int bind_and_listen(const char *port);
	int initServ(struct conf_data *pconf_data);
}; 
int make_socket_non_blocking(int sfd);

#endif
