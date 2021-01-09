- Engine Options, methods to set/get combos, strings, and checks
- Set level, do I need to also set the movetime? 
- Do I need to set the depth?


- Setup position screen
- Support multiple games in the pgn autosave file. When ever the **new** button is pressed, it will start a new game, and game is added to file.
- Settings screen
    - Computer level options
- histogram
- engine evals

- Sprite moveTo call that will move sprite over time.
- Sprite should be centered
- Text button should be able to default to correct size, unless size is specified
- UIGroup should draw at correct location

# Engine options
Each option is listed when you type **uci**. The option line specifies the type of option it is, and what values it can have. 

Types: string, spin, button, check.

For example:

    uci
    option name Debug Log File type string default
    option name Threads type spin default 1 min 1 max 128
    option name Clear Hash type button
    option name Ponder type check default false

The option **Threads** should display as a spinner in a UI, and have a range of 1-128, when a default of 1. The **Ponder** is a checkbox, and is unchecked by default.

## Stockfish options
option name Debug Log File type string default
option name Contempt type spin default 0 min -100 max 100
option name Threads type spin default 1 min 1 max 128
option name Hash type spin default 16 min 1 max 2048
option name Clear Hash type button
option name Ponder type check default false
option name MultiPV type spin default 1 min 1 max 500
option name Skill Level type spin default 20 min 0 max 20
option name Move Overhead type spin default 30 min 0 max 5000
option name Minimum Thinking Time type spin default 20 min 0 max 5000
option name Slow Mover type spin default 89 min 10 max 1000
option name nodestime type spin default 0 min 0 max 10000
option name UCI_Chess960 type check default false
option name SyzygyPath type string default <empty>
option name SyzygyProbeDepth type spin default 1 min 1 max 100
option name Syzygy50MoveRule type check default true
option name SyzygyProbeLimit type spin default 6 min 0 max 6

# SVG
https://github.com/memononen/nanosvg
https://www.libsdl.org/projects/SDL_image/

# Phase 1
Goal: To be able to play and record a single player game. Each move will be recorded as LAN and SAN in two seperate files. 

# Last row issues

With new game set position mode, where row 1&2&7&8 are lite because they are expecting pieces to be put there.

I put pieces on a,b and the LEDs turn off as expected, but c would also turn off. Put piece on C then the rest of the row would turn off.

Attached row 2 led and reeds to row 1 chip, and still had same issue.

Swapped mcp 7 & 6 and it's working fine again.

sudo i2cdetect -y 1
Row 8 is on MCP 0 (addr 0x20)
Row 1 is on MCP 7 (addr 0x27)

rnbqkbnr/pppppppp/8/8/PPPPPPPP/RNBQKBNR/8/8 w kq -
rnbqkbnr/pppppppp/8/8/PPPPPPPP/RNBQKBNR/8/8 w kq - 0 1
rnbqkbnr/pppppppp/8/8/8/PPPPPPPP/RNBQKBNR/8 w kq - 0 1