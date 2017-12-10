#ifndef __CLI_FRONT_END_TELNET_H
#define __CLI_FRONT_END_TELNET_H

#include <ev.h>

#include "cli_front_end.h"

#define CLI_TELNET_DEFAULT_PORT 23
#define CLI_TELNET_DEFAULT_CAPACITY 16

class cli_client;

class cli_front_end_telnet:public cli_front_end
{
public:
    int fd;
    struct ev_loop *loop;
    ev_io read_watcher;
    
public:
    cli_front_end_telnet(struct ev_loop *loop,cli_core *core);
    int init();
    void exit();
    int echo_on(cli_client *client);
    int echo_off(cli_client *client);
    const char *ttyname(cli_client *client);
    
};

#endif
