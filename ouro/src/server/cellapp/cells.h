// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_CELLS_H
#define OURO_CELLS_H

// common include
#include "cell.h"
#include "helper/debug_helper.h"
#include "common/common.h"


namespace Ouroboros{

class Cells
{
public:
	Cells();
	~Cells();

	ArraySize size() const{ return (ArraySize)cells_.size(); }

private:
	std::map<CELL_ID, Cell> cells_;
};

}
#endif
