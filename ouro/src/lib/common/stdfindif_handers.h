// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_STDFINDIF_HANDERS_H
#define OURO_STDFINDIF_HANDERS_H

#include "common/platform.h"

namespace Ouroboros{

// Vector <string> It's easy to use std::find_if to find out if there is a string
template<typename T>
class find_vec_string_exist_handle
{
public:
	find_vec_string_exist_handle(const std::basic_string< T >& str)
	: str_(str) {}

	bool operator()(const std::basic_string< T > &strSrc)
	{
		return strSrc == str_;
	}

	bool operator()(const T* strSrc)
	{
		return strSrc == str_;
	}

private:
	std::basic_string< T > str_;
};


// Vector <obj*> It's easy to use std::find_if to find out if there is an object
template<typename T>
class findif_vector_obj_exist_handler
{
public:
	findif_vector_obj_exist_handler(T obj)
	: obj_(obj) {}

	bool operator()(const T &obj)
	{
		return obj == obj_;
	}

	bool operator()(const T* obj)
	{
		return obj == obj_;
	}

private:
	T obj_;
};

}

#endif // OURO_STDFINDIF_HANDERS_H
