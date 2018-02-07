#ifndef SEBUTILS_H
#define SEBUTILS_H

#include <stdio.h>
#include <iostream>
#include <string>
#include <stdint.h>
#include <vector>
#include <sstream>
#include <utility>
#include <fstream>

namespace su
{
    // ------ DATA STRUCTURES ------ //
    struct RGBA
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };

    struct CoordUint32
    {
        uint32_t x;
        uint32_t y;
    };

    struct CoordShortInt
    {
        signed short int x;
        signed short int y;
    };

    struct CoordDouble
    {
        double x;
        double y;
    };

    struct CoordInt
    {
        int x;
        int y;
    };

    struct Player
    {
        CoordUint32 pos;
    };

    struct Level
    {
        uint32_t width;
        uint32_t height;
        std::string *level;
        Player player;
    };

    // ------ UTILITY FUNCTIONS ------ //

    // explode() function stolen from PHP :)
    // Usage: auto v = explode(string, token)
    std::vector<std::string> explode(std::string const & s, char delim);

    // load level from file with name filename
    bool loadLevel(std::string filename, Level *l);

    // set player position in level l
    bool setPlayerPosition(Level *l);

    // substitute spaces in the 2D map (if any) with zeros
    void substituteSpacesWithZeros(Level *l);
}

#endif // SEBUTILS_H
