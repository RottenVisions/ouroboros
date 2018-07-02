// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#include "resmgr.h"
#include "helper/watcher.h"
#include "thread/threadguard.h"

#if OURO_PLATFORM != PLATFORM_WIN32
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#else
#include <tchar.h>
#include <direct.h>
#endif

namespace Ouroboros{
OURO_SINGLETON_INIT(Resmgr);

uint64 Resmgr::respool_timeout = 0;
uint32 Resmgr::respool_buffersize = 0;
uint32 Resmgr::respool_checktick = 0;

//-------------------------------------------------------------------------------------
Resmgr::Resmgr():
kb_env_(),
respaths_(),
isInit_(false),
respool_(),
mutex_()
{
}

//-------------------------------------------------------------------------------------
Resmgr::~Resmgr()
{
	respool_.clear();
}

//-------------------------------------------------------------------------------------
bool Resmgr::initializeWatcher()
{
	WATCH_OBJECT("syspaths/OURO_ROOT", kb_env_.root_path);
	WATCH_OBJECT("syspaths/OURO_RES_PATH", kb_env_.res_path);
	WATCH_OBJECT("syspaths/OURO_BIN_PATH", kb_env_.bin_path);
	return true;
}

//-------------------------------------------------------------------------------------
void Resmgr::autoSetPaths()
{
	char path[MAX_BUF];
	char* ret = getcwd(path, MAX_BUF);
	if(ret == NULL)
		return;

	std::string s = path;
	size_t pos1;

	strutil::ouro_replace(s, "\\", "/");
	strutil::ouro_replace(s, "//", "/");
	pos1 = s.find("/ouro/bin/");

	if(pos1 == std::string::npos)
		return;

	s = s.substr(0, pos1 + 1);
	kb_env_.root_path = s;
	kb_env_.res_path = kb_env_.root_path + "ouro/res/;" + kb_env_.root_path + "/assets/;" + kb_env_.root_path + "/assets/scripts/;" + kb_env_.root_path + "/assets/res/";
}

//-------------------------------------------------------------------------------------
void Resmgr::updatePaths()
{
	char ch;

	if (kb_env_.root_path.size() > 0)
	{
		ch = kb_env_.root_path.at(kb_env_.root_path.size() - 1);
		if(ch != '/' && ch != '\\')
			kb_env_.root_path += "/";

		strutil::ouro_replace(kb_env_.root_path, "\\", "/");
		strutil::ouro_replace(kb_env_.root_path, "//", "/");
	}

	if(kb_env_.bin_path.size() > 0)
	{
		ch =  kb_env_.bin_path.at(kb_env_.bin_path.size() - 1);
		if(ch != '/' && ch != '\\')
			kb_env_.bin_path += "/";

		strutil::ouro_replace(kb_env_.bin_path, "\\", "/");
		strutil::ouro_replace(kb_env_.bin_path, "//", "/");
	}

	respaths_.clear();
	std::string tbuf = kb_env_.res_path;
	char splitFlag = ';';
	strutil::ouro_split<char>(tbuf, splitFlag, respaths_);

	// Windows users cannot split the colon and may split the drive letter
#if OURO_PLATFORM != PLATFORM_WIN32
	if(respaths_.size() < 2)
	{
		respaths_.clear();
		splitFlag = ':';
		strutil::ouro_split<char>(tbuf, splitFlag, respaths_);
	}
#endif

	kb_env_.res_path = "";
	std::vector<std::string>::iterator iter = respaths_.begin();
	for(; iter != respaths_.end(); ++iter)
	{
		if((*iter).size() <= 0)
			continue;

		ch =  (*iter).at((*iter).size() - 1);
		if(ch != '/' && ch != '\\')
			(*iter) += "/";

		kb_env_.res_path += (*iter);
		kb_env_.res_path += splitFlag;
		strutil::ouro_replace(kb_env_.res_path, "\\", "/");
		strutil::ouro_replace(kb_env_.res_path, "//", "/");
	}

	if(kb_env_.res_path.size() > 0)
		kb_env_.res_path.erase(kb_env_.res_path.size() - 1);
}

//-------------------------------------------------------------------------------------
bool Resmgr::initialize()
{
	//if(isInit())
	//	return true;

	// Get engine environment configuration
	kb_env_.root_path		= getenv("OURO_ROOT") == NULL ? "" : getenv("OURO_ROOT");
	kb_env_.res_path		= getenv("OURO_RES_PATH") == NULL ? "" : getenv("OURO_RES_PATH");
	kb_env_.bin_path		= getenv("OURO_BIN_PATH") == NULL ? "" : getenv("OURO_BIN_PATH");

	//kb_env_.root			= "/home/ouroboros/";
	//kb_env_.res_path		= "/home/ouroboros/ouro/res/;/home/ouroboros/assets/;/home/ouroboros/assets/scripts/;/home/ouroboros/assets/res/";
	//kb_env_.bin_path		= "/home/ouroboros/ouro/bin/server/";
	updatePaths();

	if (kb_env_.root_path == "" || kb_env_.res_path == "")
		autoSetPaths();

	updatePaths();
	if(getPySysResPath() == "" || getPyUserResPath() == "" || getPyUserScriptsPath() == "")
	{
		printf("[ERROR] Resmgr::initialize: not set environment, (OURO_ROOT=%s, OURO_RES_PATH=%s, OURO_BIN_PATH=%s) invalid!\n",
			kb_env_.root_path.c_str(), kb_env_.res_path.c_str(), kb_env_.bin_path.c_str());
#if OURO_PLATFORM == PLATFORM_WIN32
		::MessageBox(0, L"Resmgr::initialize: not set environment, (OURO_ROOT, OURO_RES_PATH, OURO_BIN_PATH) invalid!\n", L"ERROR", MB_ICONERROR);
#endif
	}

	isInit_ = true;

	respool_.clear();
	return true;
}

//-------------------------------------------------------------------------------------
void Resmgr::print(void)
{
	INFO_MSG(fmt::format("Resmgr::initialize: OURO_ROOT={0}\n", kb_env_.root_path));
	INFO_MSG(fmt::format("Resmgr::initialize: OURO_RES_PATH={0}\n", kb_env_.res_path));
	INFO_MSG(fmt::format("Resmgr::initialize: OURO_BIN_PATH={0}\n", kb_env_.bin_path));

#if OURO_PLATFORM == PLATFORM_WIN32
	printf("%s", fmt::format("OURO_ROOT = {0}\n", kb_env_.root_path).c_str());
	printf("%s", fmt::format("OURO_RES_PATH = {0}\n", kb_env_.res_path).c_str());
	printf("%s", fmt::format("OURO_BIN_PATH = {0}\n", kb_env_.bin_path).c_str());
	printf("\n");
#endif
}

//-------------------------------------------------------------------------------------
std::string Resmgr::matchRes(const std::string& res)
{
	return matchRes(res.c_str());
}

//-------------------------------------------------------------------------------------
std::string Resmgr::matchRes(const char* res)
{
	std::vector<std::string>::iterator iter = respaths_.begin();

	for(; iter != respaths_.end(); ++iter)
	{
		std::string fpath = ((*iter) + res);

		strutil::ouro_replace(fpath, "\\", "/");
		strutil::ouro_replace(fpath, "//", "/");

		if (access(fpath.c_str(), 0) == 0)
		{
			return fpath;
		}
	}

	return res;
}

//-------------------------------------------------------------------------------------
bool Resmgr::hasRes(const std::string& res)
{
	std::vector<std::string>::iterator iter = respaths_.begin();

	for(; iter != respaths_.end(); ++iter)
	{
		std::string fpath = ((*iter) + res);

		strutil::ouro_replace(fpath, "\\", "/");
		strutil::ouro_replace(fpath, "//", "/");

		if (access(fpath.c_str(), 0) == 0)
		{
			return true;
		}
	}

	return false;
}

//-------------------------------------------------------------------------------------
FILE* Resmgr::openRes(std::string res, const char* mode)
{
	std::vector<std::string>::iterator iter = respaths_.begin();

	for(; iter != respaths_.end(); ++iter)
	{
		std::string fpath = ((*iter) + res);

		strutil::ouro_replace(fpath, "\\", "/");
		strutil::ouro_replace(fpath, "//", "/");

		FILE * f = fopen (fpath.c_str(), mode);
		if(f != NULL)
		{
			return f;
		}
	}

	return NULL;
}

//-------------------------------------------------------------------------------------
bool Resmgr::listPathRes(std::wstring path, const std::wstring& extendName, std::vector<std::wstring>& results)
{
	if(path.size() == 0)
	{
		ERROR_MSG("Resmgr::listPathRes: open dir [NULL] error!\n");
		return false;
	}

	if(path[path.size() - 1] != L'\\' && path[path.size() - 1] != L'/')
		path += L"/";

	std::vector<std::wstring> extendNames;
	strutil::ouro_split<wchar_t>(extendName, L'|', extendNames);

#if OURO_PLATFORM != PLATFORM_WIN32
	struct dirent *filename;
	DIR *dir;

    char* cpath = strutil::wchar2char(path.c_str());
    char pathstr[MAX_PATH];
    strcpy(pathstr, cpath);
    free(cpath);

	dir = opendir(pathstr);
	if(dir == NULL)
	{
		ERROR_MSG(fmt::format("Resmgr::listPathRes: open dir [{}] error!\n", pathstr));
		return false;
	}

	while((filename = readdir(dir)) != NULL)
	{
		if(strcmp(filename->d_name, ".") == 0 || strcmp(filename->d_name, "..") == 0)
			continue;

		struct stat s;
		char pathstrtmp[MAX_PATH];
		sprintf(pathstrtmp,"%s%s",pathstr, filename->d_name);
		lstat(pathstrtmp, &s);

		if(S_ISDIR(s.st_mode))
		{
			wchar_t* wstr = strutil::char2wchar(pathstrtmp);
			listPathRes(wstr, extendName, results);
			free(wstr);
		}
		else
		{
			wchar_t* wstr = strutil::char2wchar(filename->d_name);

			if(extendName.size() == 0 || extendName == L"*" || extendName == L"*.*")
			{
				results.push_back(path + wstr);
			}
			else
			{
				if(extendNames.size() > 0)
				{
					std::vector<std::wstring> vec;
					strutil::ouro_split<wchar_t>(wstr, L'.', vec);

					for(size_t ext = 0; ext < extendNames.size(); ++ext)
					{
						if(extendNames[ext].size() > 0 && vec.size() > 1 && vec[vec.size() - 1] == extendNames[ext])
						{
							results.push_back(path + wstr);
						}
					}
				}
				else
				{
					results.push_back(path + wstr);
				}
			}

			free(wstr);
		}
	}

	closedir(dir);

#else
	wchar_t szFind[MAX_PATH];
	WIN32_FIND_DATA FindFileData;
	wcscpy(szFind, path.c_str());
	wcscat(szFind, L"*");

	HANDLE hFind = FindFirstFile(szFind, &FindFileData);
	if(INVALID_HANDLE_VALUE == hFind)
	{
		char* cstr = strutil::wchar2char(path.c_str());
		ERROR_MSG(fmt::format("Resmgr::listPathRes: open dir [{}] error!\n", cstr));
		free(cstr);
		return false;
	}

	while(TRUE)
	{
		if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if(FindFileData.cFileName[0] != L'.')
			{
				wcscpy(szFind, path.c_str());
				wcscat(szFind, L"");
				wcscat(szFind, FindFileData.cFileName);
				listPathRes(szFind, extendName, results);
			}
		}
		else
		{
			if(extendName.size() == 0 || extendName == L"*" || extendName == L"*.*")
			{
				results.push_back(path + FindFileData.cFileName);
			}
			else
			{
				if(extendNames.size() > 0)
				{
					std::vector<std::wstring> vec;
					strutil::ouro_split<wchar_t>(FindFileData.cFileName, L'.', vec);

					for(size_t ext = 0; ext < extendNames.size(); ++ext)
					{
						if(extendNames[ext].size() > 0 && vec.size() > 1 && vec[vec.size() - 1] == extendNames[ext])
						{
							results.push_back(path + FindFileData.cFileName);
						}
					}
				}
				else
				{
					results.push_back(path + FindFileData.cFileName);
				}
			}
		}

		if(!FindNextFile(hFind, &FindFileData))
			break;
	}

