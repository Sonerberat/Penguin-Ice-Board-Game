#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

#include "univfuncs.h"


int getcurrentid(int _ppinfo_)
{
    /*
    extracts the id of the current player from ppinfo using the formula evaluated for the ranges of the values that make up ppinfo.
    Adding/Subtracting ARRAYSHIFT (1) is necessary when the range of possible values starts at 1 and not 0. When that is the case,
    the function is only guaranteed to return the correct value when the parameter tested (_ppinfo_ in this case) is 1 or higher.
    While the number storing currentid inside ppinfo has a range of 1 to 9, this function returns a value from 0 to 8. This is done
    out of convenience to make dealing with arrays (which start with an index of 0) simpler.
    Originally, the formula was _currentid_ = ((_ppinfo_ - ARRAYSHIFT) % MAXPLYRID) + ARRAYSHIFT.
    */

    int _currentid_;

    _currentid_ = ((_ppinfo_ - ARRAYSHIFT) % MAXPLYRID);

    return _currentid_;
}

int getnumofplyrs(int _ppinfo_) //extracts the number of players in the game (including those that cannot make any more moves)
{
    int _numofplyrs_;

    _numofplyrs_ = (((_ppinfo_ - ARRAYSHIFT) / MAXPLYRID) % MAXNUMPLYRS) + ARRAYSHIFT;

    return _numofplyrs_;
}

int getpengsperplyr(int _ppinfo_) //extracts the number of penguins per player (includes penguins that are trapped/not placed yet)
{
    int _pengsperplyr_;

    _pengsperplyr_ = ((((_ppinfo_ - ARRAYSHIFT) / MAXPLYRID) / MAXNUMPLYRS) % MAXPENGS) + ARRAYSHIFT;

    return _pengsperplyr_;
}

int getstartingfreetiles(int _ppinfo_) //extracts the total number of free tiles that were on the board at the start of the game
{
    int _startfreetiles_;

    _startfreetiles_ = (((_ppinfo_ - ARRAYSHIFT) / MAXPLYRID) / MAXNUMPLYRS) / MAXPENGS;

    return _startfreetiles_;
}


int getppinfosum(int _currentid_, int _numofplyrs_, int _pengsperplyr_, int _startfreetiles_) //used when changing or first calculating ppinfo
{
    int _ppinfo_;

    /*Since the function getcurrentid() returns a value from 0 to 8 instead of 1 to 9, the _currentid_ inputted into the
    getppinfosum() function is 1 too low (ppinfo stores currentid as an integer from 1 to 9). So, ARRAYSHIFT must be added to
    _currentid_ in the sum.*/
    _ppinfo_ = (_currentid_ + ARRAYSHIFT) + ((_numofplyrs_ - ARRAYSHIFT) * MAXPLYRID) + ((_pengsperplyr_ - ARRAYSHIFT) * MAXPLYRID * MAXNUMPLYRS) + (_startfreetiles_ * MAXPLYRID * MAXNUMPLYRS * MAXPENGS);


    return _ppinfo_;
}





int getnumoffish(int _rawval_) //extracts the number of fish on a tile from the value obtained by boarddata[tile].intrninfo
{
    int _fish_;

    _fish_ = _rawval_ % (MAXFISH + ARRAYSHIFT);

    return _fish_;
}

int getadjmoves(int _rawval_) //extracts a number which corresponds to the directions of all free adjacent tiles
{
    int _adjmoves_;

    _adjmoves_ = (_rawval_ / (MAXFISH + ARRAYSHIFT)) % ADJTILECOMBOS;

    return _adjmoves_;
}

int getintrnpengid(int _rawval_) //extracts the id of the penguin occupying the tile (0 if the tile is not occupied)
{
    /*While the number storing ipengid inside boarddata[].intrninfo has a range of 0 to (MAXPENGID + ARRAYSHIFT) (with a
    value of 0 corresponding to an empty tile), this function returns a value from -1 to (MAXPENGID - ARRAYSHIFT). This is done
    out of convenience to make dealing with arrays (which start with an index of 0) simpler.
    Originally, the formula was _ipengid_ = ((_rawval_ / (MAXFISH + ARRAYSHIFT)) / ADJTILECOMBOS) % (MAXPENGID + ARRAYSHIFT).*/

    int _ipengid_;

    _ipengid_ = (((_rawval_ / (MAXFISH + ARRAYSHIFT)) / ADJTILECOMBOS) % (MAXPENGID + ARRAYSHIFT)) - ARRAYSHIFT;

    return _ipengid_;
}

