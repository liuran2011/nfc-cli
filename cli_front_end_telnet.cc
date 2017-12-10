#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>

#include "cli_front_end_telnet.h"
#include "pearl2.h"
#include "cli_core.h"
#include "cli_client.h"

void connect_arrive(struct ev_loop *loop,ev_io *w,int events)
{
    cli_front_end_telnet *front_end=NULL;
    struct sockaddr_in addr;
    socklen_t addr_len;
    int new_fd=-1;
    
    front_end=container_of(w,cli_front_end_telnet,read_watcher);

    addr_len=sizeof(struct sockaddr_in);
    new_fd=accept(front_end->fd,(struct sockaddr*)&addr,&addr_len);
    if(new_fd<0)
    {
        log_add(LOG_ERROR,"accept new client failed.error:%d %s",
                errno,strerror(errno));
        return;
    }

    front_end->core->server.add_client(new_fd,front_end);
}

cli_front_end_telnet::cli_front_end_telnet(struct ev_loop * loop,cli_core *core):
    cli_front_end(core)
{
    this->loop=loop;
    this->fd=-1;
}

int cli_front_end_telnet::init()
{
    int sock;
    int state=1;
    struct sockaddr_in addr;
    
    sock=socket(AF_INET,SOCK_STREAM,0);
    if(sock<0)
    {
        log_add(LOG_ERROR,"telnet front end, socket failed. error:%d %s",
                errno,strerror(errno));
        return -1;
    }

    setsockopt (sock,SOL_SOCKET, 
                SO_REUSEADDR,&state,
                sizeof (state));
                
    memset(&addr,0,sizeof(struct sockaddr));
    addr.sin_family=AF_INET;
    addr.sin_port=htons(CLI_TELNET_DEFAULT_PORT);
    addr.sin_addr.s_addr=INADDR_ANY;

    if(bind(sock,(struct sockaddr *)&addr,sizeof(addr))<0)
    {
        log_add(LOG_ERROR,"telnet front end,bind sock failed.error:%d %s",
                errno,strerror(errno));
        close(sock);
        return -1;
    }
    
    if(listen(sock,CLI_TELNET_DEFAULT_CAPACITY)<0)
    {
        log_add(LOG_ERROR,"telnet front end, listen failed.error:%d %s",
                errno,strerror(errno));
        close(sock);
        return -1;
    }

    this->fd=sock;

    ev_io_init(&(this->read_watcher),connect_arrive,this->fd,EV_READ);
    ev_io_start(this->loop,&(this->read_watcher));
    
    return 0;
}

void cli_front_end_telnet::exit()
{
    if(this->fd!=-1)
    {
        ev_io_stop(this->loop,&(this->read_watcher));
        
        close(this->fd);
        this->fd=-1;
    }    
}

int cli_front_end_telnet::echo_on(cli_client *client)
{
    return 0;
}

int cli_front_end_telnet::echo_off(cli_client *client)
{
    return 0;
}

const char *cli_front_end_telnet::ttyname(cli_client *client)
{
    return NULL;
}