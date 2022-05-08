#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

#include "univfuncs.h"
#include "structini.h"
#include "aialgs.h"
#include "sharedalgs.h"

#include "displayui.h"


int getmovefreedomrating(int _freetilesleft, int _ppinfo)
{
    /*
    This function returns a number representing the relative freedom of motion of each penguin, which is obtained from the
    fraction of free tiles remaining (determined from _freetilesleft and _startfreetiles (from _ppinfo)). The value returned,
    _freedomlevel, ranges from 0 to 4 and is decided by comparing the fraction of free tiles remaining (boardicecover) to
    various thresholds.
    */

    float _startfreetiles;
    float boardicecover;

    int _freedomlevel;

    _startfreetiles = getstartingfreetiles(_ppinfo);

    boardicecover = _freetilesleft / _startfreetiles;


    if (boardicecover > 0.85)
    {
        _freedomlevel = 0;
    }
    else if (boardicecover > 0.78 && boardicecover <= 0.85)
    {
        _freedomlevel = 1;
    }
    else if (boardicecover > 0.72 && boardicecover <= 0.78)
    {
        _freedomlevel = 2;
    }
    else if (boardicecover > 0.57 && boardicecover <= 0.72)
    {
        _freedomlevel = 3;
    }
    else
    {
        _freedomlevel = 4;
    }


    return _freedomlevel;
}

int getbestpengplace(struct Board _boarddata[], int _boarddim[], int _freedomlevel, int _ppinfo)
{
    /*
    This function finds the best tile to place the next penguin on by determining the ai modifier value of each tile on the
    board and choosing the tile with the highest ai modifier value. Only tiles that a penguin can legally be placed on are
    considered.
    */

    int currentaimodval, maxaimodval, maxvaltile;
    int tilecount, _totnumtiles;

    _totnumtiles = _boarddim[0] * _boarddim[1];

    tilecount = 0;
    maxaimodval = 0;

    maxvaltile = -1;

    /*This loop checks each tile on the board. Tiles that are not legal are automatically skipped. If the ai modifier value
    of the current tile is greater than or equal to the ai modifier values of all previous tiles, the current tile becomes the
    best tile to place the penguin on (maxvaaltile).*/
    while (tilecount < _totnumtiles)
    {
        if ((getnumoffish(_boarddata[tilecount].intrninfo)) != STARTTILEFISH || (getintrnpengid(_boarddata[tilecount].intrninfo)) != -1)
        {
            tilecount++;

            continue;
        }

        getaimodifiervalue(_boarddata, _boarddim, tilecount, _freedomlevel, _ppinfo);

        currentaimodval = getaimodval(_boarddata[tilecount].intrninfo);

        if (currentaimodval >= maxaimodval) //>= ensures maxvaltile will exist if aimodval is 0 for all possible tiles
        {
            maxaimodval = currentaimodval;
            maxvaltile = tilecount;
        }

        tilecount++;
    }

    return maxvaltile;
}

void getbestpengmovesaimods(struct Board _boarddata[], struct Player _playerdata[], int _boarddim[], int _freedomlevel, int _ppinfo)
{
    /*
    This function determines the ai modifier values of the tiles occupied by each of the player's penguins that can move. For
    each penguin, it then determines the ai modifier value of each tile a penguin can move to and finds the destination tile
    with the highest ai modifier value. If the ai mode of a penguin is not default, the freedom level is 4 (where ai modifier
    values are no longer used to determine the best move), or a penguin is blocked by the findislands() function (even if it
    can technically legally move), the 2nd part of this function is skipped. Instead, the ai modifier value of that penguin's
    best move (obtained from the findislands() function) is determined, and the player's next penguin is checked.
    */

    int _currentid, _pengsperplyr;

    int currentaimodval, maxaimodval;
    int _pengnum, currentpengtile, maxvaltile, possiblemoves;
    int _aimode, _maxislepoints;

    int temptile, dircount, dirmovecount;

    _currentid = getcurrentid(_ppinfo);
    _pengsperplyr = getpengsperplyr(_ppinfo);


    _pengnum = 0;

    while (_pengnum < _pengsperplyr)
    {
        /*Penguins that cannot move are skipped.*/
        if ((ispos(_playerdata[_currentid].pengdata[_pengnum])) == UNFLAG)
        {
            _pengnum++;

            continue;
        }

        currentpengtile = getpengtile(_playerdata[_currentid].pengdata[_pengnum]);
        maxvaltile = getpengbestmove(_playerdata[_currentid].pengdata[_pengnum]);
        _aimode = getpengaimode(_playerdata[_currentid].pengdata[_pengnum]);
        _maxislepoints = getpengislesize(_playerdata[_currentid].pengdata[_pengnum]);

        getaimodifiervalue(_boarddata, _boarddim, currentpengtile, _freedomlevel, _ppinfo);

        dircount = 0;
        maxaimodval = 0;

        while (dircount < MOVEDIRECTIONS)
        {
            /*The AI modifier values are only used to determine a penguin's move when the freedom level is 0, 1, 2, or 3 (3 only
            if there are many fish remaining that the penguin can potentially obtain). In other cases, the modified maze solving
            algorithm findbestpath() finds a penguin's best move. If the freedom level is 3 and many fish remain (only counting the
            fish that are possible for the penguin to pick up), The ai modifier values are only found for tiles in the direction(s)
            that have access to the most fish.*/
            if (_aimode != DEFAULT || _freedomlevel == 4 || (_freedomlevel == 3 && _maxislepoints < 60) || (maxvaltile == currentpengtile && _aimode == DEFAULT && _freedomlevel > 0))
            {
                getaimodifiervalue(_boarddata, _boarddim, maxvaltile, _freedomlevel, _ppinfo);

                break;
            }
            else if (_aimode == DEFAULT && _freedomlevel == 3 && maxvaltile != currentpengtile)
            {
                if ((getaddressislenum(_boarddata[currentpengtile].extinfo[dircount])) != (getaddressislenum(_boarddata[currentpengtile].extinfo[(getmovedirection(_boarddim, currentpengtile, maxvaltile))])))
                {
                    dircount++;

                    continue;
                }
            }


            temptile = currentpengtile;

            possiblemoves = gettotposmoves(_boarddata[currentpengtile].extinfo[dircount]);

            dirmovecount = 0;

            while (dirmovecount < possiblemoves)
            {
                temptile = shiftonetile(_boarddim, temptile, dircount);

                getaimodifiervalue(_boarddata, _boarddim, temptile, _freedomlevel, _ppinfo);
                currentaimodval = getaimodval(_boarddata[temptile].intrninfo);

                if (_aimode == DEFAULT && currentaimodval >= maxaimodval)
                {
                    maxaimodval = currentaimodval;
                    maxvaltile = temptile;
                }

                dirmovecount++;
            }

            dircount++;
        }

        _playerdata[_currentid].pengdata[_pengnum] = getpengdatasum(currentpengtile, maxvaltile, _aimode, _maxislepoints);

        _pengnum++;
    }
}


