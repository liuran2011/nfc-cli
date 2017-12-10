#ifndef __CLI_CORE_H
#define __CLI_CORE_H

#include <string>
#include <ev.h>

#include "cli_cmd.h"
#include "cli_history.h"
#include "cli_server.h"
#include "cli_user.h"
#include "cli_front_end_telnet.h"
#include "cli_front_end_console.h"

class cli_core
{
private:
    std::string banner;
    int banner_enable;
    std::string prompt;
	int auth_enable;
    struct ev_loop *loop;

public:	
    cli_cmd_node cli_cmd;
    cli_history history;
	cli_server server;
	cli_user user;
	cli_front_end_telnet telnet;
	cli_front_end_console console;
	
private:
	void cursor_to_up(cli_client * client);
	void cursor_to_down(cli_client * client);
	void cursor_to_left(cli_client * client);
	void cursor_to_right(cli_client * client);
	void cursor_to_home(cli_client *client);
	
	void retrans_char_proc(cli_client * client,unsigned char c);
	void auto_complete(cli_client *client);
	void common_char_proc(cli_client * client,unsigned char c);
	void cmd_process(cli_client * client);
	void auth_proc(cli_client *client,unsigned char c);
	
	void send_nl_cr_to_client(cli_client * client);
	void send_buffer_to_client(cli_client *client);
	
	void add_char(cli_client * client,unsigned char c);
	void clear_buffer(cli_client *client);
	void remove_char(cli_client *client);
	void show_tip(cli_client * client);

public:
	cli_core(struct ev_loop *loop);
	int init();
	void exit();
	void process_chars(cli_client *client,const unsigned char *buffer,int len);
	void send_banner_to_client(cli_client * client);
	void send_prompt_to_client(cli_client *client);
	void send_auth_to_client(cli_client *client);

	cli_cmd_node *get_cmd_root() 
	{
		return &(this->cli_cmd);
	}
	
	void set_banner(const char *banner) 
	{
		this->banner=banner;
	}

	void set_prompt(const char *prompt)
	{
		this->prompt=prompt;
	}

	void turn_on_banner() 
	{
		banner_enable=1;
	}

	void turn_off_banner()
	{
		banner_enable=0;
	}

	int banner_on() 
	{
		return banner_enable;
	}

	void enable_auth()
	{
		auth_enable=1;
	}

	void disable_auth()
	{
		auth_enable=0;
	}

	int need_auth()
	{
		return auth_enable;
	}
};

#endif
