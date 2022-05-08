#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

#include "univfuncs.h"
#include "structini.h"
#include "aiturn.h"
#include "sharedalgs.h"
#include "aialgs.h"


int aiplay(struct Board _boarddata[], struct Player _playerdata[], int _boarddim[], int _phase, int _freetilesleft, int _ppinfo)
{
    /*
    aiplay is used when the game is played in autonomous mode. It uses a series of algorithms combined with the information
    gathered and stored in the playerdata and boarddata array structures to determine which penguin will be moved/placed and where.
    If no moves can be made by any penguin, it automatically returns 1.

    If it is the placement phase, the ai modifier value (a number that represents the strategic value of a tile) is calculated
    for each tile on the board. The unoccupied tile (that has 1 fish on it) with the highest ai modifier value is the one that
    getbestpengplace chooses to place the next penguin.

    If it is the movement phase, the freedom level is first calculated. Using the number of free tiles currently left on the
    board and the number of free tiles that the board originally started with, getmovefreedomrating() calculates and returns an
    integer, freedomlevel, that represents the relative freedom (of motion) that each penguin has. As the game progresses and the
    number of accessible tiles drops, the average number of moves each penguin can make on any given turn decreases. aiplay keeps
    track of the state of the board with getmovefreedomrating() and changes the method used for determining the best move as the
    number of accessible tiles decreases. freedomlevel starts at 0 and increases every time the fraction of tiles remaining
    crosses a predetermined threshhold up to a maximum value of 4.

    If freedomlevel > 0, findislands() attempts to find large regions of connected tiles (islands) that only the current player's
    penguins can access. The function searches for islands that are in the immediate vicinity of the current player's penguins,
    as well as for those that can be reached after making one or two moves (assuming that the 2nd move does not have a chance of
    being blocked by an opposing penguin). If found, the maximum point value of the island is calculated, as well as the move in
    the direction of the island that results in the maximum number of points gained. Once an island is secured by a penguin, that
    penguin does not move until all other penguins are blocked, all other penguins have also secured an island, or a bigger island
    is found. If no islands can be found or they are too small, the default method is used to determine which penguin moves and
    where.

    If freedomlevel != 4, the default ai mode determines the best move by comparing the ai modifier values of the highest valued
    tiles each penguin can reach with the ai modifier values of the tiles occupied by those penguins. The penguin/move combo with
    the highest combined ai modifier value is the one chosen to be moved. If freedomlevel == 4, a maze solving algorithm is used
    instead to find the most efficient path through the remaining tiles.

    The final move chosen is primarily dependent on the ai modes of the penguins. Penguins attempting to secure an island have the
    highest priority, and penguins that have already secured an island have the lowest priority. If two or more penguins have
    the same priority and no other penguins have a higher priority, the penguin with the most points moves first (aimodval if
    aimode is DEFAULT, otherwise islepoints).
    */


    int _playstatus, currentid, pengsperplyr;
    int islenumtested, numofguesses;
    int freedomlevel;
    int bestmovepengnum;
    int _depttile, _desttile;

    currentid = getcurrentid(_ppinfo);
    pengsperplyr = getpengsperplyr(_ppinfo);

    if ((_phase == PLACEMENT && (countplyrpenguins(_playerdata, currentid, _ppinfo)) == pengsperplyr) || (_phase == MOVEMENT && _playerdata[currentid].pengsleft == 0))
    {
        printf("%s cannot take any actions this turn.\n\n\n", _playerdata[currentid].plyrname);

        _playstatus = 1;

        return _playstatus;
    }

    freedomlevel = getmovefreedomrating(_freetilesleft, _ppinfo);

    if (_phase == PLACEMENT)
    {
        _desttile = getbestpengplace(_boarddata, _boarddim, freedomlevel, _ppinfo);

        if (_desttile == -1)
        {
            printf("Error: The AI was unable to find a tile to place a penguin on.\n");

            _playstatus = 3;

            return _playstatus;
        }

        _playstatus = placepenguin(_boarddata, _playerdata, _desttile, _ppinfo);

        printf("\n%s placed a penguin.\n\n\n", _playerdata[currentid].plyrname);

        return _playstatus;
    }

    islenumtested = 0;
    numofguesses = 0;

    /*Islands are unlikely to be found early in the game (when most tiles are still free). To save time and space, the program
    only searches for islands when at least 15% of the board is gone.*/
    if (freedomlevel > 0)
    {
       findislands(_boarddata, _playerdata, _boarddim, islenumtested, numofguesses, freedomlevel, _ppinfo);
    }

    /*Finds the AI modifier values for each penguin's tile, as well as the tiles that they can move to.
    Also finds the best move for each penguin that still has ai mode  of default using ai modifier values.*/
    getbestpengmovesaimods(_boarddata, _playerdata, _boarddim, freedomlevel, _ppinfo);

    /*Finds the best penguin to move.*/
    bestmovepengnum = getfinalpengmove(_boarddata, _playerdata, _boarddim, _ppinfo);

    if (bestmovepengnum == -1)
    {
        printf("Error: The AI had at least one free penguin, but was unable to make a move.\n");

        _playstatus = 3;

        return _playstatus;
    }

    _depttile = getpengtile(_playerdata[currentid].pengdata[bestmovepengnum]);
    _desttile = getpengbestmove(_playerdata[currentid].pengdata[bestmovepengnum]);

    _playstatus = movepenguin(_boarddata, _playerdata, _depttile, _desttile, _ppinfo);

    printf("\n%s moved a penguin.\n\n\n", _playerdata[currentid].plyrname);

    return _playstatus;
}