void getaimodifiervalue(struct Board _boarddata[], int _boarddim[], int _tilenum, int _freedomlevel, int _ppinfo)
{
    /*This function finds the ai modifier value (0 to 29999) for each tile tested. A number of variables affect the final
    score, and each variable is weighted. The amount each value is weighted changes depending on the freedom level.*/

    int _fish, _adjmoves, _ipengid, _islandnum, aimodval;
    int _moves, _hifish, _epengid;

    /*adj1moves is the number of free adjacent tiles, adj2moves is the number of free tiles that are 2 tiles away, and
    adj3moves is the number of free tiles that are 3 tiles away.*/
    int adj1moves, adj2moves, adj3moves, totmovesalldir;
    int hifishnumdir, tothifish;
    int allyepengidcount, foeepengidcount;

    int a, b, c, d, e, f, g, h, i, j, k, m, n, p, q, r, s, t, u, v;


    /*
    Weights of all the factors that affect the ai modifier value. Higher values indicate favorable factors. Lower values
    indicate unfavorable factors. Certain factors have multiple weights when their effects suffer from diminishing returns
    (their effectiveness is not linear to their quantity).

    a is the weight of adj1moves when adj1moves == 4 (so the weight of each possible adjacent move when there are 4 adjacent moves).
    b is the weight of adj1moves when adj1moves == 3.
    c is the weight of adj1moves when adj1moves == 2.
    d is the weight of adj2moves when adj1moves == 4 and adj2moves ==4.
    e is the weight of adj2moves in all other cases.
    f is the weight of adj3moves.
    g is the weight of totmovesalldir (the total number of moves that can be made from the tile tested).

    h is the weight of hifishnumdir (the number of directions in which there is at least 1 tile with 3 fish can be moved to).
    i is the weight of tothifish (the total number of tiles with 3 fish that can be moved to).

    j is the weight of allypengidcount (the number of friendly penguins that have line of sight with the tile tested).
    k is the weight of foepengidcount (the number of opposing players' penguins that have line of sight with the tile tested).

    m is the weight each tile that has 3 fish.
    n is the weight each tile that has 2 fish.
    p is the weight each tile that has 1 fish.

    q is the weight of a tile that is 0, 1, or 2 tiles away from the board's edge.
    r is the weight of a tile that is 0 or 1 tiles away from the board's edge.
    s is the weight of a tile that is on the board's edge.

    t is the weight of a tile that is part of an island (a group of connected tiles separated from all other tiles) that only the
    player's penguins can access. This number is extremely low (-6000) since only penguins with an ai mode of TURTLE should be
    moving into player controlled islands. Since only DEFAULT mode penguins use ai modifier values to determine their best move
    and the values of tiles inside player controlled islands are so low, penguins that are in DEFAULT mode should almost never
    enter those islands, while penguins in TURTLE mode should remain unaffected (they use a different algorithm to decide their
    best move.

    u is the weight of a tile when adjmoves1 == 1.
    v is the weight of a tile when adjmoves1 == 0.
    Moves to tiles that have 0 or 1 possible adjacent moves are unfavorable. So the values are negative.
    */
    if (_freedomlevel < 2)
    {
        a = 65, b = 73, c = 80, d = 27, e = 30, f = 20, g = 30, h = 25, i = 40, j = -30, k = -50, m = 250, n = 100, p = 0, q = -20, r = -50, s = -120, t = -6000, u = -1300, v = -2000;
    }
    else if (_freedomlevel == 2)
    {
        a = 65, b = 73, c = 80, d = 27, e = 30, f = 20, g = 30, h = 15, i = 15, j = -20, k = -35, m = 60, n = 30, p = 15, q = -10, r = -25, s = -80, t = -6000, u = -600, v = -1200;
    }
    else
    {
        a = 65, b = 73, c = 80, d = 27, e = 30, f = 20, g = 35, h = 0, i = 0, j = -10, k = -17, m = 10, n = 10, p = 10, q = -5, r = -10, s = -25, t = -6000, u = -500, v = -1000;
    }


    int _rownum, _colnum, maxrownum, maxcolnum;
    int dircount;


    _rownum = getrownum(_boarddim, _tilenum);
    _colnum = getcolnum(_boarddim, _tilenum);

    maxrownum = _boarddim[0] - ARRAYSHIFT;
    maxcolnum = _boarddim[1] - ARRAYSHIFT;

    _fish = getnumoffish(_boarddata[_tilenum].intrninfo);
    _adjmoves = getadjmoves(_boarddata[_tilenum].intrninfo);
    _ipengid = getintrnpengid(_boarddata[_tilenum].intrninfo);
    _islandnum = getislandnum(_boarddata[_tilenum].intrninfo);

    aimodval = STARTAIMODVAL;

    totmovesalldir = 0;
    tothifish = 0;

    adj1moves = 0;
    adj2moves = 0;
    adj3moves = 0;

    hifishnumdir = 0;

    allyepengidcount = 0;
    foeepengidcount = 0;

    dircount = 0;

    while (dircount < MOVEDIRECTIONS)
    {
        _moves = gettotposmoves(_boarddata[_tilenum].extinfo[dircount]);
        _hifish = gethifishtiles(_boarddata[_tilenum].extinfo[dircount]);
        _epengid = getextpengid(_boarddata[_tilenum].extinfo[dircount]);

        totmovesalldir = totmovesalldir + _moves;
        tothifish = tothifish + _hifish;

        if (_moves >= 1)
        {
            adj1moves++;
        }
        else if (_moves >= 2)
        {
            adj2moves++;
        }
        else if (_moves >= 3)
        {
            adj3moves++;
        }

        if (_hifish >= 1)
        {
            hifishnumdir++;
        }

        if ((isallypengid(_epengid, _ppinfo)) == FLAG && _epengid != _ipengid)
        {
            allyepengidcount++;
        }
        else if((isfoepengid(_epengid, _ppinfo)) == FLAG && _epengid != _ipengid)
        {
            foeepengidcount++;
        }

        dircount++;
    }

    if (adj1moves == 4)
    {
        if (adj2moves == 4)
        {
            aimodval = aimodval + (adj1moves * a) + (adj2moves * d) + (adj3moves * f) + (totmovesalldir * g);
        }
        else
        {
            aimodval = aimodval + (adj1moves * a) + (adj2moves * e) + (adj3moves * f) + (totmovesalldir * g);
        }
    }
    else if (adj1moves == 3)
    {
        aimodval = aimodval + (adj1moves * b) + (adj2moves * e) + (adj3moves * f) + (totmovesalldir * g);
    }
    else if (adj1moves == 2)
    {
        aimodval = aimodval + (adj1moves * c) + (adj2moves * e) + (adj3moves * f) + (totmovesalldir * g);
    }
    else if (adj1moves == 1)
    {
        aimodval = aimodval + (totmovesalldir * g) + u;
    }
    else
    {
        aimodval = aimodval + v;
    }

    aimodval = aimodval + (hifishnumdir * h) + (tothifish * i);
    aimodval = aimodval + (allyepengidcount * j) + (foeepengidcount * k);

    if (_fish == MAXFISH)
    {
        aimodval = aimodval + (_fish * m);
    }
    else if (_fish == (MAXFISH - 1))
    {
        aimodval = aimodval + (_fish * n);
    }
    else
    {
        aimodval = aimodval + (_fish * p);
    }

    if (_rownum <= 2 || _rownum >= (maxrownum - 2))
    {
        aimodval = aimodval + q;

        if (_rownum <= 1 || _rownum >= (maxrownum - 1))
        {
            aimodval = aimodval + r;

            if (_rownum == 0 || _rownum == maxrownum)
            {
                aimodval = aimodval + s;
            }
        }
    }

    if (_colnum <= 2 || _colnum >= (maxcolnum - 2))
    {
        aimodval = aimodval + q;

        if (_colnum <= 1 || _colnum >= (maxcolnum - 1))
        {
            aimodval = aimodval + r;

            if (_colnum == 0 || _colnum == maxcolnum)
            {
                aimodval = aimodval + s;
            }
        }
    }

    if ((getislandnum(_boarddata[_tilenum].intrninfo)) == OWNISLENUM)
    {
        aimodval = aimodval + t;
    }

    _boarddata[_tilenum].intrninfo = getintrnboarddatasum(_fish, _adjmoves, _ipengid, _islandnum, aimodval);
}

int getfinalpengmove(struct Board _boarddata[], struct Player _playerdata[], int _boarddim[], int _ppinfo)
{
    /*This function compares ai modes, ai modifier values, and island sizes to determine which penguin will be moved.*/

    int _currentid, _pengsperplyr;
    int intrnaimodval, extaimodval, netaimodval, maxaimodval;
    int _aimode, maxaimode, islesize, maxislesize, turtmaxislesize;
    int _pengnum, maxvalpeng, intrntile, exttile;

    _currentid = getcurrentid(_ppinfo);
    _pengsperplyr = getpengsperplyr(_ppinfo);

    _pengnum = 0;
    maxaimodval = 0;
    maxaimode = 0;
    maxislesize = 0;
    turtmaxislesize = 0;

    maxvalpeng = -1;


    while (_pengnum < _pengsperplyr)
    {
        if ((ispos(_playerdata[_currentid].pengdata[_pengnum])) == UNFLAG)
        {
            _pengnum++;

            continue;
        }

        intrntile = getpengtile(_playerdata[_currentid].pengdata[_pengnum]);
        exttile = getpengbestmove(_playerdata[_currentid].pengdata[_pengnum]);
        _aimode = getpengaimode(_playerdata[_currentid].pengdata[_pengnum]);
        islesize = getpengislesize(_playerdata[_currentid].pengdata[_pengnum]);

        intrnaimodval = getaimodval(_boarddata[intrntile].intrninfo);
        extaimodval = getaimodval(_boarddata[exttile].intrninfo);

        netaimodval = extaimodval - (intrnaimodval - STARTAIMODVAL);


        /*In some cases (when 2 penguins control the same island or when 2 penguins want to move to gain control an island,
        which requires the 2nd penguin to not move this turn) a penguin's current tile and best move tile will be the same.
        This ensures that one of those penguins will not be chosen to move.*/
        if (exttile == intrntile)
        {
            _pengnum++;

            continue;
        }

        /*The priority system used to determine which penguin will ultimately be moved.*/
        if (_aimode > DEFAULT && islesize > maxislesize)
        {
            if (_aimode > maxaimode)
            {
                maxaimode = _aimode;
            }

            maxislesize = islesize;
            maxvalpeng = _pengnum;
        }
        else if (_aimode > maxaimode)
        {
            if (_aimode == DEFAULT)
            {
                maxaimode = _aimode;
                maxaimodval = netaimodval;
                maxvalpeng = _pengnum;
            }
            else if (_aimode > DEFAULT && islesize >= maxislesize)
            {
                maxaimode = _aimode;
                maxislesize = islesize;
                maxvalpeng = _pengnum;
            }
        }
        else if (_aimode == maxaimode)
        {
            if (_aimode == TURTLE && islesize >= turtmaxislesize)
            {
                turtmaxislesize = islesize;
                maxvalpeng = _pengnum;
            }
            else if (_aimode == DEFAULT && netaimodval >= maxaimodval)
            {
                maxaimodval = netaimodval;
                maxvalpeng = _pengnum;
            }
            else if (_aimode > DEFAULT && islesize >= maxislesize)
            {
                maxislesize = islesize;
                maxvalpeng = _pengnum;
            }
        }

        _pengnum++;
    }

    return maxvalpeng;
}

