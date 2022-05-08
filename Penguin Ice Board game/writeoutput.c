#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

#include "univfuncs.h"
#include "structini.h"
#include "writeoutput.h"


void outputboardtxt(struct Board _boarddata[], struct Player _playerdata[], int _boarddim[], char _outboardfile[], int _ppinfo)
{
    /*
    This function opens board.txt as write. The  functions printboarddim(), printboardvalues(), and printscoreboard() then write
    to the .txt file, after which board.txt is closed.
    */

    FILE *pubboardout = fopen(_outboardfile, "w");

    printboarddim(pubboardout, _boarddim);
    printboardvalues(pubboardout, _boarddata, _boarddim, _ppinfo);
    printscoreboard(pubboardout, _playerdata, _ppinfo);

    fclose(pubboardout);
}

void printboarddim(FILE *_pubboardout, int _boarddim[])
{
    /*
    This function copies the board dimensions from boarddim[] to board.txt.
    */

    fprintf(_pubboardout, "%d %d\n", _boarddim[0], _boarddim[1]);
}

void printboardvalues(FILE *_pubboardout, struct Board _boarddata[], int _boarddim[], int _ppinfo)
{
    /*
    This function uses the structure array boarddata[].intrninfo to get the number of fish and the player id on each tile.
    Those values are then copied to board.txt using the proper format.
    */

    int _tilecount, _totnumtiles, _pengsperplyr;
    int _fish, _ipengid;
    int _plyrid;

    _totnumtiles = _boarddim[0] * _boarddim[1];
    _pengsperplyr = getpengsperplyr(_ppinfo);

    _tilecount = 0;

    while (_tilecount < _totnumtiles)
    {
        _fish = getnumoffish(_boarddata[_tilecount].intrninfo);
        _ipengid = getintrnpengid(_boarddata[_tilecount].intrninfo);

        _plyrid = (_ipengid + _pengsperplyr) / _pengsperplyr;

        fprintf(_pubboardout, "%d%d", _fish, _plyrid);

        _tilecount++;

        if ((_tilecount % _boarddim[1]) == 0)
        {
            fprintf(_pubboardout, "\n");
        }
        else
        {
            fprintf(_pubboardout, " ");
        }
    }
}

void printscoreboard(FILE *_pubboardout, struct Player _playerdata[], int _ppinfo)
{
    /*
    This function copies the values of the structure array _playerdata[].plyrname and _playerdata[_plyrnum].score into board.txt.
    */

    int _plyrnum, _numofplyrs;

    _numofplyrs = getnumofplyrs(_ppinfo);

    _plyrnum = 0;

    while (_plyrnum < _numofplyrs)
    {
        fprintf(_pubboardout, "%s %d %d", _playerdata[_plyrnum].plyrname, (_plyrnum + ARRAYSHIFT), _playerdata[_plyrnum].score);

        _plyrnum++;

        if (_plyrnum < _numofplyrs) //After the last player's info is copied into board.txt, the file ends (there is no new line).
        {
            fprintf(_pubboardout, "\n");
        }
    }
}

void outputprivtxt(char _stringid[], int _phase, int _movestatus, int _ppinfo)
{
    /*
    This function opens ppinfo.txt and copies the value of ppinfo into it as well as the player's ID string. If it is the
    movement phase, ppinfo is made negative before it is copied into ppinfo.txt. At the end, ppinfo.txt is closed.
    */

    FILE *privtxtout = fopen("ppinfo.txt", "w");

    if (_phase == MOVEMENT)
    {
        _ppinfo = _ppinfo * NEGONE;
    }

    fprintf(privtxtout, "%d %s", _ppinfo, _stringid);

    fclose(privtxtout);
}


