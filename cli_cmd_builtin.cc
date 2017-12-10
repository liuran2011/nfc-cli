#include <list>
#include <string>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include "cli_cmd_builtin.h"
#include "cli_client.h"
#include "cli_core.h"
#include "cli.h"

void cli_cmd_log_set_level(cli_client *client,int argc,const char **argv)
{
    cli_out(client,"argc:%d\r\n",argc);

    for(int i=0;i<argc;i++)
    {
        cli_out(client,"%d %s\r\n",i,argv[i]);
    }
}

struct cli_cmd_t cli_cmd_log_set_level_cmd=
{
    "log set level <STRING>",
    "Log tune\n"
         "Set log parameter.\n"
         "Log level.\n"
         "Level.\n",
    cli_cmd_log_set_level,
};

void cli_cmd__exit(cli_client *client,int argc,const char **argv)
{
    client->set_cmd_mode(client->get_cmd_mode()->get_previous_mode());
}

struct cli_cmd_t cli_cmd_exit_cmd=
{
    "exit",
    "exit",
    cli_cmd__exit,
};

void cli_cmd_history(cli_client *client,int argc,const char **argv)
{
    std::list<std::string>::const_iterator it;

    for(it=client->core->history.begin();
        it!=client->core->history.end();
        it++)
    {
        cli_out(client,"%s\r\n",
                (*it).c_str());
    }
}

struct cli_cmd_t cli_cmd_history_cmd=
{
	"history",
	"Show history commands",
	cli_cmd_history,
};

void cli_cmd_who(cli_client *client,int argc,const char **argv)
{
    client->core->server.show_clients(client);
}

struct cli_cmd_t cli_cmd_who_cmd=
{
    "who",
    "Show users who have logined in",
    cli_cmd_who,
};

void cli_cmd_linux_shell(cli_client *client,int argc,const char **argv)
{
    pid_t pid;

    /* turn on line mode.*/
    client->front_end->echo_on(client);
    
    pid=fork();
    if(pid<0)
    {
        cli_out(client,"fork process failed.error %s\r\n",
                strerror(errno));
        client->front_end->echo_off(client);
        return;
    }
    else if(0==pid)
    {
        execl("/bin/bash","/bin/bash",NULL);
        fprintf(stderr,"errno:%d %s\n",errno,strerror(errno));
        exit(0);
    }
    else
    {
        client->monitor_sh_process(pid);
    }    
}

struct cli_cmd_t cli_cmd_linux_shell_cmd=
{
    "linux-shell",
    "Change to linux shell",
    cli_cmd_linux_shell
};

void cli_cmd_configure(cli_client *client,int argc,const char **argv)
{
    client->set_cmd_mode(client->get_cmd_current_node());    
}

struct cli_mode_t cli_cmd_configure_mode=
{
    "configure",
    "Enter configure mode",
    cli_cmd_configure,
    "(config)#"
};


