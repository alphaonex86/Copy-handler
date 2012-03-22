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
/** \file module.h
 *  \brief File contain declaration of the module (and related) classes.
 */
 
#ifndef __MODULE_H__
#define __MODULE_H__

#include "libicpf.h"
#include "gen_types.h"
/*#include "mutex.h"
#include "cfg.h"
#include "log.h"
#include "file.h"
#include <map>
#include <vector>

// inclusion of dirent.h
#if HAVE_DIRENT_H
	#include <dirent.h>
	#define NAMLEN(dirent) strlen((dirent)->d_name)
#else
	#define dirent direct
	#define NAMLEN(dirent) (dirent)->d_namlen
	#if HAVE_SYS_NDIR_H
		#include <sys/ndir.h>
	#endif
	#if HAVE_SYS_DIR_H
		#include <sys/dir.h>
	#endif
	#if HAVE_NDIR_H
		#include <ndir.h>
	#endif
#endif

#ifndef MODULE_EXT
    /// File extension to use with modules
    #define MODULE_EXT ""
#endif
*/
BEGIN_ICPF_NAMESPACE
/*
/// NULL module ID
#define NULL_MODULE		0x00000000

// module types
/// Module type NONE
#define MT_NONE			0x00000000
/// Module type ALL
#define MT_ALL			0xffffffff

// module flags
/// Module flag NONE
#define MF_NONE			0x00000000
/// The module is an external one
#define MF_EXTERNAL		0x00000001

/** \brief States that the init() func for the module has been successfully called.
 * 
 * If the flag is set it means that the module function init() has been called succesfully
 * and the uninit() call may be executed. Flag used to make sure there will be only one
 * subsequent call to init() and uninit() functions in the external module even if they
 * will be called more than once in a program.
 */
/*#define MF_INITIALIZED	0x00000002

/// Type describes the module id.
typedef ulonglong_t moduleid_t;
/// Module ID formatting text to be used in formatting routines
#define MODIDFMT	ULLFMT
/// Module ID (hex) formatting text to be used in formatting routines
#define MODIDXFMT	ULLXFMT

/** Makes a module ID from a given parameters.
 * \param[in] internal - bool that specifies if this is the internal module
 * \param[in] type - module type(could be a bitmask) - only the lower 28 bits will be used
 * \param[in] unique_id - 32-bit unique id (should be randomly generated)
 */
/*#define MAKE_MODID(internal,type,unique_id)\
 	((internal ? 0ULL : 0x8000000000000000ULL) | (((ulonglong_t)type & 0x0fffffff) << 32) | unique_id)

/** \brief Class for managing the parameters of a module.
 * 
 * Class provides support for a module parameters. It could be used outside the module (ie. after
 * module destruction) to store module settings to a file or to configuration.
 * \todo This class needs some more clarification - how to use it, the purpose, ...
 */
/*class LIBICPF_API module_param
{
public:
/** \name Construction/destruction */
/**@{*/
/*	module_param();				///< Standard constructor
	virtual ~module_param();	///< Standard destructor
/**@}*/
	
/** \name Locking/unlocking */
/**@{*/
/*	void lock();		///< Locks the class (gets exclusive ownership of this object)
	void unlock();		///< Unlocks the class (releases the ownership)
/**@}*/

/** \name Informations */
/**@*/
/*	moduleid_t get_moduleid() const;			///< Returns a module id associated with this class
/**@}*/

/** \name Configuration support */
/**@{*/ 
/*	virtual void read_config(config* pcfg);		///< Reads the configuration properties from a config object
	virtual void write_config(config* pcfg);	///< Writes the internal properties to the config object
	virtual void register_properties(config* pcfg);	///< Registers properties for use with the config object
/**@}*/
	
