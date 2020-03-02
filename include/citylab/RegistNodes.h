#pragma once

#include "citylab/Node.h"

namespace citylab
{

void nodes_regist_rttr();

namespace node
{

#define EXE_FILEPATH "citylab/node_def_gen.h"
#include "citylab/node_regist_cfg.h"
#undef EXE_FILEPATH

}

}