	FindClose(hFind);

#endif

	return true;
}

//-------------------------------------------------------------------------------------
std::string Resmgr::matchPath(const std::string& path)
{
	return matchPath(path.c_str());
}

//-------------------------------------------------------------------------------------
std::string Resmgr::matchPath(const char* path)
{
	std::vector<std::string>::iterator iter = respaths_.begin();

	std::string npath = path;
	strutil::ouro_replace(npath, "\\", "/");
	strutil::ouro_replace(npath, "//", "/");

	for(; iter != respaths_.end(); ++iter)
	{
		std::string fpath = ((*iter) + npath);

		strutil::ouro_replace(fpath, "\\", "/");
		strutil::ouro_replace(fpath, "//", "/");

		if(!access(fpath.c_str(), 0))
		{
			return fpath;
		}
	}

	return "";
}

//-------------------------------------------------------------------------------------
std::string Resmgr::getPySysResPath()
{
	static std::string respath = "";

	if(respath == "")
	{
		respath = matchRes("server/ouroboros_defaults.xml");
		std::vector<std::string> tmpvec;
		tmpvec = Ouroboros::strutil::ouro_splits(respath, "server/ouroboros_defaults.xml");

		if(tmpvec.size() > 1)
		{
			respath = tmpvec[0];
		}
		else
		{
			if(respaths_.size() > 0)
				respath = respaths_[0];
		}
	}

	return respath;
}

