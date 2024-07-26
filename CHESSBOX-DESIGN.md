# Chessbox

![](img/logo-sm.png)

This document contains information about the design of the software for the GUI and controller, and hardware controller. 
# Building and running
See the main [[README]] for build information. 

# Reference board  

  The following shows the coordinates and indexes for a chess board.
  
```  
*    a  b  c  d  e  f  g  h  
* 8  00 01 02 03 04 05 06 07  8  
* 7  08 09 10 11 12 13 14 15  7  
* 6  16 17 18 19 20 21 22 23  6  
* 5  24 25 26 27 28 29 30 31  5  
* 4  32 33 34 35 36 37 38 39  4  
* 3  40 41 42 43 44 45 46 47  3  
* 2  48 49 50 51 52 53 54 55  2  
* 1  56 57 58 59 60 61 62 63  1  
*    a  b  c  d  e  f  g  h  
```

# Communication protocol
## Actions from controller  
  
### Error response  
If there was an error trying to process a json command, a **success** of **false** is returned, and an **errors** array is returned.  
  
| field   | description                    |  
| ------- | ------------------------------ |  
| errors  | Array of string error messages |  
| success | Will be false                  |  
  
Example:   
  
```json
{  
  "errors": [  
    "invalid from square",  
    "invalid to square"  
  ],  
  "success": false  
}
```  
  
### First connected  
When a client first connects, it gets a hello message with version information  
  
```json  
{  
  "message": "Chessbox controller says hello",  
  "success": true,  
  "version": "1.00.00"  
}  
```  
  
| field   | description                                        |  
| ------- | -------------------------------------------------- |  
| message | Message from chessbox, showing name and hello text |  
| version | The version of the controller.                     |  
| success | Always true                                        |  
  
### Flash  
Make the specified LEDs flash continuously, or turn off flashing. It doesn't matter if the LED is currently on or off, it will start flashing. Turning flash off will return the LED to it's previous state. So if it was off when it started flashing, it will be off when the flashing stops.  
  
| field   | description                                             |  
| ------- | ------------------------------------------------------- |  
| action  | flash                                                     |  
| on      | true to turn on, false to turn off                      |  
| squares | Array of board square numbers (0-63) to turn on or off. |  
  
Example:  
  
```json  
{  
  "action": "flash",  
  "on": true,  
  "squares": [  
    0,    1,    2,    3  ]}  
```  
  
### Led  
Turn on or off a set of LEDs. If the LEDs are already on or already off, nothing happens, and a `success:true` is still returned.  
  
| field   | description                                             |  
| ------- | ------------------------------------------------------- |  
| action  | led                                                     |  
| on      | true to turn on, false to turn off                      |  
| squares | Array of board square numbers (0-63) to turn on or off. |  
  
  
Example:  
  
```json  
{  
  "action": "led",  
  "on": true,  
  "squares": [  
    0,    1,    2,    3  ]}  
```  
  
Response:   
  
```json  
{"success": true}  
```  
  
### Led All  
  
Example:   
  
```json  
{  
  "action": "led_all",  
  "on": false  
}  
```  
  
### Move piece  
Have the board light up LEDs to indicate where to move from and to. Specify the from/to in lan form, which will be 4 characters.  
  
If there is not a piece in the from square a `success:false` will be returned.  
  
The board will wait for the piece to be removed from the *from* square, and put down on the *to* square. Any other pieces lifted or put down will result is a flashing square to indicate an error.  
  
| field  | description                         |
| ------ | ----------------------------------- |
| action | move                                |
| lan    | From and to in lan form. ie: "a2a3" |
  
Example:  
  
```json  
{  
  "action": "move",  
  "lan": "a2a3"  
}  
```  
  
Result:  
  
```json  
{"success":true}  
or  
{"success":false}  
```  
  
#### Completing the move  
As the player moves the piece, you will receive `piece_up` and `piece_down` and once complete, `move`.

Example showing command from GUI and the responses:

```json
-> {"action":"move","lan":"a3a4"}
<- {"success":true}  
<- {"action":"piece_up","lan":"a3","square":40,"success":true}  
<- {"action":"piece_down","lan":"a4","square":32,"success":true}  
<- {"action":"move","from":40,"success":true,"to":32}  
```  
 
### Piece up  
A piece was lifted off the board. Controller sends this to GUI.
  
| field   | description                                             |
| ------- | ------------------------------------------------------- |
| action  | piece_up                                                |
| square  | board coordinate, from 0 thru 63, with 0 being top left |
| lan     | short chess notation                                    |
| success | Always true                                             |

Example:  
  
```json  
{  
  "action": "piece_up",  
  "lan": "c2",  
  "square": 50,  
  "success": true  
}  
```  
  
### Piece down  
A piece was put down onto the board.  
  
| field   | description                                             |
| ------- | ------------------------------------------------------- |
| action  | piece_down                                              |
| square  | board coordinate, from 0 thru 63, with 0 being top left |
| lan     | short chess notation                                    |
| success | Always true                                             |
  
Example:  
  
```json  
{  
  "action": "piece_down",  
  "lan": "c5",  
  "square": 26,  
  "success": true  
}  
```  
  
### Ping 
Just to get a response to see everything is working.   
  
Example:  
  
```json 
{"action":"ping"}  
```  
  
Response:  
  
```json  
{"action":"pong","success":true}
```  
  
### Quit  
Closes the connection to the client.   
  
Example:  
  
```json  
{"action":"quit"}  
```  
  
Response:  
  
```json  
{"message":"Good bye","success":true}
```  
  
### Query LEDs  
Queries the board controller to return what LEDs are currently lit.   
Example:  
  
```json  
{"action": "query_leds"}  
```  
  
Response:   
  
```json  
{  
  "leds_on": [  
    0,    1,    2,    3  ],  
  "success": true  
}  
```  
  
### Query pieces  
Asks board to send what board positions currently sense a piece on them. The response contains all square numbers that have a piece. If there are no pieces on the board, the array will be empty.  
  
Example:  
  
```json  
{"action": "query_pieces"}  
```  
  
Response for a board setup for a new chess game. Any square with a piece has it's square index returned:  
  
```json  
{  
  "has_piece":[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63],  
  "success":true  
}  
```  

Response for an empty board:
  
```json  
{"has_piece":[],"success":true}  
```  
  
  
### Setup pieces  
Tells the board to create a new game and ensure all squares are occupied. Any square that isn't occupied will have the LED solid. Any square that should not be occupied and is, will flash.   
  
Any current move will be reset.  
  
```json
{"action": "setup_pieces",  
"squares": [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63]  
}  
```

# Controller 
See the [[controller/README|controller README]] for build and run instructions. 

The controller runs on the board hardware PI and controls the LEDs and reading piece movement.

# GUI

## Using GUI before connecting
Allows the user to make moves in the GUI, then when connecting, the board will automatically be set the the GUIs position.

## Buttons
| Button              | Meaning                                                      |
| ------------------- | ------------------------------------------------------------ |
| Level               | Change the level of the engine (0 default)                   |
| Skill               | Change the skill of the engine (0 default)                   |
| Depth               | Change the thinking depth      (0 default)                   |
| Time                | Change the amount of time the engine is allowed (0 default)  |
| Connect             | Connect to the board controller                              |
| Load                | Load a PGN game                                              |
| Export              | Export the current game in PGN                               |
| DB                  | Change PGN database file                                     |
| Hint                | Get a good move from the computer                            |
| Inspect             |                                                              |
| Mark                | Mark the current position                                    |
| Flip                | Flip white/black                                             |
| New game as white   | New single player game as white                              |
| New game as black   | New single player game as black                              |
| New two player game |                                                              |
| New two player game |                                                              |
| Menu                |                                                              |
| Back Back           | Goes to the beginning of the game or back to marked position |
| Back                | back one move                                                |
| Forward             | Forward one move                                             |


