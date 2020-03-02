#include "citylab/Node.h"
#include "citylab/PinType.h"

#include <blueprint/Pin.h>
#include <blueprint/BackendAdapter.h>

#include <citygraph/OpVarType.h>

namespace citylab
{

Node::Node(const std::string& title)
    : bp::Node(title)
{
}

void Node::InitPins(const std::string& name)
{
    auto back2front = [](const dag::Node<citygraph::OpVarType>::Port& back) -> bp::PinDesc
    {
        bp::PinDesc front;

        switch (back.var.type)
        {
        case citygraph::OpVarType::Heightmap:
            front.type = PIN_HEIGHTMAP;
            break;
        case citygraph::OpVarType::Path:
            front.type = PIN_PATH;
            break;
        default:
            assert(0);
        }

        front.name = back.var.full_name;

        return front;
    };

    bp::BackendAdapter<citygraph::OpVarType>
        trans("citygraph", back2front);
    trans.InitNodePins(*this, name);
}

}