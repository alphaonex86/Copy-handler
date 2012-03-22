/***************************************************************************
*   Copyright (C) 2001-2008 by Józef Starosczyk                           *
*   ixen@copyhandler.com                                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU Library General Public License          *
*   (version 2) as published by the Free Software Foundation;             *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU Library General Public     *
*   License along with this program; if not, write to the                 *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
#ifndef __TASK_H__
#define __TASK_H__

#include "FileInfo.h"
#include "DataBuffer.h"
#include "../libchcore/FeedbackHandlerBase.h"
#include "FileFilter.h"
#include "DestPath.h"

class CDestPath;

#define ST_NULL_STATUS		0x00000000

#define ST_WRITE_MASK		0x000fffff

//------------------------------------
#define ST_STEP_MASK		0x000000ff
#define ST_SEARCHING		0x00000001
#define ST_COPYING			0x00000002
#define ST_DELETING			0x00000003
#define ST_FINISHED			0x00000004
#define ST_CANCELLED		0x00000005

//------------------------------------
#define ST_OPERATION_MASK	0x00000f00
#define ST_COPY				0x00000100
// moving - delete after copying all files
#define ST_MOVE				0x00000200

//------------------------------------
#define ST_SPECIAL_MASK		0x0000f000
// simultaneous flags
#define ST_IGNORE_DIRS		0x00001000
#define ST_IGNORE_CONTENT	0x00002000
#define ST_FORCE_DIRS		0x00004000

//------------------------------------
#define ST_WORKING_MASK		0x000f0000
#define ST_ERROR			0x000C0000
#define ST_PAUSED			0x00080000

//------------------------------------
#define ST_WAITING_MASK		0x00f00000
#define ST_WAITING			0x00100000

///////////////////////////////////////////////////////////////////////////
// Exceptions

#define E_KILL_REQUEST		0x00
#define E_ERROR				0x01
#define E_CANCEL			0x02
#define E_PAUSE				0x03

/////////////////////////////////////////////////////////////////////
// CTask

class CTask;

struct TASK_CREATE_DATA
{
	__int64 *pTasksProcessed;
	__int64 *pTasksAll;

	UINT *puiOperationsPending;
	LONG *plFinished;

	CCriticalSection* pcs;
};

// structure for gettings status of a task
struct TASK_DISPLAY_DATA
{
	CFileInfo m_fi;		// fi at CurrIndex
	int m_iCurrentBufferIndex;
	int m_iIndex;
	int m_iSize;

	CDestPath* m_pdpDestPath;
	CFiltersArray* m_pafFilters;

	UINT	m_uiStatus;
	DWORD	m_dwOsErrorCode;
	CString m_strErrorDesc;

	const BUFFERSIZES* m_pbsSizes;
	int		m_nPriority;

	ull_t	m_ullProcessedSize;
	ull_t	m_ullSizeAll;
	int		m_nPercent;

	long	m_lTimeElapsed;

	unsigned char m_ucCurrentCopy;
	unsigned char m_ucCopies;

	const CString* m_pstrUniqueName;	// doesn't change from first setting

	TCHAR m_szStatusText[_MAX_PATH];
};

struct TASK_MINI_DISPLAY_DATA
{
	CFileInfo m_fi;		// fi at CurrIndex

	UINT	m_uiStatus;
	int		m_nPercent;
};

struct CUSTOM_COPY_PARAMS
{
	CTask* pTask;			// ptr to CTask object on which we do the operation

	CFileInfo* pfiSrcFile;	// CFileInfo - src file
	CString strDstFile;		// dest path with filename

	CDataBuffer dbBuffer;	// buffer handling
	bool bOnlyCreate;		// flag from configuration - skips real copying - only create
	bool bProcessed;		// has the element been processed ? (false if skipped)
};

class CTask
{
public:
	CTask(chcore::IFeedbackHandler* piFeedbackHandler, const TASK_CREATE_DATA *pCreateData);
	~CTask();

	// m_clipboard
	void AddClipboardData(CClipboardEntry* pEntry);
	CClipboardEntry* GetClipboardData(int nIndex);
	int		GetClipboardDataSize();
	int		ReplaceClipboardStrings(CString strOld, CString strNew);

	// m_files
	int FilesAddDir(const CString strDirName, const CFiltersArray* pFilters, int iSrcIndex,
		const bool bRecurse, const bool bIncludeDirs);
	void FilesAdd(CFileInfo fi);
	CFileInfo FilesGetAt(int nIndex);
	CFileInfo& FilesGetAtCurrentIndex();
	void FilesRemoveAll();
	size_t FilesGetSize();

	// m_nCurrentIndex
	void IncreaseCurrentIndex();
	int  GetCurrentIndex();
	void SetCurrentIndex(int nIndex);

	// m_strDestPath
	void SetDestPath(LPCTSTR lpszPath);
	const CDestPath& GetDestPath();
	int GetDestDriveNumber();

	// m_nStatus
	void SetStatus(UINT nStatus, UINT nMask);
	UINT GetStatus(UINT nMask=0xffffffff);

	// m_nBufferSize
	void SetBufferSizes(const BUFFERSIZES* bsSizes);
	const BUFFERSIZES* GetBufferSizes();
	int GetCurrentBufferIndex();

	// m_pThread
	// m_nPriority
	int  GetPriority();
	void SetPriority(int nPriority);

	// m_nProcessed
	void	IncreaseProcessedSize(__int64 nSize);
	void	SetProcessedSize(__int64 nSize);
	__int64 GetProcessedSize();

	// m_nAll
	void	SetAllSize(__int64 nSize);
	__int64 GetAllSize();
	void	CalcAllSize();

	// m_pnTasksProcessed
	void	IncreaseProcessedTasksSize(__int64 nSize);
	void	DecreaseProcessedTasksSize(__int64 nSize);

	// m_pnTasksAll
	void	IncreaseAllTasksSize(__int64 nSize);
	void	DecreaseAllTasksSize(__int64 nSize);

	// m_bKill
	void SetKillFlag(bool bKill=true);
	bool GetKillFlag();

	// m_bKilled
	void SetKilledFlag(bool bKilled=true);
	bool GetKilledFlag();

	void KillThread();
	void CleanupAfterKill();

	// m_strUniqueName
	CString GetUniqueName();

	void Load(icpf::archive& ar, bool bData);
	void Store(bool bData);

	void BeginProcessing();

	void PauseProcessing();		// pause
	void ResumeProcessing();	// resume
	bool RetryProcessing(bool bOnlyErrors=false, UINT uiInterval=0);		// retry
	void RestartProcessing();	// from beginning
	void CancelProcessing();	// cancel

	void GetSnapshot(TASK_DISPLAY_DATA *pData);
	void GetMiniSnapshot(TASK_MINI_DISPLAY_DATA *pData);

	void DeleteProgress(LPCTSTR lpszDirectory);

	void SetOsErrorCode(DWORD dwError, LPCTSTR lpszErrDesc);
	void CalcProcessedSize();

	void DecreaseOperationsPending(UINT uiBy=1);
	void IncreaseOperationsPending(UINT uiBy=1);

	bool CanBegin();

	void UpdateTime();

	void SetFilters(const CFiltersArray* pFilters);
	const CFiltersArray* GetFilters();

	void SetCopies(unsigned char ucCopies);
	unsigned char GetCopies();
	void SetCurrentCopy(unsigned char ucCopy);
	unsigned char GetCurrentCopy();

	CClipboardArray* GetClipboard() { return &m_clipboard; };

	void SetLastProcessedIndex(int iIndex);
	int GetLastProcessedIndex();

	//	CString GetLogName();

	bool GetRequiredFreeSpace(ull_t *pi64Needed, ull_t *pi64Available);

	void SetTaskPath(const tchar_t* pszDir);
	const tchar_t* GetTaskPath() const;

	chcore::IFeedbackHandler* GetFeedbackHandler() const { return m_piFeedbackHandler; }

	void SetForceFlag(bool bFlag=true);
	bool GetForceFlag();
	void SetContinueFlag(bool bFlag=true);
	bool GetContinueFlag();

protected:
	static UINT ThrdProc(LPVOID pParam);
	static void CheckForWaitState(CTask* pTask);
	static void ProcessFiles(CTask* pTask);
	static void CustomCopyFile(CUSTOM_COPY_PARAMS* pData);
	static void DeleteFiles(CTask* pTask);
	static void RecurseDirectories(CTask* pTask);
	static bool SetFileDirectoryTime(LPCTSTR lpszName, CFileInfo* pSrcInfo);
	static bool TimeToFileTime(const COleDateTime& time, LPFILETIME pFileTime);

public:
	//	CLogFile m_log;
	icpf::log_file m_log;
	mutable CCriticalSection m_cs;	// protection for this class

	UINT m_uiResumeInterval;	// works only if the thread is off
	// time
	long m_lTimeElapsed;	// store
	long m_lLastTime;		// not store

	// feedback
	chcore::IFeedbackHandler* m_piFeedbackHandler;

	// ptr to count of currently started tasks
	LONG* m_plFinished;
	bool m_bForce;		// if the continuation of tasks should be independent of limitation
	bool m_bContinue;	// used by ClipboardMonitorProc

protected:
	CClipboardArray m_clipboard;
	CFileInfoArray m_files;
	volatile int m_nCurrentIndex;
	int m_iLastProcessedIndex;

	CDestPath m_dpDestPath;

	volatile UINT m_nStatus;

	// info about last error
	DWORD m_lOsError;
	CString m_strErrorDesc;

	// buffers
	BUFFERSIZES m_bsSizes;

	CWinThread *m_pThread;
	int m_nPriority;

	__int64 m_nProcessed;
	__int64 m_nAll;

	__int64 *m_pnTasksProcessed;
	__int64 *m_pnTasksAll;

	bool m_bQueued;		// has operations pending for this task been increased ?
	UINT *m_puiOperationsPending;

	volatile bool m_bKill;
	volatile bool m_bKilled;

	// other stuff
	CString m_strUniqueName;

	// mask (filter)
	CFiltersArray m_afFilters;

	// copies count
	unsigned char m_ucCopies;
	unsigned char m_ucCurrentCopy;

	tstring_t m_strTaskBasePath;	// base path at which the files will be stored
	bool m_bSaved;		// has the state been saved ('til next modification)

	CCriticalSection* m_pcs;	// protects *m_pnTasksProcessed & *m_pnTasksAll from external array
};

///////////////////////////////////////////////////////////////////////////
// CProcessingException

class CProcessingException
{
public:
	CProcessingException(int iType, CTask* pTask) { m_iType=iType; m_pTask=pTask; m_dwError=0; };
	CProcessingException(int iType, CTask* pTask, UINT uiFmtID, DWORD dwError, ...);
	CProcessingException(int iType, CTask* pTask, DWORD dwError, const tchar_t* pszDesc);

	// Implementation
public:
	int m_iType;	// kill request, error, ...
	CTask* m_pTask;

	CString m_strErrorDesc;
	DWORD m_dwError;
};

///////////////////////////////////////////////////////////////////////////
// CTaskArray

class CTaskArray : public CArray<CTask*, CTask*>
{
public:
	CTaskArray();
	~CTaskArray();

	void Create(chcore::IFeedbackHandlerFactory* piFeedbackHandlerFactory);

	CTask* CreateTask();

	int GetSize( );
	int GetUpperBound( );
	void SetSize( int nNewSize, int nGrowBy = -1 );

	CTask* GetAt( int nIndex );
	//	void SetAt( int nIndex, CTask* newElement );
	int Add( CTask* newElement );

	void RemoveAt( int nIndex, int nCount = 1 );
	void RemoveAll();
	void RemoveAllFinished();
	void RemoveFinished(CTask** pSelTask);

	void SaveData();
	void SaveProgress();
	void LoadDataProgress();

	void TasksBeginProcessing();
	void TasksPauseProcessing();
	void TasksResumeProcessing();
	void TasksRestartProcessing();
	bool TasksRetryProcessing(bool bOnlyErrors=false, UINT uiInterval=0);
	void TasksCancelProcessing();

	ull_t GetPosition();
	ull_t GetRange();
	int	GetPercent();

	UINT GetOperationsPending();
	void SetLimitOperations(UINT uiLimit);
	UINT GetLimitOperations();

	bool IsFinished();

	void SetTasksDir(const tchar_t* pszPath);

public:
	__int64 m_uhRange, m_uhPosition;
	tstring_t m_strTasksDir;

	UINT m_uiOperationsPending;		// count of current operations
	LONG m_lFinished;				// count of finished tasks

	CCriticalSection m_cs;
	TASK_CREATE_DATA m_tcd;

protected:
	chcore::IFeedbackHandlerFactory* m_piFeedbackFactory;
};

#endif
