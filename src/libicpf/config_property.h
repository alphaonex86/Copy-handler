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
#ifndef __CONFIGPROPERTY_H__
#define __CONFIGPROPERTY_H__

#include "gen_types.h"
#include "libicpf.h"

BEGIN_ICPF_NAMESPACE

/** \brief Basic property description class.
 */
class LIBICPF_API property
{
public:
	/// Masks identifiers for property type
	enum prop_mask
	{
		mask_type=0x0000ffff,				///< Property type mask
		mask_flags=0xffff0000				///< Property flags mask
	};

	/// Property type definitions
	enum prop_type
	{
		type_unknown=0x00000001,			/// Unknown type (partial synonym of PT_STRING)
		type_signed_num=0x00000002,			/// Signed 64-bit type property
		type_unsigned_num=0x00000003,		/// Unsigned 64-bit type property
		type_bool=0x00000004,				/// Bool type property
		type_string=0x00000005				/// String type property
	};

	/// Property flags definitions
	enum prop_flags
	{
		flag_none=0x00000000,				/// Standard property flag
		flag_path=0x00010000,				/// The string specifies a pathname flag
		flag_encrypt=0x00040000,			/// This flag indicates that the property has been encrypted with a password (only string values)
		flag_decoded=0x00080000,			/// The property is currently in decrypted state (but should be encrypted when saving)
		flag_array=0x00100000,				/// Array property type
		flag_modified=0x00200000			/// Modification flag
	};

	// actions used by set_xxx()
	enum actions
	{
		action_add,							///< Value should be added
		action_replace,						///< Value should replace all previous values
		action_setat						///< Value should replace only a specific value
	};
public:
/** \brief Construction/destruction/operators */
/**@{*/
	property();											///< Standard constructor
	property(const tchar_t* pszName, uint_t uiType);	///< Constructor with initializer
	property(const property& src);						///< Copy constructor
	~property();										///< Standard destructor

	property& operator=(const property& rSrc);			///< Assignment operator
/**@}*/

/** \brief Property settings/operations */
/**@{*/
	/// Resets the internal members
	void clear();

	/// Sets a property type
	void init(const tchar_t* pszName, uint_t uiType, bool bClear=true);
	/// Retrieves a property type (with flags)
	uint_t get_type() const { return m_uiPropType; };
	/// Checks if the property is array-based
	bool is_array() const { return (m_uiPropType & flag_array) != false; };

	/// Sets a property name
	void set_name(const tchar_t* pszName) { m_pszName=copy_string(pszName); };
	/// Gets a property name
	const tchar_t* get_name() const { return m_pszName; };

	/// Sets the modified flag
	void set_modified(bool bModified) { if (bModified) m_uiPropType |= flag_modified; else m_uiPropType &= ~flag_modified; };
	/// Gets the modified flag
	bool is_modified() const { return (m_uiPropType & flag_modified) != false; };
/**@}*/

/** \brief Property values */
/**@{*/
	/// Sets a value from string
	void set_value(const tchar_t* pszValue, actions a=action_replace, size_t tIndex=0);
	/// Gets the value as string
	const tchar_t* get_value(tchar_t* pszString, size_t stMaxSize, size_t stIndex=0);

	/// Sets the string value
	void set_string(const tchar_t* pszValue, actions a=action_replace, size_t tIndex=0);
	/// Gets the string value
	const tchar_t* get_string(size_t stIndex=0) const;

	/// Sets the signed number value
	void set_signed_num(ll_t llValue, actions a=action_replace, size_t tIndex=0);
	/// Sets the signed number range
	void set_signed_range(ll_t llMin, ll_t llMax);
	/// Gets the signed number value
	ll_t get_signed_num(size_t stIndex=0) const;

	/// Sets the unsigned number value
	void set_unsigned_num(ull_t ullValue, actions a=action_replace, size_t tIndex=0);
	/// Sets the unsigned number range
	void set_unsigned_range(ull_t ullMin, ull_t ullMax);
	/// Gets the unsigned number value
	ull_t get_unsigned_num(size_t stIndex=0) const;

	/// Sets the bool value
	void set_bool(bool bValue, actions a=action_replace, size_t tIndex=0);
	/// Gets the bool value
	bool get_bool(size_t stIndex=0) const;

	/// Gets the property count for an array property type
	size_t get_count() const;
	/// Removes a property at a given index
	void remove(size_t stIndex);
	/// Clears the array
	void clear_array();
/**@}*/

protected:
	void clear_value();									///< Clears the current value (frees any allocated memory)
	void check_range();									///< Performs a range check on the property value

	tchar_t* copy_string(const tchar_t* pszSrc);		///< Makes a copy of a given string
	bool bool_from_string(const tchar_t* pszSrc);		///< Retrieves a bool value from a string
	ll_t signed_from_string(const tchar_t* pszSrc);		///< Retrieves a signed number from a string
	ull_t unsigned_from_string(const tchar_t* pszSrc);	///< Retrieves an unsigned number from a string

	void copy_from(const property& rSrc, bool bClear);	///< Makes a copy of a given property

protected:
	// basic, common property description
	uint_t m_uiPropType;					///< Property type and flags
	tchar_t* m_pszName;						///< Name of the property

	// values
	union _VALUE				/// Union with different types of properties
	{
		ll_t llVal;				///< Signed number value
		ull_t ullVal;			///< Unsigned number value
		bool bVal;				///< A bool-type value
		tchar_t* pszVal;		///< A string-type value
		ptr_t hArray;			///< An array-type value
	} m_val;

	union _RANGE				/// Union with numeric properties ranges
	{
		struct LLRANGE
		{
			ll_t llLo;			///< Minimum allowed value for the longlong_t property
			ll_t llHi;			///< Maximum allowed value for the longlong_t property
		} ll;
		struct ULLRANGE
		{
			ull_t ullLo;		///< Minimum allowed value for the ull_t property
			ull_t ullHi;		///< Maximum allowed value for the ull_t property
		} ull;
	} m_range;
};

END_ICPF_NAMESPACE

#endif
