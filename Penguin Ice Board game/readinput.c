#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

#include "univfuncs.h"
#include "structini.h"
#include "readinput.h"


int getsavedppinfo(char _stringid[], int _phase)
{
    /*
    This function opens and reads the file that contains ppinfo. If the file does not exist or if the value of ppinfo is
    negative AND it is the placement phase, the value of ppinfo is set to 0. Otherwise, ppinfo takes on the value in the
    .txt file. ppinfo is then returned.
    */

    int _ppinfo;
    char ppstr[15];
    char tempstringid[MAXNAMELEN];

    FILE *privtxtin = fopen("ppinfo.txt", "r");

    if (privtxtin == NULL)
    {
        _ppinfo = 0;

        return _ppinfo;
    }
    else
    {
        fscanf(privtxtin, "%s", ppstr);
        _ppinfo = atoi(ppstr); //the value saved to the .txt file is a string and must be converted to an integer.

        fscanf(privtxtin, "%s", tempstringid);

        fclose(privtxtin);

        if (_ppinfo > 0)
        {
            strcpy(_stringid, tempstringid);

            return _ppinfo;
        }
        else if (_ppinfo < 0 && _phase == MOVEMENT)
        {
            _ppinfo = _ppinfo * NEGONE;

            strcpy(_stringid, tempstringid);

            return _ppinfo;
        }
        else
        {
            _ppinfo = 0;

            return _ppinfo;
        }
    }
}

int getboarddimensions(FILE *_pubboardin, int _boarddim[])
{
    /*
    This function reads the first line of board.txt and saves the values do the array _boarddim[2]. An error value is returned
    if an error is encountered when reading the file.
    */

    int readstate, filereadfinish;

    char rowstr[15];
    char colstr[15];

    filereadfinish = UNFLAG;


    readstate = fscanf(_pubboardin, "%s", rowstr);

    if (readstate == EOF)
    {
        EOFerror(filereadfinish);

        return 1;
    }

    readstate = fscanf(_pubboardin, "%s", colstr);

    if (readstate == EOF)
    {
        EOFerror(filereadfinish);

        return 1;
    }

    if ((strlen(rowstr)) > 2 || (strlen(colstr)) > 2)
    {
        printf("Error: One or more of the board dimension values in the game state file have too many characters.\n");

        return 1;
    }
    else if ((isnumber(rowstr)) == UNFLAG || (isnumber(colstr)) == UNFLAG)
    {
        printf("Error: One or more of the board dimension values in the game state file have invalid characters.\n");

        return 1;
    }


    _boarddim[0] = atoi(rowstr);
    _boarddim[1] = atoi(colstr);


    if (_boarddim[0] < MINDIMENSION || _boarddim[1] < MINDIMENSION)
    {
        printf("Error: One or more of the board dimension values in the game state file are too small.\n");

        return 1;
    }
    else if (_boarddim[0] > MAXDIMENSION || _boarddim[1] > MAXDIMENSION)
    {
        printf("Error: One or more of the board dimension values in the game state file are too large.\n");

        return 1;
    }


    return 0;
}

int gettempboardvalues(FILE *_pubboardin, int _tempboard[], int _totnumtiles)
{
    /*
    This function reads and copies the value of each tile in board.txt to the array _tempboarddata[].
    */

    int tilenum;
    int readstate, filereadfinish;

    char tilevalstr[15];

    filereadfinish = UNFLAG;

    tilenum = 0;

    while (tilenum < _totnumtiles)
    {
        readstate = fscanf(_pubboardin, "%s", tilevalstr);

        if (readstate == EOF)
        {
            EOFerror(filereadfinish);

            return 1;
        }
        else if ((strlen(tilevalstr)) > 2)
        {
            printf("Error: One or more of the board's tile values in the game state file have too many characters.\n");

            return 1;
        }
        else if ((isnumber(tilevalstr)) == UNFLAG)
        {
            printf("Error: One or more of the board's tile values in the game state file have invalid characters.\n");

            return 1;
        }


        _tempboard[tilenum] = atoi(tilevalstr);


        if (_tempboard[tilenum] < 0 || (_tempboard[tilenum] > 10 && _tempboard[tilenum] != 20 && _tempboard[tilenum] != 30))
        {
            printf("Error: One or more of the board's tile values in the game state file have invalid values.\n");

            return 1;
        }


        tilenum++;
    }


    return 0;
}