## Scorecard
The scorecard shows the moves made in the game. You have a few options for how the card is displayed.

Features:
- Add a single move, handles if it is on the left of the card, or right, and what move number it is. 
- Scrolls automatically when you add a new move.
- Highlights the move that was just made.
- If you click on a move, it will change the board to match what the board should be at that position. Sends that board info to the board controller.

### Variations
Variations will be broken onto a separate line, colored differently from the main line.

> See <chess.com> analysis board as an example. ![[Pasted image 20230116000407.png]]

### Notation
A compact style, showing as many moves as possible on a line. If half move wont fit, then it is put on the next line. A full move can be split between lines.

```
1. d4 e6 2. e4 h5 3. Nf3 f5 4. e5 d5 5. Bd3 Nd7 6. O-O h4 7. h3 c5 8. Nfd2 cxd4 9. Qh5+ Rxh5 0-1
```

### Notation scoresheet 
A scoresheet with moves shown in two columns. Piece names are shown in chess notation style. Annotations are not shown. 

```
1. d4    d5
2. c4    c6
3. Nf3   Nf6
```

### Coordinate Scoresheet
A scoresheet with moves shown in two columns. Moves are shown in board cordinates. Piece name will be shown as it does in notation format. Annotations are not shown.

```
1. d2-d4    d7-d5
2. Ng2-f3   Bd2xc3
```


## Screens overview
Load game
Play/view game

Setup game manually
Setup game from FEN

2 player game can optionally use a clock
Clock setup
Multimove

### Layouts
#### Normal
Normal Shows a large board, and small scorecard in notation scoresheet format.

#### Small
Shows a small board, and a large scorecard. 


## Playing
One player example. **Menu > New 1 player game as white** sets the board to a new game, white on the bottom. If the real board doesn't match, LEDs will be lit, and message is shown on screen to set your pieces to match the screen board.

| button                  | function                                                                                              |
| ----------------------- | ----------------------------------------------------------------------------------------------------- |
| connect                 | connects to the board controller                                                                      |
| menu                    | brings up the menu system                                                                             |
| begining                | Moves to the start of the game                                                                        |
| back                    | takes back move                                                                                       |
| forward                 | puts the taken move back                                                                              |
| hint                    | first tap will show what piece the engine would make, second will show where to move the engine piece |
| flag                    | anotate the current position to look at later                                                         |
| mark                    | Remember current position so we can return                                                            |
| play                    | Makes the computer play the current move and continue as that color.                                                             |
| flip                    | flips the onscreen board                                                                              |
| new game as white       | Starts a new game with player as white, on bottom.                                                    |
| new game as black       | Starts a new game with player as black, on bottom.                                                    |
| new two player as white | Starts a new two player game with white on bottom.                                                    |
| new two player as black | Starts a new two player game with black on bottom.                                                    |

### Connect
This will connect and disconnect to the board controller. In later designs, it will be connected automatically. 

### Menu
Brings up menu allowing for starting a new game, setup the board position, load and save games, select a different database and change the engines level settings, turn off the board, setup a clock, change the display layout, change the scorecard style.

### Back
Takes the last move made back. The move that was made stays on the scorecard until a new move is made. If a new move is made, it will make a variation of the move. 

If there are no moves to take back, this does nothing.

### Forward
Puts the board back to a position one forward from the current move. 

If there are now moves forward, this does nothing.

### Hint
Tapping once will show the piece the engine would move. Tapping a second time will show the complete move, what piece and where the engine would move the piece.

> The idea is to allow you to think about the move before having it handed to you. 

The engine will be set to the highest level.

### Mark
This will led the move so you can study the position later.

> Do we want to led the move on the scorecard display?

### Bookmark
When you bookmark a position, Chessbox will remember the position. You will then be able to make as many moves as you want, and then return to the bookmarked position.

Returning to the position can be done two ways. By using #1, you can walk your way back.

1. Tap bookmark and it will start taking moves back until it has returned to the bookmarked position.
2. Tap twice and it will jump to the bookmarked position, showing what the board should look like. 

