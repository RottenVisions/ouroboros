// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_SCRIPT_COMPRESSION_H
#define OURO_SCRIPT_COMPRESSION_H

#include "common/common.h"
#include "scriptobject.h"

namespace Ouroboros{ namespace script{

class PyCompression
{						
public:	
	static bool zipCompressDirectory(const std::string& sourceDir, const std::string& outfile);
	static bool tarCompressDirectory(const std::string& sourceDir, const std::string& outfile);

		/** Initialization*/
	static bool initialize(void);
	static void finalise(void);

private:
	static bool isInit; // whether it has been initialized
} ;

}
}
#endif // OURO_SCRIPT_COMPRESSION_H
