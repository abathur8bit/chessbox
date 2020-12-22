/********************************************************************
       Copyright (c) 2006, Lee Patterson
       http://ssobjects.sourceforge.net

       filename :  msdefs.h
       author   :  Lee Patterson (workerant@users.sourceforge.net)
*********************************************************************/

#ifndef MS_DEFS
#define MS_DEFS

//Constants needed to be compatible with MSVC compiler

#ifdef WIN32
# include <wtypes.h>
#endif

#ifdef DEBUG
# ifndef _DEBUG
#   define _DEBUG
# endif
# define __DEBUG
#endif

#ifndef WIN32
#include <sys/types.h>
#endif

//in wtypes.h, ms defines all there types. I' assuming that if one of the types
//are defined, then they all will be defined. I can't test for each of them because 
//some are just typedefed and doesn't have a define with it.

#ifndef _DWORD_DEFINED
#define _WORD_DEFINED
#define _BYTE_DEFINED
#define _DWORD_DEFINED
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef unsigned long DWORD;
typedef unsigned int UINT;
typedef int BOOL;
#endif //!_WORD_DEFINED


#ifndef TRUE
# define TRUE true
# define FALSE false
#endif

#ifdef WIN32
# define strcasecmp stricmp
# define strncasecmp strnicmp
//# define vsnprintf _vsnprintf
//# define snprintf _snprintf
#else
 typedef const char* LPCSTR;
 typedef char* LPSTR;
#endif

//MAXPATHLEN = 1024 (I didn't want to have to include glib.h)
#ifndef WIN32
# ifndef _MAX_PATH
#   define _MAX_PATH 1024
# endif
#endif

#ifndef MAX_PATH
#define MAX_PATH _MAX_PATH
#endif

#endif //MSDEFS
