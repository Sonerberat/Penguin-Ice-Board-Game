#ifndef PLYRTURN_H_INCLUDED
#define PLYRTURN_H_INCLUDED

/*
This file contains functions that are used when the game is played in the interactive mode. After confirming that at least one
move/placement can be made, the functions obtain an input from the user, check the validity of the inputted value(s), and
move/place the penguin to the tile chosen by the user.
*/


int playerturn(struct Board _boarddata[], struct Player _playerdata[], int _boarddim[], int phase, int _ppinfo);
int playerinptplace(struct Board _boarddata[], struct Player _playerdata[], int _boarddim[], int _ppinfo);
int playerinptmove(struct Board _boarddata[], struct Player _playerdata[], int _boarddim[], int _ppinfo);
int checkinptval(int _boarddim[]);

#endif // PLYRTURN_H_INCLUDED
