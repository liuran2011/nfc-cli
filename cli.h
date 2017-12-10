#ifndef __CLI_H
#define __CLI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ev.h>

int cli_init(struct ev_loop *loop);
void cli_exit();

int cli_add_client(int fd);
void cli_del_client(int fd);

int cli_set_banner(char *banner);
int cli_turn_off_banner();
int cli_turn_on_banner();

int cli_set_prompt(char *prompt);

void cli_out(void *,const char *,...);

#ifdef __cplusplus
}
#endif
#endif
