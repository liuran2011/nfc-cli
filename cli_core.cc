#include <string.h>

#include "cli_core.h"
#include "cli_defs.h"
#include "cli.h"
#include "cli_cmd_builtin.h"

cli_core::cli_core(struct ev_loop *loop):
    server(loop,this),cli_cmd(1),telnet(loop,this),
    console(loop,this)
{
    banner_enable=1;
    auth_enable=1;
    this->loop=loop;
    banner=CLI_DEFAULT_BANNER;
    prompt=CLI_DEFAULT_PROMPT;

    /* add prefix to root node.*/
    this->cli_cmd.set_mode_prefix("#");
    
    /* add default cmds to root node.*/
    this->cli_cmd.add_default_cmd();
    this->cli_cmd.add_cmd(&cli_cmd_history_cmd);
    this->cli_cmd.add_cmd(&cli_cmd_linux_shell_cmd);
    this->cli_cmd.add_cmd(&cli_cmd_who_cmd);
    
    cli_cmd_node *mode_node=this->cli_cmd.add_mode(&cli_cmd_configure_mode);
    mode_node->add_cmd(&cli_cmd_log_set_level_cmd);
}

int cli_core::init()
{
    if(telnet.init()<0)
    {
        return -1;
    }
}

void cli_core::exit()
{
    telnet.exit();
}

void cli_core::send_prompt_to_client(cli_client *client)
{
    cli_out(client,"%s",this->prompt.c_str());
    cli_out(client,"%s",client->prompt.c_str());
}

void cli_core::send_nl_cr_to_client(cli_client *client)
{
    cli_out(client,"\r\n");
}

void cli_core::send_banner_to_client(cli_client *client)
{
    cli_out(client,"%s",this->banner.c_str());
}

void cli_core::clear_buffer(cli_client *client)
{
    client->pos=0;
    client->cursor=0;
    client->buffer[0]=0;
}

void cli_core::cmd_process(cli_client *client)
{
    send_nl_cr_to_client(client);
        
    if(client->pos>0)
    {
        this->history.add_cmd(client->buffer);

        int len=0;
        
        if(this->cli_cmd.exec_cmd(client)<0)
        {
            len=strlen(client->get_error());
            
            /* write error info to client.*/
            cli_out(client,"%s",client->get_error());                            
        }

        send_nl_cr_to_client(client);
    }
    
    send_prompt_to_client(client);
    clear_buffer(client);
}

void cli_core::send_buffer_to_client(cli_client *client)
{
    cli_out(client,"%s",client->buffer);
}

void cli_core::send_auth_to_client(cli_client *client)
{
    send_nl_cr_to_client(client);
    send_banner_to_client(client);
    send_nl_cr_to_client(client);
    cli_out(client,"%s",CLI_USER_NAME_PROMPT);
}
	
void cli_core::show_tip(cli_client *client)
{
    cli_cmd_node *node=NULL;
    
    send_nl_cr_to_client(client);

    client->cmd_mode->show_tip(client);
    
    send_nl_cr_to_client(client);
    send_prompt_to_client(client);
    send_buffer_to_client(client);
}

void cli_core::remove_char(cli_client *client)
{   
    int i=0;
    
    if(0==client->cursor)
    {
        return;
    }

    if(client->cursor!=client->pos)
    {
        cli_out(client,"\b");

        for(i=client->cursor;i<client->pos;i++)
        {
            cli_out(client,"%c",client->buffer[i]);
            client->buffer[i-1]=client->buffer[i];
        }

        cli_out(client," ");
    }

    cli_out(client,"\b \b");

    client->pos--;
    client->buffer[client->pos]=0;
    client->cursor--;
    
    for(i=0;i<client->pos-client->cursor;i++)
    {
        cli_out(client,"\b");
    }
}

void cli_core::add_char(cli_client *client,unsigned char c)
{
    client->buffer[client->pos]=c;
    client->pos++;
    client->buffer[client->pos]='\0';
    client->cursor++;

    cli_out(client,"%c",c);
}

void cli_core::cursor_to_left(cli_client *client)
{
    client->retrans_mode=0;

    if(0==client->cursor)
    {
        return;
    }

    client->cursor--;
    
    cli_out(client,"\b");
}

void cli_core::cursor_to_up(cli_client *client)
{
    const char *prev_cmd=NULL;
    int cmd_len=0;
    
    client->retrans_mode=0;

    prev_cmd=this->history.prev_cmd();

    if(NULL==prev_cmd)
    {
        return;
    }

    while(client->cursor>0)
    {
        cli_out(client,"\b");
        client->cursor--;
    }
    
    cmd_len=strlen(prev_cmd);
    cli_out(client,"%s",prev_cmd);
    client->cursor=cmd_len;
}

