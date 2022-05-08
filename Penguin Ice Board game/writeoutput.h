#ifndef WRITEOUTPUT_H_INCLUDED
#define WRITEOUTPUT_H_INCLUDED

/*
This file contains functions that writes and saves all necessary information about the game's status (such as the board
state and ppinfo) into .txt files.
*/


void outputboardtxt(struct Board _boarddata[], struct Player _playerdata[], int _boarddim[], char _outboardfile[], int _ppinfo);
void printboarddim(FILE *_pubboardout, int _boarddim[]);
void printboardvalues(FILE *_pubboardout, struct Board _boarddata[], int _boarddim[], int _ppinfo);
void printscoreboard(FILE *_pubboardout, struct Player _playerdata[], int _ppinfo);
void outputprivtxt(char _stringid[], int _phase, int _movestatus, int _ppinfo);

#endif // WRITEOUTPUT_H_INCLUDED
