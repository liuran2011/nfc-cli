#include <map>
#include <algorithm>
#include <string>

#include <string.h>
#include <assert.h>

#include "cli_cmd.h"
#include "pearl2.h"
#include "cli_cmd_builtin.h"
#include "cli.h"
#include "cli_error.h"
#include "cli_defs.h"

int cli_cmd_node::add_default_cmd()
{
    add_cmd(&cli_cmd_exit_cmd);
}

void cli_cmd_node::init_data()
{
    this->func=NULL;
    this->parent=NULL;
    this->prev_mode=NULL;
    this->mode=0;
    this->is_param=0;
    this->optional=0;
    this->type=CLI_CMD_PARAM_TYPE_INVALID;
    this->min_value=0;
    this->max_value=0;
}

cli_cmd_node::cli_cmd_node(int mode)
{
    init_data();
    
    this->mode=mode;
}

int cli_cmd_node::parse(std::string & token)
{
    char c;
    
    c=token[0];

    /* check if token is a param.*/
    if((c>='a'&&c<='z')
       ||(c>='A'&&c<='Z')
       ||(c>='0'&&c<='9'))
    {
        return 0;
    }

    if(c!=CLI_CMD_REQUIRE_CHAR
       &&c!=CLI_CMD_OPTION_CHAR)
    {
        return 0;
    }
    
    this->is_param=1;
    
    switch(c)
    {
        case CLI_CMD_REQUIRE_CHAR:
            this->optional=0;
            break;
        case CLI_CMD_OPTION_CHAR: 
            this->optional=1;
            break;
        default:  
            this->optional=0;
            break;
    }

    /* parse type.*/
    if(token.find(CLI_CMD_TYPE_STRING)!=-1)
    {
        this->type=CLI_CMD_PARAM_TYPE_STRING;
    }
    else if(token.find(CLI_CMD_TYPE_DWORD)!=-1)
    {
        this->type=CLI_CMD_PARAM_TYPE_NUM;
    }
    else if(token.find(CLI_CMD_TYPE_IPV4)!=-1)
    {
        this->type=CLI_CMD_PARAM_TYPE_IPV4;
    }    
    else if(token.find('-')!=-1)
    {
        this->type=CLI_CMD_PARAM_TYPE_RANGE;

        int pos=token.find('-');
        std::string min=token.substr(1,pos);
        std::string max=token.substr(pos+1);
        
        this->min_value=atoi(min.c_str());
        this->max_value=atoi(max.c_str());

        if(this->min_value<this->max_value)
        {
            return -1;
        }
    }
    else
    {
        this->type=CLI_CMD_PARAM_TYPE_INVALID;
        return -1;
    }
    
    return 0;
}

cli_cmd_node::cli_cmd_node(std::string name,std::string doc,cli_cmd_exec_func func,int mode)
{
    init_data();
    
    this->name=name;
    this->doc=doc;
    this->func=func;
    this->mode=mode;  
}

void cli_cmd_node::del_tree(cli_cmd_node *node)
{
    std::map<std::string,cli_cmd_node *>::iterator it;

    for(it=node->list.begin();
        it!=node->list.end();
        it++)
    {
        del_tree(it->second);        
    }

    delete node;
}

void cli_cmd_node::strtoken(const char *s,const char *sep,std::vector<std::string> &vec)
{
    char *s_tmp=NULL;
    char *substr=NULL;
    char *saveptr=NULL;
    
    s_tmp=strdup(s);
    if(NULL==s_tmp)
    {
        log_add(LOG_ERROR,"strtoken dump string %s failed.",s);
        return;
    }
    
    substr=strtok_r(s_tmp,sep,&saveptr);

    if(substr!=NULL)
    {
        vec.push_back(substr);
    }

    while((substr=strtok_r(NULL,sep,&saveptr))!=NULL)
    {
        vec.push_back(substr);
    }

    free(s_tmp);
}

cli_cmd_node::~cli_cmd_node()
{
    std::map<std::string,cli_cmd_node *>::iterator it;

    for(it=this->list.begin();
        it!=this->list.end();
        it++)
    {
        del_tree(it->second);        
    }
}

