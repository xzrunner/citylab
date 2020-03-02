#pragma once

namespace citylab
{

enum PinType
{
#define PIN_INFO(type, color, desc) \
    PIN_##type,
#include "citylab/pin_cfg.h"
#undef PIN_INFO
};

}