int getislandnum(int _rawval_) //extracts the island number/status of the tile (0 if in interactive mode)
{
    int _islandnum_;

    _islandnum_ = (((_rawval_ / (MAXFISH + ARRAYSHIFT)) / ADJTILECOMBOS) / (MAXPENGID + ARRAYSHIFT)) % MAXISLANDS;

    return _islandnum_;
}

int changeislandnum (int _rawval_, int _newislandnum_)
{
    /*Used to directly change the island number of a tile without having to use getintrnboarddatasum*/

    int _oldislandnum_, _endval_;

    _oldislandnum_ = getislandnum(_rawval_);

    _endval_ = _rawval_ + ((_newislandnum_ - _oldislandnum_) * (MAXFISH + ARRAYSHIFT) * ADJTILECOMBOS * (MAXPENGID + ARRAYSHIFT));

    return _endval_;
}

int getaimodval(int _rawval_) //extracts the AI modifier value of the tile (0 if in interactive mode)
{
    int _aival_;

    _aival_ = (((_rawval_ / (MAXFISH + ARRAYSHIFT)) / ADJTILECOMBOS) / (MAXPENGID + ARRAYSHIFT)) / MAXISLANDS;

    return _aival_;
}


int getintrnboarddatasum(int _fish_, int _adjmoves_, int _ipengid_, int _islandnum_, int _aival_)
{
    /*used when changing or first calculating the value of boarddata[tile].intrninfo*/

    int _sum_;

    /*Since the function getintrnpengid() returns a value from -1 to (MAXPENGID) instead of 0 to (MAXPENGID + ARRAYSHIFT), the
    _ipengid_ inputted into the getintrnboarddatasum() function is 1 too low (boarddata[].intrninfo stores ipengid as an integer
    from 0 to (MAXPENGID + ARRAYSHIFT)). So, ARRAYSHIFT must be added to _ipengid_ in the sum.*/
    _sum_ = _fish_ + (_adjmoves_ * (MAXFISH + ARRAYSHIFT)) + ((_ipengid_ + ARRAYSHIFT) * (MAXFISH + ARRAYSHIFT) * ADJTILECOMBOS) + (_islandnum_ * (MAXFISH + ARRAYSHIFT) * ADJTILECOMBOS * (MAXPENGID + ARRAYSHIFT)) + (_aival_ * (MAXFISH + ARRAYSHIFT) * ADJTILECOMBOS * (MAXPENGID + ARRAYSHIFT) * MAXISLANDS);

    return _sum_;
}



int gettotposmoves(int _rawval_) //extracts the total possible moves in some direction from boarddata[tile].extinfo[direction]
{
    int _moves_;

    _moves_ = _rawval_ % MAXPOSMOVES;

    return _moves_;
}

int gethifishtiles(int _rawval_) //extracts the total number of 3 fish tiles that can be reached in some direction
{
    int _hifishmoves_;

    _hifishmoves_ = (_rawval_ / MAXPOSMOVES) % MAXHIFISHMOVES;

    return _hifishmoves_;
}

int getextpengid(int _rawval_)
{
    /*
    Extracts the the id of the penguin that is in the line of sight, in some direction, of the tile that _rawval_ was obtained
    from (boarddata[tile].extinfo[direction]). If no penguins have line of sight with that tile in that direction, the external
    penguin id is -1.

    While the number storing epengid inside boarddata[].extinfo[] has a range of 0 to MAXPENGID (with a value of 0
    corresponding to an empty tile), this function returns a value from -1 to (MAXPENGID - ARRAYSHIFT). This is done
    out of convenience to make dealing with arrays (which start with an index of 0) simpler.
    Originally, the formula was _epengid_ = (_rawval_ / MAXPOSMOVES) / MAXHIFISHMOVES.
    */

    int _epengid_;

    _epengid_ = (((_rawval_ / MAXPOSMOVES) / MAXHIFISHMOVES) % (MAXPENGID + ARRAYSHIFT)) - ARRAYSHIFT;

    return _epengid_;
}

