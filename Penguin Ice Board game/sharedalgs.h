#ifndef SHAREDALGS_H_INCLUDED
#define SHAREDALGS_H_INCLUDED

/*
This file contains functions that are used by both the interactive mode and the autonomous mode. Most of the functions involve
setting up the data structures (populating them with relevant values) and resolving the placement/movement of penguins within
those data structures.
*/


int updateppinfo(int _tempboard[], int _totnumtiles, int _pengsperplyr, int _ppinfo);
int getcurrentfreetiles(int _tempboard[], int _totnumtiles);
int isinitialturns(int _tempboard[], int totnumtiles);
int temptoboarddata(int _tempboard[], struct Board _boarddata[], struct Player _playerdata[], int _totnumtiles, int _phase, int _ppinfo);
void scanboard(struct Board _boarddata[], int _boarddim[]);
void findactivepengs(struct Board _boarddata[], struct Player _playerdata[], int _ppinfo);
int countplyrpenguins(struct Player _playerdata[], int plyrid, int _ppinfo);
int checkplcmttile(struct Board _boarddata[], struct Player _playerdata[], int _desttile, int _ppinfo);
int checkdepttile(struct Board _boarddata[], struct Player _playerdata[], int _depttile, int _ppinfo);
int checkdesttile(struct Board _boarddata[], struct Player _playerdata[], int _depttile, int _desttile, int _ppinfo);
int placepenguin(struct Board _boarddata[], struct Player _playerdata[], int _desttile, int _ppinfo);
int movepenguin(struct Board _boarddata[], struct Player _playerdata[], int _depttile, int _desttile, int _ppinfo);

#endif // SHAREDALGS_H_INCLUDED
