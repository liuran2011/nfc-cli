#ifndef __CLI_CMD_BUILTIN_H
#define __CLI_CMD_BUILTIN_H

#include "cli_cmd.h"

extern struct cli_cmd_t cli_cmd_log_set_level_cmd;
extern struct cli_cmd_t cli_cmd_exit_cmd;
extern struct cli_cmd_t cli_cmd_history_cmd;
extern struct cli_cmd_t cli_cmd_linux_shell_cmd;
extern struct cli_cmd_t cli_cmd_who_cmd;

extern struct cli_mode_t cli_cmd_configure_mode;

#endif