int getaddressislenum(int _rawval_)
{
    /*
    Extracts the address island number of the adjacent tile. The address island number acts as an address for the islands that
    can be accessed by the player's penguins.
    */

    int _addressislenum_;

    _addressislenum_ = (((_rawval_ / MAXPOSMOVES) / MAXHIFISHMOVES) / (MAXPENGID + ARRAYSHIFT)) - ARRAYSHIFT;

    return _addressislenum_;
}

int changeaddressislenum(int _rawval_, int _newaddressislenum_)
{
    /*Changes the value of address island number inside the raw value without needing to use getextboarddatasum().*/

    int _oldaddressislenum_, _endval_;

    _oldaddressislenum_ = getaddressislenum(_rawval_);

    _endval_ = _rawval_ + ((_newaddressislenum_ - _oldaddressislenum_) * MAXPOSMOVES * MAXHIFISHMOVES * (MAXPENGID + ARRAYSHIFT));

    return _endval_;
}


int getextboarddatasum(int _moves_, int _hifishmoves_, int _epengid_, int _addressislenum_)
{
    /*Used when changing or first calculating the value of boarddata[tile].extinfo[direction]*/

    int _sum_;

    /*Since the function getextpengid() returns a value from -1 to (MAXPENGID - ARRAYSHIFT) instead of 0 to MAXPENGID, the
    _epengid_ inputted into the getextboarddatasum() function is 1 too low (boarddata[].extinfo[] stores epengid as an integer
    from 0 to MAXPENGID). So, ARRAYSHIFT must be added to _epengid_ in the sum.*/
    _sum_ = _moves_ + (_hifishmoves_ * MAXPOSMOVES) + ((_epengid_ + ARRAYSHIFT) * MAXPOSMOVES * MAXHIFISHMOVES) + ((_addressislenum_ + ARRAYSHIFT) * MAXPOSMOVES * MAXHIFISHMOVES * (MAXPENGID + ARRAYSHIFT));


    return _sum_;
}





int getpengtile(int _rawval_)
{
    /*
    Extracts the tile number that a penguin occupies from _rawval_, which is obtained from
    playerdata[playernumber].pengdata[penguin number]. Since the value of 0 for playerdata[playernumber].pengdata[penguin number]
    is reserved for penguins that have not been placed yet, the value 1 corresponds to tile 0. Since the extracted value can be
    manipulated without affecting the original value in the structure array, and the actual tile number is preferable as a return
    value (as opposed to (tile number + 1)), the value of _tilenum_ can be subtracted by ARRAYSHIFT (1) before it is returned.
    This function, as well as the other functions that extract values from playerdata[].pengdata[], only guarantees correct return
    values when _rawval_ is not 0.
    */

    int _tilenum_;

    /*The number partially composing _rawval_ that represents the penguin's tile can have a value from 1 to MAXTILES. The formula to
    extract this number is ((abs(_rawval_) - ARRAYSHIFT) % MAXTILES) + ARRAYSHIFT. Subtracting 1 (ARRAYSHIFT), results in the formula
    for the tile number.*/
    _tilenum_ = (abs(_rawval_) - ARRAYSHIFT) % MAXTILES;

    return _tilenum_;
}

int getpengbestmove(int _rawval_)
{
    /*extracts the destination tile of the best move for the penguin (not used in interactive mode)*/

    int _bestmove_;

    _bestmove_ = ((abs(_rawval_) - ARRAYSHIFT) / MAXTILES) % MAXTILES;

    return _bestmove_;
}

int changepengbestmove(int _rawval_, int _newbestmove_)
{
    /*Used to directly change a penguin's best move without having to use getpengdatasum*/

    int _oldbestmove_, _endval_;

    _oldbestmove_ = getpengbestmove(_rawval_);

    _endval_ = abs(_rawval_) + ((_newbestmove_ - _oldbestmove_) * MAXTILES);

    return _endval_;
}

int getpengaimode(int _rawval_)
{
    /*extracts the value dictating the penguin's AI mode (not used in interactive mode)*/

    int _aimode_;

    _aimode_ = (((abs(_rawval_) - ARRAYSHIFT) / MAXTILES) / MAXTILES) % AIMODES;

    return _aimode_;
}

int getpengislesize(int _rawval_)
{
    /*extracts the maximum number of points that can be gained from controlling an island (not used in interactive mode)*/

    int _maxislepoints_;

    _maxislepoints_ = (((abs(_rawval_) - ARRAYSHIFT) / MAXTILES) / MAXTILES) / AIMODES;

    return _maxislepoints_;
}


