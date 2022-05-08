#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

#include "univfuncs.h"
#include "structini.h"
#include "sharedalgs.h"


int updateppinfo(int _tempboard[], int _totnumtiles, int _pengsperplyr, int _ppinfo)
{
    /*
    This function calculates ppinfo each turn during the placement phase. Using the values in _tempboarddata[], it first finds
    the number of players. If ppinfo is 0 (during the first turn), it increments the number of players by 1, sets currentid
    equal to the new number of players, and obtains _startfreetiles. The value of ppinfo is then calculated and returned.
    */

    int tilenum, currentid, plyrid, _numofplyrs, _startfreetiles; //plyrid has values from 0 to 8

    _numofplyrs = 0;

    tilenum = 0;

    while (tilenum < _totnumtiles)
    {
        plyrid = (_tempboard[tilenum] % 10) - ARRAYSHIFT; //formula to get plyrid from the values saved to each tile

        if ((plyrid + ARRAYSHIFT) > _numofplyrs) //checks to see if plyrid+1 is greater than _numofplyrs
        {
            _numofplyrs = plyrid + ARRAYSHIFT; //gets the number of players after loop is finished (unless it is the 1st turn)
        }

        tilenum++;
    }

    if (_ppinfo == 0)
    {
        currentid = _numofplyrs; //currentid has values from 0 to 8, _numofplyrs has values from 1 to 9

        _startfreetiles = currentid + (getcurrentfreetiles(_tempboard, _totnumtiles));

        _numofplyrs++;
    }
    else
    {
        currentid = getcurrentid(_ppinfo);

        _startfreetiles = getstartingfreetiles(_ppinfo);
    }

    _ppinfo = getppinfosum(currentid, _numofplyrs, _pengsperplyr, _startfreetiles); //calculates ppinfo


    return _ppinfo;
}

int getcurrentfreetiles(int _tempboard[], int _totnumtiles)
{
    /*
    Calculates and returns the current number of free tiles remaining on the board.
    */

    int tilecount, freetilecounter;
    int fish;

    freetilecounter = 0;

    tilecount = 0;

    while (tilecount < _totnumtiles)
    {
        fish = _tempboard[tilecount] / 10;

        if (fish > 0)
        {
            freetilecounter++;
        }

        tilecount++;
    }

    return freetilecounter;
}

int isinitialturns(int _tempboard[], int _totnumtiles)
{
    /*This function checks if each player has placed no more than one penguin.*/

    int tilecounter, plyrid;
    int pengsonboard;

    int pengcounter[MAXNUMPLYRS];


    plyrid = 0;

    while (plyrid < MAXNUMPLYRS)
    {
        pengcounter[plyrid] = 0;

        plyrid++;
    }

    pengsonboard = 0;

    tilecounter = 0;

    while (tilecounter < _totnumtiles)
    {
        plyrid = (_tempboard[tilecounter] % 10) - ARRAYSHIFT;

        if (plyrid >= 0)
        {
            pengcounter[plyrid] = pengcounter[plyrid] + 1;
            pengsonboard++;

            if (pengcounter[plyrid] > 1 || pengsonboard == (MAXNUMPLYRS))
            {
                return UNFLAG;
            }
        }

        tilecounter++;
    }

    return FLAG;
}

