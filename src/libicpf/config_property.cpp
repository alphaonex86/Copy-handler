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
#include "config_property.h"
#include "exception.h"
#include "err_codes.h"
#include <vector>
#include <assert.h>

BEGIN_ICPF_NAMESPACE

//////////////////////////////////////////////////////////////////////////////////
// property class
// fast access to the array property types
#define m_paStrings ((std::vector<tstring>*)m_val.hArray)
#define m_paSigneds ((std::vector<ll_t>*)m_val.hArray)
#define m_paUnsigneds ((std::vector<ull_t>*)m_val.hArray)
#define m_paBools ((std::vector<bool>*)m_val.hArray)

/** Constructs a property object.
 */
property::property() :
	m_uiPropType(type_unknown | flag_none),
	m_pszName(NULL)
{
	memset(&m_val, 0, sizeof(_VALUE));
	memset(&m_range, 0, sizeof(_RANGE));
}

/** Constructs a property object with type initializer.
 *
 * \param[in] uiType - type and flags to set the property to
 */
property::property(const tchar_t* pszName, uint_t uiType) :
	m_uiPropType(uiType),
	m_pszName(NULL)
{
	memset(&m_val, 0, sizeof(_VALUE));
	memset(&m_range, 0, sizeof(_RANGE));

	// init
	init(pszName, uiType, false);
}

/** Constructs a property object based on some other property object.
 *
 * \param[in] src - a source property object
 */
property::property(const property& src)
{
	copy_from(src, false);
}

/** Destructs the property object.
 */
property::~property()
{
	clear();
}

/** Assigns one property to another.
 *
 * \param[in] rSrc - a source property to copy
 * \return Reference to this object.
 */
property& property::operator=(const property& rSrc)
{
	if (this != &rSrc)
		copy_from(rSrc, true);

	return *this;
}

/** Clears all the internal members.
 */
void property::clear()
{
	// delete the property name
	delete [] m_pszName;
	m_pszName=NULL;

	clear_value();

	// reset other members
	m_uiPropType=type_unknown | flag_none;
}

/** Initializes the property for storaging of the specific property
 *  type. Also sets the property name.
 *  The current property contents are being cleared before setting
 *  the new type.
 *
 * \param[in] uiType - the new property type
 */
void property::init(const tchar_t* pszName, uint_t uiType, bool bClear)
{
	// clear the current stuff
	if (bClear)
		clear();

	// standard members
	m_pszName=copy_string(pszName);
	m_uiPropType=uiType;

	// and alloc memory for the array property types
	if (uiType & flag_array)
	{
		switch(uiType & mask_type)
		{
		case type_string:
			m_val.hArray=(ptr_t)new std::vector<tstring>;
			break;
		case type_signed_num:
			m_val.hArray=(ptr_t)new std::vector<ll_t>;
			m_range.ll.llLo=_I64_MIN;
			m_range.ll.llHi=_I64_MAX;
			break;
		case type_unsigned_num:
			m_val.hArray=(ptr_t)new std::vector<ull_t>;
			m_range.ull.ullLo=0;
			m_range.ull.ullHi=_UI64_MAX;
			break;
		case type_bool:
			m_val.hArray=(ptr_t)new std::vector<bool>;
			break;
		default:
			assert(false);		// unhandled property type
		}
	}
	else
	{
		switch(uiType & mask_type)
		{
		case type_string:
		case type_bool:
			break;
		case type_signed_num:
			m_range.ll.llLo=_I64_MIN;
			m_range.ll.llHi=_I64_MAX;
			break;
		case type_unsigned_num:
			m_range.ull.ullLo=0;
			m_range.ull.ullHi=_UI64_MAX;
			break;
		default:
			assert(false);		// unhandled property type
		}
	}
}

/** Sets a property value from a given string. If this is the array property
 *  type, than the operation is defined by a given action - it either replaces
 *  all the previous values, or adds at the end or replaces value at a specific
 *  index.
 *
 * \param[in] pszValue - value to set (stored in a string)
 * \param[in] a - action to take when the property is array-based
 * \param[in] tIndex - an index at which to place the value (only meaningful
 *                      for array property type).
 */