### Flip board
First tap flips the onscreen board, second tap fips the physical board, third tap returns onscreen and physical board to normal. 

> Normal is white on the bottom of Chessbox.

## Edit position
You can setup the game manually, by choosing a piece then placing it on the board. Or type in a FEN board.

![[Pasted image 20230116002108.png]]

### Shredder edit position example
FEN for below image
```
r1b2k2/pp3qpp/2p5/4n3/3p4/1Q6/PPPN1PPP/2KRR3 w - -
```

![[Pasted image 20221229004804.png]]
Up/down/left/right will shift all pieces that way. Pieces at the side of the board will wrap to the other side. For example, hitting right, will move the rook at A8 to H8.


## Clock
Because it can be hard to place a piece in the right spot right away, we need a way to allow the clock not to penalize the player. Perhaps either a delay for each move, or a delay when the piece is picked up. When the piece is picked up, you get 5 seconds to place the piece before the timer continues for the current player. Once the piece is set down, the clock starts for the other player.

### Count up
Just a timer to show how long each player has taken.

## Handler method example
Trying to figure out how to put methods into a map. So I can just use the action to map to a method call instead of having a if/then code block.   

  
```cpp  
//class Handler {  
//    friend SimServer;  
//    void (SimServer::*handler)(SocketInstance sock,const char* json);  
//    char command[20];  
//    Handler(const char* command,void (SimServer::*handler)(SocketInstance sock,const char* json)) : handler(nullptr) {}  
////    (*this.*amethod)(sock);  
//    void makeCall(const char* json);  
//};
```