/** \name Serialization support */
/**@{*/
/*	virtual void store(file& ser);		///< Stores the internal properties in the serialization object (file)
	virtual void load(file& ser);		///< Loads the internal properties from a serialization object (file)
/**@}*/
/*	
protected:
	moduleid_t m_midModuleID;		///< ID of a module that owns this parameters
	uint_t m_uiPropStart;			///< ID of the first registered property (see register_properties())
	mutex m_lock;					///< Access lock for performing safe multi-threaded operations
};

/** \brief Class handling lists of module_param's.
 * 
 * Class handles the management of lists of module_param classes. One module can have only
 * one module_param (or derived) class associated with it.
 */
/*class LIBICPF_API modparam_list
{
public:
/** \name Construction/destruction */
/**@{*/
/*	modparam_list();						///< Standard constructor
	~modparam_list();						///< Standard destructor
/**@}*/
	
/** \name Standard operations */
/**@{*/
/*	void insert(module_param* pEntry);						///< Inserts a new module_param to this container
	bool remove(moduleid_t tEntry, bool bDelete=true);		///< Removes a module_param associated with a given module id
	void clear(bool bDelete=true);							///< Removes all the entries from the list
	module_param* find(moduleid_t mid);						///< Searches for a module_param associated with a given module id
/**@}*/

/** \name Configuration support */
/**@{*/ 
/*	void read_config(config* pcfg);			///< Reads the configuration properties from a config object
	void write_config(config* pcfg);		///< Writes the internal properties to the config object
	void register_properties(config* pcfg);	///< Registers properties for use with the config object
/**@}*/

/** \name Serialization support */
/**@{*/
/*	void store(file& ser);		///< Stores the internal properties in the serialization object (file)
	void load(file& ser);		///< Loads the internal properties from a serialization object (file)
/**@}*/
/*	
protected:
	void* m_pMods;										///< Internal map of module parameters
//	std::map<moduleid_t, module_param*> m_mMods;		
	mutex m_lock;										///< A locking mutex
};

/** \brief Module information struct
 * 
 * Structure contains some fields used to identify a module (module name,
 * id, type, author information and so on).
 */
/*struct MODULE_INFO
{
	uint_t uiInfoLen;		///< Count of bytes contained in this struct (filled by plugin)
	char_t szAuthor[128];	///< Author's full name
	char_t szName[128];		///< Plugin name
	char_t szVersion[32];	///< Version string
	moduleid_t midID;		///< 64-bit module ID
	uint_t uiType;			///< Type of a module (app-dependent)
};

/** \brief Structure with initialization data for a module
 * 
 * Structure contains init parameters passed into the constructor and
 * the init() function of a module and module_list class.
 */
/*struct MODULE_INITDATA
{
	config *pcfg;		///< Global configuration object
	log_file *plog;		///< Log file object to perform logging to
};

// external functions typedefs
/// Prototype of the external module get_info() function
typedef void(*PFNMGETINFO)(MODULE_INFO*);
/// Prototype of the external module init() function
typedef bool(*PFNMINIT)(const MODULE_INITDATA*, module_param**);
/// Prototype of the external module uninit() function
typedef bool(*PFNMUNINIT)(module_param**);
/// Allocates a new module_param-derived parameter
typedef module_param*(*PFNALLOCMODPARAM)();

// module handle definition
#ifdef _WIN32
	/// System dependent module handle definition
	#define MHANDLE HMODULE
#else
	/// System dependent module handle definition
	#define MHANDLE ptr_t
#endif

// helper for loading exports
#ifdef _WIN32
	/** \brief Maps a module exported function to some class member
	 * 
	 * Macro used to simplify loading function exported from an external
	 * modules (see module::load_exports()).
	 * \param[in] module - external module handle
	 * \param[in] var - variable that will receive the function address
	 * \param[in] fn_name - string with the function name to get addres of
	 * \note Macro throws an exception if the export cannot be loaded.
	 */
