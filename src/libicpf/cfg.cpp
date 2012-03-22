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
/** \file cfg.cpp
 *  \brief A placeholder for config class definitions.
 *  \todo Modify the class to use file class as a file access layer.
 */
 
#include "cfg.h"
#include <assert.h>
#include "exception.h"
#include <vector>
#include <set>
#include "err_codes.h"
#include "cfg_xml.h"
#include "cfg_ini.h"

BEGIN_ICPF_NAMESPACE

//////////////////////////////////////////////////////////////////////////////////
// property_tracker class
#define m_psProperties ((std::set<uint_t>*)m_hProperties)

/** Constructs the property_tracker object.
 */
property_tracker::property_tracker() :
	m_hProperties((ptr_t)new std::set<uint_t>)
{
}

/** Constructs the property_tracker by copying data from source object.
 *
 * \param[in] rSrc - source property tracker
 */
property_tracker::property_tracker(const property_tracker& rSrc) :
	m_hProperties((ptr_t)new std::set<uint_t>(*(std::set<uint_t>*)rSrc.m_hProperties))
{
}

/** Destructs the property tracker object.
 */
property_tracker::~property_tracker()
{
	delete m_psProperties;
}

/** Function adds a new property id to the group.
 *
 * \param[in] uiProp - id of a property to add
 */
void property_tracker::add(uint_t uiProp)
{
	m_psProperties->insert(uiProp);
}

/** Function searches for a specific property id inside the list.
 *
 * \param[in] uiProp - property id to check for
 * \return True if the property has been found, false if not.
 */
bool property_tracker::is_set(uint_t uiProp)
{
	return m_psProperties->find(uiProp) != m_psProperties->end();
}

/** Function returns a count of properties contained in the list.
 *
 * \return A count of id's.
 */
size_t property_tracker::count() const
{
	return m_psProperties->size();
}

/** Function retrieves the id's contained in this tracker by copying
 *  them to the given array.
 *
 * \param[out] puiProps - pointer to the array of uint's to receive id's
 * \param[in] stMaxCount - size of the array (max count of elements to retrieve)
 */
size_t property_tracker::get_ids(uint_t* puiProps, size_t stMaxCount)
{
	size_t tIndex=0;
	for (std::set<uint_t>::iterator it=m_psProperties->begin();it != m_psProperties->end();it++)
	{
		puiProps[tIndex++]=(*it);
		if(tIndex >= stMaxCount)
			break;
	}

	return tIndex;
}

/** Function enumerates id's contained in this property_tracker using
 *  a callback function.
 *
 * \param[in] pfn - function to be called
 * \param[in] pParam - parameter to pass to the callback
 */