void findislands(struct Board _boarddata[], struct Player _playerdata[], int _boarddim[], int _islenumtested, int _numofguesses, int _freedomlevel, int _ppinfo)
{
    /*
    This function searches the board for groups of tiles that are all connected to one another (islands). These islands are
    separated from all the other tiles. Once it identifies an island, it checks if any of the current player's and/or another
    player's penguins can move to any tiles belonging to that island. If only the current player's penguins can reach the island,
    a modified maze solving algorithm is used to determine the number of fish that can be obtained from that island, as well as
    the next move that the penguin should make to get the maximum number of fish from that island. If the current player's
    penguins AND another player's penguins can access an island, the function tries to find a move or a combination of two
    moves that will result in one or more of the player's penguins gaining control of an island (splitting the original island
    into 2 or more pieces).

    Additionally, in the case of a shared island, the total number of fish that can be obtained by moving in each direction
    by each of the player's penguins is found and saved. If it is found that a move (or series of moves) can be made to capture
    an island, the island's value is compared against the total number of fish that can be obtained if the island is not captured
    (adjusted to take into account the fact that only a small fraction of the total fish in a contested island will actually be
    picked up by the player's penguins). Capturing an island is only be considered as a best move for a penguin if its value
    is high enough.

    When freedomlevel is 3 or 4 (depending on the number of fish that can be obtained by a penguin when freedom level is 3),
    the maze solving algorithm (findbestpath()) is used to determine the best moves for penguins that remain in DEFAULT mode.
    When this is the case, the ai modifier values found later only affect the move priority and not the actual moves the penguins
    make.

    Finally, the function determines the best combination of moves for all penguins (initially, each penguin has up to 4 best
    moves, one in each direction). This is necessary in case a penguin has access to more than one island or two penguins have
    access to the same island (or countless other more complex combinations of penguins and islands). Since the function can
    only analyze one island at a time and each penguin can access up to 4 different islands, it is necessary to store the ideal
    move in each of the 4 directions for each penguin until the end of the function, when all the move combinations can be
    analyzed.
    */

    int _fish, fishonisle, _adjmoves, _epengid;
    int _currentid, _pengnum, _pengsperplyr;

    int tilecount, currenttile, _temptile, islemappingtile, _totnumtiles, isleadjtile, dircount;
    int _tileislenum, _adjtileislenum, _islepoints;

    int newtiletested, ownedisle, allypengcheck, foepengcheck;

    int bestmoveindex, boundpengindex, allytotislenum;


    _adjtileislenum = -1;
    _islepoints = 0;

    _currentid = getcurrentid(_ppinfo);
    _pengsperplyr = getpengsperplyr(_ppinfo);

    _totnumtiles = _boarddim[0] * _boarddim[1];


    int bestmoveinfo[(_pengsperplyr * MOVEDIRECTIONS)];
    int maxbmoveinfo[(_pengsperplyr * MOVEDIRECTIONS)];
    int boundpenginfo[(_pengsperplyr * MOVEDIRECTIONS)];


    bestmoveindex = 0;

    while (bestmoveindex < (_pengsperplyr * MOVEDIRECTIONS))
    {
        bestmoveinfo[bestmoveindex] = 0;
        maxbmoveinfo[bestmoveindex] = 0;
        boundpenginfo[bestmoveindex] = 0;

        bestmoveindex++;
    }

    allytotislenum = 0;
    tilecount = 0;

    while (tilecount < _totnumtiles)
    {
        ownedisle = UNFLAG;

        _fish = getnumoffish(_boarddata[tilecount].intrninfo);
        _tileislenum = getislandnum(_boarddata[tilecount].intrninfo);

        fishonisle = 0;
        boundpengindex = -1;

        /*Each tile is checked until one that has an island number that is being searched for is found.*/
        if (_fish > 0 && _tileislenum == _islenumtested)
        {
            newtiletested = UNFLAG;

            allypengcheck = UNFLAG;
            foepengcheck = UNFLAG;

            _tileislenum = ISLETILEFOUND;
            _boarddata[tilecount].intrninfo = changeislandnum(_boarddata[tilecount].intrninfo, _tileislenum);

            islemappingtile = tilecount;

            /*Once a tile with the desired island number is found, it's adjacent tiles are searched. Adjacent tiles that are
            free must be part of the same island. The search is repeated (checking each tile) until no new free adjacent tiles
            are found. If any tile can be accessed by the player's penguins or another player's penguins, it is recorded.*/
            while (islemappingtile < _totnumtiles || newtiletested == FLAG)
            {
                if (islemappingtile == _totnumtiles)
                {
                    islemappingtile = tilecount;

                    newtiletested = UNFLAG;
                }

                if ((getislandnum(_boarddata[islemappingtile].intrninfo)) == ISLETILEFOUND)
                {
                    newtiletested = FLAG;

                    fishonisle = fishonisle + getnumoffish(_boarddata[islemappingtile].intrninfo);

                    _adjmoves = getadjmoves(_boarddata[islemappingtile].intrninfo);

                    _tileislenum = ISLETILETESTED;
                    _boarddata[islemappingtile].intrninfo = changeislandnum(_boarddata[islemappingtile].intrninfo, _tileislenum);

                    dircount = 0;

                    while (dircount < MOVEDIRECTIONS)
                    {
                        _epengid = getextpengid(_boarddata[islemappingtile].extinfo[dircount]);

                        if ((isallypengid(_epengid, _ppinfo)) == FLAG)
                        {
                            allypengcheck = FLAG;
                        }
                        else if ((isfoepengid(_epengid, _ppinfo)) == FLAG)
                        {
                            foepengcheck = FLAG;
                        }


                        if ((_adjmoves % 2) == 1)
                        {
                            isleadjtile = shiftonetile(_boarddim, islemappingtile, dircount);

                            if ((getislandnum(_boarddata[isleadjtile].intrninfo)) == _islenumtested)
                            {
                                _tileislenum = ISLETILEFOUND;
                                _boarddata[isleadjtile].intrninfo = changeislandnum(_boarddata[isleadjtile].intrninfo, _tileislenum);
                            }
                        }

                        _adjmoves = _adjmoves / 2;

                        dircount++;
                    }
                }

                islemappingtile++;
            }


            islemappingtile = tilecount;

            /*If the island is contested, the function attempts to guess tiles that the player's penguins can move to so that
            the player can gain control of part of it. The contested island's value (based on the total number of fish inside it)
            is also found and used to determine if it is worth securing a particular island.
            If the island is already controlled by the player, the value of the island and the best move for the penguin
            "claiming" the island is found.*/
            if (allypengcheck == FLAG && foepengcheck == FLAG && _numofguesses < 2)
            {
                _tileislenum = _islenumtested + 1;

                while (islemappingtile < _totnumtiles)
                {
                    if ((getislandnum(_boarddata[islemappingtile].intrninfo)) == ISLETILETESTED)
                    {
                        _boarddata[islemappingtile].intrninfo = changeislandnum(_boarddata[islemappingtile].intrninfo, _tileislenum);
                    }

                    islemappingtile++;
                }


                /*This function guesses the tiles that the player's penguins should move to in order to secure an island.*/
                boundpengindex = tileprediction(_boarddata, _playerdata, _boarddim, _tileislenum, _numofguesses, _ppinfo);


                /*Whether or not an island is secured as well as the moves for DEFAULT mode at high freedom levels are determined
                here.*/
                if (_numofguesses == 0)
                {
                    _pengnum = 0;

                    while (_pengnum < _pengsperplyr)
                    {
                        currenttile = getpengtile(_playerdata[_currentid].pengdata[_pengnum]);

                        dircount = 0;

                        while (dircount < MOVEDIRECTIONS)
                        {
                            _temptile = shiftonetile(_boarddim, currenttile, dircount);

                            if (_temptile == -1 || (getnumoffish(_boarddata[_temptile].intrninfo)) == 0)
                            {
                                dircount++;

                                continue;
                            }


                            if ((getislandnum(_boarddata[_temptile].intrninfo)) == _tileislenum && (getintrnpengid(_boarddata[currenttile].intrninfo) == (getextpengid(_boarddata[_temptile].extinfo[(dircount + 2) % 4]))))
                            {
                                if (_freedomlevel == 3 || _freedomlevel == 4)
                                {
                                    int resetpathmoves, incrementpathmoves;

                                    /*Copies of the structure arrays are made to ensure that the original values are not overwritten
                                    when move predictions are made.*/
                                    struct Board dummyboarddata[_totnumtiles];
                                    struct Player dummyplayerdata[getnumofplyrs(_ppinfo)];

                                    copytodummyboard(dummyboarddata, _boarddata, _boarddim);
                                    copytodummyplayer(dummyplayerdata, _playerdata, _ppinfo);


                                    resetpathmoves = FLAG;
                                    incrementpathmoves = UNFLAG;

                                    recursioncounter(resetpathmoves, incrementpathmoves);

                                    /*The maze solving algorithm finds the optimal path through an island (it only saves the first
                                    move in the structure array) and returns the number of fish that can be obtained from the island
                                    searched*/
                                    _islepoints = findbestpath(dummyboarddata, dummyplayerdata, _boarddim, currenttile, _pengnum, _tileislenum, 0, 0, _ppinfo);


                                    bestmoveinfo[((_pengnum * MOVEDIRECTIONS) + dircount)] = gettempisleinfosum(allytotislenum, getpengbestmove(dummyplayerdata[_currentid].pengdata[_pengnum]), DEFAULT, _islepoints);

                                    _boarddata[currenttile].extinfo[dircount] = changeaddressislenum(_boarddata[currenttile].extinfo[dircount], allytotislenum);
                                }
                                else
                                {
                                    bestmoveinfo[((_pengnum * MOVEDIRECTIONS) + dircount)] = gettempisleinfosum(allytotislenum, _temptile, DEFAULT, fishonisle);

                                    _boarddata[currenttile].extinfo[dircount] = changeaddressislenum(_boarddata[currenttile].extinfo[dircount], allytotislenum);
                                }
                            }

                            dircount++;
                        }


                        _temptile = getpengbestmove(_playerdata[_currentid].pengdata[_pengnum]);

                        if ((getislandnum(_boarddata[_temptile].intrninfo)) == _tileislenum && (getpengaimode(_playerdata[_currentid].pengdata[_pengnum])) > DEFAULT)
                        {
                            bestmoveindex = getbestmoveindex(_boarddim, currenttile, _temptile, _pengnum);

                            if ((getpengislesize(_playerdata[_currentid].pengdata[_pengnum])) > (getadjustedfishval(getallyislesize(bestmoveinfo[bestmoveindex]), _freedomlevel, _ppinfo)))
                            {
                                bestmoveinfo[bestmoveindex] = gettempisleinfosum(allytotislenum, _temptile, (getpengaimode(_playerdata[_currentid].pengdata[_pengnum])), (getpengislesize(_playerdata[_currentid].pengdata[_pengnum])));

                                /*If 2 different penguins are to secure an island (which will take 2 turns), the 2nd penguin must
                                be bound to ensure that is will be available to make the planned move next turn. Bound penguins
                                cannot move and are treated as if they are in DEFAULT mode when their island points are
                                determined. The ids of the 2 penguins bound to one another are saved.*/
                                if ((isneg(boundpengindex)) == UNFLAG)
                                {
                                    boundpenginfo[bestmoveindex] = boundpengindex + ARRAYSHIFT;
                                    boundpenginfo[boundpengindex] = (bestmoveindex + ARRAYSHIFT) * NEGONE;
                                }
                            }
                        }

                        _pengnum++;
                    }

                    allytotislenum++;
                }
            }
            else if (allypengcheck == FLAG && foepengcheck == UNFLAG)
            {
                /*Copies of the structure arrays are made to ensure that the original values are not overwritten
                when move predictions are made.*/
                struct Board dummyboarddata[_totnumtiles];
                struct Player dummyplayerdata[getnumofplyrs(_ppinfo)];


                ownedisle = UNFLAG;

                _pengnum = 0;


                while (_pengnum < _pengsperplyr)
                {
                    dircount = 0;
                    _adjtileislenum = 0;

                    currenttile = getpengtile(_playerdata[_currentid].pengdata[_pengnum]);

                    while (dircount < MOVEDIRECTIONS)
                    {
                        if ((gettotposmoves(_boarddata[currenttile].extinfo[dircount])) > 0)
                        {
                            _temptile = shiftonetile(_boarddim, currenttile, dircount);
                            _tileislenum = getislandnum(_boarddata[_temptile].intrninfo);

                            if (_tileislenum == ISLETILETESTED)
                            {
                                _adjtileislenum = ISLETILETESTED;
                            }
                        }

                        dircount++;
                    }

                    /*Makes sure that only penguins that have access to the island tested are checked.*/
                    if (_adjtileislenum == ISLETILETESTED)
                    {
                        int resetpathmoves, incrementpathmoves;

                        resetpathmoves = FLAG;
                        incrementpathmoves = UNFLAG;

                        recursioncounter(resetpathmoves, incrementpathmoves);

                        copytodummyboard(dummyboarddata, _boarddata, _boarddim);
                        copytodummyplayer(dummyplayerdata, _playerdata, _ppinfo);

                        /*The maze solving algorithm finds the optimal path through an island (it only saves the first
                        move in the structure array) and returns the number of fish that can be obtained from the island
                        searched*/
                        _islepoints = findbestpath(dummyboarddata, dummyplayerdata, _boarddim, currenttile, _pengnum, _adjtileislenum, 0, _numofguesses, _ppinfo);

                        ownedisle = FLAG;

                        bestmoveindex = getbestmoveindex(_boarddim, currenttile, getpengbestmove(dummyplayerdata[_currentid].pengdata[_pengnum]), _pengnum);
                        bestmoveinfo[bestmoveindex] = gettempisleinfosum(allytotislenum, (getpengbestmove(dummyplayerdata[_currentid].pengdata[_pengnum])), TURTLE, _islepoints);

                        _boarddata[currenttile].extinfo[(bestmoveindex % MOVEDIRECTIONS)] = changeaddressislenum(_boarddata[currenttile].extinfo[(bestmoveindex % MOVEDIRECTIONS)], allytotislenum);
                    }

                    _pengnum++;
                }

                allytotislenum++;
            }


            islemappingtile = tilecount;

            /*After each island is found and analyzed, the tiles that make up that island are "discarded".*/
            if (ownedisle == FLAG)
            {
                _tileislenum = OWNISLENUM;
            }
            else
            {
                _tileislenum = JUNKISLENUM;
            }


            while (islemappingtile < _totnumtiles)
            {
                if ((getislandnum(_boarddata[islemappingtile].intrninfo)) == ISLETILETESTED || ((getislandnum(_boarddata[islemappingtile].intrninfo)) == (_islenumtested + 1) && _numofguesses < 2))
                {
                    _boarddata[islemappingtile].intrninfo = changeislandnum(_boarddata[islemappingtile].intrninfo, _tileislenum);
                }

                islemappingtile++;
            }

        }

        tilecount++;
    }


    /*For each penguin, the best move in each of the 4 directions is found (assuming the penguin can move in all 4 directions).
    This function determines the best move for each penguin.*/
    getbestpengmovesisles(_playerdata, _boarddim, bestmoveinfo, maxbmoveinfo, boundpenginfo, _freedomlevel, _ppinfo);
}