int getscoreboard(FILE *_pubboardin, struct Player _playerdata[], char _stringid[], int _strinptest, int _possiblereset, int _ppinfo)
{
    /*
    This function reads and copies the bottom values in board.txr to the structure array _playerdata[]. Additionally, if
    _strinptest is equal to FLAG (it is the player's first turn) and all the values in board.txt have been copied, the function
    prompts the user to enter a name, which is saved to _playerdata[].plyrname. An error value is returned if an error is
    encountered when reading board.txt.
    */

    int _numofplyrs, currentid;
    int _startfreetiles, maxpointspossible;
    int checkplyrid, checkstringid, getnameflag;
    int readstate, filereadfinish;

    static int plyrnum = 0;

    char scrbrdstr[15];


    _numofplyrs = getnumofplyrs(_ppinfo);
    _startfreetiles = getstartingfreetiles(_ppinfo);

    maxpointspossible = _startfreetiles * MAXFISH;

    checkstringid = UNFLAG;
    filereadfinish = UNFLAG;
    getnameflag = UNFLAG;


    while (plyrnum < _numofplyrs)
    {
        if (_strinptest == FLAG && plyrnum == (_numofplyrs - ARRAYSHIFT))
        {
            getnameflag = FLAG;
        }
        else
        {
            readstate = fscanf(_pubboardin, "%s", _playerdata[plyrnum].plyrname); //reads a player's string id


            if (readstate == EOF)
            {
                if (_possiblereset == FLAG && _strinptest == UNFLAG && checkstringid == UNFLAG)
                {
                    return -1;
                }
                else
                {
                    EOFerror(filereadfinish);

                    return 1;
                }
            }
            else if ((strlen(_playerdata[plyrnum].plyrname)) > (MAXNAMELEN - 2))
            {
                printf("Error: One or more player ID in the game state file has too many characters.\n");

                return 1;
            }

            if (_strinptest == UNFLAG && (strcmp(_playerdata[plyrnum].plyrname, _stringid)) == 0)
            {
                checkstringid++;

                currentid = plyrnum;
            }


            readstate = fscanf(_pubboardin, "%s", scrbrdstr); //reads a player's id number


            if (readstate == EOF)
            {
                EOFerror(filereadfinish);

                return 1;
            }
            else if ((strlen(scrbrdstr)) > 1)
            {
                printf("Error: One or more player number values in the game state file have too many characters.\n");

                return 1;
            }
            else if ((isnumber(scrbrdstr)) == UNFLAG)
            {
                printf("Error: One or more player number values in the game state file have invalid characters.\n");

                return 1;
            }

            checkplyrid = atoi(scrbrdstr);

            if (checkplyrid != (plyrnum + ARRAYSHIFT) || checkplyrid > MAXNUMPLYRS)
            {
                printf("Error: One or more player number values in the game state file are incorrect.\n");

                return 1;
            }


            readstate = fscanf(_pubboardin, "%s", scrbrdstr); //reads a player's score


            if (readstate == EOF)
            {
                EOFerror(filereadfinish);

                return 1;
            }
            else if ((strlen(scrbrdstr)) > 4)
            {
                printf("Error: One or more score values in the game state file have too many characters.\n");

                return 1;
            }
            else if ((isnumber(scrbrdstr)) == UNFLAG)
            {
                printf("Error: One or more score values in the game state file have invalid characters.\n");

                return 1;
            }

            _playerdata[plyrnum].score = atoi(scrbrdstr);

            if (_playerdata[plyrnum].score < MINSCORE)
            {
                printf("Error: One or more score values in the game state file are too low.\n");

                return 1;
            }
            else if (_playerdata[plyrnum].score > maxpointspossible)
            {
                printf("Error: One or more score values in the game state file are too high.\n");

                return 1;
            }
        }

        plyrnum++;
    }


    if (_possiblereset == FLAG && _strinptest == UNFLAG && checkstringid == UNFLAG)
    {
        return -1;
    }


    filereadfinish = FLAG;

    readstate = fscanf(_pubboardin, "%s", scrbrdstr);


    if (readstate != EOF)
    {
        EOFerror(filereadfinish);

        return 1;
    }
    else if (_strinptest == UNFLAG && checkstringid == UNFLAG)
    {
        printf("Error: Player's ID (%s) not found in game state file.\n", _stringid);

        return 1;
    }
    else if (_strinptest == UNFLAG && checkstringid == FLAG && currentid != (getcurrentid(_ppinfo)))
    {
        printf("Error: Player's number does not match Player's ID (%s) in game state file.\n", _stringid);

        return 1;
    }
    else if (_strinptest == UNFLAG && checkstringid > FLAG)
    {
        printf("Error: Multiple instances of Player ID (%s) found in game state file.\n", _stringid);

        return 1;
    }


    if (getnameflag == FLAG)
    {
        getteamname(_playerdata, (_numofplyrs - ARRAYSHIFT));

        _playerdata[_numofplyrs - ARRAYSHIFT].score = 0;
    }


    return 0;
}