int temptoboarddata(int _tempboard[], struct Board _boarddata[], struct Player _playerdata[], int _totnumtiles, int _phase, int _ppinfo)
{
    /*
    This function extracts from _tempboard[] the number of fish and the player id on each tile. Simultaneously, it assigns each
    penguin an internal penguin id (ipengid) based on the penguin's owner's id (plyrid), the number of penguins per player, and
    the number of the player's penguins that have already been assigned an id. These values are used to populate the structure
    arrays _boarddata[].intrninfo and _playerdata[].pengdata[]. This function also sets the values of _playerdata[].pengdata[]
    equal to 0 for any penguins that have not been placed on the board yet.
    */

    int plyrid, numofplyrs, pengsperplyr, plyrnum, tilecounter;

    numofplyrs = getnumofplyrs(_ppinfo);
    pengsperplyr = getpengsperplyr(_ppinfo);

    int pengidcounter[numofplyrs]; //counts the number of penguins that have been assigned an id for each player

    int fish, ipengid; //values that will be obtained
    int adjmoves = 0, islandnum = 0, aival = STARTAIMODVAL; //values not obtained in this function

    int bestmove, aimode = DEFAULT, maxislepoints = 0; //values not obtained in this function


    plyrnum = 0;

    while (plyrnum < numofplyrs)
    {
        pengidcounter[plyrnum] = 0; //set the value of the array at each index to 0

        plyrnum++;
    }

    tilecounter = 0;

    while (tilecounter < _totnumtiles)
    {
        fish = _tempboard[tilecounter] / 10;
        plyrid = (_tempboard[tilecounter] % 10) - ARRAYSHIFT;

        if (plyrid >= 0) //if a penguin occupies the tile tilecounter, assign it an id and populate _playerdata[].pengdata[]
        {
            bestmove = tilecounter;

            if (plyrid >= numofplyrs)
            {
                printf("Error: Player found on board (Player %d) does not exist.\n", (plyrid + ARRAYSHIFT));

                return 1;
            }
            else if (pengidcounter[plyrid] == pengsperplyr || (pengidcounter[plyrid] == _playerdata[plyrid].score && _phase == PLACEMENT))
            {
                printf("Error: Player %d has too many penguins on the board.\n", (plyrid + ARRAYSHIFT));

                return 1;
            }

            ipengid = (plyrid * pengsperplyr) + pengidcounter[plyrid];
            _playerdata[plyrid].pengdata[pengidcounter[plyrid]] = getpengdatasum(tilecounter, bestmove, aimode, maxislepoints);

            pengidcounter[plyrid]++;
        }
        else
        {
            ipengid = -1;
        }

        if (fish > 0)
        {
            islandnum = 0;
        }
        else
        {
            islandnum = NULLISLENUM;
        }

        /*Populates _boarddata[tilecounter].intrninfo with fish and ipengid.*/
        _boarddata[tilecounter].intrninfo = getintrnboarddatasum(fish, adjmoves, ipengid, islandnum, aival);

        tilecounter++;
    }


    plyrid = 0;

    while (plyrid < numofplyrs)
    {
        if ((pengidcounter[plyrid] < pengsperplyr && plyrid == (getcurrentid(_ppinfo)) && _phase == MOVEMENT) || (pengidcounter[plyrid] < _playerdata[plyrid].score && _phase == PLACEMENT))
        {
            printf("Error: Player %d has too few penguins on the board.\n", (plyrid + ARRAYSHIFT));

            return 1;
        }

        while (pengidcounter[plyrid] < pengsperplyr)
        {
            /*Populates _playerdata[].pengdata[] for any penguins that have not been placed on the board yet with 0.*/
            _playerdata[plyrid].pengdata[pengidcounter[plyrid]] = 0;

            pengidcounter[plyrid]++;
        }

        plyrid++;
    }


    return 0;
}

