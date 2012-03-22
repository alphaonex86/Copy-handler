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
///** \file module.cpp
// *  \brief File contain an implementation of the module (and related) classes.
// */
//
#include "module.h"
//#include <assert.h>
//#include "err_codes.h"
//
//#ifndef _WIN32
//	#include <dlfcn.h>
//#endif
//
BEGIN_ICPF_NAMESPACE
//
//#define m_pmMods ((std::map<moduleid_t, module_param*>*)m_pMods)
//
///** Constructs a module_param class and initializes all the internal members
// *  to their initial values.
// */
//module_param::module_param()
//{
//	m_midModuleID=NULL_MODULE;
//	m_uiPropStart=0;
//}
//
///** Destructs the module_param class.
// */
//module_param::~module_param()
//{
//}
//
///** Locks the class (multi-threaded access).
// */
//void module_param::lock()
//{
//	m_lock.lock();
//}
//
///** Unlocks the class (multi-threaded access).
// */
//void module_param::unlock()
//{
//	m_lock.unlock();
//}
//
///** Returns a module id associated with this class.
// * \return Module ID
// * \note The usage of mutex inside of this function is unnecessary, because
// *		 the module id is not supposed to change.
// */
//moduleid_t module_param::get_moduleid() const
//{
//	return m_midModuleID;
//}
//
///** Reads the properties from the configuration object (config class). Function
// *  implemented as virtual - it does not do anything in this (base) class. Should
// *  be implemented in the derived classes. Function should read the properties registered
// *  earlier with register_properties() from a given config object and initialize
// *  the internal structure data (m_pParams). The given structure should be allocated
// *  earlier (ie. in the constructor of the derived class).
// *  Calling this function should be the first operation done in the derived class' function.
// * \param[in] pcfg - pointer to the config object to read the properties from
// */
//void module_param::read_config(config* /*pcfg*/)
//{
//}
//
///** Function writes the internal configuration options to the given configuration
// *  object (config class). Declared virtual and the base implementation does not
// *  do anything. Should be implemented in the derived classes. The purpose of this
// *  function is to write data from the internal data structure (m_pParams) to the
// *  given config class. The structure should be allocated and initialized earlier.
// *  Calling this function should be the first operation done in the derived class' function.
// * \param[in] pcfg - pointer to the config object to write the properties to
// */
//void module_param::write_config(config* /*pcfg*/)
//{
//}
//
///** Function registers the properties used in the internal data structure (m_pParams)
// *  to the given configuration object (config class). Function declared as virtual and
// *  the base implementation does not do anything. In derived classes this function should
// *  register the properties with the config object and store the first returned property ID
// *  in the m_ulPropStart internal member.
// *  Calling this function should be the first operation done in the derived class' function.
// * \note When registering properties user should lock the config object to make sure
// *       the ID's will be the subsequent numbers and not pseudo-random ones.
// * \param[in] pcfg - pointer to the configuration object with which the props should be registered.
// */
//void module_param::register_properties(config* /*pcfg*/)
//{
//}
//
//// store/restore from/to a file (serializer)
///** Function stores the internal data structure settings to the external file (file class).
// *  Declared as virtual - base implementation does not do anything. In derived classes this
// *  function should store the members of the internal structure (m_pParams) in the given
// *  file object in some order (that must be used also in load() function).
// *  Calling this function should be the first operation done in the derived class' function.
// * \param[in] ser - serialization object to write the data to
// */
//void module_param::store(file& /*ser*/)
//{
//}
//
///** Function loads the internal data structure from a file (file class). Declared as
// *  virtual - base implementation does not do anything. In derived classes function
// *  should read the properties from a given file object (in the order used in store() function).
// *  Calling this function should be the first operation done in the derived class' function.
// * \param[in] ser - serialization object that contains the data to be read
// */
//void module_param::load(file& /*ser*/)
//{
//}
//
///** Standard constructor - does nothing currently.
// */
//modparam_list::modparam_list()
//{
//	m_pMods=new std::map<moduleid_t, module_param*>;
//}
//
///** Standard destructor - clears the internal list of module_params. Also, each entry
// *  is being deleted before clearing. If you want to preserve the elements from being deleted - 
// *  use the clear(false) method before destrying this object.
// */
//modparam_list::~modparam_list()
//{
//	clear(true);
//	delete m_pmMods;
//}
//
///** Inserts a module_param to this list.
// * \param[in] pEntry - address of a module_param class to be inserted. It should be allocated by
// * 					the 'new' operator if you would like to use bDelete parameter set in other
// * 					methods.
// */
//void modparam_list::insert(module_param* pEntry)
//{
//	assert(pEntry);
//	m_lock.lock();
//	m_pmMods->insert(std::pair<moduleid_t, module_param*>(pEntry->get_moduleid(), pEntry));
//	m_lock.unlock();
//}
//
///** Removes a module from the list. Also delete a module_param if specified.
// * \param[in] tEntry - module id associated with an entry to remove
// * \param[in] bDelete - specifies, if the delete operator should be called on an entry
// * 						before removing it from the list.
// * \return If the entry was successfully removed (true) or not (false).
// */
//bool modparam_list::remove(moduleid_t tEntry, bool bDelete)
//{
//	m_lock.lock();
//	std::map<moduleid_t, module_param*>::iterator it = m_pmMods->find(tEntry);
//	if (it != m_pmMods->end())
//	{
//		// delete if needed
//		if (bDelete)
//			delete it->second;
//		m_pmMods->erase(it);
//		m_lock.unlock();
//		return true;
//	}
//	else
//	{
//		m_lock.unlock();
//		return false;
//	}
//}
//
///** Removes all the items from this list.
// * \param[in] bDelete - if true, then all the items will be 'delete''d  before removing.
// */
//void modparam_list::clear(bool bDelete)
//{
//	m_lock.lock();
//	if (bDelete)
//	{
//		for (std::map<moduleid_t, module_param*>::iterator it=m_pmMods->begin();it != m_pmMods->end();it++)
//		{
//			delete it->second;
//		}
//	}
//	
//	m_pmMods->clear();
//	m_lock.unlock();
//}
//
///** Searches for a module_param associated with a given module id.
// * \param[in] mid - module id to search for
// * \return Pointer to a module_param class, or NULL if not found.
// */
//module_param* modparam_list::find(moduleid_t mid)
//{
//	m_lock.lock();
//	std::map<moduleid_t, module_param*>::iterator it = m_pmMods->find(mid);
//	if (it != m_pmMods->end())
//	{
//		m_lock.unlock();
//		return it->second;
//	}
//	else
//	{
//		m_lock.unlock();
//		return NULL;
//	}
//}
//
///** A group wrapper over the module_param::read_config(). Calls the method for each of the module_param's.
// * \param[in] pcfg - pointer to a configuration object to read the data from.
// */
//void modparam_list::read_config(config* pcfg)
//{
//	// read the config for all entries
//	m_lock.lock();
//	try
//	{
//		for (std::map<moduleid_t, module_param*>::iterator it=m_pmMods->begin();it != m_pmMods->end();it++)
//		{
//			it->second->read_config(pcfg);
//		}
//	}
//	catch(...)
//	{
//		m_lock.unlock();
//		throw;
//	}
//	m_lock.unlock();
//}
//
///** A group wrapper over the module_param::write_config(). Calls the method for each of the module_param's.
// * \param[in] pcfg - pointer to a configuration object to write the data to.
// */
//void modparam_list::write_config(config* pcfg)
//{
//	m_lock.lock();
//	try
//	{
//		for (std::map<moduleid_t, module_param*>::iterator it=m_pmMods->begin();it != m_pmMods->end();it++)
//		{
//			it->second->write_config(pcfg);
//		}
//	}
//	catch(...)
//	{
//		m_lock.unlock();
//		throw;
//	}
//	m_lock.unlock();
//}
//
///** A group wrapper over the module_param::register_properties(). Calls the method for each of the module_param's.
// * \param[in] pcfg - pointer to a configuration object to register the properties with.
// */
//void modparam_list::register_properties(config* pcfg)
//{
//	m_lock.lock();
//	try
//	{
//		for (std::map<moduleid_t, module_param*>::iterator it=m_pmMods->begin();it != m_pmMods->end();it++)
//		{
//			it->second->register_properties(pcfg);
//		}
//	}
//	catch(...)
//	{
//		m_lock.unlock();
//		throw;
//	}
//	m_lock.unlock();
//}
//
///** A group wrapper over the module_param::store(). Calls the method for each of the module_param's.
// * \param[in] ser - a serialization object to write the data to
// */
//void modparam_list::store(file& ser)
//{
//	m_lock.lock();
//	try
//	{
//		for (std::map<moduleid_t, module_param*>::iterator it=m_pmMods->begin();it != m_pmMods->end();it++)
//		{
//			it->second->store(ser);
//		}
//	}
//	catch(...)
//	{
//		m_lock.unlock();
//		throw;
//	}
//	m_lock.unlock();
//}
//
///** A group wrapper over the module_param::load(). Calls the method for each of the module_param's.
// * \param[in] ser - a serialization object to read the data from
// */
//void modparam_list::load(file& ser)
//{
//	m_lock.lock();
//	try
//	{
//		for (std::map<moduleid_t, module_param*>::iterator it=m_pmMods->begin();it != m_pmMods->end();it++)
//		{
//			it->second->load(ser);
//		}
//	}
//	catch(...)
//	{
//		m_lock.unlock();
//		throw;
//	}
//	m_lock.unlock();
//}
//
/////////////////////////////////////////////////////////////////////////////
///** Constructor - makes a copy of the MODULE_INITDATA passed as the parameter
// *  and stores the given flags in the internal member. Also nullifies all the other class
// *  members. The module information is initialized with NULL values and for internal
// *  modules this should be corrected in the constructor of the derived class.
// * \param[in] pData - pointer to the structure with some parameters (the copy of it
// *                    will be stored in the internal member - not the pointer itself).
// * \param[in] uiFlags - module flags that are about to be stored in the internal member (MF_*)
// */
//module::module(const MODULE_INITDATA* pData, uint_t uiFlags)
//{
//	m_pmp=NULL;
//	m_pmid=pData;
//	m_ulFlags=uiFlags;
//	m_lRefCount=0;
//	
//	// module information
//	m_mi.uiInfoLen=sizeof(MODULE_INFO);
//	m_mi.midID=NULL_MODULE;
//	m_mi.szAuthor[0]='\0';
//	m_mi.szName[0]='\0';
//	m_mi.szVersion[0]='\0';
//	m_mi.uiType=MT_NONE;
//	
//	m_hModule=NULL;
//	m_pszPath=NULL;
//	m_pfnGetInfo=NULL;
//	m_pfnInit=NULL;
//	m_pfnUninit=NULL;
//	m_pfnAllocModparam=NULL;
//}
//
///** Destructor. Tries to close the module (close(true) function). If the closing
// *  function throws an exception it is caught, logged to the log file (MODULE_INITDATA)
// *  and the exception is removed.
// */
//module::~module()
//{
//	try
//	{
//		close(true);
//	}
//	catch(exception* e)
//	{
//		LOG_EXCEPTION(e, m_pmid->plog);
//		e->del();
//	}
//}
//
//// external modules support (called only for the external modules)
///** Function opens the external file as the program module. After successful
// *  file opening this function loads all exports from the module (using load_exports()
// *  function) and caches the module information in the internal member if all goes ok.
// *  If something goes wrong the exception is thrown. All information (excluding exceptions
// *  are logged to the log file (MODULE_INITDATA)).
// * \param[in] pszPath - full path to the module that is about to be loaded
// */
//void module::open(const char_t* pszPath)
//{
//	assert(m_ulFlags & MF_EXTERNAL);	// only for the external modules
//	
//	m_pmid->plog->logi("[module] Loading external module " STRFMT, pszPath);
//	
//	// try to load external library
//#ifdef _WIN32
//	if ( (m_hModule=::LoadLibrary(pszPath)) == NULL)
//		THROW(exception::format("Cannot load external module " STRFMT, pszPath), PE_CANNOTLOAD, GetLastError(), 0);
//#else
//	if ( (m_hModule=dlopen(pszPath, RTLD_LAZY)) == NULL)
//		THROW(exception::format("Cannot load external module " STRFMT " (" STRFMT ")", pszPath, dlerror()), PE_CANNOTLOAD, 0, 0);
//#endif
//
//	m_pmid->plog->logi("[module] External module loaded successfully (handle " PTRFMT ")", m_hModule);
//	
//	// load all needed exports (function must throw if export does not exist)
//	m_pmid->plog->logd("[module] Loading exports for the module (handle " PTRFMT ")", m_hModule);
//	load_exports();
//	m_pmid->plog->logd("[module] Exports loaded for the module (handle " PTRFMT ")", m_hModule);
//	
//	// cache the module information
//	m_pmid->plog->logd("[module] Caching module (handle " PTRFMT ") information", m_hModule);
//	(*m_pfnGetInfo)(&m_mi);
//	m_pmid->plog->logd("[module] Cached module (handle " PTRFMT ") information - id: " MODIDFMT ", type: " ULFMT ", name: " STRFMT ", version: " STRFMT ", author: " STRFMT "", m_hModule, m_mi.midID, m_mi.uiType, m_mi.szName, m_mi.szVersion, m_mi.szAuthor);
//	
//	// store the path
//	m_pszPath=new char_t[strlen(pszPath)+1];
//	strcpy(m_pszPath, pszPath);
//}
//
//// close the module - it's safe to call it more than once
///** Closes the external module. At first it uninitializes the module that is about to
// *  be unloaded and then closes the module and resets all(except the module info) the
// *  internal data. Function is safe to be called more than once. If any problem occur
// *  there is the exception thrown.
// * \param[in] bFullDestruct - should be true only in destructor. Means deleting the path string
// *                            before uninitialization (and not after as with false).
// */
//void module::close(bool bFullDestruct)
//{
//	// if called from a destructor - release some of the memory allocated
//	if (bFullDestruct)
//	{
//		// delete the path allocated earlier
//		delete [] m_pszPath;
//		m_pszPath=NULL;
//	}
//
//	// uninit the module if wasn't already
//	uninit();
//	
//	// release stuff related to external module
//	if (m_hModule != NULL)
//	{
//		m_pmid->plog->logd("[module] Unloading an external module (handle " PTRFMT ")", m_hModule);
//		
//#ifdef _WIN32
//		if (!::FreeLibrary(m_hModule))
//			THROW(exception::format("Cannot unload the external module (handle " PTRFMT ")", m_hModule), PE_CANNOTUNLOAD, GetLastError(), 0);
//#else
//		if (dlclose(m_hModule) != 0)
//			THROW(exception::format("Cannot unload the external module - " STRFMT " (handle " PTRFMT ")", dlerror(), m_hModule), PE_CANNOTUNLOAD, 0, 0); 
//#endif
//		m_pmid->plog->logd("[module] ...external module unloaded (handle " PTRFMT ")", m_hModule);
//	}
//	
//	m_hModule=NULL;
//	m_pfnGetInfo=NULL;
//	m_pfnInit=NULL;
//	m_pfnUninit=NULL;
//	
//	// release a memory when sure the module has been succesfully freed
//	if (!bFullDestruct)
//	{
//		delete [] m_pszPath;
//		m_pszPath=NULL;
//	}
//}
//
///** Retrieves the module information (author, ... as in MODULE_INFO struct). This function
// *  does not use the internal cache for external modules - there is always a call made to the
// *  module. Internal modules always use caching (it's their only info source).
// * \param[out] pInfo - receives the module information
// */
//void module::get_info(MODULE_INFO* pInfo)
//{
//	if (m_ulFlags & MF_EXTERNAL)
//	{
//		assert(m_hModule);
//		
//		(*m_pfnGetInfo)(pInfo);
//	}
//	else
//		*pInfo=m_mi;
//}
//
///** Function initializes the module. For external modules the module's init() function will be
// *  called. For internal modules this should be the first function called from within the init()
// *  function of the derived class. If the function fails it can return false or throw an exception.
// *  Function is safe to be called multiple times - the real init() functions will be called
// *  only once.
// * \note In the internal modules the init() function in the derived classes should check for
// *       the MF_INITIALIZED flag and do not perform any initialization if flag is set to 0.
// * \param[in] pData - module initialization data - should be the same as in constructor
// * \return True if the function succeeds, false otherwise.
// */
//bool module::init(const MODULE_INITDATA* pData)
//{
//	// return if already initialized
//	if (m_ulFlags & MF_INITIALIZED)
//		return true;
//	
//	if (m_ulFlags & MF_EXTERNAL)
//	{
//		m_pmid->plog->logi("[module] Making external module initialization (id=" MODIDXFMT ")...", get_id());
//		if ((*m_pfnInit)(pData, &m_pmp))
//		{
//			m_pmid->plog->logi("[module] ...external module initialized successfully (id=" MODIDXFMT ").", get_id());
//			m_ulFlags |= MF_INITIALIZED;
//			return true;
//		}
//		else
//		{
//			m_pmid->plog->logi("[module] ...external module initialization failed (id=" MODIDXFMT ").", get_id());
//			return false;
//		}
//	}
//	else
//	{
//		m_ulFlags |= MF_INITIALIZED;
//		return true;
//	}
//}
//
//// uninitializes a module
//// safe to call multiple times (ext module uninit() func will be called only once)
///** Uninitializes a module. This is the first function to be called in uninit() function
// *  of the derived classes (internal modules). For external modules this function calls the
// *  module's init() function. This function is safe to be called multiple times - the real
// *  initialization functions will be called only once. On error either false value can be returned
// *  or exception will be thrown.
// * \note For internal modules - this function at first should check if the module has been
// *       initialized (by checking the MF_INITIALIZED flag - it must be set). If it is not then
// *       no uninitialization should be done.
// * \return True if all went ok, false otherwise.
// */
//bool module::uninit()
//{
//	if (m_ulFlags & MF_INITIALIZED)
//	{
//		if (m_ulFlags & MF_EXTERNAL)
//		{
//			m_pmid->plog->logi("[module] Making external module uninitialization (id=" MODIDXFMT ")...", get_id());
//			if ((*m_pfnUninit)(&m_pmp))
//			{
//				m_pmid->plog->logi("[module] ...external module uninitialization succeeded (id=" MODIDXFMT ").", get_id());
//				
//				// delete the module parameters/informations if allocated
//				cleanup();
//
//				return true;
//			}
//			else
//			{
//				m_pmid->plog->logi("[module] ...external module uninitialization failed (id=" MODIDXFMT ").", get_id());
//				return false;
//			}
//		}
//		else
//		{
//			// delete the module parameters/informations if allocated
//			cleanup();
//			
//			return true;
//		}
//	}
//	else
//		return true;	// already uninitialized
//}
//
///** Allocates a module_param for this module. External modules should allocate the needed class
// *  and return it in the alloc_modparam(). Internal modules do not need to call this function.
// *  And overloaded function should just alloc the class and return it.
// * \return Allocated class. Note that the returned pointer is being cast to module_param, but most
// *         likely this will be another class that has module_param as a base class.
// */
//module_param* module::alloc_modparam()
//{
//	assert(m_ulFlags & MF_INITIALIZED);
//	if (m_ulFlags & MF_EXTERNAL)
//		return (*m_pfnAllocModparam)();
//	else
//		return NULL;
//}
//
//// called to load all exported functions (must be called for any derived load_exports())
///** Loads the exports associated with a given type of module. This should be the first function
// *  to be called in load_exports() of derived classes. If a specified exports does not
// *  exist in a module an exception is thrown.
// * \note Use the MAP_EXPORT macro here to assign together the function name to the 
// *       function address.
// */
//void module::load_exports()
//{
//	MAP_EXPORT(m_hModule, m_pfnGetInfo, "get_info");
//	MAP_EXPORT(m_hModule, m_pfnInit, "init");
//	MAP_EXPORT(m_hModule, m_pfnUninit, "uninit");
//	MAP_EXPORT(m_hModule, m_pfnAllocModparam, "alloc_modparam");
//}
//
///** Cleanup function used in uninit() to make sure the module_param stuff
// *  is freed if needed. Also resets the MF_INITIALIZED flag.
// */
//void module::cleanup()
//{
//	// delete the module parameters/informations if allocated
//	if (m_pmp)
//	{
//		m_pmp->write_config(m_pmid->pcfg);
//		delete m_pmp;
//		m_pmp=NULL;
//	}
//
//	m_ulFlags &= ~MF_INITIALIZED;
//}
//
///////////////////////////////////////////////////////////////////
//#define m_pvModules ((std::vector<module*>*)m_vModules)
//#define m_pmModules ((std::map<moduleid_t, module*>*)m_mModules)
//
///** Constructor - makes a copy of the MODULE_INITDATA structure and
// *  stores it in the internal member.
// */
//module_list::module_list(const MODULE_INITDATA* pData)
//{
//	m_pmid=pData;
//	m_vModules=(void*)new std::vector<module*>;
//	m_mModules=(void*)new std::map<moduleid_t, module*>;
//}
//
///** Destructor - calls the remove_all(true) to get rid of all modules before
// *  class is destroyed. Any exception thrown in the remove_all() function is being
// *  logged to a log file (MODULE_INITDATA) and the exception is deleted.
// */
//module_list::~module_list()
//{
//	try
//	{
//		remove_all(true);
//	}
//	catch(exception& e)
//	{
//		LOG_EXCEPTION(e, m_pmid->plog);
//		e->del();
//	}
//
//	delete m_pvModules;
//	delete m_pmModules;
//}
//
//#ifndef _WIN32
///** Helper function for filtering filesystem entries. It allows selecting only the
// *  filesystem entries that appears to be the proper modules. Function used in linux.
// * \param[in] pent - directory entry to process
// * \return >0 for the entry to be accepted, 0 if not.
// */
//int_t module_list::mod_filter(const struct dirent *pent)
//{
//	size_t tLen=strlen(pent->d_name), tExtLen=strlen(MODULE_EXT);
//	if (tLen >= tExtLen && strcmp(pent->d_name+tLen-tExtLen, MODULE_EXT) == 0)
//		return 1;
//	else
//		return 0;
//}
//#endif
//
///** Function scans a specified directory for the files that looks like modules.
// *  Macro MODULE_EXT specifies the file extension (in format ".ext") used in modules.
// *  Only modules that types matches (even partially) the specified type are added to
// *  the list. All the performed operations are logged into the log file.
// *  There are no exception throws or return values.
// * \param[in] pszPath - path to the directory with modules (must be trailed with '\\' or '/' - system dependent)
// * \param[in] uiType - types of modules to be added to the list
// */
//void module_list::scan(const char_t* pszPath, uint_t uiType)
//{
//	m_pmid->plog->logi("[module_list] Scanning directory " STRFMT " for external modules of type " ULFMT, pszPath, uiType);
//	uint_t uiCount=0;		// count of modules found
//	
//#ifdef _WIN32
//	// create full search path
//	char_t sz[_MAX_PATH];
//	_snprintf(sz, _MAX_PATH, STRFMT "*" STRFMT, pszPath, MODULE_EXT);
//	size_t tLen=strlen(pszPath);
//
//	WIN32_FIND_DATA wfd;
//	HANDLE hFind=::FindFirstFile(sz, &wfd);
//	if (hFind != INVALID_HANDLE_VALUE)
//	{
//		BOOL bFound=TRUE;
//		while (bFound)
//		{
//			// append a name to the input path (NOTE: it's a small optimization so it looks like there's something's missing).
//			strcpy(sz+tLen, wfd.cFileName);
//#else
//	dirent **ppde;
//	char_t sz[PATH_MAX];
//	strcpy(sz, pszPath);
//	size_t tLen=strlen(pszPath);
//	
//	int_t iCnt=scandir(pszPath, &ppde, mod_filter, NULL);
//	while (iCnt--)
//	{
//		strcpy(sz+tLen, ppde[iCnt]->d_name);
//#endif
//			module* pmod=new module(m_pmid, true);
//			try
//			{
//				pmod->open(sz);
//				
//				if (pmod->get_type() & uiType)
//				{
//					push_back(pmod);
//					uiCount++;
//				}
//				else
//					delete pmod;	// also calls module::close(), but does not throw an exception
//			}
//			catch(exception e)
//			{
//				m_pmid->plog->logw("[module_list] Caught an exception while trying to open a module (path=" STRFMT ").Ignoring module.", sz);
//				LOG_EXCEPTION(e, m_pmid->plog);
//				
//				e->del();
//				delete pmod;
//			}
//#ifdef _WIN32		
//			bFound=::FindNextFile(hFind, &wfd);
//		}
//
//		if (!::FindClose(hFind))
//			m_pmid->plog->logd("[module_list] Cannot close a find handle in module::open(), system error " ULFMT ". Ignoring.", GetLastError());
//	}
//	
//#else
//		free(ppde[iCnt]);
//	}
//	free(ppde);
//#endif
//	m_pmid->plog->logi("[module_list] Completed scanning for external modules in directory " STRFMT ". Found " ULFMT " modules with type matching " ULFMT, pszPath, uiCount, uiType);
//}
//
//// getting a module from a vector
///** Searches for a module in a list given it's ID. When using this function
// *  user should fully lock the module_list and before releasing the lock he should
// *  use module::acquire() to make sure the module won't be unloaded when being used.
// * \param[in] mid - module ID to find in the list
// * \return Pointer to the module or NULL if module not found.
// */
//module* module_list::find(moduleid_t mid)
//{
//	module* mod;
//	
//	m_lock.lock();
//	std::map<moduleid_t, module*>::iterator it=m_pmModules->find(mid);
//	if (it != m_pmModules->end())
//		mod=(*it).second;
//	else
//		mod=NULL;
//	m_lock.unlock();
//	
//	return mod;
//}
//
///** Function returns a module at a specified position. When using this function
// *  user should lock the entire module_list and before releasing lock he should
// *  use module::acquire() function to ensure the module to remain loaded.
// * \param[in] tPos - index of the module to return address of; the position must
// *                   be in range. The debug version of program asserts if range exceeded.
// * \return Address of a module.
// */
//module* module_list::at(size_t tPos)
//{
//	assert(tPos < m_pvModules->size());
//	
//	m_lock.lock();
//	module* mod=m_pvModules->at(tPos);
//	m_lock.unlock();
//	
//	return mod;
//}
//
//// adding a new items (modules)
///** Function inserts a module into the list at the specified position. Module is being 
// *  initialized before insertion (if not already initialized). An exception* is thrown
// *  if any error occurs.
// * \param[in] tPos - position in the list to insert the module at (-1 = at the end)
// * \param[in] tModule - address of a module to be inserted into the list
// */
//void module_list::insert(size_t tPos, module* tModule)
//{
//	m_pmid->plog->logd("[module_list] Initializing the module (id=" MODIDXFMT ")", tModule->get_id());
//	tModule->init(m_pmid);	// can throw an exception
//	
//	m_pmid->plog->logd("[module_list] Inserting the module (id=" MODIDXFMT ") to the module list at the position " ULPTRXFMT, tModule->get_id(), tPos);
//	
//	m_lock.lock();
//
//	try
//	{
//		std::map<moduleid_t, module*>::iterator it=m_pmModules->find(tModule->get_id());
//		if (it != m_pmModules->end())
//		{
//			THROW(exception::format("Module with a specified id=" MODIDXFMT " (name: " STRFMT ", version: " STRFMT ", author: " STRFMT ") already exists (name: " STRFMT ", version: " STRFMT ", author: " STRFMT ")",
//				tModule->get_id(), tModule->get_name(), tModule->get_version(), tModule->get_author(),
//				(*it).second->get_name(), (*it).second->get_version(), (*it).second->get_author()),
//				PE_DUPLICATEPLUG, 0, 0);
//		}
//		else
//		{
//			if (tPos != (size_t)-1)
//			{
//				assert(tPos <= m_pvModules->size());
//				m_pvModules->insert(m_pvModules->begin()+tPos, tModule);
//			}
//			else
//				m_pvModules->push_back(tModule);
//			
//			m_pmModules->insert(std::pair<moduleid_t, module*>(tModule->get_id(), tModule));
//		}
//		
//		m_lock.unlock();
//	}
//	catch(...)
//	{
//		m_lock.unlock();
//		throw;
//	}
//}
//
///** Adds a module at the beginning of the list. Function uses the insert() function.
// * \param[in] tModule - address of the module to add
// */
//void module_list::push_front(module* tModule)
//{
//	insert(0, tModule);
//}
//
///** Adds a module at the end of a list. Function uses the insert() function to perform
// *  the requested operation.
// * \param[in] tModule - address of a module to add
// */
//void module_list::push_back(module* tModule)
//{
//	insert((size_t)-1, tModule);
//}
//
//// repositioning modules in a vector
///** Changes the positions of the 2 modules (given by their ID's) - swaps them.
// * \param[in] t1, t2 - module ID's of the modules to be swapped
// */
//void module_list::swap(moduleid_t t1, moduleid_t t2)
//{
//	m_lock.lock();
//	std::vector<module*>::iterator it1, it2, it;
//	
//	// enum through the all modules
//	if (find_module(t1, t2, &it1, &it2))
//		swap(it1, it2);
//	else
//		m_pmid->plog->logd("[module_list] Swapping modules failed - one of the modules not found (id1=" MODIDXFMT ", id2=" MODIDXFMT ")", t1, t2);
//	
//	m_lock.unlock();
//}
//
///** Changes the positions of the 2 modules (given by their positions) - swaps them.
// * \param[in] tPos1, tPos2 - positions of the modules to be swapped
// */
//void module_list::swap(size_t tPos1, size_t tPos2)
//{
//	assert(tPos1 <= m_pvModules->size() && tPos2 <= m_pvModules->size());
//	
//	m_lock.lock();
//	swap(m_pvModules->begin()+tPos1, m_pvModules->begin()+tPos2);
//	m_lock.unlock();
//}
//
///** Moves the module (given by it's ID) to the position given as a second parameter.
// * \param[in] tID - ID of a module to move
// * \param[in] tNewPos - new position at which the module should appear
// */
//void module_list::move(moduleid_t tID, size_t tNewPos)
//{
//	assert(tNewPos < m_pvModules->size());
//	
//	m_lock.lock();
//	
//	std::vector<module*>::iterator it;
//	if (find_module(tID, &it))
//	{
//		module* mod=(*it);
//		m_pvModules->erase(it);
//		m_pvModules->insert(m_pvModules->begin()+tNewPos, mod);
//	}
//	
//	m_lock.unlock();
//}
//
///** Sorts the modules in order given by the vector passed as the parameter.
// * \param[in] vIDs - address of a vector that contains sorted (in the requested way)
// *                   module ID's.
// */
//void module_list::sort(std::vector<moduleid_t>* vIDs)
//{
//	m_lock.lock();
//	
//	// clear the vector
//	m_pvModules->clear();
//	
//	// and now process the data from map
//	module* mod;
//	for (std::vector<moduleid_t>::iterator it=vIDs->begin();it != vIDs->end();it++)
//	{
//		if ( (mod=find(*it)) != NULL_MODULE )
//			m_pvModules->push_back(mod);
//	}
//	
//	m_lock.unlock();
//}
//
///** Function fills the vector given as the parameter with the modules ID's
// *  with the order as in the current list.
// * \param[in] vIDs - address of a vector that will receive the module ID's.
// */
//void module_list::get_positions(std::vector<moduleid_t>* vIDs)
//{
//	m_lock.lock();
//	
//	for (std::vector<module*>::iterator it=m_pvModules->begin();it != m_pvModules->end();it++)
//	{
//		vIDs->push_back((*it)->get_id());
//	}
//	
//	m_lock.unlock();
//}
//
///** Returns the current count of modules contained in the list.
// * \return Count of modules.
// */
//size_t module_list::size()
//{
//	m_lock.lock();
//	size_t tLen=m_pvModules->size();
//	m_lock.unlock();
//	
//	return tLen;
//}
//
//// removing
///** Removes a module from a list (given it's ID). It means uninitializing the module
// *  and then closing it. If the module is being currently used (see module::acquire() and module::release())
// *  then it is not removed (except when the bForce flag is specified). In case of error
// *  the exception is thrown or false is returned.
// * \param[in] tID - id of a module to remove
// * \param[in] bForce - if true then the module reference count >0 does not block removing.
// * \return True if everything went ok, false otherwise.
// */
//bool module_list::remove(moduleid_t tID, bool bForce)
//{
//	// find the tID module iterator
//	bool bRes;
//	
//	m_lock.lock();
//	
//	try
//	{
//		std::vector<module*>::iterator it;
//		if (find_module(tID, &it))
//			bRes=remove(it, bForce);
//		else
//		{
//			m_pmid->plog->logd("[module_list] Cannot remove module (id=" MODIDXFMT ") - it does not exist", tID);
//			bRes=false;
//		}
//		
//		m_lock.unlock();
//	}
//	catch(...)
//	{
//		m_lock.unlock();
//		throw;
//	}
//	
//	return bRes;
//}
//
///** Removes a module from a list (given it's position). It means uninitializing the module
// *  and then closing it. If the module is being currently used (see module::acquire() and module::release())
// *  then it is not removed (except when the bForce flag is specified). In case of error
// *  the exception is thrown or false is returned.
// * \param[in] tPos - position of a module to remove
// * \param[in] bForce - if true then the module reference count >0 does not block removing.
// * \return True if everything went ok, false otherwise.
// */
//bool module_list::remove(size_t tPos, bool bForce)
//{
//	assert(tPos <= m_pvModules->size());
//	
//	m_lock.lock();
//	
//	bool bRes;
//	try
//	{
//		bRes=remove(m_pvModules->begin()+tPos, bForce);
//		m_lock.unlock();
//	}
//	catch(...)
//	{
//		m_lock.unlock();
//		throw;
//	}
//	
//	return bRes;
//}
//
///** Removes all the modules from a list. Depending the bForce parameter either all
// *  modules are removed or only the unused ones. When error is encountered while removing
// *  individual modules then it is logged to the log file and removed.
// * \param[in] bForce - specifies if the modules should be removed only if they are unused (false)
// *                     or always (true).
// */
//void module_list::remove_all(bool bForce)
//{
//	m_lock.lock();
//	std::vector<module*>::iterator it=m_pvModules->end();
//	while (it != m_pvModules->begin())
//	{
//		try
//		{
//			remove(--it, bForce);
//		}
//		catch(exception& e)
//		{
//			m_pmid->plog->logd("[module_list] Caught an exception in module_list::remove_all() while removing module from a list.Ignoring.");
//			LOG_EXCEPTION(e, m_pmid->plog);
//			e->del();
//		}
//	}
//	m_lock.unlock();
//}
//
////////////////////////////////////////
///** Removes a module from a list (given it's internal iterator). It means uninitializing the module
// *  and then closing it. If the module is being currently used (see module::acquire() and module::release())
// *  then it is not removed (except when the bForce flag is specified). In case of error
// *  the exception is thrown or false is returned.
// * \param[in] it - iterator that specifies position of a module in the internal vector
// * \param[in] bForce - if true then the module reference count >0 does not block removing
// * \return True if everything went ok, false otherwise.
// */
//bool module_list::remove(std::vector<module*>::iterator it, bool bForce)
//{
//	module* mod=(*it);
//	moduleid_t tid=mod->get_id();
//	
//	m_pmid->plog->logi("[module_list] Trying to remove module (id=" MODIDXFMT ")", tid);
//	
//	if (mod->get_refcount() != 0)
//	{
//		if (!bForce)
//		{
//			m_pmid->plog->logw("[module_list] Cannot remove module (id=" MODIDXFMT ") due to module's reference count=" LFMT, tid, mod->get_refcount());
//			return false;		// cannot unload
//		}
//		else
//			m_pmid->plog->logw("[module_list] Removing module (id=" MODIDXFMT ") with reference count=" LFMT, tid, mod->get_refcount());
//	}
//	
//	// uninit&close the module - both can throw an exception
//	try
//	{
//		if (!mod->uninit())
//		{
//			// cannot uninit module
//			if (!bForce)
//			{
//				m_pmid->plog->logw("[module_list] Cannot remove module (id=" MODIDXFMT ") due to uninit problems", tid);
//				return false;
//			}
//			else
//				m_pmid->plog->logw("[module_list] Removing module (id=" MODIDXFMT ") knowing that module uninit proc failed", tid);
//		}
//	}
//	catch(exception& e)
//	{
//		if (!bForce)
//			throw;		// rethrow the exception - will be reported by some other func
//		else
//		{
//			m_pmid->plog->logw("[module_list] Removing module (id=" MODIDXFMT ") knowing that module uninit proc had thrown an exception", tid);
//			LOG_EXCEPTION(e, m_pmid->plog);
//			e->del();
//		}
//	}
//
//	// try to close module
//	try
//	{
//		mod->close();
//	}
//	catch(exception& e)
//	{
//		if (!bForce)
//			throw;
//		else
//		{
//			m_pmid->plog->logw("[module_list] Removing module (id=" MODIDXFMT ") knowing that module close proc had thrown an exception", tid);
//			LOG_EXCEPTION(e, m_pmid->plog);
//			e->del();
//		}
//	}
//	
//	// remove the module from the list
//	m_pvModules->erase(it);
//	std::map<moduleid_t, module*>::iterator mit=m_pmModules->find(tid);
//	if (mit != m_pmModules->end())
//		m_pmModules->erase(mit);
//	
//	m_pmid->plog->logi("[module_list] Module (id=" MODIDXFMT ") removed successfully", tid);
//	return true;
//}
//
///** Changes the placement of the two modules (given by their iterators in the internal vector).
// * \param[in] it1, it2 - positions of the modules in the internal vector
// */
//void module_list::swap(std::vector<module*>::iterator it1, std::vector<module*>::iterator it2)
//{
//	module* mod=(*it1);
//	(*it1)=(*it2);
//	(*it2)=mod;
//}
//
///** Searches for a specified module (by it's ID) and stores the iterator in the iterator
// *  passed as the parameter.
// * \param[in] tID - ID of the module to search for
// * \param[out] pit - address of an iterator that is about to receive the module position
// * \return True if the module was found, false otherwise.
// */
//bool module_list::find_module(moduleid_t tID, std::vector<module*>::iterator* pit)
//{
//	// find the requested module
//	std::vector<module*>::iterator it;
//	(*pit)=m_pvModules->end();
//	
//	for (it=m_pvModules->begin();it != m_pvModules->end();it++)
//	{
//		// check if this is one of the requested modules
//		if ((*it)->get_id() == tID)
//		{
//			(*pit)=it;
//			break;
//		}
//	}
//	
//	return ((*pit) != m_pvModules->end());
//}
//
///** Searches for a specified modules (by their ID's) and stores the iterators in the iterators
// *  passed as the parameters.
// * \param[in] tID1, tID2 - ID's of the modules to search for
// * \param[out] pit1, pit2 - address of an iterators that are about to receive the module positions
// * \return True if the module was found, false otherwise.
// */
//bool module_list::find_module(moduleid_t tID1, moduleid_t tID2, std::vector<module*>::iterator* pit1, std::vector<module*>::iterator* pit2)
//{
//	// find the requested module
//	std::vector<module*>::iterator it;
//	(*pit1)=(*pit2)=m_pvModules->end();
//	
//	for (it=m_pvModules->begin();it != m_pvModules->end();it++)
//	{
//		// check if this is one of the requested modules
//		if ((*it)->get_id() == tID1)
//			(*pit1)=it;
//		else if ((*it)->get_id() == tID2)
//			(*pit2)=it;
//	}
//	
//	return ((*pit1) != m_pvModules->end() && (*pit2) != m_pvModules->end());
//}

END_ICPF_NAMESPACE
