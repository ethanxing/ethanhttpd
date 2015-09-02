#include <>

#define

struct epd_config{
	int max_work_num;
	char doc_root[1024];
	char server_port[8];
	char server_name[1024];
};
init_epd_config(struct epd_config *pconf){
	if(pconf == NULL){
		WARNGING_LOG("init epd config error");
		abort();
	}
	get_conf_int("max_work_num", pconf_data, &max_work_num);
	get_conf_nstr("doc_root", pconf_data, doc_root, 1024);
	get_conf_nstr("server_port", pconf_data, server_port, 8);
	get_conf_nstr("server_name", pconf_data, server_name, 1024);
}
static int get_conf_int(struct conf_data *pconf_data, const char *key, int *value)
{
	if(pconf_data == NULL || key == NULL || value == NULL){
		WARNING_LOG("illegal params!");
		return -1;
	}
	int i = 0;
	for(; i < pconf_data->real_num; ++i){
		if(strncmp(key, pconf_data->conf_item_arr[i].key, strlen(key)) == 0){
			sscanf(pconf_data->conf_item_arr[i].value, "%d", *value);
			break;
		}
	}
	if(i == pconf_data->real_num){
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
	int i = 0;
	for(; i < pconf_data->real_num; ++i){
		if(strncmp(key, pconf_data->conf_item_arr[i].key, strlen(key)) == 0){
			snprintf(value, size, "%s", conf_data->conf_item_arr[i].value);
			break;
		}
	}
	if(i == pconf_data->real_num){
		WARNING_LOG("conf file don't has param [%s] !", key);
		return -1;
	}
	return 0;

}
static int make_socket_non_blocking(int sfd)
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
		WARNING_LOG()"fcntl error: %s", strerror(errno);
		return -1;
	}
	return 0;
}

class  Serv{
public:
	int listenfd;
	char serv_name[1024];
	int listenClient();
	struct epd_conf *pconf;
	Serv(){
		pconf = NULL;
		listenfd  = -1;
	}
};
int Serv::listenClient(const char *serv, socklent_t *addrlenp)
{
	int n;
	const int on = 1;
	struct addrinfo hints, *res = NULL, ressave = NULL;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_flags= AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if((n = getaddrinfo(NULL, serv, &hins, &res)) != 0){
		WARNING_LOG("tcp listen error for %s, %s: %s", host, serv, gai_strerror(n));
	}
	resave = res;
	do{
		listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if(listenfd <£°){
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
	}while( (res = res->next) != NULL );
	if(res == NULL){
		WARNING_LOG("tcp listen error for %s, %s", host, serv);
		return -1;
	}
	listen(listenfd, 1024);
	if(addrlenp){
		*addrlenp = res->ai_addrlen;
	}
	freeaddrinfo(resave);
	return listenfd;
}


