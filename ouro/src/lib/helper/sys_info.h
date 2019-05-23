// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OUROBOROS_SYSINFO_H
#define OUROBOROS_SYSINFO_H

#include "debug_helper.h"
#include "common/common.h"
#include "common/singleton.h"

namespace Ouroboros
{

class SystemInfo : public Singleton<SystemInfo>
{
public:
	SystemInfo();
	~SystemInfo();

	struct PROCESS_INFOS
	{
		float cpu;
		uint64 memused;
		bool error;
	};

	struct MEM_INFOS
	{
		uint64 total;
		uint64 free;
		uint64 used;
	};

	SystemInfo::MEM_INFOS getMemInfos();
	float getCPUPer();
	float getCPUPerByPID(uint32 pid = 0);
	uint64 getMemUsedByPID(uint32 pid = 0);
	bool hasProcess(uint32 pid);

	uint32 countCPU();

	uint64 totalmem();

	SystemInfo::PROCESS_INFOS getProcessInfo(uint32 pid = 0);

	void update();
	
	void clear();

	std::vector< std::string > getMacAddresses();

private:
	bool _autocreate();
	uint64 totalmem_;
};

}

#include "sys_info.inl"

#endif // OUROBOROS_SYSINFO_H