void property::set_value(const tchar_t* pszValue, actions a, size_t tIndex)
{
	if (m_uiPropType & flag_array)
	{
		switch(m_uiPropType & mask_type)
		{
		case type_string:
			{
				switch(a)
				{
				case action_replace:
					{
						m_paStrings->clear();
						m_paStrings->push_back(tstring(pszValue));
						break;
					}
				case action_add:
					{
						m_paStrings->push_back(tstring(pszValue));
						break;
					}
				case action_setat:
					{
						assert(tIndex < m_paStrings->size());

						tstring& str=m_paStrings->at(tIndex);
						str=pszValue;
						break;
					}
				default:
					assert(false);	// unhandled action type
				}
				break;
			}
		case type_bool:
			{
				switch(a)
				{
				case action_replace:
					{
						m_paBools->clear();
						m_paBools->push_back(bool_from_string(pszValue));
						break;
					}
				case action_add:
					{
						m_paBools->push_back(bool_from_string(pszValue));
						break;
					}
				case action_setat:
					{
						assert(tIndex < m_paBools->size());

						std::vector<bool>::iterator it=m_paBools->begin()+tIndex;
						(*it)=bool_from_string(pszValue);
						break;
					}
				default:
					assert(false);	// unhandled action type
				}
				break;
			}
		case type_signed_num:
			{
				switch(a)
				{
				case action_replace:
					{
						m_paSigneds->clear();
						m_paSigneds->push_back(signed_from_string(pszValue));
						break;
					}
				case action_add:
					{
						m_paSigneds->push_back(signed_from_string(pszValue));
						break;
					}
				case action_setat:
					{
						assert(tIndex < m_paSigneds->size());

						ll_t& ll=m_paSigneds->at(tIndex);
						ll=signed_from_string(pszValue);
						break;
					}
				default:
					assert(false);	// unhandled action type
				}
				break;
			}
		case type_unsigned_num:
			{
				switch(a)
				{
				case action_replace:
					{
						m_paUnsigneds->clear();
						m_paUnsigneds->push_back(unsigned_from_string(pszValue));
						break;
					}
				case action_add:
					{
						m_paUnsigneds->push_back(unsigned_from_string(pszValue));
						break;
					}
				case action_setat:
					{
						assert(tIndex < m_paUnsigneds->size());

						ull_t& ull=m_paUnsigneds->at(tIndex);
						ull=unsigned_from_string(pszValue);
						break;
					}
				default:
					assert(false);	// unhandled action type
				}
				break;
			}
		}
	}
	else
	{
		switch(m_uiPropType & mask_type)
		{
		case type_string:
			{
				delete [] m_val.pszVal;
				m_val.pszVal=copy_string(pszValue);
				break;
			}
		case type_signed_num:
			{
				m_val.llVal=signed_from_string(pszValue);
				break;
			}
		case type_unsigned_num:
			{
				m_val.ullVal=unsigned_from_string(pszValue);
				break;
			}
		case type_bool:
			{
				m_val.bVal=bool_from_string(pszValue);
				break;
			}
		default:
			assert(false);		// not implemented?
		}
	}
}

/** Retrieves the value as a string.
 *
 * \param[out] pszString - pointer to a string that will receive the value (could
 *						   be NULL when retrieving string value type)
 * \param[in] stMaxSize - size of the buffer (could be 0 for string value retrieval)
 * \param[in] stIndex - an index at which to get the value (only meaningful
 *                      for array property type).
 * \return Pointer to the string with value.
 * \note Always use the returned value as a string pointer - it could be different
 *		 than the one provided as a buffer (in case of retrieving string value).
 */
