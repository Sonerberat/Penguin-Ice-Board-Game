#ifndef READINPUT_H_INCLUDED
#define READINPUT_H_INCLUDED

/*
This file contains functions that read/organize files at the beginning of each turn and transfer the contents of those files
into the data structures used in the program.
*/


int getsavedppinfo(char _stringid[], int _phase);
int getboarddimensions(FILE *_pubboardin, int _boarddim[]);
int gettempboardvalues(FILE *_pubboardin, int _tempboard[], int _totnumtiles);
int getscoreboard(FILE *_pubboardin, struct Player _playerdata[], char stringid[], int _strinptest, int _possiblereset, int _ppinfo);
void getteamname(struct Player _playerdata[], int _plyrnum);
void EOFerror(int _filereadfinish);
int checkphase(char _cmdstr[]);
int checkid(char _cmdstr[]);
int checkpengsperplyr(char _cmdstr[]);
int checkboardfilename(char _cmdstr[]);


#endif // READINPUT_H_INCLUDED
