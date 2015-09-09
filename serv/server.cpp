#include "server.h"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>

static int get_conf_int(struct conf_data *pconf_data, const char *key, int *value)
{
	if(pconf_data == NULL || key == NULL || value == NULL){
		WARNING_LOG("illegal params!");
		return -1;
	}
	int n = 0;
	int i = 0;
	n = pconf_data->real_num;
	for(; i < n; ++i){
		if(strncmp(key, pconf_data->conf_item_arr[i].key, strlen(key)) == 0){
			sscanf((pconf_data->conf_item_arr)[i].value, "%d", value);
			break;
		}
	}
	if(i == n){
		WARNING_LOG("conf file don't has param [%s] !", key);
		return -1;
	}
	return 0;
}
static int get_conf_nstr(struct conf_data *pconf_data, const char *key, char *value, const int size)
{
	if(pconf_data == NULL || key == NULL || value == NULL){
		WARNING_LOG("illegal params!");
		return -1;
	}
	int n = 0;
	int i = 0;
	n = pconf_data->real_num;
	for(; i < n; ++i){
		if(strncmp(key, pconf_data->conf_item_arr[i].key, strlen(key)) == 0){
			snprintf(value, size, "%s", (pconf_data->conf_item_arr)[i].value);
			break;
		}
	}
	if(i == n){
		WARNING_LOG("conf file don't has param [%s] !", key);
		return -1;
	}
	return 0;

}
static int init_epd_config(struct epd_config *pepd_conf, struct conf_data *pconf_data){
	if(pepd_conf == NULL){
		WARNING_LOG("init epd config error");
		return -1;
	}
	get_conf_int(pconf_data, "max_work_num", &(pepd_conf->max_work_num));
	get_conf_nstr(pconf_data, "doc_root", pepd_conf->doc_root, 1024);
	get_conf_nstr(pconf_data, "server_port", pepd_conf->server_port, 8);
	get_conf_nstr(pconf_data, "server_name", pepd_conf->server_name, 1024);
	return 0;
}
int make_socket_non_blocking(int sfd)
{
	int flags, s;
	flags = fcntl(sfd, F_GETFL, 0);
	if(flags == -1){
		WARNING_LOG("fcntl error: %s", strerror(errno));
		return -1;
	}
	flags |= O_NONBLOCK;
	s = fcntl(sfd, F_SETFL, flags);
	if(s == -1){
		WARNING_LOG("fcntl error: %s", strerror(errno));
		return -1;
	}
	return 0;
}

int Serv::initServ(struct conf_data *pconf_data)
{
	pepd_conf = new struct epd_config;
	if(pepd_conf == NULL){
		WARNING_LOG("Out of memory.");
		return -1;
	}
	if( init_epd_config(pepd_conf, pconf_data) < 0 ){
		WARNING_LOG("init_epd_config failed");
		return -1;
	}	
	if( bind_and_listen(pepd_conf->server_port) < 0){
		WARNING_LOG("failed to bind and listen!");
		return -1;
	}
	return 0;
}

int Serv::bind_and_listen(const char *serv_port) 
{ 
	int n; const int on = 1; 
	struct addrinfo hints;
    struct addrinfo	*res = NULL, *ressave = NULL; 
	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_flags= AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if((n = getaddrinfo(NULL, serv_port, &hints, &res)) != 0){
		WARNING_LOG("tcp listen error for %s : %s", serv_port, gai_strerror(n));
	}
	ressave = res;
	do{
		listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if(listenfd < 0){
			continue;
		}
		if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0){
			WARNING_LOG("setsockopt REUSEADDR error!");
			break;
		}
		if(bind(listenfd, res->ai_addr, res->ai_addrlen) == 0){
			break;
		}
		close(listenfd);
	}while( (res = res->ai_next) != NULL );
	if(res == NULL){
		WARNING_LOG("tcp listen error for %s", serv_port);
		return -1;
	}
	make_socket_non_blocking(listenfd);
	listen(listenfd, 1024);
	/*
	if(addrlenp){
		*addrlenp = res->ai_addrlen;
	}
	*/
	freeaddrinfo(ressave);
	return listenfd;
}

