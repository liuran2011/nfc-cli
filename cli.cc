#include <stdarg.h>

#include "cli.h"
#include "cli_core.h"
#include "pearl2.h"

cli_core *cli_core_obj;

int cli_init(struct ev_loop *loop)
{
    cli_core_obj=new cli_core(loop);
    if(NULL==cli_core_obj)
    {
        log_add(LOG_ERROR,"cli core new failed.");
        return -1;
    }

    if(cli_core_obj->init()<0)
    {
        log_add(LOG_ERROR,"cli core init failed.");
        delete cli_core_obj;
        cli_core_obj=NULL;
        return -1;
    }
    
    return 0;
}

void cli_exit()
{
    cli_core_obj->exit();
    
    delete cli_core_obj;
    cli_core_obj=NULL;
}

int cli_set_banner(char *banner)
{
    cli_core_obj->set_banner(banner);
    
    return 0;
}

int cli_turn_off_banner()
{
    cli_core_obj->turn_off_banner();
    
    return 0;
}

int ci_turn_on_banner()
{
    cli_core_obj->turn_on_banner();
    
    return 0;
}

int cli_set_prompt(char *prompt)
{   
    cli_core_obj->set_prompt(prompt);
    
    return 0;
}

int cli_add_client(int fd)
{
    return cli_core_obj->server.add_client(fd,&cli_core_obj->console);
}

void cli_del_client(int fd)
{
    return cli_core_obj->server.del_client(fd);
}

void cli_out(void *c,const char *format,...)
{
    va_list va;
    cli_client *client=NULL;

    client=(cli_client *)c;

    if(!client->echo_enable)
    {
        return;
    }
    
    va_start(va,format);
    vfprintf(client->fp,format,va);
    va_end(va);

    fflush(client->fp);
}

