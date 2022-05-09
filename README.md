# Penguin-Ice-Board-Game
# Project “Fishing”
Game instruction
  1. This is a simplified version of board game “Hey, that’s mine fish".
  2. Game is played in turns - in each turn each player moves one penguin. The aim is to collect the 
  most fish. 
  3. The game board is arranged in a square grid with m rows and n columns. Each field of the grid 
  can be empty or can have an ice floe with 1, 2 or 3 fish on it.
  4. Each game is played on a new, possibly randomly generated, board. Each player has a certain 
  (known) number of penguins. 
  5. There are two phases of the game:
  a) Placing the penguins on the board;
  b) Playing the game.
  6. Placing the penguins:
  a) In a single turn the player places one of his penguins on an unoccupied ice floe with exactly 
  one fish on it.
  b) When placing the penguin, the fish on the target ice floe is collected by the player and 
  removed from the ice floe. This way the ice flow with a penguin does not have any fish.
  c) The move is compulsory. 
  d) The placing phase ends when all the penguins are placed on the game board. 
  7. Playing the game:
  a) In his turn, the player chooses one of his penguins and moves it in a straight line (only along
  the grid lines) to another unoccupied ice floe. The previous ice floe is removed from the 
  board. The move over the field without ice floe or over other penguin is forbidden. When 
  placing the penguin, the fish on the target ice floe is collected by the player and removed 
  from the ice floe, immediately. 
  b) If possible, the move is compulsory.
  c) Game ends when no penguin can make a move.
  8. The player who collects the most fish wins.
# The text file format describing the state of the game
row 1 (board dimension): m n
rows 2 to m+1: n fields separated by single spaces, each field consists of 2 digits: first digit represents 
number of fish (0-3), second digit represents player’s number (1 to at most 9 players, or 0 if
the tile is unoccupied). A combination of numbers 00 represents the grid field without the 
ice floe. In his turn the player modifies the field of departure and field of arrival. The field 
of departure is modified only in the second phase. 
row m+2 and consecutive: 3 fields separated by single spaces: first field is player ID (string of any 
length, without spaces, without quotation marks), second field is player’s number (1 to 9), 
third field shows the number of fish collected so far (the score). If there is no row with 
player’s ID (at the beginning of the game) player must add a corresponding row at the end 
of the file, containing the ID of his choice, consecutive player’s number and the number of 
collected fish (which will be 1 in the case of the first placement action). Otherwise, the 
player should alter his current score by the number of the collected fish, and leave other 
fields unchanged.
Each line of the file may end with any number of spaces.
# Program should accept command line parameters:
● phase=phase_mark – phase_mark can take value placement or movement
● penguins=N, where N is a number of penguins that player has; this parameter is only used if 
phase=placement;
● inputboardfile – name of the file that contains current game state
● outputboardfile – name of the file in which the new game state should be stored
● id – request to display player’s ID end exit the program.
iputboardfile and outputboardfile can be the same file (can have the same name)
Examples:
penguins.exe phase=placement penguins=3 inputboard.txt outpuboard.txt
penguins.exe phase=movement board.txt board.txt
penguins.exe id
If phase=placement, then the program is supposed to read the game state from the file, place one 
penguin on the board, save the game state to the output file and exit. The program cannot place more 
than N penguins on the board (as defined by the penguins parameter). In such a case the program 
exits immediately returning appropriate error code (see later). 
If phase=movement, then the program is supposed to read the game state, move one of his penguins,
alter the score, save the game state and exit. If the move is not possible, the program should exit 
immediately returning appropriate error code (see later).
If parameter id is used, then other parameters are ignored. Program should display only the player’s 
ID hidden in the code. This option enables the game master to identify players.

Interactive game
The program should also be ready to be compiled for an interactive mode (use preprocessor
directives to choose the mode). In the interactive mode the program should read current game state 
from the input file and present it to the user. The user is supposed to make decision about penguin 
placement or movement. The new game state should be stored in an output file. The game state can be 
presented in a text mode (graphics mode is not compulsory

Result returned by the program
Program should end in a controlled way, returning to the operation system one of the values (specified 
as the argument of the return statement in the main function):
0 - program made the correct move
1 - program can not make any move (in the placement phase - all penguins have been placed; in 
the movement phase – all penguins are blocked)
2 – error of the game state in the input file; in such a case additional error message should be 
displayed
3 - internal program error, possibly with additional message
