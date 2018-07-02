// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#include "math.h"
namespace Ouroboros{ namespace script{ namespace math {

//-------------------------------------------------------------------------------------
bool installModule(const char* moduleName)
{
	// Initialize a math-related module
	PyObject *mathModule = PyImport_AddModule(moduleName);
	PyObject_SetAttrString(mathModule, "__doc__", PyUnicode_FromString("This module is created by Ouroboros!"));

	// Initialize ScriptVector2
	script::ScriptVector2::installScript(mathModule, "Vector2");
	// Initialize ScriptVector3
	script::ScriptVector3::installScript(mathModule, "Vector3");
	// Initialize ScriptVector4
	script::ScriptVector4::installScript(mathModule, "Vector4");
	return true;
}

//-------------------------------------------------------------------------------------
bool uninstallModule()
{
	script::ScriptVector2::uninstallScript();
	script::ScriptVector3::uninstallScript();
	script::ScriptVector4::uninstallScript();
	return true;
}

}
}
}
