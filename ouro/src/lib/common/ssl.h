// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_SSL_H
#define OURO_SSL_H

#include "common/common.h"

namespace Ouroboros
{

class MemoryStream;

class KB_SSL
{
public:
	static bool initialize();
	static void finalise();

	/**
		Whether it is Https/Wss protocol
		return to the specific protocol version
	*/
	static int isSSLProtocal(MemoryStream* s);

};

}

#endif // OURO_SSL_H