void scanboard(struct Board _boarddata[], int _boarddim[])
{
    /*
    The scanboard function moves across the board, tile by tile, and collects information about each tile's surroundings.
    It works like a radar, checking subsequent tiles (starting from the adjacent tile) in each direction (one direction at a time)
    until it reaches the edge of the board, a tile where the ice has broken through, or another penguin. At that point, the
    function starts checking subsequent tiles (starting from the adjacent tile) in another direction. After all 4 directions
    surrounding the tile have been searched, scanboard repeats the search on the next tile until all tiles' surroundings have
    been searched.

    tilecount is the tile whose surroundings are being checked.
    temptile is the external tile being checked. It increments by one tile in a direction (starting from tilecount) until it
    reaches a tile it cannot cross. It then returns to tilecount and moves outwards again, 1 tile at a time, until it has tested
    tiles in all 4 directions.
    edgetest takes the value of the row of temptile, if temptile is moving up or down, and the column of temptile, if temptile
    is moving left or right. It ensures that only the tiles inside the board are searched.

    _moves is the number of moves that can be made in each direction.
    _hifishmoves is the number of moves to 3 fish tiles that can be made in each direction.
    _epengid (the external penguin id) is the id of the penguin that is in the line of sight of tilecount in some direction. If no
    penguin is in the tilecount's line of sight in some direction, _epengid is 0. If _epengid>0 and there are fish on tilecount
    (tilecount is not occupied), the penguin with id _epengid has the ability to move to that tile.

    _fish is the number of fish on a tile.
    _ipengid (internal penguin id) is the id of the penguin occupying tilecount (0 if tilecount is unoccupied).
    _islandnum (island number) is a value used by the ai to identify groups of connected tiles.
    _aival (AI modifier value) is a value used by the ai to determine which move should be made in the DEFAULT AI mode.

    _adjmoves is a number (0 to 15) which contains the directions of all adjacent tiles that are unoccupied and have fish on them.
    If _adjmoves is 0 for a tile, it has no free adjacent tiles. For _adjmoves, right = 1, down = 2, left = 4, and up = 8. So, if
    _adjmoves = 11 for a tile, the up, down, and right adjacent tiles are free (8 + 2 + 1 = 11), while the left adjacent tile cannot
    be moved to.
    dircount (direction count) gives the direction that temptile is moving in, right = 0, down = 1, left = 2, and up = 3.
    dirid (direction id) is used to change _adjmoves. dirid = 2^(dircount).
    */

    int _fish, _adjmoves, _ipengid, _islandnum, _aival;
    int _moves, _hifishmoves, _epengid, _addressislenum;

    int tilecount, _totnumtiles, dircount, dirid, temptile, edgetest;

    tilecount = 0;
    _totnumtiles = _boarddim[0] * _boarddim[1];

    while (tilecount < _totnumtiles)
    {
        _fish = getnumoffish(_boarddata[tilecount].intrninfo);
        _ipengid = getintrnpengid(_boarddata[tilecount].intrninfo);
        _islandnum = getislandnum(_boarddata[tilecount].intrninfo);
        _aival = getaimodval(_boarddata[tilecount].intrninfo);


        dircount = 0;
        dirid = 1;
        _adjmoves = 0;

        while (dircount < MOVEDIRECTIONS) //checking each direction
        {
            temptile = tilecount;
            _moves = 0;
            _hifishmoves = 0;
            _epengid = -1;
            _addressislenum = -1;

            do //checking each subsequent tile in a direction
            {
                if ((getnumoffish(_boarddata[tilecount].intrninfo)) == 0 && (getintrnpengid(_boarddata[tilecount].intrninfo)) == -1)
                {
                    //surrounding tiles are only searched if tilecount has fish on it or it is occupied by a penguin
                    break;
                }
                else if ((dircount % 2) == 0) //temptile moving left/right
                {
                    edgetest = getcolnum(_boarddim, temptile);
                    temptile = temptile - (dircount - 1);

                    if ((edgetest == 0 && temptile < tilecount) || (edgetest == (_boarddim[1] - ARRAYSHIFT) && temptile > tilecount))
                    {
                        //over the edge of the board
                        break;
                    }
                }
                else //temptile moving up/down
                {
                    edgetest = getrownum(_boarddim, temptile);
                    temptile = temptile - (_boarddim[1] * (dircount - 2));

                    if ((edgetest == 0 && temptile < tilecount) || (edgetest == (_boarddim[0] - ARRAYSHIFT) && temptile > tilecount))
                    {
                        //over the edge of the board
                        break;
                    }
                }

                //_move, _hifishmoves, _epengid only changed if the original tile (tilecount) either has fish or is occupied by
                //a penguin
                if ((getnumoffish(_boarddata[temptile].intrninfo)) > 0)
                {
                    _moves++;

                    if ((getnumoffish(_boarddata[temptile].intrninfo)) == MAXFISH)
                    {
                        _hifishmoves++;
                    }
                }

                _epengid = getintrnpengid(_boarddata[temptile].intrninfo);

            }
            while ((getnumoffish(_boarddata[temptile].intrninfo)) > 0 && _epengid == -1);



            if (_fish > 0 || _ipengid >= 0)
            {
                _boarddata[tilecount].extinfo[dircount] = getextboarddatasum(_moves, _hifishmoves, _epengid, _addressislenum); //updating _boarddata[].extinfo[]
            }
            else
            {
                _boarddata[tilecount].extinfo[dircount] = getextboarddatasum(0, 0, -1, -1); //updating _boarddata[].extinfo[] for tile that has broken through
            }


            if (_moves > 0) //if at least one move exists in a direction, _adjmoves increases by an amount corresponding to that direction
            {
                _adjmoves = _adjmoves + dirid;
            }

            dirid = dirid * 2;

            dircount++;
        }

        //updating _boarddata[].intrninfo to include _adjmoves
        _boarddata[tilecount].intrninfo = getintrnboarddatasum(_fish, _adjmoves, _ipengid, _islandnum, _aival);


        tilecount++;
    }
}

