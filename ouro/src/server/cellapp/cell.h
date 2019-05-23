// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com

#ifndef OURO_CELL_H
#define OURO_CELL_H

// common include
#include "helper/debug_helper.h"
#include "common/common.h"


namespace Ouroboros{

class Cell
{
public:
	Cell(CELL_ID id);
	~Cell();

	CELL_ID id() const{ return id_; }

private:
	CELL_ID id_;
};

}
#endif
