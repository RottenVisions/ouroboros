// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


/*
	usage:
		class A:public Singleton<A>
		{
		};
		In the cpp file:
		template<> A* Singleton<A>::singleton_ = 0;
*/
#ifndef OURO_SINGLETON_H
#define OURO_SINGLETON_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common/platform.h"

namespace Ouroboros{

template <typename T>
class Singleton
{
protected:
	static T* singleton_;

public:
	Singleton(void)
	{
		assert(!singleton_);
#if defined(_MSC_VER) && _MSC_VER < 1200
		int offset = (int)(T*)1 - (int)(Singleton <T>*)(T*)1;
		singleton_ = (T*)((int)this + offset);
#else
		singleton_ = static_cast< T* >(this);
#endif
	}


	~Singleton(void){  assert(singleton_);  singleton_ = 0; }

	static T& getSingleton(void) { assert(singleton_);  return (*singleton_); }
	static T* getSingletonPtr(void){ return singleton_; }
};

#define OURO_SINGLETON_INIT( TYPE )							\
template <>	 TYPE * Singleton< TYPE >::singleton_ = 0;	\

}
#endif // OURO_SINGLETON_H