//-------------------------------------------------------------------------------------
std::string Resmgr::getPyUserResPath()
{
	static std::string respath = "";

	if(respath == "")
	{
		respath = matchRes("server/ouroboros.xml");
		std::vector<std::string> tmpvec;
		tmpvec = Ouroboros::strutil::ouro_splits(respath, "server/ouroboros.xml");

		if(tmpvec.size() > 1)
		{
			respath = tmpvec[0];
		}
		else
		{
			if(respaths_.size() > 1)
				respath = respaths_[1];
			else if(respaths_.size() > 0)
				respath = respaths_[0];
		}
	}

	return respath;
}

//-------------------------------------------------------------------------------------
std::string Resmgr::getPyUserScriptsPath()
{
	static std::string path = "";

	if(path == "")
	{
		std::string entities_xml = "entities.xml";
		path = matchRes(entities_xml);

		if(path == entities_xml)
		{
			entities_xml = "scripts/" + entities_xml;
			path = matchRes(entities_xml);
			entities_xml = "entities.xml";
		}


		std::vector<std::string> tmpvec;
		tmpvec = Ouroboros::strutil::ouro_splits(path, entities_xml);
		if(tmpvec.size() > 1)
		{
			path = tmpvec[0];
		}
		else
		{
			if(respaths_.size() > 2)
				path = respaths_[2];
			else if(respaths_.size() > 1)
				path = respaths_[1];
			else if(respaths_.size() > 0)
				path = respaths_[0];
		}
	}

	return path;
}

