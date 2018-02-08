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
            std::cout << "K is empty." << std::endl;
            return false;
        }

        l->height = k.size();
        l->width = k.front().size();
        int maxdim = 0;

        for(int i = 0; i < k.size(); i++)
        {
            // trova stringa piu lunga
            if(k.at(i).size() > l->width)
            {
                l->width = k.at(i).size();
            }
        }

        if(l->height > l->width)
        {
            // matrice quadrata con lato lungo altezza
            maxdim = l->height;
        }
        else
        {
            // matrice quadrata con lato lungo larghezza
            maxdim = l->width;
        }
        //l->height = l->width = maxdim;
        l->level = new std::string[maxdim];

        // inizializzo tutta la mappa a 0
        for(int i = 0; i < maxdim; i++)
        {
            l->level[i] = std::string(maxdim, '0');
        }

        for(int i = 0; i < k.size(); i++)
        {
            for(int j = 0; j < k.at(i).size(); j++)
            {
                l->level[i].at(j) = k.at(i).at(j);
            }
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