/*	#define MAP_EXPORT(module,var,fn_name)\
		(FARPROC&)var=::GetProcAddress(module, fn_name);\
		if (var == NULL)\
			THROW(exception::format("Cannot load an export " STRFMT " from the external module (handle " PTRFMT ")", fn_name, module), PE_CALLNOTIMPLEMENTED, GetLastError(), 0);
#else
	/** \brief Maps a module exported function to some class member
     * 
     * Macro used to simplify loading function exported from an external
     * modules (see module::load_exports()).
     * \param[in] module - external module handle
     * \param[in] var - variable that will receive the function address
     * \param[in] fn_name - string with the function name to get addres of
     * \note Macro throws an exception if the export cannot be loaded.
	 */
/*	#define MAP_EXPORT(module,var,fn_name)\
		*((ptr_t*)&var)=dlsym(module, fn_name);\
		if (var == NULL)\
			THROW(exception::format("Cannot load an export " STRFMT " from the external module - " STRFMT " (handle " PTRFMT ")", fn_name, dlerror(), module), PE_CALLNOTIMPLEMENTED, 0, 0);
#endif

/** \brief Module handling class
 * 
 * This class allows handling of the internal and external modules. This is a base class
 * from which should be derived any other module classes that handle different types
 * of modules.
 */
/*class LIBICPF_API module
{
public:
/** \name Construction/destruction */
/**@{*/
/*	module(const MODULE_INITDATA* pData, uint_t uiFlags=MF_EXTERNAL);	///< Standard constructor
	virtual ~module();		///< Standard destructor
/**@}*/
	
/** \name External modules support */
/**@{*/
/*	void open(const char_t* pszPath);			///< Opens an external module (file)
	void close(bool bFullDestruct=false);	///< Closes an external module (uninitializes if needed)
/**@}*/
	
/** \name Module information */
/**@{*/
/*	/// Retrieves the module information directly from the module
	void get_info(MODULE_INFO* pInfo);
	/// Returns an address of the cached module information structure
	const MODULE_INFO* get_info() { return &m_mi; };
	/// Returns module ID from the cached information struct
	moduleid_t get_id() const { return m_mi.midID; };
	/// Returns author string address from the cached information struct
	const char_t* get_author() const { return m_mi.szAuthor; };
	/// Returns version string address from the cached information struct
	const char_t* get_version() const { return m_mi.szVersion; };
	/// Returns module name string address from the cached information struct
	const char_t* get_name() const { return m_mi.szName; };
	/// Returns module type from the cached information struct
	uint_t get_type() const { return m_mi.uiType; };
/**@}*/
	
/** \name Module parameters */
/**@{*/
/*	virtual module_param* alloc_modparam();					///< Allocates a module_param (or derived) class - should be overloaded.
/**@}*/

/** \name Initialization/uninitialization */
/**@{*/
/*	virtual bool init(const MODULE_INITDATA* pData);	///< Initializes the module (if not initialized yet)
	virtual bool uninit();								///< Uninitializes the module (if not uninitialized yet)
/**@}*/
	
/** \name Reference counting */
/**@{*/
/*	/// Retrieves the current reference count
	int_t get_refcount() const { return m_lRefCount; };
	/// Increases the reference count
	void acquire() { ++m_lRefCount; };
	/// Decreases the reference count
	int_t release() { return --m_lRefCount; };
/**@}*/
	
