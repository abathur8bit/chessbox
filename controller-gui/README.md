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

# Display
Notes about PI display rotation. [My article](https://8bitcoder.com/chesslr/9) which doesn't work with the xinput command any more. 

First change was to add display_rotate=1 to the /boot/config.txt:

# For more options and information see
# http://www.raspberrypi.org/documentation/configuration/config-txt.md
# Some settings may impact device functionality. See link above for details

# screen rotation
display_rotate=1

# uncomment if you get no picture on HDMI for a default "safe" mode
#hdmi_safe=1
Other values for rotate are:

0   no rotation
1   rotate 90 degrees clockwise
2   rotate 180 degrees clockwise
3   rotate 270 degrees clockwise
This only rotates the display. It doesn't effect the touch input. So you also need to run the following xinput commands. I put them into a shell script to make it easier to automate the process in the next step. I put the script in /home/pi/bin/rot.sh.

**FT5406 memory based driver** isn't found on new image build. But I have seen reference to FB, and dmesg reports **bcm2708_fb**. 
xinput set-prop 'FT5406 memory based driver' 'Evdev Axes Swap' 1
xinput set-prop 'FT5406 memory based driver' 'Evdev Axis Inversion' 0 1


bcm2708_fb
```
[    1.321280] bcm2708_fb soc:fb: FB found 1 display(s)
[    1.337010] Console: switching to colour frame buffer device 60x50
[    1.346517] bcm2708_fb soc:fb: Registered framebuffer for display 0, size 480x800
[    1.354347] bcm2835-rng 3f104000.rng: hwrng registered
[    1.354882] vc-mem: phys_addr:0x00000000 mem_base=0x3ec00000 mem_size:0x40000000(1024 MiB)
[    1.355591] vc-sm: Videocore shared memory driver
```
