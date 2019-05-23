// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_MGR_SPACE_H
#define OURO_MGR_SPACE_H

// common include
#include "cells.h"
#include "helper/debug_helper.h"
#include "common/common.h"


namespace Ouroboros{

class Space
{
public:
	Space();
	~Space();

	void updateGeomappingPath(const std::string& geomappingPath);
	void updateScriptModuleName(const std::string& scriptModuleName);
	void setSpaceID(SPACE_ID spaceID) { spaceID_ = spaceID; }

	SPACE_ID id() const { return spaceID_; }
	std::string& getGeomappingPath() { return geomappingPath_; }
	std::string& getScriptModuleName() { return scriptModuleName_; }

	Cells& cells() { return cells_; }

private:
	SPACE_ID spaceID_;
	Cells cells_;

	std::string geomappingPath_;
	std::string scriptModuleName_;
};

}
#endif