const tchar_t* property::get_value(tchar_t* pszString, size_t stMaxSize, size_t stIndex)
{
	assert(pszString);
	if(!pszString)
		THROW(_t("Invalid argument"), GE_INVALIDARG, 0, 0);

	if (m_uiPropType & flag_array)
	{
		switch(m_uiPropType & mask_type)
		{
		case type_string:
			assert(stIndex < m_paStrings->size());
			return m_paStrings->at(stIndex).c_str();
			break;
		case type_signed_num:
			assert(stIndex < m_paSigneds->size());
			_sntprintf(pszString, stMaxSize, LLFMT, m_paSigneds->at(stIndex));
			break;
		case type_unsigned_num:
			assert(stIndex < m_paUnsigneds->size());
			_sntprintf(pszString, stMaxSize, ULLFMT, m_paUnsigneds->at(stIndex));
			break;
		case type_bool:
			assert(stIndex < m_paBools->size());
			_sntprintf(pszString, stMaxSize, USFMT, (ushort_t)m_paBools->at(stIndex));
			break;
		default:
			assert(false);
		}
	}
	else
	{
		switch(m_uiPropType & mask_type)
		{
		case type_string:
			return m_val.pszVal;
			break;
		case type_signed_num:
			_sntprintf(pszString, stMaxSize, LLFMT, m_val.llVal);
			break;
		case type_unsigned_num:
			_sntprintf(pszString, stMaxSize, ULLFMT, m_val.ullVal);
			break;
		case type_bool:
			_sntprintf(pszString, stMaxSize, USFMT, (ushort_t)m_val.bVal);
			break;
		default:
			assert(false);
		}
	}

	return pszString;
}

/** Sets the string value for this property.
 *
 * \param[in] pszValue - string to set
 * \param[in] a - action to take when property is array-based
 * \param[in] tIndex - index at which to replace value in case of item
 *					   replace action (for array-based property types)
 */
void property::set_string(const tchar_t* pszValue, actions a, size_t tIndex)
{
	assert((m_uiPropType & mask_type) == type_string);

	if (m_uiPropType & flag_array)
	{
		switch(a)
		{
		case action_replace:
			{
				m_paStrings->clear();
				m_paStrings->push_back(tstring(pszValue));
				break;
			}
		case action_add:
			{
				m_paStrings->push_back(tstring(pszValue));
				break;
			}
		case action_setat:
			{
				assert(tIndex < m_paStrings->size());
				tstring& str=m_paStrings->at(tIndex);
				str=pszValue;
				break;
			}
		default:
			assert(false);	// unhandled action type
		}
	}
	else
	{
		delete [] m_val.pszVal;
		m_val.pszVal=copy_string(pszValue);
	}
}

/** Retrieves the string value type.
 *
 * \param[in] stIndex - index at which to retrieve value (meaningful only
 *						for array property type)
 * \return Pointer to the string.
 */
const tchar_t* property::get_string(size_t stIndex) const
{
	assert((m_uiPropType & mask_type) == type_string);

	if (m_uiPropType & flag_array)
	{
		assert(stIndex < m_paStrings->size());
		return m_paStrings->at(stIndex).c_str();
	}
	else
        return m_val.pszVal;
}

/** Sets a signed number property value.
 *
 * \param[in] llValue - signed number value to set
 * \param[in] a - action to take when property is array-based
 * \param[in] tIndex - index at which to replace value in case of item
 *					   replace action (for array-based property types)
 */
void property::set_signed_num(ll_t llValue, actions a, size_t tIndex)
{
	assert((m_uiPropType & mask_type) == type_signed_num);

	if (m_uiPropType & flag_array)
	{
		switch(a)
		{
		case action_replace:
			{
				m_paSigneds->clear();
				m_paSigneds->push_back(llValue);
				break;
			}
		case action_add:
			{
				m_paSigneds->push_back(llValue);
				break;
			}
		case action_setat:
			{
				assert(tIndex < m_paSigneds->size());
				ll_t& ll=m_paSigneds->at(tIndex);
				ll=llValue;
				break;
			}
		default:
			assert(false);	// unhandled action type
		}
	}
	else
		m_val.llVal=llValue;

	check_range();
}

/** Sets the range of the signed number property value.
 *
 * \param[in] llMin - minimum acceptable value of the property
 * \param[in] llMax - maximum acceptable value of the property
 */
void property::set_signed_range(ll_t llMin, ll_t llMax)
{
	assert((m_uiPropType & mask_type) == type_signed_num);

	// set new range
	m_range.ll.llLo=llMin;
	m_range.ll.llHi=llMax;

	// check range
	check_range();
}

/** Retrieves the signed number value.
 *
 * \param[in] stIndex - index at which to retrieve value (array-based
 *						property types)
 * \return Signed number value.
 */
