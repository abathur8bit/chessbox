# Controller GUI
The Controller GUI runs on the Chessbox PI and is the layer between the user and the controller. The user interacts with the GUI, and it will tell the controller what to do, and display the controllers results.

The GUI manages the chess engine, as this allows you more flexibility on what engine you want to run, as well as letting you run the GUI on different platforms. For example, during development, I run the GUI on a Windows box, and it connects to the controller process running on my Raspberry PI. 

![](img/gui.png)

# Building and running
SDL is used for graphics layer. Download and install before building and running Chessbox. Installs to /usr/local/include/SDL2 and /usr/local/lib.

    sudo apt-get install libxext-dev
    wget https://libsdl.org/release/SDL2-2.0.12.zip
    wget https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.0.4.tar.gz
    wget https://www.libsdl.org/projects/SDL_image/release/SDL2_image-2.0.5.tar.gz
    wget https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-2.0.15.tar.gz
    
    unzip SDL2-2.0.12.zip
    tar xzf SDL2_image-2.0.5.tar.gz
    cd SDL2-2.0.12
    ./configure && make && sudo make install

## Compile
You will need the tiny-process-library to compile the GUI. It provides the ability to launch and control processes, cross platform. This is required to launch and control chess engines. 

    $ cd chessbox
    $ git clone https://github.com/eidheim/tiny-process-library.git
    $ mkdir -p tiny-process-library/build 
    $ cd tiny-process-library/build
    $ cmake ..
    $ make 

Once that is built, you can move onto building controller-gui.

    $ cd ../../controller-gui/
    $ mkdir -p build
    $ cd build
    $ cmake ..
    $ make
    $ cd ..

## Run

Copy **cbgui-sample.json** to **cbgui.json** and update the  controller, engine, and where the current game should be saved to. 

Run with the following command. ESCAPE key exists the program. Assumes that the stockfish executable is located in the GUI's folder. If it's somewhere else, point to that location instead.

    $ build/cbgui -f -e ../engine/stockfish-8-linux/src/stockfish
    
        -f = Full screen
        -e = Location of the engine to use.
