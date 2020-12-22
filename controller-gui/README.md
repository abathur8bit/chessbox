# Controller GUI
The Controller GUI runs on the Chessbox PI. It is the layer between the user and the controller. 

# Building notes
sudo apt-get install libxext-dev
wget https://libsdl.org/release/SDL2-2.0.12.zip
wget https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.0.4.tar.gz
wget https://www.libsdl.org/projects/SDL_image/release/SDL2_image-2.0.5.tar.gz
wget https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-2.0.15.tar.gz

**Installs to /usr/local/include/SDL2 and /usr/local/lib**

cd SDL2-2.0.12
./configure && make && sudo make install

cd SDL2_mixer-2.0.4
cd SDL2_image-2.0.5



## TTF fonts
if(TTF_Init()==-1) {
    printf("TTF_Init: %s\n", TTF_GetError());
    exit(2);
}