void cli_core::cursor_to_down(cli_client *client)
{
    const char *cmd=NULL;
    int cmd_len=0;
    
    client->retrans_mode=0;

    cmd=this->history.next_cmd();
    if(NULL==cmd)
    {
        return;
    }

    while(client->cursor>0)
    {
        cli_out(client,"\b");
        client->cursor--;
    }
    
    cmd_len=strlen(cmd);
    cli_out(client,"%s",cmd);
    client->cursor=cmd_len;
}

void cli_core::cursor_to_home(cli_client * client)
{
   
}

void cli_core::cursor_to_right(cli_client *client)
{
    client->retrans_mode=0;
    
    if(client->cursor>=client->pos)
    {
        return;
    }

    cli_out(client,"%c",client->buffer[client->cursor]);

    client->cursor++;
}

void cli_core::auto_complete(cli_client *client)
{
    std::vector<std::string> candi_vec;
    std::vector<std::string>::iterator it;

    this->cli_cmd.candidate(client,candi_vec);
    if(candi_vec.size()==0)
    {
        return;
    }

    if(candi_vec.size()==1)
    {
        /* only one command matches, auto complete .*/
        strcpy(client->buffer,candi_vec[0].c_str());
        strcat(client->buffer," ");
        client->pos=candi_vec[0].length()+1;
        client->cursor=candi_vec[0].length()+1;
    }
    else
    {        
        send_nl_cr_to_client(client);
        
        for(it=candi_vec.begin();it!=candi_vec.end();it++)
        {
            cli_out(client,"%s ",(*it).c_str());        
        }
    }
    
    send_nl_cr_to_client(client);
    send_prompt_to_client(client);
    cli_out(client,"%s",client->buffer);
}

void cli_core::common_char_proc(cli_client *client,unsigned char c)
{
    switch(c)
    {
        case '\n':
            cmd_process(client);
            break;
        case CLI_CHAR_BACKSPACE:
            remove_char(client);
            break;   
        case CLI_CHAR_RETRANS:
            client->retrans_mode=1;
            break;
        case CLI_CHAR_TAB:
            client->auto_complete=1;
            break;
        case '?':
            show_tip(client);
            break;
        default:
            add_char(client,c);
            break;
    }
}

void cli_core::retrans_char_proc(cli_client *client,
                                     unsigned char c)
{
    switch(c)
    {
        case CLI_CHAR_RETRANS_DIRECT:
            break;
        case CLI_CHAR_RETRANS_DIRECT_UP:
            cursor_to_up(client);
            break;
        case CLI_CHAR_RETRANS_DIRECT_DOWN:
            cursor_to_down(client);            
            break;
        case CLI_CHAR_RETRANS_DIRECT_LEFT:
            cursor_to_left(client);
            break;
        case CLI_CHAR_RETRANS_DIRECT_RIGHT:
            cursor_to_right(client);
            break;
        default:
            client->retrans_mode=0;
            break;
    }
}

void cli_core::auth_proc(cli_client *client,unsigned char c)
{
    switch(c)
    {   
        case '\n':
            if(client->auth_mode_user)
            {
                if(strlen(client->buffer)==0)
                {
                    send_auth_to_client(client);
                }
                else
                {
                    strcpy(client->user_name,client->buffer);
                    clear_buffer(client);
                    client->auth_mode_user=0;
                    send_nl_cr_to_client(client);
                    cli_out(client,"%s",CLI_PASSWORD_PROMPT);
                    client->echo_enable=0;
                }
            }
            else
            {
                /* get password.*/
                if(this->user.check(client->user_name,client->buffer)==0)
                {
                    /* user ok.*/
                    client->auth_mode=0;
                    client->auth_mode_user=1;

                    client->echo_enable=1;
                    send_nl_cr_to_client(client);
                    send_prompt_to_client(client);
                }
                else
                {
                    client->echo_enable=1;
                    send_auth_to_client(client);
                    client->auth_mode_user=1;
                }
            }
            break;

        case CLI_CHAR_BACKSPACE:
            remove_char(client);
            break;
            
        default:
            add_char(client,c);
            break;
    }
}

void cli_core::process_chars(cli_client *client,const unsigned char *buffer,int len)
{
    int pos=0;
    
    while(len>0)
    {
        if(client->auth_mode)
        {
            /* authentication process.*/
            auth_proc(client,buffer[pos]);
        }        
        else if(client->retrans_mode)
        {
            retrans_char_proc(client,buffer[pos]);
        }
        else if(client->auto_complete)
        {
            if(CLI_CHAR_TAB==buffer[pos])
            {
                auto_complete(client);
            }
            else
            {
                common_char_proc(client,buffer[pos]);
            }

            client->auto_complete=0;
        }
        else
        {   
            common_char_proc(client,buffer[pos]);
        }

        pos++;
        len--;
    }
}
