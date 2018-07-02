// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com


#ifndef OURO_MEMORY_HELPER_H
#define OURO_MEMORY_HELPER_H

#include "common/common.h"
#include "common/ouromalloc.h"
#include "helper/debug_helper.h"

#if OURO_PLATFORM == PLATFORM_WIN32
#ifdef _DEBUG
//#include "vld/vld.h"

namespace Ouroboros{
	inline void startLeakDetection(COMPONENT_TYPE type, COMPONENT_ID id)
	{
		//std::wstring leak_filename = fmt::format(L".\\{}_{}.leaks", COMPONENT_NAME_EX(type), 
		//	id);

		//VLDSetReportOptions (VLD_OPT_REPORT_TO_DEBUGGER | VLD_OPT_REPORT_TO_FILE, leak_filename.c_str());
	}
}
#else
namespace Ouroboros{
	inline void startLeakDetection(COMPONENT_TYPE type, COMPONENT_ID id){}
}
#endif
#else
namespace Ouroboros{
	inline void startLeakDetection(COMPONENT_TYPE type, COMPONENT_ID id){}
}
#endif

namespace Ouroboros{



}

#endif // OURO_MEMORY_HELPER_H
