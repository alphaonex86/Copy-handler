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
#ifndef __CHARVECT_H__
#define __CHARVECT_H__

#include <vector>

using namespace std;

class char_vector : public vector<PTSTR>
{
public:
	char_vector() : vector<PTSTR>() { };
	char_vector(const char_vector& cv, bool bCopy) { insert(begin(), cv.begin(), cv.end(), bCopy); };
	~char_vector() { erase(begin(), end(), true); };

	void assign(size_type _Count, const PTSTR& _Type, bool bDelete, bool bCopy) { erase(begin(), end(), bDelete); insert(begin(), _Count, _Type, bCopy); };
	template <class _It> void assign(_It _First, _It _Last, bool bDelete, bool bCopy) { erase(begin(), end(), bDelete); insert(begin(), _First, _Last, bCopy); };

	void clear(bool bDelete) { erase(begin(), end(), bDelete); };

	iterator erase(iterator _Where, bool bDelete) { if (bDelete) delete [] (*_Where); return ((vector<PTSTR>*)this)->erase(_Where); };
	iterator erase(iterator _First, iterator _Last, bool bDelete) { if (bDelete) for (iterator _Run=_First;_Run != _Last;_Run++) delete [] (*_Run); return ((vector<PTSTR>*)this)->erase(_First, _Last); };

	void replace(iterator _Where, const PTSTR& _Val, bool bDelete, bool bCopy)
	{
		if (bDelete)
			delete [] (*_Where);
		if (bCopy)
		{
			(*_Where)=new TCHAR[_tcslen(_Val)+1];
			_tcscpy(*_Where, _Val);
		}
		else
			(*_Where)=_Val;
	};
	iterator insert(iterator _Where, const PTSTR& _Val, bool bCopy)
	{
		size_type _O = _Where - begin();
		insert(_Where, 1, _Val, bCopy);
		return (begin() + _O); 
	};
	void insert(iterator _Where, size_type _Count, const PTSTR& _Val, bool bCopy)
	{
		if (bCopy)
		{
			size_type _Size=_tcslen(_Val)+1;
			PTSTR *ppsz=new PTSTR[_Count];
			try
			{
				for (size_type i=0;i<_Count;i++)
				{
					ppsz[i]=new TCHAR[_Size];
					_tcscpy(ppsz[i], _Val);
				}

				((vector<PTSTR>*)this)->insert(_Where, ppsz, ppsz+_Count);
			}
			catch(...)
			{
				delete [] ppsz;
				throw;
			}
			delete [] ppsz;
		}
		else
			((vector<PTSTR>*)this)->insert(_Where, _Count, _Val);
	};
	void insert(iterator _Where, size_type _Count, const PCTSTR& _Val)
	{
		size_type _Size=_tcslen(_Val)+1;
		PTSTR *ppsz=new PTSTR[_Count];
		try
		{
			for(size_type i=0;i<_Count;i++)
			{
				ppsz[i]=new TCHAR[_Size];
				_tcscpy(ppsz[i], _Val);
			}

			((vector<PTSTR>*)this)->insert(_Where, ppsz, ppsz+_Count);
		}
		catch(...)
		{
			delete [] ppsz;
		}
	};
	template<class _It> void insert(iterator _Where, _It _First, _It _Last, bool bCopy)
	{
		if (bCopy)
		{
			size_type _Cnt=_Last-_First;
			PTSTR *ppsz=new PTSTR[_Cnt];
			try
			{
				for (size_type i=0;i<_Cnt;i++)
				{
					ppsz[i]=new TCHAR[_tcslen(*_First)+1];
					_tcscpy(ppsz[i], *_First);

					_First++;
				}

				((vector<PTSTR>*)this)->insert(_Where, ppsz, ppsz+_Cnt);
			}
			catch(...)
			{
				delete [] ppsz;
				throw;
			}
			delete [] ppsz;
		}
		else
			((vector<PTSTR>*)this)->insert(_Where, _First, _Last);
	};

	void swap_items(iterator _Item1, iterator _Item2) { PTSTR psz=(*_Item1); (*_Item1)=(*_Item2); (*_Item2)=psz; };

	void pop_back(bool bDelete) { if (bDelete) delete [] (*(end()-1)); ((vector<PTSTR>*)this)->pop_back(); };
	void push_back(const PTSTR& _Val, bool bCopy) { insert(end(), 1, _Val, bCopy); };
	void push_back(const PCTSTR& _Val) { insert(end(), 1, _Val); };
};

#endif