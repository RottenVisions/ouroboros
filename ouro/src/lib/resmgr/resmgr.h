// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

/*
	Resource Manager.
*/
#ifndef OURO_RESMGR_H
#define OURO_RESMGR_H

#include "resourceobject.h"
#include "common/common.h"
#include "common/singleton.h"
#include "common/timer.h"
#include "xml/xml.h"
#include "common/smartpointer.h"

namespace Ouroboros{

#define RESOURCE_NORMAL	0x00000000
#define RESOURCE_RESIDENT 0x00000001
#define RESOURCE_READ 0x00000002
#define RESOURCE_WRITE 0x00000004
#define RESOURCE_APPEND 0x00000008

class Resmgr : public Singleton<Resmgr>, public TimerHandler
{
public:
	// Engine environment variables
	struct OUROEnv
	{
		std::string root_path;
		std::string res_path;
		std::string bin_path;
	};

	static uint64 respool_timeout;
	static uint32 respool_buffersize;
	static uint32 respool_checktick;

public:
	Resmgr();
	~Resmgr();

	bool initialize();

	void autoSetPaths();
	void updatePaths();

	const Resmgr::OUROEnv& getEnv() { return kb_env_; }

	/*
		Match from the resource path (specified in the environment variable) to the full resource address
	*/
	std::string matchRes(const std::string& res);
	std::string matchRes(const char* res);

	bool hasRes(const std::string& res);

	FILE* openRes(std::string res, const char* mode = "r");

	/*
		List all resource files in the directory
	*/
	bool listPathRes(std::wstring path, const std::wstring& extendName, std::vector<std::wstring>& results);

	/*
		Matches from the resource path (specified in the environment variable) to the directory
	*/
	std::string matchPath(const std::string& path);
	std::string matchPath(const char* path);

	const std::vector<std::string>& respaths() {
		return respaths_;
	}

	void print(void);

	bool isInit(){
		return isInit_;
	}

	/**
		Get Engine System Level Resource Directory
		ouro\\res\\*
	*/
	std::string getPySysResPath();

	/**
		Get a user-level resource directory
		assets\\res\\*
	*/
	std::string getPyUserResPath();

	/**
		Get a user-level script directory
		assets\\scripts\\*
	*/
	std::string getPyUserScriptsPath();

	/**
		Get User Level Library Directory
		assets\\*
	*/
	std::string getPyUserAssetsPath();

	ResourceObjectPtr openResource(const char* res, const char* model,
		uint32 flags = RESOURCE_NORMAL);

	bool initializeWatcher();

	void update();

private:

	virtual void handleTimeout(TimerHandle handle, void * arg);

	OUROEnv kb_env_;
	std::vector<std::string> respaths_;
	bool isInit_;

	OUROUnordered_map< std::string, ResourceObjectPtr > respool_;

	Ouroboros::thread::ThreadMutex mutex_;
};

}

#endif // OURO_RESMGR_H
