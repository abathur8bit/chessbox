# chesslrcontroller
Board controller for ChessLR e-board. This is the new low level controller that handles the i2c interface. Moving the controller logic from ChessLR project to here also allows me to be able to connect to the controller remotely, from a desktop for instance. 

I broke out the controller from ChessLR for two reasons. The main one was that the Java GPIO lib pi4j has issues when using so many MCP chips. It just randomly messes up the handlers that get called when the input state changes. 

The other reason was to allow for being able to connect from a desktop computer, as it would be easier to annotate the moves using a desktop/laptop then it would be using the touch screen.

**Status:** In progress. Currently prototyping.

# Compiling and Running
You need to have
- [ssobjects](https://github.com/abathur8bit/ssobjects) installed to compile chess controller, as it uses the network to allow remote connections.
- [nlohmann json](https://github.com/nlohmann/json) for json parsing and creation.

## Compile chesslrcontroller

    $ cmake -f CMakeLists.txt
    $ make 

## Running
You might need to use **sudo** when running, since the wiringPI lib needs access to the i2c hardware.

    $ sudo ./chesslrcontroller
    binding to port 9999
    construction
    Turning off led's...
    server running on port 9999

The server is now running and listening for connections on port **9999**. 

## Sending commands
To send chesslrcontroller commands, you connect make a tcp/ip connection to port **9999**. You can do this using **telnet** or **nc**. The preferred way is **nc**, as you can also use it to send batch commands instead of typing commands out by hand.

To test connectivity:

    $ echo '{"action":"ping"}' | nc -C -N localhost 9999
    Hello
    pong

When you first connect, the controller will send a *Hello*. Then the json command **{"action":"ping"}** is sent, and the **pong** response is sent back. 

> Note: you might need to hit Ctrl-C to stop **nc**.

To light up a square on the board

    $ echo '{"action":"move","description":null,"moves":[{"from":"a1","to":"a2","type":"move"}]}' | nc -C -N localhost 9999
    Hello
    move 0 = A2A3 type=move index 48 40
    row 8 col A
    row 1 col H
    Action=move

The squares at a2 and a3 should be lit. 

Capture

    $ echo '{"action":"move","description":null,"moves":[{"from":"a1","to":"a2","type":"capture"}]}' | nc -C -N localhost 9999

## Detecting piece and down
The controller is able to sense when a piece has been put down or lifted up and lets any one connected know. When chesslrcontroller running in a different terminal run **nc** again without the echo in front. When you see the **Hello** you can put a piece down on the A1 square, then lift it up. You should see the following appear:

     $ nc -C localhost 9999
     Hello
     piece at A1 is now occupied
     piece at A1 is now empty

 
 
# Reference board

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

# JSON

Sample JSON

Sample for a normal move:

    {"action":"move","description":null,"moves":[{"from":"e1","to":"g1","type":"move"}]}
    {"action":"move","description":null,"moves":[{"from":"a2","to":"a3","type":"move"}]}
    {"action":"move","description":null,"moves":[{"from":"a7","to":"a6","type":"move"}]}
    {"action":"move","description":null,"moves":[{"from":"a3","to":"a4","type":"move"}]}
    {"action":"move","description":null,"moves":[{"from":"b7","to":"b5","type":"move"}]}
    {"action":"move","description":null,"moves":[{"from":"a1","to":"a3","type":"move"}]}

And another for castling short:

One line:

    {"action":"move","description":"Castle short","moves":[{"from":"e1","to":"g1","type":"move"},{"from":"h1","to":"f1","type":"move"}]}
    
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

# ChessLR replacement Names
Some of the old games are chess challenger and chess mate.   

- Cuttlefish Chess. A little play off Stockfish chess engine, and the image can be the cuttlefish holding a chess piece in its tentacle, or holding a king in one, and the crown broken off in the other.
- pichallenger 
- pi mate
- PiMaster
- PiEChess
- EGambit2020
- EChallenger2020
- PiEGambit, 
- PiEChallenger, 
- DandyTandy, 
- Challenger RS1850
- TandyChess21 for next year
- Gecko eChess

# Referance links
- [Lichess API](https://lichess.org/api)
- [ChessBot API](https://github.com/ShailChoksi/lichess-bot)

# Lichess
I am considering the idea of connecting ChessLR to Lichess so I can use my eboard to play against people. Using their API I can create games, submit moves, and get moves from the other player on Lichess. The connection to Lichess will be via the GUI that connects to chesslrcontroller.

## Explore moves
Rather then submitting the move right away, a mode called **explore moves** will let you make as many moves for white and black as you want without submitting your move. You can hit a button to put the board back to the saved position and try different a variant. When you want to submit the move to lichess, you can make the move and tap a submit button and the move is sent to Lichess. 