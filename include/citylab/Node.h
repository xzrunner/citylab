#pragma once

#include <blueprint/Node.h>

#include <citygraph/Operator.h>

namespace citylab
{

class Node : public bp::Node
{
public:
    Node(const std::string& title);

public:
    static constexpr char* const STR_PROP_DISPLAY = "Display";

protected:
    void InitPins(const std::string& name);

    RTTR_ENABLE(bp::Node)

}; // Node

}