void cli_cmd_node::add_cmd(struct cli_cmd_t * cmd)
{
    cli_cmd_node *parent=NULL;
    cli_cmd_node *new_node=NULL;
    std::map<std::string,cli_cmd_node *>::iterator node_it;
    std::vector<std::string> name_vec;
    std::vector<std::string> doc_vec;
    int name_pos;

    if(0==this->mode)
    {
        log_add(LOG_ERROR,"node %s node mode.",this->name.c_str());
        return;
    }
    
    strtoken(cmd->name," ",name_vec);

    /* TODO. command sanity check.*/
    strtoken(cmd->doc,"\n",doc_vec);

    if(name_vec.size()==0)
    {
        log_add(LOG_ERROR,"cli cmd add failed.strdup name or doc failed.");
        return;
    }

    parent=this;

    for(name_pos=0;name_pos<name_vec.size();name_pos++)
    {
        node_it=parent->list.find(name_vec[name_pos]);
        if(node_it==parent->list.end())
        {
            new_node=new cli_cmd_node(name_vec[name_pos],
                                      doc_vec.size()>name_pos?doc_vec[name_pos]:NULL,
                                      cmd->func,0);
            if(NULL==new_node)
            {
                log_add(LOG_ERROR,"add cli node %s failed. no memory.",
                        name_vec[name_pos].c_str());
                break;
            }

            if(new_node->parse(name_vec[name_pos])<0)
            {
                log_add(LOG_ERROR,"parse %s failed.",name_vec[name_pos].c_str());
                delete new_node;
                break;
            }
            
            parent->list[name_vec[name_pos]]=new_node;
            parent=new_node;
            new_node->parent=parent;
        }
        else
        {
            parent=node_it->second;
        }            
    } 
}

cli_cmd_node *cli_cmd_node::add_mode(struct cli_mode_t *mode)
{
    std::vector<std::string> tok_vec;
    std::vector<std::string> doc_vec;
    std::map<std::string,cli_cmd_node *>::iterator node_it;
    cli_cmd_node *node=NULL;
    cli_cmd_node *new_node=NULL;
    cli_cmd_node *mode_node=NULL;
    int name_pos=0;
    
    if(0==this->mode)
    {
        log_add(LOG_ERROR,"node %s node mode node.",this->name.c_str());
        return mode_node;
    }
    
    strtoken(mode->name," ",tok_vec);
    strtoken(mode->doc,"\n",doc_vec);
    if(tok_vec.size()==0)
    {
        log_add(LOG_ERROR,"add mode failed.");
        return mode_node;
    }

    node=this;

    for(name_pos=0;name_pos<tok_vec.size();name_pos++)
    {
        node_it=node->list.find(tok_vec[name_pos]);
        if(node_it==node->list.end())
        {
            int mode_flag=0;

            mode_flag=(name_pos==tok_vec.size()-1)?1:0;
            new_node=new cli_cmd_node(tok_vec[name_pos],
                             doc_vec.size()>name_pos?doc_vec[name_pos]:NULL,
                                      mode->func,
                                      mode_flag);
            if(NULL==new_node)
            {
                log_add(LOG_ERROR,"add mode %s failed.new cli_cmd_node failed.",
                        mode->name);
                break;
            }

            if(parse(tok_vec[name_pos])<0)
            {
                log_add(LOG_ERROR,"add mode, parse %s failed.",tok_vec[name_pos].c_str());
                delete new_node;
                break;
            }
            
            node->list[tok_vec[name_pos]]=new_node;
            new_node->parent=node;
            if(mode_flag)
            {
                new_node->prev_mode=this;
                new_node->prefix=mode->prefix;
                new_node->add_default_cmd();
                mode_node=new_node;
                break;
            }
        }
        else
        {
            node=node_it->second;
        }
    }   

    return mode_node;
}

void cli_cmd_node::show_tip(cli_client *client)
{
    cli_cmd_node *node=NULL;
    std::map<std::string,cli_cmd_node *>::iterator it;
    int last=0;
    char *save_ptr=NULL;
    char *token=NULL;
    int token_len=0;
    char *line_buffer=NULL;

    line_buffer=strdup((char *)client->buffer);
    if(NULL==line_buffer)
    {
        log_add(LOG_ERROR,"show tip.  strdup line buffer failed.");
        return;
    }

    node=this;
    token=strtok_r((char *)line_buffer," ",&save_ptr);
   
    while(1)
    {       
        if(NULL==token)
        {
            for(it=node->list.begin();
                it!=node->list.end();
                it++)
            {
                cli_out(client,
                       "  %s\t%s\r\n",
                        it->second->name.c_str(),
                        it->second->doc.c_str());
            }
            
            goto end;
        }
    
        token_len=strlen(token);
        /* check if token is the last token.*/
        if(token+token_len-line_buffer
           ==client->pos)
        {
            last=1;
        }

        if(1==last)
        {
             for(it=node->list.begin();
                it!=node->list.end();
                it++)
             {
                if(it->second->name.compare(0,token_len,token)==0)
                {
                    cli_out(client,
                           "  %s\t%s\r\n",
                            it->second->name.c_str(),
                            it->second->doc.c_str());
                }
             }

             goto end;
        }
        else
        {
            for(it=node->list.begin();
                it!=node->list.end();
                it++)
            {
                if(it->second->name.compare(0,strlen(token),token)==0)
                {
                    break;
                }
            }
            
            if(it==node->list.end())
            {
                /* nothing find. */
                goto end;
            }        

            node=it->second;        
        }

         token=strtok_r(NULL," ",&save_ptr);
    }

end:
    free(line_buffer);
}