//protected:
/*	/// Loads all needed exports from an external module
	virtual void load_exports();
	
	/// Cleans up the internal stuff
	void cleanup();

protected:
	const MODULE_INITDATA* m_pmid;	///< Module data struct ptr passed in the constructor
	module_param* m_pmp;			///< Pointer to a module parameters class (managed by a module).

	MODULE_INFO m_mi;				///< Module information struct (cached for external modules, filled in constructor for internal)
	
	int_t m_lRefCount;				///< Current reference count
	
	// external stuff
	uint_t m_ulFlags;			///< Module flags (MF_*)
	char_t* m_pszPath;			///< Full file path for an external module
	MHANDLE m_hModule;			///< Handle to the loaded external module (NULL_MODULE if not loaded)
	PFNMGETINFO m_pfnGetInfo;	///< Pointer to the module's get_info() function
	PFNMINIT m_pfnInit;			///< Pointer to the module's init() function
	PFNMUNINIT m_pfnUninit;		///< Pointer to the module's uninit() function
	PFNALLOCMODPARAM m_pfnAllocModparam;	///< Pointer to the module's alloc_modparam() function
};

/** \brief Module management class
 * 
 * Class was designed to allow easier handling of a module lists. Provides
 * a basic operations on a list of modules (searching, inserting, removing, ...).
 * Should be a base for any module management class.
 */
/*class LIBICPF_API module_list
{
public:
/** \name Construction/destruction */
/**@{*/
/*	module_list(const MODULE_INITDATA* pData);		///< Standard constructor
	~module_list();									///< Standard destructor
/**@}*/

/** \name Adding/removing */
/**@{*/
/*	void scan(const char_t* pszPath, uint_t uiType=MT_ALL);	///< Scans a directory for some modules

	// adding a new items (modules)
	void insert(size_t tPos, module* tModule);		///< Inserts a module at a specified position
	void push_front(module* tModule);				///< Adds a module at the beginning of a list
	void push_back(module* tModule);				///< Adds a module at the end of a list

	// removing
	bool remove(moduleid_t tID, bool bForce=false);	///< Removes a module from the list by its ID
	bool remove(size_t tPos, bool bForce=false);	///< Removes a module from the list by its position
	void remove_all(bool bForce=false);				///< Removes all the modules from the list
/**@}*/

/** \name Searching */
/**@{*/
/*	module* find(moduleid_t mid);	///< Searches a list for the module with the specified ID
	module* at(size_t tPos);		///< Gets the module at a specified position on the list
/**@}*/
	
/** \name Module repositioning */
/**@{*/
/*	void swap(moduleid_t t1, moduleid_t t2);			///< Swaps two modules positions by their ID's
	void swap(size_t tPos1, size_t tPos2);				///< Swaps two modules positions by their positions
	void move(moduleid_t tID, size_t tNewPos);			///< Moves the specified module to a new position
	void sort(std::vector<moduleid_t>* vIDs);			///< Sorts the modules using a module id vector
	void get_positions(std::vector<moduleid_t>* vIDs);	///< Retrieves the current modules positions
/**@}*/
	
/** \name Other */
/**@{*/
/*	size_t size();		///< Retrieves a count of modules in a list
/**@}*/
/*
protected:
	void swap(std::vector<module*>::iterator it1, std::vector<module*>::iterator it2);	///< Swaps two modules positions by their vector iterators
	bool find_module(moduleid_t tID, std::vector<module*>::iterator* pit);	///< Searches for the module by it's ID and returns an iterator
	bool find_module(moduleid_t tID1, moduleid_t tID2, std::vector<module*>::iterator* pit1, std::vector<module*>::iterator* pit2);	///< Searches for two modules by their ID's - returns iterators
	bool remove(std::vector<module*>::iterator it, bool bForce=false);		///< Removes a module from this list given the vector iterator
	
#ifndef _WIN32
	static int_t mod_filter(const struct dirent *pent);		///< Helper function to filter directory entries under linux
#endif

protected:
//	std::vector<module*> m_vModules;			
//	std::map<moduleid_t, module*> m_mModules;	
	void* m_vModules;						///< Array of modules (used to make this class preserve the module positions) - internal.
	void* m_mModules;						///< Mapping module id->module pointer (internal)
	const MODULE_INITDATA* m_pmid;				///< Module initialization data (used for module::init() functions and/or constructors)

	mutex m_lock;								///< Thread-safe access guarantee
};
*/
END_ICPF_NAMESPACE

#endif