int getpengdatasum(int _tilenum_, int _bestmove_, int _aimode_, int _maxislepoints_)
{
    /*Used when changing or first calculating the value of playerdata[playernumber].pengdata[penguinnumber]. ARRAYSHIFT is added
    to _tilenum_ since the number within the structure array corresponding to the tile number ranges from 1 to MAXTILES, whereas
    the tile number ranges from 0 to (MAXTILES - 1).*/

    int _sum_;

    _sum_ = (_tilenum_ + ARRAYSHIFT) + (_bestmove_ * MAXTILES) + (_aimode_ * MAXTILES * MAXTILES) + (_maxislepoints_ * MAXTILES * MAXTILES * AIMODES);

    return _sum_;
}




int getallytotislenum(int _rawval_)
{
    /*Extracts the temporary island number adjacent to one of the current player's penguins from the bestmoveinfo
    or maxbmoveinfo array.*/

    int _allytotislenum_;

    _allytotislenum_ = (abs(_rawval_) - ARRAYSHIFT) % (MAXPENGS * MOVEDIRECTIONS);

    return _allytotislenum_;
}

int getallybestmove(int _rawval_)
{
    /*Extracts the temporary best move in a direction of one of the current player's penguins from the bestmoveinfo
    or maxbmoveinfo array.*/

    int _tempbestmove_;

    _tempbestmove_ = ((abs(_rawval_) - ARRAYSHIFT) / (MAXPENGS * MOVEDIRECTIONS)) % MAXTILES;

    return _tempbestmove_;
}

int changeallybestmove(int _rawval_, int _newbestmove_)
{
    /*Changes the temporary best move directly without needing to use gettempisleinfosum.*/

    int _oldbestmove_, _endval_;

    _oldbestmove_ = getallybestmove(_rawval_);

    _endval_ = abs(_rawval_) + ((_newbestmove_ - _oldbestmove_) * (MAXPENGS * MOVEDIRECTIONS));

    return _endval_;
}

int getallyaimode(int _rawval_)
{
    /*Extracts the temporary aimode in a direction of one of the current player's penguins from the bestmoveinfo
    or maxbmoveinfo array.*/

    int _tempaimode_;

    _tempaimode_ = (((abs(_rawval_) - ARRAYSHIFT) / (MAXPENGS * MOVEDIRECTIONS)) / MAXTILES) % AIMODES;

    return _tempaimode_;
}

int getallyislesize(int _rawval_)
{
    /*Extracts the temporary isle size in a direction of one of the current player's penguins from the bestmoveinfo
    or maxbmoveinfo array.*/

    int _tempislesize_;

    _tempislesize_ = (((abs(_rawval_) - ARRAYSHIFT) / (MAXPENGS * MOVEDIRECTIONS)) / MAXTILES) / AIMODES;

    return _tempislesize_;
}


int gettempisleinfosum(int _allytotislenum_, int _tempbestmove_, int _tempaimode_, int _tempislesize_)
{
    /*Used to calculate the value of the bestmoveinfo array at some index.*/

    int _sum_;

    _sum_ = (_allytotislenum_ + ARRAYSHIFT) + (_tempbestmove_ * (MAXPENGS * MOVEDIRECTIONS)) + (_tempaimode_ * (MAXPENGS * MOVEDIRECTIONS) * MAXTILES) + (_tempislesize_ * (MAXPENGS * MOVEDIRECTIONS) * MAXTILES * AIMODES);

    return _sum_;
}




int getrownum(int _boarddim_[], int _tile_) //gets the row number of a tile using the total number of columns (_boarddim_[1])
{
    int _rownum_;

    _rownum_ = _tile_ / _boarddim_[1];

    return _rownum_;
}

int getcolnum(int _boarddim_[], int _tile_) //gets the column number of a tile using the total number of columns (_boarddim_[1])
{
    int _colnum_;

    _colnum_ = _tile_ % _boarddim_[1];

    return _colnum_;
}


int gettilenum(int _boarddim_[], int _rownum_, int _colnum_) //gets the tile number of the row/column combination using the total number of columns
{
    int _tilenum_;

    _tilenum_ = (_rownum_ * _boarddim_[1]) + _colnum_;

    return _tilenum_;
}

