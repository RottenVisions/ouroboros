// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "KBECommon.h"
#include "KBVar.h"


/*
	Basic data types supported by entitydef
	The classes in the modified module abstract all the supported types and provide serialization of these types of data into binary data and deserialization operations (mainly used for packaging and unpacking network communication).
*/
namespace Ouroboros
{

class Bundle;
class MemoryStream;

class OUROBOROSPLUGINS_API DATATYPE_BASE
{
public:
		DATATYPE_BASE()
		{
		}
		
		virtual ~DATATYPE_BASE()
		{
		}

		virtual void bind()
		{
		}

		virtual KBVar* createFromStream(MemoryStream& stream)
		{
			return NULL;
		}
		
		virtual void addToStream(Bundle& stream, KBVar& v)
		{
		}
		
		virtual KBVar* parseDefaultValStr(const FString& v)
		{
			return NULL;
		}
		
		virtual bool isSameType(KBVar& v)
		{
			return false;
		}
		
		virtual FString c_str() const {
			return TEXT("UNKNOWN");
		}

protected:

};

class OUROBOROSPLUGINS_API DATATYPE_INT8 : public DATATYPE_BASE
{
public:
	virtual KBVar* createFromStream(MemoryStream& stream) override;
	virtual void addToStream(Bundle& stream, KBVar& v) override;

	virtual KBVar* parseDefaultValStr(const FString& v) override;

	virtual bool isSameType(KBVar& v) override;

	virtual FString c_str() const {
		return TEXT("INT8");
	}
};

class OUROBOROSPLUGINS_API DATATYPE_INT16 : public DATATYPE_BASE
{
public:
	virtual KBVar* createFromStream(MemoryStream& stream) override;
	virtual void addToStream(Bundle& stream, KBVar& v) override;

	virtual KBVar* parseDefaultValStr(const FString& v) override;

	virtual bool isSameType(KBVar& v) override;

	virtual FString c_str() const {
		return TEXT("INT16");
	}
};

class OUROBOROSPLUGINS_API DATATYPE_INT32 : public DATATYPE_BASE
{
public:
	virtual KBVar* createFromStream(MemoryStream& stream) override;
	virtual void addToStream(Ouroboros::Bundle& stream, KBVar& v) override;

	virtual KBVar* parseDefaultValStr(const FString& v) override;

	virtual bool isSameType(KBVar& v) override;

	virtual FString c_str() const {
		return TEXT("INT32");
	}
};

class OUROBOROSPLUGINS_API DATATYPE_INT64 : public DATATYPE_BASE
{
public:
	virtual KBVar* createFromStream(MemoryStream& stream) override;
	virtual void addToStream(Bundle& stream, KBVar& v) override;

	virtual KBVar* parseDefaultValStr(const FString& v) override;

	virtual bool isSameType(KBVar& v) override;

	virtual FString c_str() const {
		return TEXT("INT64");
	}
};

class OUROBOROSPLUGINS_API DATATYPE_UINT8 : public DATATYPE_BASE
{
public:
	virtual KBVar* createFromStream(MemoryStream& stream) override;
	virtual void addToStream(Bundle& stream, KBVar& v) override;

	virtual KBVar* parseDefaultValStr(const FString& v) override;

	virtual bool isSameType(KBVar& v) override;

	virtual FString c_str() const {
		return TEXT("UINT8");
	}
};

class OUROBOROSPLUGINS_API DATATYPE_UINT16 : public DATATYPE_BASE
{
public:
	virtual KBVar* createFromStream(MemoryStream& stream) override;
	virtual void addToStream(Bundle& stream, KBVar& v) override;

	virtual KBVar* parseDefaultValStr(const FString& v) override;

	virtual bool isSameType(KBVar& v) override;

	virtual FString c_str() const {
		return TEXT("UINT6");
	}
};

class OUROBOROSPLUGINS_API DATATYPE_UINT32 : public DATATYPE_BASE
{
public:
	virtual KBVar* createFromStream(MemoryStream& stream) override;
	virtual void addToStream(Bundle& stream, KBVar& v) override;

	virtual KBVar* parseDefaultValStr(const FString& v) override;

	virtual bool isSameType(KBVar& v) override;

	virtual FString c_str() const {
		return TEXT("UINT32");
	}
};

class OUROBOROSPLUGINS_API DATATYPE_UINT64 : public DATATYPE_BASE
{
public:
	virtual KBVar* createFromStream(MemoryStream& stream) override;
	virtual void addToStream(Bundle& stream, KBVar& v) override;

	virtual KBVar* parseDefaultValStr(const FString& v) override;

	virtual bool isSameType(KBVar& v) override;

	virtual FString c_str() const {
		return TEXT("UINT64");
	}
};

class OUROBOROSPLUGINS_API DATATYPE_FLOAT : public DATATYPE_BASE
{
public:
	virtual KBVar* createFromStream(MemoryStream& stream) override;
	virtual void addToStream(Bundle& stream, KBVar& v) override;

	virtual KBVar* parseDefaultValStr(const FString& v) override;

	virtual bool isSameType(KBVar& v) override;

	virtual FString c_str() const {
		return TEXT("FLOAT");
	}
};

class OUROBOROSPLUGINS_API DATATYPE_DOUBLE : public DATATYPE_BASE
{
public:
	virtual KBVar* createFromStream(MemoryStream& stream) override;
	virtual void addToStream(Bundle& stream, KBVar& v) override;

