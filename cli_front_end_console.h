#ifndef __CLI_FRONT_END_CONSOLE_H
#define __CLI_FRONT_END_CONSOLE_H

#include "cli_front_end.h"

class cli_client;
class cli_core;

class cli_front_end_console: public cli_front_end
{
public:
    cli_front_end_console(struct ev_loop *loop,cli_core *core);
    int init();
    void exit();
    int echo_on(cli_client *client);
    int echo_off(cli_client *client);
    const char *ttyname(cli_client *client);
};

#endif
