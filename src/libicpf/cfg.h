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
#ifndef __CFG_H__
#define __CFG_H__

/** \file cfg.h
 *  \brief A placeholder for config class.
 */
#include "mutex.h"
#include "libicpf.h"
#include "gen_types.h"
#include "config_base.h"
#include "config_property.h"

BEGIN_ICPF_NAMESPACE

/// Callback function definition
typedef void(*PFNPROPERTYCHANGED)(uint_t, ptr_t);
/// Enumeration callback
typedef void(*PFNCFGENUMCALLBACK)(bool, const tchar_t*, const tchar_t*, ptr_t);

/** \brief Property group handling class
 *
 *  Class is being used to manipulate the property groups (in connection with config::begin_group() and
 *  config::end_group().
 */
class LIBICPF_API property_tracker
{
public:
/** \name Construction/destruction/operators */
/**@{*/
	property_tracker();											///< Standard constructor
	property_tracker(const property_tracker& rSrc);				///< Copy constructor
	~property_tracker();										///< Standard destructor

	property_tracker& operator=(const property_tracker& rSrc);	///< Assignment operator
/**@}*/

/** \name Operations */
/**@{*/
	void add(uint_t uiProp);			///< Adds a new property id to the list
	bool is_set(uint_t uiProp);			///< Checks if a property id is set inside this list
	size_t count() const;				///< Returns a count of properties in a list

	/// Retrieves the list of ID's
	size_t get_ids(uint_t* puiProps, size_t stMaxCount);
	/// Retrieves the list of ID's using an enumeration function
	void enum_ids(bool(*pfn)(uint_t uiProp, ptr_t pParam), ptr_t pParam);
/**@}*/

protected:
	ptr_t m_hProperties;				///< Internal member. Pointer to a storage structure with an int_t.
};

/** \brief Configuration management class.
 *
 *  Class allows user to read and write configuration file in standard unix
 *  format (comments, empty lines and key=value strings). Class is fully thread-safe.
 *  Access to the properties is done by registering one and then getting or setting
 *  a value using the property identifier.
 */
class LIBICPF_API config
{
public:
	enum config_base_types
	{
		eXml,
		eIni
	};
public:
/** \name Construction/destruction */
/**@{*/
	config(config_base_types eCfgType);	///< Standard constructor
	virtual ~config();						///< Standard destructor
/**@}*/
	
/** \name Reading and writing to the external medium */
/**@{*/
	void read(const tchar_t *pszPath);		///< Reads the properties from the source file
	void read_from_buffer(const tchar_t* pszData, size_t stSize);
	void write(const tchar_t* pszPath);		///< Saves the properties to the file
/**@}*/

/** \name Class lock/unlock functions */
/**@{*/
	/// Locks the config class for one thread
	void lock() { m_lock.lock(); };
	/// Unlocks the class
	void unlock() { m_lock.unlock(); };
/**@}*/
	
	// property type management
/** Property types */
/**@{*/
	uint_t get_type(uint_t uiProp);						///< Retrieves the property type
	size_t get_value_count(uint_t uiProp);				///< Retrieves the count of values for array-based property types
	void remove_array_value(uint_t uiProp, size_t stIndex);	///< Removes a value at a specified index in array-based property type
	void clear_array_values(uint_t uiProp);				///< Removes all values in array-based property
	size_t count();										///< Retrieves the count of properties contained in this config
/**@}*/

	// registering the properties
/** \name Properties registration functions */
/**@{*/
	/// Registers signed number-type property
	uint_t register_signed_num(const tchar_t* pszName, ll_t llDef, ll_t llLo, ll_t llHi, uint_t uiFlags=property::flag_none);
	/// Registers unsigned number-type property
	uint_t register_unsigned_num(const tchar_t* pszName, ull_t ullDef, ull_t ullLo, ull_t ullHi, uint_t uiFlags=property::flag_none);
	/// Registers bool-type property
	uint_t register_bool(const tchar_t* pszName, bool bDef, uint_t uiFlags=property::flag_none);
	/// Registers string-type property
	uint_t register_string(const tchar_t* pszName, const tchar_t* pszDef, uint_t uiFlags=property::flag_none);
/**@}*/
	
	// getting property data
/** \name Getting and setting values */
/**@{*/
	/// Gets the value of string-type property
	const tchar_t* get_value(uint_t uiProp, tchar_t* pszBuffer, size_t stMaxSize, size_t stIndex=0);
	/// Gets the value of longlong_t-type property
	ll_t get_signed_num(uint_t uiProp, size_t stIndex=0);
	/// Gets the value of ulonglong_t-type property
	ull_t get_unsigned_num(uint_t uiProp, size_t stIndex=0);
	/// Gets the value of bool-type property
	bool get_bool(uint_t uiProp, size_t stIndex=0);
	/// Gets the value of string-type property
	const tchar_t* get_string(uint_t uiProp, size_t stIndex=0);
	/// Retrieves the copy of the string
	const tchar_t* get_string(uint_t uiProp, tchar_t* pszBuffer, size_t stBufferSize, size_t stIndex=0);

	/// Enumerates attributes (and groups)
	bool enum_properties(const tchar_t* pszName, PFNCFGENUMCALLBACK pfn, ptr_t pParam);

	// setting property data
	/// Sets the value from the string
	void set_value(uint_t uiProp, const tchar_t* pszVal, property::actions a=property::action_replace, size_t tIndex=0, property_tracker* pTracker=NULL);
	/// Sets the value of longlong_t-type property
	void set_signed_num(uint_t uiProp, ll_t llVal, property::actions a=property::action_replace, size_t tIndex=0, property_tracker* pTracker=NULL);
	/// Sets the value of ulonglong_t-type property
	void set_unsigned_num(uint_t uiProp, ull_t ullVal, property::actions a=property::action_replace, size_t tIndex=0, property_tracker* pTracker=NULL);
	/// Sets the value of bool-type property
	void set_bool(uint_t uiProp, bool bVal, property::actions a=property::action_replace, size_t tIndex=0, property_tracker* pTracker=NULL);
	/// Sets the value of string-type property
	void set_string(uint_t uiProp, const tchar_t* pszVal, property::actions a=property::action_replace, size_t tIndex=0, property_tracker* pTracker=NULL);
	/// Sets the string manually, without using registered properties; does not notify about change.
	void set_string(const tchar_t* pszName, const tchar_t* pszVal, property::actions a=property::action_replace);
/**@}*/

/** \name Notifications */
/**@{*/
	void set_callback(PFNPROPERTYCHANGED pfnCallback, ptr_t pParam);
/**@}*/

protected:
	void load_registered();			///< Loads the registered property values from the underlying config base
	void store_registered();		///< Stores the registered property values to the underlying config base

	void property_changed_notify(uint_t uiPropID);	///< Calls the callback function to notify about the property value change
protected:
	mutex m_lock;					///< Lock for the multi-threaded access to the properties
	ptr_t m_hProps;					///< Handle to the registered property storage
	config_base* m_pCfgBase;		///< Underlying base for this class
	tchar_t* m_pszCurrentPath;		///< Current path (one specified when reading the file)
	PFNPROPERTYCHANGED m_pfnNotifyCallback;	///< Function to be called when property changes
	ptr_t m_pCallbackParam;					///< User-defined parameter to pass to the callback function
};

END_ICPF_NAMESPACE

#endif