	virtual KBVar* parseDefaultValStr(const FString& v) override;

	virtual bool isSameType(KBVar& v) override;

	virtual FString c_str() const {
		return TEXT("DOUBLE");
	}
};

class OUROBOROSPLUGINS_API DATATYPE_STRING : public DATATYPE_BASE
{
public:
	virtual KBVar* createFromStream(MemoryStream& stream) override;
	virtual void addToStream(Bundle& stream, KBVar& v) override;

	virtual KBVar* parseDefaultValStr(const FString& v) override;

	virtual bool isSameType(KBVar& v) override;

	virtual FString c_str() const {
		return TEXT("STRING");
	}
};

class OUROBOROSPLUGINS_API DATATYPE_VECTOR2 : public DATATYPE_BASE
{
public:
	virtual KBVar* createFromStream(MemoryStream& stream) override;
	virtual void addToStream(Bundle& stream, KBVar& v) override;

	virtual KBVar* parseDefaultValStr(const FString& v) override;

	virtual bool isSameType(KBVar& v) override;

	virtual FString c_str() const {
		return TEXT("VECTOR2");
	}
};

class OUROBOROSPLUGINS_API DATATYPE_VECTOR3 : public DATATYPE_BASE
{
public:
	virtual KBVar* createFromStream(MemoryStream& stream) override;
	virtual void addToStream(Bundle& stream, KBVar& v) override;

	virtual KBVar* parseDefaultValStr(const FString& v) override;

	virtual bool isSameType(KBVar& v) override;

	virtual FString c_str() const {
		return TEXT("VECTOR3");
	}
};

class OUROBOROSPLUGINS_API DATATYPE_VECTOR4 : public DATATYPE_BASE
{
public:
	virtual KBVar* createFromStream(MemoryStream& stream) override;
	virtual void addToStream(Bundle& stream, KBVar& v) override;

	virtual KBVar* parseDefaultValStr(const FString& v) override;

	virtual bool isSameType(KBVar& v) override;

	virtual FString c_str() const {
		return TEXT("VECTOR4");
	}
};

class OUROBOROSPLUGINS_API DATATYPE_PYTHON : public DATATYPE_BASE
{
public:
	virtual KBVar* createFromStream(MemoryStream& stream) override;
	virtual void addToStream(Bundle& stream, KBVar& v) override;

	virtual KBVar* parseDefaultValStr(const FString& v) override;

	virtual bool isSameType(KBVar& v) override;

	virtual FString c_str() const {
		return TEXT("PYTHON");
	}
};

class OUROBOROSPLUGINS_API DATATYPE_UNICODE : public DATATYPE_BASE
{
public:
	virtual KBVar* createFromStream(MemoryStream& stream) override;
	virtual void addToStream(Bundle& stream, KBVar& v) override;

	virtual KBVar* parseDefaultValStr(const FString& v) override;

	virtual bool isSameType(KBVar& v) override;

	virtual FString c_str() const {
		return TEXT("UNICODE|STRING");
	}
};

class OUROBOROSPLUGINS_API DATATYPE_ENTITYCALL : public DATATYPE_BASE
{
public:
	virtual KBVar* createFromStream(MemoryStream& stream) override;
	virtual void addToStream(Bundle& stream, KBVar& v) override;

	virtual KBVar* parseDefaultValStr(const FString& v) override;

	virtual bool isSameType(KBVar& v) override;

	virtual FString c_str() const {
		return TEXT("ENTITYCALL");
	}
};

class OUROBOROSPLUGINS_API DATATYPE_BLOB : public DATATYPE_BASE
{
public:
	virtual KBVar* createFromStream(MemoryStream& stream) override;
	virtual void addToStream(Bundle& stream, KBVar& v) override;

	virtual KBVar* parseDefaultValStr(const FString& v) override;

	virtual bool isSameType(KBVar& v) override;

	virtual FString c_str() const {
		return TEXT("BLOB|TArray<uint8>");
	}
};

class OUROBOROSPLUGINS_API DATATYPE_ARRAY : public DATATYPE_BASE
{
public:
	DATATYPE_ARRAY() :
		vtype(NULL),
		tmpset_uitemtype(-1)
	{

	}

	virtual void bind() override;

	virtual KBVar* createFromStream(MemoryStream& stream) override;
	virtual void addToStream(Bundle& stream, KBVar& v) override;

	virtual KBVar* parseDefaultValStr(const FString& v) override;

	virtual bool isSameType(KBVar& v) override;

	virtual FString c_str() const {
		return TEXT("KB_ARRAY");
	}

public:
	DATATYPE_BASE* vtype;
	int tmpset_uitemtype;
};

class OUROBOROSPLUGINS_API DATATYPE_FIXED_DICT : public DATATYPE_BASE
{
public:
	DATATYPE_FIXED_DICT() :
		implementedBy(),
		dicttype(),
		dicttype_map()
	{

	}

	virtual void bind() override;

	virtual KBVar* createFromStream(MemoryStream& stream) override;
	virtual void addToStream(Bundle& stream, KBVar& v) override;

	virtual KBVar* parseDefaultValStr(const FString& v) override;

	virtual bool isSameType(KBVar& v) override;

	virtual FString c_str() const {
		return TEXT("KB_FIXED_DICT");
	}

public:
	FString implementedBy;
	TMap<FString, DATATYPE_BASE*> dicttype;
	TMap<FString, uint16> dicttype_map;
};

}
