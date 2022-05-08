#ifndef UNIVFUNCS_H_INCLUDED
#define UNIVFUNCS_H_INCLUDED


/*
This file contains macros, constants, and functions that are used throughout the whole program. Most of these functions serve to
manipulate the "stacked" data structures utilized and either extract information from them or modify their values. The rest of
the functions are mostly very general and are there for convenience.
*/

/*Define INTERACTIVE to play the game in interactive mode. Otherwise, the game runs in autonomous mode.*/
#define INTERACTIVE


#define ARRAYSHIFT 1 //Used when values shift by 1 (usually as a result of the first index being 0, not 1).
#define MAXDIMENSION 35 //Maximum board dimension.
#define MINDIMENSION 1 //Minimum board dimension.
#define MAXPLYRID 9 //Maximum number of players.
#define MAXNUMPLYRS 9 //Maximum number of players.
#define MAXPENGS 10 //Maximum number of penguins per player.
#define MAXFISH 3 //Maximum number of fish per tile.
#define MAXNAMELEN 52 //Maximum name length (50). Value of 52 ensures that spaces/return carriages/null characters don't cause issues.
#define ADJTILECOMBOS 16 //Number of movement combinations to adjacent tiles.
#define MAXPENGID (MAXPENGS * MAXNUMPLYRS)
#define MAXISLANDS 8 //Number of different islands. Values of 0, 1, and 2 indicate tiles that have not been tested for islands yet.
#define JUNKISLENUM 3 //Island number for groups of tiles that the player's penguins don't have exclusive access to.
#define OWNISLENUM 4 //Island number for groups of tiles that only the player's penguins have access to.
#define NULLISLENUM 5 //Island number for tiles that cannot be moved to.
#define ISLETILEFOUND 6 //Free tile that has been found by an adjacent tile.
#define ISLETILETESTED 7 //Tile that has been checked for free adjacent tiles.
#define STARTAIMODVAL 20000 //The default value of the ai modifier value (on average, the ai mod value is less than 20000).
#define MAXAIMODVAL 30000 //The number of different ai modifier values (1 to 29999)
#define MAXISLEPOINTS 351 //The maximum number of isle points combinations (0 to 350). Isle points greater than 350 are set equat to 350.
#define MAXPOSMOVES (MAXDIMENSION) //The maximum number of moves in each direction.
#define MAXHIFISHMOVES (MAXDIMENSION) //The maximum number of moves to tiles with 3 fish in each direction.
#define MAXTILES (MAXDIMENSION * MAXDIMENSION) //The maximum number of tiles a board can have.
#define MAXSCORE (MAXTILES * MAXFISH) //The maximum possible score attainable (given the board dimension restrictions).
#define MINSCORE 1 //The minimum possible score a player can have after taking at least 1 move.
#define HYPHEN 45 //The ASCII value of '-'.
#define AIMODES 4 //The number of different ai modes a penguin can have (used to determing move priority).
#define TURTLE 0 //The penguin controls an island. It does not move until no penguins with a higher priority (that can move) remain.
#define DEFAULT 1 //The penguin does not control an island and has no moves that will allow it to gain control of one. Medium priority.
#define SECURE 2 //The penguin can gain control of an island with 2 moves (highest priority).
#define OVERRIDE 3 //The penguin can gain control of an island with 1 move (highest priority).
#define MOVEDIRECTIONS 4 //The maximum number of directions it is possible to move in.
#define DIAGDIRECTIONS 4 //The maximum number of tiles diagonal to a tile.
#define MOVEANDDIAGDIR (MOVEDIRECTIONS + DIAGDIRECTIONS) //The maximum number of tiles surrounding a tile.
#define STARTTILEFISH 1 //Number of fish needed on a tile to be able to place a penguin on it.
#define NEGONE -1
#define PLACEMENT 1 //Placement phase
#define MOVEMENT 2 //Movement phase
#define FLAG 1 //True (or active)
#define UNFLAG 0 //False (or inactive)


int getcurrentid(int _ppinfo_); //0 to 8 (1 to 9 inside ppinfo)
int getnumofplyrs(int _ppinfo_); //1 to 9
int getpengsperplyr(int _ppinfo_); //1 to 10
int getstartingfreetiles(int _ppinfo_); //0 to 1225
int getppinfosum(int _currentid_, int _numofplyrs_, int _pengsperplyr_, int _startfreetiles_);
int getnumoffish(int _rawval_); //0 to 3
int getadjmoves(int _rawval_); //0 to 15
int getintrnpengid(int _rawval_); //-1 to 89 (0 to 90 inside rawval)
int getislandnum(int _rawval_); //0 to 7
int changeislandnum (int _rawval_, int _newislandnum_);
int getaimodval(int _rawval_); //0 to 29999
int getintrnboarddatasum(int _fish_, int _adjmoves_, int _ipengid_, int _islandnum_, int _aival_);
int gettotposmoves(int _rawval_); //0 to 34
int gethifishtiles(int _rawval_); //0 to 34
int getextpengid(int _rawval_); //-1 to 89 (0 to 90 inside rawval)
int getaddressislenum(int _rawval_);//-1 to 39 (0 to 40 inside rawval)
int changeaddressislenum(int _rawval_, int _newaddressislenum_);
int getextboarddatasum(int _moves_, int _hifishmoves_, int _epengid_, int _addressislenum_);
int getpengtile(int _rawval_); //0 to 1224 (1 to 1225 inside rawval)
int getpengbestmove(int _rawval_); //0 to 1224
int changepengbestmove(int _rawval_, int _newbestmove_);
int getpengaimode(int _rawval_); //0 to 3
int getpengislesize(int _rawval_); //0 to 350
int getpengdatasum(int _tilenum_, int _bestmove_, int _aimode_, int _maxislepoints_);
int getallytotislenum(int _rawval_); //0 to 39 (1 to 40 inside rawval)
int getallybestmove(int _rawval_); //0 to 1224
int changeallybestmove(int _rawval_, int _newbestmove_);
int getallyaimode(int _rawval_); //0 to 3
int getallyislesize(int _rawval_); //0 to 3675
int gettempisleinfosum(int _allytotislenum_, int _tempbestmove_, int _tempaimode_, int _tempislesize_);
int getrownum(int _boarddim_[], int _tile_); //0 to 34
int getcolnum(int _boarddim_[], int _tile_); //0 to 34
int gettilenum(int _boarddim_[], int _rownum_, int _colnum_);
int shiftonetile(int _boarddim_[], int _depttile_, int _dircount_);
int isallypengid(int _pengid_, int _ppinfo_); //0 or 1
int isfoepengid(int _pengid_, int _ppinfo_); //0 or 1

int expon(int num, int expo);
int isnumber(char testedstr[]);
int isneg(int num);
int ispos(int num);
void getlowercase(char _cmdstr[]);

#endif // UNIVFUNCS_H_INCLUDED
