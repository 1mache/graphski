#pragma once

#include "core/GraphFunctions.h"

namespace graphski
{
    nlohmann::json serializeDrawableNode(const INode& node, NodeId nodeId);
} // namespace graphski