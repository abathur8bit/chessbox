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
```

response:

```json
{"action":"pong","success":true}
```

When you first connect, the controller will send a *Hello* message. See the communication protocol of the [[CHESSBOX-DESIGN]]

> Note: you might need to hit Ctrl-C to stop **nc**.

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
  
# Referance links
- [Lichess API](https://lichess.org/api)
- [ChessBot API](https://github.com/ShailChoksi/lichess-bot)

