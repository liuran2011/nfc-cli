#include <termios.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>

#include "cli_front_end_console.h"
#include "pearl2.h"
#include "cli_client.h"

cli_front_end_console::cli_front_end_console(struct ev_loop * loop,cli_core * core):
    cli_front_end(core)
{
}

int cli_front_end_console::init()
{
    return 0;
}

void cli_front_end_console::exit()
{
}

int cli_front_end_console::echo_on(cli_client *client)
{
    struct termios term;
    int fd=client->fd;
    
    if(!isatty(fd))
    {
        log_add(LOG_DEBUG,"fd %d not a tty.",fd);
        return 0;
    }

    if(tcgetattr(fd,&term)<0)
    {
        log_add(LOG_ERROR,"tcgetattr %d failed.error:%d %s",fd,errno,
                strerror(errno));
        return -1;
    }

    term.c_lflag|=(ICANON|ECHO);
    
    if(tcsetattr(fd,TCSANOW,&term)<0)
    {
        log_add(LOG_ERROR,"tcsetattr %d failed.error:%d %s",
                fd,errno,strerror(errno));
        return -1;
    }

    return 0;
}

int cli_front_end_console::echo_off(cli_client *client)
{
    struct termios term;
    int fd=client->fd;
    
    if(!isatty(fd))
    {
        log_add(LOG_DEBUG,"fd %d not a tty.",fd);
        return 0;
    }

    if(tcgetattr(fd,&term)<0)
    {
        log_add(LOG_ERROR,"tcgetattr %d failed.error:%d %s",fd,errno,
                strerror(errno));
        return -1;
    }

    term.c_lflag&=~(ICANON|ECHO);
    term.c_cc[VMIN]=0;
    term.c_cc[VTIME]=0;

    if(tcsetattr(fd,TCSANOW,&term)<0)
    {
        log_add(LOG_ERROR,"tcsetattr %d failed.error:%d %s",
                fd,errno,strerror(errno));
        return -1;
    }

    return 0;
}

const char *cli_front_end_console::ttyname(cli_client *client)
{
    basename(::ttyname(client->fd));
}