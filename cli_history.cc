#include <list>

#include "cli_history.h"
#include "pearl2.h"
#include "cli.h"

cli_history::cli_history()
{
    this->pos=0;
    this->capacity=CLI_HISTORY_DEFAULT_CAPACITY;
}

int cli_history::add_cmd(char * cmd)
{
    if(this->history.size()>=this->capacity)
    {
        this->history.pop_front();
    }

    this->history.push_back(cmd);
    this->pos=this->history.size();
    
    return 0;
}

const char * cli_history::prev_cmd()
{
    const char *ret=NULL;
    int i=0;
    std::list<std::string>::reverse_iterator it;
    int delta=0;
    
    if(this->history.empty())
    {
        return NULL;
    }

    delta=this->history.size()-this->pos;

    for(it=this->history.rbegin();
        it!=this->history.rend()&&delta>0;
        it++,delta--);
    
    if(it!=this->history.rend())
    {
        this->pos--;
        return (*it).c_str();
    }
    
    return ret;
}

const char *cli_history::next_cmd()
{
    const char *ret=NULL;
    std::list<std::string>::iterator it;
    int delta=0;
    
    if(this->history.empty())
    {
        return NULL;
    }

    delta=this->pos;
    
    for(it=this->history.begin();
        it!=this->history.end()&&delta>0;
        it++,delta--);

    if(it!=this->history.end())
    {
        this->pos++;

        return (*it).c_str();
    }

    return ret;
}