// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_SCRIPT_PY_PROFILE_H
#define OURO_SCRIPT_PY_PROFILE_H

#include "common/common.h"
#include "common/smartpointer.h"
#include "scriptobject.h"

namespace Ouroboros{ 
class MemoryStream;	
typedef SmartPointer<PyObject> PyObjectPtr;

namespace script{

class Script;

class PyProfile
{						
public:	
	/** 
		Activate and stop a profile
	*/
	static bool start(std::string profile);
	static bool stop(std::string profile);
	static bool dump(std::string profile, std::string fileName);
	static void addToStream(std::string profile, MemoryStream* s);
	static bool remove(std::string profile);

	static void print_stats(const std::string& sort = "time", const std::string& profileName = "ouroboros");

	/** 
		Initialize pickler
	*/
	static bool initialize(Script* pScript);
	static void finalise(void);
	
private:
	typedef OUROUnordered_map< std::string, PyObjectPtr > PROFILES;
	static PROFILES profiles_;

	static PyObject* profileMethod_;

	static bool isInit; // whether it has been initialized

	static Script* pScript_;
} ;

}
}
#endif // OURO_SCRIPT_PY_PROFILE_H
