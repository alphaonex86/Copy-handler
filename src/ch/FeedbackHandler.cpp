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
#include "stdafx.h"
#include "FeedbackHandler.h"
#include "FeedbackReplaceDlg.h"
#include "FeedbackFileErrorDlg.h"
#include "FeedbackNotEnoughSpaceDlg.h"
#include "ch.h"
#include "mmsystem.h"
#include "FileInfo.h"

CFeedbackHandler::CFeedbackHandler() :
	chcore::IFeedbackHandler()
{
	memset(m_aeFeedbackTypeStatus, 0, sizeof(m_aeFeedbackTypeStatus));
}

CFeedbackHandler::~CFeedbackHandler()
{

}

ull_t CFeedbackHandler::RequestFeedback(ull_t ullFeedbackID, ptr_t pFeedbackParam)
{
	BOOST_ASSERT(ullFeedbackID < eFT_LastType);
	if(ullFeedbackID >= eFT_LastType)
		return eResult_Unknown;

	// if we have an action selected for this type (e.g. by selecting 'use for all items')
	if(m_aeFeedbackTypeStatus[ullFeedbackID] != eResult_Unknown)
		return m_aeFeedbackTypeStatus[ullFeedbackID];

	// standard processing of feedback
	EFeedbackResult eFeedbackResult = eResult_Unknown;
	BOOL bUseForAllItems = FALSE;
	switch(ullFeedbackID)
	{
	case eFT_FileAlreadyExists:
		{
			BOOST_ASSERT(pFeedbackParam);
			if(!pFeedbackParam)
				return eResult_Unknown;

			FEEDBACK_ALREADYEXISTS* pData = (FEEDBACK_ALREADYEXISTS*)pFeedbackParam;
			CFeedbackReplaceDlg dlg(pData->pfiSrc, pData->pfiDst);
			eFeedbackResult = (EFeedbackResult)dlg.DoModal();
			bUseForAllItems = dlg.m_bAllItems;

			break;
		}
	case eFT_FileError:
		{
			BOOST_ASSERT(pFeedbackParam);
			if(!pFeedbackParam)
				return eResult_Unknown;

			FEEDBACK_FILEERROR* pData = (FEEDBACK_FILEERROR*)pFeedbackParam;
			CFeedbackFileErrorDlg dlg(pData->pszSrcPath, pData->pszDstPath, pData->ulError);
			eFeedbackResult = (EFeedbackResult)dlg.DoModal();
			bUseForAllItems = dlg.m_bAllItems;

			break;
		}
	case eFT_NotEnoughSpace:
		{
			BOOST_ASSERT(pFeedbackParam);
			if(!pFeedbackParam)
				return eResult_Unknown;

			FEEDBACK_NOTENOUGHSPACE* pData = (FEEDBACK_NOTENOUGHSPACE*)pFeedbackParam;
			CFeedbackNotEnoughSpaceDlg dlg(pData->ullRequiredSize, pData->pszSrcPath, pData->pszDstPath);
			eFeedbackResult = (EFeedbackResult)dlg.DoModal();
			bUseForAllItems = dlg.m_bAllItems;

			break;
		}
	case eFT_OperationFinished:
		{
			if(GetConfig().get_bool(PP_SNDPLAYSOUNDS))
			{
				TCHAR* pszPath = new TCHAR[_MAX_PATH];
				GetConfig().get_string(PP_SNDFINISHEDSOUNDPATH, pszPath, _MAX_PATH);
				GetApp().ExpandPath(pszPath);
				PlaySound(pszPath, NULL, SND_FILENAME | SND_ASYNC);
				delete [] pszPath;
			}

			break;
		}
	case eFT_OperationError:
		{
			if(GetConfig().get_bool(PP_SNDPLAYSOUNDS))
			{
				TCHAR* pszPath = new TCHAR[_MAX_PATH];
				GetConfig().get_string(PP_SNDERRORSOUNDPATH, pszPath, _MAX_PATH);
				GetApp().ExpandPath(pszPath);
				PlaySound(pszPath, NULL, SND_FILENAME | SND_ASYNC);
				delete [] pszPath;
			}

			break;
		}
	default:
		BOOST_ASSERT(false);
		return eResult_Unknown;
	}

	// remember feedback option for next time
	if(bUseForAllItems)
		m_aeFeedbackTypeStatus[ullFeedbackID] = eFeedbackResult;

	return eFeedbackResult;
}

void CFeedbackHandler::Delete()
{
	delete this;
}

chcore::IFeedbackHandler* CFeedbackHandlerFactory::Create()
{
	return new CFeedbackHandler;
}

chcore::IFeedbackHandlerFactory* CFeedbackHandlerFactory::CreateFactory()
{
	return new CFeedbackHandlerFactory;
}

void CFeedbackHandlerFactory::Delete()
{
	delete this;
}
