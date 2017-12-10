#ifndef __CLI_CMD_H
#define __CLI_CMD_H

#include <string>
#include <vector>
#include <map>

#include "cli_client.h"

typedef void (*cli_cmd_exec_func)(cli_client *client,int argc,const char **argv);

class cli_cmd_node;

typedef enum
{
	CLI_CMD_PARAM_TYPE_STRING,
	CLI_CMD_PARAM_TYPE_NUM,
	CLI_CMD_PARAM_TYPE_IPV4,
	CLI_CMD_PARAM_TYPE_RANGE,
	CLI_CMD_PARAM_TYPE_INVALID
}cli_cmd_param_type;

class cli_cmd_node
{
public:
    std::string name;
    std::string doc;
    cli_cmd_exec_func func;

	int is_param;
	int optional;
	cli_cmd_param_type type;
	int min_value;
	int max_value;
	
	int mode;
	std::string prefix;
	
    std::map<std::string,cli_cmd_node *> list;

	cli_cmd_node *parent;
	cli_cmd_node *prev_mode;

private:
	void init_data();
	int parse(std::string &token);
	void del_tree(cli_cmd_node *node);
	void strtoken(const char * s,
		           const char * sep, 
		           std::vector < std::string > & vec);
	void find_node(cli_cmd_node * node,
		              const char * s, 
		              std::vector <cli_cmd_node *> & vec);
	int check_param(cli_cmd_node *node,std::string &param);
	
public:
    cli_cmd_node(int mode);
    cli_cmd_node(std::string name,std::string doc,cli_cmd_exec_func func,int mode);
	~cli_cmd_node();

	int add_default_cmd();
	void add_cmd(struct cli_cmd_t *cmd);
	cli_cmd_node *add_mode(struct cli_mode_t *mode);
	const char *get_mode_prefix();
	void set_mode_prefix(const char *prefix) { this->prefix=prefix; }
	cli_cmd_node *get_previous_mode() { return this->prev_mode; }
	void show_tip(cli_client *client);
	int exec_cmd(cli_client *client);
	void candidate(cli_client *client,std::vector<std::string> &vec);
};

struct cli_cmd_t
{
    const char *name;
    const char *doc;
    cli_cmd_exec_func func;
};

struct cli_mode_t
{
	const char *name;
	const char *doc;
	cli_cmd_exec_func func;
	const char *prefix;
};

#endif
