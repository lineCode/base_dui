// LogFile.cpp: implementation of the LogFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Log.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LogFile::LogFile()
{
    m_dwFileSize = 100;       // 每个日志文件默认大小10m
    memset(m_szFileName, 0, MAX_FILENAME_LEN);
    strncpy(m_szFileName, "Log.log", strlen("Log.log"));
    InitializeCriticalSection(&m_FileCriticalSection);
}

LogFile::LogFile(const char *pszFileName, DWORD dwFileSize)
: m_dwFileSize(dwFileSize)
{
    memset(m_szFileName, 0, MAX_FILENAME_LEN);
    strncpy(m_szFileName, pszFileName, strlen(pszFileName));
    InitializeCriticalSection(&m_FileCriticalSection);
}

void LogFile::Init()
{

}

void LogFile::WriteLog(const char *pszLog)
{
    if (NULL == pszLog)
    {
        return;
    }

    EnterCriticalSection(&m_FileCriticalSection);
    FILE *pFile = NULL;
    pFile = fopen(m_szFileName, "a");
    if (NULL == pFile)
    {
        LeaveCriticalSection(&m_FileCriticalSection);
        return;
    }

//    if (::GetFileSize(pFile, NULL) >= m_dwFileSize)
//    {
//        fseek(pFile, 0, SEEK_SET);        
//    }

    char cTime[32];
    memset(cTime, 0, 32);
    GetSysTime(cTime);
    fwrite(cTime, sizeof(char), strlen(cTime), pFile);
    fwrite(pszLog, sizeof(char), strlen(pszLog), pFile);
    fwrite("\n", 1, 1, pFile);
    fclose(pFile);
    LeaveCriticalSection(&m_FileCriticalSection);    
}

void LogFile::GetSysTime(char *pTime)
{
    if (NULL == pTime)
    {
        return;
    }

    SYSTEMTIME sTime;
    ::GetLocalTime(&sTime);
    sprintf(pTime, "[%d-%02d-%02d %02d:%02d:%02d]", sTime.wYear, sTime.wMonth, 
        sTime.wDay, sTime.wHour, sTime.wMinute, sTime.wSecond);
}

LogFile::~LogFile()
{
    ::DeleteCriticalSection(&m_FileCriticalSection);
}
