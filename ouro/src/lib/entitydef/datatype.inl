// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


namespace Ouroboros { 

INLINE DATATYPE_UID DataType::id() const 
{ 
	return id_; 
}

INLINE void DataType::aliasName(std::string aliasName)
{ 
	aliasName_ = aliasName; 
}

INLINE const char* DataType::aliasName(void) const
{ 
	return aliasName_.c_str(); 
}

}
