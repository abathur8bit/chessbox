1.17.30 

# Chessbox Controller
Board controller for Chessbox e-board. This is the new low level controller that handles the i2c interface. Moving the controller logic from Chessbox project to here also allows me to be able to connect to the controller remotely, from a desktop for instance. 

I broke out the controller from Chessbox for two reasons. The main one was that the Java GPIO lib pi4j has issues when using so many MCP chips. It just randomly messes up the handlers that get called when the input state changes. 

The other reason was to allow for being able to connect from a desktop computer, as it would be easier to annotate the moves using a desktop/laptop then it would be using the touch screen.

**Status:** In progress. Currently prototyping.


# Compiling and Running
You need to have
- [ssobjects](https://github.com/abathur8bit/ssobjects) installed to compile chess controller, as it uses the network to allow remote connections.
- [nlohmann json](https://github.com/nlohmann/json) for json parsing and creation.

## Compile 

  $ cd chessbox/controller
  $ mkdir -p build
  $ cd build
  $ cmake ..
  $ make 
  $ cd ..

## Run
You might need to use **sudo** when running, since the wiringPI lib needs access to the i2c hardware.

> Note: Use the **-s** parameter to swap switches on Lee's board.

```
$ build/cbcontroller
binding to port 9999
construction
Turning off led's...
server m_running on port 9999
```

The server is now m_running and listening for connections on port **9999**. 

## Sending commands
To send Chessboxcontroller commands, you connect make a tcp/ip connection to port **9999**. You can do this using **telnet** or **nc**. The preferred way is **nc**, as you can also use it to send batch commands instead of typing commands out by hand.

To test connectivity:

```
$ echo '{"action":"ping"}' | nc -C -N localhost 9999
Hello
pong
```

When you first connect, the controller will send a *Hello*. Then the json command **{"action":"ping"}** is sent, and the **pong** response is sent back. 

> Note: you might need to hit Ctrl-C to stop **nc**.

To light up a square on the board

```
$ echo '{"action":"move","description":null,"moves":[{"from":"a1","to":"a2","type":"move"}]}' | nc -C -N localhost 9999
Hello
move 0 = A2A3 type=move index 48 40
row 8 col A
row 1 col H
Action=move
```

The squares at a2 and a3 should be lit. 

Capture

```
$ echo '{"action":"move","description":null,"moves":[{"from":"a1","to":"a2","type":"capture"}]}' | nc -C -N localhost 9999
```


echo '{"action":"setmode","mode":"inspect"}' | nc -C -N localhost 9999

## Detecting piece and down
The controller is able to sense when a piece has been put down or lifted up and lets any one connected know. When Chessboxcontroller m_running in a different terminal run **nc** again without the echo in front. When you see the **Hello** you can put a piece down on the A1 square, then lift it up. You should see the following appear:

```
 $ nc -C localhost 9999
 Hello
 piece at A1 is now occupied
 piece at A1 is now empty
```
 
 
# Reference board

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

# Notation
Standard algebraic notation (SAN) can be long or short. <https://en.wikipedia.org/wiki/Chess_notation>
- Long algebraic notation (LAN) includes the starting file and rank of the piece. Can also use a hyphen.
    - **a4, Nf3, exd5, Bxd5, Qe4+, qe4#**
    - a2a3, Nb1-c3, Rd3xd7
- Short algebraic notation omits the starting file and rank of the piece, unless it is necessary to disambiguate the move. This is the standard.
    - a4, Nf3, exd5, Bxd5, Qe4+, qe4#
- Reversible algebraic notation (RAN) is based on LAN, but adds an additional letter for the piece that was captured, if any.
    - Rd2xBd6
    

# JSON

Sample JSON

Sample for a normal move:

```
{"action":"move","description":null,"moves":[{"from":"e1","to":"g1","type":"move"}]}
{"action":"move","description":null,"moves":[{"from":"a2","to":"a3","type":"move"}]}
{"action":"move","description":null,"moves":[{"from":"a7","to":"a6","type":"move"}]}
{"action":"move","description":null,"moves":[{"from":"a3","to":"a4","type":"move"}]}
{"action":"move","description":null,"moves":[{"from":"b7","to":"b5","type":"move"}]}
{"action":"move","description":null,"moves":[{"from":"a1","to":"a3","type":"move"}]}

```
And another for castling short:

One line:

```
{"action":"move","description":"Castle short","moves":[{"from":"e1","to":"g1","type":"move"},{"from":"h1","to":"f1","type":"move"}]}
```

Multiline:    

```
{
  "action": "move",
  "description": "Castle short",
  "moves": [
    {
      "from": "e1",
      "to": "g1",
      "type": "move"
    },
    {
      "from": "h1",
      "to": "f1",
      "type": "move"
    }
  ]
}
```


```json
action:flash,x:1,y:1,speed:fast/slow
action:led,x:1,y:1,on:true/false
action:ledall,on:true/false
action:queryleds
action:querystate

{"acion":"quit"}
{"action":"quit"}
{"action":"led","squares":[1,2,3],"on":true}
{"action":"flash","squares":[0,1,2,3],"on":true}
{"action":"led_all","on":true}
{"action":"led_all","on":false}
{"action":"query_leds"}
response: {"leds_on":[1,2,3],"success":true}
{"action":"query_pieces"}
response: {"has_piece":[0,1,2],"success":true}

{"action":"piece_down","lan":"b8","square":1,"success":true}
{"action":"piece_up","lan":"b8","square":1,"success":true}
```

## Move
Start sequence to show player what piece to move, and where to move it. Move is described in long algebraic notation (LAN). Will send piece up and piece down.

Request
```json
{"action":"move","lan":"a7a6"}
{"action":"move","lan":"A7A6"}
```

Response
```json
{"success":true}
```

Pick up wrong piece
Pick up right piece, put in wrong square
Put a piece on the board
## Query LEDs
Gets the current state of all the leds. true is on, false is off. 
## Query state
Gets the state of all the piece switches. true is occupied, false is empty.


# TODO
- Modes
- Be able to send a FEN command to setup the board

## Modes
### Setup
Used to send FEN board positions. Will light up all the LEDs that are suppose to have pieces on them. If a piece is already there, the LED will turn off. If there is a piece where there should not be, the LED will flash quickly.

{"action":"setmode","mode":"setup"}

fen


### Play
Waits for a command to indicate a move, or to detect when a piece has been lifted. 

# Referance links
- [Lichess API](https://lichess.org/api)
- [ChessBot API](https://github.com/ShailChoksi/lichess-bot)

# Lichess
I am considering the idea of connecting Chessbox to Lichess so I can use my eboard to play against people. Using their API I can create games, submit moves, and get moves from the other player on Lichess. The connection to Lichess will be via the GUI that connects to Chessboxcontroller.

## Explore moves
Rather then submitting the move right away, a mode called **explore moves** will let you make as many moves for white and black as you want without submitting your move. You can hit a button to put the board back to the saved position and try different a variant. When you want to submit the move to lichess, you can make the move and tap a submit button and the move is sent to Lichess. 

# Commands
modified board
echo '{"action":"setposition","fen":"rnbqkbnr/pppppppp/8/8/8/PPPPPPPP/RNBQKBNR/8 w kq - 0 1"}' | nc -C -N chessbox 9999
normal board:
echo '{"action":"setposition","fen":"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w kq - 0 1"}' | nc -C -N chessbox 9999
echo '{"action":"move","description":null,"moves":[{"from":"b3","long":"b3b4","lan":"b4","to":"b4","type":"move"}]}' | nc -C -N chessbox 9999
echo '{"action":"move","description":null,"moves":[{"from":"a2","long":"a2a3","lan":"a3","to":"a3","type":"move"}]}' | nc -C -N chessbox 9999
echo '{"action":"setmode","mode":"inspect"}' | nc -C -N chessbox 9999
rn1qk2r/pp2ppbp/2p1bnp1/8/2pPP3/2N2N2/PP2BPPP/R1BQK2R w KQkq -
{"action":"setposition","fen":"rn1qk2r/pp2ppbp/2p1bnp1/8/2pPP3/2N2N2/PP2BPPP/R1BQK2R w KQkq - 2 8"}
