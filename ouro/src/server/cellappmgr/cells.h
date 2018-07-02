// 2017-2018 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_MGR_CELLS_H
#define OURO_MGR_CELLS_H

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

	std::map<CELL_ID, Cell>& cells() {
		return cells_;
	}

private:
	std::map<CELL_ID, Cell> cells_;
};

}
#endif