void findactivepengs(struct Board _boarddata[], struct Player _playerdata[], int _ppinfo)
{
    /*
    Finds how many penguins that are on the board and not trapped each player has left by checking the number representing the
    possible adjacent moves on each tile occupied by a penguin. getadjmoves(_boarddata[pengtile].intrninfo) returns this number.
    If it is 0, there are no free tiles adjacent to the penguin's tile, and it is trapped. If _playerdata[plyrnum].pengdata[pengnum]
    is equal to 0, that penguin has not been placed on the board yet.
    */

    int plyrnum, pengnum, pengtile, actvpengcounter;

    plyrnum = 0;

    while (plyrnum < (getnumofplyrs(_ppinfo)))
    {
        pengnum = 0;
        actvpengcounter = 0;

        while (pengnum < (getpengsperplyr(_ppinfo)))
        {
            if (_playerdata[plyrnum].pengdata[pengnum] != 0)
            {
                pengtile = getpengtile(_playerdata[plyrnum].pengdata[pengnum]); //find the tile of a penguin

                if ((getadjmoves(_boarddata[pengtile].intrninfo)) > 0)
                {
                    actvpengcounter++;
                }
                else
                {
                    _playerdata[plyrnum].pengdata[pengnum] = _playerdata[plyrnum].pengdata[pengnum] * NEGONE;
                    //If a penguin is found to be trapped, _playerdata[plyrnum].pengdata[pengnum] becomes negative.
                    //This will allow for quick identification of remaining penguins without needing to recall this function.
                }
            }

            pengnum++;
        }

        _playerdata[plyrnum].pengsleft = actvpengcounter; //store the number of remaining penguins each player has

        plyrnum++;
    }
}

int countplyrpenguins(struct Player _playerdata[], int _plyrid, int _ppinfo)
{
    /*
    This function counts the number of penguins that have been placed (even if they are trapped). It is used in place of
    findactivepengs during the placement phase in case one or more penguins gets trapped before the movement phase begins.
    A penguin has been placed if _playerdata[_plyridlessone].pengdata[pengnum] does not equal 0.
    */

    int pengnum, pengcounter;

    pengnum = 0;
    pengcounter = 0;

    while (pengnum < (getpengsperplyr(_ppinfo)))
    {
        if (_playerdata[_plyrid].pengdata[pengnum] != 0)
        {
            pengcounter++;
        }

        pengnum++;
    }

    return pengcounter;
}

int checkplcmttile(struct Board _boarddata[], struct Player _playerdata[], int _desttile, int _ppinfo)
{
    /*
    This function checks to make sure that the player chooses a correct tile when placing their penguins. If the destination
    internal penguin id (destipengid) is not 0 or the destination fish (destfish) is not 1, the play is flagged invalid.
    */

    int destfish, destipengid;

    int _invalidplay;

    destfish = getnumoffish(_boarddata[_desttile].intrninfo); //get the number of fish on the chosen tile
    destipengid = getintrnpengid(_boarddata[_desttile].intrninfo); //get the penguin id of the chosen tile

    if (destfish != STARTTILEFISH || destipengid >= 0)
    {
        _invalidplay = FLAG;
    }
    else
    {
        _invalidplay = UNFLAG;
    }

    return _invalidplay;
}

int checkdepttile(struct Board _boarddata[], struct Player _playerdata[], int _depttile, int _ppinfo)
{
    /*
    This function checks to make sure that the tile chosen to move a penguin out of actually contains the player's penguin and
    that that penguin is not trapped. If those conditions are not met, the function returns _invalidplay as 1.
    */

    int deptipengid, deptadjmoves;

    int _invalidplay;


    deptipengid = getintrnpengid(_boarddata[_depttile].intrninfo); //getting the penguin id of the departure tile
    deptadjmoves = getadjmoves(_boarddata[_depttile].intrninfo);

    /*penguin id on the chosen departure tile must be in the range of penguin ids belonging to the player found earlier.
    The penguin occupying the departure tile chosen must also have at least 1 possible adjacent move.*/
    if ((isallypengid(deptipengid, _ppinfo)) == FLAG && deptadjmoves > 0)
    {
        _invalidplay = UNFLAG;
    }
    else
    {
        _invalidplay = FLAG;
    }

    return _invalidplay;
}

