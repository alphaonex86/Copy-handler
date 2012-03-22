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
 /** \file callback.cpp
 *  \brief File provides the implementation of callback classes.
 */
#include "callback.h"
#include <vector>
#include <assert.h>

BEGIN_ICPF_NAMESPACE

#define STORAGE ((std::vector<CLBDATA>*)m_pStorage)

callback_list::callback_list() :
	m_lock(),
	m_pStorage((void*)new std::vector<CLBDATA>)
{
}

callback_list::callback_list(const callback_list& rSrc) :
	m_lock(),
	m_pStorage((void*)new std::vector<CLBDATA>)
{
	STORAGE->assign(((std::vector<CLBDATA>*)rSrc.m_pStorage)->begin(), ((std::vector<CLBDATA>*)rSrc.m_pStorage)->end());
	assert(false);		// we should not use the copy constructor at all !!!
}

callback_list::~callback_list()
{
	try
	{
		delete STORAGE;
	}
	catch(...)
	{

	}
}

const callback_list& callback_list::operator=(const callback_list& rSrc)
{
	assert(false);		// we shouldn't use the assignment operator at all!!!
	if (this != &rSrc)
	{
		delete STORAGE;
		m_pStorage=(void*)new std::vector<CLBDATA>;
		STORAGE->assign(((std::vector<CLBDATA>*)rSrc.m_pStorage)->begin(), ((std::vector<CLBDATA>*)rSrc.m_pStorage)->end());
	}

	return *this;
}

void callback_list::add(PFNFUNC pfn, ptr_t param)
{
	m_lock.lock();
	CLBDATA clb = { pfn, param };
	STORAGE->push_back(clb);
	m_lock.unlock();
}

bool callback_list::remove(PFNFUNC pfn)
{
	m_lock.lock();
	for (std::vector<CLBDATA>::iterator it=STORAGE->begin();it != STORAGE->end();it++)
	{
		if ((*it).pfn == pfn)
		{
			STORAGE->erase(it);
			m_lock.unlock();
			return true;
		}
	}

	m_lock.unlock();
	return false;
}

void callback_list::clear()
{
	m_lock.lock();
	STORAGE->clear();
	m_lock.unlock();
}

size_t callback_list::size()
{
	m_lock.lock();
	size_t tSize=STORAGE->size();
	m_lock.unlock();

	return tSize;
}

CLBDATA* callback_list::at(size_t tIndex)
{
	CLBDATA* pData=NULL;
	m_lock.lock();
	if (tIndex < STORAGE->size())
		pData=&(STORAGE->at(tIndex));
	m_lock.unlock();

	return pData;
}

void callback_list::lock()
{
	m_lock.lock();
}

void callback_list::unlock()
{
	m_lock.unlock();
}


END_ICPF_NAMESPACE
