// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OUROBOROS_DEF_PROPERTY_H
#define OUROBOROS_DEF_PROPERTY_H

#include "common/common.h"
#if OURO_PLATFORM == PLATFORM_WIN32
#pragma warning (disable : 4910)
#pragma warning (disable : 4251)
#endif

#include "fixedarray.h"
#include "fixeddict.h"
#include "datatype.h"
#include "common/refcountable.h"
#include "common/memorystream.h"


namespace Ouroboros{

class RefCountable;
class PropertyDescription : public RefCountable
{
public:
	PropertyDescription(ENTITY_PROPERTY_UID utype,
		std::string dataTypeName,
		std::string name,
		uint32 flags,
		bool isPersistent,
		DataType* dataType,
		bool isIdentifier,
		std::string indexType,
		uint32 databaseLength,
		std::string defaultStr,
		DETAIL_TYPE detailLevel);

	virtual ~PropertyDescription();

	/**
		Get the detailLevel of this property
	*/
	INLINE int8 getDetailLevel(void) const;

	/**
		Whether it is a property saved to the database
	*/
	INLINE bool isPersistent(void) const;
	INLINE void isPersistent(bool v);

	/**
		Get the data type of this attribute
	*/
	INLINE DataType* getDataType(void) const;

	/**
		Get the attribute's flag cell_public etc.
	*/
	INLINE uint32 getFlags(void) const;
	INLINE void setFlags(uint32 flags);

	/**
		Get the property name
	*/
	INLINE const char* getName(void) const;

	/**
		Get the string data category name UINT32, BAG..
	*/
	INLINE const char* getDataTypeName(void) const;

	/**
		Get the initial value string
	*/
	INLINE const char* getDefaultValStr(void) const;

	/**
		The digital category of attributes for transmission identification on the network
	*/
	INLINE ENTITY_PROPERTY_UID getUType(void) const;

	/**
		Get attribute index category
	*/
	INLINE const char* indexType(void) const;

	/**
			Alias id, when the total number of exposed methods or broadcasted attributes is less than 255
			we do not use utype and use 1 byte aliasID to transmit
	*/
	INLINE int16 aliasID() const;
	INLINE uint8 aliasIDAsUint8() const;
	INLINE void aliasID(int16 v);

	/**
		Set this property as an index key
	*/
	INLINE void setIdentifier(bool isIdentifier);

	/**
		Set the length of this property in the database
	*/
	INLINE void setDatabaseLength(uint32 databaseLength);
	INLINE uint32 getDatabaseLength() const;

	/**
		Get this property to describe the default value defined in the def file
	*/
	virtual PyObject* newDefaultVal(void);

	/**
		Get the total number of attribute descriptions
	*/
	static uint32 getDescriptionCount(void){ return propertyDescriptionCount_; }
	static void resetDescriptionCount(void){ propertyDescriptionCount_ = 0; }

	/**
		Generate a description instance based on the type
	*/
	static PropertyDescription* createDescription(ENTITY_PROPERTY_UID utype,
		const std::string& dataTypeName,
		const std::string& name,
		uint32 flags,
		bool isPersistent,
		DataType* dataType,
		bool isIdentifier,
		std::string indexType,
		uint32 databaseLength,
		const std::string& defaultStr,
		DETAIL_TYPE detailLevel);

	/**
		Script request to set the value of this property
	*/
	virtual PyObject* onSetValue(PyObject* parentObj, PyObject* value);

	virtual void addToStream(MemoryStream* mstream, PyObject* pyValue);
	virtual PyObject* createFromStream(MemoryStream* mstream);
	virtual PyObject* parseDefaultStr(const std::string& defaultVal);

	virtual bool isSameType(PyObject* pyValue);
	virtual bool isSamePersistentType(PyObject* pyValue);

	virtual void addPersistentToStream(MemoryStream* mstream, PyObject* pyValue);
	virtual PyObject* createFromPersistentStream(MemoryStream* mstream);