int checkdesttile(struct Board _boarddata[], struct Player _playerdata[], int _depttile, int _desttile, int _ppinfo)
{
    /*
    This function checks to see if the penguin occupying the departure tile (_depttile) can move to the destination tile chosen
    (_desttile). In order for that to be the case, one of the destination tile's external penguin ids (destepengid) must be equal
    to the departure tile's internal penguin id (deptipengid), and the destination tile must have at least 1 fish on it. Otherwise,
    the destination tile chosen is invalid and _invalidplay is returned as 1.
    */

    int deptipengid;
    int destepengid;

    int dircount;

    int _invalidplay;

    deptipengid = getintrnpengid(_boarddata[_depttile].intrninfo);

    dircount = 0;

    while (dircount < MOVEDIRECTIONS)
    {
        destepengid = getextpengid(_boarddata[_desttile].extinfo[dircount]);

        if (destepengid == deptipengid && (getnumoffish(_boarddata[_desttile].intrninfo)) > 0)
        {
            _invalidplay = UNFLAG;

            break;
        }
        else
        {
            _invalidplay = FLAG;
        }

        dircount++;
    }

    return _invalidplay;
}

int placepenguin(struct Board _boarddata[], struct Player _playerdata[], int _desttile, int _ppinfo)
{
    /*
    This function resolves the placement of a penguin. An id for the new penguin is created and assigned to the destination
    tile (_desttile). Then, the number of fish on the destination tile (destfish) is added to the player's score. Finally, the
    number of fish on the destination tile is changed to 0. Afterwards, both structure arrays are updated with the new values.
    */

    int destfish, destadjmoves, destipengid;
    int destislandnum = NULLISLENUM, destaival = STARTAIMODVAL; //AI values are no longer needed once a move has been chosen. So, they can be made equal to 0.

    int _bestmove = _desttile, _aimode = DEFAULT, _maxislepoints = 0; //AI values are set to 0 for simplicity

    int _currentid, currentpengs, pengnum;

    _currentid = getcurrentid(_ppinfo);

    //the player's penguins that have already been placed are counted to get the next penguin's id
    currentpengs = countplyrpenguins(_playerdata, _currentid, _ppinfo);

    pengnum = currentpengs;

    destfish = getnumoffish(_boarddata[_desttile].intrninfo);
    destadjmoves = getadjmoves(_boarddata[_desttile].intrninfo);
    destipengid = (_currentid * (getpengsperplyr(_ppinfo))) + pengnum;

    _playerdata[_currentid].score = _playerdata[_currentid].score + destfish;

    destfish = 0;

    //both structure arrays are recalculated
    _boarddata[_desttile].intrninfo = getintrnboarddatasum(destfish, destadjmoves, destipengid, destislandnum, destaival);
    _playerdata[_currentid].pengdata[pengnum] = getpengdatasum(_desttile, _bestmove, _aimode, _maxislepoints);


    return 0;
}

int movepenguin(struct Board _boarddata[], struct Player _playerdata[], int _depttile, int _desttile, int _ppinfo)
{
    /*
    This function resolves the movement of a penguin. First, the destination tile internal penguin id (destipengid) is set equal
    to the departure tile's internal penguin id. Then, the number of fish on the destination tile (destfish) is added to the
    player's score. Finally, the number of fish on the destination tile is changed to 0. Afterwards, both structure arrays are
    updated with the new values.
    */

    int destfish, destadjmoves, destipengid;
    int destislandnum = NULLISLENUM, destaival = STARTAIMODVAL;

    int _bestmove, _aimode, _maxislepoints;

    int pengnum, _currentid;


    destfish = getnumoffish(_boarddata[_desttile].intrninfo);
    destadjmoves = getadjmoves(_boarddata[_desttile].intrninfo);
    destipengid = getintrnpengid(_boarddata[_depttile].intrninfo);

    pengnum = destipengid % (getpengsperplyr(_ppinfo));
    _currentid = getcurrentid(_ppinfo);

    _bestmove = getpengbestmove(_playerdata[_currentid].pengdata[pengnum]);
    _aimode = getpengaimode(_playerdata[_currentid].pengdata[pengnum]);
    _maxislepoints = getpengislesize(_playerdata[_currentid].pengdata[pengnum]);

    _playerdata[_currentid].score = _playerdata[_currentid].score + destfish;

    destfish = 0;

    _boarddata[_desttile].intrninfo = getintrnboarddatasum(destfish, destadjmoves, destipengid, destislandnum, destaival);
    _playerdata[_currentid].pengdata[pengnum] = getpengdatasum(_desttile, _bestmove, _aimode, _maxislepoints);
    _boarddata[_depttile].intrninfo = getintrnboarddatasum(0, 0, -1, NULLISLENUM, STARTAIMODVAL);

    return 0;
}


