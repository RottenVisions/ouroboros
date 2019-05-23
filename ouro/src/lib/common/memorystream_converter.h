// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

/*
	Endian swap processing module:
		 Because network communication generally uses BIG byte order \ also known as network byte order.
 		 ?The PC or embedded system we use may use BIG byte order or LITTEN (small endian)
 		 ?So we have to do an endian conversion between them.
*/
#ifndef OURO_MEMORYSTREAMCONVERTER_H
#define OURO_MEMORYSTREAMCONVERTER_H

#include "common/common.h"
	
namespace Ouroboros{

namespace MemoryStreamConverter
{
    template<size_t T>
        inline void convert(char *val)
    {
        std::swap(*val, *(val + T - 1));
        convert<T - 2>(val + 1);
    }

    template<> inline void convert<0>(char *) {}
    template<> inline void convert<1>(char *) {}            // ignore central byte

    template<typename T> inline void apply(T *val)
    {
        convert<sizeof(T)>((char *)(val));
    }

	inline void convert(char *val, size_t size)
	{
		if(size < 2)
			return;

		std::swap(*val, *(val + size - 1));
		convert(val + 1, size - 2);
	}
}

#if OUROBOROS_ENDIAN == OUROBOROS_BIG_ENDIAN // can be tested with sys.isPlatformLittleEndian()
template<typename T> inline void EndianConvert(T& val) { MemoryStreamConverter::apply<T>(&val); }
template<typename T> inline void EndianConvertReverse(T&) { }
#else
template<typename T> inline void EndianConvert(T&) { }
template<typename T> inline void EndianConvertReverse(T& val) { MemoryStreamConverter::apply<T>(&val); }
#endif

template<typename T> void EndianConvert(T*);         // will generate link error
template<typename T> void EndianConvertReverse(T*);  // will generate link error

inline void EndianConvert(uint8&) { }
inline void EndianConvert(int8&) { }
inline void EndianConvertReverse(uint8&) { }
inline void EndianConvertReverse(int8&) { }

}
#endif
