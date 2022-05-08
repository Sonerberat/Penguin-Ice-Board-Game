#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>


#include "univfuncs.h" //definitions of constants, macros, and functions that manipulate/extract data structures' variables
#include "structini.h" //initializes the structures as global variables
#include "readinput.h" //functions that read/manipulate information found in the .txt files
#include "writeoutput.h" //functions that write and save .txt files for future use
#include "sharedalgs.h" //algorithms that are used by both interactive mode and autonomous mode
#include "displayui.h" //functions that print a user friendly interface on the screen
#include "plyrturn.h" //functions that get the user input and carry out the player's move/placement in interactive mode
#include "aiturn.h" //functions that carry out the AI's move/placement in autonomous mode.
#include "aialgs.h" //algorithms that are used by the AI to choose the best move/placement



int main(int argc, char *argv[])
{
    int totnumtiles, freetilesleft, ppinfo;
    int strinptest, possiblereset;
    int dimerrval, gamebrderrval, scrbrderrval, pengcounterrval, movestatus;
    int argcounter;
    int boarddim[2];

    int phase = 0, id = UNFLAG, pengsperplyr = 0, numoffilepaths = 0;

    char stringid[MAXNAMELEN];
    char inpboardfile[100];
    char outboardfile[100];
    char cmdstr[100];

    /*
    totnumtiles is the total number of tiles (including tiles with 0 fish) on the board.

    strinptest is the string input test, which indicates whether or not the player has entered their team name (string id) yet.

    strinptest == FLAG (1) indicates that the player has not yet entered a stringid, while strinptest == UNFLAG (0) indicates
    that the player already has a stringid.

    movestatus is 0 if the player was able to place/move a penguin this turn and 1 if the player cannot place/move a penguin this turn.

    dimerrval, gamebrderrval, and scrbrderrval (dimension, game board, and score board error value) are equal to 1 (FLAG) if
    an error is encountered reading board.txt. Otherwise, they are 0 (UNFLAG).

    boarddim[2] is an array containing the values of the dimensions of the board. boarddim[0] is the number of rows, and boarddim[1]
    is the number of columns.


    ppinfo is an integer that "stores" 4 numbers, the player's id number, the number of penguins per player (regardless of their
    status), the current total number of players in the game (regardless of their status), and the total number of free tiles
    that existed before the game started. This integer is saved to a .txt file before the end of each turn. The player's id number
    needs to be saved at the end of each turn since it is the only way for the program to recognize which penguins (and score value)
    belong to the user. The number of pengs per player are saved since that information is only available during the placement
    phase. The number of free tiles initially on the board is used by the AI to gauge the progress of the game, while the total
    number of players is stored for convenience.

    The player's id (pid) is a number from 1 to 9, the number of players in the game (numofplyrs) can be from 1 to 9, the number
    of penguins per player (pengsperplyr) can be from 1 to MAXPENGS, and the initial number of free tiles can be from 0 to 1225.
    To reduce the number of variables that need to be saved/moved around, these 4 numbers are "stacked" to make 1 number.
    Each integer value (equal to 1 or higher in this case) can be written as a unique combination of 4 smaller integers.

    Assuming the initial number of free tiles is 0 (for simplicity), if ppinfo has a value of 1 to 9, it means that numofplyrs
    is 1, pengsperplyr is 1, and pid is 1 to 9. Since it is not possible for pid to be higher than numofplyrs, ppinfo will
    never have values 2 to 8; ppinfo will never equal a number that produces an "impossible" combination. When ppinfo is from
    10 to 18, numofplyrs is 2, pengsperplyr is 1, and pid is 1 to 9. When ppinfo is from 19 to 27, numofplyrs is 3,
    pengsperplyr is 1, and pid is 1 to 9 (if ppinfo is 19, pid is 1, if ppinfo is 20, pid is 2, if ppinfo is 21, pid is 3, etc.).

    If ppinfo is from 82 to 90, numofplyrs is 1, pengsperplyr is 2, and pid is 1 to 9. When ppinfo is from 91 to 99, numofplyrs is 2,
    pengsperplyr is 2, and pid is 1 to 9. When ppinfo is from 100 to 108, numofplyrs is 3, pengsperplyr is 2, and pid is 1 to 9.
    When ppinfo is from 163 to 171, numofplyrs is 1, pengsperplyr is 3, and pid is 1 to 9. This pattern continues.

    The formula for ppinfo is then, ppinfo = pid + ((numofplyrs - 1) * MAXPLYRID) + ((pengsperplyr - 1) * MAXPLYRID * MAXNUMPLYRS)
    (without startfreetiles) with both MAXPLYRID and MAXNUMPLYRS equaling 9. With startfreetiles included, the term
    (startfreetiles * MAXPLYRID * MAXNUMPLYRS * MAXPENGS) is added to the end of the formula. The actual formula used in the
    function is slightly different, since pid is shifted by one so that it can start from 0.

    Before the end of the turn, ppinfo is saved to ppinfo.txt. If it is the movement phase, ppinfo is saved as a negative number.
    At the beginning of the turn, ppinfo takes on the value read from ppinfo.txt (ppinfo becomes positive if it is the
    movement phase). If ppinfo.txt cannot be found OR it is the placement phase and ppinfo is negative, ppinfo is set equal
    to 0 (indicating that it is the player's first turn). During the placement phase, ppinfo is reevaluated each turn, in case
    the number of players since the previous turn has increased (possible during turn 2).
    */


    argcounter = 1;

    /*Reads the command line and populates variables with the values it provides.*/
    while (argcounter < argc)
    {
        strcpy(cmdstr, argv[argcounter]);

        getlowercase(cmdstr);


        if (((phase > 0) && (checkphase(cmdstr)) > 0) || (id > 0 && (checkid(cmdstr)) > 0) || (pengsperplyr > 0 && (checkpengsperplyr(cmdstr)) > 0) || (numoffilepaths == 2 && (checkboardfilename(cmdstr)) > 0))
        {
            printf("Error: Too many arguments in command line.\n");

            return 2;
        }
        else
        {
            if (phase == 0)
            {
                phase = checkphase(cmdstr);
            }

            if (id == 0)
            {
                id = checkid(cmdstr);
            }

            if (pengsperplyr == 0)
            {
                pengsperplyr = checkpengsperplyr(cmdstr);
            }

            if (numoffilepaths < 2 && (checkboardfilename(cmdstr)) > 0)
            {
                if (numoffilepaths == 0)
                {
                    strcpy(inpboardfile, cmdstr);
                }
                else if (numoffilepaths == 1)
                {
                    strcpy(outboardfile, cmdstr);
                }

                numoffilepaths++;
            }
        }

        argcounter++;
    }


    if (id == UNFLAG && (((phase != PLACEMENT) && (phase != MOVEMENT)) || numoffilepaths != 2 || (phase == PLACEMENT && (pengsperplyr < 1 || pengsperplyr > MAXPENGS))))
    {
        printf("Error: Missing or invalid arguments in command line.\n");

        return 2;
    }


    ppinfo = getsavedppinfo(stringid, phase);

    /*ppinfo is 0 during the first turn, indicating that the player has not chosen a string id*/
    if (ppinfo == 0)
    {
        strinptest = FLAG;
    }
    else
    {
        strinptest = UNFLAG;
    }

    /*if id is read in the command line, returns the player's id*/
    if (id == FLAG)
    {
        if (strinptest == FLAG)
        {
            printf("The player has not selected an ID yet.\n");
        }
        else
        {
            printf("The player's ID is %s\n", stringid);
        }

        return 0;
    }

    /*opens board.txt to read*/
    FILE *pubboardin = fopen(inpboardfile, "r");

    if (pubboardin == NULL)
    {
        printf("Error: Game state file not found.\n");

        return 2;
    }

    /*Reads the 1st line of board.txt and saves the values to boarddim[]. The variable returned indicates any errors.*/
    dimerrval = getboarddimensions(pubboardin, boarddim);

    if (dimerrval == 1)
    {
        return 2;
    }

    totnumtiles = boarddim[0] * boarddim[1]; //total rows * total columns equals the total number of tiles on the board

    int tempboard[totnumtiles]; //array used to temporarily store the board values in board.txt

    /*Copies the board values in board.txt to tempboard[]. The variable returned indicates any errors.*/
    gamebrderrval = gettempboardvalues(pubboardin, tempboard, totnumtiles);

    if (gamebrderrval == 1)
    {
        return 2;
    }

    possiblereset = UNFLAG;

    /*Updates ppinfo (in case the number of players changed since the previous turn). Resets ppinfo if the previous game was
    restarted before it had finished.*/
    if (phase == PLACEMENT)
    {
        ppinfo = updateppinfo(tempboard, totnumtiles, pengsperplyr, ppinfo); //ppinfo is calculated if it is the movement phase

        if ((getcurrentid(ppinfo)) >= (getnumofplyrs(ppinfo)))
        {
            ppinfo = 0;

            strinptest = FLAG;

            ppinfo = updateppinfo(tempboard, totnumtiles, pengsperplyr, ppinfo); //ppinfo is calculated if it is the movement phase
        }
        else if (strinptest == UNFLAG)
        {
            possiblereset = isinitialturns(tempboard, totnumtiles);
        }
    }

    freetilesleft = getcurrentfreetiles(tempboard, totnumtiles);


    struct Board boarddata[totnumtiles]; //structure array that will contain info about each tile
    struct Player playerdata[(getnumofplyrs(ppinfo) - phase + 2)]; //structure array that will contain info about each player/penguin


    lateppinforeset:

    /*Reads the score board in board.txt, and saves the values to the structure array playerdata[].
    Also asks the user to enter a valid string id if strinptest is equal to FLAG (1)
    The variable returned indicates any errors (1 if there was an error reading the file. -1 if the game was not restarted
    properly and ppinfo needs to be reset.*/
    scrbrderrval = getscoreboard(pubboardin, playerdata, stringid, strinptest, possiblereset, ppinfo);

    if (scrbrderrval == 1)
    {
        return 2;
    }
    else if (scrbrderrval == -1)
    {
        ppinfo = 0;

        strinptest = FLAG;
        possiblereset = UNFLAG;

        ppinfo = updateppinfo(tempboard, totnumtiles, pengsperplyr, ppinfo);

        goto lateppinforeset;
    }

    /*closes board.txt*/
    fclose(pubboardin);


    /*Converts and transfers the values of the tempboard[] array to the structure array boarddata[].
    Assigns an id to each penguin (starting with 0). Also, initializes playerdata[].pengdata[] at all indices.
    If there are too few or too many penguins on the board, returns 1. Otherwise, returns 0.*/
    pengcounterrval = temptoboarddata(tempboard, boarddata, playerdata, totnumtiles, phase, ppinfo);

    if (pengcounterrval == 1)
    {
        return 2;
    }

    /*For each tile where the ice has not broken through, finds and records the directions of any free adjacent tiles, the number
    of moves that can be made in each direction, the ids of any penguins that are in the line of sight of that tile, and the
    number of 3 fish tiles that can be reached in each direction. These values are stored in the structure array boarddata[].*/
    scanboard(boarddata, boarddim);

    /*Finds how many penguins that are on the board and not trapped each player has left by checking the number of free adjacent
    tiles from the tile occupied by each penguin.*/
    findactivepengs(boarddata, playerdata, ppinfo);

    /*Prints the current (before the player has taken their turn) playing board and score board on the screen for the user.*/
    displaygameboard(boarddata, boarddim, ppinfo);
    displayscoreboard(playerdata, ppinfo);

    #ifdef INTERACTIVE

    /*Prompts the player to place/move a penguin. Once a valid choice is made, the penguin is placed/moved, the structure arrays
    boarddata[] and playerdata[] are updated, and the number 0 is returned. If no moves can be made, the number 1 is returned.
    If something went wrong, and the function did not work properly, the number 3 is returned.*/
    movestatus = playerturn(boarddata, playerdata, boarddim, phase, ppinfo);

    #else

    /*The AI places/moves a penguin without any input from the player. Once a valid choice is made, the penguin is placed/moved,
    the structure arrays boarddata[] and playerdata[] are updated, and the number 0 is returned. If no moves can be made, the
    number 1 is returned. If the AI could not place or move a penguin despite it being possible to do so, the number 3 is returned.*/
    movestatus = aiplay(boarddata, playerdata, boarddim, phase, freetilesleft, ppinfo);

    #endif

    /*Prints the updated (after the penguin has been placed/moved) playing board and score board on the screen for the user.*/
    displaygameboard(boarddata, boarddim, ppinfo);
    displayscoreboard(playerdata, ppinfo);

    /*Saves the final game state to a txt file using boarddata[]*/
    outputboardtxt(boarddata, playerdata, boarddim, outboardfile, ppinfo);

    /*Saves ppinfo to a txt file*/
    outputprivtxt(playerdata[getcurrentid(ppinfo)].plyrname, phase, movestatus, ppinfo);

    /*Returns a value corresponding with the outcome of the program*/
    return movestatus;
}








