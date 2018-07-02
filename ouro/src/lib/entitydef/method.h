// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OUROBOROS_DEF_METHOD_H
#define OUROBOROS_DEF_METHOD_H

#include "common/common.h"
#if OURO_PLATFORM == PLATFORM_WIN32
#pragma warning (disable : 4910)
#pragma warning (disable : 4251)
#endif

#include "datatype.h"
#include "datatypes.h"
#include "helper/debug_helper.h"
#include "network/packet.h"
#include "entitycallabstract.h"
#include "pyscript/scriptobject.h"


namespace Ouroboros{

class MethodDescription
{
public:
	MethodDescription(ENTITY_METHOD_UID utype, COMPONENT_ID domain,
		std::string name,
		bool isExposed = false);

	virtual ~MethodDescription();

	INLINE const char* getName(void) const;

	INLINE ENTITY_METHOD_UID getUType(void) const;
	INLINE void setUType(ENTITY_METHOD_UID muid);

	static uint32 getDescriptionCount(void){ return methodDescriptionCount_; }
	static void resetDescriptionCount(void){ methodDescriptionCount_ = 0; }

	INLINE bool isExposed(void) const;

	void setExposed(void);

	bool pushArgType(DataType* dataType);

	INLINE std::vector<DataType*>& getArgTypes(void);

	size_t getArgSize(void);

	/**
		Check if a call is valid
	*/
	bool checkArgs(PyObject* args);

	/**
		Add each parameter package to the stream,
		The information contained in this stream is the parameters passed in by this method when the script is invoked
	*/
	void addToStream(MemoryStream* mstream, PyObject* args);

	/**
		Unpacks a call and returns a args of type PyObject
	*/
	PyObject* createFromStream(MemoryStream* mstream);

	/**
		Call a method
	*/
	PyObject* call(PyObject* func, PyObject* args);

	INLINE COMPONENT_ID domain() const;

	INLINE bool isClient() const;
	INLINE bool isCell() const;
	INLINE bool isBase() const;

	/**
		Alias id, when the total number of exposed methods or broadcasted attributes is less than 255
		We do not use utype and use 1-byte aliasID to transmit
	*/
	INLINE int16 aliasID() const;
	INLINE uint8 aliasIDAsUint8() const;
	INLINE void aliasID(int16 v);

protected:
	static uint32							methodDescriptionCount_;					// The number of all attribute descriptions

	COMPONENT_ID							methodDomain_;

	std::string								name_;										// The name of this method
	ENTITY_METHOD_UID						utype_;										// The digital category of this method for transmission identification on the network

	std::vector<DataType*>					argTypes_;									// List of parameter categories for this attribute

	bool									isExposed_;									// Is it an exposure method

	int16									aliasID_;									// Alias id, when the total number of exposed methods or broadcasted attributes is less than 255, we do not use utype and use 1 byte aliasID to transmit
};

}

#ifdef CODE_INLINE
#include "method.inl"
#endif
#endif // OUROBOROS_DEF_METHOD_H
