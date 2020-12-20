//compile with: gcc -Wall -o row rowtest.c -lwiringPi

//pin numbers are BCM

/** LEDs are on bank B, reed switches are on bank A.
 *
 * Pin layout:
 *
 * ```
 *                   +----- -----+
 * LED 7 - GPB0 <--> |1    -   28| <--> GPA7 - Reed switch 7
 * LED 6 - GPB1 <--> |2        27| <--> GPA6 - Reed switch 6
 * LED 5 - GPB2 <--> |3        26| <--> GPA5 - Reed switch 5
 * LED 4 - GPB3 <--> |4    M   25| <--> GPA4 - Reed switch 4
 * LED 3 - GPB4 <--> |5    C   24| <--> GPA3 - Reed switch 3
 * LED 2 - GPB5 <--> |6    P   23| <--> GPA2 - Reed switch 2
 * LED 1 - GPB6 <--> |7    2   22| <--> GPA1 - Reed switch 1
 * LED 0 - GPB7 <--> |8    3   21| <--> GPA0 - Reed switch 0
 *          VDD ---> |9    0   20| ---> INTA
 *          VSS ---> |10   1   19| ---> INTB
 *           NC ---- |11   7   18| ---> RESET
 *          SCL ---> |12       17| <--- A2
 *          SDA <--> |13       16| <--- A1
 *           NC ---- |14       15| <--- A0
 *                   +-----------+
 * ```
 *
 * $ sudo i2cdetect -y 1
 *
 *       0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
 *  00:          -- -- -- -- -- -- -- -- -- -- -- -- --
 *  10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 *  20: 20 21 22 23 24 25 26 27 -- -- -- -- -- -- -- --
 *  30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 *  40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 *  50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 *  60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 *  70: -- -- -- -- -- -- -- --
 */

#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <mcp23017.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>

#define VERSION "1.0"

#define WAIT_SLOW 500
#define WAIT_FAST 100



char* getString(const char* prompt,char* value,int size) {
    char* buffer = (char*)calloc(1,size);
    printf("%s [%s]: ",prompt,value);
    fgets(buffer,size,stdin);
    if(strlen(buffer)) {
        sscanf(buffer,"%s",value);
    }
    free(buffer);

    return value;
}

int getNumber(const char* prompt,int value) {
    char buffer[80];
    printf("%s [%d]: ",prompt,value);
    fgets(buffer,sizeof(buffer),stdin);
    if(strlen(buffer)) {
        sscanf(buffer,"%d",&value);
    }

    return value;
}

int getBool(const char* prompt,int value) {
    char buffer[3] = "N";
    if(value) {
        strcpy(buffer,"Y");
    }
    getString(prompt,buffer,sizeof(buffer));
    if('y' == buffer[0] || 'Y' == buffer[0])
        value=1;
    else
        value=0;

    return value;
}

int writeReg8(int fd,int reg,int data) {
    int retval = wiringPiI2CWriteReg8(fd,reg,data);
    if(-1 == retval) {
        perror("wiringPiI2CWriteReg8");
        exit(-1);
    }
    return retval;
}

int readReg8(int fd,int reg) {
    int data = wiringPiI2CReadReg8(fd,reg);
    if(-1 == data) {
        perror("wiringPiI2CReadReg8");
        exit(-1);
    }
    return data;
}

void readRow(int base,int* rowState,int offset) {
    int i;
    for(i=0; i<8; i++) {
        rowState[offset+i] = digitalRead(base+i);
    }
}

void showRow(int base,int* rowState,int offset) {
    int i;
    for(i=0; i<8; i++) {
        digitalWrite(base+i,rowState[offset+(7-i)]?0:1);
    }
}

#define BASE_I2C 0x50
#define BASE_I2C_INPUT  BASE_I2C     //first 8 pins (bank A)
#define BASE_I2C_OUTPUT BASE_I2C+8   //last 8 pins (bank B)

int main(int argc,char* argv[]) {
    int i,j;
    int rowState[64];
    int rowInputBase[8] =  {BASE_I2C_INPUT, BASE_I2C_INPUT+0x10, BASE_I2C_INPUT+0x20, BASE_I2C_INPUT+0x30, BASE_I2C_INPUT+0x40, BASE_I2C_INPUT+0x50, BASE_I2C_INPUT+0x60, BASE_I2C_INPUT+0x70};
    int rowOutputBase[8] = {BASE_I2C_OUTPUT,BASE_I2C_OUTPUT+0x10,BASE_I2C_OUTPUT+0x20,BASE_I2C_OUTPUT+0x30,BASE_I2C_OUTPUT+0x40,BASE_I2C_OUTPUT+0x50,BASE_I2C_OUTPUT+0x60,BASE_I2C_OUTPUT+0x70};

    memset(rowState,0,sizeof(rowState));
    printf("MCP Tester %s\n",VERSION);

    wiringPiSetup();
    mcp23017Setup(BASE_I2C,0x20);
    mcp23017Setup(BASE_I2C+0x10,0x21);
    mcp23017Setup(BASE_I2C+0x20,0x22);
    mcp23017Setup(BASE_I2C+0x30,0x23);
    mcp23017Setup(BASE_I2C+0x40,0x24);
    mcp23017Setup(BASE_I2C+0x50,0x25);
    mcp23017Setup(BASE_I2C+0x60,0x26);
    mcp23017Setup(BASE_I2C+0x70,0x27);

    for(j=0; j<8; j++) {
        for(i=0; i<8; i++) {
            pinMode(rowOutputBase[j]+i,OUTPUT);
            pinMode(rowInputBase[j]+i,INPUT);
            pullUpDnControl(rowInputBase[j]+i,PUD_UP);
        }
    }

    printf("square lights up when piece down\n");
    while(1)
    {
        for(j=0; j<8; j++) {
//            printf("input base=%02XH output base=%02XH rowstate=%02XH\n",rowInputBase[j],rowOutputBase[j],j*16);
            readRow(rowInputBase[j],rowState,j*8);
            showRow(rowOutputBase[j],rowState,j*8);
        }
        delay(10);
    }

    printf("done\n");

    return 0;
}
