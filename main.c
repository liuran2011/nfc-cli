#include "cli.h"
#include <ev.h>
#include <stdio.h>
#include <signal.h>

int main()
{
	signal(SIGTTIN,SIG_IGN);
	
    cli_init(EV_DEFAULT);
    cli_add_client(0);

    ev_run(EV_DEFAULT,0);

    return 0;
}
