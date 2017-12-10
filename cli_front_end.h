#ifndef __CLI_FRONT_END_H
#define __CLI_FRONT_END_H

class cli_core;
class cli_client;

class cli_front_end
{
public:
    cli_core *core;
    
public:
    cli_front_end(cli_core *core);
    virtual int init()=0;
    virtual void exit()=0;
    virtual int echo_off(cli_client *client)=0;
    virtual int echo_on(cli_client *client)=0;
    virtual const char *ttyname(cli_client *client)=0;
};

#endif
