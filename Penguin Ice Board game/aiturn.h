#ifndef AITURN_H_INCLUDED
#define AITURN_H_INCLUDED

/*
The function in this file, aiplay, analyzes the information contained in the structure arrays used and determines the best penguin
placement/move. It is used when the game is played in autonomous mode (in place of the plyrturn file) and moves/places penguins
independently.
*/


int aiplay(struct Board _boarddata[], struct Player _playerdata[], int _boarddim[], int _phase, int _freetilesleft, int _ppinfo);

#endif // AITURN_H_INCLUDED
