// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LIBCHCORE_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LIBCHCORE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#if defined(_WIN32) || defined(_WIN64)
	#ifdef LIBCHCORE_EXPORTS
		#define LIBCHCORE_API __declspec(dllexport)
	#else
		#define LIBCHCORE_API __declspec(dllimport)
	#endif
#else
	/** \brief Import/export macros
	*
	*  These macros are being used throughout the whole code. They are meant to
	*  export symbols (if the LIBCHCORE_EXPORTS is defined) from this library
	*  (also for importing (when LIBCHCORE_EXPORTS macro is undefined) in other apps).
	*/
	#define LIBCHCORE_API
#endif

/// Begins ch namespace
#define BEGIN_CHCORE_NAMESPACE namespace chcore {
/// Ends ch namespace
#define END_CHCORE_NAMESPACE }
