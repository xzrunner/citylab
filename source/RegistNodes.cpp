#include "citylab/RegistNodes.h"

#include <ee0/ReflectPropTypes.h>

#include <js/RTTR.h>

RTTR_REGISTRATION
{

// base

rttr::registration::class_<citylab::Node>("citylab::node")
.property("name", &citylab::Node::GetName, &citylab::Node::SetName)
(
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Name"))
)
;

#define EXE_FILEPATH "citylab/node_rttr_gen.h"
#define SKIP_FILE_NODE
#include "citylab/node_regist_cfg.h"
#undef SKIP_FILE_NODE
#undef EXE_FILEPATH

}

namespace citylab
{

void nodes_regist_rttr()
{
}

}