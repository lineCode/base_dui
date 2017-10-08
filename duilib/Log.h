// LogFile.h: interface for the CLogFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGFILE_H__25AD29CF_2499_4A06_80C5_EC68579795CE__INCLUDED_)
#define AFX_LOGFILE_H__25AD29CF_2499_4A06_80C5_EC68579795CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef MAX_FILENAME_LEN
#define MAX_FILENAME_LEN 256
#endif

const DWORD DEFAULT_LOGFILE_SIZE = 10 * 1024 * 1024;        //默认日志大小为10m

class CLogFile  
{
public:
	CLogFile();
    CLogFile(const char *pszFileName, DWORD dwFileSize);
	virtual ~CLogFile();

    void Init();
    void WriteLog(const char *pszLog);
private:
    DWORD m_dwFileSize;
    char  m_szFileName[MAX_FILENAME_LEN];
    CRITICAL_SECTION m_FileCriticalSection;

    void GetSysTime(char *pTime);

};

#endif // !defined(AFX_LOGFILE_H__25AD29CF_2499_4A06_80C5_EC68579795CE__INCLUDED_)
