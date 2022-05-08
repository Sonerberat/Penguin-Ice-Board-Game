#ifndef STRUCTINI_H_INCLUDED
#define STRUCTINI_H_INCLUDED

#include "univfuncs.h"

/*
This file initializes the data structures Player and Board as global. pengdata, intrninfo, and extinfo are all "stacked" and
contain multiple values stored as 1 integer. pengdata contains information relative to the penguins. intrninfo contains
information about the tile itself, such as the number of fish, the id of the penguin occupying it, the ai modifier value, and
the island number. extinfo contains information about the tiles surrounding a tile (ones that are in the tile's line of sight),
such as the number of moves that can be made in each direction, the id of any penguin  that is in the tile's line of sight, and
the number of tiles with 3 fish in each direction. The functions that manipulate these data structures are located in the
univfuncs.c file.
*/

struct Player
{
    char plyrname[MAXNAMELEN];

    int score;
    int pengsleft;
    int pengdata[MAXPENGS];
};

struct Board
{
    int intrninfo;
    int extinfo[MOVEDIRECTIONS];
};

#endif // STRUCTINI_H_INCLUDED