int tileprediction(struct Board _boarddata[], struct Player _playerdata[], int _boarddim[], int _islenumtested, int _numofguesses, int _ppinfo)
{
    /*
    This function guesses the tiles that need to be moved to in order to gain control of an island. It is optimized by discarding
    guesses that are known to not produce a new island. If an island is to be captured using 2 moves, the 2nd move must be
    guaranteed (it cannot have a chance of being intercepted by another player's penguin). A necessary condition for obtaining
    a new island is that the last tile that is moved to must be a choke point (the new island must be cut off from the old island).
    The value returned is used to determine if a 2nd penguin's behavior needs to be changed (if 2 different penguins are used to
    secure an island). If that is the case, the 2nd penguin becomes "bound" and is unable to move this turn, in addition to
    being treated as a DEFAULT penguin when determining its island points. The 2nd penguin is only bound if the 1st penguin
    (the penguin to move this turn) ends up having a best move that includes the 2nd penguin. If the 1st penguin has the ability
    to capture 2 different islands, one by itself and one with the help of the 2nd penguin, and the island that it can capture
    by itself is found to be worth more points, the 2nd penguin will no longer be bound.
    */

    int _currentid, _pengsperplyr;

    int _pengnum, _testpengnum, _pengtile, _tempaimode, _islepoints;
    int _bestpengnum, _bestpengtile, _bestmove, _aimode, _maxislepoints;

    int _epengid, possiblemoves, foepengintercept;

    int _tileislenum, possibleisle;
    int  _temptile, dircount, tempdircount, dirmovestested, _totnumtiles;

    int _boundpengindex, _tempboundpengindex;


    _totnumtiles = _boarddim[0] * _boarddim[1];

    _currentid = getcurrentid(_ppinfo);
    _pengsperplyr = getpengsperplyr(_ppinfo);

    possibleisle = UNFLAG;

    _maxislepoints = 0;

    _boundpengindex = -1;

    _pengnum = 0;

    while (_pengnum < _pengsperplyr)
    {
        dircount = 0;

        _pengtile = getpengtile(_playerdata[_currentid].pengdata[_pengnum]);

        while (dircount < MOVEDIRECTIONS)
        {
            dirmovestested = 0;

            _temptile = _pengtile;

            possiblemoves = gettotposmoves(_boarddata[_pengtile].extinfo[dircount]);

            while (dirmovestested < possiblemoves)
            {
                _temptile = shiftonetile(_boarddim, _temptile, dircount);
                _tileislenum = getislandnum(_boarddata[_temptile].intrninfo);

                foepengintercept = UNFLAG;

                tempdircount = 0;

                while (tempdircount < MOVEDIRECTIONS)
                {
                    _epengid = getextpengid(_boarddata[_temptile].extinfo[tempdircount]);

                    if ((isfoepengid(_epengid, _ppinfo)) == FLAG)
                    {
                        foepengintercept = FLAG;
                    }

                    tempdircount++;
                }

                if (_tileislenum != _islenumtested || (foepengintercept == FLAG && _numofguesses > 0))
                {
                    break;
                }
                else if ((ischokepoint(_boarddata, _boarddim, _temptile, _ppinfo)) == FLAG || _numofguesses == 0)
                {
                    int pengdatarawvals[_pengsperplyr];

                    /*Copies of the structure arrays are made to ensure that the original values are not overwritten
                    when move predictions are made.*/
                    struct Board dummyboarddata[_totnumtiles];
                    struct Player dummyplayerdata[getnumofplyrs(_ppinfo)];

                    copytodummyboard(dummyboarddata, _boarddata, _boarddim);
                    copytodummyplayer(dummyplayerdata, _playerdata, _ppinfo);


                    movepenguin(dummyboarddata, dummyplayerdata, _pengtile, _temptile, _ppinfo);
                    scanboard(dummyboarddata, _boarddim);


                    _testpengnum = 0;

                    while (_testpengnum < _pengsperplyr)
                    {
                        pengdatarawvals[_testpengnum] = dummyplayerdata[_currentid].pengdata[_testpengnum];

                        _testpengnum++;
                    }

                    /*findislands() is used recursively to find and analyze the island(s) after the dummy move is made. Since
                    _islenumtested is incremented each time tileprediction() is called, only the relevant tiles are tested.
                    Since this function only forecasts up to 2 moves, the 2nd guessed move must be a choke point. If the 1st
                    guessed move is not a choke point, tileprediction() is called again.*/
                    if ((ischokepoint(_boarddata, _boarddim, _temptile, _ppinfo)) == FLAG)
                    {
                        findislands(dummyboarddata, dummyplayerdata, _boarddim, _islenumtested, (_numofguesses + 1), 0, _ppinfo);
                    }
                    else if ((ischokepoint(_boarddata, _boarddim, _temptile, _ppinfo)) == UNFLAG)
                    {
                        tileprediction(dummyboarddata, dummyplayerdata, _boarddim, _islenumtested, (_numofguesses + 1), _ppinfo);
                    }


                    _testpengnum = 0;

                    /*Determines the best possible island that can be obtained (if one exists) based on the island points.*/
                    while (_testpengnum < _pengsperplyr)
                    {
                        if (dummyplayerdata[_currentid].pengdata[_testpengnum] != pengdatarawvals[_testpengnum] && (getpengaimode(dummyplayerdata[_currentid].pengdata[_testpengnum])) != DEFAULT && (ispos(dummyplayerdata[_currentid].pengdata[_testpengnum]) == FLAG))
                        {
                            _islepoints = getpengislesize(dummyplayerdata[_currentid].pengdata[_testpengnum]);

                            _tempboundpengindex = -1;

                            if ((getpengaimode(dummyplayerdata[_currentid].pengdata[_testpengnum])) == TURTLE && _numofguesses == 0)
                            {
                                _tempaimode = OVERRIDE;
                            }
                            else if ((getpengaimode(dummyplayerdata[_currentid].pengdata[_testpengnum])) == TURTLE && _numofguesses == 1)
                            {
                                _tempaimode = SECURE;
                            }
                            else
                            {
                                _tempaimode = getpengaimode(dummyplayerdata[_currentid].pengdata[_testpengnum]);

                                if (_pengnum != _testpengnum)
                                {
                                    _tempboundpengindex = getbestmoveindex(_boarddim, getpengtile(dummyplayerdata[_currentid].pengdata[_testpengnum]), getpengbestmove(dummyplayerdata[_currentid].pengdata[_testpengnum]), _testpengnum);
                                }
                            }


                            if ((_islepoints >= _maxislepoints && _tempaimode == OVERRIDE) || (_islepoints > _maxislepoints && _tempaimode == SECURE))
                            {
                                possibleisle = FLAG;

                                _boundpengindex = _tempboundpengindex;

                                _maxislepoints = _islepoints;
                                _bestpengnum = _pengnum;
                                _bestpengtile = _pengtile;
                                _bestmove = _temptile;
                                _aimode = _tempaimode;
                            }
                        }

                        _testpengnum++;
                    }
                }

                dirmovestested++;
            }

            dircount++;
        }

        _pengnum++;
    }


    if (possibleisle == FLAG)
    {
        _playerdata[_currentid].pengdata[_bestpengnum] = getpengdatasum(_bestpengtile, _bestmove, _aimode, _maxislepoints);
    }


    return _boundpengindex;
}

