// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OURO_DATA_TYPES_H
#define OURO_DATA_TYPES_H

#include "common/common.h"
#if OURO_PLATFORM == PLATFORM_WIN32
#pragma warning (disable : 4910)
#pragma warning (disable : 4251)
#endif
// common include
#include "datatype.h"
#include "xml/xml.h"
#include "common/smartpointer.h"

namespace Ouroboros{
typedef SmartPointer<DataType> DataTypePtr;

class DataTypes
{
public:
	typedef std::map<std::string, DataTypePtr> DATATYPE_MAP;
	typedef std::map<DATATYPE_UID, DataType*> UID_DATATYPE_MAP;
	typedef std::vector< std::string > DATATYPE_ORDERS;

	DataTypes();
	virtual ~DataTypes();

	static bool initialize(std::string file);
	static void finalise(void);

	static bool addDataType(std::string name, DataType* dataType);
	static bool addDataType(DATATYPE_UID uid, DataType* dataType);
	static void delDataType(std::string name);

	static DataType* getDataType(std::string name, bool notFoundOutError = true);
	static DataType* getDataType(const char* name, bool notFoundOutError = true);
	static DataType* getDataType(DATATYPE_UID uid);

	static bool loadTypes(std::string& file);

	static const DATATYPE_MAP& dataTypes(){ return dataTypes_; }
	static const UID_DATATYPE_MAP& uid_dataTypes(){ return uid_dataTypes_; }
	static const DATATYPE_ORDERS& dataTypesOrders() { return dataTypesOrders_; }

protected:
	static DATATYPE_MAP dataTypes_;
	static DATATYPE_MAP dataTypesLowerName_;
	static UID_DATATYPE_MAP uid_dataTypes_;

	// The order of the type definition is used for code generation, c and other languages need to be ordered
	static DATATYPE_ORDERS dataTypesOrders_;
};

}
#endif // OURO_DATA_TYPES_H