void getteamname(struct Player _playerdata[], int _plyrnum)
{
    /*
    Prompts the user to create a team name that has a maximum of 50 characters, no spaces, and is not already in use by
    another player. In autonomous mode, the name is automatically chosen.
    */

    int charcounter, plyrcounter;
    int tempchar;


    #ifdef INTERACTIVE

    invalidnameinpt:

    printf("Enter a team name using any combination of characters, numbers, and symbols \nnot currently used by another team (max 50 chars and no spaces):\n\n");

    fgets(_playerdata[_plyrnum].plyrname, sizeof _playerdata[_plyrnum].plyrname, stdin);
    printf("\n\n");


    if (_playerdata[_plyrnum].plyrname[0] == '\n')
    {
        printf("Invalid input. Name must be at least one character long.\n");

        goto invalidnameinpt;
    }

    charcounter = 0;

    while (charcounter < MAXNAMELEN && _playerdata[_plyrnum].plyrname[charcounter] != '\0')
    {
        if (charcounter == (MAXNAMELEN - ARRAYSHIFT - 1) && _playerdata[_plyrnum].plyrname[charcounter] != '\n')
        {
            while ((tempchar = getchar()) != '\n' && tempchar != EOF)
            {
                //if too many characters are inputted by the user, this clears any extra saved characters that may interfere with
                //future inputs.
            }

            printf("Invalid input. Name entered is too long.\n");

            goto invalidnameinpt;
        }
        else if ((_playerdata[_plyrnum].plyrname[charcounter] >= 33 && _playerdata[_plyrnum].plyrname[charcounter] <= 126) || _playerdata[_plyrnum].plyrname[charcounter] == '\n')
        {
            if (_playerdata[_plyrnum].plyrname[charcounter] == '\n')
            {
                _playerdata[_plyrnum].plyrname[charcounter] = '\0';
            }

            charcounter++;
        }
        else
        {
            while (charcounter < MAXNAMELEN && _playerdata[_plyrnum].plyrname[charcounter] != '\0')
            {
                if (charcounter == (MAXNAMELEN - ARRAYSHIFT - 1) && _playerdata[_plyrnum].plyrname[charcounter] != '\n')
                {
                    while ((tempchar = getchar()) != '\n' && tempchar != EOF)
                    {
                        //if too many characters are inputted by the user, this clears any extra saved characters that may interfere with
                        //future inputs.
                    }

                    printf("Invalid input. Name entered contains forbidden character(s) and is too long.\n");

                    goto invalidnameinpt;
                }

                charcounter++;
            }

            printf("Invalid input. Name entered contains forbidden character(s).\n");

            goto invalidnameinpt;
        }
    }

    plyrcounter = 0;

    while (plyrcounter < _plyrnum)
    {
        if ((strcmp(_playerdata[plyrcounter].plyrname, _playerdata[_plyrnum].plyrname)) == 0)
        {
            printf("Invalid input. Name entered already taken.\n");

            goto invalidnameinpt;
        }

        plyrcounter++;
    }

    #else

    strcpy(_playerdata[_plyrnum].plyrname, "Chemtrails");

    #endif
}

void EOFerror(int _filereadfinish)
{
    /*Prints error message if the end of the file is reached and too many/too few elements were read.*/

    if (_filereadfinish == FLAG)
    {
        printf("Error: Game state file has too many elements.\n");
    }
    else
    {
        printf("Error: Game state file is missing one or more elements.\n");
    }
}


int checkphase(char _cmdstr[])
{
    /*Finds the phase from the values entered in the command line.*/

    char teststr1[10];
    char teststr2[10];
    char teststr3[10];

    strcpy(teststr1, "phase=");
    strcpy(teststr2, "place");
    strcpy(teststr3, "move");

    if (strstr(_cmdstr, teststr1))
    {
        if (strstr(_cmdstr, teststr2))
        {
            return 1;
        }
        else if (strstr(_cmdstr, teststr3))
        {
            return 2;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}

int checkid(char _cmdstr[])
{
    /*Checks to see if one of the values entered in the command line is id.*/

    char teststr[5];

    strcpy(teststr, "id");

    if (strcmp(_cmdstr, teststr) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int checkpengsperplyr(char _cmdstr[])
{
    /*Finds the number of penguins per player from the values entered in the command line.*/

    int _pengsperplyr;

    char teststr1[15];
    char teststr2[15];

    strcpy(teststr1, "penguins=");
    strcpy(teststr2, "penguin=");

    if ((strstr(_cmdstr, teststr1)) || (strstr(_cmdstr, teststr2)))
    {
        int i = 0;

        while ((isdigit(_cmdstr[i])) == 0)
        {
            _cmdstr[i] = '0';

            i++;
        }

        _pengsperplyr = atoi(_cmdstr);

        return _pengsperplyr;
    }
    else
    {
        _pengsperplyr = 0;

        return _pengsperplyr;
    }
}

int checkboardfilename(char _cmdstr[])
{
    /*Finds the names of the input and output game state files from the values entered in the command line.*/

    char teststr[5];

    strcpy(teststr, ".txt");

    if (strstr(_cmdstr, teststr))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}