int ischokepoint(struct Board _boarddata[], int _boarddim[], int _tiletocheck, int _ppinfo)
{
    /*
    This function checks if a tile is a choke point. To be valid, the tile (if occupied) must split an existing island into
    2 or more smaller island, and only the player's penguins can access the desired island. This is accomplished by checking
    the accessibility of all 8 tiles surrounding the tile being tested.
    */

    int _rownum, _colnum, maxrownum, maxcolnum, minrownum, mincolnum, rowchange, colchange;
    int temptile, dircountpass1, dircountpass2;

    int tileaccess, accessflipcount;
    int chokepoint;

    minrownum = 0;
    mincolnum = 0;

    maxrownum = _boarddim[0] - ARRAYSHIFT;
    maxcolnum = _boarddim[1] - ARRAYSHIFT;

    _rownum = getrownum(_boarddim, _tiletocheck);
    _colnum = getcolnum(_boarddim, _tiletocheck);

    temptile = _tiletocheck - _boarddim[1] - 1;
    rowchange = -1;
    colchange = -1;

    tileaccess = FLAG;
    accessflipcount = 0;

    dircountpass1 = 0;
    dircountpass2 = -1;

    while (dircountpass1 < MOVEANDDIAGDIR && dircountpass2 < MOVEANDDIAGDIR)
    {
        if ((rowchange == -1 && _rownum == minrownum) || (rowchange == 1 && _rownum == maxrownum) || (colchange == -1 && _colnum == mincolnum) || (colchange == 1 && _colnum == maxcolnum))
        {
            if (dircountpass2 == -1)
            {
                dircountpass2 = 0;
            }

            tileaccess = UNFLAG;
        }
        else if ((getnumoffish(_boarddata[temptile].intrninfo)) == 0 && (isfoepengid(getintrnpengid(_boarddata[temptile].intrninfo), _ppinfo)) == UNFLAG)
        {
            if (dircountpass2 == -1)
            {
                dircountpass2 = 0;
            }

            tileaccess = UNFLAG;
        }
        else if (dircountpass2 != -1 && (isallypengid(getintrnpengid(_boarddata[temptile].intrninfo), _ppinfo)) == UNFLAG && (rowchange == 0 || colchange == 0) && tileaccess == UNFLAG)
        {
            tileaccess = FLAG;

            accessflipcount++;
        }


        if (((dircountpass1 / 2) % 2) == 0)
        {
            temptile = temptile - ((dircountpass1 / 2) - 1);
            colchange = colchange - ((dircountpass1 / 2) - 1);
        }
        else
        {
            temptile = temptile - (((dircountpass1 / 2) - 2) * _boarddim[1]);
            rowchange = rowchange - ((dircountpass1 / 2) - 2);
        }


        dircountpass1++;

        if (dircountpass2 != -1)
        {
            dircountpass2++;

            if (dircountpass1 == MOVEANDDIAGDIR)
            {
                dircountpass1 = 0;
            }
        }
    }


    if (accessflipcount >= 2)
    {
        chokepoint = FLAG;
    }
    else
    {
        chokepoint = UNFLAG;
    }

    return chokepoint;
}