int shiftonetile(int _boarddim_[], int _depttile_, int _dircount_) //returns the tile adjacent to _depttile_ in direction dircount
{
    int _newtile_, _edgetest_;


    if ((_dircount_ % 2) == 0) //tile moving left/right
    {
        _edgetest_ = getcolnum(_boarddim_, _depttile_);

        if ((_edgetest_ == 0 && _dircount_ == 2) || (_edgetest_ == (_boarddim_[1] - ARRAYSHIFT) && _dircount_ == 0))
        {
            //over the edge of the board
            return -1;
        }
        else
        {
            _newtile_ = _depttile_ - (_dircount_ - 1);
        }
    }
    else if ((_dircount_ % 2) == 1) //tile moving up/down
    {
        _edgetest_ = getrownum(_boarddim_, _depttile_);

        if ((_edgetest_ == 0 && _dircount_ == 3) || (_edgetest_ == (_boarddim_[0] - ARRAYSHIFT) && _dircount_ == 1))
        {
            //over the edge of the board
            return -1;
        }
        else
        {
            _newtile_ = _depttile_ - (_boarddim_[1] * (_dircount_ - 2));
        }
    }
    else
    {
        return -1;
    }


    return _newtile_;
}

int isallypengid(int _pengid_, int _ppinfo_)
{
    /*Returns 1 if _pengid_ belongs to one of the player's penguins. Returns 0 otherwise.*/

    int _currentid_, _pengsperplyr_, _minallypengid_, _maxallypengid_;
    int _isally_;

    _currentid_ = getcurrentid(_ppinfo_);
    _pengsperplyr_ = getpengsperplyr(_ppinfo_);

    _minallypengid_ = _currentid_ * _pengsperplyr_; //the range of penguin ids that belongs to the player
    _maxallypengid_ = _minallypengid_ + (_pengsperplyr_ - ARRAYSHIFT);

    if (_pengid_ >= _minallypengid_ && _pengid_ <= _maxallypengid_)
    {
        _isally_ = FLAG;
    }
    else
    {
        _isally_ = UNFLAG;
    }

    return _isally_;
}

int isfoepengid(int _pengid_, int _ppinfo_)
{
    /*Returns 1 if _pengid_ belongs to another player's penguins. Returns 0 otherwise.*/

    int _currentid_, _pengsperplyr_, _numofplyrs_, _minallypengid_, _maxallypengid_, _maxpengid_;
    int _isfoe_;

    _currentid_ = getcurrentid(_ppinfo_);
    _pengsperplyr_ = getpengsperplyr(_ppinfo_);
    _numofplyrs_ = getnumofplyrs(_ppinfo_);

    _minallypengid_ = _currentid_ * _pengsperplyr_; //the range of penguin ids that belongs to the player
    _maxallypengid_ = _minallypengid_ + (_pengsperplyr_ - ARRAYSHIFT);

    _maxpengid_ = (_numofplyrs_ * _pengsperplyr_) - ARRAYSHIFT;

    if ((_pengid_ >= 0 && _pengid_ < _minallypengid_) || (_pengid_ > _maxallypengid_ && _pengid_ <= _maxpengid_))
    {
        _isfoe_ = FLAG;
    }
    else
    {
        _isfoe_ = UNFLAG;
    }

    return _isfoe_;
}



int expon(int num, int expo)
{
    /*Performs num^expo operation*/

    int prod, counter;

    prod = 1;

    counter = 0;

    while (counter < expo)
    {
        prod = prod * num;

        counter++;
    }

    return prod;
}


int isnumber(char testedstr[]) //Returns 1 if string is a number and 0 if it is not a number.
{
    int i;

    i = 0;

    while (testedstr[i] != '\0')
    {
        if ((isdigit(testedstr[i])) == 0)
        {
            return UNFLAG;
        }

        i++;
    }

    return FLAG;
}

int isneg(int num) //Returns 1 (FLAG) if number is negative and 0 (UNFLAG) otherwise
{
    if (num < 0)
    {
        return FLAG;
    }
    else
    {
        return UNFLAG;
    }
}

int ispos(int num) //Returns 1 (FLAG) if number is positive and 0 (UNFLAG) otherwise
{
    if (num > 0)
    {
        return FLAG;
    }
    else
    {
        return UNFLAG;
    }
}


void getlowercase(char _cmdstr[]) //converts a string to lowercase.
{
    int index;

    index = 0;

    while (_cmdstr[index] != '\0')
    {
        _cmdstr[index] = tolower(_cmdstr[index]);

        index++;
    }
}

