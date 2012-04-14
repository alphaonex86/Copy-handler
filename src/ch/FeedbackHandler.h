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
#ifndef __FEEDBACKHANDLER_H__
#define __FEEDBACKHANDLER_H__

#include "../libchcore/FeedbackHandlerBase.h"

class CFileInfo;
enum EFileError;

struct FEEDBACK_ALREADYEXISTS
{
	CFileInfo* pfiSrc;
	CFileInfo* pfiDst;
};

struct FEEDBACK_FILEERROR
{
	const tchar_t* pszSrcPath;
	const tchar_t* pszDstPath;
	EFileError eFileError;			// error type
	ulong_t ulError;				// system error
};

struct FEEDBACK_NOTENOUGHSPACE
{
	ull_t ullRequiredSize;
	const tchar_t* pszSrcPath;
	const tchar_t* pszDstPath;
};

class CFeedbackHandler : public chcore::IFeedbackHandler
{
public:
	enum EFeedbackType
	{
		eFT_Unknown = 0,
		// requests for use feedback
		eFT_FileAlreadyExists,
		eFT_FileError,
		eFT_NotEnoughSpace,
		// notifications
		eFT_OperationFinished,	///< Task has finished processing
		eFT_OperationError,		///< Error encountered while processing task
		eFT_LastType
	};

	enum EFeedbackResult
	{
		eResult_Unknown = 0,
		eResult_Overwrite,
		eResult_CopyRest,
		eResult_Skip,
		eResult_Cancel,
		eResult_Pause,
		eResult_Retry
	};

protected:
	CFeedbackHandler();
	~CFeedbackHandler();

public:
	virtual ull_t RequestFeedback(ull_t ullFeedbackID, ptr_t pFeedbackParam);
	virtual void Delete();

protected:
	EFeedbackResult m_aeFeedbackTypeStatus[eFT_LastType];

	friend class CFeedbackHandlerFactory;
};

class CFeedbackHandlerFactory : public chcore::IFeedbackHandlerFactory
{
protected:
	CFeedbackHandlerFactory() {}
	~CFeedbackHandlerFactory() {}

public:
	chcore::IFeedbackHandler* Create();
	virtual void Delete();

	static IFeedbackHandlerFactory* CreateFactory();
};

#endif