int findbestpath(struct Board _boarddata[], struct Player _playerdata[], int _boarddim[], int currenttile, int _pengnum, int _islenumtested, int pathlength, int _numofguesses, int _ppinfo)
{
    /*
    This is the maze solving algorithm used to determine the most optimal path through a connected group of tiles, as well as
    the number of points that can be obtained. Since the function is recursive and the number of move combinations increases
    exponentially with the number of tiles, it can be problematic if an island is too large or is square shaped (long snaking
    islands have fewer combinations). This is resolved by counting the number of times the function has been called. It is always
    guaranteed to predict at least 4 moves. But, if the number of times it is called exceeds a certain amount, assuming at least
    4 moves have been predicted for every combination, it stops. It then counts all remaining fish on the island and adds that
    number to the current island points (it is functionally identical to the first part of findislands()).
    */

    int _currentid;

    int _temptile, _adjmoves, _tempadjmoves;
    int islemappingtile, _tileislenum, _totnumtiles;
    int dircount, tempdircount, tilesindir, dirnum;

    int newtiletested;

    int currentscore, maxscore, bestnextmove;

    int pathingmoves, maxpathingmoves, resetpathmoves, incrementpathmoves;

    _currentid = getcurrentid(_ppinfo);

    _totnumtiles = _boarddim[0] * _boarddim[1];

    currentscore = getnumoffish(_boarddata[currenttile].intrninfo);
    maxscore = currentscore;

    bestnextmove = -1;


    /*This function changes the values of the number representing the possible adjacent moves of the tiles that are adjacent to
    the current tile tested so that it is not possible to move to a tile twice as findbestpath() moves through the island. This
    effect is reversed right before findbestpath() ends.*/
    changeadjtilemoves(_boarddata, _boarddim, currenttile, -1);


    resetpathmoves = UNFLAG;
    incrementpathmoves = UNFLAG;

    pathingmoves = recursioncounter(resetpathmoves, incrementpathmoves);

    if (_numofguesses == 0)
    {
        maxpathingmoves = 500000;
    }
    else
    {
        maxpathingmoves = 12000;
    }


    dircount = 0;

    if (pathlength < 4 || (pathlength < 5 && _numofguesses == 0) || pathingmoves < maxpathingmoves)
    {
        /*All possible moves up to 6 tiles away are tested (at times jumping tiles results in more total points gained). The
        moves that result in the highest points are saved, as well as the point values.*/
        while (dircount < (MOVEDIRECTIONS * 6))
        {
            tilesindir = 0;

            _temptile = currenttile;
            _adjmoves = getadjmoves(_boarddata[currenttile].intrninfo);

            while (tilesindir <= (dircount / MOVEDIRECTIONS) && _adjmoves > 0)
            {
                dirnum = 0;

                _adjmoves = getadjmoves(_boarddata[_temptile].intrninfo);

                while (dirnum < (dircount % MOVEDIRECTIONS))
                {
                    _adjmoves = _adjmoves / 2;

                    dirnum++;
                }

                _adjmoves = _adjmoves % 2;

                _temptile = shiftonetile(_boarddim, _temptile, (dircount % MOVEDIRECTIONS));

                tilesindir++;
            }

            if (_temptile == -1)
            {
                dircount++;

                continue;
            }

            if (_adjmoves == 1 && (getislandnum(_boarddata[_temptile].intrninfo)) == _islenumtested)
            {
                incrementpathmoves = FLAG;
                pathingmoves = recursioncounter(resetpathmoves, incrementpathmoves);

                currentscore = getnumoffish(_boarddata[currenttile].intrninfo) + findbestpath(_boarddata, _playerdata, _boarddim, _temptile, _pengnum, _islenumtested, (pathlength + 1), _numofguesses, _ppinfo);

                if (currentscore >= maxscore)
                {
                    maxscore = currentscore;

                    bestnextmove = _temptile;
                }
            }

            dircount++;
        }
    }
    else
    {
        struct Board dummyboarddata[_totnumtiles];

        copytodummyboard(dummyboarddata, _boarddata, _boarddim);


        _adjmoves = getadjmoves(_boarddata[currenttile].intrninfo);

        while (dircount < MOVEDIRECTIONS)
        {
            if ((_adjmoves % 2) == 1)
            {
                _temptile = shiftonetile(_boarddim, currenttile, dircount);

                if ((getnumoffish(dummyboarddata[_temptile].intrninfo)) > 0 && (getislandnum(dummyboarddata[_temptile].intrninfo)) == _islenumtested)
                {
                    newtiletested = UNFLAG;

                    _tileislenum = ISLETILEFOUND;
                    dummyboarddata[_temptile].intrninfo = changeislandnum(dummyboarddata[_temptile].intrninfo, _tileislenum);

                    islemappingtile = 0;

                    while (islemappingtile < _totnumtiles || newtiletested == FLAG)
                    {
                        if (islemappingtile == _totnumtiles)
                        {
                            islemappingtile = 0;

                            newtiletested = UNFLAG;
                        }

                        if ((getislandnum(dummyboarddata[islemappingtile].intrninfo)) == ISLETILEFOUND)
                        {
                            newtiletested = FLAG;

                            currentscore = currentscore + getnumoffish(dummyboarddata[islemappingtile].intrninfo);

                            _tempadjmoves = getadjmoves(dummyboarddata[islemappingtile].intrninfo);

                            _tileislenum = OWNISLENUM;
                            dummyboarddata[islemappingtile].intrninfo = changeislandnum(dummyboarddata[islemappingtile].intrninfo, _tileislenum);

                            tempdircount = 0;

                            while (tempdircount < MOVEDIRECTIONS)
                            {
                                if ((_tempadjmoves % 2) == 1)
                                {
                                    _temptile = shiftonetile(_boarddim, islemappingtile, tempdircount);

                                    if ((getislandnum(dummyboarddata[_temptile].intrninfo)) == _islenumtested)
                                    {
                                        _tileislenum = ISLETILEFOUND;
                                        dummyboarddata[_temptile].intrninfo = changeislandnum(dummyboarddata[_temptile].intrninfo, _tileislenum);
                                    }
                                }

                                _tempadjmoves = _tempadjmoves / 2;

                                tempdircount++;
                            }
                        }

                        islemappingtile++;
                    }
                }
            }

            if (currentscore > maxscore)
            {
                maxscore = currentscore;
            }

            currentscore = getnumoffish(_boarddata[currenttile].intrninfo);

            _adjmoves = _adjmoves / 2;

            dircount++;
        }
    }

    /*Changes _adjtiles of all tiles for which it was changed at the beginning of findbestpath() back to to its original
    value.*/
    changeadjtilemoves(_boarddata, _boarddim, currenttile, 1);


    /*Saves the move that results in the most points gained. Since it is overwritten each time the function is called, the final
    value left is the move that should be made from the current tile.*/
    if (bestnextmove >= 0)
    {
        _playerdata[_currentid].pengdata[_pengnum] = changepengbestmove(_playerdata[_currentid].pengdata[_pengnum], bestnextmove);
    }
    else if (bestnextmove == -1)
    {
        _playerdata[_currentid].pengdata[_pengnum] = changepengbestmove(_playerdata[_currentid].pengdata[_pengnum], currenttile);
    }

    return maxscore;
}

void changeadjtilemoves(struct Board _boarddata[], int _boarddim[], int _currenttile, int dirofchange)
{
    /*
    This function changes the values of _adjmoves of tiles surrounding _currenttile so that, as the findbestpath() function
    moves through an island, the same tile is not moved to twice. This operation is reversed before findbestpath() returns a
    value.
    */

    int _temptile, _adjmoves;
    int dircount, tilesindir, dirnum, binproduct;

    dircount = 0;
    tilesindir = 0;

    while ((dircount / MOVEDIRECTIONS) == tilesindir)
    {
        dirnum = 0;

        binproduct = 1;

        _adjmoves = getadjmoves(_boarddata[_currenttile].intrninfo);

        while (dirnum < (dircount % MOVEDIRECTIONS))
        {
            binproduct = binproduct * 2;

            _adjmoves = _adjmoves / 2;

            dirnum++;
        }

        _adjmoves = _adjmoves % 2;

        if ((getnumoffish(_boarddata[_currenttile].intrninfo)) > 0)
        {
            if (_adjmoves == 1 && (dircount % 2) == 0)
            {
                _temptile = _currenttile - ((dircount % MOVEDIRECTIONS) - 1);

                _boarddata[_temptile].intrninfo = _boarddata[_temptile].intrninfo + (((5 - binproduct) * (MAXFISH + ARRAYSHIFT)) * dirofchange);
            }
            else if (_adjmoves == 1 && (dircount % 2) == 1)
            {
                _temptile = _currenttile - (((dircount % MOVEDIRECTIONS) - 2) * _boarddim[1]);

                _boarddata[_temptile].intrninfo = _boarddata[_temptile].intrninfo + (((10 - binproduct) * (MAXFISH + ARRAYSHIFT)) * dirofchange);
            }
        }

        dircount++;
    }
}

int recursioncounter(int _reset, int _increment)
{
    /*
    This function counts the number of times findbestpath() has been called. The static variable is reset each time findbestpath()
    is called for the first time (nonrecursively).
    */

    static int _pathingmoves = 0;

    if (_reset == FLAG)
    {
        _pathingmoves = 0;
    }
    else if (_increment == FLAG)
    {
        _pathingmoves++;
    }

    return _pathingmoves;
}

