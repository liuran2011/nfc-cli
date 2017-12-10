#ifndef __CLI_SERVER_H
#define __CLI_SERVER_H

#include <list>
#include <ev.h>

class cli_client;
class cli_core;
class cli_front_end;

class cli_server
{
private:
    struct ev_loop *loop;
    std::list<cli_client *> client_list;
	cli_core *core;
	
public:
	cli_server(struct ev_loop *loop,cli_core *core)  
	{
		this->loop=loop;
		this->core=core;
	}
	~cli_server();

	int add_client(int fd,cli_front_end *front_end);
	void del_client(int fd);
	void show_clients(cli_client *c);
};

#endif