//-------------------------------------------------------------------------------------
std::string Resmgr::getPyUserAssetsPath()
{
	static std::string path = "";

	if (path == "")
	{
		path = getPyUserScriptsPath();
		strutil::ouro_replace(path, "/scripts", "");
		strutil::ouro_replace(path, "\\scripts", "");
	}

	return path;
}

//-------------------------------------------------------------------------------------
ResourceObjectPtr Resmgr::openResource(const char* res, const char* model, uint32 flags)
{
	std::string respath = matchRes(res);

	if(Resmgr::respool_checktick == 0)
	{
		return new FileObject(respath.c_str(), flags, model);
	}

	Ouroboros::thread::ThreadGuard tg(&mutex_);
	OUROUnordered_map< std::string, ResourceObjectPtr >::iterator iter = respool_.find(respath);
	if(iter == respool_.end())
	{
		FileObject* fobj = new FileObject(respath.c_str(), flags, model);
		respool_[respath] = fobj;
		fobj->update();
		return fobj;
	}

	iter->second->update();
	return iter->second;
}

//-------------------------------------------------------------------------------------
void Resmgr::update()
{
	Ouroboros::thread::ThreadGuard tg(&mutex_);
	OUROUnordered_map< std::string, ResourceObjectPtr >::iterator iter = respool_.begin();
	for(; iter != respool_.end();)
	{
		if(!iter->second->valid())
		{
			respool_.erase(iter++);
		}
		else
		{
			iter++;
		}
	}
}

//-------------------------------------------------------------------------------------
void Resmgr::handleTimeout(TimerHandle handle, void * arg)
{
	update();
}

//-------------------------------------------------------------------------------------
}
