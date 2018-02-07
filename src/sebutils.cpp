#include "sebutils.h"

namespace su
{
    std::vector<std::string> explode(std::string const & s, char delim)
    {
        std::vector<std::string> result;
        std::istringstream iss(s);

        for (std::string token; std::getline(iss, token, delim); )
        {
            result.push_back(std::move(token));
        }

        return result;
    }

    bool loadLevel(std::string filename, Level *l)
    {
        std::ifstream ifs(filename);
        std::string content( (std::istreambuf_iterator<char>(ifs) ), (std::istreambuf_iterator<char>() ) );
        auto k = explode(content, '\n');

        if(k.empty())
        {
            return false;
        }

        l->height = k.size();
        l->width = k.front().size();
        l->level = new std::string[k.size()];

        for(int i = 0; i < k.size(); i++)
        {
            l->level[i] = k.at(i);
        }

        substituteSpacesWithZeros(l);

        return (setPlayerPosition(l));
    }

    void substituteSpacesWithZeros(Level *l)
    {
        for(int i = 0; i < l->height; i++)
        {
            for(int j = 0; j < l->width; j++)
            {
                if(l->level[i].at(j) == ' ')
                {
                    l->level[i].at(j) = '0';
                }
            }
        }
    }

    bool setPlayerPosition(Level *l)
    {
        for(int i = 0; i < l->height; i++)
        {
            for(int j = 0; j < l->width; j++)
            {
                if(l->level[i].at(j) == 'P')
                {
                    l->player.pos.x = j;
                    l->player.pos.y = i;
                    l->level[i].at(j) = '0';
                    return true;
                }
            }
        }
        return false;
    }
}
