#ifndef DISPLAYUI_H_INCLUDED
#define DISPLAYUI_H_INCLUDED

/*
This file contains functions used to display the user interface for the interactive mode.
*/

void displaygameboard(struct Board _boarddata[], int _boarddim[], int _ppinfo);
void displayscoreboard(struct Player _playerdata[], int _ppinfo);
int gettileletterval(int dimnum);
int getplyrsymbol(int _plyrid);

#endif // DISPLAYUI_H_INCLUDED
