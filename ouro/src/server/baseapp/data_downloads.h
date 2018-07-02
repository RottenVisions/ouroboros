// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_DATA_DOWNLOADS_H
#define OURO_DATA_DOWNLOADS_H

#include "helper/debug_helper.h"
#include "common/common.h"

namespace Ouroboros{

class DataDownload;


class DataDownloads
{
public:
	DataDownloads();
	~DataDownloads();
	
	int16 pushDownload(DataDownload* pdl);

	void onDownloadCompleted(DataDownload* pdl);

	int16 freeID(int16 id);
private:
	std::map<int16, DataDownload*> downloads_;

	std::set< uint16 > usedIDs_;
};

class DataDownloadFactory
{
public:
	enum DataDownloadType
	{
		DATA_DOWNLOAD_STREAM_FILE = 1,
		DATA_DOWNLOAD_STREAM_STRING = 2
	};

	static DataDownload * create(DataDownloadType dltype, PyObjectPtr objptr, 
			const std::string & desc, int16 id);

};


}

#endif // OURO_DATA_DOWNLOADS_H
