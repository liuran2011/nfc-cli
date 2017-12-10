#ifndef __CLI_CLIENT_H
#define __CLI_CLIENT_H

#include <string>
#include <ev.h>
#include <stdio.h>

#include "cli_defs.h"

class cli_cmd_node;
class cli_core;
class cli_front_end;

class cli_client
{
public:
	int fd;
    FILE *fp;
    int pos;
    int cursor;
	int auth_mode;
	int auth_mode_user;
    int retrans_mode;
    int auto_complete;
	int echo_enable;
	
	struct ev_loop *loop;
	ev_io read_watcher;
	ev_child sh_watcher;
	int sh_pid;
	
	char buffer[CLI_LINE_LENGTH];
	char user_name[CLI_USER_NAME_LENGTH+1];
	
    cli_cmd_node *cmd_mode;
	cli_cmd_node *current_cmd_node;
	cli_core *core;
    cli_front_end *front_end;
    
	std::string error;
	std::string prompt;
	std::string tty_name;
	std::string login_time;
	
public:
	cli_client(struct ev_loop *loop,cli_core *core,int fd,cli_front_end *front_end);
	void monitor_sh_process(int pid);
	void set_cmd_mode(cli_cmd_node *node);
	cli_cmd_node *get_cmd_mode() { return this->cmd_mode; }
	void set_cmd_current_node(cli_cmd_node *node)
	{
		this->current_cmd_node=node;
	}
	cli_cmd_node *get_cmd_current_node() { return this->current_cmd_node; }
	void set_prompt(const char *prompt) { this->prompt=prompt; }
	void set_error(const char *error) { this->error=error; }
	const char *get_error() { return this->error.c_str();}
	int get_fd() { return this->fd; }
	int init();
	void exit();
};

#endif