void cli_cmd_node::find_node(cli_cmd_node *node,
                               const char *s,
                               std::vector<cli_cmd_node *> &vec)
{
    std::map<std::string,cli_cmd_node *>::iterator it;

    for(it=node->list.begin();it!=node->list.end();it++)
    {
        if(it->second->is_param)
        {
            vec.push_back(it->second);
        }
        else if(it->second->name.compare(0,strlen(s),s)==0)
        {
            vec.push_back(it->second);
        }
    }
}

int cli_cmd_node::check_param(cli_cmd_node *node,std::string &param)
{
    int type_match=0;
    int i;
    int value;
    
    switch(node->type)
    {   
        case CLI_CMD_PARAM_TYPE_IPV4:
            type_match=1;
            break;
        case CLI_CMD_PARAM_TYPE_STRING:
            type_match=1;
            break;
        case CLI_CMD_PARAM_TYPE_NUM:            
            for(i=0;i<param.length();i++)
            {
                if(param[i]<'0'||param[i]>'9')
                {
                    break;
                }
            }

            if(i==param.length())
            {
                type_match=1;
            }
            break;
        case CLI_CMD_PARAM_TYPE_RANGE:
            value=atoi(param.c_str());
            if(value>=node->min_value&&value<=node->max_value)
            {
                type_match=1;
            }
            break;
        default:
            break;
    }

    if(0==type_match&&node->optional)
    {
        type_match=1;
    }
    
    return type_match;
}

int cli_cmd_node::exec_cmd(cli_client *client)
{
    std::vector<std::string> vec;
    std::vector<std::string>::iterator it;
    cli_cmd_node *node=NULL;
    int argc=0;
    const char *argv[CLI_MAX_ARG_CNT];
    int ret=-1;
    
    strtoken((char *)client->buffer," ",vec);
    if(vec.size()==0)
    {
        client->set_error(CLI_ERROR_NO_MEMORY);
        return ret;
    }

    node=(cli_cmd_node *)client->cmd_mode;

    for(it=vec.begin();it!=vec.end();it++)
    {
        std::vector<cli_cmd_node *> node_vec;

        find_node(node,(*it).c_str(),node_vec);

        if(node_vec.size()==0)
        {
            client->set_error(CLI_ERROR_CMD_NOT_FOUND);
            return ret;
        }
        else if(node_vec.size()>1)
        {
            /* multiple commands.*/
            client->set_error(CLI_ERROR_AMBIGUOUS_CMD);
            return ret;
        }
        else
        {
            if(node_vec[0]->is_param)
            {
                if(check_param(node_vec[0],*it)==0)
                {
                    client->set_error(CLI_ERROR_INVALID_PARAM);
                    return ret;
                }

                argv[argc]=(*it).c_str();
                argc++;            
            }
            
            node=node_vec[0];
        }  

        node_vec.clear();
    }

    if(!node->mode&&node->list.size()!=0)
    {
        client->set_error(CLI_ERROR_CMD_INCOMPLETE);
        return ret;
    }

    client->set_cmd_current_node(node);
    node->func(client,argc,argv);

    ret=0;
    
    return ret;
}

void cli_cmd_node::candidate(cli_client *client,
                        std::vector < std :: string > & vec)
{
    std::vector<std::string> token_vec;
    std::vector<cli_cmd_node *> node_vec;
    int pos=0;
    cli_cmd_node *node=NULL;
    
    strtoken((const char *)client->buffer," ",token_vec);
    if(token_vec.size()==0)
    {
        return;
    }

    node=(cli_cmd_node*)client->cmd_mode;

    for(pos=0;pos<token_vec.size()-1;pos++)
    {
        find_node(node,token_vec[pos].c_str(),node_vec);

        if(node_vec.size()>1)
        {
            /* ambiguous commd.*/
            return;
        }

        node=node_vec[0];
        
        node_vec.clear();
    }

    /* last token.*/
    find_node(node,token_vec[pos].c_str(),node_vec);
    for(pos=0;pos<node_vec.size();pos++)
    {
        vec.push_back(node_vec[pos]->name);
    }
}

const char *cli_cmd_node::get_mode_prefix()
{
    return (this->mode==0)?NULL:this->prefix.c_str();
}