	INLINE bool hasCell(void) const;
	INLINE bool hasBase(void) const;
	INLINE bool hasClient(void) const;

protected:
	static uint32				propertyDescriptionCount_;						// The number of all attribute descriptions
	std::string					name_;											// The name of this attribute
	std::string					dataTypeName_;									// The string data category name of this attribute
	uint32						flags_;											// Some of the attributes of this attribute such as cell_public
	bool						isPersistent_;									// Is it a property stored to the database
	DataType*					dataType_;										// The data type of this attribute
	bool						isIdentifier_;									// Is it an index key?
	uint32						databaseLength_;								// The length of this attribute in the database
	ENTITY_PROPERTY_UID			utype_;											// The digital category of this attribute, used for transmission identification on the network
	std::string					defaultValStr_;									// The default value of this property
	DETAIL_TYPE					detailLevel_;									// The lod detail level of this attribute looks at the common: definition of the lod broadcast level range of the attribute
	int16						aliasID_;										// Alias id, when the total number of exposed methods or broadcasted attributes is less than 255, we do not use utype and use 1 byte aliasID to transmit
	std::string					indexType_;										// The attribute index category, UNIQUE, INDEX, corresponding to no setting, unique index, ordinary index
};

class FixedDictDescription : public PropertyDescription
{
public:
	FixedDictDescription(ENTITY_PROPERTY_UID utype,
		std::string dataTypeName,
		std::string name,
		uint32 flags,
		bool isPersistent,
		DataType* dataType,
		bool isIdentifier,
		std::string indexType,
		uint32 databaseLength,
		std::string defaultStr,
		DETAIL_TYPE detailLevel);

	virtual ~FixedDictDescription();

	/**
		Script request to set the value of this property
	*/
	PyObject* onSetValue(PyObject* parentObj, PyObject* value);

	virtual void addPersistentToStream(MemoryStream* mstream, PyObject* pyValue);
	virtual PyObject* createFromPersistentStream(MemoryStream* mstream);

	typedef std::vector<std::pair<std::string, OUROShared_ptr<PropertyDescription> > > CHILD_PROPERTYS;

protected:
	CHILD_PROPERTYS childPropertys_;
};

class ArrayDescription : public PropertyDescription
{
public:
	ArrayDescription(ENTITY_PROPERTY_UID utype,
		std::string dataTypeName,
		std::string name,
		uint32 flags,
		bool isPersistent,
		DataType* dataType,
		bool isIdentifier,
		std::string indexType,
		uint32 databaseLength,
		std::string defaultStr,
		DETAIL_TYPE detailLevel);

	virtual ~ArrayDescription();

	/**
		Script request to set the value of this property
	*/
	PyObject* onSetValue(PyObject* parentObj, PyObject* value);

	virtual void addPersistentToStream(MemoryStream* mstream, PyObject* pyValue);
	virtual PyObject* createFromPersistentStream(MemoryStream* mstream);

protected:
};

class VectorDescription : public PropertyDescription
{
public:
	VectorDescription(ENTITY_PROPERTY_UID utype,
		std::string dataTypeName,
		std::string name,
		uint32 flags,
		bool isPersistent,
		DataType* dataType,
		bool isIdentifier,
		std::string indexType,
		uint32 databaseLength,
		std::string defaultStr,
		DETAIL_TYPE detailLevel,
		uint8 elemCount);

	virtual ~VectorDescription();

	/**
		Script request to set the value of this property
	*/
	PyObject* onSetValue(PyObject* parentObj, PyObject* value);

protected:
	uint8 elemCount_;
};

class EntityComponentDescription : public PropertyDescription
{
public:
	EntityComponentDescription(ENTITY_PROPERTY_UID utype,
		std::string dataTypeName,
		std::string name,
		uint32 flags,
		bool isPersistent,
		DataType* dataType,
		bool isIdentifier,
		std::string indexType,
		uint32 databaseLength,
		std::string defaultStr,
		DETAIL_TYPE detailLevel);

	virtual ~EntityComponentDescription();

	/**
		Script request to set the value of this property
	*/
	PyObject* onSetValue(PyObject* parentObj, PyObject* value);

	virtual bool isSamePersistentType(PyObject* pyValue);
	virtual void addPersistentToStream(MemoryStream* mstream, PyObject* pyValue);
	void addPersistentToStreamTemplates(ScriptDefModule* pScriptModule, MemoryStream* mstream);
	virtual PyObject* createFromPersistentStream(MemoryStream* mstream);
	PyObject* createFromPersistentStream(ScriptDefModule* pScriptModule, MemoryStream* mstream);

	virtual PyObject* createFromStream(MemoryStream* mstream);

	/**
		Get this property to describe the default value defined in the def file
	*/
	virtual PyObject* newDefaultVal(void);

	virtual PyObject* parseDefaultStr(const std::string& defaultVal);

protected:
};

}

#ifdef CODE_INLINE
#include "property.inl"
#endif
#endif // OUROBOROS_DEF_PROPERTY_H
