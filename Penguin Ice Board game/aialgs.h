#ifndef AIALGS_H_INCLUDED
#define AIALGS_H_INCLUDED

/*
This file contains the algorithms used by the AI when the game is played in autonomous mode. The first set of functions
pertains to the default ai mode, where tiles are given numerical values based on their general usefulness, and the move/placement
chosen by the AI is the one with the most points. The other functions locate regions of tiles that are connected to each other
(islands) and determine which (if any) moves can be made to gain control of favorable islands, as well as which moves should be
made so that as many fish as possible can be collected from an island. The best moves found from the islanding ai mode are
prioritized over the best moves found from the default ai mode.
*/


int getmovefreedomrating(int _freetilesleft, int _ppinfo);
int getbestpengplace(struct Board _boarddata[], int _boarddim[], int _freedomlevel, int _ppinfo);
void getbestpengmovesaimods(struct Board _boarddata[], struct Player _playerdata[], int _boarddim[], int _freedomlevel, int _ppinfo);
void getaimodifiervalue(struct Board _boarddata[], int _boarddim[], int _tilenum, int _freedomlevel, int _ppinfo);
int getfinalpengmove(struct Board _boarddata[], struct Player _playerdata[], int _boarddim[], int _ppinfo);
void findislands(struct Board _boarddata[], struct Player _playerdata[], int _boarddim[], int _islenumtested, int _numofguesses, int _freedomlevel, int _ppinfo);
int tileprediction(struct Board _boarddata[], struct Player _playerdata[], int _boarddim[], int _islenumtested, int _numofguesses, int _ppinfo);
int ischokepoint(struct Board _boarddata[], int _boarddim[], int _tiletocheck, int _ppinfo);
int findbestpath(struct Board _boarddata[], struct Player _playerdata[], int _boarddim[], int currenttile, int _pengnum, int _islenumtested, int pathlength, int _numofguesses, int _ppinfo);
void changeadjtilemoves(struct Board _boarddata[], int _boarddim[], int _currenttile, int dirofchange);
int recursioncounter(int _reset, int _increment);
void getbestpengmovesisles(struct Player _playerdata[], int _boarddim[], int _bestmoveinfo[], int _maxbmoveinfo[], int _boundpenginfo[], int _freedomlevel, int _ppinfo);
int getadjustedfishval(int _fishonisle, int _freedomlevel, int _ppinfo);
int getbestmoveindex(int _boarddim[], int _depttile, int _desttile, int _pengnum);
int getmovedirection(int _boarddim[], int _depttile, int _desttile);
void copytodummyboard(struct Board _dummyboarddata[], struct Board _boarddata[], int _boarddim[]);
void copytodummyplayer(struct Player _dummyplayerdata[], struct Player _playerdata[], int _ppinfo);

#endif // AIALGS_H_INCLUDED