# UCIClient
Used for communicating with the chess engine via the [uci protocol](https://8bitcoder.com/uci). 

OptionType
EngineOption
EngineStringOption
EngineCheckOption
EngineSpinOption
EngineComboOption

When we connect we then issue a **uci** command to tell the engine to use the UCI (universal chess interface) and discover the engine options. The end of the options is indicated by **uciok** line.

```
connecting to uci engine at C:/Program Files/tarrasch-v3.12b-g/Engines/stockfish_8_x64.exe
sending command [uci]
read line [Stockfish 8 64 by T. Romstad, M. Costalba, J. Kiiski, G. Linscott]
read line []
read line [id name Stockfish 8 64]
read line [id author T. Romstad, M. Costalba, J. Kiiski, G. Linscott]
read line []
read line [option name Debug Log File type string default ]
read line [option name Contempt type spin default 0 min -100 max 100]
read line [option name Threads type spin default 1 min 1 max 128]
read line [option name Hash type spin default 16 min 1 max 1048576]
read line [option name Clear Hash type button]
read line [option name Ponder type check default false]
read line [option name MultiPV type spin default 1 min 1 max 500]
read line [option name Skill Level type spin default 20 min 0 max 20]
read line [option name Move Overhead type spin default 30 min 0 max 5000]
read line [option name Minimum Thinking Time type spin default 20 min 0 max 5000]
read line [option name Slow Mover type spin default 89 min 10 max 1000]
read line [option name nodestime type spin default 0 min 0 max 10000]
read line [option name UCI_Chess960 type check default false]
read line [option name SyzygyPath type string default <empty>]
read line [option name SyzygyProbeDepth type spin default 1 min 1 max 100]
read line [option name Syzygy50MoveRule type check default true]
read line [option name SyzygyProbeLimit type spin default 6 min 0 max 6]
read line [uciok]
read line []
sending command [setoption name Skill Level value 0]
sending command [ucinewgame]
```


## Options
Options are returned with **uci** command. An option has a name, type, and a default value. Types include string, spin, check, combo, button. Spin will also include a min and max.

```
option name Threads type spin default 1 min 1 max 128
```


## UCIClient::start() 
Starts the engine process, and keeps reading lines and pushing them onto a queue via uciPush(). 

uciParse()
uciPull
uciPush
uciWait
uciClear

# Notation  
Standard algebraic notation (SAN) can be long or short. <https://en.wikipedia.org/wiki/Chess_notation>  
- Long algebraic notation (LAN) includes the starting file and rank of the piece. Can also use a hyphen.  
  - **a4, Nf3, exd5, Bxd5, Qe4+, qe4#**  
  - a2a3, Nb1-c3, Rd3xd7  
- Short algebraic notation omits the starting file and rank of the piece, unless it is necessary to disambiguate the move. This is the standard.  
  - a4, Nf3, exd5, Bxd5, Qe4+, qe4#  
- Reversible algebraic notation (RAN) is based on LAN, but adds an additional letter for the piece that was captured, if any.  
  - Rd2xBd6
  
# FEN notation
[Forsyth-Edwards Notation (FEN)](https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation) is a standard [notation](https://en.wikipedia.org/wiki/Chess_notation "Chess notation") for describing a particular board position of a [chess](https://en.wikipedia.org/wiki/Chess "Chess") game. The purpose of FEN is to provide all the necessary information to restart a game from a particular position.

Other [Chess notations](https://en.wikipedia.org/wiki/Chess_notation#:~:text=store%20a%20game.-,Forsyth%E2%80%93Edwards%20Notation%20(FEN).,number%2C%20and%20color%20on%20move.)

Example of FEN for chess starting postion:

```
rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
```

## Parsing
Each letter respresents the piece. Lowercase is black, upper case is white. 

A FEN record contains six fields, each separated by a space. The fields are as follows:[[5]](https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation#cite_note-5)

1. Piece placement data: Each [rank](https://en.wikipedia.org/wiki/Glossary_of_chess#rank "Glossary of chess") is described, starting with rank 8 and ending with rank 1, with a "/" between each one; within each rank, the contents of the squares are described in order from the a-[file](https://en.wikipedia.org/wiki/Glossary_of_chess#file "Glossary of chess") to the h-file. Each piece is identified by a single letter taken from the standard English names in [algebraic notation](https://en.wikipedia.org/wiki/Algebraic_notation_(chess) "Algebraic notation (chess)") (pawn = "P", knight = "N", bishop = "B", rook = "R", queen = "Q" and king = "K"). White pieces are designated using uppercase letters ("PNBRQK"), while black pieces use lowercase letters ("pnbrqk"). A set of one or more consecutive empty squares within a rank is denoted by a digit from "1" to "8", corresponding to the number of squares.
2. Active color: "w" means that White is to move; "b" means that Black is to move.
3. [Castling](https://en.wikipedia.org/wiki/Castling "Castling") availability: If neither side has the ability to castle, this field uses the character "-". Otherwise, this field contains one or more letters: "K" if White can castle kingside, "Q" if White can castle queenside, "k" if Black can castle kingside, and "q" if Black can castle queenside. A situation that temporarily prevents castling does not prevent the use of this notation.
4. [En passant](https://en.wikipedia.org/wiki/En_passant "En passant") target square: This is a square over which a pawn has just passed while moving two squares; it is given in algebraic notation. If there is no _en passant_ target square, this field uses the character "-". This is recorded regardless of whether there is a pawn in position to capture _en passant_. An updated version of the spec has since made it so the target square is recorded only if a legal _en passant_ capture is possible, but the old version of the standard is the one most commonly used.[[7]](https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation#cite_note-7)[[8]](https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation#cite_note-8)
5. Halfmove clock: The number of halfmoves since the last capture or pawn advance, used for the [fifty-move rule](https://en.wikipedia.org/wiki/Fifty-move_rule "Fifty-move rule").[[9]](https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation#cite_note-9)
6. Fullmove number: The number of the full moves. It starts at 1 and is incremented after Black's move.

# Hardware
## Pieces
### Models
Took then [Knight from thingiverse](https://www.thingiverse.com/thing:4589832/files) and used the top part of it, and used a base from my set to create a new piece.

This [chess set with box](https://www.thingiverse.com/thing:2892472/files) might be good to look at if we wanted to create a printed chess box.

### USCF & FIDE Standards
[Article on chess.com](https://www.chess.com/article/view/chess-board-dimensions) has some excellent info.

USCF
King should be 3.375 to 4.5" tall. 
USCF stanards, squares are 2.25" (57mm), king 3.75" tall.

FIDE
Squares 50-60cm, king 95mm

King base diameter should be 40-50% of the height, and 75-80% of the square size.

King of 3.75", base 1.75 (47% king height), square 2.25".

### New size for printed pieces
Square is 57mm. King is 95mm tall, with base of 45mm. Ratio is based on the size of the king. So the queen height is 79% of the size of the king, and 92% of the base size.

| piece  | height mm | base mm   | height ratio | base ratio |
| ------ | --------- | --------- | ------------ | ---------- |
| king   | 95        | 45 (22.5) | 1            | 1          |
| queen  | 75        | 41 (20.5) | .79          | .92        |
| rook   | 57        | 37 (18.5) | .6           | .82        |
| knight | 63        | 36 (18)   | .66          | .79        |
| bishop | 69        | 35 (17.5) | .73          | .78        |
| pawn   | 50        | 29 (14.5) | .53          | .73        |

### Fancy set I bought for the silicon board
King is 97.8mm tall and 45.8mm wide.
### Large set
Large plastic set for mouse pad board to get height and base ratio.

| piece  | height mm | base mm | H ratio | B ratio |
| ------ | --------- | ------- | ------- | ------- |
| king   | 105       | 48.52   | 1       | 1       |
| queen  | 83.14     | 44.96   | .79     | .92     |
| rook   | 63        | 40      | .6      | .82     |
| knight | 69.5      | 38.3    | .66     | .79     |
| bishop | 76.9      | 38.1    | .73     | .78     |
| pawn   | 55.8      | 35.5    | .53     | .73     |

### My original printed pieces
| piece  | height mm | inch | base mm | inch |
| ------ | --------- | ---- | ------- | ---- |
| king   | 104       | 4.1  | 48      | 1.9  |
| queen  | 92        | 3.6  | 45      | 1.8  |
| rook   | 60        | 2.4  | 42      | 1.6  |
| knight | 75        | 3    | 40      | 1.5  |
| bishop | 78        | 3    | 39      | 1.5  |
| pawn   | 53        | 2    | 34      | 1.3  |

## Board
Square size is 57mm (2.25"). 

Got an idea to print on paper, then assemble into a full sized board. This does a great job of hiding the LEDs when they are off, and letting them shine through when they are on.

Staples has a [Blueprint printing service](https://www.staples.com/services/printing/engineering-blueprints/) that would work for B&W and [Posters](https://www.staples.com/services/printing/product/b904ce0e33e714d7). I'd like to add the logo to the image as I have extra paper to fill.

## Game + Word
Like (6B) on a Saitek clock. Fixed game time (30 min by default) but when the clock reaches zero, it switches to a count up. This allows you to track how much over time the player took. 

## Multi-move
Allows you to enter moves in a single player game, and the computer will not respond with a move. 

This could also be an evaluation button which just remembers what the board position was. then you can make moves to explore a different possibility, then you can use back to keep taking back moves till you get back to where you were. 

# Lichess  
I am considering the idea of connecting Chessbox to Lichess so I can use my eboard to play against people. Using their API I can create games, submit moves, and get moves from the other player on Lichess. The connection to Lichess will be via the GUI that connects to Chessboxcontroller.  
  
## Explore moves  
Rather then submitting the move right away, a mode called **explore moves** will let you make as many moves for white and black as you want without submitting your move. You can hit a button to put the board back to the saved position and try different a variant. When you want to submit the move to lichess, you can make the move and tap a submit button and the move is sent to Lichess.

# Initial Design

> For reference only

![](img/gui2.png)

# Notes

`m_board.m_rules.history_idx` points to the next available slot in `rules.history[]` array and will be odd when white is to play, even when black is to play. `history[0]` isn't used.

thc::ChessRules : ChessPosition : ChessPositionRaw


