#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

#include "univfuncs.h"
#include "structini.h"
#include "plyrturn.h"
#include "sharedalgs.h"


int playerturn(struct Board _boarddata[], struct Player _playerdata[], int _boarddim[], int phase, int _ppinfo)
{
    /*
    This function checks if the player can make a move if it is the movement phase or place a penguin if it is the placement
    phase. If the player cannot make a move or place a penguin, the function returns _playstatus = 1. Otherwise, the function
    calls on one of two functions (depending on the phase), which prompt the player to input a move.
    */

    int _playstatus, currentid, pengsperplyr;

    currentid = getcurrentid(_ppinfo);
    pengsperplyr = getpengsperplyr(_ppinfo);

    _playstatus = 3;

    if ((phase == PLACEMENT && (countplyrpenguins(_playerdata, currentid, _ppinfo)) == pengsperplyr) || (phase == MOVEMENT && _playerdata[currentid].pengsleft == 0))
    {
        printf("%s cannot take any actions this turn.\n\n", _playerdata[currentid].plyrname);

        _playstatus = 1;

        return _playstatus;
    }
    else
    {
        if (phase == PLACEMENT)
        {
            _playstatus = playerinptplace(_boarddata, _playerdata, _boarddim, _ppinfo);

            printf("\n\n%s placed a penguin.\n\n\n", _playerdata[currentid].plyrname);
        }
        else if (phase == MOVEMENT)
        {
            _playstatus = playerinptmove(_boarddata, _playerdata, _boarddim, _ppinfo);

            printf("\n\n%s moved a penguin.\n\n\n", _playerdata[currentid].plyrname);
        }
    }


    if (_playstatus == 3)
    {
        printf("Error: An unexpected error was encountered.\n");
    }

    return _playstatus;
}

int playerinptplace(struct Board _boarddata[], struct Player _playerdata[], int _boarddim[], int _ppinfo)
{
    /*
    This function asks the user to place a penguin by inputting letters corresponding to a tile on the board. The checkinptval()
    function then accepts the value(s) typed in and checks for the validity of the input. If the input is found to be invalid,
    checkinptval() returns -1, and an error message is displayed, prompting the user to enter a different value. If the
    input is found to be valid (it corresponds to a tile that exists), the checkplcmttile() function checks to see if a penguin
    can be placed on that tile. If a penguin cannot be placed on the tile chosen, the function returns -1, and the user is asked
    to input a different value. If the tile chosen is valid, the placepenguin() function places the player's penguin on the tile
    chosen.
    */

    int placestatus, desttile;
    int invalidplay;

    invalidinputplace:

    printf("%s, select a tile to place your penguin on:\n\n", _playerdata[getcurrentid(_ppinfo)].plyrname);

    desttile = checkinptval(_boarddim);

    if (desttile == -1)
    {
        printf("Value entered is invalid. ");

        goto invalidinputplace;
    }
    else
    {
        invalidplay = checkplcmttile(_boarddata, _playerdata, desttile, _ppinfo);

        if (invalidplay == FLAG)
        {
            printf("Value entered is invalid. ");

            goto invalidinputplace;
        }
        else
        {
            placestatus = placepenguin(_boarddata, _playerdata, desttile, _ppinfo);
        }
    }

    return placestatus;
}

int playerinptmove(struct Board _boarddata[], struct Player _playerdata[], int _boarddim[], int _ppinfo)
{
    /*
    This function is very similar to the playerinptplace() function. The key difference is that the user is asked for two
    separate inputs (the departure tile and destination tile). The player is not asked for the 2nd input value until it is
    confirmed that the 1st input value is valid by both the checkinptval() and checkdepttile() functions. If both input
    values are found to be valid, the movepenguin() function moves the chosen penguin to the chosen tile.
    */

    int movestatus, depttile, desttile;
    int invalidplay;

    invalidinputdept:

    printf("%s, select a tile to move your penguin from:\n\n", _playerdata[getcurrentid(_ppinfo)].plyrname);

    depttile = checkinptval(_boarddim);

    if (depttile == -1)
    {
        printf("Value entered is invalid. ");

        goto invalidinputdept;
    }
    else
    {
        invalidplay = checkdepttile(_boarddata, _playerdata, depttile, _ppinfo);

        if (invalidplay == FLAG)
        {
            printf("Value entered is invalid. ");

            goto invalidinputdept;
        }
    }


    invalidinputdest:

    printf("%s, select a tile to move your penguin to:\n\n", _playerdata[getcurrentid(_ppinfo)].plyrname);

    desttile = checkinptval(_boarddim);

    if (desttile == -1)
    {
        printf("Value entered is invalid. ");

        goto invalidinputdest;
    }
    else
    {
        invalidplay = checkdesttile(_boarddata, _playerdata, depttile, desttile, _ppinfo);

        if (invalidplay == FLAG)
        {
            printf("Value entered is invalid. ");

            goto invalidinputdest;
        }
        else
        {
            movestatus = movepenguin(_boarddata, _playerdata, depttile, desttile, _ppinfo);
        }
    }

    return movestatus;
}

int checkinptval(int _boarddim[])
{
    /*
    This function accepts a string input and checks if the inputted value corresponds to a tile number that exists on the board.
    If the input is valid, the tile number (tilenum) corresponding to the inputted value is returned. Otherwise, -1 is returned.
    */

    int tilenum, readcounter;
    int tilecoords[2];
    int tempchar;
    char tileinpt[4]; //only 3 characters (not including '\0') are recorded

    readcounter = 0;

    fgets(tileinpt, sizeof tileinpt, stdin); //the inputted value is saved to tileinpt

    printf("\n");

    if (tileinpt[2] == '\n') //The input must have only 2 characters (no spaces) to be valid. So, the 3rd character must be '\n'.
    {
        while (readcounter < 2) //there are 2 indices in tileinpt[] to be checked
        {
            if (tileinpt[readcounter] >= 65 && tileinpt[readcounter] <= 90) //checks to see if the character is uppercase
            {
                //converts the char into an integer which corresponds to the row/column number
                tilecoords[readcounter] = tileinpt[readcounter] - 39;
            }
            else if (tileinpt[readcounter] >= 97 && tileinpt[readcounter] <= 122) //checks to see if the character is lowercase
            {
                tilecoords[readcounter] = tileinpt[readcounter] - 97;
            }
            else
            {
                tilenum = -1; //if any of the inputted characters is not a lowercase or uppercase letter, the input is invalid

                return tilenum;
            }

            if (tilecoords[readcounter] >= _boarddim[readcounter])
            {
                /*If the row/column obtained from the input is greater than or equal to the number of rows/columns the board has,
                the input is invalid (the row/column numbers from the input start at 0).*/
                tilenum = -1;

                return tilenum;
            }

            readcounter++;
        }
    }
    else if (tileinpt[2] == '\0')
    {
        /*If the 3rd character is '\0', the 2nd character is '\n'. Since 2 letters need to be inputted to choose a tile, the
        input is invalid.*/
        tilenum = -1;

        return tilenum;
    }
    else
    {
        while ((tempchar = getchar()) != '\n' && tempchar != EOF)
        {
            //if too many characters are inputted by the user, this clears any extra saved characters that may interfere with
            //future inputs.
        }

        tilenum = -1; //input is invalid since too many characters were inputted

        return tilenum;
    }

    tilenum = gettilenum(_boarddim, tilecoords[0], tilecoords[1]); //tilenum is obtained from the inputted values

    return tilenum;
}