ll_t property::get_signed_num(size_t stIndex) const
{
	assert((m_uiPropType & mask_type) == type_signed_num);

	if (m_uiPropType & flag_array)
	{
		assert(stIndex < m_paSigneds->size());
		return m_paSigneds->at(stIndex);
	}
	else
        return m_val.llVal;
}

/** Sets an unsigned number property value.
 *
 * \param[in] ullValue - unsigned number value to set
 * \param[in] a - action to take when property is array-based
 * \param[in] tIndex - index at which to replace value in case of item
 *					   replace action (for array-based property types)
 */
void property::set_unsigned_num(ull_t ullValue, actions a, size_t tIndex)
{
	assert((m_uiPropType & mask_type) == type_unsigned_num);

	if (m_uiPropType & flag_array)
	{
		switch(a)
		{
		case action_replace:
			{
				m_paUnsigneds->clear();
				m_paUnsigneds->push_back(ullValue);
				break;
			}
		case action_add:
			{
				m_paUnsigneds->push_back(ullValue);
				break;
			}
		case action_setat:
			{
				assert(tIndex < m_paUnsigneds->size());
				ull_t& ull=m_paUnsigneds->at(tIndex);
				ull=ullValue;
				break;
			}
		default:
			assert(false);	// unhandled action type
		}
	}
	else
		m_val.ullVal=ullValue;

	check_range();
}

/** Sets the range of the unsigned number property value.
 *
 * \param[in] ullMin - minimum acceptable value of the property
 * \param[in] ullMax - maximum acceptable value of the property
 */
void property::set_unsigned_range(ull_t ullMin, ull_t ullMax)
{
	assert((m_uiPropType & mask_type) == type_unsigned_num);

	// set new range
	m_range.ull.ullLo=ullMin;
	m_range.ull.ullHi=ullMax;

	// check range
	check_range();
}

/** Retrieves the unsigned number value.
 *
 * \param[in] stIndex - index at which to retrieve value (array-based
 *						property types)
 * \return Unsigned number value.
 */
ull_t property::get_unsigned_num(size_t stIndex) const
{
	assert((m_uiPropType & mask_type) == type_unsigned_num);

	if (m_uiPropType & flag_array)
	{
		assert(stIndex < m_paUnsigneds->size());
		return m_paUnsigneds->at(stIndex);
	}
	else
        return m_val.ullVal;
}

/** Sets a bool property value.
 *
 * \param[in] bValue - bool value to set
 * \param[in] a - action to take when property is array-based
 * \param[in] tIndex - index at which to replace value in case of item
 *					   replace action (for array-based property types)
 */
void property::set_bool(bool bValue, actions a, size_t tIndex)
{
	assert((m_uiPropType & mask_type) == type_bool);

	if (m_uiPropType & flag_array)
	{
		switch(a)
		{
		case action_replace:
			{
				m_paBools->clear();
				m_paBools->push_back(bValue);
				break;
			}
		case action_add:
			{
				m_paBools->push_back(bValue);
				break;
			}
		case action_setat:
			{
				assert(tIndex < m_paBools->size());
				std::vector<bool>::iterator it=m_paBools->begin()+tIndex;
				(*it)=bValue;
				break;
			}
		default:
			assert(false);	// unhandled action type
		}
	}
	else
		m_val.bVal=bValue;
}

/** Retrieves the bool value.
 *
 * \param[in] stIndex - index at which to retrieve value (array-based
 *						property types)
 * \return Bool value.
 */
bool property::get_bool(size_t stIndex) const
{
	assert((m_uiPropType & mask_type) == type_bool);

	if (m_uiPropType & flag_array)
	{
		assert(stIndex < m_paBools->size());
		return m_paBools->at(stIndex);
	}
	else
        return m_val.bVal;
}

/** Retrieves the property count for this property.
 *
 * \return Property count.
 */
size_t property::get_count() const
{
	if (m_uiPropType & flag_array)
	{
		switch(m_uiPropType & mask_type)
		{
		case type_string:
			return m_paStrings->size();
		case type_signed_num:
			return m_paSigneds->size();
		case type_unsigned_num:
			return m_paUnsigneds->size();
		case type_bool:
			return m_paBools->size();
		default:
			assert(false);		// unhandled property type
			return 0;
		}
	}
	else
		return 1;
}

