// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OUROBOROS_SCRIPT_H
#define OUROBOROS_SCRIPT_H

#include "helper/debug_helper.h"
#include "common/common.h"
#include "common/singleton.h"
#include "scriptobject.h"
#include "scriptstdouterr.h"
#include "scriptstdouterrhook.h"

namespace Ouroboros{ namespace script{

/** Script system path*/
#ifdef _LP64
#define SCRIPT_PATH													\
					L"../../res/scripts;"							\
					L"../../res/scripts/common;"					\
					L"../../res/scripts/common/lib-dynload;"		\
					L"../../res/scripts/common/DLLs;"				\
					L"../../res/scripts/common/Lib;"				\
					L"../../res/scripts/common/Lib/site-packages;"	\
					L"../../res/scripts/common/Lib/dist-packages"

#else
#define SCRIPT_PATH													\
					L"../../res/scripts;"							\
					L"../../res/scripts/common;"					\
					L"../../res/scripts/common/lib-dynload;"		\
					L"../../res/scripts/common/DLLs;"				\
					L"../../res/scripts/common/Lib;"				\
					L"../../res/scripts/common/Lib/site-packages;"	\
					L"../../res/scripts/common/Lib/dist-packages"

#endif

#define APPEND_PYSYSPATH(PY_PATHS)									\
	std::wstring pySysPaths = SCRIPT_PATH;							\
	wchar_t* pwpySysResPath = strutil::char2wchar(const_cast<char*>(Resmgr::getSingleton().getPySysResPath().c_str()));	\
	strutil::ouro_replace(pySysPaths, L"../../res/", pwpySysResPath);\
	PY_PATHS += pySysPaths;											\
	free(pwpySysResPath);


PyObject * PyTuple_FromStringVector(const std::vector< std::string > & v);

template<class T>
PyObject * PyTuple_FromIntVector(const std::vector< T > & v)
{
	int sz = v.size();
	PyObject * t = PyTuple_New( sz );
	for (int i = 0; i < sz; ++i)
	{
		PyTuple_SetItem( t, i, PyLong_FromLong( v[i] ) );
	}

	return t;
}

template<>
inline PyObject * PyTuple_FromIntVector<int64>(const std::vector< int64 > & v)
{
	int sz = (int)v.size();
	PyObject * t = PyTuple_New( sz );
	for (int i = 0; i < sz; ++i)
	{
		PyTuple_SetItem( t, i, PyLong_FromLongLong( v[i] ) );
	}

	return t;
}

template<>
inline PyObject * PyTuple_FromIntVector<uint64>(const std::vector< uint64 > & v)
{
	int sz = (int)v.size();
	PyObject * t = PyTuple_New( sz );
	for (int i = 0; i < sz; ++i)
	{
		PyTuple_SetItem( t, i, PyLong_FromUnsignedLongLong( v[i] ) );
	}

	return t;
}

class Script: public Singleton<Script>
{						
public:	
	Script();
	virtual ~Script();
	
	/** 
		Install and uninstall script modules
	*/
	virtual bool install(const wchar_t* pythonHomeDir, std::wstring pyPaths, 
		const char* moduleName, COMPONENT_TYPE componentType);

	virtual bool uninstall(void);
	
	bool installExtraModule(const char* moduleName);

	/** 
		Add an extension interface to the engine extension module
	*/
	bool registerExtraMethod(const char* attrName, PyMethodDef* pyFunc);

	/** 
		Add an extended attribute to the engine extension module
	*/
	bool registerExtraObject(const char* attrName, PyObject* pyObj);

	/** 
		Get the script base module
	*/
	INLINE PyObject* getModule(void) const;

	/** 
		Get script extension module
	*/
	INLINE PyObject* getExtraModule(void) const;

	/**
		Get the module when the script is initialized
	*/
	INLINE PyObject* getSysInitModules(void) const;

	int run_simpleString(const char* command, std::string* retBufferPtr);
	INLINE int run_simpleString(std::string command, std::string* retBufferPtr);

	int registerToModule(const char* attrName, PyObject* pyObj);
	int unregisterToModule(const char* attrName);

	INLINE ScriptStdOutErr* pyStdouterr() const;

	INLINE void pyPrint(const std::string& str);

	void setenv(const std::string& name, const std::string& value);

protected:
	PyObject* 					module_;
	PyObject* extraModule_; // extended script module
	PyObject* sysInitModules_; // Initially sys loaded module

	ScriptStdOutErr*			pyStdouterr_;
} ;

}
}

#ifdef CODE_INLINE
#include "script.inl"
#endif

#endif // OUROBOROS_SCRIPT_H
