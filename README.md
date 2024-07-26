# Chessbox

![](img/logo-sm.png)

Chessbox is the sum of three parts. The controller, the GUI, and the hardware. See the [[CHESSBOX-DESIGN]] document for design information, such as GUI layout, and communication protocol etc.

**The hardware** is the chess board with the LEDs, piece switches, MCPs and all the wires. You can read about building that on the [Chessbox blog](https://8bitcoder.com/chessbox).

**The controller** is the software interface to the hardware. It turns the LEDs on/off and uses the piece switches to sense when a piece is on a square. This runs on the PI. 

**The simulator** is software with a GUI that pretends to be the controller and hardware. It allows the GUI to connect and think it is talking to the hardware. The simulator uses the same classes that the controller uses.

**The GUI** is the user interface that a person interacts with to start a new game, set the skill level etc. The GUI connects to a chess engine in order to know how to make moves.

# Build and run
You will need to start the controller or simulator in one terminal, then the GUI in another. You can run the GUI on the PI or you can build and run it on your desktop.

## Simulator
See the simulator section of [[controller-gui/README|GUI README]].

## Controller 
See the [[controller/README|controller README]].

## GUI
See the [[controller-gui/README|GUI README]].

# Directory Structure

| file/dir             | description                                  |
| -------------------- | -------------------------------------------- |
| .git                 | GIT ignore file                              |
| .idea                | IntelliJ project                             |
| .obsidian            | Obsidian settings                            |
| art                  | Chessbox related artwork                     |
| cmake-build-debug    | C build dir                                  |
| common               | Common C files                               |
| controller           | Board controller in C                        |
| controller-gui       | GUI in C                                     |
| docs                 | Documentation and reference material         |
| esp32                | ESP32 related code                           |
| img                  | Images for README.md file                    |
| java_version         | Old Java based version of controller and GUI |
| martin               | Export to martin                             |
| models               | 3D models                                    |
| stockfish            | Expanded stockfish used by C version         |
| tiny-process-library | C lib                                        |

# Reference
- Chessbox [home page](https://8bitcoder.com/chessbox)
- UCI command reference [with notes](https://8bitcoder.com/uci), [original from Shredder](http://download.shredderchess.com/div/uci.zip)
- Chess rules from <https://github.com/billforsternz/thc-chess-library>
- [Decent chess program](https://github.com/billforsternz/tarrasch-chess-gui) I used for capturing FEN and testing PGN files. 
- [Stockfish](https://stockfishchess.org/) chess engine
- [PI4J](https://www.pi4j.com/) previously used when I was using Java. I now use C++ as it is **much** faster on the PI.
- Source for [monochrome 128x64 pixel OLED display (SSD1306)](https://github.com/adafruit/Adafruit_SSD1306)


