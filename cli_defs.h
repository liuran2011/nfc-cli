#ifndef __CLI_DEFS_H
#define __CLI_DEFS_H

#define CLI_LINE_LENGTH (256)
#define CLI_PROMPT_LENGTH (32)
#define CLI_USER_NAME_LENGTH (32)

#define CLI_DEFAULT_BANNER ("Pearl2 Programmable Virtual Router")
#define CLI_DEFAULT_PROMPT ("pearl2")
#define CLI_USER_NAME_PROMPT ("login:")
#define CLI_PASSWORD_PROMPT ("password:")

#define CLI_PROMPT_CHAR ("#")

#define CLI_CHAR_TAB (9)
#define CLI_CHAR_BACKSPACE (127)
#define CLI_CHAR_RETRANS (27)
#define CLI_CHAR_RETRANS_DIRECT (91)
#define CLI_CHAR_RETRANS_DIRECT_UP (65)
#define CLI_CHAR_RETRANS_DIRECT_DOWN (66)
#define CLI_CHAR_RETRANS_DIRECT_LEFT (68)
#define CLI_CHAR_RETRANS_DIRECT_RIGHT (67)

#define CLI_CMD_REQUIRE_CHAR '<'
#define CLI_CMD_OPTION_CHAR '['
#define CLI_CMD_TYPE_STRING "STRING"
#define CLI_CMD_TYPE_DWORD "DWORD"
#define CLI_CMD_TYPE_IPV4 "x.x.x.x"

#define CLI_MAX_ARG_CNT (32)

#endif
