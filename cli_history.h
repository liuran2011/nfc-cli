#ifndef __CLI_HISTORY_H
#define __CLI_HISTORY_H

#include <list>
#include <string>

#define CLI_HISTORY_DEFAULT_CAPACITY (20)

/* TODO. multiple client support.*/
class cli_history
{
private:
	std::list<std::string> history;
	int pos;
	int capacity;
	
public:
	cli_history();
	int add_cmd(char *cmd);
	const char *prev_cmd();
	const char *next_cmd();

	std::list<std::string>::const_iterator begin() { return history.begin();}
	std::list<std::string>::const_iterator end() { return history.end();}
};

#endif
