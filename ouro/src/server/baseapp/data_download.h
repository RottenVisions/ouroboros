// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_DATA_DOWNLOAD_H
#define OURO_DATA_DOWNLOAD_H

#include "helper/debug_helper.h"
#include "common/common.h"
#include "common/memorystream.h"
#include "thread/threadtask.h"
#include "network/message_handler.h"

namespace Ouroboros{

class DataDownloads;

class DataDownload : public thread::TPTask
{
public:
	DataDownload(PyObjectPtr objptr, 
		const std::string & descr, int16 id);

	virtual ~DataDownload();
	
	virtual bool checkDescr(){ return true; }

	void pDataDownloads(DataDownloads* pDataDownloads){ pDataDownloads_ = pDataDownloads; }
	DataDownloads* pDataDownloads(){ return pDataDownloads_; }

	virtual thread::TPTask::TPTaskState presentMainThread();

	void entityID(ENTITY_ID entityID){ entityID_ = entityID; }
	ENTITY_ID entityID(){ return entityID_; }

	bool send(const Network::MessageHandler& msgHandler, Network::Bundle* pBundle);

	void id(int16 i){ id_ = i; }
	int16 id() const{ return id_; }

	uint32 totalBytes() const{ return totalBytes_; }

	virtual char* getOutStream(){ return stream_; }

	virtual int8 type() = 0;
protected:
	PyObjectPtr objptr_;
	std::string descr_;
	int16 id_;
	DataDownloads* pDataDownloads_;

	bool sentStart_;

	uint32 totalBytes_;

	// The total number of bytes sent
	uint32 totalSentBytes_;
	uint32 remainSent_;
	uint32 currSent_;

	char* stream_;

	ENTITY_ID entityID_;

	bool error_;
};

class StringDataDownload : public DataDownload
{
public:
	StringDataDownload(PyObjectPtr objptr, 
		const std::string & descr, int16 id);

	virtual ~StringDataDownload();

	virtual bool process();

	virtual char* getOutStream();

	virtual int8 type();
};

class FileDataDownload : public DataDownload
{
public:
	FileDataDownload(PyObjectPtr objptr, 
		const std::string & descr, int16 id);

	virtual ~FileDataDownload();

	virtual bool process();

	virtual int8 type();
protected:
	std::string path_;
	
};

}

#endif // OURO_DATA_DOWNLOAD_H
