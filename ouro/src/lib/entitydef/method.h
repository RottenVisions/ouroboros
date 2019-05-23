// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


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
	// type of exposure method
	enum EXPOSED_TYPE
	{
		// default, non-exposure method
		NO_EXPOSED = 0,

		// By default, script methods can be added without caller parameters
		EXPOSED = 1,

		// The first parameter of the script method is the caller ID, providing a script to check the caller's legitimacy
		EXPOSED_AND_CALLER_CHECK = 2
	};

public:	
	MethodDescription(ENTITY_METHOD_UID utype, COMPONENT_ID domain,
		std::string name,
		EXPOSED_TYPE exposedType = NO_EXPOSED);

	virtual ~MethodDescription();
	
	INLINE const char* getName(void) const;

	INLINE ENTITY_METHOD_UID getUType(void) const;
	INLINE void setUType(ENTITY_METHOD_UID muid);

	static uint32 getDescriptionCount(void){ return methodDescriptionCount_; }
	static void resetDescriptionCount(void){ methodDescriptionCount_ = 0; }

	INLINE EXPOSED_TYPE isExposed(void) const;

	void setExposed(EXPOSED_TYPE type = EXPOSED);

	bool pushArgType(DataType* dataType);

	INLINE std::vector<DataType*>& getArgTypes(void);

	size_t getArgSize(void);
	
	/** 
		Check if a call is legal
	*/
	bool checkArgs(PyObject* args);		
	
	/** 
		Pack each parameter to the stream,
		The information contained in this stream is the parameter passed by this method when the script is called.
	*/
	void addToStream(MemoryStream* mstream, PyObject* args);

	/** 
		Unpack a call stream and return a args of type PyObject
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
		Alias id, when the total number of exposed methods or broadcast properties is less than 255
		We do not use utype and use a 1-byte aliasID to transmit
	*/
	INLINE int16 aliasID() const;
	INLINE uint8 aliasIDAsUint8() const;
	INLINE void aliasID(int16 v);
	
protected:
	static uint32 methodDescriptionCount_; // number of all attribute descriptions

	COMPONENT_ID							methodDomain_;

	std::string name_; // the name of this method
	ENTITY_METHOD_UID utype_; // The numeric category of this method, used for transmission identification on the network

	std::vector<DataType*> argTypes_; // List of parameter categories for this attribute

	EXPOSED_TYPE exposedType_; // Is it an exposure method?

	int16 aliasID_; // alias id, when the total number of exposed methods or broadcast properties is less than 255, we do not use utype and use 1 byte aliasID to transmit
};

}

#ifdef CODE_INLINE
#include "method.inl"
#endif
#endif // OUROBOROS_DEF_METHOD_H