void property_tracker::enum_ids(bool(*pfn)(uint_t uiProp, ptr_t pParam), ptr_t pParam)
{
	for (std::set<uint_t>::iterator it=m_psProperties->begin();it != m_psProperties->end();it++)
	{
		if(!(*pfn)((*it), pParam))
			break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////
// config class

#define m_pvProps ((std::vector<property>*)m_hProps)

/** Constructs a config object.
 * \param[in] pCfgBase - pointer to a base handler of the configuration strings
 *						 cound be pointer to xml handler, ini handler or any other
 */
config::config(config_base_types eCfgType) :
	m_lock(),
	m_hProps((ptr_t)new std::vector<property>),
	m_pszCurrentPath(NULL)
{
	switch(eCfgType)
	{
	//case eXml:
	//	m_pCfgBase = new xml_cfg;
	//	break;
	case eIni:
		m_pCfgBase = new ini_cfg;
		break;
	default:
		THROW(_t("Undefined config base type"), 0, 0, 0);
	}
}

/** Destructs the config class.
 */
config::~config()
{
	delete m_pvProps;
	delete [] m_pszCurrentPath;
	delete m_pCfgBase;
}

/** Function opens the specified file using the underlying config base
 *  and converts the values read to a list of properties registered
 *  earlier.
 *
 * \param[in] pszPath - path to a file to be read
 */
void config::read(const tchar_t* pszPath)
{
	assert(pszPath);
	if(!pszPath)
		THROW(_T("Path to the file not provided."), FERR_OPEN, 0, 0);

	// remembering path before operation and not freeing it on fail is done on purpose here
	// (to support future write() to this file in case file does not exist yet)
	size_t stLen = _tcslen(pszPath);
	if(stLen)
	{
		delete [] m_pszCurrentPath;
		m_pszCurrentPath = new tchar_t[stLen + 1];
		_tcscpy(m_pszCurrentPath, pszPath);
		m_pszCurrentPath[stLen] = _t('\0');
	}

	m_lock.lock();
	try
	{
		// read the data using underlying object
		m_pCfgBase->read(pszPath);

		// and transform it to eatable form using registered properties
		load_registered();
	}
	catch(...)
	{
		m_lock.unlock();
		throw;
	}
	m_lock.unlock();
}

/** Reads the configuration data from the provided buffer.
 *
 * \param[in] pszData - pointer to the buffer with data
 * \param[in] stSize - size of the data in buffer
 */
void config::read_from_buffer(const tchar_t* pszData, size_t stSize)
{
	m_lock.lock();
	try
	{
		m_pCfgBase->read_from_buffer(pszData, stSize);

		// and transform it to eatable form using registered properties
		load_registered();
	}
	catch(...)
	{
		m_lock.unlock();
		throw;
	}
	m_lock.unlock();
}

/** Writes all the registered properties into the given file using
 *  the underlying config base to do this.
 *
 * \param[in] pszPath - path to a file to write the properties to
 */
void config::write(const tchar_t* pszPath)
{
	if(!m_pszCurrentPath && !pszPath)
		THROW(_T("No path specified"), FERR_OPEN, 0, 0);
	m_lock.lock();

	try
	{
		// store current properties to the underlying object
		store_registered();

		if(pszPath)
		{
			size_t stLen = _tcslen(pszPath);
			if(stLen)
			{
				delete [] m_pszCurrentPath;
				m_pszCurrentPath = new tchar_t[stLen + 1];
				_tcscpy(m_pszCurrentPath, pszPath);
				m_pszCurrentPath[stLen] = _t('\0');
			}
		}

		// and save
		m_pCfgBase->save(m_pszCurrentPath);
	}
	catch(...)
	{
		m_lock.unlock();
		throw;
	}

	m_lock.unlock();
}

/** Function returns a property type for a given property id.
 *
 * \param[in] uiProp - property id to get info about
 * \return The property type along with its flags.
 */
uint_t config::get_type(uint_t uiProp)
{
	m_lock.lock();
	uint_t uiRet=m_pvProps->at(uiProp).get_type();
	m_lock.unlock();

	return uiRet;
}

/** Retrieves the count of values in the specified property.
 *
 * \param[in] uiProp - property id to retrieve information about
 * \return Count of values.
 */
size_t config::get_value_count(uint_t uiProp)
{
	m_lock.lock();
	size_t stRet=m_pvProps->at(uiProp).get_count();
	m_lock.unlock();

	return stRet;
}

/** Removes an array value at a given index.
 *
 * \param[in] uiProp - property id to have the value removed
 * \param[in] stIndex - index of the value to remove
 */
void config::remove_array_value(uint_t uiProp, size_t stIndex)
{
	m_lock.lock();
	m_pvProps->at(uiProp).remove(stIndex);
	m_lock.unlock();
}

/** Clears the list of values in the given property.
 *
 * \param[in] uiProp - property id to have the values cleared
 */
void config::clear_array_values(uint_t uiProp)
{
	m_lock.lock();
	m_pvProps->at(uiProp).clear_array();
	m_lock.unlock();
}

/** Retrieves the count of registered properties contained in this config.
 *
 * \return Count of properties.
 */
size_t config::count()
{
	return m_pvProps->size();
}

/** Function registers the signed number property. If the underlying base object
 *  contains a string with a specified key - the value is being translated to 
 *  the value of this property.
 *
 * \param[in] pszName - name of the property
 * \param[in] llDef - default value for the property
 * \param[in] llLo - the lower bound of the allowable value range
 * \param[in] llHi - the higher bound of the allowable value range
 * \param[in] uiFlags - additional flags that should be associated with property
 * \return Property ID of the newly registered property.
 */
uint_t config::register_signed_num(const tchar_t* pszName, ll_t llDef, ll_t llLo, ll_t llHi, uint_t uiFlags)
{
	// prepare the property to insert
	property prop(pszName, property::type_signed_num | (uiFlags & property::mask_flags));
	prop.set_signed_range(llLo, llHi);

	// and operate inside the internals
	m_lock.lock();

	// get the value for the property name
	ptr_t hFind=NULL;
	if( (hFind=m_pCfgBase->find(pszName)) != NULL )
	{
		PROPINFO pi;
		while(m_pCfgBase->find_next(hFind, pi))
		{
			assert(!pi.bGroup);
			prop.set_value(pi.pszValue, property::action_add);
		}

		m_pCfgBase->find_close(hFind);
	}
	else if(!(uiFlags & property::flag_array))
		prop.set_signed_num(llDef);

	// add to the vector
	m_pvProps->push_back(prop);
	uint_t uiProp=(uint_t)(m_pvProps->size()-1);

	m_lock.unlock();
		
	return uiProp;
}

/** Function registers the unsigned number property. If the underlying base object
 *  contains a string with a specified key - the value is being translated to 
 *  the value of this property.
 *
 * \param[in] pszName - name of the property
 * \param[in] ullDef - default value for the property
 * \param[in] ullLo - the lower bound of the allowable value range
 * \param[in] ullHi - the higher bound of the allowable value range
 * \param[in] uiFlags - additional flags that should be associated with property
 * \return Property ID of the newly registered property.
 */
uint_t config::register_unsigned_num(const tchar_t* pszName, ull_t ullDef, ull_t ullLo, ull_t ullHi, uint_t uiFlags)
{
	// prepare the property to insert
	property prop(pszName, property::type_unsigned_num | (uiFlags & property::mask_flags));
	prop.set_unsigned_range(ullLo, ullHi);

	// and operate inside the internals
	m_lock.lock();

	// get the value for the property name
	ptr_t hFind=NULL;
	if( (hFind=m_pCfgBase->find(pszName)) != NULL )
	{
		PROPINFO pi;
		while(m_pCfgBase->find_next(hFind, pi))
		{
			assert(!pi.bGroup);
			prop.set_value(pi.pszValue, property::action_add);
		}

		m_pCfgBase->find_close(hFind);
	}
	else if(!(uiFlags & property::flag_array))
		prop.set_unsigned_num(ullDef);

	// add to the vector
	m_pvProps->push_back(prop);
	uint_t uiProp=(uint_t)(m_pvProps->size()-1);

	m_lock.unlock();
		
	return uiProp;
}

/** Function registers the boolean property. If the underlying base object
 *  contains a string with a specified key - the value is being translated to 
 *  the value of this property.
 *
 * \param[in] pszName - name of the property
 * \param[in] bDef - default value for the property
 * \param[in] uiFlags - additional flags that should be associated with property
 * \return Property ID of the newly registered property.
 */
uint_t config::register_bool(const tchar_t* pszName, bool bDef, uint_t uiFlags)
{
	// prepare the property to insert
	property prop(pszName, property::type_bool | (uiFlags & property::mask_flags));

	// and operate inside the internals
	m_lock.lock();

	// get the value for the property name
	ptr_t hFind=NULL;
	if( (hFind=m_pCfgBase->find(pszName)) != NULL )
	{
		PROPINFO pi;
		while(m_pCfgBase->find_next(hFind, pi))
		{
			assert(!pi.bGroup);
			prop.set_value(pi.pszValue, property::action_add);
		}

		m_pCfgBase->find_close(hFind);
	}
	else if(!(uiFlags & property::flag_array))
		prop.set_bool(bDef);

	// add to the vector
	m_pvProps->push_back(prop);
	uint_t uiProp=(uint_t)(m_pvProps->size()-1);

	m_lock.unlock();
		
	return uiProp;
}

/** Function registers the string property. If the underlying base object
 *  contains a string with a specified key - the value is being translated to 
 *  the value of this property.
 *
 * \param[in] pszName - name of the property
 * \param[in] pszDef - default value for the property
 * \param[in] uiFlags - additional flags that should be associated with property
 * \return Property ID of the newly registered property.
 */
uint_t config::register_string(const tchar_t* pszName, const tchar_t* pszDef, uint_t uiFlags)
{
	// prepare the property to insert
	property prop(pszName, property::type_string | (uiFlags & property::mask_flags));

	// and operate inside the internals
	m_lock.lock();

	// get the value for the property name
	ptr_t hFind=NULL;
	if( (hFind=m_pCfgBase->find(pszName)) != NULL )
	{
		PROPINFO pi;
		while(m_pCfgBase->find_next(hFind, pi))
		{
			assert(!pi.bGroup);
			prop.set_value(pi.pszValue, property::action_add);
		}

		m_pCfgBase->find_close(hFind);
	}
	else if(!(uiFlags & property::flag_array))
		prop.set_string(pszDef);

	// add to the vector
	m_pvProps->push_back(prop);
	uint_t uiProp=(uint_t)(m_pvProps->size()-1);

	m_lock.unlock();
		
	return uiProp;
}

/** Function retrieves the value as string.
 *
 * \param[in] uiProp - property to retrieve the value of
 * \param[out] pszBuffer - pointer to a buffer to receive the string (unused
 *						   if retrieving a string value)
 * \param[in] stMaxSize - size of the buffer
 * \param[in] stIndex - index of the value to retrieve (meaningful only for
 *						array-based properties)
 * \return Pointer to the string.
 *
 * \note Always use the returned value instead of the buffer contents. Returned
 *		 value may point to some other memory location instead of pszBuffer.
 */
const tchar_t* config::get_value(uint_t uiProp, tchar_t* pszBuffer, size_t stMaxSize, size_t stIndex)
{
	m_lock.lock();
	if(uiProp >= m_pvProps->size())
	{
		m_lock.unlock();
		THROW(_t("Index out of range"), 0, 0, 0);
	}
	const tchar_t* psz=m_pvProps->at(uiProp).get_value(pszBuffer, stMaxSize, stIndex);
	m_lock.unlock();

	return psz;
}

/** Function retrieves the signed number value.
 *
 * \param[in] uiProp - property to retrieve the value of
 * \param[in] stIndex - index of the value to retrieve (meaningful only for
 *						array-based properties)
 * \return Property value.
 */
ll_t config::get_signed_num(uint_t uiProp, size_t stIndex)
{
	m_lock.lock();
	if(uiProp >= m_pvProps->size())
	{
		m_lock.unlock();
		THROW(_t("Index out of range"), 0, 0, 0);
	}
	ll_t ll=m_pvProps->at(uiProp).get_signed_num(stIndex);
	m_lock.unlock();
	return ll;
}

/** Function retrieves the unsigned number value.
 *
 * \param[in] uiProp - property to retrieve the value of
 * \param[in] stIndex - index of the value to retrieve (meaningful only for
 *						array-based properties)
 * \return Property value.
 */
ull_t config::get_unsigned_num(uint_t uiProp, size_t stIndex)
{
	m_lock.lock();
	if(uiProp >= m_pvProps->size())
	{
		m_lock.unlock();
		THROW(_t("Index out of range"), 0, 0, 0);
	}
	ull_t ull=m_pvProps->at(uiProp).get_unsigned_num(stIndex);
	m_lock.unlock();
	return ull;
}

/** Function retrieves the bool value.
 *
 * \param[in] uiProp - property to retrieve the value of
 * \param[in] stIndex - index of the value to retrieve (meaningful only for
 *						array-based properties)
 * \return Property value.
 */
bool config::get_bool(uint_t uiProp, size_t stIndex)
{
	m_lock.lock();
	if(uiProp >= m_pvProps->size())
	{
		m_lock.unlock();
		THROW(_t("Index out of range"), 0, 0, 0);
	}
	bool b=m_pvProps->at(uiProp).get_bool(stIndex);
	m_lock.unlock();
	return b;
}

/** Function retrieves the string value.
 *
 * \param[in] uiProp - property to retrieve the value of
 * \param[in] stIndex - index of the value to retrieve (meaningful only for
 *						array-based properties)
 * \return Property value.
 */
const tchar_t* config::get_string(uint_t uiProp, size_t stIndex)
{
	m_lock.lock();
	if(uiProp >= m_pvProps->size())
	{
		m_lock.unlock();
		THROW(_t("Index out of range"), 0, 0, 0);
	}
	const tchar_t* psz=m_pvProps->at(uiProp).get_string(stIndex);
	m_lock.unlock();

	return psz;
}

/** Function retrieves the string value.
*
* \param[in] uiProp - property to retrieve the value of
* \param[in] stIndex - index of the value to retrieve (meaningful only for
*						array-based properties)
* \return Property value.
*/
const tchar_t* config::get_string(uint_t uiProp, tchar_t* pszBuffer, size_t stBufferSize, size_t stIndex)
{
	if(!pszBuffer || stBufferSize < 1)
		return NULL;

	m_lock.lock();
	if(uiProp >= m_pvProps->size())
	{
		m_lock.unlock();
		THROW(_t("Index out of range"), 0, 0, 0);
	}
	size_t stLen = 0;
	const tchar_t* psz=m_pvProps->at(uiProp).get_string(stIndex);
	if(psz)
	{
		stLen = _tcslen(psz);
		if(stLen >= stBufferSize)
			stLen = stBufferSize - 1;

		_tcsncpy(pszBuffer, psz, stLen);
	}
	pszBuffer[stLen] = _t('\0');
	m_lock.unlock();
	return pszBuffer;
}

bool config::enum_properties(const tchar_t* pszName, PFNCFGENUMCALLBACK pfn, ptr_t pParam)
{
	ptr_t pFind = m_pCfgBase->find(pszName);
	if(pFind)
	{
		PROPINFO pi;
		while(m_pCfgBase->find_next(pFind, pi))
		{
			(*pfn)(pi.bGroup, pi.pszName, pi.pszValue, pParam);
		}

		m_pCfgBase->find_close(pFind);
		return true;
	}
	else
		return false;
}

/** Function sets the property value from string.
 *
 * \param[in] uiProp - property id to set the value for
 * \param[in] pszVal - string with property value
 * \param[in] a - action to take if the property is array based
 * \param[in] tIndex - index of a value to set at (for action action_setat)
 * \param[out] pTracker - property tracker that collects the property ID's
 */
void config::set_value(uint_t uiProp, const tchar_t* pszVal, property::actions a, size_t tIndex, property_tracker* pTracker)
{
	m_lock.lock();
	m_pvProps->at(uiProp).set_value(pszVal, a, tIndex);
	if(pTracker)
		pTracker->add(uiProp);
	m_lock.unlock();
	property_changed_notify(uiProp);
}

/** Function sets the signed number property value.
 *
 * \param[in] uiProp - property id to set the value for
 * \param[in] llVal - property value to set
 * \param[in] a - action to take if the property is array based
 * \param[in] tIndex - index of a value to set at (for action action_setat)
 * \param[out] pTracker - property tracker that collects the property ID's
 */
void config::set_signed_num(uint_t uiProp, ll_t llVal, property::actions a, size_t tIndex, property_tracker* pTracker)
{
	m_lock.lock();
	m_pvProps->at(uiProp).set_signed_num(llVal, a, tIndex);
	if(pTracker)
		pTracker->add(uiProp);
	m_lock.unlock();
	property_changed_notify(uiProp);
}

/** Function sets the unsigned number property value.
 *
 * \param[in] uiProp - property id to set the value for
 * \param[in] llVal - property value to set
 * \param[in] a - action to take if the property is array based
 * \param[in] tIndex - index of a value to set at (for action action_setat)
 * \param[out] pTracker - property tracker that collects the property ID's
 */
void config::set_unsigned_num(uint_t uiProp, ull_t ullVal, property::actions a, size_t tIndex, property_tracker* pTracker)
{
	m_lock.lock();
	m_pvProps->at(uiProp).set_unsigned_num(ullVal, a, tIndex);
	if(pTracker)
		pTracker->add(uiProp);
	m_lock.unlock();
	property_changed_notify(uiProp);
}

/** Function sets the bool property value.
 *
 * \param[in] uiProp - property id to set the value for
 * \param[in] llVal - property value to set
 * \param[in] a - action to take if the property is array based
 * \param[in] tIndex - index of a value to set at (for action action_setat)
 * \param[out] pTracker - property tracker that collects the property ID's
 */
void config::set_bool(uint_t uiProp, bool bVal, property::actions a, size_t tIndex, property_tracker* pTracker)
{
	m_lock.lock();
	m_pvProps->at(uiProp).set_bool(bVal, a, tIndex);
	if(pTracker)
		pTracker->add(uiProp);
	m_lock.unlock();
	property_changed_notify(uiProp);
}

/** Function sets the string property value.
 *
 * \param[in] uiProp - property id to set the value for
 * \param[in] llVal - property value to set
 * \param[in] a - action to take if the property is array based
 * \param[in] tIndex - index of a value to set at (for action action_setat)
 * \param[out] pTracker - property tracker that collects the property ID's
 */
void config::set_string(uint_t uiProp, const tchar_t* pszVal, property::actions a, size_t tIndex, property_tracker* pTracker)
{
	m_lock.lock();
	m_pvProps->at(uiProp).set_string(pszVal, a, tIndex);
	if(pTracker)
		pTracker->add(uiProp);
	m_lock.unlock();
	property_changed_notify(uiProp);
}

/** Sets the string manually, without using registered properties; does not notify about change.
*
* \param[in] pszName - name of the property
* \param[in] pszVal - value of the property
* \param[in] a - action to take if the property is array based
* \param[in] tIndex - index of a value to set at (for action action_setat)
*/
void config::set_string(const tchar_t* pszName, const tchar_t* pszVal, property::actions a)
{
	config_base::actions action;
	switch(a)
	{
	case property::action_add:
		action = config_base::action_add;
		break;
	case property::action_replace:
		action = config_base::action_replace;
		break;
	default:
		THROW(_t("Undefined or unsupported action."), 0, 0, 0);
	}
	m_pCfgBase->set_value(pszName, pszVal, action);
}

/** Function sets the callback function to be called on property change.
 *  \param[in] pfnCallback - pointer to the function
 *  \param[in] pParam - user defined parameter to pass to the callback
 */
void config::set_callback(PFNPROPERTYCHANGED pfnCallback, ptr_t pParam)
{
	m_pfnNotifyCallback = pfnCallback;
	m_pCallbackParam = pParam;
}

/** Function reads the values for the registered properties from the underlying
 *  base config object.
 */
void config::load_registered()
{
	m_lock.lock();

	ptr_t hFind=NULL;
	for (std::vector<property>::iterator it=m_pvProps->begin();it != m_pvProps->end();it++)
	{
		// is this an array property ?
		if((*it).is_array())
			(*it).clear_array();

		// and fill with value(s)
		if( (hFind=m_pCfgBase->find((*it).get_name())) != NULL)
		{
			PROPINFO pi;
			while(m_pCfgBase->find_next(hFind, pi))
			{
				assert(!pi.bGroup);
				(*it).set_value(pi.pszValue, property::action_add);
			}
		}

		m_pCfgBase->find_close(hFind);
	}

	m_lock.unlock();
}

/** Function stores the values of a registered properties to the underlying
 *  base config object.
 */
void config::store_registered()
{
	m_lock.lock();

	tchar_t szBuffer[128];
	for (std::vector<property>::iterator it=m_pvProps->begin();it != m_pvProps->end();it++)
	{
		// clear the current attributes for the property
		m_pCfgBase->clear((*it).get_name());

		// and fill with value(s)
		size_t tCount=(*it).get_count();
		for (size_t t=0;t != tCount;t++)
		{
			m_pCfgBase->set_value((*it).get_name(), (*it).get_value(szBuffer, 128, t));
		}
	}

	m_lock.unlock();
}

/** Function executes the callback to notify about property value change.
 * \param[in] uiPropID - property ID that changed
 */
void config::property_changed_notify(uint_t uiPropID)
{
	if(m_pfnNotifyCallback)
		(*m_pfnNotifyCallback)(uiPropID, m_pCallbackParam);
}

END_ICPF_NAMESPACE
