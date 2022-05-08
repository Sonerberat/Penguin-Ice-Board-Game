#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

#include "univfuncs.h"
#include "structini.h"
#include "displayui.h"


void displaygameboard(struct Board _boarddata[], int _boarddim[], int _ppinfo)
{
    /*
    This function prints out a slightly modified, more user friendly version of board.txt onto the screen. The board is
    formatted in such a way that each tile is separated by a space in every direction. Each tile consists of 4 characters
    arranged in a square shape. The bottom 2 characters are letters that represent the row/column of the tile. To choose a tile,
    the player inputs the letter combination (with no spaces) that is seen on the desired tile. The top 2 characters display the
    number of fish on the tile if it is unoccupied, and a player's symbol (used in place of a player id num) if the tile is
    occupied. Since a tile will never be both occupied by a penguin and still have fish on it, displaying the number of fish on
    a tile that is occupied by a penguin is unnecessary. The number of fish/player symbol on each tile is printed twice for
    aesthetics reasons. When a player moves out of a tile and the ice breaks through, that tile's 4 characters all become dashes.
    This makes it much easier for the user to see the state of the board at any given time.
    */

    int _tilecount, _totnumtiles, _pengsperplyr;
    int _fish, _ipengid;
    int _plyrid;

    int _rownum, _colnum, rowcharnum, colcharnum, plyrsymbnum;

    int passnum;

    /*
    Since the same set of tile numbers is used to print the 2 lines of characters and spaces that make up each row of tiles
    displayed, and printing to the screen can only be done from left to right, one line at a time, every other time the tile
    number reaches the last column, it must return to the first column of the same row instead of continuing to the next row.
    passnum is used to determine if the top or bottom row is printed and whether or not _tilecount returns to the beginning of
    the row after it gets to the last column.
    */

    _totnumtiles = _boarddim[0] * _boarddim[1];
    _pengsperplyr = getpengsperplyr(_ppinfo);

    _tilecount = 0;
    passnum = 0;

    while (_tilecount < _totnumtiles)
    {
        _rownum = getrownum(_boarddim, _tilecount);
        _colnum = getcolnum(_boarddim, _tilecount);

        rowcharnum = gettileletterval(_rownum); //finds the letter corresponding to _rownum
        colcharnum = gettileletterval(_colnum); //finds the letter corresponding to _colnum

        _fish = getnumoffish(_boarddata[_tilecount].intrninfo);

        _ipengid = getintrnpengid(_boarddata[_tilecount].intrninfo);
        _plyrid = (_ipengid + _pengsperplyr) / _pengsperplyr;
        plyrsymbnum = getplyrsymbol(_plyrid);

        if (passnum == 0) //when passnum is 0, the top row is printed.
        {
            if (_plyrid > 0)
            {
                printf("%c%c", plyrsymbnum, plyrsymbnum);
            }
            else if (_fish > 0)
            {
                printf("%d%d", _fish, _fish);
            }
            else
            {
                printf("%c%c", HYPHEN, HYPHEN);
            }
        }
        else //when passnum is 0, the bottom row is printed
        {
            if (_plyrid > 0 || _fish > 0)
            {
                printf("%c%c", rowcharnum, colcharnum);
            }
            else
            {
                printf("%c%c", HYPHEN, HYPHEN);
            }
        }

        _tilecount++;

        if ((_tilecount % _boarddim[1]) == 0) //end of the row
        {
            if (passnum == 0)
            {
                printf("\n");

                _tilecount = _tilecount - _boarddim[1]; //_tilecount goes back to the beginning of the row if passnum is 0.

                passnum = 1; //passnum alternates between 0 and 1
            }
            else //tilenum increments normally if passnum is 1.
            {
                printf("\n\n");

                passnum = 0; //passnum alternates
            }
        }
        else
        {
            printf("  ");
        }
    }
}

void displayscoreboard(struct Player _playerdata[], int _ppinfo)
{
    /*
    This function displays the scoreboard, much like how board.txt does. In place of the player id number, the player symbol is
    displayed twice, allowing the user to quickly identify their penguins. Symbols are used to mark the penguins since they are
    much easier to identify.
    */

    int _plyrnum, _plyrid, _numofplyrs, plyrsymbnum;

    _numofplyrs = getnumofplyrs(_ppinfo);

    _plyrnum = 0;

    while (_plyrnum < _numofplyrs)
    {
        _plyrid = _plyrnum + ARRAYSHIFT;
        plyrsymbnum = getplyrsymbol(_plyrid); //gets the ASCII value of the symbol assigned to the player id inputted

        printf("%s  %c%c  %d", _playerdata[_plyrnum].plyrname, plyrsymbnum, plyrsymbnum, _playerdata[_plyrnum].score);

        _plyrnum++;

        printf("\n");
    }

    printf("\n\n");
}

int gettileletterval(int dimnum)
{
    /*This function returns the ASCII value of the letter that corresponds to the dimension inputted.*/

    int letterval;

    if (dimnum >= 0 && dimnum <= 25) //for lower dimensions use lowercase letters
    {
            letterval = dimnum + 97;
    }
    else if (dimnum >= 26 && dimnum <= 51) //for higher dimensions use uppercase letters
    {
            letterval = dimnum + 39;
    }

    return letterval;
}

int getplyrsymbol(int _plyrid)
{
    /*This function returns the ASCII value of the symbol that is assigned to the player id inputted.*/

    int numofsymb;

    if (_plyrid == 0)
    {
        numofsymb = 0;
    }
    else if (_plyrid == 1)
    {
        numofsymb = 36;
    }
    else if (_plyrid == 2)
    {
        numofsymb = 38;
    }
    else if (_plyrid == 3)
    {
        numofsymb = 63;
    }
    else if (_plyrid == 4)
    {
        numofsymb = 125;
    }
    else if (_plyrid == 5)
    {
        numofsymb = 35;
    }
    else if (_plyrid == 6)
    {
        numofsymb = 92;
    }
    else if (_plyrid == 7)
    {
        numofsymb = 64;
    }
    else if (_plyrid == 8)
    {
        numofsymb = 62;
    }
    else if (_plyrid == 9)
    {
        numofsymb = 33;
    }

    return numofsymb;
}
