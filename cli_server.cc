#include "cli_server.h"
#include "cli_core.h"
#include "cli_client.h"
#include "cli.h"

cli_server::~cli_server()
{
    while(!client_list.empty())
    {
        client_list.front()->exit();
        delete (client_list.front());
        client_list.pop_front();
    }    
}

int cli_server::add_client(int fd,cli_front_end *front_end)
{
    cli_client *client=NULL;

    client=new cli_client(this->loop,this->core,fd,front_end);

    if(client->init()<0)
    {
        delete client;
        return -1;
    }
    
    this->client_list.push_back(client);
    
    client->set_cmd_mode(this->core->get_cmd_root());

    if(this->core->need_auth())
    {
        this->core->send_auth_to_client(client);
    }
    else if(this->core->banner_on())
    {
        this->core->send_banner_to_client(client);
    }
    else
    {
        this->core->send_prompt_to_client(client);
    }
    
    return 0;
}

void cli_server::del_client(int fd)
{
    std::list<cli_client *>::iterator it;

    for(it=client_list.begin();
        it!=client_list.end();
        it++)
    {
        if((*it)->get_fd()==fd)
        {
            break;
        }
    }

    if(it!=client_list.end())
    {
        (*it)->exit();
        delete *it;
        client_list.erase(it);
    }
}

void cli_server::show_clients(cli_client * c)
{
    std::list<cli_client*>::iterator it;
    const char *prefix=NULL;

    cli_out(c," tty\tuser\tlogin time\r\n");
    
    for(it=client_list.begin();
        it!=client_list.end();
        it++)
    {
        if(*it==c)
        {
            prefix="*";
        }
        else
        {
            prefix=" ";
        }
        
        cli_out(c,"%s%s\t%s\t%s\r\n",
                prefix,(*it)->tty_name.c_str(),
                (*it)->user_name,
                (*it)->login_time.c_str());
    }
}