/** Removes a property value at a given index.
 *
 * \param[in] stIndex - index of value to remove
 */
void property::remove(size_t stIndex)
{
	if (m_uiPropType & flag_array)
	{
		switch(m_uiPropType & mask_type)
		{
		case type_string:
			{
				assert(stIndex < m_paStrings->size());
				m_paStrings->erase(m_paStrings->begin()+stIndex);
				break;
			}
		case type_signed_num:
			{
				assert(stIndex < m_paSigneds->size());
				m_paSigneds->erase(m_paSigneds->begin()+stIndex);
				break;
			}
		case type_unsigned_num:
			{
				assert(stIndex < m_paUnsigneds->size());
				m_paUnsigneds->erase(m_paUnsigneds->begin()+stIndex);
				break;
			}
		case type_bool:
			{
				assert(stIndex < m_paBools->size());
				m_paBools->erase(m_paBools->begin()+stIndex);
				break;
			}
		default:
			assert(false);		// unhandled property type
		}
	}
	else
		assert(false);
}

/** Clears the array property value.
 */
void property::clear_array()
{
	if (m_uiPropType & flag_array)
	{
		switch(m_uiPropType & mask_type)
		{
		case type_string:
			m_paStrings->clear();
			break;
		case type_signed_num:
			m_paSigneds->clear();
			break;
		case type_unsigned_num:
			m_paUnsigneds->clear();
			break;
		case type_bool:
			m_paBools->clear();
			break;
		default:
			assert(false);		// unhandled property type
		}
	}
}

/** Completely clears the value part internal members.
 *  Unallocates all the memory associated with values and sets
 *  those members to NULL.
 */
void property::clear_value()
{
	if (m_uiPropType & flag_array)
	{
		switch(m_uiPropType & mask_type)
		{
		case type_string:
			delete m_paStrings;
			break;
		case type_signed_num:
			delete m_paSigneds;
			break;
		case type_unsigned_num:
			delete m_paUnsigneds;
			break;
		case type_bool:
			delete m_paBools;
			break;
		default:
			assert(false);		// unhandled property type
		}

		m_val.hArray=NULL;
	}
	else
	{
		switch(m_uiPropType & mask_type)
		{
		case type_string:
			delete [] m_val.pszVal;
			m_val.pszVal=NULL;
			break;
		case type_signed_num:
			m_val.llVal=0LL;
			break;
		case type_unsigned_num:
			m_val.ullVal=0ULL;
			break;
		case type_bool:
			m_val.bVal=false;
			break;
		default:
			assert(false);		// not implemented?
		}
	}
}

/** Function corrects the property value(s) based on the provided
 *  property value range.
 */
void property::check_range()
{
	if (m_uiPropType & flag_array)
	{
		switch(m_uiPropType & mask_type)
		{
		case type_signed_num:
			{
				for (std::vector<ll_t>::iterator it=m_paSigneds->begin();it != m_paSigneds->end();it++)
				{
					if ((*it) < m_range.ll.llLo)
						(*it)=m_range.ll.llLo;
					else if ((*it) > m_range.ll.llHi)
						(*it)=m_range.ll.llHi;
				}
				break;
			}
		case type_unsigned_num:
			{
				for (std::vector<ull_t>::iterator it=m_paUnsigneds->begin();it != m_paUnsigneds->end();it++)
				{
					if ((*it) < m_range.ull.ullLo)
						(*it)=m_range.ull.ullLo;
					else if ((*it) > m_range.ull.ullHi)
						(*it)=m_range.ull.ullHi;
				}
				break;
			}
		}
	}
	else
	{
		switch(m_uiPropType & mask_type)
		{
		case type_signed_num:
			{
				// check range
				if (m_val.llVal < m_range.ll.llLo)
					m_val.llVal=m_range.ll.llLo;
				else if (m_val.llVal > m_range.ll.llHi)
					m_val.llVal=m_range.ll.llHi;
				break;
			}
		case type_unsigned_num:
			{
				// check range
				if (m_val.ullVal < m_range.ull.ullLo)
					m_val.ullVal=m_range.ull.ullLo;
				else if (m_val.ullVal > m_range.ull.ullHi)
					m_val.ullVal=m_range.ull.ullHi;
				break;
			}
		default:
			assert(false);		// need to be implemented
		}
	}
}

