/********************************************************************
        Copyright (c) 2006, Lee Patterson
        http://ssobjects.sourceforge.net

        created  :  03/21/2000 9:00pm
        filename :  generalexception.h
        author   :  Lee Patterson (workerant@users.sourceforge.net)
        
        purpose  :  base exception class
*********************************************************************/

#ifndef GENERALEXCEPTION_H
#define GENERALEXCEPTION_H

#include <stdio.h>
#include <stdarg.h>

#include <string>

using namespace std;

namespace ssobjects
{

#define throwGeneralException(m) (throw GeneralException(m,__FILE__,__LINE__))

class GeneralException
{
  private:
    string        m_sMessage;
    string        m_sFullMessage;
    int           m_nError;

  public:       // Constructor/destructor
    GeneralException(const char* pszMessage,const char* pszFname,const int iLine);
    virtual ~GeneralException() {}

  public:
    const char*       getErrorMsg()           {return m_sMessage.c_str();}
    const char*       getFullErrorMsg()       {return m_sFullMessage.c_str();}
    int         getError()              {return m_nError;}
};

};

#endif





