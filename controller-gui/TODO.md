- Sprite moveTo call that will move sprite over time.
- Sprite should be centered
- Text button should be able to default to correct size, unless size is specified
- UIGroup should draw at correct location

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