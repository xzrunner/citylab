#include "citylab/Node.h"
#include "citylab/PinType.h"

#include <blueprint/Pin.h>
#include <blueprint/BackendAdapter.h>

#include <citygraph/ParamType.h>

namespace citylab
{

Node::Node(const std::string& title)
    : bp::Node(title)
{
}

void Node::InitPins(const std::string& name)
{
    auto back2front = [](const dag::Node<citygraph::ParamType>::Port& back) -> bp::PinDesc
    {
        bp::PinDesc front;

        switch (back.var.type)
        {
        case citygraph::ParamType::HeightField:
            front.type = PIN_HEIGHT_FIELD;
            break;
        case citygraph::ParamType::TensorField:
            front.type = PIN_TENSOR_FIELD;
            break;
        case citygraph::ParamType::Path:
            front.type = PIN_PATH;
            break;
        default:
            assert(0);
        }

        front.name = back.var.full_name;

        return front;
    };

    bp::BackendAdapter<citygraph::ParamType>
        trans("citygraph", back2front);
    trans.InitNodePins(*this, name);
}

}