void getbestpengmovesisles(struct Player _playerdata[], int _boarddim[], int _bestmoveinfo[], int _maxbmoveinfo[], int _boundpenginfo[], int _freedomlevel, int _ppinfo)
{
    /*
    The function findislands() is only capable of analyzing one island at a time. Since each penguin can theoretically be able to
    move to tiles belonging to up to 4 different islands, findislands() cannot determine the best overall move for each penguin.
    Instead, it finds the best move in each direction, and, at the end, getbestpengmoveisles() determines the best move for each
    penguin. This function gets those best moves by testing out all possible penguin and direction combinations. Combinations
    that are known to be incorrect are skipped. The penguin and direction combination that has the most total points (obtained
    by summing the island points of each penguin in the chosen move direction) is chosen as the best overall set of moves. These
    moves are recorded in the structure array playerdata[].

    This function also prevents two or more of the player's penguins that control the same island from moving through the
    island at the same time. Finally, it ensures that, if one penguin's best move is to secure an island with the help of a 2nd
    penguin, that 2nd penguin will make the correct move on the player's next turn.
    */

    int _bestdepttile, _bestdesttile, _aimode, _islepoints, _maxislepoints, totmaxislepoints;

    int _currentid, _pengnum, _bestpengnum, _pengsperplyr;

    int movecombocounter, tempmovecombo;
    int pengcounter, indexnum, tempindex, dircount, passnum;

    int turtislerepeated, openislerepeated, maxbminforeset;


    _currentid = getcurrentid(_ppinfo);
    _pengsperplyr = getpengsperplyr(_ppinfo);

    totmaxislepoints = 0;
    movecombocounter = 0;


    while (movecombocounter < (expon((MOVEDIRECTIONS + ARRAYSHIFT), _pengsperplyr)))
    {
        turtislerepeated = UNFLAG;
        maxbminforeset = UNFLAG;

        _islepoints = 0;
        passnum = 0;

        _pengnum = _pengsperplyr - ARRAYSHIFT;


        while (_pengnum >= 0)
        {
            openislerepeated = 0;

            tempmovecombo = movecombocounter;
            pengcounter = 0;

            while (pengcounter < ((_pengsperplyr - ARRAYSHIFT) - _pengnum))
            {
                tempmovecombo = tempmovecombo / (MOVEDIRECTIONS + ARRAYSHIFT);

                pengcounter++;
            }

            if ((tempmovecombo % (MOVEDIRECTIONS + ARRAYSHIFT)) == 0)
            {
                _pengnum--;

                if (_pengnum < 0 && passnum < 3)
                {
                    _pengnum = _pengsperplyr - ARRAYSHIFT;

                    passnum++;
                }

                continue;
            }
            else
            {
                dircount = (tempmovecombo % (MOVEDIRECTIONS + ARRAYSHIFT)) - ARRAYSHIFT;

                indexnum = (_pengnum * MOVEDIRECTIONS) + dircount;
            }

            if (_bestmoveinfo[indexnum] == 0)
            {
                break;
            }


            if (passnum == 1)
            {
                tempindex = indexnum;

                while (tempindex < (_pengsperplyr * MOVEDIRECTIONS))
                {
                    if ((isneg(_bestmoveinfo[tempindex])) == FLAG && turtislerepeated == UNFLAG && (getallytotislenum(_bestmoveinfo[indexnum])) == (getallytotislenum(_bestmoveinfo[tempindex])))
                    {
                        if ((getallyaimode(_bestmoveinfo[tempindex])) == TURTLE || (getallyaimode(_bestmoveinfo[indexnum])) == TURTLE)
                        {
                            turtislerepeated = FLAG;

                            tempindex = indexnum;
                        }
                        else if ((getallyaimode(_bestmoveinfo[tempindex])) == DEFAULT && (getallyaimode(_bestmoveinfo[indexnum])) == DEFAULT)
                        {
                            openislerepeated++;
                        }
                    }

                    if (turtislerepeated == FLAG && (isneg(_bestmoveinfo[tempindex])) == FLAG)
                    {
                        _bestmoveinfo[tempindex] = _bestmoveinfo[tempindex] * NEGONE;
                    }

                    tempindex++;
                }

                if (turtislerepeated == FLAG)
                {
                    _islepoints = 0;

                    break;
                }

                _bestmoveinfo[indexnum] = _bestmoveinfo[indexnum] * NEGONE;


                if (openislerepeated > 0)
                {
                    if ((getadjustedfishval((getallyislesize(_bestmoveinfo[indexnum]) / (2 * openislerepeated)), _freedomlevel, _ppinfo)) == 0 && (getallyislesize(_bestmoveinfo[indexnum])) > 0)
                    {
                        _islepoints = _islepoints + 1;
                    }
                    else
                    {
                        _islepoints = _islepoints + getadjustedfishval((getallyislesize(_bestmoveinfo[indexnum]) / (2 * openislerepeated)), _freedomlevel, _ppinfo);
                    }
                }
                else
                {
                    if ((getallyaimode(_bestmoveinfo[indexnum])) == DEFAULT)
                    {
                        if ((getadjustedfishval(getallyislesize(_bestmoveinfo[indexnum]), _freedomlevel, _ppinfo)) == 0 && (getallyislesize(_bestmoveinfo[indexnum])) > 0)
                        {
                            _islepoints = _islepoints + 1;
                        }
                        else
                        {
                            _islepoints = _islepoints + getadjustedfishval(getallyislesize(_bestmoveinfo[indexnum]), _freedomlevel, _ppinfo);
                        }
                    }
                    else
                    {
                        _islepoints = _islepoints + getallyislesize(_bestmoveinfo[indexnum]);
                    }
                }
            }
            else if (passnum == 2)
            {
                if ((getallyaimode(_bestmoveinfo[indexnum])) == SECURE && _boundpenginfo[indexnum] > 0)
                {
                    if ((isneg(_bestmoveinfo[(_boundpenginfo[indexnum] - ARRAYSHIFT)])) == UNFLAG)
                    {
                        tempindex = 0;

                        while (tempindex < (_pengsperplyr * MOVEDIRECTIONS))
                        {
                             if ((isneg(_bestmoveinfo[tempindex])) == FLAG)
                            {
                                _bestmoveinfo[tempindex] = _bestmoveinfo[tempindex] * NEGONE;
                            }

                            tempindex++;
                        }

                        _islepoints = 0;

                        break;
                    }
                }
            }
            else if (passnum == 3)
            {
                if (_islepoints >= totmaxislepoints)
                {
                    if (maxbminforeset == UNFLAG)
                    {
                        maxbminforeset = FLAG;

                        tempindex = 0;

                        while (tempindex < (_pengsperplyr * MOVEDIRECTIONS))
                        {
                            _maxbmoveinfo[tempindex] = 0;

                            tempindex++;
                        }
                    }

                    _maxbmoveinfo[indexnum] = _bestmoveinfo[indexnum] * NEGONE;

                    if (_boundpenginfo[indexnum] < 0)
                    {
                        _maxbmoveinfo[indexnum] = changeallybestmove(_maxbmoveinfo[indexnum], getpengtile(_playerdata[_currentid].pengdata[_pengnum]));
                    }
                }

                _bestmoveinfo[indexnum] = _bestmoveinfo[indexnum] * NEGONE;
            }


            _pengnum--;

            if (_pengnum < 0 && passnum < 3)
            {
                _pengnum = _pengsperplyr - ARRAYSHIFT;

                passnum++;
            }
        }

        if (_islepoints >= totmaxislepoints)
        {
            totmaxislepoints = _islepoints;
        }

        movecombocounter++;
    }


    if (totmaxislepoints > 0)
    {
        indexnum = 0;

        while (indexnum < (_pengsperplyr * MOVEDIRECTIONS))
        {
            if (_maxbmoveinfo[indexnum] > 0)
            {
                _bestpengnum = indexnum / MOVEDIRECTIONS;
                _bestdepttile = getpengtile(_playerdata[_currentid].pengdata[_bestpengnum]);
                _bestdesttile = getallybestmove(_maxbmoveinfo[indexnum]);
                _aimode = getallyaimode(_maxbmoveinfo[indexnum]);

                if (_aimode != DEFAULT || _freedomlevel == 3 || _freedomlevel == 4)
                {
                    _maxislepoints = getallyislesize(_maxbmoveinfo[indexnum]);

                    if (_maxislepoints > (MAXISLEPOINTS - ARRAYSHIFT))
                    {
                        _maxislepoints = MAXISLEPOINTS - ARRAYSHIFT;
                    }
                }
                else
                {
                    _maxislepoints = 0;
                }


                _playerdata[_currentid].pengdata[_bestpengnum] = getpengdatasum(_bestdepttile, _bestdesttile, _aimode, _maxislepoints);
            }

            indexnum++;
        }
    }
}