/** Makes a copy of a given string with allocating the necessary memory.
 *
 * \param[in] pszSrc - a source string
 * \return Pointer to a newly allocated memory with new string
 */
tchar_t* property::copy_string(const tchar_t* pszSrc)
{
	if (pszSrc)
	{
		tchar_t *psz=new tchar_t[_tcslen(pszSrc)+1];
		_tcscpy(psz, pszSrc);
		return psz;
	}
	else
		return NULL;
}

/** Converts a string to a boolean value.
 *
 * \param[in] pszSrc - string to convert
 * \return Converted value.
 */
bool property::bool_from_string(const tchar_t* pszSrc)
{
	assert(pszSrc);
	if(!pszSrc)
		THROW(_t("Invalid argument"), GE_INVALIDARG, 0, 0);

	return pszSrc[0] != _t('0');
}

/** Converts a string to a signed number value.
 *
 * \param[in] pszSrc - string to convert
 * \return Converted value.
 */
ll_t property::signed_from_string(const tchar_t* pszSrc)
{
#if defined(_WIN32) || defined(_WIN64)
	return _ttoi64(pszSrc);
#else
	return atoll(pszSrc);
#endif
}

/** Converts a string to an unsigned number value.
 *
 * \param[in] pszSrc - string to convert
 * \return Converted value.
 */
ull_t property::unsigned_from_string(const tchar_t* pszSrc)
{
	// currently does not support full range of unsigned long long
	// since there are no (?) function to convert string to ull_t
#if defined(_WIN32) || defined(_WIN64)
	return _ttoi64(pszSrc);
#else
	return atoll(pszSrc);
#endif
}

/** Function makes a copy of a given property storing it in this one.
 *
 * \param[in] rSrc - property to copy from
 * \param[in] bClear - should we cleat current contents first
 */
void property::copy_from(const property& rSrc, bool bClear)
{
	// clear values in this class
	if (bClear)
		clear_value();

	// copy the value(s)
	if (rSrc.m_uiPropType & flag_array)
	{
		// source property is an array
		switch(rSrc.m_uiPropType & mask_type)
		{
		case type_string:
			m_val.hArray=new std::vector<tstring>(*(std::vector<tstring>*)rSrc.m_val.hArray);
			break;
		case type_signed_num:
			m_val.hArray=new std::vector<ll_t>(*(std::vector<ll_t>*)rSrc.m_val.hArray);
			break;
		case type_unsigned_num:
			m_val.hArray=new std::vector<ull_t>(*(std::vector<ull_t>*)rSrc.m_val.hArray);
			break;
		case type_bool:
			m_val.hArray=new std::vector<bool>(*(std::vector<bool>*)rSrc.m_val.hArray);
			break;
		default:
			assert(false);	// unknown property type
		}
	}
	else
	{
		// source property is normal value
		switch(rSrc.m_uiPropType & mask_type)
		{
		case type_string:
			{
				m_val.pszVal=copy_string(rSrc.m_val.pszVal);
				break;
			}
		case type_signed_num:
			{
				m_val.llVal=rSrc.m_val.llVal;
				m_range.ll.llHi=rSrc.m_range.ll.llHi;
				m_range.ll.llLo=rSrc.m_range.ll.llLo;
				break;
			}
		case type_unsigned_num:
			{
				m_val.ullVal=rSrc.m_val.ullVal;
				m_range.ull.ullHi=rSrc.m_range.ull.ullHi;
				m_range.ull.ullLo=rSrc.m_range.ull.ullLo;
				break;
			}
		case type_bool:
			{
				m_val.bVal=rSrc.m_val.bVal;
				break;
			}
		default:
			assert(false);		// property type not implemented?
		}
	}

	// copy values
	m_uiPropType=rSrc.m_uiPropType;
	m_pszName=copy_string(rSrc.m_pszName);
}

END_ICPF_NAMESPACE
