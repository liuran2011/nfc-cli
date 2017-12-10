#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stddef.h>
#include <time.h>

#include "cli_client.h"
#include "pearl2.h"
#include "cli_core.h"
#include "cli_cmd.h"
#include "cli_front_end.h"

void data_arrive(struct ev_loop * l,ev_io * w,int events)
{
    cli_client *client=NULL;
    int len=0;
    unsigned char buffer[CLI_LINE_LENGTH];

    client=container_of(w,cli_client,read_watcher);
    
    len=read(client->get_fd(),buffer,CLI_LINE_LENGTH);

    if(len>0)
    {
        client->core->process_chars(client,buffer,len);
    }   
}

void sh_process_exit(struct ev_loop *l,ev_child *c,int events)
{
    cli_client *client=NULL;

    client=container_of(c,cli_client,sh_watcher);
    client->front_end->echo_off(client);

    ev_child_stop(client->loop,&(client->sh_watcher));
}

cli_client::cli_client(struct ev_loop *loop,cli_core *core,
                       int fd,cli_front_end *front_end)
{
    this->fd=fd;
    this->fp=NULL;
    this->pos=0;
    this->cursor=0;
    this->auth_mode=1;
    this->auth_mode_user=1;
    this->retrans_mode=0;
    this->auto_complete=0;
    this->loop=loop;
    this->buffer[0]=0;
    this->cmd_mode=NULL;
    this->current_cmd_node=NULL;
    this->core=core;
    this->echo_enable=1;
    this->front_end=front_end;
}

int cli_client::init()
{
    int fd_flags=0;

    fd_flags=fcntl(fd,F_GETFL);
    fd_flags|=O_NONBLOCK;
    if(fcntl(fd,F_SETFL,fd_flags)<0)
    {
        log_add(LOG_ERROR,"set fd %d to non block mode failed. error:%d %s",
                fd,errno,strerror(errno));
        return -1;
    }

    if(front_end->echo_off(this)<0)
    {
        return -1;
    }
    
    this->fp=fdopen(fd,"w");
    if(NULL==this->fp)
    {
        log_add(LOG_ERROR,"fdopen %d failed.",fd);
        return -1;
    }

    this->tty_name=front_end->ttyname(this);
    this->loop=loop;
    this->core=core;
    this->fd=fd;
    this->prompt=CLI_PROMPT_CHAR;

    time_t tm;
    time(&tm);
    this->login_time=asctime(localtime(&tm));
    
    ev_io_init(&(this->read_watcher),data_arrive,fd,EV_READ);
    ev_io_start(this->loop,&(this->read_watcher));
    
    return 0;
}

void cli_client::exit()
{
    ev_io_stop(this->loop,&(this->read_watcher));
}

void cli_client::set_cmd_mode(cli_cmd_node * node)
{
	this->cmd_mode=node;
	this->prompt=node->get_mode_prefix();
}

void cli_client::monitor_sh_process(int pid)
{
    ev_child_init(&(this->sh_watcher),sh_process_exit,pid,0);
    ev_child_start(this->loop,&(this->sh_watcher));
    this->sh_pid=pid;
}