int getadjustedfishval(int _fishonisle, int _freedomlevel, int _ppinfo)
{
    /*
    This function finds and returns the number of island points that a contested island is worth based on how many fish the
    island has. Since more than one player's penguins have access to the island, it is likely that each player will only be
    able to collect a small fraction of all the fish available on the island. As a result, these adjusted values are comparatively
    low. These values are used to determine if holding on to or securing an island is worth more than continuing to collect fish
    in contested areas.

    There are also modifier values that change the amount that fish in contested islands are worth, depending on the board size,
    the number of penguins per player, and the number of players. Since there is less competition and more fish need to be collected
    when there are fewer players, contested islands are worth more points. As a result, islands that the player controls
    must be worth more to be able to compete with the contested islands. This prevents penguins from securing islands that are
    too small and prematurely cutting themselves off from the rest of the board.
    */

    int _startfreetiles, totnumofpengs;
    int bigboardmod, numofplyrsmod, freedomlvlmod;
    int _adjustedfishval;

    _startfreetiles = getstartingfreetiles(_ppinfo);
    totnumofpengs = (getpengsperplyr(_ppinfo)) * (getnumofplyrs(_ppinfo));


    if ((_startfreetiles / totnumofpengs) >= 22 && (getpengsperplyr(_ppinfo)) < 5)
    {
        bigboardmod = 1;

        if ((getnumofplyrs(_ppinfo)) == 2 && (_startfreetiles / totnumofpengs) >= 40)
        {
            numofplyrsmod = 1;
        }
        else if (((getnumofplyrs(_ppinfo)) == 2 && (_startfreetiles / totnumofpengs) >= 28) || (getnumofplyrs(_ppinfo)) == 3)
        {
            numofplyrsmod = 0;
        }
        else
        {
            numofplyrsmod = -1;
        }
    }
    else
    {
        bigboardmod = 0;
        numofplyrsmod = 0;
    }


    if (_freedomlevel == 3 && (getnumofplyrs(_ppinfo)) > 3)
    {
        if (bigboardmod == 1)
        {
            bigboardmod = 0;
            numofplyrsmod = 0;
            freedomlvlmod = 0;
        }
        else
        {
            freedomlvlmod = -1;
        }
    }
    else if (_freedomlevel == 4)
    {
        freedomlvlmod = -1;
        bigboardmod = 0;
        numofplyrsmod = 0;
    }
    else
    {
        freedomlvlmod = 0;
    }


    if (_fishonisle > 0 && _fishonisle <= 5)
    {
        _adjustedfishval = _fishonisle / 2;
    }
    else if (_fishonisle > 5 && _fishonisle <= 10)
    {
        _adjustedfishval = (_fishonisle * 3) / 7;
    }
    else if (_fishonisle > 10 && _fishonisle <= 15)
    {
        _adjustedfishval = (_fishonisle * 3) / 8;
    }
    else if (_fishonisle > 15 && _fishonisle <= 20)
    {
        _adjustedfishval = _fishonisle / 3;
    }
    else if (_fishonisle > 20 && _fishonisle <= 30)
    {
        _adjustedfishval = 7 + (freedomlvlmod * 1);
    }
    else if (_fishonisle > 30 && _fishonisle <= 45)
    {
        _adjustedfishval = 8 + (freedomlvlmod * 1);
    }
    else if (_fishonisle > 45 && _fishonisle <= 55)
    {
        _adjustedfishval = 9 + (freedomlvlmod * 1);
    }
    else if (_fishonisle > 55 && _fishonisle <= 65)
    {
        _adjustedfishval = 9 + (bigboardmod * 1) + (freedomlvlmod * 1);
    }
    else if (_fishonisle > 65 && _fishonisle <= 80)
    {
        _adjustedfishval = 10 + (bigboardmod * 1) + (freedomlvlmod * 1);
    }
    else if (_fishonisle > 80 && _fishonisle <= 100)
    {
        _adjustedfishval = 10 + (bigboardmod * 2) + (numofplyrsmod * 1) + (freedomlvlmod * 1);
    }
    else if (_fishonisle > 100 && _fishonisle <= 120)
    {
        _adjustedfishval = 11 + (bigboardmod * 2) + (numofplyrsmod * 1) + (freedomlvlmod * 1);
    }
    else if (_fishonisle > 120 && _fishonisle <= 150)
    {
        _adjustedfishval = 11 + (bigboardmod * 3) + (numofplyrsmod * 2) + (freedomlvlmod * 1);
    }
    else if (_fishonisle > 150 && _fishonisle <= 185)
    {
        _adjustedfishval = 12 + (bigboardmod * 3) + (numofplyrsmod * 2) + (freedomlvlmod * 1);
    }
    else if (_fishonisle > 185 && _fishonisle <= 225)
    {
        _adjustedfishval = 12 + (bigboardmod * 4) + (numofplyrsmod * 2) + (freedomlvlmod * 1);
    }
    else if (_fishonisle > 225 && _fishonisle <= 280)
    {
        _adjustedfishval = 13 + (bigboardmod * 4) + (numofplyrsmod * 2) + (freedomlvlmod * 1);
    }
    else if (_fishonisle > 280 && _fishonisle <= 350)
    {
        _adjustedfishval = 13 + (bigboardmod * 5) + (numofplyrsmod * 3) + (freedomlvlmod * 1);
    }
    else if (_fishonisle > 350 && _fishonisle <= 440)
    {
        _adjustedfishval = 14 + (bigboardmod * 5) + (numofplyrsmod * 3) + (freedomlvlmod * 1);
    }
    else if (_fishonisle > 440 && _fishonisle <= 550)
    {
        _adjustedfishval = 14 + (bigboardmod * 6) + (numofplyrsmod * 3) + (freedomlvlmod * 1);
    }
    else if (_fishonisle > 550 && _fishonisle <= 700)
    {
        _adjustedfishval = 15 + (bigboardmod * 6) + (numofplyrsmod * 3) + (freedomlvlmod * 1);
    }
    else if (_fishonisle > 700 && _fishonisle <= 900)
    {
        _adjustedfishval = 15 + (bigboardmod * 7) + (numofplyrsmod * 4) + (freedomlvlmod * 1);
    }
    else if (_fishonisle > 900)
    {
        _adjustedfishval = 16 + (bigboardmod * 7) + (numofplyrsmod * 4) + (freedomlvlmod * 1);
    }
    else
    {
        _adjustedfishval = 0;
    }


    return _adjustedfishval;
}

int getbestmoveindex(int _boarddim[], int _depttile, int _desttile, int _pengnum)
{
    /*
    This function gets the index of the arrays bestmoveinfo[], maxbmoveinfo[], boundpenginfo[] based on _pengnum, _depttile,
    and _desttile.
    */

    int _bestmoveindex;

    _bestmoveindex = (_pengnum * MOVEDIRECTIONS) + (getmovedirection(_boarddim, _depttile, _desttile));

    return _bestmoveindex;
}

int getmovedirection(int _boarddim[], int _depttile, int _desttile)
{
    /*
    This function gets direction of a move made going from _depttile to _desttile.
    */

    int dircount;


    if ((_desttile - _depttile) < _boarddim[1] && (_desttile - _depttile) > 0)
    {
        dircount = 0;
    }
    else if ((_desttile - _depttile) >= _boarddim[1])
    {
        dircount = 1;
    }
    else if ((_desttile - _depttile) > (_boarddim[1] * NEGONE) && (_desttile - _depttile) < 0)
    {
        dircount = 2;
    }
    else if ((_desttile - _depttile) <= (_boarddim[1] * NEGONE))
    {
        dircount = 3;
    }
    else
    {
        dircount = -1;
    }


    return dircount;
}

void copytodummyboard(struct Board _dummyboarddata[], struct Board _boarddata[], int _boarddim[])
{
    /*
    This functions copies the structure array boarddata[] into a new temporary structure array.
    */

    int _tilecount, _totnumtiles;
    int dircount;

    _totnumtiles = _boarddim[0] * _boarddim[1];

    _tilecount = 0;

    while (_tilecount < _totnumtiles)
    {
        dircount = 0;

        _dummyboarddata[_tilecount].intrninfo = _boarddata[_tilecount].intrninfo;

        while (dircount < MOVEDIRECTIONS)
        {
            _dummyboarddata[_tilecount].extinfo[dircount] = _boarddata[_tilecount].extinfo[dircount];

            dircount++;
        }

        _tilecount++;
    }
}

void copytodummyplayer(struct Player _dummyplayerdata[], struct Player _playerdata[], int _ppinfo)
{
    /*
    This functions copies the structure array playerdata[] into a new temporary structure array.
    */

    int _numofplyrs, _pengsperplyr;
    int _plyrnum, _pengnum;

    _numofplyrs = getnumofplyrs(_ppinfo);
    _pengsperplyr = getpengsperplyr(_ppinfo);

    _plyrnum = 0;

    while (_plyrnum < _numofplyrs)
    {
        _pengnum = 0;

        strcpy(_dummyplayerdata[_plyrnum].plyrname, _playerdata[_plyrnum].plyrname);

        _dummyplayerdata[_plyrnum].score = _playerdata[_plyrnum].score;
        _dummyplayerdata[_plyrnum].pengsleft = _playerdata[_plyrnum].pengsleft;

        while (_pengnum < _pengsperplyr)
        {
            _dummyplayerdata[_plyrnum].pengdata[_pengnum] = _playerdata[_plyrnum].pengdata[_pengnum];

            _pengnum++;
        }

        _plyrnum++;
    }
}






