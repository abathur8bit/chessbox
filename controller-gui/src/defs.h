/********************************************************************
        Copyright (c) 2006, Lee Patterson
        http://ssobjects.sourceforge.net

        created  :  00/00/2000 00:00am
        filename :  defs.h
        author   :  Lee Patterson (workerant@users.sourceforge.net)
        
        purpose  :  common defines 
*********************************************************************/

#ifndef DEFS_H
#define DEFS_H

#include <stdint.h>
#include <sys/types.h>
#include "msdefs.h"

#define PRINTF_BUFFER       8000        //used by loggers, and routines that print to a buffer

//types to keep things simple

//  short sNumber
//  ushort uNumber
//  int iNumber;
//  uint nNumber
//  byte byNumber
//  bool bNumber;

#ifndef WIN32
#    ifndef minimum
# define minimum(x,y) (((x)<(y))?(x):(y))
#    endif

#    ifndef maximum
# define maximum(x,y) (((x)>(y))?(x):(y))
#    endif
#endif

typedef unsigned char byte;     //8 bits
typedef unsigned short word;    //16 bits
typedef short sword;            //16 bits
typedef long dsword;            //32 bits
typedef unsigned long dword;    //32 bites
#ifndef __USE_MISC
typedef unsigned long ulong;
typedef unsigned int uint;
#endif

typedef const           char    CONSTCHAR;              //sz (makes it easier to sort functions headers by fields if const char is one word
typedef const           char*   PCONSTCHAR;             //psz (LPCSTR)
typedef unsigned  char  unsigned8;    //by
typedef signed    char  signed8;      //sb
typedef unsigned  short unsigned16;   //wNumber
typedef signed    short signed16;     //swNumber
typedef uint32_t        unsigned32;   //nNumber
typedef int32_t         signed32;     //iNumber

//needs string.h
#define ZEROMEMORY(address, numBytes)   memset((void*)(address), (int)0, (size_t)(numBytes))
#define FREE_NULL(ptr)                  {free(ptr);ptr=NULL;}
#ifdef __cplusplus
#define DELETE_NULL(ptr)    { delete ptr; ptr = NULL; }
#endif

//other handy defines
#define IS_EVEN(n) ((int)((n)/2)*2 == (n))
#define IS_ODD(n) (!((int)((n)/2)*2 == (n)))
#define NULL_TERMINATE(s,n) ((s[(n)-1] = '\0'))         //make the last character of a char array a null character

//Will select number from and including min, upto and including max.
//TODO: should I add some brackets around max and min like NULL_TERMINATE to make it a safer macro?
#define RND(min,max) (rand()%(max+1-min)+min)

// 
// BREAK is like a breakpoint in a program and will stop the debugger on the
// line following the BREAK call. In debug mode, it has no effect. 
// BREAK_ALWAYS will break in DEBUG and RELEASE mode.
//
// NOTE: If not run in a debugger, it will cause the program to raise a signal
//  and generate a core file. You can then use: "gdb binary core"
//
#ifndef DEBUG
#   define BREAK
#else
#   define BREAK asm("int $3")
#endif

#define BREAK_ALWAYS asm("int $3")


#ifndef UNUSED
#   ifdef DEBUG
#       define UNUSED(x) ((void)x)           //kills warning when a parameter isn't used
#   else
#       define UNUSED(x)
#   endif
#endif

#ifndef UNUSED_ALWAYS
#   define UNUSED_ALWAYS(x) ((void)x)
#endif


//#ifndef OS_LINUX
//defines needed for FreeBSD
//typedef unsigned long unsigned32;
//typedef int socklen_t;
//#endif


#endif //DEFS_H
